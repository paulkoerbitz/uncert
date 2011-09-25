#ifndef ql_extensions__mcmc__mcmc_models_hpp
#define ql_extensions__mcmc__mcmc_models_hpp

#define IR_MODELS_H

#include <cmath>

#include <ql/quantlib.hpp>
#include <flens/flens.h>
#include <ool/ool_conmin.h>

#include "parameter_access.hpp"

namespace ql=QuantLib;

namespace QuantLibExt {

	typedef flens::SyMatrix<flens::FullStorage<double, flens::ColMajor> >   SYMatrix;
	typedef flens::GeMatrix<flens::FullStorage<double, flens::ColMajor> >   GEMatrix;
	typedef flens::DenseVector<flens::Array<double> >                       DEVector;
	 
	
	class McmcModel {
	public:
		typedef std::vector<double> ParamType;
	    virtual double log_likelihood(const ParamType& x) const = 0;
	};
	 
	class McmcModel_w_grad : public McmcModel {
	public:
	    virtual void log_likelihood_gradient(const ParamType& x, DEVector& g) const = 0;
	    virtual void log_likelihood_gradient(const ParamType& x, gsl_vector* dl) const = 0;
	};
	 
	class McmcModel_w_hess : public McmcModel_w_grad {
	public:
	    virtual void log_likelihood_Hessian(const ParamType& x, SYMatrix& H) const = 0;
	    virtual void log_likelihood_Hessian(const ParamType& x, GEMatrix& H) const = 0;
	};
	 
 
	class BlackScholesMcmcModel : public McmcModel,
								  private BlackScholesAccess<McmcModel::ParamType,0>
	{
	public:
	 	BlackScholesMcmcModel(const ql::Path& path);
	    virtual double log_likelihood(const ParamType& p) const;
	private:
	 	ql::Path path;
	};
	 
	class CevMcmcModel : public McmcModel,
						 private CevAccess<McmcModel::ParamType,0>
	{
	public:
	 	CevMcmcModel(const ql::Path& path);
	    virtual double log_likelihood(const ParamType& p) const;
	private:
	 	ql::Path path;
	};
	 
	 
	class VasicekMcmcModel : public McmcModel,
							 private VasicekAccess<McmcModel::ParamType,0>
	{
	public:
	 	VasicekMcmcModel(const ql::Path& path);
	    virtual double log_likelihood(const ParamType& p) const;
	private:
	 	ql::Path path;
	};
	 
	 
	class CklsMcmcModel : public McmcModel_w_hess,
						  private CklsAccess<McmcModel::ParamType,0>
	{
	public:
	 	CklsMcmcModel(const ql::Path& path);
	 	virtual double log_likelihood(const ParamType& p) const;
	    virtual void log_likelihood_gradient(const ParamType& x, DEVector &g) const;
	    virtual void log_likelihood_gradient(const ParamType& x, gsl_vector* dl) const;
	    virtual void log_likelihood_Hessian(const ParamType& x, SYMatrix &H) const;
	    virtual void log_likelihood_Hessian(const ParamType& x, GEMatrix &H) const;
	private:
		ql::Path path;
	};
	 
	 
	class BlackScholesVasicekMcmcModel : public McmcModel,
										 private BlackScholesAccess<McmcModel::ParamType,0>,
										 private VasicekAccess<McmcModel::ParamType,2>,
										 private RhoAccess<McmcModel::ParamType,5>
	{
	public:
	 	BlackScholesVasicekMcmcModel(const ql::MultiPath& path);
	    virtual double log_likelihood(const ParamType &p) const;
	private:
	 	ql::MultiPath path;
	};
	 
	 
	class CevCklsMcmcModel : public McmcModel, 
							 private CevAccess<McmcModel::ParamType,0>,
							 private CklsAccess<McmcModel::ParamType,3>,
							 private RhoAccess<McmcModel::ParamType,7>
	{
	public:
	 	CevCklsMcmcModel(const ql::MultiPath& path);
	    virtual double log_likelihood(const ParamType &p) const;
	private:
		ql::MultiPath path;
	};
}

#endif
