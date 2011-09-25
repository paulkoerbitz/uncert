// Generic 1-D array for elementwise math, I stole and adapted this from QuantLib

#ifndef ql_extensions__math__array_hpp__
#define ql_extensions__math__array_hpp__

#include <ql/types.hpp>
#include <ql/errors.hpp>
#include <ql/utilities/disposable.hpp>
#include <ql/utilities/null.hpp>
#include <boost/iterator/reverse_iterator.hpp>
#include <boost/scoped_array.hpp>
#include <boost/type_traits.hpp>
#include <functional>
#include <numeric>
#include <vector>
#include <iomanip>

namespace QuantLibExt {

template <class T=double>
class Array {
  public:
    //! \name Constructors, destructor, and assignment
    //@{
    //! creates the array with the given dimension
    explicit Array(ql::Size size = 0);
    //! creates the array and fills it with <tt>value</tt>
    Array(ql::Size size, T value);
    /*! \brief creates the array and fills it according to
        \f$ a_{0} = value, a_{i}=a_{i-1}+increment \f$
    */
    Array(ql::Size size, T value, T increment);
    Array(const Array<T> &);
    Array(const ql::Disposable<Array<T> >&);
    //! creates the array from an iterable sequence
    //template <class ForwardIterator>
    //Array(ForwardIterator begin, ForwardIterator end);

    Array<T>& operator=(const Array<T>&);
    Array<T>& operator=(const ql::Disposable<Array<T> >&);
    bool operator==(const Array<T>&) const;
    bool operator!=(const Array<T>&) const;
    //@}
    /*! \name Vector algebra

        <tt>v += x</tt> and similar operation involving a scalar value
        are shortcuts for \f$ \forall i : v_i = v_i + x \f$

        <tt>v *= w</tt> and similar operation involving two vectors are
        shortcuts for \f$ \forall i : v_i = v_i \times w_i \f$

        \pre all arrays involved in an algebraic expression must have
        the same size.
    */
    //@{
    const Array<T>& operator+=(const Array<T>&);
    const Array<T>& operator+=(T);
    const Array<T>& operator-=(const Array<T>&);
    const Array<T>& operator-=(T);
    const Array<T>& operator*=(const Array<T>&);
    const Array<T>& operator*=(T);
    const Array<T>& operator/=(const Array<T>&);
    const Array<T>& operator/=(T);
    //@}
    //! \name Element access
    //@{
    //! read-only
    T operator[](ql::Size) const;
    T at(ql::Size) const;
    T front() const;
    T back() const;
    //! read-write
    T& operator[](ql::Size);
    T& at(ql::Size);
    T& front();
    T& back();
    //@}
    //! \name Inspectors
    //@{
    //! dimension of the array
    ql::Size size() const;
    //! whether the array is empty
    bool empty() const;
    //@}
    typedef ql::Size size_type;
    typedef T value_type;
    typedef T* iterator;
    typedef const T* const_iterator;
    typedef boost::reverse_iterator<iterator> reverse_iterator;
    typedef boost::reverse_iterator<const_iterator> const_reverse_iterator;
    //! \name Iterator access
    //@{
    const_iterator begin() const;
    iterator begin();
    const_iterator end() const;
    iterator end();
    const_reverse_iterator rbegin() const;
    reverse_iterator rbegin();
    const_reverse_iterator rend() const;
    reverse_iterator rend();
    //@}
    //! \name Utilities
    //@{
    void swap(Array&);  // never throws
    //@}

  private:
    boost::scoped_array<T> data_;
    ql::Size n_;
};

//! specialization of null template for this class
// template <>
// class Null<Array> {
//   public:
//     Null() {}
//     operator Array() const { return Array(); }
// };



/*! \relates Array */
template<class T1, class T2>
T1 DotProduct(const Array<T1>&, const Array<T2>&);

// unary operators
/*! \relates Array */
template <class T>
const ql::Disposable<Array<T> > operator+(const Array<T>& v);
/*! \relates Array */
template <class T>
const ql::Disposable<Array<T> > operator-(const Array<T>& v);

// binary operators
/*! \relates Array */
template <class T>
const ql::Disposable<Array<T> > operator+(const Array<T>&, const Array<T>&);
/*! \relates Array */
template <class T>
const ql::Disposable<Array<T> > operator+(const Array<T>&, T);
/*! \relates Array */
template <class T>
const ql::Disposable<Array<T> > operator+(T, const Array<T>&);
/*! \relates Array */
template <class T>
const ql::Disposable<Array<T> > operator-(const Array<T>&, const Array<T>&);
/*! \relates Array */
template <class T>
const ql::Disposable<Array<T> > operator-(const Array<T>&, T);
/*! \relates Array */
template <class T>
const ql::Disposable<Array<T> > operator-(T, const Array<T>&);
/*! \relates Array */
template <class T>
const ql::Disposable<Array<T> > operator*(const Array<T>&, const Array<T>&);
/*! \relates Array */
template <class T>
const ql::Disposable<Array<T> > operator*(const Array<T>&, T);
/*! \relates Array */
template <class T>
const ql::Disposable<Array<T> > operator*(T, const Array<T>&);
/*! \relates Array */
template <class T>
const ql::Disposable<Array<T> > operator/(const Array<T>&, const Array<T>&);
/*! \relates Array */
template <class T>
const ql::Disposable<Array<T> > operator/(const Array<T>&, T);
/*! \relates Array */
// template <class T>
// const ql::Disposable<Array<T> > operator/(const Array<T>&, double);
/*! \relates Array */
template <class T>
const ql::Disposable<Array<T> > operator/(T, const Array<T>&);

// math functions
/*! \relates Array */
template <class T>
const ql::Disposable<Array<T> > Abs(const Array<T>&);
/*! \relates Array */
template <class T>
const ql::Disposable<Array<T> > Sqrt(const Array<T>&);
/*! \relates Array */
template <class T>
const ql::Disposable<Array<T> > Log(const Array<T>&);
/*! \relates Array */
template <class T>
const ql::Disposable<Array<T> > Exp(const Array<T>&);

// utilities
/*! \relates Array */
template <class T>
void swap(Array<T> &, Array<T> &);

// format
/*! \relates Array */
template <class T>
std::ostream& operator<<(std::ostream&, const Array<T>&);


// inline definitions
template <class T>
inline Array<T>::Array(ql::Size size)
: data_(size ? new T[size] : (T*)(0)), n_(size) {}

template <class T>
inline Array<T>::Array(ql::Size size, T value)
: data_(size ? new T[size] : (T*)(0)), n_(size) {
    std::fill(begin(),end(),value);
}

template <class T>
inline Array<T>::Array(ql::Size size, T value, T increment)
: data_(size ? new T[size] : (T*)(0)), n_(size) {
    for (iterator i=begin(); i!=end(); i++,value+=increment)
        *i = value;
}

template <class T>
inline Array<T>::Array(const Array& from)
: data_(from.n_ ? new T[from.n_] : (T*)(0)), n_(from.n_) {
    #if defined(QL_PATCH_MSVC) && defined(QL_DEBUG)
    if (n_)
    #endif
    std::copy(from.begin(),from.end(),begin());
}

template <class T>
inline Array<T>::Array(const ql::Disposable<Array>& from)
: data_((T*)(0)), n_(0) {
    swap(const_cast<ql::Disposable<Array>&>(from));
}

// namespace detail {
// 
//     template <class T, class I>
//     inline void _fill_array_(Array<T>& a,
//                              boost::scoped_array<T>& data_,
//                              ql::Size& n_,
//                              I begin, I end,
//                              const boost::true_type&) {
//         // we got redirected here from a call like Array(3, 4)
//         // because it matched the constructor below exactly with
//         // ForwardIterator = int.  What we wanted was fill an
//         // Array with a given value, which we do here.
//         ql::Size n = begin;
//         T value = end;
//         data_.reset(n ? new T[n] : (T*)(0));
//         n_ = n;
//         std::fill(a.begin(),a.end(),value);
//     }
// 
//     template <class T, class I>
//     inline void _fill_array_(Array<T>& a,
//                              boost::scoped_array<T>& data_,
//                              ql::Size& n_,
//                              I begin, I end,
//                              const boost::false_type&) {
//         // true iterators
//         ql::Size n = std::distance(begin, end);
//         data_.reset(n ? new T[n] : (T*)(0));
//         n_ = n;
//         #if defined(QL_PATCH_MSVC) && defined(QL_DEBUG)
//         if (n_)
//         #endif
//         std::copy(begin, end, a.begin());
//     }
// 
// }
// 
// template <class T, class ForwardIterator>
// inline Array<T>::Array(ForwardIterator begin, ForwardIterator end) {
//     // Unfortunately, calls such as Array(3, 4) match this constructor.
//     // We have to detect integral types and dispatch.
//     detail::_fill_array_(*this, data_, n_, begin, end,
//                          boost::is_integral<ForwardIterator>());
// }

template <class T>
inline Array<T>& Array<T>::operator=(const Array<T>& from) {
    // strong guarantee
    Array<T> temp(from);
    swap(temp);
    return *this;
}

template <class T>
inline bool Array<T>::operator==(const Array<T>& to) const {
    return (n_ == to.n_) && std::equal(begin(), end(), to.begin());
}

template <class T>
inline bool Array<T>::operator!=(const Array<T>& to) const {
    return !(this->operator==(to));
}

template <class T>
inline Array<T>& Array<T>::operator=(const ql::Disposable<Array<T> >& from) {
    swap(const_cast<ql::Disposable<Array<T> >&>(from));
    return *this;
}

template <class T>
inline const Array<T>& Array<T>::operator+=(const Array<T>& v) {
    QL_REQUIRE(n_ == v.n_,
               "arrays with different sizes (" << n_ << ", "
               << v.n_ << ") cannot be added");
    std::transform(begin(),end(),v.begin(),begin(),
                   std::plus<T>());
    return *this;
}


template <class T>
inline const Array<T>& Array<T>::operator+=(T x) {
    std::transform(begin(),end(),begin(),
                   std::bind2nd(std::plus<T>(),x));
    return *this;
}

template <class T>
inline const Array<T> & Array<T> ::operator-=(const Array<T> & v) {
    QL_REQUIRE(n_ == v.n_,
               "arrays with different sizes (" << n_ << ", "
               << v.n_ << ") cannot be subtracted");
    std::transform(begin(),end(),v.begin(),begin(),
                   std::minus<T>());
    return *this;
}

template <class T>
inline const Array<T> & Array<T> ::operator-=(T x) {
    std::transform(begin(),end(),begin(),
                   std::bind2nd(std::minus<T>(),x));
    return *this;
}

template <class T>
inline const Array<T> & Array<T> ::operator*=(const Array<T> & v) {
    QL_REQUIRE(n_ == v.n_,
               "arrays with different sizes (" << n_ << ", "
               << v.n_ << ") cannot be multiplied");
    std::transform(begin(),end(),v.begin(),begin(),
                   std::multiplies<T>());
    return *this;
}

template <class T>
inline const Array<T>& Array<T>::operator*=(T x) {
    std::transform(begin(),end(),begin(),
                   std::bind2nd(std::multiplies<T>(),x));
    return *this;
}

template <class T>
inline const Array<T>& Array<T>::operator/=(const Array<T>& v) {
    QL_REQUIRE(n_ == v.n_,
               "arrays with different sizes (" << n_ << ", "
               << v.n_ << ") cannot be divided");
    std::transform(begin(),end(),v.begin(),begin(),
                   std::divides<T>());
    return *this;
}

template <class T>
inline const Array<T>& Array<T>::operator/=(T x) {
    std::transform(begin(),end(),begin(),
                   std::bind2nd(std::divides<T>(),x));
    return *this;
}

template <class T>
inline T Array<T>::operator[](ql::Size i) const {
    #if defined(QL_EXTRA_SAFETY_CHECKS)
    QL_REQUIRE(i<n_,
               "index (" << i << ") must be less than " << n_ <<
               ": array access out of range");
    #endif
    return data_.get()[i];
}

template <class T>
inline T Array<T>::at(ql::Size i) const {
    QL_REQUIRE(i<n_,
               "index (" << i << ") must be less than " << n_ <<
               ": array access out of range");
    return data_.get()[i];
}

template <class T>
inline T Array<T>::front() const {
    #if defined(QL_EXTRA_SAFETY_CHECKS)
    QL_REQUIRE(n_>0, "null Array<T>: array access out of range");
    #endif
    return data_.get()[0];
}

template <class T>
inline T Array<T>::back() const {
    #if defined(QL_EXTRA_SAFETY_CHECKS)
    QL_REQUIRE(n_>0, "null Array<T>: array access out of range");
    #endif
    return data_.get()[n_-1];
}

template <class T>
inline T& Array<T>::operator[](ql::Size i) {
    #if defined(QL_EXTRA_SAFETY_CHECKS)
    QL_REQUIRE(i<n_,
               "index (" << i << ") must be less than " << n_ <<
               ": array access out of range");
    #endif
    return data_.get()[i];
}

template <class T>
inline T& Array<T>::at(ql::Size i) {
    QL_REQUIRE(i<n_,
               "index (" << i << ") must be less than " << n_ <<
               ": array access out of range");
    return data_.get()[i];
}

template <class T>
inline T& Array<T>::front() {
    #if defined(QL_EXTRA_SAFETY_CHECKS)
    QL_REQUIRE(n_>0, "null Array<T>: array access out of range");
    #endif
    return data_.get()[0];
}

template <class T>
inline T& Array<T>::back() {
    #if defined(QL_EXTRA_SAFETY_CHECKS)
    QL_REQUIRE(n_>0, "null Array<T>: array access out of range");
    #endif
    return data_.get()[n_-1];
}

template <class T>
inline ql::Size Array<T>::size() const {
    return n_;
}

template <class T>
inline bool Array<T>::empty() const {
    return n_ == 0;
}

template <class T>
inline typename Array<T>::const_iterator Array<T>::begin() const {
    return data_.get();
}

template <class T>
inline typename Array<T>::iterator Array<T>::begin() {
    return data_.get();
}

template <class T>
inline typename Array<T>::const_iterator Array<T>::end() const {
    return data_.get()+n_;
}

template <class T>
inline typename Array<T>::iterator Array<T>::end() {
    return data_.get()+n_;
}

template <class T>
inline typename Array<T>::const_reverse_iterator Array<T>::rbegin() const {
    return const_reverse_iterator(end());
}

template <class T>
inline typename Array<T>::reverse_iterator Array<T>::rbegin() {
    return reverse_iterator(end());
}

template <class T>
inline typename Array<T>::const_reverse_iterator Array<T>::rend() const {
    return const_reverse_iterator(begin());
}

template <class T>
inline typename Array<T>::reverse_iterator Array<T>::rend() {
    return reverse_iterator(begin());
}

template <class T>
inline void Array<T>::swap(Array<T>& from) {
    using std::swap;
    data_.swap(from.data_);
    swap(n_,from.n_);
}

// dot product

template <class ValT1, class ValT2>
inline ValT1 DotProduct(const Array<ValT1>& v1, const Array<ValT2>& v2
                       ,const ValT1 init) {
    QL_REQUIRE(v1.size() == v2.size(),
               "arrays with different sizes (" << v1.size() << ", "
               << v2.size() << ") cannot be multiplied");
    return std::inner_product(v1.begin(),v1.end(),v2.begin(),init);
}

// overloaded operators

// unary

template <class T>
inline const ql::Disposable<Array<T> > operator+(const Array<T>& v) {
    Array<T> result = v;
    return result;
}

template <class T>
inline const ql::Disposable<Array<T> > operator-(const Array<T>& v) {
    Array<T> result(v.size());
    std::transform(v.begin(),v.end(),result.begin(),
                   std::negate<T>());
    return result;
}


// binary operators

template <class T>
inline const ql::Disposable<Array<T> > operator+(const Array<T>& v1,
                                         const Array<T>& v2) {
    QL_REQUIRE(v1.size() == v2.size(),
               "arrays with different sizes (" << v1.size() << ", "
               << v2.size() << ") cannot be added");
    Array<T> result(v1.size());
    std::transform(v1.begin(),v1.end(),v2.begin(),result.begin(),
                   std::plus<T>());
    return result;
}

template <class T>
inline const ql::Disposable<Array<T> > operator+(const Array<T>& v1, T a) {
    Array<T> result(v1.size());
    std::transform(v1.begin(),v1.end(),result.begin(),
                   std::bind2nd(std::plus<T>(),a));
    return result;
}

template <class T>
inline const ql::Disposable<Array<T> > operator+(T a, const Array<T>& v2) {
    Array<T> result(v2.size());
    std::transform(v2.begin(),v2.end(),result.begin(),
                   std::bind1st(std::plus<T>(),a));
    return result;
}

template <class T>
inline const ql::Disposable<Array<T> > operator-(const Array<T>& v1,
                                         const Array<T>& v2) {
    QL_REQUIRE(v1.size() == v2.size(),
               "arrays with different sizes (" << v1.size() << ", "
               << v2.size() << ") cannot be subtracted");
    Array<T> result(v1.size());
    std::transform(v1.begin(),v1.end(),v2.begin(),result.begin(),
                   std::minus<T>());
    return result;
}

template <class T>
inline const ql::Disposable<Array<T> > operator-(const Array<T>& v1, T a) {
    Array<T> result(v1.size());
    std::transform(v1.begin(),v1.end(),result.begin(),
                   std::bind2nd(std::minus<T>(),a));
    return result;
}

template <class T>
inline const ql::Disposable<Array<T> > operator-(T a, const Array<T>& v2) {
    Array<T> result(v2.size());
    std::transform(v2.begin(),v2.end(),result.begin(),
                   std::bind1st(std::minus<T>(),a));
    return result;
}

template <class T>
inline const ql::Disposable<Array<T> > operator*(const Array<T>& v1,
                                         const Array<T>& v2) {
    QL_REQUIRE(v1.size() == v2.size(),
               "arrays with different sizes (" << v1.size() << ", "
               << v2.size() << ") cannot be multiplied");
    Array<T> result(v1.size());
    std::transform(v1.begin(),v1.end(),v2.begin(),result.begin(),
                   std::multiplies<T>());
    return result;
}

template <class T>
inline const ql::Disposable<Array<T> > operator*(const Array<T>& v1, T a) {
    Array<T> result(v1.size());
    std::transform(v1.begin(),v1.end(),result.begin(),
                   std::bind2nd(std::multiplies<T>(),a));
    return result;
}

template <class T>
inline const ql::Disposable<Array<T> > operator*(T a, const Array<T>& v2) {
    Array<T> result(v2.size());
    std::transform(v2.begin(),v2.end(),result.begin(),
                   std::bind1st(std::multiplies<T>(),a));
    return result;
}

template <class T>
inline const ql::Disposable<Array<T> > operator/(const Array<T>& v1,
                                                 const Array<T>& v2) {
    QL_REQUIRE(v1.size() == v2.size(),
               "arrays with different sizes (" << v1.size() << ", "
               << v2.size() << ") cannot be divided");
    Array<T> result(v1.size());
    std::transform(v1.begin(),v1.end(),v2.begin(),result.begin(),
                   std::divides<T>());
    return result;
}

template <class T>
inline const ql::Disposable<Array<T> > operator/(const Array<T>& v1, T a) {
    Array<T> result(v1.size());
    std::transform(v1.begin(),v1.end(),result.begin(),
                   std::bind2nd(std::divides<T>(),a));
    return result;
}

// template <class T>
// inline const ql::Disposable<Array<T> > operator/(const Array<T>& v1, double a) {
//     Array<T> result(v1.size());
//     std::transform(v1.begin(),v1.end(),result.begin(),
//                    std::bind2nd(std::binary_function<T,T,double>(),a));
//     return result;
// }

template <class T>
inline const ql::Disposable<Array<T> > operator/(T a, const Array<T>& v2) {
    Array<T> result(v2.size());
    std::transform(v2.begin(),v2.end(),result.begin(),
                   std::bind1st(std::divides<T>(),a));
    return result;
}


// functions

template <class T>
inline const ql::Disposable<Array<T> > Abs(const Array<T>& v) {
    Array<T> result(v.size());
    std::transform(v.begin(),v.end(),result.begin(),
                   std::ptr_fun<T,T>(std::fabs));
    return result;
}

template <class T>
inline const ql::Disposable<Array<T> > Sqrt(const Array<T>& v) {
    Array<T> result(v.size());
    std::transform(v.begin(),v.end(),result.begin(),
                   std::ptr_fun<T,T>(std::sqrt));
    return result;
}

template <class T>
inline const ql::Disposable<Array<T> > Log(const Array<T>& v) {
    Array<T> result(v.size());
    std::transform(v.begin(),v.end(),result.begin(),
                   std::ptr_fun<T,T>(std::log));
    return result;
}

template <class T>
inline const ql::Disposable<Array<T> > Exp(const Array<T>& v) {
    Array<T> result(v.size());
    std::transform(v.begin(),v.end(),result.begin(),
                   std::ptr_fun<T,T>(std::exp));
    return result;
}

template <class T>
inline void swap(Array<T>& v, Array<T>& w) {
    v.swap(w);
}

template <class T>
inline std::ostream& operator<<(std::ostream& out, const Array<T>& a) {
    std::streamsize width = out.width();
    out << "[ ";
    if (!a.empty()) {
        for (ql::Size n=0; n<a.size()-1; ++n)
            out << std::setw(width) << a[n] << "; ";
        out << std::setw(width) << a.back();
    }
    out << " ]";
    return out;
}

}
#endif
