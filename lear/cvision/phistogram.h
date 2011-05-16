// {{{ file documentation
/**
 * @file
 * @brief Precision Histogram utility
 */
// }}}

#ifndef _LEAR_PRECISION_HISTOGRAM_H_
#define _LEAR_PRECISION_HISTOGRAM_H_

#include <lear/cvision/histogram.h>
#include <lear/cvision/interpolate.h>

namespace lear {

/**
 * Create precision histogram i.e. use linear interpolation between
 * histogram bins. This implies that template HType must be real 
 * typed value, e.g. float or double.
 */
template<class HType, class VType, int VDim = 1> 
class PrecisionHistogram : public Histogram< 
    PrecisionHistogram<HType, VType, VDim>, HType, VType, VDim > 
{
    public:
    typedef Histogram< 
        PrecisionHistogram<HType, VType, VDim>, HType, VType, VDim > 
                                                        Parent;
        typedef typename Parent::HistElementType    HistElementType;
        typedef typename Parent::IndexType          IndexType;
        typedef typename Parent::ValueType          ValueType;
        typedef typename Parent::FloatType          FloatType;
        typedef typename Parent::PrecisionType      PrecisionType;
        typedef typename Parent::HistType           HistType; 
        typedef typename Parent::RealType           RealType;
        typedef typename Parent::DistType           DistType;
        typedef typename HistType::iterator         iterator;
        typedef typename HistType::const_iterator   const_iterator;
        typedef typename Parent::ImplementorType    HistogramType;
        typedef typename Parent::ImplementorType    ImplementorType;

        typedef blitz::TinyVector<bool, VDim>           BoolType;
        typedef blitz::TinyVector<int, VDim>            TVIndexType;
        typedef blitz::TinyVector<FloatType, VDim>      TVPrecisionType;
        
    inline PrecisionHistogram(
            const ValueType& lbound_,
            const ValueType& extent_,
            const ValueType& bandwidth_,
            const BoolType& warp_ = false)
        :   
            Parent(lbound_,extent_,bandwidth_),
            warp_(warp_), onehalf_(0.5), tvbin_(Parent::bin_)
        {}

    template <class HTypeIndexType>
    inline PrecisionHistogram(
            const ValueType& lbound_,
            const ValueType& extent_,
            const ValueType& bandwidth_,
            const BoolType& warp_,
            const HTypeIndexType hextent_)
        :   
            Parent(lbound_,extent_,bandwidth_,hextent_),
            warp_(warp_), onehalf_(0.5), tvbin_(Parent::bin_)
        {}
    /**
     * Makes a copy of the histogram element. Note the objects is deep copied,
     * i.e. its the array type is not shared.
     */
    PrecisionHistogram(const PrecisionHistogram& o)/// Copy Construcotr// {{{
        : 
        Parent(o),
        warp_(o.warp_), 
        onehalf_(o.onehalf_),
        tvbin_(o.tvbin_)
    {}// }}}

    inline PrecisionHistogram(const std::string& filename) {
        lear::BiIStream from(filename.c_str());
        if (!from) {
            throw lear::Exception("compute()",
                "Unable to open histogram file " + filename);
        }
        bload(from);
    }

    /// Precision fill histogram at value with weight (default 1)
    void push(
            const ValueType value, 
            const HistElementType weight = 1) 
    {
        using namespace blitz;
        using namespace std;
        PrecisionType pI ( toindex( value));// precision index
        TVIndexType fI ( static_cast<IndexType>(floor(pI))); // floor index
        TVIndexType cI ( static_cast<IndexType>(ceil(pI))); // ceil index
        TVPrecisionType d = pI - fI; // delta

        BoolType lowervalid=true;
        BoolType uppervalid=true;
        for (int i= 0; i< VDim; ++i) 
            if (warp_[i]) {
                cI[i] %= tvbin_[i];
                fI[i] %= tvbin_[i];
                if (fI[i] < 0)
                    fI[i] += tvbin_[i];
                if (cI[i] < 0)
                    cI[i] += tvbin_[i];
            } else {
                if (cI[i] >=tvbin_[i]-onehalf_ || cI[i] < -onehalf_)
                    uppervalid[i] = false;
                if (fI[i] < -onehalf_ || fI[i] >=tvbin_[i]-onehalf_)
                    lowervalid[i] = false;
            }

        Interpolate<VDim>::linear(Parent::histogram_,weight,d,fI,cI,
                                    lowervalid,uppervalid);
    }
    /// Convert value to histogram index
    inline PrecisionType toindex(const ValueType& value) const {
        return Parent::halfBin_ - onehalf_ +  
            (value - Parent::cenBound_)/Parent::bandwidth_;
    }
    inline ValueType tovalue(const PrecisionType& index) const {
        return (index - Parent::halfBin_ + onehalf_)
            *Parent::bandwidth_ + Parent::cenBound_;
    }

    inline BiIStream& bload(BiIStream& in) {
        Parent::bload_common(FileHeader("PHistgrm",100),in);

        in >> warp_;
        onehalf_=0.5; 
        tvbin_=Parent::bin_;
        return in;
    }

    inline BiOStream& bsave(BiOStream& out) const {
        Parent::bsave_common(FileHeader("PHistgrm",100), out);
        out << warp_;
        return out;
    }
    void print(std::ostream& o ) const {
        o << "PrecisionHistogram::" << std::endl;
        Parent::print(o);
        o << "Warp " << warp_ << ", OneHalf " << onehalf_ << ", TinyVectorBin " << tvbin_ << std::endl;
    }
    protected:
        PrecisionHistogram& operator=(const PrecisionHistogram& o){// {{{
            Parent::operator=(o);
            if (this != & o) {
                warp_=o.warp_; 
                onehalf_=o.onehalf_;
                tvbin_=o.tvbin_;
            }
            return *this;
        }// }}}


        BoolType  warp_;
    private:
        FloatType     onehalf_;
        TVIndexType   tvbin_;
};

template<class HType, class VType, int VDim> 
std::ostream& operator<<(std::ostream& o, const PrecisionHistogram<HType, VType, VDim>& h ) {
    h.print(o);
    return o;
}


} // namespace lear
#endif // _LEAR_PRECISION_HISTOGRAM_H_

