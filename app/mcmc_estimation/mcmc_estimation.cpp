#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <stdexcept>

#include <boost/shared_ptr.hpp>
#include <boost/random/mersenne_twister.hpp>

#include <ql/quantlib.hpp>

#include <ql_extensions.hpp>

#include "program_options.hpp"

#define CONFIG_FILE "conf/mcmc_estimation/mcmc_estimation.cfg" 

namespace ql = QuantLib;
namespace qe = QuantLibExt;

typedef flens::SyMatrix<flens::FullStorage<double, flens::ColMajor> > SYMatrix;

boost::shared_ptr<qe::McmcAlgo> setupMcmcAlgo(const ProgramOptions& options,
                                              boost::shared_ptr<qe::McmcModel> p_Model)
{
    boost::mt19937 MersenneTwister(1u);
    boost::shared_ptr<qe::McmcAlgo> p_McmcAlgo;

    if ( options.algo() == "rwalk" ) {
        p_McmcAlgo.reset(new qe::RandomWalkMH(p_Model,
                                              options.start(),
                                              options.lb(),
                                              options.ub(),
                                              MersenneTwister,
                                              options.tune(),
                                              options.thin()));
    } else if ( options.algo() == "indep" ) {
        p_McmcAlgo.reset(new qe::IndependentMH(p_Model,
                                               options.start(),
                                               options.lb(),
                                               options.ub(),
                                               options.mean(),
                                               MersenneTwister,
                                               options.tune(),
                                               options.thin()));
    }
    return p_McmcAlgo;
}

boost::shared_ptr<qe::McmcModel> setupMcmcModel(const ProgramOptions& options) {

    boost::shared_ptr<qe::McmcModel> p_Model;
    if ( options.isSinglePathModel() ) {
        ql::Path path = qe::parsePath(options.file(), options.dt());
        if ( options.model() == "BS" ) {
            p_Model.reset(new qe::BlackScholesMcmcModel(path));
        } else if ( options.model() == "Cev" ) {
            p_Model.reset(new qe::CevMcmcModel(path));
        } else if ( options.model() == "Vasicek" ) {
            p_Model.reset(new qe::VasicekMcmcModel(path));
        } else if ( options.model() == "Ckls" ) {
            p_Model.reset(new qe::CklsMcmcModel(path));
        }
    } else {
        ql::MultiPath path = qe::parseMultiPath(options.file(), options.dt(), 2);
        if ( options.model() == "BsVasicek" ) {
            p_Model.reset(new qe::BlackScholesVasicekMcmcModel(path));
        } else if ( options.model() == "CevCkls" ) {
            p_Model.reset(new qe::CevCklsMcmcModel(path));
        } 
    }
    return p_Model;
}

void writeResults(const std::vector<std::vector<double> >& parameters,
				  const std::string& outfilename) {
	std::ofstream ofile(outfilename.c_str());
	if (ofile) {
		for (std::vector<std::vector<double> >::const_iterator i = parameters.begin();
			 i != parameters.end(); ++i) {
			std::copy(i->begin(), i->end(), std::ostream_iterator<double>(ofile,","));
			ofile << std::endl;
		}
		ofile.close();
	} else {
		throw std::runtime_error("Can't open file for writing: " + outfilename);
	}
}

int main(int ac, char** av)
{
    try {

        ProgramOptions options(ac, av, CONFIG_FILE);
        if (not options.options_valid()) {
            options.print_errors();
            return 1;
        } else {
            options.print_status();
        }

        boost::shared_ptr<qe::McmcAlgo>
            p_McmcAlgo(setupMcmcAlgo(options, setupMcmcModel(options)));

        for (unsigned i=0; i < options.burn(); ++i)
            p_McmcAlgo->next_scenario();

        std::vector< std::vector<double> > parameters;
        for (unsigned i=0; i < options.N(); ++i)
            parameters.push_back(p_McmcAlgo->next_scenario());

        writeResults(parameters, options.outfile());

    } catch (std::exception& e) {
        std::cerr << "main(): caught exception, message: " << std::endl
                  << e.what() << std::endl ;
        return 1;
    } catch (...) {
        std::cerr << "unknown error" << std::endl;
        return 1;
    }
    return 0;
}
