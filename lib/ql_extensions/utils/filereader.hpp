#ifndef file_reader_hpp__
#define file_reader_hpp__

#include <fstream>
#include <string>

namespace QuantLibExt {

class FileReader {
  public:
    FileReader(std::string filename);
    FileReader& operator>>(std::string& line);
    std::string next();
    void skipNLines(unsigned n);
    ~FileReader();

  private:
    FileReader(const FileReader& other);
    FileReader& operator=(const FileReader& other);

    std::ifstream file;
};


bool file_exists(const std::string& filename);

} // namespace QuantLibExt

#endif
