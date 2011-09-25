#ifndef ql_extensions__instruments__termfixinsurance__valuevector_hpp__
#define ql_extensions__instruments__termfixinsurance__valuevector_hpp__

#include <ostream>
#include <iomanip>
#include <iostream>
#include <cmath>

namespace QuantLibExt {

struct ValueVector {
    double V, C, D, Res, Surr;

    ValueVector(double v=0.0, double c=0.0, double d=0.0, 
                double res=0.0, double surr=0.0) 
        : V(v), C(c), D(d), Res(res), Surr(surr)  {}

    ValueVector& operator=(double h) {
        this->V = h;
        this->C = h;
        this->D = h;
        this->Res = h;
        this->Surr = h;
        return *this;
    }
    ValueVector& operator=(const ValueVector& other) {
        if (this != &other) {
            this->V     = other.V;
            this->C     = other.C;
            this->D     = other.D;
            this->Res   = other.Res;
            this->Surr  = other.Surr;
        }
        return *this;
    }
    ValueVector operator+(const ValueVector& other) const {
        ValueVector temp;
        temp.V    = V    + other.V   ;
        temp.C    = C    + other.C   ;
        temp.D    = D    + other.D   ;
        temp.Res  = Res  + other.Res ;
        temp.Surr = Surr + other.Surr;
        return temp; 
    }
    ValueVector operator-(const ValueVector& other) const {
        ValueVector temp;
        temp.V    = V    - other.V   ;
        temp.C    = C    - other.C   ;
        temp.D    = D    - other.D   ;
        temp.Res  = Res  - other.Res ;
        temp.Surr = Surr - other.Surr;
        return temp; 
    }
    ValueVector operator/(double h) const {
        ValueVector temp;
        temp.V    = V    / h;
        temp.C    = C    / h;
        temp.D    = D    / h;
        temp.Res  = Res  / h;
        temp.Surr = Surr / h;
        return temp; 
    }    
    ValueVector operator*(double h) const {
        ValueVector temp;
        temp.V    = V    * h;
        temp.C    = C    * h;
        temp.D    = D    * h;
        temp.Res  = Res  * h;
        temp.Surr = Surr * h;
        return temp; 
    }    
    ValueVector operator*(const ValueVector &other) const {
        ValueVector temp;
        temp.V    = V    * other.V;
        temp.C    = C    * other.C;
        temp.D    = D    * other.D;
        temp.Res  = Res  * other.Res;
        temp.Surr = Surr * other.Surr;
        return temp; 
    }    
    ValueVector& operator/=(double h) {
        V /= h;
        C /= h;
        D /= h;
        Res /= h;
        Surr /= h;
        return *this;
    }
    ValueVector& operator*=(double h) {
        V *= h;
        C *= h;
        D *= h;
        Res *= h;
        Surr *= h;
        return *this;
    }    
    ValueVector& operator+=(const ValueVector& other) {
        V += other.V;
        C += other.C;
        D += other.D;
        Res += other.Res;
        Surr += other.Surr;
        return *this;
    }
    ValueVector& operator-=(const ValueVector& other) {
        V    -= other.V;
        C    -= other.C;
        D    -= other.D;
        Res  -= other.Res;
        Surr -= other.Surr;
        return *this;
    }
    std::string printHeader() {
        return std::string("European Contract, Interest Rate Guarantee,"
               " Value of Dividends, Value of Reserve, Surrender Options");
    }
    friend std::ostream& operator<<(std::ostream& out, const ValueVector& vv);
    friend ValueVector abs(const ValueVector& o);
};
 

std::ostream& operator<<(std::ostream& out, const ValueVector& vv) {
    out << vv.V << "," ;
    out << vv.C << "," ;
    out << vv.D << "," ;
    out << vv.Res << "," ;
    out << vv.Surr ;
    return out;
}


ValueVector abs(const ValueVector& o) {
    ValueVector temp;
    temp.V    = std::abs(o.V   );
    temp.C    = std::abs(o.C   );
    temp.D    = std::abs(o.D   );
    temp.Res  = std::abs(o.Res );
    temp.Surr = std::abs(o.Surr);
    return temp;
}

ValueVector sqrt(const ValueVector& o) {
    ValueVector temp;
    temp.V    = std::sqrt(o.V   );
    temp.C    = std::sqrt(o.C   );
    temp.D    = std::sqrt(o.D   );
    temp.Res  = std::sqrt(o.Res );
    temp.Surr = std::sqrt(o.Surr);
    return temp;
}


};

#endif
