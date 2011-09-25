#include <fstream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <boost/tokenizer.hpp>
#include <boost/shared_ptr.hpp>

double string_to_double(const std::string& s) {
	return atof(s.c_str());
}

std::vector<std::vector<double> > parseParameters(std::string filename)
{
    std::vector<std::vector<double> > parameters;
	std::ifstream infile(filename.c_str());
    QL_REQUIRE(infile.is_open(), "Can't open file " + filename);
	std::string line;

	while(std::getline(infile, line)) {
		std::vector<double> params;
		boost::tokenizer<boost::escaped_list_separator<char> > tok(line);
		std::transform(tok.begin(), tok.end(), std::back_inserter(params),
					   string_to_double);
		parameters.push_back(params);
	}
    return parameters;
}
