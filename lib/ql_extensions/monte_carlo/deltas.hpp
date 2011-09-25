#ifndef ql_extensions__monte_carlo__deltas__hpp__
#define ql_extensions__monte_carlo__deltas__hpp__

#include <boost/function.hpp>

#include "../instruments/termfixinsurance/valuevector.hpp"

#include "path.hpp"
#include "assets.hpp"
#include "variates.hpp"
#include "dynamics.hpp"

namespace QuantLibExt {

void updatePathFromVariatesWithOffset(const rational& t
                                     ,Path<Assets>&  assetPath
                                     ,const Path<Variates>& variates
                                     ,const ModelDynamics& dynamics
                                     ,const Assets& offset) {
    assetPath[t] += offset;
    updatePathFromVariates(assetPath.iteratorAtTime(t),assetPath.end()
                          ,variates.iteratorAtTime(t),dynamics);
}

typedef boost::function<ValueVector 
            (const rational&, Path<Assets>& ,const Assets&)> ValueVecFromPathFunc;
typedef boost::function<double 
            (const rational&, Path<Assets>& ,const Assets&)> DoubleFromPathFunc;

ValueVector
computeFDDeltaFromVariates(const rational& t
                          ,Path<Assets>& assetPath
                          ,const Path<Variates>& variates
                          ,const Assets &offset
                          ,const ModelDynamics& dynamics
                          ,const ValueVecFromPathFunc& numeratorEval
                          ,const DoubleFromPathFunc& denominatorEval) {
    Assets origPathValue = assetPath[t];
    updatePathFromVariatesWithOffset(t,assetPath,variates,dynamics,offset);
    ValueVector num1 = numeratorEval(t,assetPath,origPathValue);
    double denom1 = denominatorEval(t,assetPath,origPathValue);
   
    assetPath[t] = origPathValue;
    updatePathFromVariatesWithOffset(t,assetPath,variates,dynamics,-offset);
    ValueVector num2 = numeratorEval(t,assetPath,origPathValue);
    double denom2 = denominatorEval(t,assetPath,origPathValue);

    assetPath[t] = origPathValue;
    return (num1-num2) / (denom1-denom2);
}

Array<ValueVector>
computeStockBondFDDeltaFromVariates
                       (const rational& t 
                       ,Path<Assets>& assetPath
                       ,const Path<Variates>& variates 
                       ,const ModelDynamics& dynamics
                       ,const ValueVecFromPathFunc& contractEval
                       ,const DoubleFromPathFunc& stock
                       ,const DoubleFromPathFunc& discountBond
                       ,const std::pair<Assets,Assets> &offsets) {
    Array<ValueVector> deltas(2);
    deltas[0] = computeFDDeltaFromVariates(t,assetPath,variates,offsets.first
                                          ,dynamics,contractEval,stock);
    deltas[1] = computeFDDeltaFromVariates(t,assetPath,variates,offsets.second
                                          ,dynamics,contractEval,discountBond); 
    return deltas;
}

}

#endif
