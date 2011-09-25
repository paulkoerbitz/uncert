#include <utils/filereader.hpp>

namespace QuantLibExt {

FileReader::FileReader(std::string filename) {
    file.open(filename.c_str());
    if (!file.good()) 
        throw("FileReader: Can't open file " + filename + ".\n");
}

FileReader::~FileReader() {
    if (file.is_open())
        file.close();
}

FileReader& FileReader::operator>>(std::string& line) {
    if (file.good())
        getline(file,line);
    else 
        line = "";
    return *this;
}

std::string FileReader::next() {
    std::string line;
    *this >> line;
    return line;
}

void FileReader::skipNLines(unsigned n) {
    std::string line;
    for (unsigned i=0; i<n; ++i) {
       line = this->next();
       if (line == "") 
           break;
    } 
}

bool file_exists(const std::string& filename)
{
	std::ifstream ifile(filename.c_str());
	return (bool) ifile;
}

} // namespace QuantLibExt
