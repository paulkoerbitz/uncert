#ifndef funcMC_wrappers_hpp__
#define funcMC_wrappers_hpp__

#include <vector>
#include <iostream>

#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>
#include <boost/rational.hpp>
#include <boost/foreach.hpp>

#include "../instruments/termfixinsurance/valuevector.hpp"

#include "path.hpp"
#include "dynamics.hpp"
#include "insurance_contract.hpp"
#include "replication.hpp"

namespace QuantLibExt {

ValueVector simpleMC(unsigned nSamples
                        ,unsigned seed
                        ,const ModelDynamics& riskNeutralDynamics
                        ,const AssetPathTraits& assetPathTraits
                        ,const ContractTraits& contractTraits) 
{
    boost::function<double ()> scenarioGenerator = 
        NormalRandomNumberGenerator(seed);

    boost::shared_ptr<InsContrMCPricingModelFactory> p_PricingFactory
      (new InsContrMCPricingModelFactory
       (nSamples,assetPathTraits.dt, scenarioGenerator,
        riskNeutralDynamics, contractTraits));

    Path<Assets> assetPath(assetPathTraits.dt,assetPathTraits.T);
    assetPath[0] = assetPathTraits.initialAssetValues;

    Path<ContractStates> contractStatePath(contractTraits.dt,contractTraits.T);
    contractStatePath[0] = contractTraits.initialContractStates;

    boost::shared_ptr<
        PricingModel<ValT, UnderlT, DeltaT> >
        p_Pricer(p_PricingFactory->make(rational(0,1),assetPath
                                       ,contractStatePath));
    return p_Pricer->value();
}

Path<Assets> generateRealWorldAssetPath(
         boost::function<double ()>& rndNumberGenerator,
		 AssetPathTraits assetPathTraits,
		 const std::vector<double>& p,
		 const std::string& model_name)
{
    boost::function<Assets (const Variates&,const Assets&, double)>
		realWorldDynamics = makeRealWorldDynamics(p, model_name);

    Path<Variates> variates = makeVariates(assetPathTraits.dt,
										   assetPathTraits.T,
										   assetPathTraits.t0,
										   rndNumberGenerator);

    return makePathFromVariates(variates,assetPathTraits.initialAssetValues,
								realWorldDynamics);
}


typedef boost::function<ValueVector (const Path<Assets>&
        ,const Path<ContractStates>&, const Path<ValueVector>&)>
        ProfitAndLossComputer;

ValueVector singleProfitAndLossSimulation(
		const std::vector<double>& p,
		const std::string& model_name,
		unsigned seed,
		const AssetPathTraits& assetPathTraits,
		const ContractTraits& contractTraits,
        const ProfitAndLossComputer& computeProfitAndLoss)
{
    boost::function<double ()> rndNumberGenerator 
        = NormalRandomNumberGenerator(seed);

    Path<Assets> assetPath 
        = generateRealWorldAssetPath(rndNumberGenerator, assetPathTraits,
									 p, model_name);

    Path<ContractStates> contractStatePath 
        = makeContractStatePath(assetPath, contractTraits);
    Path<ValueVector> contractPayoffs
        = payoffPathFromContractStates(contractStatePath);

    return computeProfitAndLoss(assetPath, contractStatePath, contractPayoffs);
}

}

#endif
