#ifndef ql_extensions__monte_carlo__function_types_hpp__
#define ql_extensions__monte_carlo__function_types_hpp__

namespace QuantLibExt {

/*
 * Function interfaces
 */

template <class ReturnType>
struct NiladicFunction {
    virtual ReturnType operator()() const = 0;
};

template <class ReturnType, class ArgType>
struct MonadicFunction {
    virtual ReturnType operator()(ArgType) const = 0;
};

template <class ReturnType, class ArgType1, class ArgType2>
struct DyadicFunction {
    virtual ReturnType operator()(ArgType1,
                                  ArgType2) const = 0;
};

template <class ReturnType, class ArgType1, class ArgType2, class ArgType3>
struct TriadicFunction {
    virtual ReturnType operator()(ArgType1,
                                  ArgType2,
                                  ArgType3) const = 0;
};

};
#endif
