#ifndef PaulsExtensions__TestUtils__hpp__
#define PaulsExtensions__TestUtils__hpp__

#include <cmath>
#include <boost/format.hpp>

namespace PaulsTestUtils {

#define TOLERANCE 1E-6

bool resultsAbsolutelyClose(double result, double expected
                           ,double tol=TOLERANCE) {
    return std::abs(result-expected) < tol;
}

bool resultsRelativelyClose(double result, double expected 
                           ,double tol=TOLERANCE) {
    return std::abs((result-expected) / expected) < tol;
}

bool resultsClose(double result, double expected, double tol=TOLERANCE) {
    if ( std::abs(expected < 1.0) ) {
        return resultsAbsolutelyClose(result,expected,tol);
    } else {
        return resultsRelativelyClose(result,expected,tol);
    }
}


std::string errorMessage(std::string prefix, double result, double expected) {
    return (boost::format("%5s : expected %14.7f, got %14.7f\n") 
                % prefix % expected % result).str();
}

bool closeEnoughOrMessage(std::string prefix
        , double result, double expected, std::string &message) {
    if (resultsClose(result,expected)) {
        return true;
    } else {
        message += errorMessage(prefix,result,expected);
        return false;
    }
}

}

#endif
