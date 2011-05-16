// {{{ file documentation
/**
 * @file
 * @brief Histogram utility
 */
// }}}

#ifndef _LEAR_DISCRETE_HISTOGRAM_H_
#define _LEAR_DISCRETE_HISTOGRAM_H_

#include <lear/cvision/histogram.h>

namespace lear {

template<class HType, class VType, int VDim = 1> 
class DiscreteHistogram : public Histogram< 
    DiscreteHistogram<HType, VType, VDim>, HType, VType, VDim > 
{
    public:
    typedef Histogram< 
        DiscreteHistogram<HType, VType, VDim>, HType, VType, VDim > 
                                                            Parent;

        typedef typename Parent::HistElementType    HistElementType;
        typedef typename Parent::IndexType          IndexType;
        typedef typename Parent::ValueType          ValueType;
        typedef typename Parent::FloatType          FloatType;
        typedef typename Parent::PrecisionType      PrecisionType;
        typedef typename Parent::HistType           HistType; 
        typedef typename Parent::RealType           RealType;
        typedef typename Parent::DistType           DistType;
        typedef typename HistType::iterator             iterator;
        typedef typename HistType::const_iterator       const_iterator;
        typedef typename Parent::HistogramType      HistogramType;
        typedef typename Parent::ImplementorType    ImplementorType;

    inline DiscreteHistogram(
            const ValueType& lbound_,
            const ValueType& extent_,
            const ValueType& bandwidth_)
        : 
            Parent(lbound_,extent_, bandwidth_) {}

    /// Fill histogram at value with weight (default 1)
    inline void push(
            const ValueType value, 
            const HistElementType weight = 1) 
    {
        histogram_( toindex( value) ) += weight;
    }

    /// Convert value to histogram index
    inline IndexType toindex(const ValueType& value) const {
        return static_cast<IndexType> (Parent::halfBin_ + 
                (value - Parent::cenBound_)/Parent::bandwidth_ );
    }
    /// Convert index to histogram value 
    inline ValueType tovalue(const IndexType& index) const {
        return (index - Parent::halfBin_)*Parent::bandwidth_+ 
                Parent::cenBound_;
    }
    inline BiIStream& bload(BiIStream& in) {
        Parent::bload_common(lear::FileHeader("DHistgrm",100),in);
        return in;
    }

    inline BiOStream& bsave(BiOStream& out) const {
        Parent::bsave_common(lear::FileHeader("DHistgrm",100), out);
        return out;
    }
    void print(std::ostream& o ) const {
        o << "DiscreteHistogram::" << std::endl;
        Parent::print(o);
    }
};

template<class HType, class VType, int VDim> 
std::ostream& operator<<(std::ostream& o, const DiscreteHistogram<HType, VType, VDim>& h ) {
    h.print(o);
    return o;
}

} // namespace lear
#endif // _LEAR_DISCRETE_HISTOGRAM_H_

