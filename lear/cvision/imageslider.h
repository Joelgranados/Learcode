#ifndef _LEAR_IMAGE_SLIDER_H_
#define _LEAR_IMAGE_SLIDER_H_

#include <blitz/tinyvec.h>
#include <lear/cvision/densegrid.h>
#include <lear/blitz/ext/domainiter.h>

namespace lear {

template<int N>
class ImageSlider {
    typedef ImageSlider<N>                          ThisType;
    public:
        enum {Rank = N};
        typedef typename DenseGrid<N>::GridType     GridType;
        typedef typename DenseGrid<N>::IndexType    IndexType;

        typedef IndexType                           value_type;
        typedef blitz::DomainIterBinder<ThisType,N> iterator;

        ImageSlider(
                const IndexType t_extent_, 
                const IndexType t_wbound_, 
                const IndexType& t_stride_)
            :// {{{ 
            extent_(t_extent_),
            wbound_(t_wbound_),
            stride_(t_stride_),
            // get sliding window grid over whole image 
            slideGrid_ (DenseGrid<N>::get(extent_,wbound_,stride_)),
            size_(slideGrid_.size())
        {}
        // }}}
        value_type lbound() const { return slideGrid_(slideGrid_.lbound()); }
        value_type ubound() const { return slideGrid_(slideGrid_.ubound()); }

        value_type operator[](value_type i) const { return slideGrid_(i); }
        value_type operator()(value_type i) const { return slideGrid_(i); }

        iterator begin() const { return iterator(*this,0,slideGrid_.ubound());}
        static const blitz::DomainIter<0> end(){return blitz::DomainIter<0>();}
        int size() const { return size_; }
        value_type elem_extent() const { return slideGrid_.extent();}
        
    protected:        
        /// Sliding window stride
        IndexType extent_, wbound_, stride_;

        /// compute descriptor over points in this grid
        GridType  slideGrid_;

        int size_;
};

}

#endif // _LEAR_IMAGE_SLIDER_H_

