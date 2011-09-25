#include <utils/printstatevector.hpp>

/*****************************
 * Node
 ****************************/
std::vector<std::string> Node::toStringVector() const {
    std::vector<std::string> output;
    std::stringstream s;

    for (unsigned i=0; i<fieldNames_.size(); ++i) {
        s << fieldNames_[i] << std::setw(width_) 
          << std::setprecision(precision_) << fieldData_[i]
          << " ";
        output.push_back( s.str() );
        s.str("");
    }
    return output;
}

Node Node::spawn(std::vector<double> data) const {
    Node n(this->rowsPerNode_, this->colsPerNode_, 
           this->width_, this->precision_,
           this->fieldNames_);
    // std::cout << this->width_ << ", " << this->precision_ << std::endl;
    // std::cout << n.width_ << ", " << n.precision_ << std::endl;
    n.fieldData_ = data;
    return n;
}

/*****************************
 * TwoDTreeVisualization
 ****************************/

void TwoDTreeVisualization::nextLine() {
    row_ += rowsPerNode_;
    row_ += 1; // give me some space man!
    col_ = 0;
    ensureSize(0,0);
}

void TwoDTreeVisualization::ensureSize(unsigned nRows, unsigned nCols) {
    while (row_+nRows > data_.size()) {
        data_.push_back(std::string());
    }

    for (unsigned n = row_; n<row_+nRows; ++n) {
        data_[n].resize(col_+nCols,' ');
    }
}

std::ostream& operator<<(std::ostream &out, const TwoDTreeVisualization &nv) {
    for (unsigned n=0; n<nv.data_.size(); ++n) {
        out << nv.data_[n] << std::endl;
    }
    return out;
}

TwoDTreeVisualization& operator<<(TwoDTreeVisualization& out, const Node& n) {

    std::vector<std::string> output;
    output = n.toStringVector();

    out.ensureSize(output.size(),0);

    for (unsigned i=0; i<output.size(); ++i) {
        out.data_[out.row_+i] += output[i];
    }

    out.col_ += n.colsPerNode_;

    return out;
}
