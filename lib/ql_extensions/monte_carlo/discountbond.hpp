#ifndef ql_extensions__monte_carlo__discountbond_hpp__
#define ql_extensions__monte_carlo__discountbond_hpp__

#include "path.hpp"
#include "assets.hpp"
#include "variates.hpp"

namespace QuantLibExt {

double discountBond1(ConstAssetPathIter start, ConstAssetPathIter end) {
    double sumIntR = 0;
    for ( ++start; start != end; ++start) {
        sumIntR += start->intR;
    }
    return std::exp(-sumIntR);
}

double discountBond(const rational& t, const Path<Assets> &path) {
    return discountBond1(path.iteratorAtTime(t),path.end());
}

double discountBondFromPathWithOffset(
        const rational& t, const Path<Assets> &path, const Assets&) 
{
    return discountBond1(path.iteratorAtTime(t),path.end());
}

Array<>
underlyingsFromVariates(const rational& t
                       ,Path<Assets>& assetPath
                       ,const Path<Variates>& variates 
                       ,const ModelDynamics& dynamics) {
    updatePathFromVariates(assetPath.iteratorAtTime(t),assetPath.end()
                          ,variates.iteratorAtTime(t),dynamics);
    Array<> underlyings(2);
    underlyings[0] = assetPath[t].S;
    underlyings[1] = discountBond(t,assetPath);
    return underlyings;
}

}

#endif
