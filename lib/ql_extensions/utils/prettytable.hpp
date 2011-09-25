#ifndef pretty_table_hpp__
#define pretty_table_hpp__

#include <iostream>
#include <iomanip>

// STL
#include <vector>
#include <string>


namespace fmt {

class prettyTable {
  public:
    prettyTable(unsigned precision=4, unsigned width=10, 
                bool fillRowWise=true, char separator='|');
    prettyTable& addValue(double value);
    prettyTable& addValue(double value, unsigned i, unsigned j);
    double       getValue(unsigned row, unsigned col) const;
    prettyTable& addTitle(std::string title);
    prettyTable& nextTitleRow();
    prettyTable& addColumnName(std::string name);
    prettyTable& nextColumnNameColumn();
    prettyTable& setPrecision(unsigned precision);
    prettyTable& setMinWidth(unsigned widht);
    prettyTable& setRowWiseFilling();
    prettyTable& setColumnWiseFilling();
   
    friend std::ostream& operator<<(std::ostream& out, 
                                    const prettyTable &table);
  protected:
    void ensureSize();
    void updateMinWidths() const;

  private:
    std::vector<std::vector<double> > values_;
    mutable std::vector<unsigned> min_widths_;
    unsigned precision_;
    unsigned min_width_;
    std::vector<std::vector<std::string> > rowTitles_;
    std::vector<std::vector<std::string> > colNames_;
    bool fillRowWise_;
    int row_, col_;
    int rowColNames_, colColNames_, rowRowTitles_, colRowTitles_;
    int maxRowColNames_, maxColRowTitles_;
    int maxRow_, maxCol_;
    char separator_;
};

}

#endif

