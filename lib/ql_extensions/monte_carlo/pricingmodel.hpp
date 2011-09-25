#ifndef ql_extensions__monte_carlo__pricingmodel_hpp__
#define ql_extensions__monte_carlo__pricingmodel_hpp__ 

namespace QuantLibExt {

template <class ValT, class UnderlT, class DeltaT>
class PricingModel {
  public:
    virtual ValT    value() const = 0;
    virtual UnderlT underlyings() const = 0;
    virtual DeltaT  deltas() const = 0;
};

}


#endif
