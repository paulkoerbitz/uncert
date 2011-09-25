#include "mcmc_algorithms.hpp"

namespace QuantLibExt {

/*************************************************
 * McmcAlgo
 ************************************************/

McmcAlgo::McmcAlgo(boost::shared_ptr<McmcModel> p_model,
				   const ParamType& start_values,
				   const ParamType& lower_bound,
				   const ParamType& upper_bound,
				   boost::mt19937 generator,
				   unsigned thining)
	: p_Model(p_model),
	  ud(generator, boost::uniform_real<>(0,1)),
	  nd(generator, boost::normal_distribution<>(0,1)),
	  last_accepted(start_values), candidate(start_values),
	  lb(lower_bound), ub(upper_bound),
	  thin(thining),
	  n_generated(0),
	  n_accepted(0),
	  n_parameters(start_values.size())
{}

bool McmcAlgo::fulfills_constraints(const ParamType& p) const {
	return check(lb.begin(), lb.end(), p.begin(), bl::_1 <= bl::_2)
		&& check(ub.begin(), ub.end(), p.begin(), bl::_1 >= bl::_2);
}

double McmcAlgo::acceptance_ratio() const {
    return double(n_accepted)/double(n_generated);
}

    
/*************************************************
 * MetropolisHastings
 ************************************************/
MetropolisHastings::MetropolisHastings(boost::shared_ptr<McmcModel> p_Model,
                                       const ParamType& start_values,
                                       const ParamType& lower_bound,
                                       const ParamType& upper_bound,
                                       boost::mt19937 generator,
                                       double tuning,
                                       unsigned thin,
                                       const SYMatrix& Hessian)
	: McmcAlgo(p_Model,start_values, lower_bound, upper_bound, generator, thin),
	  tuning_parameter(tuning),
	  candidate_v(n_parameters),
	  last_accepted_v(n_parameters),
	  random_v(n_parameters),
	  cholesky_ll(n_parameters, n_parameters), 
	  inverse_covar(n_parameters, n_parameters)
{
    copy_to_devector(last_accepted.begin(), last_accepted_v);

    // If no hessian was passed use identity matrix
    if ( Hessian.dim() == 0 ) {
        SYMatrix new_hess(n_parameters, flens::Upper);
        for (unsigned j=1; j <= n_parameters; ++j) {
            new_hess(j,j) = 1.0;
            for (unsigned k=j+1; k <= n_parameters; ++k) {
				new_hess(j,k) = 0.0;
            }
        }
		setup_matricies(new_hess, tuning_parameter, cholesky_ll, inverse_covar);
    } else {
		setup_matricies(Hessian, tuning_parameter, cholesky_ll, inverse_covar);
	}
}

typedef flens::DenseVector<flens::Array<int> > Pivots;

void MetropolisHastings::setup_matricies(const SYMatrix &Hessian, double tp, 
											   GEMatrix &L, GEMatrix &invCov)
{
    unsigned N = Hessian.dim();
    SYMatrix U(N,flens::Upper);                

    // invert Hessian
    GEMatrix inv_Hessian(N, N);
    Pivots   P(Hessian.dim());
    for (unsigned k=1; k <= N; ++k ) {
        for (unsigned j=k; j <= N; ++j ) {
            // Divide Hessian by tuning_parameter <=> 
            // multiply Covar matrix by tuning_parameter
            inv_Hessian(k,j) = Hessian(k,j) / tp;
            inv_Hessian(j,k) = Hessian(k,j) / tp;
            // -1*(Hessian/tuning_parameter) is the inverse of the 
            // Covariance of the porposal density
            invCov(k,j)             = -Hessian(k,j) / tp;
            invCov(j,k)             = -Hessian(k,j) / tp;
        }
    }
    trf(inv_Hessian,P);
    tri(inv_Hessian,P); // inv_Hessian now holds inverse of the Hessian

    // Copy the negative inverse of the Hessian 
    // (= Covariance Matrix of proposal density)
    // to a symmetric matrix with upper storage
    // for cholesky decompostion
    for (unsigned k=1; k <= N; ++k) {
        for (unsigned j=k; j <= N; ++j) {
            U(k,j) = -inv_Hessian(k,j);
        }
    }

    trf(U); // Cholesky decomposition for the negative inverse of the Hessian

    // Set up L = lower left matrix of Cholesky decomposition 
    // of the negative inverse of the Hessian
    for (unsigned row=1; row <= N; ++row) {
        for (unsigned col=row+1; col <= N; ++col) {
            L(row,col) = 0;
        }
        for (unsigned col=1; col <=row; ++col) {
            L(row,col) = U(col,row);
        }
    }
}

ParamType MetropolisHastings::next_scenario() 
{
    for ( unsigned l=0 ; l < thin; ++l ) {
		draw_random_vector();
		generate_candidate();
		++n_generated;

		if (accept_candidate()) {
			++n_accepted;
            last_accepted_v = candidate_v;
            last_accepted = candidate;
		}
    }
	return last_accepted;
}

void MetropolisHastings::draw_random_vector() {
	for (unsigned k=1; k<= n_parameters; ++k) 
            random_v(k) = nd();
}

bool MetropolisHastings::accept_candidate() const
{
    if ( fulfills_constraints(candidate) ) { 

        double log_f_old, log_f_new, log_q_old, log_q_new, alpha;

        log_f_old = p_Model->log_likelihood(last_accepted);
        log_f_new = p_Model->log_likelihood(candidate);

        log_q_old = log_proposal_density(last_accepted);
        log_q_new = log_proposal_density(candidate);

        alpha = exp(log_f_new - log_f_old + log_q_old - log_q_new);

        return ( alpha > ud() );
    } else { 
        return false;
    }
}

/*************************************************
 * RandomWalkMH
 ************************************************/
RandomWalkMH::RandomWalkMH(boost::shared_ptr<McmcModel> p_Model,
                           const ParamType& start_values,
						   const ParamType& lb,
						   const ParamType& ub,
						   boost::mt19937 generator,
						   double tuning,
						   unsigned thin,
						   const SYMatrix& Hessian)
	: MetropolisHastings(p_Model, start_values, lb, ub,
                         generator, tuning, thin, Hessian)
{}

double RandomWalkMH::log_proposal_density(const ParamType& p) const
{
    // since proposal density is symmetric for RandomWalkMH, this can be ignored
    return 0.0;
}

void RandomWalkMH::generate_candidate() {
	candidate_v = last_accepted_v + cholesky_ll * random_v;
	copy_from_devector(candidate_v, candidate.begin());
}

/*************************************************
 * IndependentMH
 ************************************************/

IndependentMH::IndependentMH(boost::shared_ptr<McmcModel> p_Model,
                             const ParamType& start_values,
                             const ParamType& lb,
                             const ParamType& ub,
                             const ParamType& mean,
                             boost::mt19937 generator,
                             double tuning,
                             unsigned thin,
                             const SYMatrix& Hessian)
	: MetropolisHastings(p_Model, start_values, lb, ub,
                         generator, tuning, thin, Hessian),
	  mean_v(n_parameters), buf_v(n_parameters), mean(mean)
{
    copy_to_devector(mean.begin(), mean_v);
}

void IndependentMH::generate_candidate() {
    candidate_v = cholesky_ll*random_v + mean_v;
	copy_from_devector(candidate_v, candidate.begin());
}

double IndependentMH::log_proposal_density(const ParamType &p) const {
	// Returns the log a multivariate normal pdf
	// the term - log(2pi^(n/2)*det(Covar)) is omitted
	// (cancels out in calculations)
	DEVector buf1 = candidate_v - mean_v;
    DEVector buf2 = inverse_covar * buf1;
	DEVector result = buf1*buf2;
    return -0.5*result(1);
}

} // namespace QuantLibExt
