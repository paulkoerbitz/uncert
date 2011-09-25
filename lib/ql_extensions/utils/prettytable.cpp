#include <utils/prettytable.hpp>


namespace fmt {

prettyTable::prettyTable(unsigned precision, unsigned width, 
        bool fillRowWise, char separator)
    : precision_(precision), min_width_(width),
      fillRowWise_(fillRowWise),
      row_(0), col_(-1), 
      rowColNames_(-1), colColNames_(0), rowRowTitles_(0), colRowTitles_(-1),
      maxRowColNames_(-1), maxColRowTitles_(-1),
      //nRowsRowTitles_(1), nColsColNames_(1),
      maxRow_(0), maxCol_(-1),
      separator_(separator)
{}

void prettyTable::ensureSize() {
    maxRow_ = std::max( std::max(row_,maxRowColNames_) ,maxRow_);
    maxCol_ = std::max( std::max(col_,maxColRowTitles_) ,maxCol_);

    values_.resize(maxRow_+1);
    min_widths_.resize(maxCol_+1+colColNames_+1,min_width_);
    colNames_.resize(maxRow_+1);
    rowTitles_.resize(rowRowTitles_+1);

    for (unsigned row=0; row<=unsigned(maxRow_); ++row) {
        colNames_[row].resize(colColNames_+1);
        values_[row].resize(maxCol_+1);
    }

    for (unsigned row=0; row<unsigned(rowRowTitles_+1); ++row) {
        rowTitles_[row].resize(maxCol_+1);
    }
}

void prettyTable::updateMinWidths() const {
    for (unsigned col=0; col < unsigned(colColNames_+1); ++col) {
        // loop over all colNames_ entries
        for (unsigned row=0; row < colNames_.size(); ++row) {
            min_widths_[col] 
                = std::max(min_widths_[col],
                        unsigned(colNames_[row][col].size())+2); 
        }
    }
    for (unsigned col=0; col<=unsigned(maxCol_); ++col) {
        // loop over all rowTitles_ entries
        for (unsigned row=0; row < rowTitles_.size(); ++row) {
            min_widths_[col+colColNames_+1] 
                = std::max(min_widths_[col+colColNames_+1],
                        unsigned(rowTitles_[row][col].size())+2); 
        }
    }
}

prettyTable& prettyTable::addTitle(std::string title) {
    colRowTitles_++;
    maxColRowTitles_ = std::max(maxColRowTitles_, colRowTitles_);
    ensureSize();
    rowTitles_[rowRowTitles_][colRowTitles_] = title;

    return *this;
}

prettyTable& prettyTable::nextTitleRow() {
    maxColRowTitles_ = std::max(maxColRowTitles_, colRowTitles_);
    colRowTitles_ = -1;
    rowRowTitles_++;
    ensureSize();

    return *this;
}

prettyTable& prettyTable::addColumnName(std::string name) {
    rowColNames_++;
    maxRowColNames_ = std::max(maxRowColNames_, rowColNames_);
    ensureSize();
    colNames_[rowColNames_][colColNames_] = name;

    return *this;
}

prettyTable& prettyTable::nextColumnNameColumn() {
    //maxRowColNames_ = std::max(maxRowColNames_, rowColNames_);
    rowColNames_ = -1;
    colColNames_++;
    ensureSize();

    return *this;
}

prettyTable& prettyTable::setPrecision(unsigned precision) {
    precision_ = precision;
    return *this;
}

prettyTable& prettyTable::setMinWidth(unsigned width) {
    min_width_ = width;
    return *this;
}

prettyTable& prettyTable::setRowWiseFilling() {
    fillRowWise_ = true;
    return *this;
}

prettyTable& prettyTable::setColumnWiseFilling() {
    fillRowWise_ = false;
    return *this;
}


prettyTable& prettyTable::addValue(double value) {

    if (fillRowWise_) {
        if (col_ == maxCol_) {
            ++row_;
            col_ = 0;
        } else {
            ++col_;
        }
    } else {
        if (row_ == maxRow_) {
            ++col_;
            row_ = 0;
        } else {
            ++row_;
        }
    }
    ensureSize();
    values_[row_][col_] = value;

    return *this;
}

prettyTable& prettyTable::addValue(double value, unsigned row, unsigned col) {
    row_ = row;
    col_ = col;
    return addValue(value);
}

double prettyTable::getValue(unsigned row, unsigned col) const {
    return values_[row][col];
}

std::ostream& operator<<(std::ostream &out, const prettyTable &table) {

    table.updateMinWidths();

    // output titles
    for (unsigned i=0; i < table.rowTitles_.size(); ++i) {
        // space in upper left corner of the table
        for (unsigned k=0; k<table.colNames_[0].size(); ++k) {
            out << std::string(table.min_widths_[k],' ');
        }
        out << std::string(table.colNames_[0].size()-1,' ');
        for (unsigned j=0; j<table.rowTitles_[i].size(); ++j) {
           out << table.separator_ << ' ' 
               << std::setw(table.min_widths_[i+table.colNames_[0].size()]-1)
               << std::left << table.rowTitles_[i][j];
        }
        out << table.separator_ << std::endl;
    }

    // separator
    for (unsigned i=0; i<= table.colNames_[0].size()+unsigned(table.maxCol_); ++i) {
        out << std::string(table.min_widths_[i],'-');
        out << ((table.separator_ == '|') ? '+' : '-');
    }
    out << std::endl;

    for (unsigned row=0; row<=unsigned(table.maxRow_); ++row) {

        // All column names for this column
        for (unsigned col=0; col<table.colNames_[row].size(); ++col) {
            out << ' ' << std::left << std::setw(table.min_widths_[col]-1)
                << table.colNames_[row][col] << table.separator_;
        }

        for (unsigned col=0; col<=unsigned(table.maxCol_); ++col) {
            out << std::right 
                << std::setw(table.min_widths_[col+table.colColNames_+1])
                << std::setprecision(table.precision_) 
                << table.values_[row][col]
                << table.separator_;
        }
        out << std::endl;
    }

    return out;
}

} // namespace fmt



    
    
