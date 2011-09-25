#ifndef ql_extensions__monte_carlo__mcmodel_hpp__
#define ql_extensions__monte_carlo__mcmodel_hpp__ 

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "pricingmodel.hpp"

namespace QuantLibExt {

template <class ValT, class ScenT>
ValT computeMCExpectations(boost::function<ScenT ()> scenarioGenerator
                          ,boost::function<ValT (const ScenT&)> evaluator
                          ,unsigned nScenarios) 
{
    ValT accumulator = evaluator(scenarioGenerator());
    for (unsigned i=1; i<nScenarios; ++i) 
        accumulator += evaluator(scenarioGenerator());
    return accumulator / (double)nScenarios;
}

template <class ValT, class UnderlT, class DeltaT, class ScenT>
class MCPricingModel : public PricingModel<ValT,UnderlT,DeltaT> {
  public:
    MCPricingModel(
            unsigned nScenarios
           ,const boost::function<ScenT ()> scenarioGenerator
           ,const boost::function<ValT (const ScenT&)> contractPricer
           ,const boost::function<UnderlT (const ScenT&)> underlyingsPricer
           ,const boost::function<DeltaT (const ScenT&)> deltaPricer) 
        : nScenarios_(nScenarios)
         ,scenarioGenerator_(scenarioGenerator)
         ,contractPricer_(contractPricer)
         ,underlyingsPricer_(underlyingsPricer)
         ,deltaPricer_(deltaPricer)
    {}
    virtual ValT    value() const; 
    virtual UnderlT underlyings() const;
    virtual DeltaT  deltas() const;

  protected:
    unsigned nScenarios_;
    boost::function<ScenT   ()>             scenarioGenerator_;
    boost::function<ValT    (const ScenT&)> contractPricer_;
    boost::function<UnderlT (const ScenT&)> underlyingsPricer_;
    boost::function<DeltaT  (const ScenT&)> deltaPricer_;
};

template <class ValT, class UnderlT, class DeltaT, class ScenT>
ValT MCPricingModel<ValT,UnderlT,DeltaT,ScenT>::value() const {
    return computeMCExpectations(scenarioGenerator_ 
            ,contractPricer_, nScenarios_);
}

template <class ValT, class UnderlT, class DeltaT, class ScenT>
UnderlT MCPricingModel<ValT,UnderlT,DeltaT,ScenT>::underlyings() const {
    return computeMCExpectations(scenarioGenerator_ 
            ,underlyingsPricer_, nScenarios_);
}

template <class ValT, class UnderlT, class DeltaT, class ScenT>
DeltaT MCPricingModel<ValT,UnderlT,DeltaT,ScenT>::deltas() const {
    return computeMCExpectations(scenarioGenerator_ 
            ,deltaPricer_, nScenarios_);
}

}

#endif
