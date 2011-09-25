#ifndef ql_extensions__monte_carlo__dynamics__hpp__
#define ql_extensions__monte_carlo__dynamics__hpp__

#include <boost/function.hpp>

#include "function_types.hpp"
#include "path.hpp"
#include "assets.hpp"

namespace QuantLibExt {

typedef boost::function<Assets (const Variates&, const Assets&, double)> 
    ModelDynamics;  

class Dynamics : public TriadicFunction<Assets ,const Variates&, const Assets&, double> {
  public:
    virtual Assets operator()(
            const Variates &v, const Assets &a, double dt) const = 0;
};

class RnBSVasicekDynamics : public Dynamics {
  public:
    RnBSVasicekDynamics(const std::vector<double>& p) 
      : mu_(p[0]), s_(p[1]), k_(p[2]), t_(p[3]), sr_(p[4]), rho_(p[5]),
        rhoComplement_(std::sqrt(1.0-rho_*rho_)), dt_(0.0/0.0) 
    {}
    
    virtual Assets operator()(
            const Variates &v, const Assets &a, double dt) const {

        updateCache(dt);
        Assets new_a;
        new_a.r    = std::min(std::max(a.r*ekt_ + t_*(1.0-ekt_) 
                                       + stdR_*v.W1,1E-6),0.5);
        new_a.intR = std::min(std::max(a.r*Psi_ + t_*(dt-Psi_) 
                                       + stdIntR_*v.W1,1E-6),0.5);
        new_a.S    = a.S*std::exp(drift(new_a.intR,dt) 
                          + s_*sqrtDt_*(rho_*v.W1 + rhoComplement_*v.W2));
        return new_a;
    }

  protected:
    virtual double drift(double intR, double dt) const {
        return intR - driftCorrection_; // risk-neutral drift
    }
    void updateCache(double dt) const {
        if (dt != dt_) {
            dt_              = dt;
            sqrtDt_          = std::sqrt(dt);
            ekt_             = std::exp(-k_*dt_);
            e2kt_            = ekt_*ekt_;
            Psi_             = (1.0-ekt_)/k_;
            stdR_            = sr_*std::sqrt((1.0-e2kt_)/(2.0*k_));
            stdIntR_         = sr_/k_*std::sqrt(dt_-2.0*(1.0-ekt_)/k_
                               + (1.0-e2kt_)/(2.0*k_));
            driftCorrection_ = 0.5*s_*s_*dt;
        }
    }
    double  mu_, s_, k_, t_, sr_, rho_, rhoComplement_;
    mutable double ekt_, e2kt_, Psi_, stdR_, 
                   stdIntR_, driftCorrection_, dt_, sqrtDt_;
};

class RwBSVasicekDynamics : public RnBSVasicekDynamics {
  public:
    RwBSVasicekDynamics(const std::vector<double>& p) 
        : RnBSVasicekDynamics(p) {}
  protected:
    virtual double drift(double intR, double dt) const {
        return mu_*dt;
    }
};

class RnCevCklsDynamics : public Dynamics {
  public:
    RnCevCklsDynamics(const std::vector<double>& p)
      : mu_(p[0]),  s_(p[1]), a_(p[2]), k_(p[3]), t_(p[4]), sr_(p[5]),
        xi_(p[6]), rho_(p[7]), 
        rhoComplement_(std::sqrt(1.0 - rho_*rho_))
    {}
    
    virtual Assets operator()(
            const Variates &v, const Assets &a, double dt) const {

        Assets new_a;
        double sqrtDt = std::sqrt(dt);
        new_a.r    = std::min(std::max(a.r + k_*(t_-a.r)*dt 
		     + sr_*std::pow(a.r,xi_)*sqrtDt*v.W1,0.0),0.5);
        new_a.intR = 0.5*(a.r + new_a.r)*dt;
        new_a.S    = a.S + drift(a.r)*a.S*dt 
                     + s_*std::pow(a.S,a_)
                        *sqrtDt*(rho_*v.W1 + rhoComplement_*v.W2);

        return new_a;
    }
  protected:
    virtual double drift(double r) const {
        return r; // risk-neutral drift
    }
    double  mu_, s_, a_, k_, t_, sr_, xi_, rho_, rhoComplement_;
};

class RwCevCklsDynamics : public RnCevCklsDynamics {
  public:
    RwCevCklsDynamics(const std::vector<double>& p)
        : RnCevCklsDynamics(p) {}
  protected:
    virtual double drift(double r) const {
        return mu_; // objective drift
    }
};

//
// Factories
//
typedef boost::function<
    Assets (const Variates&, const Assets&, double)> ModelDynamics;

ModelDynamics makeRealWorldDynamics(const std::vector<double>& p,
										const std::string& model_name) {
	if (model_name == "CevCkls")  {
        return ModelDynamics(RwCevCklsDynamics(p));
	} else if (model_name == "BS_Vas") {
        return ModelDynamics(RwBSVasicekDynamics(p));
    } else {
		QL_FAIL("makeRealWorldDynamics: Illegal model_name: " + model_name);
	}
}

ModelDynamics makeRiskNeutralDynamics(const std::vector<double>& p,
									  const std::string& model_name) {
	if (model_name == "CevCkls")  {
        return ModelDynamics(RnCevCklsDynamics(p));
    } else if (model_name == "BS_Vas") {
        return ModelDynamics(RnBSVasicekDynamics(p));
    } else {
		QL_FAIL("makeRealWorldDynamics: Illegal model_name: " + model_name);
	}
}

};


#endif
