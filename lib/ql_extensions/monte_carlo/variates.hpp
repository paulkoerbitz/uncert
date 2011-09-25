#ifndef ql_extension__monte_carlo__variates__hpp__
#define ql_extension__monte_carlo__variates__hpp__

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>

#include <boost/function.hpp>

#include "path.hpp"

namespace QuantLibExt {

struct Variates {
    double W1, W2;
};

typedef Path<Variates>::iterator VariatePathIter;
typedef Path<Variates>::const_iterator ConstVariatePathIter;

Path<Variates> makeVariates(const rational &dt ,const rational& T 
                           ,const rational &t
                           ,boost::function<double ()> &n) {
    Path<Variates> path(dt,T,t);
    for (Path<Variates>::iterator it=path.begin(); 
            it != path.end(); ++it) {
        it->W1 = n();
        it->W2 = n();
    }
    return path;
}

struct NormalRandomNumberGenerator {

    NormalRandomNumberGenerator(unsigned seed)
        : n_(boost::mt19937(seed),boost::normal_distribution<>(0.,1.)) {}
    double operator()() {
       return n_();
    } 

  private:
    boost::variate_generator<boost::mt19937,boost::normal_distribution<> > n_;
};

class ScenarioGenerator {
  public:
    ScenarioGenerator(const rational& dt, const rational& T 
                     ,const rational& t, unsigned seed=42u)
        : dt_(dt), T_(T), t_(t), n_(NormalRandomNumberGenerator(seed)) {}
    ScenarioGenerator(const rational& dt, const rational& T 
                     ,const rational& t, const boost::function<double ()> &n)
        : dt_(dt), T_(T), t_(t), n_(n) {}

    Path<Variates> operator()() {
        return makeVariates(dt_,T_,t_,n_);
    }

  protected:
    rational dt_, T_, t_;
    mutable boost::function<double ()> n_;
};

}

#endif
