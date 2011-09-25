#ifndef ql_extensions__mcmc__parameter_access_hpp
#define ql_extensions__mcmc__parameter_access_hpp

namespace QuantLibExt {			 

	template <class RandomAccessContainer, int Offset>
	class BlackScholesAccess
	{
	public:
	 	static double drift(const RandomAccessContainer& p) {
	 		return p[Offset+0];
	 	}
	 	static double vola(const RandomAccessContainer& p) {
	 		return p[Offset+1];
	 	}
	};
	 
	template <class RandomAccessContainer, int Offset>
	class CevAccess : public BlackScholesAccess<RandomAccessContainer,Offset>
	{
	public:
	 	static double exp(const RandomAccessContainer& p) {
	 		return p[Offset+2];
	 	}
	};
	 
	template <class RandomAccessContainer, int Offset>
	class VasicekAccess {
	public:
	 	static double speed(const RandomAccessContainer& p) {
	 		return p[Offset+0];
	 	}
	 	static double mean(const RandomAccessContainer& p) {
	 		return p[Offset+1];
	 	}
	 	static double ir_vola(const RandomAccessContainer& p) {
	 		return p[Offset+2];
	 	}
	};
	 
	template <class RandomAccessContainer, int Offset>
	class CklsAccess : public VasicekAccess<RandomAccessContainer,Offset>
	{
	public:
	 	static double ir_exp(const RandomAccessContainer& p) {
	 		return p[Offset+3];
	 	}
	};
	 
	template <class RandomAccessContainer, int Offset>
	class RhoAccess {
	public:
	 	static double rho(const RandomAccessContainer& p) {
	 		return p[Offset];
	 	}
	};
}

#endif
