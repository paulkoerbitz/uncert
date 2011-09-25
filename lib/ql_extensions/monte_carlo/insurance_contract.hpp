#ifndef ql_extensions__monte_carlo__insurance_contract_hpp__
#define ql_extensions__monte_carlo__insurance_contract_hpp__

#include "../instruments/termfixinsurance/valuevector.hpp"

#include "path.hpp"
#include "assets.hpp"
#include "variates.hpp"
#include "dynamics.hpp"

namespace QuantLibExt {

struct ContractStates {
    ContractStates(double LL=0.0, double AAp=0.0, double cc=0.0, double dd=0.0)
        : L(LL), Ap(AAp), c(cc), d(dd) {}
    double L, Ap, c, d;
};

typedef Path<ContractStates>::iterator ContrStatePathIter;
typedef Path<ContractStates>::const_iterator ConstContrStatePathIter;

struct ContractTraits {
    ContractTraits() {}
    ContractTraits(double gg, double yy, double ddelta
                  ,double L=10000, double Ap=11000
                  ,rational ddt=rational(1,1)
                  ,rational TT=rational(10,1)) 
        : g(gg), y(yy), delta(ddelta)
         ,initialContractStates(L,Ap)
         ,dt(ddt), T(TT) {}

    double g, y, delta;
    ContractStates initialContractStates;
    rational dt, T;
};


void computeContractStatePath(
                     ConstAssetPathIter   iAPLastAnniversary
                    ,ConstAssetPathIter   iAPend
                    ,ContrStatePathIter   iCSnow
                    ,ContrStatePathIter   iCSend
                    ,const ContractTraits &ct) {
    ConstAssetPathIter iterAP = iAPLastAnniversary;
    double L1 = iCSnow->L;
    double Ap = iCSnow->Ap;
    double res_quot = (Ap - L1)/L1;
    double S1 = iterAP->S;

    for (++iCSnow; iCSnow != iCSend; ++iCSnow) {

        double S0 = S1; 
        while ( iterAP.t() < iCSnow.t() ) { 
            ++iterAP; 
        }
        S1 = iterAP->S;
        
        double x = S1 / S0 - 1.0;
        double Am = Ap * (1.0+x);
        double L0 = L1;

        double cond1 = (ct.delta*ct.y*x*(1+res_quot) > ct.g) ? 1.0 : 0.0;;
        double cond2 = 1-cond1;
        double cond3 = (ct.g <= ct.y*x*(1+res_quot)) ? 1.0 : 0.0;
       
        L1        = (1 + std::max(ct.delta*ct.y*x*(1.0+res_quot),ct.g))*L0;
        double d  = (1-ct.delta)*ct.y*Ap*x*cond1 
                   + (ct.y*x*(1+res_quot)-ct.g)*L0*cond2*cond3;

        Ap = std::max(Am-d,L1);

        double c = std::max(0.0,L1-Am);
        res_quot = (Ap - L1)/L1;

        iCSnow->L  = L1;
        iCSnow->Ap = Ap;
        iCSnow->c  = c;
        iCSnow->d  = d;
    }
}

Path<ContractStates> makeContractStatePath(
        const Path<Assets>& assetPath
       ,const ContractTraits& contractTraits) 
{
    Path<ContractStates> contractStatePath(contractTraits.dt,contractTraits.T);
    contractStatePath[0] = contractTraits.initialContractStates;

    computeContractStatePath(assetPath.begin(),assetPath.end()
            ,contractStatePath.begin(), contractStatePath.end()
            ,contractTraits);
    return contractStatePath;
}
 
Path<ValueVector> payoffPathFromContractStates(
        const Path<ContractStates>& contrStatePath) {
    Path<ValueVector> payoff(contrStatePath.dt()
                                ,contrStatePath.T()
                                ,contrStatePath.t0());
    payoff[0].Res = (-1)*(contrStatePath[0].Ap-contrStatePath[0].L);
    for (unsigned i=1; i<contrStatePath.size(); ++i) {
        payoff[i].C = contrStatePath[i].c;
        payoff[i].D = contrStatePath[i].d;
    }
    unsigned end    = contrStatePath.size()-1;
    payoff[end].V   = contrStatePath[end].L;
    payoff[end].Res = contrStatePath[end].Ap-contrStatePath[end].L;

    return payoff;
}


ValueVector valueContract(rational t
                             ,const Path<Assets>& assetPath
                             ,const Path<ContractStates>& contrStatePath) {
    return discountValue(t,assetPath,
            payoffPathFromContractStates(contrStatePath));
}

ValueVector valueContractFromPath(rational t
                                     ,const Path<Assets>   &assetPath
                                     ,Path<ContractStates>    contractStatePath
                                     ,const ContractTraits &contractTraits) {

    Path<ContractStates>::iterator iCSLastAnniversary 
        = contractStatePath.lastIteratorOnOrBeforeTime(t);
    rational t_lastAnniversary = iCSLastAnniversary.t();
    Path<Assets>::const_iterator iAPLastAnniversary
        = assetPath.iteratorAtTime(t_lastAnniversary);
    computeContractStatePath(iAPLastAnniversary,assetPath.end()
                    ,iCSLastAnniversary, contractStatePath.end()
                    ,contractTraits);
    return valueContract(t,assetPath,contractStatePath);
}

ValueVector valueContractFromPathWithOffset(
         rational t
        ,Path<Assets> &assetPath                        // is logically const
        ,const Assets& originalAssetValue
        ,const Path<ContractStates>& contractStatePath
        ,const ContractTraits &contractTraits)
{
    Assets offsetedValue = assetPath[t];
    assetPath[t] = originalAssetValue;
    ValueVector contractValue 
        = valueContractFromPath(t,assetPath,contractStatePath,contractTraits);
    assetPath[t] = offsetedValue;
    return contractValue;
}
        

ValueVector valueContractFromVariates
                        (rational t
                        ,Path<Assets> assetPath
                        ,const Path<Variates>& variates
                        ,const Path<ContractStates>& contractStatePath
                        ,const ContractTraits& contractTraits
                        ,const ModelDynamics& dynamics) {
    updatePathFromVariates(assetPath.iteratorAtTime(t),assetPath.end()
                          ,variates.iteratorAtTime(t),dynamics);
    return valueContractFromPath(t,assetPath,contractStatePath,contractTraits);
}


typedef Path<Variates> ScenT;
typedef Assets PathV;
typedef ValueVector ValT;
typedef Array<> UnderlT;
typedef Array<ValueVector> DeltaT;


ValueVector divide(const ValueVector& vv, double d) {
    return vv/d;
}

// This template specialisation is required 
// so that the computeMCExpectation works with Array<ValueVector>
const ql::Disposable<Array<ValueVector> > operator/(
        const Array<ValueVector> &v, double d) {

    Array<ValueVector> result(v.size());
    std::transform(v.begin(),v.end(),result.begin(), boost::bind(divide,_1,d));
    return result;
}


class InsContrEndPointPricingModel 
            : public PricingModel<ValT,UnderlT, DeltaT> {
  public:
    InsContrEndPointPricingModel(Assets finalAssetValues
                                ,ContractStates finalContractStates)
        : finalAssetValues_(finalAssetValues)
         ,finalContractStates_(finalContractStates)
    {}

    virtual ValT    value() const {
        ValueVector vv;
        vv.V   = finalContractStates_.L;
        vv.C   = finalContractStates_.c;
        vv.D   = finalContractStates_.d;
        vv.Res = finalContractStates_.Ap - finalContractStates_.L;
        return vv;
    }
    virtual UnderlT underlyings() const {
        Array<> underlyings(2);
        underlyings[0] = finalAssetValues_.S;
        underlyings[1] = 1.0;
        return underlyings;
    }
    virtual DeltaT  deltas() const {
        return Array<ValueVector>(2); 
    }

  protected:
    Assets finalAssetValues_;
    ContractStates finalContractStates_;
};

class InsContrMCPricingModelFactory {
public:
  InsContrMCPricingModelFactory(unsigned nScenarios,
                                rational hedgePathDt,
                                boost::function<double ()> n,
                                ModelDynamics dynamics,
                                ContractTraits contractTraits,
                                double offset_S=0.0,
                                double offset_r=0.0) 
    : nScenarios_(nScenarios), hedgePathDt_(hedgePathDt),
      n_(n), dynamics_(dynamics), contractTraits_(contractTraits),
      offset_S_(offset_S), offset_r_(offset_r) {}

  virtual PricingModel<ValT,UnderlT,DeltaT>* make(const rational&,
                                                  const Path<Assets>&,
                                                  const Path<ContractStates>&) const;

protected:
  Assets offset_S(Assets) const;
  Assets offset_r(Assets) const;    

  unsigned nScenarios_;
  rational hedgePathDt_;
  boost::function<double ()> n_;
  ModelDynamics dynamics_;
  ContractTraits contractTraits_;
  double offset_S_, offset_r_;
};

Assets InsContrMCPricingModelFactory::offset_S(Assets a) const {
    a.S *= offset_S_;
    a.r = 0.0;
    a.intR = 0.0;
    return a;
}

Assets InsContrMCPricingModelFactory::offset_r(Assets a) const {
    a.S = 0.0;
    a.r = offset_r_;
    a.intR = 0.0;
    return a;
}

PricingModel<ValT,UnderlT,DeltaT>* InsContrMCPricingModelFactory::make(
    const rational& t, const Path<Assets>& assetPath,
    const Path<ContractStates>& contractStatePath) const
{
  if (t == contractStatePath.T()) {
    return new InsContrEndPointPricingModel(assetPath[t],contractStatePath[t]);
  } else {
    Path<Assets> hedgePath(hedgePathDt_, assetPath.T(), assetPath.t0());
    for (rational tt=hedgePath.t0(); tt <= t; tt += hedgePath.dt()) 
      hedgePath[tt] = assetPath[tt];

    boost::function<ScenT ()> scenarioGenerator
      = ScenarioGenerator(hedgePath.dt(), hedgePath.T(), t, n_);


    boost::function<ValT (const ScenT&)> contractPricer
      = boost::bind(valueContractFromVariates, t, hedgePath,
                    _1, contractStatePath, contractTraits_, dynamics_);

    boost::function<UnderlT (const ScenT&)> underlyingsPricer
      = boost::bind(underlyingsFromVariates, t, hedgePath, _1, dynamics_);

    std::pair<Assets,Assets> offsets;
    offsets.first  = offset_S(hedgePath[t]);
    offsets.second = offset_r(hedgePath[t]);

    boost::function<ValT (const rational&, Path<Assets>&, const Assets&)>
      contractEvaluatorForDelta
      = boost::bind(valueContractFromPathWithOffset,_1,_2,_3,
                    contractStatePath,contractTraits_);

    boost::function<DeltaT (const ScenT&)> deltaPricer
      = boost::bind(computeStockBondFDDeltaFromVariates, t, hedgePath,
                    _1, dynamics_, contractEvaluatorForDelta,
                    &stockFromPathWithOffset, &discountBondFromPathWithOffset,
                    offsets);
       
    return new MCPricingModel<ValT,UnderlT,DeltaT,ScenT> (nScenarios_,
                                                          scenarioGenerator,
                                                          contractPricer,
                                                          underlyingsPricer,
                                                          deltaPricer);
    }
}

}

#endif
