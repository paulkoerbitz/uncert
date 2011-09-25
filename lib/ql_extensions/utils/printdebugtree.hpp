#ifndef print_debug_tree_hpp
#define print_debug_tree_hpp

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <iomanip>

#include <boost/function.hpp>

#include <ql/types.hpp>

void printTree(std::string title, const std::vector<std::vector<double> > &values,
               unsigned width, unsigned precision ); 


// void debugOut2dTree(const std::vector<BivariateTrinomialTree::Branching> 
//                     &branchings, 
//                     std::vector<Real> dx, 
//                     Real x0)

namespace QuantLib {
class NodeVisualization {
  public:
    NodeVisualization()
        : row_(0), col_(0), rowsPerNode_(10), colsPerNode_(30),
          direction_(9) 
    {
        direction_[0] = "dd ";
        direction_[1] = "dm ";
        direction_[2] = "du ";
        direction_[3] = "md ";
        direction_[4] = "mm ";
        direction_[5] = "mu ";
        direction_[6] = "ud ";
        direction_[7] = "um ";
        direction_[8] = "uu ";
    }
    void addNode(Size i, Integer xOffset, Integer yOffset, Real x, Real y,
                 std::vector<Real> p, std::vector<Size> d);
    void nextLine();

    friend std::ostream& operator<<(std::ostream &out, 
                                    const NodeVisualization &n);
  private:
    void ensureSize(Size nRows, Size nCols);

    std::vector<std::string> data_;
    Size row_;
    Size col_;
    const Size rowsPerNode_;
    const Size colsPerNode_;
    Integer lastXOffset_;

    // title strings
    std::vector<std::string> direction_;
};

} // namespace QuantLib





#endif
