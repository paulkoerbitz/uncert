#ifndef ql_extensions__monte_carlo__path__hpp__
#define ql_extensions__monte_carlo__path__hpp__

#include <iostream>

#include <vector>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/rational.hpp>

#include <ql/quantlib.hpp>

namespace QuantLibExt {

typedef boost::rational<int> rational;


template<class Type>
class Path {
  public:
    template<class Value, class Parent=Path<Value> >
    class path_iter
        : public boost::iterator_facade<
              path_iter<Value,Parent>,
              Value,
              std::random_access_iterator_tag > {
      public:
        path_iter(Value *p, Parent* const p_parent)
            : p_node_(p), 
              p_parent_(p_parent) {}

        path_iter(Parent* const p_parent)
            : p_node_(0), 
              p_parent_(p_parent) {}

        rational t() const { 
            return (p_node_-&(*(p_parent_->begin())))*(p_parent_->dt()); 
        }
        rational dt() const {
            return p_parent_->dt(); 
        }

      private: 
        // Everything Boost's iterator facade needs
        friend class boost::iterator_core_access;
        Value& dereference() const { return *p_node_; }
        bool equal(path_iter<Value,Parent> const & other) const { 
            return p_node_ == other.p_node_; }
        void increment() { ++p_node_; }
        void decrement() { --p_node_; }
        void advance(int n) { p_node_ += n; }
        int distance_to(path_iter<Value,Parent> const & other) const { 
            return other.p_node_ - p_node_; }

        Value *p_node_;
        Parent *p_parent_;
    };
    typedef path_iter<Type> iterator;
    typedef path_iter<Type const, Path<Type> const> const_iterator;

    const_iterator begin() const { 
        return const_iterator(&(*pathPoints_.begin()),this); }
    const_iterator end()   const { 
        return const_iterator(&(*pathPoints_.end()),  this); }
    const_iterator iteratorAtTime(rational t) const {
        return const_iterator(&pathPoints_[indexAtTime_(t)],this); }
    const_iterator lastIteratorOnOrBeforeTime(rational t) const {
        return const_iterator(&pathPoints_[lastIndexOnOrBeforeTime_(t)],this); }
    const_iterator firstIteratorAfterTime(rational t) const {
        return const_iterator(&pathPoints_[firstIndexAfterTime_(t)],this); }

    iterator begin() { return iterator(&(*pathPoints_.begin()),this); }
    iterator end()   { return iterator(&(*pathPoints_.end()),  this); }
    iterator iteratorAtTime(rational t) {
        return iterator(&pathPoints_[indexAtTime_(t)],this); }
    iterator lastIteratorOnOrBeforeTime(rational t) {
        return iterator(&pathPoints_[lastIndexOnOrBeforeTime_(t)],this); }
    iterator firstIteratorAfterTime(rational t) {
        return iterator(&pathPoints_[firstIndexAfterTime_(t)],this); }

    Path(rational dt, rational T, rational t0=rational(0,1));

    unsigned size() const { return pathPoints_.size(); }
    rational t0() const { return t0_; }
    rational dt() const { return dt_; }
    rational T() const { return T_; }

    bool hasTimepointAt(const rational& t) const {
        return (t <= T_) && (t >= t0_) 
            && (((t - t0_)/dt_).denominator() == 1);
    }

    Type operator[](unsigned i) const {
        return pathPoints_[i]; }
    Type& operator[](unsigned i) {
        return pathPoints_[i]; }

    Type operator[](rational t) const {
        return pathPoints_[indexAtTime_(t)];
    }
    Type& operator[](rational t) {
        return pathPoints_[indexAtTime_(t)];
    }

  protected:
    unsigned indexAtTime_(const rational& t) const;
    unsigned lastIndexOnOrBeforeTime_(const rational& t) const;
    unsigned firstIndexAfterTime_(const rational& t) const;

    rational dt_, t0_, T_;
    std::vector<Type> pathPoints_;
};

template <class Type>
Path<Type>::Path(rational dt, rational T, rational t0)
: dt_(dt), t0_(t0), T_(T)
{
    QL_REQUIRE( ((T_ - t0_)/dt_).denominator() == 1 
            ,"Path: T - t0 not divisible by dt");
    pathPoints_ = std::vector<Type>( ((T_ - t0_) / dt_).numerator() + 1);
}

template <class Type>
inline unsigned Path<Type>::indexAtTime_(const rational& t) const {
    rational index = (t - t0_) / dt_;
    QL_REQUIRE(index.denominator() == 1
            ,"Path: No point on path at time t");
    QL_REQUIRE( (t >= t0_) && (t <= T_)
            ,"Path: Illegal t: t out of bounds");
    return index.numerator();
}

template <class Type>
inline unsigned Path<Type>::lastIndexOnOrBeforeTime_(const rational& t) const {
    QL_REQUIRE((t >= t0_)
        ,"Path: Illegal t: t < t0 when querying index / iterator before or on t");
    rational index = (t-t0_) / dt_;
    return std::min(boost::rational_cast<unsigned>( (t-t0_) / dt_)
                         ,unsigned(pathPoints_.size()-1));
}

template <class Type>
inline unsigned Path<Type>::firstIndexAfterTime_(const rational& t) const {
    QL_REQUIRE( (t < T_)
        ,"Path: Illegal t: t >= T when querying index / iterator after t");
    return unsigned(std::max(
            int(std::floor(boost::rational_cast<double>((t-t0_)/dt_))+1),0));
}

}
#endif
