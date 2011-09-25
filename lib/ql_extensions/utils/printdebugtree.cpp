#include <utils/printdebugtree.hpp>
#include <sstream>

void printTree(std::string title, const std::vector<std::vector<double> > &values,
               unsigned width, unsigned precision ) 
{
    unsigned nSteps = values.size();
    unsigned nNodes = std::max(values.begin()->size(), (values.end()-1)->size());
    unsigned totalWidth = (width+1)*nNodes;

    std::string ruler(totalWidth,'-');
    std::cout << ruler << std::endl ;
    std::cout << std::string((totalWidth-title.length())/2,' ')
              << title << std::endl ;
    std::cout << ruler << std::endl ;
    
    std::string whitespace(width+1,' ');

    for (unsigned i=0; i<nSteps; ++i) {
        unsigned nNodesHere = values[i].size();
        for (unsigned j=0; j<(nNodes-nNodesHere)/2; ++j)
            std::cout << whitespace;
        for (unsigned j=0; j<nNodesHere; ++j)
            std::cout << std::setw(width) << std::setprecision(precision)
                      << values[i][j] << " ";
        std::cout << std::endl ;
    }
}

namespace QuantLib {
/****
 * NodeVisualization definitions
 */
void NodeVisualization::addNode(Size i, Integer xOffset, Integer yOffset, Real x, Real y,
                 std::vector<Real> p, std::vector<Size> d)
{
    if (xOffset != lastXOffset_) {
        lastXOffset_ = xOffset;
        nextLine();
    }
    ensureSize(rowsPerNode_, colsPerNode_);

    // print node
    std::stringstream sstream;
    data_[row_] = data_[row_].replace(col_, colsPerNode_, 
            " index         p       d    ");
    for (Size n=0; n<9; ++n)  {
        sstream.str(std::string());
        switch (n) {
            case 0:
                sstream << std::setw(8) << std::right << i << " ";
                break;
            case 1:
                sstream << " xOffset ";
                break;
           case 2:
                sstream << std::setw(8) << std::right << xOffset << " ";
                break;
           case 3:
                sstream << " yOffset ";
                break;
           case 4:
                sstream << std::setw(8) << std::right << yOffset << " ";
                break;
           case 5:
                sstream << " x       ";
                break;
           case 6:
                sstream << std::setw(8) << std::right 
                        << std::setprecision(3) << x << " ";
                break;
           case 7:
                sstream << " y       ";
                break;
           case 8:
                sstream << std::setw(8) << std::right 
                        << std::setprecision(3) << y << " ";
                break;
        }


        sstream << direction_[n] 
                << std::setw(7) << std::right 
                << std::setprecision(3) << p[n] << " "
                << std::setw(7) << std::right << d[n] << " ";
        data_[row_+1+n] = 
            data_[row_+1+n].replace(col_, colsPerNode_, sstream.str());
    }

    // maintain invariants
    col_ += colsPerNode_;
}

void NodeVisualization::nextLine() {
    row_ += rowsPerNode_;
    row_ += 1; // give me some space man!
    col_ = 0;
    ensureSize(0,0);
}

void NodeVisualization::ensureSize(Size nRows, Size nCols) {
    while (row_+nRows > data_.size()) {
        data_.push_back(std::string());
    }

    for (Size n = row_; n<row_+nRows; ++n) {
        data_[n].resize(col_+nCols,' ');
    }
}

std::ostream& operator<<(std::ostream &out, const NodeVisualization &nv) {
    for (Size n=0; n<nv.row_; ++n) {
        out << nv.data_[n] << std::endl;
    }
    return out;
}

} // namespace QuantLib
