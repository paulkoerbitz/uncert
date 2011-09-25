#include <utils/math_utils.hpp>
#include <stdexcept>
#include <cmath>

namespace QuantLibExt {

boost::shared_ptr<std::vector<double> >
    logspace(double a, double b, unsigned nSteps, double base)
{
    boost::shared_ptr<std::vector<double> > 
        pv(new std::vector<double>(nSteps+1u,0.0));

    unsigned first, last;
    int inc;
    double step;

    if ( a < b ) {
        first = 0;
        last = nSteps;
        inc = -1;
    }
    else {
        first = nSteps;
        last = 0;
        inc = 1;
    }

    step = b-a;
        
    (*pv)[first] = a;
    (*pv)[last] = b;
    for (unsigned i=last; i != first; i += inc)  {
        step /= base;
        (*pv)[i+inc] = a + step;
    }

    return pv;
}

double line(double x, double x1, double y1, double x2, double y2) {
    return y1 + (x-x1)/(x2-x1)*(y2-y1);
}

void  ComplexToNPower(double& real, double& img, unsigned n) {
    // Compute the sum
    //
    // sum_k=0..n  binom_coef(n,k)  * (i*img)^k * real^(n-k)
    //
    // since k determines the sign and 'realness' of the expression
    // we start from k=0 and work our way to k=n in steps of two
    
    // Rule out some special cases
    if (n == 0) {
        throw std::invalid_argument("ComplexToNPower: Illegal argument: n >= 1 required");
    } else if (n == 1) {
        return;
    }

    double real_sum = 0.0;
    double img_sum = 0.0;
    //double real_1 = 1.0;
    double real_exp = std::pow(real,double(n)); // initial real^(n-k) = real^n
    double img_exp = 1.0;               // initial img^k = img^0 = 1.0
    //double img_2 = std::pow(img,n);

    int bincoeff = 1;
    int sign = 1;

    for (unsigned k=0; k < n; k+=2) {
        // real part - for k even 
        real_sum += bincoeff * sign * img_exp * real_exp; 
        
        // imaginary part - for the next k so use k+1 (k+1 is odd)
        bincoeff = bincoeff*(n-k)/(k+1);
        real_exp /= real;
        img_exp *= img;
        img_sum += bincoeff*real_exp*img_exp;

        // increment bincoeff and real_1, img_2 
        // for next repetition -> so use k+2
        bincoeff = bincoeff*(n-k-1)/(k+2);
        real_exp /= real;
        img_exp *= img;
        // i^k is positive for k mod 4 == 0, 1 and
        // negative for k mod 4 == 2, 3 so switch sign
        sign *= -1;     
    }

    // if n is even we've skip the last real summand
    if (n%2==0) {
        real_sum += bincoeff * sign * img_exp * real_exp;
    }

    real = real_sum;
    img = img_sum;
}

} // namespace QuantLibExt
