#ifndef ql_extensions__utils__pathparser_hpp
#define ql_extensions__utils__pathparser_hpp

#include <fstream>
#include <vector>
#include <string>

#include <ql/quantlib.hpp>

#include "csvparser.hpp"

namespace ql=QuantLib;

namespace QuantLibExt {

ql::Path parsePath(std::string filename, double dt);
ql::MultiPath parseMultiPath(std::string filename, double dt, unsigned numPaths);

}

#endif
