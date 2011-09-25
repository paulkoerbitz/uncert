#ifndef PaulsExtensions__testCaseParser__hpp__ 
#define PaulsExtensions__testCaseParser__hpp__

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <boost/regex.hpp>

namespace StringUtils {

const std::string RE_FLOAT("[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?");
const std::string RE_WHITESPACE("\\s*");

template <class T>
T parseString(std::string s) {
    std::istringstream i(s);
    T result;
    i >> result;
    return result;
}

template <class T>
std::vector<T> parseStringToVector(
                    const std::string &stringToParse, std::string sep) {
    std::vector<std::string> splits;
    boost::split(splits,stringToParse,boost::is_any_of(sep));

    std::vector<T> result;
    for (std::vector<std::string>::const_iterator is=splits.begin();
            is != splits.end(); ++is) {
        result.push_back(parseString<T>(*is));
    }
    return result;
}


bool contains(const std::string &searchString, const std::string substr) {
    return searchString.find(substr) != std::string::npos;
}

std::string makeRegexListOfItem(const std::string &item 
                               ,const std::string &sep) {
    return std::string( 
          item
        + std::string("(")
          + RE_WHITESPACE
          + sep
          + RE_WHITESPACE
          + item
          + RE_WHITESPACE
        + std::string(")*"));
}

std::string makeValidLineRegexString(const std::string &line_sep
                                    ,const std::string &keyword_sep 
                                    ,const std::string val_sep) {
    const std::string listOfFloats = makeRegexListOfItem(RE_FLOAT,val_sep);
    const std::string keywordWithFloatList = std::string("\\w+") 
        + RE_WHITESPACE + keyword_sep + RE_WHITESPACE + listOfFloats;

    return std::string(makeRegexListOfItem(keywordWithFloatList,line_sep));
}
typedef std::tr1::unordered_map<std::string,std::vector<double> > 
MapStringVectorOfDouble;

MapStringVectorOfDouble 
parseStringToMapOfVectors(std::string toParse, std::string line_sep=";" 
                         ,std::string keyword_sep=":"
                         , std::string value_sep=",") {
    
    std::string validLineRegexString = makeValidLineRegexString(
            line_sep ,keyword_sep ,value_sep); 
    boost::regex validLineRegex(validLineRegexString);
    
    if (not boost::regex_match(toParse, validLineRegex)) {
        throw std::runtime_error(
                "The string to parse : " + toParse + " is not valid.");
    }

    std::vector<std::string> lines;
    boost::split(lines, toParse, boost::is_any_of(line_sep));
    
    MapStringVectorOfDouble resultMap;
    for (std::vector<std::string>::const_iterator il=lines.begin();
            il != lines.end(); ++il) {
        std::vector<std::string> keywordAndValues;
        if ( contains(*il,keyword_sep) ) {
            boost::split(keywordAndValues,*il,boost::is_any_of(keyword_sep));
            resultMap.insert(MapStringVectorOfDouble::value_type(
                boost::trim_copy(keywordAndValues[0])
                ,parseStringToVector<double>(keywordAndValues[1],value_sep)));
        }
    }
    return resultMap;
}

void printMapContents(const MapStringVectorOfDouble &map) {
    for (MapStringVectorOfDouble::const_iterator it = map.begin(); 
            it != map.end(); ++it) {
        std::cout << it->first << " : " ;
        for (MapStringVectorOfDouble::mapped_type::const_iterator 
                iv = it->second.begin(); iv != it->second.end(); ++iv) {
            std::cout << *iv << ",";
        }
        std::cout << std::endl;
    }
}

}

#endif
