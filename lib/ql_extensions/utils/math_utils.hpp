#ifndef MATH_UTILS_HPP__
#define MATH_UTILS_HPP__

#include <vector>
#include <boost/shared_ptr.hpp>
//#include <flens/flens.h>

#include <iostream>

namespace QuantLibExt {

boost::shared_ptr<std::vector<double> >
    logspace(double a, double b, unsigned nSteps, double base=2.0);

double line(double x, double x1, double y1, double x2, double y2);

void ComplexToNPower(double& real, double& img, unsigned power);

template <typename I>
bool isDivisible(I dividend, I divisor) {
    return dividend % divisor == 0;
}

} // namespace QuantLibExt
#endif
