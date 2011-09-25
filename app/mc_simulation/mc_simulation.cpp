#include <fstream>
#include <vector>
#include <algorithm>

#include <ql_extensions.hpp>

#include "program_options.hpp"
#include "helpers.hpp"

namespace qe=QuantLibExt;
namespace fmc=FuncMC;
 
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

void expandOrCropParameters(std::vector<Param_ptr>& parameters, unsigned n) 
{
    if (parameters.size() == n) 
        return;
    else if (parameters.size() > n) 
        parameters.resize(n);
    else {
        QL_REQUIRE(((size_t) n) % parameters.size() == 0,
            "expandOrCropParameters: n not divisible by parameters.size()");
        std::vector<Param_ptr> new_parameters(n);
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

std::vector<Param_ptr> setupParameters(const ProgramOptions& options)
{
    std::vector<Param_ptr> parameters;
    if (options.model() == "BSVasicek") {
        parameters = parseParameters<BlackScholesVasicek_MCMC_parameters>(
                options.MCMC_parameter_filename());
    } else if (options.model() == "CEV_CKLS") {
        parameters = parseParameters<CEV_CKLS_MCMC_parameters>(
                options.MCMC_parameter_filename());
    }
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

    std::vector<Param_ptr> parameters = setupParameters(options);

    qe::ModelDynamics riskNeutralDynamics = qe::makeRiskNeutralDynamics(
                &(*options.getRiskNeutralParameters()));

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
                   boost::bind(qe::singleProfitAndLossSimulation,_1,_2,
                               options.getAssetPathTraits(),options.getContractTraits(),
                               computeProfitAndLoss));

    writeResults(results,options.outputFilename());
    return 0;
}
