#ifndef print_state_vector_hpp__
#define print_state_vector_hpp__


#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <boost/function.hpp>

#include <ql/quantlib.hpp>

class TwoDTreeVisualization;

class Node {
  public:
    Node(unsigned rowsPerNode, unsigned colsPerNode,
         unsigned width, unsigned precision,
         std::vector<std::string> fieldNames) 
        : rowsPerNode_(rowsPerNode), colsPerNode_(colsPerNode),
          width_(width), precision_(precision),
          fieldNames_(fieldNames)
    {}

    unsigned rowsPerNode() { return rowsPerNode_; }
    unsigned colsPerNode() { return colsPerNode_; }

    Node spawn(std::vector<double> data) const ;

    friend TwoDTreeVisualization& operator<<(TwoDTreeVisualization& out,
            const Node& n);

    virtual std::vector<std::string> toStringVector() const;

  private:
    unsigned rowsPerNode_;
    unsigned colsPerNode_;
    unsigned width_;
    unsigned precision_;
    std::vector<std::string> fieldNames_;
    std::vector<double>      fieldData_;
};

// this visualizes a 2D-Tree, 
// leaves node visualiztion to the respective node class
class TwoDTreeVisualization {
  public:
    TwoDTreeVisualization(unsigned rowsPerNode) :
        row_(0), col_(0), rowsPerNode_(rowsPerNode)
    {}

    void nextLine();

    friend std::ostream& operator<<(std::ostream &out,
                                    const TwoDTreeVisualization &visTree);

    friend TwoDTreeVisualization& operator<<(TwoDTreeVisualization& out,
            const Node& n);

  private:
    void ensureSize(unsigned nRows, unsigned nCols);

    std::vector<std::string> data_;
    unsigned row_;
    unsigned col_;
    unsigned rowsPerNode_;
};

#endif
