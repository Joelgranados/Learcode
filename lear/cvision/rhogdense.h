#ifndef _LEAR_RHOG_DENSE_H_
#define _LEAR_RHOG_DENSE_H_

#include <iostream>
#include <algorithm>

#include <blitz/array.h>
#include <blitz/tinyvec.h>

#include <lear/io/biostream.h>

#include <lear/cvision/phistogram.h>
#include <lear/cvision/iprocessor.h>
#include <lear/cvision/dnormalizer.h>

namespace lear {

/**
 * Compute Rectangular Histogram of Oriented Gradients over image gradient.
 *
 * Different image normalizer and image preprocessor can be supplied.
 */
class RHOGDense {
    public:
    typedef IProcessor::RealType                    RealType; 

    protected:
    typedef lear::PrecisionHistogram<RealType,RealType,3>    
                                                    HistogramType;

    public:

    enum {N=2};
    typedef blitz::TinyVector<int,N>                IndexType;
    typedef IProcessor::InfoType                    Preprocessor;

    /// feature vector dimension 
    enum {Rank = HistogramType::Rank};

    /// feature vector element type
    typedef HistogramType::HistElementType          ElemType;

    /// feature array type
    typedef HistogramType::HistType                 FeatType;

    /// feature vector array index type
    typedef HistogramType::IndexType                FeatIndexType;

    /**
     * Initializes RHOGDense descriptor.
     *
     * It owns preprocessor and normalizer pointers and ensures that they are
     * freed.
     */
    RHOGDense(
            const IndexType cellsize_,
            const IndexType numcell_,
            const IndexType stride_,
            const int orientbin_,
            const RealType wtscale_,
            const bool semicirc_,
            const IProcessor* p,
            const DescNormalizer<RealType>* n
            );
    ~RHOGDense()
    {
        delete processor;
        delete normalizer;
    }

    template<class PixelType>
    Preprocessor preprocess(const blitz::Array<PixelType,N>& image) const 
    { return (*processor)(image); }

    /// Only changes hist_ and tmag_ variables. Rest all remains constant.
    FeatType& operator() (const IndexType point, const Preprocessor& p) const ;

    int size() const { return descsize_; }
    int length() const { return descsize_; }
    IndexType extent() const { return extent_; }
    IndexType stride() const { return stride_; }

    void print(lear::BiOStream& o) const ;

    void print(std::ostream& o) const ;

    protected:
    /// cell size i.e. position shift tolerance size (in pixels)
    IndexType cellsize_; 

    /// number of cells or numcell_ bin in the descriptor
    IndexType numcell_;

    /// optimal stride_
    IndexType stride_;

    /// number of orientation bins
    int orientbin_; 
    
    /// scale for weighting function span
    RealType wtscale_;
        
    /// if true, orientation bin range is 0-180 degree range, else 0-360
    bool semicirc_;

    /// image pre-processor to use
    const IProcessor*  processor;

    /// histogram normalizer to use
    const DescNormalizer<RealType>* normalizer;

    /// descriptor size
    int descsize_;

    /// filter bound 
    IndexType extent_, ubound_; 
    
    /// weight_ing array
    blitz::Array<RealType,N> weight_;

    blitz::TinyVector<RealType,3>   h_extent, h_bandwidth;

    /// temp array to save filter response
    mutable Preprocessor::MagAType tmag_;

    mutable HistogramType hist_;
    
};

BiOStream& operator<<(BiOStream& o, const RHOGDense& d);

}

std::ostream& operator<<(std::ostream& o, const lear::RHOGDense& d);


#endif // _LEAR_RHOG_DENSE_H_
