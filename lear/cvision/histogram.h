// {{{ file documentation
/**
 * @file
 * @brief Histogram utility
 */
// }}}

#ifndef _LEAR_HISTOGRAM_H_
#define _LEAR_HISTOGRAM_H_

// {{{ headers
#include <boost/mpl/if.hpp>

#include <cmath> // required for ceil
#include <blitz/array.h>
#include <blitz/tinyvec.h>
#include <blitz/tinyvec-et.h>

#include <lear/io/bistreamable.h>
#include <lear/io/fileheader.h>
#include <lear/blitz/tvmio.h>
#include <lear/blitz/blitzio.h>
#include <lear/blitz/ext/numtrait.h>
#include <lear/blitz/ext/domainiter.h>
// }}}

namespace lear {

/**
 * template parameters
 * HType Histogram Type (usually int)
 * VType vector type (preferable floating point)
 * VDim vector dimension 
 */
template<class Implementor, class HType, class VType, int VDim = 1> 
class Histogram : public BiStreamable< Implementor > {
    public:
        // {{{ typedefs
        /// Rank of the histogram
        enum {Rank = VDim};
        /// Value Element Type
        typedef VType                                      ValueElementType;
        /// Histogram Type
        typedef typename
            blitz::ExtNumericTraits<HType>::T_sumtype      HistElementType;

        /// Histogram index type
        typedef typename boost::mpl::if_c<  VDim == 1, 
                            int, 
                            blitz::TinyVector<int, VDim>
                        >::type                            IndexType;
        /// Value type
        typedef typename boost::mpl::if_c<  VDim == 1, 
                            VType, 
                            blitz::TinyVector<VType, VDim>
                        >::type                            ValueType;

        /// real value index precision type
        typedef typename 
            blitz::ExtNumericTraits<VType>::T_floattype    FloatType;
        typedef typename boost::mpl::if_c<  VDim == 1, 
                            FloatType, 
                            blitz::TinyVector<FloatType, VDim>
                        >::type                            PrecisionType;

        /// Histogram array type
        typedef blitz::Array< HistElementType, VDim>       HistType;

        /// Real type Histogram type
        typedef typename 
            blitz::ExtNumericTraits<HType>::T_floattype    RealType;

        /// Probability distribution array type
        typedef blitz::Array< RealType, VDim>              DistType;

        /// iterator types on histogram elements
        typedef typename HistType::iterator                iterator;
        typedef typename HistType::const_iterator          const_iterator;

        typedef Implementor                                ImplementorType;
        typedef Histogram<Implementor, HType, VType, VDim> HistogramType; 
        // }}}

    // {{{ fill functions, implements fill interface
    /**
     * Fill a histogram from lbound to ubound. use convertorS to convert
     * index to value and convertW to convert index to weight. 
     */
//      template< int SignalSize, class ConvertorS, class ConvertorW>
//      void transform(
//          const blitz::TinyVector<int,SignalSize> lbound,
//          const blitz::TinyVector<int,SignalSize> ubound,
//          const ConvertorS& convertorS,
//          const ConvertorW& convertorW)
//      {
//          for (blitz::DomainIter<SignalSize> iter(lbound,ubound); 
//                  iter != iter.end(); ++iter)
//              static_cast<Implementor&>(*this)
//                  ( convertorS( *iter ), convertorW( *iter )); 
//      }
//      /**
//       * Fill a histogram from lbound to ubound. use convertorS to convert
//       * index to value. 
//       */
//      template<class SignalType, int SignalSize, class Convertor>
//      void transform(
//          const blitz::TinyVector<int,SignalSize> lbound,
//          const blitz::TinyVector<int,SignalSize> ubound,
//          const Convertor& convertorS) 
//      {
//          for (blitz::DomainIter<SignalSize> iter(lbound,ubound); 
//                  iter != iter.end(); ++iter)
//              static_cast<Implementor&>(*this)
//                  ( convertorS( *iter )); 
//      }
    /**
     * Fill a histogram from with value iterator and weight iterator
     */
    template<class ValueIter, class WeightIter>
    ValueIter for_each(
        ValueIter s_ci, ValueIter e_ci, WeightIter wi)
    {
        for (;e_ci != s_ci; ++s_ci, ++wi)
            static_cast<Implementor&>(*this)( *s_ci, *wi);
        return s_ci;
    }
    /**
     * Fill a histogram from with value iterator
     */
    template<class ValueIter>
    ValueIter for_each(ValueIter s_ci, ValueIter e_ci)
    {
        for (;e_ci != s_ci; ++s_ci)
            static_cast<Implementor&>(*this)( *s_ci);
        return s_ci;
    }
    // Push the weight at value in the histogram
    void operator()(
            const ValueType value, 
            const HistElementType weight = 1) 
    {
        static_cast<Implementor&>(*this).push(value,weight);
    }
    // }}}
    
    /// return histogram value corresponding to value v
    inline HistElementType& operator[] (const ValueType v) {
        return histogram_( index( v) );
    }
    /// return histogram value corresponding to value v
    inline HistElementType operator[] (const ValueType v) const {
        return histogram_( index( v) );
    }

    /// pre-increment all bins in histogram 
    inline ImplementorType& operator++ () {
        ++histogram_;
        return static_cast<ImplementorType&>(*this);
    }
    /// post-increment all bins in histogram
    inline ImplementorType& operator++ (int) {
        ++histogram_;
        return static_cast<ImplementorType&>(*this);
    }
    /// increment all bins in histogram 
    inline ImplementorType& operator+= (const HistElementType e) {
        histogram_+= e;
        return static_cast<ImplementorType&>(*this);
    }
    inline ImplementorType& operator+= (const HistType e) {
        histogram_+= e;
        return static_cast<ImplementorType&>(*this);
    }
    /// divide all bins in histogram 
    inline ImplementorType& operator*= (const HistType& e) {
        histogram_*= e;
        return static_cast<ImplementorType&>(*this);
    }
    inline ImplementorType& operator*= (const HistElementType& e) {
        histogram_*= e;
        return static_cast<ImplementorType&>(*this);
    }

    /** Convert histogram to probability distribution.
     *  Allocate new array and return it.
     */
    inline DistType normalize() const {
        DistType dist(bin_);
        return normalize(dist);
    }

    /** convert histogram to probability distribution
     * and copy it to dist parameter
     *
     * @warning dist.size() and hist.size() must be of same
     * size
     */
    inline DistType& normalize(DistType& dist) const {
        RealType normfactor = blitz::sum(histogram_);
        if (normfactor >1e-10)
            dist = histogram_/normfactor;
        else
            std::for_each(dist.begin(), dist.end(),Clear());
            //dist = 0;
        return dist;
    }

    /// set histogram to zero
    inline void clear() { 
        std::for_each(histogram_.begin(), histogram_.end(),Clear());
    //    histogram_ = 0; 
    }

//      inline HistType& histogram() const { return histogram_; }
    /** histogram array, returns a reference 
     * i.e. changing array will change histogram
     */
    inline const HistType& data() const { return histogram_; }

    inline HistType& data()             { return histogram_; }

    /// start iterator over the histogram
    inline iterator begin()             { return histogram_.begin(); }
    /// end iterator over the histogram
    inline iterator end()               { return histogram_.end(); }

    /// start iterator over the histogram
    inline const_iterator begin() const { return histogram_.begin(); }
    /// end iterator over the histogram
    inline const_iterator end() const   { return histogram_.end(); }

    inline const ValueType lbound() const { return lbound_; }
    inline const ValueType extent() const { return extent_; }

    /// bandwidth of each bin in the histogram
    inline const ValueType bandwidth() const { return bandwidth_; }
    /// number of bins in the histogram
    inline const IndexType bin() const { return bin_; }
    inline const int size() const { return histogram_.size(); }

    void print(std::ostream& o ) const {
        o << "Lbound " << lbound_ << ", Extent " << extent_ << ", Bandwidth " << bandwidth_ << std::endl;
        o << "#Bins  " << bin_ << ", CenterBound " << cenBound_ << ", HalfBin " << halfBin_ << std::endl;
    }
    protected:
        ValueType lbound_;
        ValueType extent_;
        ValueType bandwidth_;

        IndexType bin_;

        HistType histogram_;

        PrecisionType cenBound_;
        PrecisionType halfBin_;

        struct Clear {
            template<class T>
            void operator()(T& a) { a = 0;}
        };

        template<class IndexT>
        struct Resize {
            Resize(IndexT e):e(e){}
            template<class T>
            void operator()(T& a) { a.resize(e);}
            IndexT e;
        };


    /**
     * Create a histogram. Number of bins are calculated appropiately 
     * using maximum bounds, minimum bounds and bandwidth.
     *
     * @lbound    minimum bound/value of signal
     * @extent    maximum extent value of signal
     * @bandwidth   specify the bandwidth of each bin.
     */
    inline Histogram(
            const ValueType& lbound_,
            const ValueType& extent_,
            const ValueType& bandwidth_)
    // {{{ constructor
        :
            lbound_(lbound_), 
            extent_(extent_), 
            bandwidth_(bandwidth_),
            bin_(static_cast<IndexType>(ceil(extent_/bandwidth_))),
            histogram_ (bin_),
            cenBound_(extent_/2.0 + lbound_),
            halfBin_ (bin_/2.0)
        {
            clear();
        }
    // }}}

    template <class HTypeIndexType>
    inline Histogram(
            const ValueType& lbound_,
            const ValueType& extent_,
            const ValueType& bandwidth_,
            const HTypeIndexType hextent_)
    // {{{ constructor
        :
            lbound_(lbound_), 
            extent_(extent_), 
            bandwidth_(bandwidth_),
            bin_(static_cast<IndexType>(ceil(extent_/bandwidth_))),
            histogram_ (bin_),
            cenBound_(extent_/2.0 + lbound_),
            halfBin_ (bin_/2.0)
        {
            std::for_each(histogram_.begin(), histogram_.end(),
                    Resize<HTypeIndexType>(hextent_));
            clear();
        }
    // }}}

    /// Protected default constructor. To be used by child class only
    inline Histogram(){}
            
    /// Copy Construcotr// {{{
    Histogram(const ImplementorType& o):
        lbound_(o.lbound_), 
        extent_(o.extent_), 
        bandwidth_(o.bandwidth_),
        bin_(o.bin_),
        histogram_ (o.histogram_.copy()),
        cenBound_(o.cenBound_),
        halfBin_ (o.halfBin_)
    {}// }}}
    Histogram& operator=(const ImplementorType& o){// {{{
        if (this != & o) {
            lbound_ = o.lbound_;
            extent_ = o.extent_;
            bandwidth_ = o.bandwidth_;
            bin_ = o.bin_;

            histogram_.reference(o.histogram_.copy());

            cenBound_ = o.cenBound_;
            halfBin_ = o.halfBin_;
        }
        return *this;
    }// }}}

    inline BiOStream& bsave_common(
            const FileHeader& header, BiOStream& out) const 
    {// {{{
        out << header;

        out << lbound_ << extent_ 
            << bandwidth_<<bin_;
        out << histogram_;

        return out;
    }// }}}
    inline BiIStream& bload_common(
            const FileHeader& header, BiIStream& in) 
    {// {{{
        FileHeader nh;
        in >> nh;
        if (!nh.identify(header.tag())) {
            throw Exception("Histogram::bload_common()",
            "Invalid header. '" + nh.tag() + 
            "' does not match class header '" + header.tag()+"'");
        }
        if (nh.version() < header.version()) { // check for version number
            throw Exception("Histogram::bload_common()",
            "Invalid version number");
        }

        in >> lbound_ >> extent_ 
           >> bandwidth_ >> bin_;
        in >> histogram_;
        cenBound_=(extent_/2.0 + lbound_);
        halfBin_=(bin_/2.0);
        return in;
    }
};// }}}


} // namespace lear
#endif // _LEAR_HISTOGRAM_H_

