#ifndef _LEAR_SCALE_PYRAMID_H_
#define _LEAR_SCALE_PYRAMID_H_

#include <blitz/tinyvec.h>
#include <lear/image/rescale.h>

namespace lear {
/**
 */
template<int N>
class ScalePyramid {
    protected:
        typedef ScalePyramid<N>     ThisType;

    public:
        typedef float                               RealType;
        typedef blitz::TinyVector<int,N>            IndexType;
        typedef blitz::Array<RealType,1>            Array1DType;
        typedef IndexType                           value_type;
        typedef blitz::Array<IndexType,1>           Array1DIndexType;


        ScalePyramid(
                const IndexType image_extent, 
                const IndexType extent_, 
                const RealType scaleRatio_ = 1.2,
                RealType endScale_ = 0,
                RealType startScale_ = 1)
        { // {{{ 
            if (scaleRatio_ <= 1) {
                throw lear::Exception("ScalePyramid::constructor()",
                        "Scale ratio must be greater than 1");
            }
            using namespace blitz;

            // check if endScale will give proper window size
            if (endScale_ > 0) {
                IndexType e (image_extent/endScale_);
                if (sum(e<extent_)) {
                    endScale_ = 0;
                    // print warning here
                    std::cout << 
" Scaling using end scale will create too small image. Changing\n"
" to appropiate end scale" << std::endl; 
                }
            }

            // if no endscale, compute endscale from image size
            if (endScale_ <= 0) {
                TinyVector<RealType,N> e = image_extent;
                endScale_ = min(e/extent_);
            } else if (endScale_ < 0) {
                // negative endscale. provides further zoom out on image
                TinyVector<RealType,N> e = image_extent;
                endScale_ = min(e/extent_*(-endScale_));
            }

            // no. of steps in pyramid
            size_ =  static_cast<int>( std::floor(
                    std::log(endScale_/startScale_)/std::log(scaleRatio_)) +1);

            if (size_ <= 0) {
                size_ = 0;
                return;
            }
            scaleLevel_.resize(size_);
            scaleLevel_ = startScale_*pow(scaleRatio_,tensor::i);
            sizePyramid_.resize(size_);

            // build scale-space pyramid
            for (int i= 0; i< size_; ++i) 
                sizePyramid_(i) = IndexType(image_extent/scaleLevel_(i));
        }
        // }}}


        value_type operator[](int i) const {
            return sizePyramid_(i);
        }

        value_type at(int i) const {
            if (i < 0 || i >= size_)
                throw lear::Exception("ScalePyramid::at","Index out of range");
            return sizePyramid_(i);
        }

        int size() const      { return size_; }
        RealType startScale() const { return scaleLevel_(0); }
        RealType endScale()   const { return scaleLevel_(size_-1); }
        RealType scale(int i) const { return scaleLevel_(i); }

    protected:        
        /// pyramid level
        int size_;

        /// scale array
        Array1DType scaleLevel_;

        /// size pyramid
        Array1DIndexType sizePyramid_;

        class common_iter{ // {{{
            public:
            bool operator!=(const common_iter& o) {
                if (o.index != index )
                    return true;
                return false;
            }
            operator bool() {
                return (index<pContainer->size() && index>=0) ? true: false;
            }
            value_type operator*() const {
                return (*pContainer)[index];
            }
            RealType scale() const {
                return pContainer->scale(index);
            }
            int getindex() const {return index;}
            protected:
                common_iter(const ScalePyramid* pContainer, const int index)
                    :
                    pContainer(pContainer),
                    index(index)
                {}
                friend class ScalePyramid;

                const ScalePyramid* pContainer;
                int index; 
        };// }}}
    public:
        class iterator : public common_iter{// {{{
            public:
            iterator& operator++() {
                // only if scaleindex is still valid
                if (common_iter::index < common_iter::pContainer->size()) 
                    ++common_iter::index;
                return *this;
            }
            iterator& operator--() {
                // only if scaleindex is still valid
                if (common_iter::index >= 0) --common_iter::index;
                return *this;
            }
            iterator& operator++(int) {
                return ++(*this);
            }
            iterator& operator--(int) {
                return --(*this);
            }
            protected:
                friend class ScalePyramid;
                iterator(const ScalePyramid* pContainer, const int index):
                    common_iter(pContainer,index) {}
        };// }}}
        class reverse_iterator : public common_iter{// {{{
            public:
            reverse_iterator& operator++() {
                // only if scaleindex is still valid
                if (common_iter::index >= 0) --common_iter::index;
                return *this;
            }
            reverse_iterator& operator--() {
                // only if scaleindex is still valid
                if (common_iter::index < common_iter::pContainer->size()) 
                    ++common_iter::index;
                return *this;
            }
            reverse_iterator& operator++(int) {
                return ++(*this);
            }
            reverse_iterator& operator--(int) {
                return --(*this);
            }
            protected:
                friend class ScalePyramid;
                reverse_iterator(const ScalePyramid* pContainer, const int index):
                    common_iter(pContainer,index) {}
        };// }}}

        iterator begin() const{ return iterator(this,0); }
        iterator end() const  { return iterator(this,size_); }
        reverse_iterator rbegin() const{ return reverse_iterator(this,size_-1); }
        reverse_iterator rend() const  { return reverse_iterator(this,-1); }

};

}

#endif // _LEAR_SCALE_PYRAMID_H_


