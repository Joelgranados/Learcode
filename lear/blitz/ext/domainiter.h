// {{{ file documentation
/**
 * @file 
 * @author Navneet Dalal
 * @brief Defines DomainIterator for Rectangular Domain.
 *
 * @warning This class has nothing to do with RectDomain.
 * It just provides iterator functionality on a Rectangular Domain.
 */
// }}}

#ifndef _LEAR_DOMAIN_ITER_H_
#define _LEAR_DOMAIN_ITER_H_

// {{{ headers
#include <blitz/tinyvec.h>
#include <blitz/tinyvec-et.h>

#include <lear/blitz/ext/tvmutil.h>
// }}}

BZ_NAMESPACE(blitz)
// // {{{
// template<class T_leaftype>
// class DomainIter {
// 
// public:
//     typedef T_leaftype LeafType;
//     typedef typename LeafType::IndexType IndexType;
// 
//     typedef std::forward_iterator_tag iterator_category;
//     typedef IndexType            value_type;
//     typedef IndexType            difference_type;
//     typedef IndexType*           pointer;
//     typedef IndexType&           reference;
// 
//     const IndexType& restrict operator*() const { return *leaf(); }
// 
//     const IndexType& restrict operator->() const { return *leaf(); }
// 
//     DomainIter& operator++(){ return static_cast<DomainIter&>(++leaf());}
// 
//     /**
//      * We follow the same principle of blitz::Array itertor ++ operator. Post-increment returns void.
//      */
//     void operator++(int) { ++(*this); }
// 
//     template<class T>
//     bool operator==(const T& x) const
//     { return leaf() == x; }
// 
//     template<class T>
//     bool operator!=(const T& x) const
//     { return leaf() != x; }
//  
// protected:
//     LeafType& leaf() {
//         return static_cast<LeafType&> (*this);
//     }
// };
// // }}}

template<int N>
class DomainIter;
/**
 * Specialize DomainIter. When N ==0, this indicates to
 * loop till end of the bounds in the DomainIter. This will
 * be much more efficient than comparing two bounds.
 */
template<>
class DomainIter<0> {// {{{
    public:
    /**
     * This function is provided for efficiency reasons.
     * To iterate till ubound in for loops, it is more 
     * efficient to use DomainIter<0> as end iterator.
     */
    template<int N>
    inline bool operator==(const DomainIter<N>& x) const
    { return !static_cast<bool>(x); }

    /**
     * This function is provided for efficiency reasons.
     * To iterate till ubound in for loops, it is more 
     * efficient to use DomainIter<0> as end iterator.
     */
    template<int N>
    inline bool operator!=(const DomainIter<N>& x) const
    { return static_cast<bool>(x); }

    // no accidental loops
    inline bool operator==(const DomainIter<0>& ) const
    { return false; }
    // no accidental loops
    inline bool operator!=(const DomainIter<0>& ) const
    { return false; }
};// }}}

    /**
     * Create a DomainIter. 
     *
     * Stride here has unusual meaning. In blitz array class, strides are used to 
     * increment data pointer but not indices (see iter.h file in blitz repository).
     * Here we use stride to increment indices. With this, user can create an 
     * iterator which provides access to (for example) every odd element.
     *
     * \warning Currently, DomainIter works only with ascending order indices.
     * Also, use of blitz iterators or DomainIter is always a bit slower when
     * compared to explicit for loops. However, explicit for loops over each 
     * dimension are complicated and error prone. So if efficiency is not the
     * prime goal, use DomainIter class. Also, if one is writing template classes 
     * using array dimension as template argument, explicit for loops does not work. 
     * In these situations, one is forced to use DomainIter.
     */
    /*
     * Though it is not difficult to extend it to descending indices, but
     * I am unable to think how can we do it without putting any
     * penalty on ascendingOrder case.
     */
template<int N>
class DomainIter {// {{{

public:
    enum {Rank = N};
    typedef TinyVector<int,N> IndexType;

    DomainIter(
            const IndexType& lbound_, const IndexType& ubound_,
            const IndexType& strides_, const IndexType& order_):
        lbound_(lbound_), ubound_(ubound_), 
        strides_(strides_), order_(order_)
    { init();}

    /**
     * Create a DomainIter. By default use unit stride and C-style ordering.
     */
    DomainIter(
            const IndexType& lbound_, const IndexType& ubound_, 
            const IndexType& strides_=1): 
        lbound_(lbound_), ubound_(ubound_), strides_(strides_)
    { 
        // set the ordering first
        for (int i= 0; i<N ; ++i) 
            order_[i] = N - i -1;
        init();
    }

    /**
     * Return current index as TinyVector
     */
    const IndexType& restrict operator*() const { return pos_; }

    /**
     * Returns current index as TinyVector
     */
    const IndexType& restrict operator->() const { return pos_; }

    DomainIter<N>& operator++();

    void operator++(int)
    { ++(*this); }
    // {{{ DomainIter comparison
    /**
     * Compare two iterators based on their current indices.
     * Returns true if and only if all two indices are equal for all N elements
     */
    bool operator==(const DomainIter<N>& x) const
    { return isEqual(pos_, x.pos_); }

    /**
     * Compare two iterators based on their current indices.
     * Returns true if and only if all two indices are less than for all N elements
     */
    bool operator< (const DomainIter<N>& x) const
    { return isLess(pos_, x.pos_); }

    /**
     * Compare two iterators based on their current indices.
     * Returns true if and only if all two indices are less than or equal to for all N elements
     */
    bool operator<=(const DomainIter<N>& x) const
    { return isLessEqual(pos_, x.pos_); }

    /**
     * Compare two iterators based on their current indices.
     * Returns true if and only if all two indices are greater than for all N elements
     */
    bool operator> (const DomainIter<N>& x) const
    { return isGreater(pos_, x.pos_); }

    /**
     * Compare two iterators based on their current indices.
     * Returns true if and only if all two indices are greater than or equal to for all N elements
     */
    bool operator>= (const DomainIter<N>& x) const
    { return isGreaterEqual(pos_, x.pos_); }

    /**
     * Compare two iterators based on their current indices.
     * Returns false if and only if all two indices are equal for all N elements
     */
    bool operator!=(const DomainIter<N>& x) const
    { return isNotEqual(pos_, x.pos_); }
    // }}}
    // {{{ DomainIter<0> comparison
    /**
     * This function is provided for efficiency reasons.
     * To iterate till ubound in for loops, it is more 
     * efficient to use DomainIter<0> as end iterator.
     */
    inline bool operator==(const DomainIter<0>& ) const
    { return !static_cast<bool>(*this); }

    /**
     * This function is provided for efficiency reasons.
     * To iterate till ubound in for loops, it is more 
     * efficient to use DomainIter<0> as end iterator.
     */
    inline bool operator!=(const DomainIter<0>& ) const
    { return static_cast<bool>(*this); }
    // }}}
    // {{{ IndexType comparison
    /**
     * Returns true if and only if all elements of current iterator index and argument index satisfies the condition.
     * These functions are provided as an extension. Though not standard for iterators.
     *
     * \warning It is not recommended to use these functions in for/while loops for efficiency reasons. Use the bool() operator or end iterator.
     */
    // @{ 
    bool operator==(const IndexType& x) const
    { return isEqual(pos_, x); }
    bool operator< (const IndexType& x) const
    { return isLess(pos_, x); }
    bool operator<=(const IndexType& x) const
    { return isLessEqual(pos_, x); }
    bool operator> (const IndexType& x) const
    { return isGreater(pos_, x); }
    bool operator>= (const IndexType& x) const
    { return isGreaterEqual(pos_, x); }
    bool operator!=(const IndexType& x) const
    { return isNotEqual(pos_, x); }
    // @}
    // }}}

    /**
     * Return true as long as iterator has more elements.
     * This function is provided for efficiency reasons.
     * To iterate till ubound in for loops, it is more 
     * efficient to use this function instead of comparison with end iterator.
     */
    operator bool() const {
        return isValid_;
    }

    /// Return a copy of this iterator
    inline DomainIter<N> begin() {
        return *this;
    }
 
    /// Return DomainIter<0> as end iterator. This is done for efficiency reason
    inline static const DomainIter<0> end() {
        return DomainIter<0>();
    }
private:
    DomainIter() { }

    void init() {
        pos_ = lbound_;

        maxRank_ = order_(0);
        stride_  = strides_(maxRank_);

        // if ubound_ < lbound_, then isValid = false;
        if (sum((ubound_ - lbound_) <0) >0)
            isValid_ = false;
        else
            isValid_ = true;
    }

protected:
    const IndexType lbound_, ubound_;
    const IndexType strides_;
    IndexType order_;

    int stride_;
    int maxRank_;

    bool isValid_;

    IndexType pos_;
};// }}}

template<class Op, int N>
class DomainIterBinder : public DomainIter<N> {// {{{
    typedef DomainIter<N> Parent;
    public:
        enum {Rank = N};
        typedef TinyVector<int,N>           IndexType;
        typedef typename Op::value_type     value_type;

        DomainIterBinder(
                const Op& op,
                const IndexType lbound, const IndexType ubound, 
                const IndexType stride, const IndexType order)
            : DomainIter<N>(lbound,ubound,stride,order), op(op) {}

        DomainIterBinder(
                const Op& op,
                const IndexType lbound, const IndexType ubound, 
                const IndexType stride =1)
            : DomainIter<N>(lbound,ubound,stride), op(op) {}

        value_type operator*() const { return op(Parent::pos_); }
    private:
        const Op& op;
};// }}}

template<int N>
DomainIter<N>& DomainIter<N>::operator++()
{// {{{
    BZPRECHECK(isValid_, "Attempted to iterate past the end of an array.");

    pos_[maxRank_] += stride_;

    // We hit this case almost all the time.
    if (pos_[maxRank_] <= ubound_[maxRank_])
    {
        return *this;
    }

    // We've hit the end of a row/column/whatever.  Need to
    // increment one of the loops over another dimension.

    int j = 1;
    for (; j < N; ++j)
    {
        const int r = order_(j);
        pos_[r] += strides_[r];

        if (pos_[r] <= ubound_[r])
            break;
    }

    // All done?
    if (j == N)
    {
        // Setting isValid_ to 0 indicates the end of the array has
        // been reached, and will match the end iterator.
        isValid_ = false;
        --j;// we donot want to reset the position for the largest 
            // stride back to lower bounds below.
    }

    // Now reset all the last pointers
    for (--j; j >= 0; --j)
    {
        const int r = order_(j);
        pos_[r] = lbound_[r];
    }
    return *this;
}// }}}


BZ_NAMESPACE_END

#endif // _LEAR_DOMAIN_ITER_H_

