#ifndef funcMC__utils__hpp__
#define funcMC__utils__hpp__ 

#include <fstream>
#include <boost/shared_ptr.hpp>

typedef boost::shared_ptr<MCMC_parameters> Param_ptr;

template <class ParamType>
std::vector<Param_ptr> parseParameters(std::string filename)
{
    std::ifstream infile;
    infile.open(filename.c_str());
    QL_REQUIRE(infile.is_open(), "Can't open file " + filename);

    CSVParser csvparser;
    std::string sLine;
    std::vector<Param_ptr> Parameters;
    
    getline(infile,sLine); // throw away the first line - contains header

    while (!infile.eof()) {
        getline(infile,sLine);
        if (sLine == "")
            continue;

        Param_ptr p(new ParamType());
        csvparser << sLine;

        for (typename ParamType::iterator it = p->begin(); 
                                        it != p->end(); ++it)
            csvparser >> *it;

        Parameters.push_back(p);
    }
    return Parameters;
}

#endif
