#ifndef ql_extensions__monte_carlo__path_debug_hpp__
#define ql_extensions__monte_carlo__path_debug_hpp__ 

#include <iostream>

#include <vector>
#include <algorithm>

#include "../instruments/termfixinsurance/valuevector.hpp"

namespace QuantLibExt {

struct PathDebugInfo {
  PathDebugInfo(double tt, double SS, double rr, double iintR, double LL, double ZZCB
                 ,ValueVector ddS, ValueVector ddZCB
                 ,ValueVector mA, ValueVector cV)
    : t(tt), S(SS), r(rr), intR(iintR), L(LL), ZCB(ZZCB)
         ,dS(ddS), dZCB(ddZCB), moneyAccount(mA), contractValue(cV) {}
  double t, S, r, intR, L, ZCB;
  ValueVector dS, dZCB, moneyAccount, contractValue;
};

void printPDI(const PathDebugInfo& p) {
    std::cerr << p.t << ","
        << p.S << ","
        << p.r << ","
        << p.intR << ","
        << p.L << ","
        << p.ZCB << ","
        << p.contractValue << ","
        << p.moneyAccount << ","
        << p.dS << ","
        << p.dZCB << std::endl;
}

void printPDIV(const std::vector<PathDebugInfo>& p) {
    std::cerr << "time, S, r, intR, L, ZCB"
        << ", V, C, D, Res, Surr"
        << ", m:V, m:C, m:D, m:Res, m:Surr"
        << ", dV/dS, dC/dS, dD/dS, dRes/dS, dSurr/dS"
        << ", dV/dZCB, dC/dZCB, dD/dZCB, dRes/dZCB, dSurr/dZCB"
        << std::endl; 
    std::for_each(p.begin(), p.end(), &printPDI);
} 

}

#endif
