#ifndef ql_extensions__mcmc_algorithms_hpp
#define ql_extensions__mcmc_algorithms_hpp

#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/lambda/lambda.hpp>

#include <boost/random/mersenne_twister.hpp>    
#include <boost/random/uniform_real.hpp>        
#include <boost/random/normal_distribution.hpp> 
#include <boost/random/variate_generator.hpp>   

#include <ql/quantlib.hpp>

#include <algorithm/check.hpp>
#include <algorithm/devector_copy.hpp>

#include "mcmc_models.hpp"


namespace bl=boost::lambda;
namespace ql=QuantLib;

namespace QuantLibExt {

typedef McmcModel::ParamType ParamType;

class McmcAlgo 
{
public:
	McmcAlgo(boost::shared_ptr<McmcModel> p_Model,
		     const ParamType& start_vals,
		     const ParamType& lb,
		     const ParamType& ub,
		     boost::mt19937 generator,
		     unsigned thin=1u);

	virtual ParamType next_scenario() = 0;
	virtual double acceptance_ratio() const;

protected:
	bool fulfills_constraints(const ParamType &p) const;

	boost::shared_ptr<McmcModel> p_Model;
	mutable boost::variate_generator<boost::mt19937, boost::uniform_real<> > ud;
	mutable boost::variate_generator<boost::mt19937, boost::normal_distribution<> > nd;

	ParamType last_accepted, candidate, lb, ub;
	unsigned thin;
	unsigned n_generated;
	unsigned n_accepted;
	unsigned n_parameters;
};


typedef flens::DenseVector<flens::Array<double> > DEVector;
typedef flens::GeMatrix<flens::FullStorage<double, flens::ColMajor> > GEMatrix;
typedef flens::SyMatrix<flens::FullStorage<double, flens::ColMajor> > SYMatrix;

class MetropolisHastings : public McmcAlgo
{
public:
    MetropolisHastings(boost::shared_ptr<McmcModel> p_Model,
					   const ParamType& start_vals,
					   const ParamType& lb,
					   const ParamType& ub,
					   boost::mt19937 generator,
					   double tuning,
					   unsigned thin=1u,
					   const SYMatrix& Hessian = SYMatrix());

protected:

    double tuning_parameter;

    DEVector    candidate_v;
    DEVector    last_accepted_v;
    DEVector    random_v;

    GEMatrix    cholesky_ll;
    GEMatrix    inverse_covar;

    virtual double log_proposal_density(const ParamType &p) const = 0;
	virtual void generate_candidate() = 0;

    static void setup_matricies(const SYMatrix &Hessian, double tp, 
								GEMatrix &cholesky_ll, GEMatrix &inverse_covar);
	virtual ParamType next_scenario();
	void draw_random_vector();
    bool accept_candidate() const; 
};


class RandomWalkMH : public MetropolisHastings
{
public:
    RandomWalkMH(boost::shared_ptr<McmcModel> p_Model,
				 const ParamType& start_vals,
				 const ParamType& lb,
				 const ParamType& ub,
				 boost::mt19937 generator,
				 double tuning,
				 unsigned thin=1u,
				 const SYMatrix& Hessian = SYMatrix());
    
protected:
	virtual void generate_candidate();
    virtual double log_proposal_density(const ParamType& p) const;
};

class IndependentMH : public MetropolisHastings
{
public:
    IndependentMH(boost::shared_ptr<McmcModel> p_Model,
				  const ParamType& start_vals,
				  const ParamType& lb,
				  const ParamType& ub,
				  const ParamType& mean,
				  boost::mt19937 generator,
				  double tuning,
				  unsigned thin=1u,
				  const SYMatrix& Hessian = SYMatrix());
  
protected:
	virtual void generate_candidate();
    virtual double log_proposal_density(const ParamType &p) const;

    DEVector mean_v, buf_v;
	ParamType mean;
};

}

#endif
