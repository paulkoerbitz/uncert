#include "mcmc_models.hpp"

namespace QuantLibExt {

	/**********************************************************************
	 * BlackScholesMcmcModel
	 *********************************************************************/
	BlackScholesMcmcModel::BlackScholesMcmcModel(const ql::Path& path)
	 	: path(path)
	{}
	 
	double BlackScholesMcmcModel::log_likelihood(const ParamType &p) const
	{
	    double sum_sq=0;
	    unsigned n = path.length();
	 
	    double h = path.time(1)-path.time(0);
	    double sig_sq = vola(p)*vola(p);
	    double h_s_2 = 2.0*h*sig_sq;
	    double summand = (drift(p)-0.5*sig_sq)*h;
	    double log_s_now, log_s_before, z;
	 
	    log_s_now = std::log(path[0]);
	 
	    for (unsigned i = 1; i < n; i++) {
	        log_s_before = log_s_now;
	        log_s_now = std::log(path[i]);
	        z = log_s_now - (log_s_before+summand);
	        sum_sq += z*z;
	    }
	 
	    return -0.5*n*std::log(M_PI*h_s_2) - sum_sq / h_s_2;
	}


	/**********************************************************************
	 * CevMcmcModel
	 *********************************************************************/
	CevMcmcModel::CevMcmcModel(const ql::Path& path)
		: path(path)
	{}

	double CevMcmcModel::log_likelihood(const ParamType &p) const
	{
	    unsigned n = path.length();
	    double dt = path.time(1)-path.time(0);
	    double sig_sq = vola(p)*vola(p);
	    double Sto2Xi, z, sum_1=0, sum_2=0;
	 
	    for (unsigned i = 1; i < n; i++) {
	        Sto2Xi = std::pow(path[i-1],2.0*exp(p));
	        sum_1 += std::log(2.0*M_PI*sig_sq*dt*Sto2Xi);
	        z = (path[i] - (1.0 + drift(p)*dt)*path[i-1]) ;
	        sum_2 += z*z / Sto2Xi;
	    }
	 
	    double first = -0.5*sum_1;
	    double second = -1.0/(2.0*sig_sq*dt) * sum_2;
	 
	    return first+second;
	}


	/**********************************************************************
	 * VasicekMcmcModel
	 *********************************************************************/
	VasicekMcmcModel::VasicekMcmcModel(const ql::Path& path)
		: path(path)
	{}

	double VasicekMcmcModel::log_likelihood(const ParamType& p) const
	{
	    double sum_sq=0, z, first_term;
	    unsigned n = path.length();
	 
	    double ekh = std::exp(-speed(p)*(path.time(1)-path.time(0)));
	    double t1ekh = mean(p)*(1.0-ekh);
	    double minus_e2kh_plus_1 = 1.0-ekh*ekh;
	 
	    double sig_sq = ir_vola(p)*ir_vola(p);
	 
	    for (unsigned k=1; k< n; ++k) {
	        z = path[k]-path[k-1]*ekh-t1ekh;
	        sum_sq += z*z;
	    }
	 
	    first_term = -0.5*n*std::log(M_PI*sig_sq/speed(p)*minus_e2kh_plus_1);
	    sum_sq *= speed(p)/(sig_sq*minus_e2kh_plus_1);
	 
	    return first_term - sum_sq;
	}


	/**********************************************************************
	 * CklsMcmcModel
	 *********************************************************************/
	CklsMcmcModel::CklsMcmcModel(const ql::Path& path)
		: path(path)
	{}


	double CklsMcmcModel::log_likelihood(const ParamType& p) const
	{
	    double sum2=0, sum_lsr=0, z;
	    double dt = path.time(1) - path.time(0);
	    
	    double fact1 = (1.0-speed(p)*dt);
	    double fact2 = speed(p)*mean(p)*dt;
	 
	    unsigned n = path.length();
	 
	    double sig_sq_2_dt = 2.0*ir_vola(p)*ir_vola(p)*dt;
	    for(unsigned int k=0; k < n-1; ++k) {
	        z    = (path[k+1]-fact1*path[k]-fact2) / std::pow(path[k],ir_exp(p));
	        sum2 += z*z;
	        sum_lsr += log(path[k]);
	    }
	    return -( n*0.5*log(M_PI*sig_sq_2_dt) 
				  + ir_exp(p)*sum_lsr + sum2/(sig_sq_2_dt) );
	}
	 
	void CklsMcmcModel::log_likelihood_gradient(const ParamType& p, DEVector& g) const
	{
	    unsigned n = path.length();
	    double dt = path.time(1) - path.time(0);
	    double buf1, buf2, lsr;
	    double numerator, r_to_2xi, sum_k=0, sum_t=0, sum_s=0, sum_x=0, sum_lsr=0; 
	    
	    for(unsigned k=1; k < n; ++k)
	    {
	        numerator = path[k] - (1.0-speed(p)*dt)*path[k-1] -speed(p)*mean(p)*dt;
	        r_to_2xi  = pow(path[k-1],2*ir_exp(p));
	 
	        buf1      = numerator/r_to_2xi;
	        buf2      = numerator*numerator/r_to_2xi;
	        lsr       = log(path[k-1]);
	 
	        sum_k    += buf1*(path[k-1]-mean(p));
	        sum_t    += buf1*speed(p);
	        sum_s    += buf2;
	        sum_x    += buf2*lsr;
	        
	        sum_lsr  += lsr;
	    }
	 
	    buf1 = ir_vola(p)*ir_vola(p);
	 
	    g(1) = - sum_k/buf1;
	    g(2) =   sum_t/buf1;
	    g(3) =   sum_s/(buf1*ir_vola(p)*dt) - (n-1)/ir_vola(p);
	    g(4) =   sum_x/(buf1*dt) - sum_lsr;
	}

	void CklsMcmcModel::log_likelihood_gradient(const ParamType& p, gsl_vector *dl) const
	{
	    DEVector g(4);
	    log_likelihood_gradient(p,g);
	 
	    for (int k=0; k<4; ++k) {
	        gsl_vector_set(dl,k,g(k+1));
	    }
	}
	  
	void CklsMcmcModel::log_likelihood_Hessian(const ParamType& p, SYMatrix& H) const
	{
	    double buf1, buf2, lsr;
	    unsigned n = path.length();
	    double dt = path.time(1) - path.time(0);
	    double numerator, r_to_2xi, 
	           sum_kk=0, sum_kt=0, sum_ks=0, sum_kx=0,
	                     sum_tt=0, sum_ts=0, sum_tx=0,
	                               sum_ss=0, sum_sx=0,
	                                         sum_xx=0; 
	    
	    for(unsigned k=1; k<n; ++k)
	    {
	        numerator = path[k] - (1.0-speed(p)*dt)*path[k-1] - speed(p)*mean(p)*dt;
	        r_to_2xi  = pow(path[k-1],2.0*ir_exp(p));
	        buf1      = numerator/r_to_2xi;
	        buf2      = numerator*numerator/r_to_2xi;
	        lsr       = log(path[k-1]);
	 
	        sum_kk   += (path[k-1]-mean(p))*(path[k-1]-mean(p))/r_to_2xi;
	        sum_kt   += (numerator + speed(p)*dt*(path[k-1]-mean(p)))/r_to_2xi;
	        sum_ks   += buf1*(path[k-1]-mean(p));
	        sum_kx   += buf1*(path[k-1]-mean(p))*lsr;
	 
	        sum_tt   += 1.0/r_to_2xi; 
	        sum_ts   += buf1;
	        sum_tx   += buf1*lsr;
	 
	        sum_ss   += buf2;
	        sum_sx   += buf2*lsr;
	 
	        sum_xx   += buf2*lsr*lsr;
	    }
	 
	    //turn sums into second derivatives
	    buf1   = ir_vola(p)*ir_vola(p);
	    buf2   = buf1*ir_vola(p);
	 
	    H(1,1) = -   dt/buf1*sum_kk;
	    H(1,2) =    1.0/buf1*sum_kt;
	    H(1,3) =   2.0/buf2*sum_ks;
	    H(1,4) =   2.0/buf1*sum_kx;
	 
	    H(2,2) = - speed(p)*speed(p)*dt/buf1*sum_tt;
	    H(2,3) = - 2.0*speed(p)/buf2*sum_ts;
	    H(2,4) = - 2.0*speed(p)/buf1*sum_tx;
	 
	    H(3,3) = - 3.0/(buf1*buf1*dt)*sum_ss + (n-1)/buf1;
	    H(3,4) = - 2.0/(buf2*dt)*sum_sx;
	 
	    H(4,4) = - 2.0/(buf1*dt)*sum_xx;
	}
	 
	void CklsMcmcModel::log_likelihood_Hessian(const ParamType& p, GEMatrix& H) const
	{
	    SYMatrix HH(p.size(), flens::Upper);
	    for (int j=1; j<=HH.dim(); ++j) {
	        for (int k=j; k<=HH.dim(); ++k) {
	            HH(j,k) = H(j,k);
	        }
	    }
	    log_likelihood_Hessian(p, HH);
	    for (int j=1; j<= HH.dim(); ++j) {
	        for (int k=j; k <= HH.dim(); ++k) {
	            H(j,k) = HH(j,k);
	            H(k,j) = HH(k,j);
	        }
	    }
	}


	/**********************************************************************
	 * BlackScholesVasicekMcmcModel
	 *********************************************************************/
	BlackScholesVasicekMcmcModel::BlackScholesVasicekMcmcModel(const ql::MultiPath& path)
		: path(path)
	{}

	double BlackScholesVasicekMcmcModel::log_likelihood(const ParamType& p) const
	{
	    // we return a value that is shifted by a constant (0.5*log(2*PI)) from the 
	    // log-likelihood function, this is sufficient for MCMC simulations
	 
	    unsigned n = path.pathSize();
	    double dt = path[0].timeGrid().dt(1);
	 
	    double ekdt = std::exp(-speed(p)*dt);
	    double t_1_ekdt = mean(p)*(1.0-ekdt);
	 
	    double sig_ls = vola(p)*std::sqrt(dt);
	    double sig_ls_sq = sig_ls*sig_ls;
	    double sig_r = ir_vola(p)*std::sqrt((1.0-ekdt*ekdt)/(2.0*speed(p)));
	    double sig_r_sq = sig_r*sig_r;
	    double ro = rho(p);
	    double rho_sq = ro*ro;
	 
	    double drift_ls = drift(p)*dt - sig_ls_sq / 2.0;
	 
	    double log_s_now, log_s_before;
	    double x1, x2;
	 
	    double sum = 0;
	    log_s_now = std::log(path[0][0]);
	 
	    for (unsigned i=1; i<n; ++i)
	    {
	        log_s_before = log_s_now;
	        log_s_now = std::log(path[0][i]);
	 
	        x1 = log_s_now - log_s_before - drift_ls;
	        x2 = path[1][i] - path[1][i-1]*ekdt - t_1_ekdt;
	 
	        sum += x1*x1 / sig_ls_sq - 2.0*ro*x1*x2 / (sig_ls*sig_r) + x2*x2 / sig_r_sq;
	    }
	 
	    return -0.5*n*std::log( (1.0-rho_sq)*sig_ls_sq*sig_r_sq ) 
	           - 0.5*sum / (1.0-rho_sq);
	}


	/**********************************************************************
	 * CevCklsMcmcModel
	 *********************************************************************/
	CevCklsMcmcModel::CevCklsMcmcModel(const ql::MultiPath& path)
		: path(path)
	{}

	double CevCklsMcmcModel::log_likelihood(const ParamType &p) const
	{
	    double dt = path[0].timeGrid().dt(1);
	    double sqrtDt = std::sqrt(dt);
	 
	    double rho_comp = 1 - rho(p)*rho(p);
	    double sum = 0.0;
	 
	    for (unsigned i=1; i<path.pathSize(); ++i) {
	        double x = path[0][i] - (1.0+drift(p)*dt)*path[0][i-1];
	        double y = path[1][i] 
	            - (path[1][i-1] + speed(p)*(mean(p) - path[1][i-1])*dt);
	        double eta_S = vola(p)*std::pow(path[0][i-1],exp(p))*sqrtDt;
	        double eta_r = ir_vola(p)*std::pow(path[1][i-1],ir_exp(p))*sqrtDt;
	        double eta_S_sq = eta_S*eta_S;
	        double eta_r_sq = eta_r*eta_r;
	        sum += -std::log(2.0*M_PI*std::sqrt(rho_comp)*eta_S*eta_r)
	               -0.5/rho_comp*(x*x/eta_S_sq 
	                              - 2.*rho(p)*x*y/(eta_S*eta_r) 
	                              + y*y/eta_r_sq);
	    }
	    return sum;
	}
}
