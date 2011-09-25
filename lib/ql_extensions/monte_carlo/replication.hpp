#ifndef ql_extensions__monte_carlo__replication_hpp__
#define ql_extensions__monte_carlo__replication_hpp__

#include <vector>
#include <algorithm>

#include <iostream>

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/rational.hpp>
#include <boost/format.hpp>

#include <ql/quantlib.hpp>

#include "../instruments/termfixinsurance/valuevector.hpp"
#include "../math/array.hpp"
#include "path.hpp"
#include "mcmodel.hpp"
#include "insurance_contract.hpp"

#ifdef PATHDEBUG
#include "pathdebug.hpp"
#endif

namespace QuantLibExt {

namespace ql=QuantLib;

typedef ValueVector        ValT;
typedef Array<>                UnderlT;
typedef Array<ValueVector> DeltaT;
typedef Path<Variates>         ScenT;
typedef boost::shared_ptr<InsContrMCPricingModelFactory> Shared_PF_Pointer;

struct HedgeTraits {
    rational dt;
    unsigned nSamplesInnerMC;
    double offset_S;
    double offset_r;
};

void updateDeltasAndMoneyAccount
                (const rational& t
                ,const Path<Assets>& assetPath
                ,const Path<ContractStates>& contractStatePath
                ,const Shared_PF_Pointer &p_PricerFactory
                ,Array<ValueVector>& oldDeltas
                ,ValueVector& moneyAccount
#ifdef PATHDEBUG
                ,std::vector<PathDebugInfo>& pdi
#endif
                ) 
{
    boost::shared_ptr<PricingModel<ValT,UnderlT,DeltaT> >
        p_Pricer(p_PricerFactory->make(t,assetPath,contractStatePath));
    UnderlT underlyings = p_Pricer->underlyings();
    DeltaT newDeltas = p_Pricer->deltas();
    moneyAccount = DotProduct(oldDeltas-newDeltas,underlyings,moneyAccount);
    oldDeltas = newDeltas;
#ifdef PATHDEBUG
    pdi.push_back(PathDebugInfo(boost::rational_cast<double>(t)
                               ,underlyings[0]
                               ,assetPath[t].r
                               ,0.0
                               ,contractStatePath.lastIteratorOnOrBeforeTime(t)->L
                               ,underlyings[1]
                               ,newDeltas[0]
                               ,newDeltas[1]
                               ,moneyAccount
                               ,p_Pricer->value()));
#endif 
}

double compoundingFactor(const rational& old_t, const rational& t 
                        ,const Path<Assets>& assetPath) {
    double sumIntR = 0.0;
    Path<Assets>::const_iterator start = assetPath.iteratorAtTime(old_t);
    Path<Assets>::const_iterator end   = assetPath.iteratorAtTime(t)+1;
    for ( ++start ; start != end; ++start)
        sumIntR += start->intR;
    return std::exp(sumIntR);
}

ValueVector computeReplicationProfitAndLoss(
         ValueVector initialValue
        ,const Path<Assets>& assetPath
        ,const Path<ContractStates>& contractStatePath
        ,const Path<ValueVector>& payoffPath
        ,boost::shared_ptr<InsContrMCPricingModelFactory> p_PricerFactory
        ,rational hedgeDt) 
{
    ValueVector moneyAccount = initialValue;
    DeltaT deltas = Array<ValueVector>(2); // Constructor initializes with zeros

#ifdef PATHDEBUG
    std::vector<PathDebugInfo> pdi;
#endif 

    for (rational t, old_t; t <= contractStatePath.T(); t += hedgeDt) {
        moneyAccount *= compoundingFactor(old_t,t,assetPath);

        if (payoffPath.hasTimepointAt(t)) 
            moneyAccount -= payoffPath[t];

        updateDeltasAndMoneyAccount(t,assetPath,contractStatePath
                ,p_PricerFactory,deltas,moneyAccount
#ifdef PATHDEBUG
                ,pdi
#endif
                );
#ifdef PATHDEBUG
        pdi.back().intR = std::log(compoundingFactor(old_t,t,assetPath));
#endif
        old_t = t;
    }

#ifdef PATHDEBUG
    printPDIV(pdi);
#endif
    return moneyAccount;
}

ValueVector computeZeroHedgeProfitAndLoss(
         ValueVector initialValue
        ,const Path<Assets>& assetPath
        ,const Path<ContractStates>& 
        ,const Path<ValueVector>& payoffPath)
{
    ValueVector moneyAccount = initialValue;

    rational t, old_t;
    while (true) {
        double compFact = compoundingFactor(old_t,t,assetPath);
        moneyAccount *= compoundingFactor(old_t,t,assetPath);
        moneyAccount -= payoffPath[t];
        std::cout << "t=" << (boost::format("%3d") % t.numerator())
                  << " || moneyAccount = " << moneyAccount
                  << " || compFact = " << compFact
                  << " || assetPath : "
                  << (boost::format("S: %8.2f, r: %8.4f, intR: %8.4f")
                      % assetPath[t].S % assetPath[t].r % assetPath[t].intR)
                  << std::endl;

        if (t < payoffPath.T()) {
            old_t = t;
            t = payoffPath.firstIteratorAfterTime(t).t();
        } else
            break;
    }
    moneyAccount /= compoundingFactor(rational(),payoffPath.T(),assetPath);

    return moneyAccount;
}

}
#endif
