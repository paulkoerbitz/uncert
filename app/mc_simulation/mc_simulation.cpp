#include <fstream>
#include <vector>
#include <algorithm>

#include <ql_extensions.hpp>

#include "program_options.hpp"
#include "helpers.hpp"

namespace qe=QuantLibExt;
 
void writeResults(const std::vector<qe::ValueVector>& results,
                  const std::string& outfilename) 
{
    std::ofstream out;
    out.open(outfilename.c_str());
    QL_REQUIRE(out.is_open(), "Can't open file " + outfilename);

    BOOST_FOREACH(qe::ValueVector res, results) {
        out << res << std::endl;
    }
    out.close();
}

void expandOrCropParameters(std::vector<std::vector<double> >& parameters, unsigned n) 
{
    if (parameters.size() == n) 
        return;
    else if (parameters.size() > n) 
        parameters.resize(n);
    else {
        QL_REQUIRE(((size_t) n) % parameters.size() == 0,
            "expandOrCropParameters: n not divisible by parameters.size()");
        std::vector<std::vector<double> > new_parameters(n);
        unsigned quotient = n / parameters.size();
        for (unsigned i=0; i<new_parameters.size(); ++i)
            new_parameters[i] = parameters[i/quotient];
        parameters = new_parameters;
    }
}

qe::ProfitAndLossComputer setupProfitAndLossComputingFunction(
         const ProgramOptions& options,
         const qe::ModelDynamics& riskNeutralDynamics,
         const qe::ValueVector& initialValue) 
{
    qe::ProfitAndLossComputer computeProfitAndLoss;
    if (options.doHedging()) {
        qe::HedgeTraits hedgeTraits = options.getHedgeTraits();
        boost::shared_ptr<qe::InsContrMCPricingModelFactory> 
          p_PricingFactory(
            new qe::InsContrMCPricingModelFactory(
                    hedgeTraits.nSamplesInnerMC,
                    hedgeTraits.dt,
                    qe::NormalRandomNumberGenerator(options.getSeed()),
                    riskNeutralDynamics,
                    options.getContractTraits(),
                    hedgeTraits.offset_S,
                    hedgeTraits.offset_r));

        computeProfitAndLoss = boost::bind(
                qe::computeReplicationProfitAndLoss,
                initialValue,
                _1,_2,_3,
                p_PricingFactory,hedgeTraits.dt);
    } else {
        computeProfitAndLoss = boost::bind(
                qe::computeZeroHedgeProfitAndLoss, initialValue,
                _1,_2,_3);
    }
    return computeProfitAndLoss;
}

std::vector<std::vector<double> > setupParameters(const ProgramOptions& options)
{
    std::vector<std::vector<double> > parameters
		= parseParameters(options.MCMC_parameter_filename());
    expandOrCropParameters(parameters, options.getNumberOfPaths());
    return parameters;
}

std::vector<unsigned> setupSeeds(const ProgramOptions& options) 
{
    std::vector<unsigned> seeds(options.getNumberOfPaths());
    for (unsigned i=0; i<seeds.size(); ++i)
        seeds[i] = options.getSeed() + i;
    return seeds;
}

int main(int ac, char** av) 
{
    ProgramOptions options;
    options.parseCommandline(ac,av);
    options.debugPrint();

    std::vector<std::vector<double> > parameters = setupParameters(options);

    qe::ModelDynamics riskNeutralDynamics
		= qe::makeRiskNeutralDynamics(options.getRiskNeutralParameters(),
									  options.model());

    qe::ValueVector initialValue = simpleMC(options.nPathsInitialMc(),
                                            options.getSeed(),
                                            riskNeutralDynamics,
                                            options.getAssetPathTraits(),
                                            options.getContractTraits());
   
    qe::ProfitAndLossComputer computeProfitAndLoss =
        setupProfitAndLossComputingFunction(options,riskNeutralDynamics,initialValue);

    std::vector<qe::ValueVector> results(options.getNumberOfPaths());
    std::vector<unsigned> seeds = setupSeeds(options);

    std::transform(parameters.begin(),parameters.end(),seeds.begin(),results.begin(),
                   boost::bind(qe::singleProfitAndLossSimulation,_1,options.model(),_2,
                               options.getAssetPathTraits(),options.getContractTraits(),
                               computeProfitAndLoss));

    writeResults(results,options.outputFilename());
    return 0;
}
