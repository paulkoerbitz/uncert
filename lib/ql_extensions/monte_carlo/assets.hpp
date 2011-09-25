#ifndef ql_extensions__monte_carlo__assets_hpp__
#define ql_extensions__monte_carlo__assets_hpp__

#include "path.hpp"
#include "variates.hpp"

namespace QuantLibExt {

struct Assets {
    Assets() {}
    Assets(double SS, double rr, double iintR) : S(SS), r(rr), intR(iintR) {}
    double S, r, intR;

    Assets& operator+=(const Assets &other) {
        this->S += other.S;
        this->r += other.r;
        this->intR += other.intR;
        return *this;
    }
    Assets operator+(const Assets &other) const {
        Assets temp
            (this->S+other.S,this->r+other.r,this->intR+other.intR);
        return temp;
    }
    Assets operator-() const {
        Assets temp(-this->S,-this->r,-this->intR);
        return temp;
    }
};

struct AssetPathTraits {
    rational dt, T, t0;
    Assets initialAssetValues;
};


typedef Path<Assets>::iterator AssetPathIter;
typedef Path<Assets>::const_iterator ConstAssetPathIter;

Path<Assets> makePathFromVariates(
       const Path<Variates> &vari, 
       const Assets &startVals,
       const boost::function<Assets (const Variates&,
                               const Assets&,double) > &dynamics) {

    Path<Assets> path(vari.dt(),vari.T(),vari.t0());
    *(path.begin()) = startVals;
    Path<Assets>::iterator ia=path.begin()+1; 
    for (Path<Variates>::const_iterator iv=vari.begin()+1; 
                iv != vari.end(); ++iv, ++ia) {
        *ia = dynamics(*iv, *(ia-1) 
                      ,boost::rational_cast<double>(vari.dt()));
    }

    return path;
}

double stock(const rational& t, const Path<Assets>& assetPath) {
    return assetPath[t].S;
}

double stockFromPathWithOffset(const rational& t 
        ,const Path<Assets>& assetPath ,const Assets& originalValue) 
{
    return assetPath[t].S;
}

template <class PathIterator, class VariateIterator
         ,class PathValue, class VariateValue>
void updatePathFromVariates(PathIterator start, PathIterator end
        ,VariateIterator variateStart 
        ,const boost::function<PathValue 
                (const VariateValue&, const PathValue& , double)> &f) 
{
    for ( ++start, ++variateStart ; start != end; ++start, ++variateStart) {
        *start = f(*variateStart,*(start-1)
                    ,boost::rational_cast<double>(variateStart.dt()));
    }
}

template <class T>
T discountValue(rational t
               ,const Path<Assets>& assetPath
               ,const Path<T>& payoffPath) {
    T value_at_t;
    double sumIntR = 0.0;
    Path<Assets>::const_iterator ia = assetPath.iteratorAtTime(t);
    if (payoffPath.hasTimepointAt(t)) 
        value_at_t = payoffPath[t];

    for (typename Path<T>::const_iterator ip = payoffPath.firstIteratorAfterTime(t)
            ; ip != payoffPath.end(); ++ip) {
        while (ia.t() < ip.t()) {
            ++ia;
            sumIntR += ia->intR;
        }
        value_at_t += (*ip) * std::exp(-sumIntR);
    }
    return value_at_t;
}

} // namespace QuantLibExt
#endif
