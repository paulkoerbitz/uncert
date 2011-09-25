#ifndef ql_extensions__utils__matrix_parser__
#define ql_extensions__utils__matrix_parser__

#include <vector>
#include <boost/tokenizer.hpp>
#include <flens/flens.h>

namespace QuantLibExt
{

typedef flens::SyMatrix<flens::FullStorage<double, flens::ColMajor> > SYMatrix;

std::vector<double> csvlineToVector(std::string line) 
{
    std::vector<double> result;

    using namespace boost;
    boost::tokenizer<boost::escaped_list_separator<char> > tk(
       line, boost::escaped_list_separator<char>('\\', ',', '\"'));
    for (boost::tokenizer<boost::escaped_list_separator<char> >::iterator i(tk.begin());
       i!=tk.end();++i) 
    {
       result.push_back(atof(i->c_str()));
    }

    return result;
}

std::vector<std::vector<double> > csvToMatrix(std::string csvfilename) 
{
    std::ifstream in(csvfilename.c_str());
    QL_REQUIRE(in.is_open(), "Can't open " + csvfilename);

    std::vector<std::vector<double> > matrix;
    std::string line;
    while (getline(in,line)) {
        if (line != "") {
            matrix.push_back(csvlineToVector(line));
        }
    }
    in.close();
    return matrix;
}

SYMatrix parseSymmetricMatrix(std::string filename) {
    std::vector<std::vector<double> > data = csvToMatrix(filename);
    SYMatrix matrix = SYMatrix(data.size(),flens::Upper);
    
    for (unsigned i_row=0; i_row<data.size(); ++i_row) {
        QL_REQUIRE(data.size() == data[i_row].size() ,"Matrix must be square");
        for (unsigned i_col=i_row; i_col < data[i_row].size(); ++i_col) {
            // Flens matrices are 1-based
            matrix(i_row+1,i_col+1) = data[i_row][i_col];
        }
    }
    return matrix;
}

}

#endif
