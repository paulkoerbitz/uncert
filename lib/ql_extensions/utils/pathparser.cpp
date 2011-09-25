#include "pathparser.hpp"

namespace QuantLibExt {

ql::Path parsePath(std::string filename, double dt)
{
    // Parse file
    std::string sLine;
    CSVParser csvparser;

    std::ifstream infile(filename.c_str());
    if (not infile.good())
        throw "Can't open file!";

    std::vector<double> path_vals;
    while (!infile.eof()) {
        getline(infile,sLine);
        if (sLine == "")
            continue;

        double tmp;
        csvparser << sLine;
        csvparser >> tmp;
        path_vals.push_back(tmp);
    }
    infile.close();

    ql::Size n = path_vals.size();
    // Set up time grid
    ql::TimeGrid myGrid(dt*n,n-1);

    // Set up array
    ql::Array tmpArray(n);

    for (unsigned i=0; i<n; ++i)
        tmpArray[i] = path_vals[i];

    return ql::Path(myGrid, tmpArray);
}

ql::MultiPath parseMultiPath(std::string filename, double dt, unsigned numPaths)
{
    // Parse file
    std::string sLine;
    CSVParser csvparser;

    std::ifstream infile(filename.c_str());
    if (not infile.good())
        QL_FAIL("Can't open file!");
    
    const unsigned buf_size = 10000;

    std::vector<std::vector<double> > path_vals(numPaths,std::vector<double>());

    for (unsigned i=0; i<numPaths; ++i)
        path_vals[i].reserve(buf_size);

    double tmp;

    // now for the actual parsing
    while (!infile.eof()) {

        getline(infile,sLine);

        if (sLine == "")
            continue;

        csvparser << sLine;
        for (unsigned i=0; i<numPaths; ++i) {
            csvparser >> tmp;
            path_vals[i].push_back(tmp);
        }
    }
    infile.close();

    ql::Size n = path_vals[0].size();
    ql::TimeGrid myGrid(dt*n,n-1);

    std::vector< ql::Path > paths;

    for (unsigned i=0; i<numPaths; ++i) {
        // Set up array
        ql::Array tmpArray(n);

        for (unsigned j=0; j<n; ++j)
            tmpArray[j] = path_vals[i][j];

        paths.push_back(ql::Path(myGrid,tmpArray)); 
    }

    return ql::MultiPath(paths);
}

} // namespace QuantLibExt
