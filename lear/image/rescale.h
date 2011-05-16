/*
 * @file rescale.h
 * @brief rescale an image using the supplied filter
 */

#ifndef _LEAR_RESCALE_H_
#define _LEAR_RESCALE_H_

#include <cmath>

#include <blitz/array.h>
#include <blitz/tinyvec.h>
#include <blitz/tinyvec-et.h>

#include <lear/blitz/ext/domainiter.h>
#include <lear/blitz/ext/numtrait.h>
#include <lear/exception.h>

namespace lear {

// {{{ BilinearFilter
/** 
 * Bilinear filter
 */
template<class T_realtype>
class BilinearFilter {
    public:
        typedef T_realtype  RealType;

        BilinearFilter () : width_(1){}

        /// Returns filter response
        RealType apply (const RealType val) const {
            const RealType t = std::fabs(val); 
            return (t < width_ ? width_ - t : 0); 
        }

        /// Returns the filter support
        RealType width() const { return width_; }

    protected:
        /// Filter support
        const RealType  width_;
};
// }}}
    namespace detail {
// {{{ WeightTable
    /**
     * Filter weights table.
     * This class stores contribution information for an entire line (row or column).
     */
    template<class RealType>
    class WeightTable {
        public:
            typedef blitz::TinyVector<int,2>           IndexType;
            /** 
             * Constructor<br>
             * Allocate and compute the weights table
             * This class stores contribution information for an entire line (row or column).
             * @param pFilter Filter used for upsampling or downsampling
             * @param uLineSize Length (in pixels) of the destination line buffer
             * @param uSrcSize Length (in pixels) of the source line buffer
             */
            WeightTable(const BilinearFilter<RealType>& filter, int uDstSize, int uSrcSize) :
                m_Scale(uDstSize/ static_cast<RealType> (uSrcSize)),
                m_LineLength (uDstSize),
                m_Width( m_Scale < 1 ? filter.width()/m_Scale : filter.width()),
                m_WindowSize (2 * static_cast<int>(std::ceil(m_Width)) + 1),
                m_WeightTable(m_LineLength, m_WindowSize), // allocate a new line contributions structure
                index(m_LineLength)
            {
                m_WeightTable = 0;
                const RealType dFScale = m_Scale < 1 ? m_Scale : 1;

                for(int u = 0; u < m_LineLength; u++) {
                        // scan through line of contributions
                        RealType dCenter = u / m_Scale;   // reverse mapping

                        // find the significant edge points that affect the pixel
                        int iLeft = std::max(static_cast<int>(std::ceil (dCenter - m_Width)), 0); 
                        int iRight= std::min(static_cast<int>(std::floor(dCenter + m_Width)), uSrcSize - 1); 

                        index(u) = iLeft, iRight;

                        RealType dTotalWeight = 0;
                        for(int iSrc = iLeft; iSrc <= iRight; ++iSrc) { // calculate weights
                            dTotalWeight += m_WeightTable(u,iSrc-iLeft) = dFScale * filter.apply( dFScale*(dCenter - iSrc) );
                        }

                        // normalize
                        if(dTotalWeight > 0) 
                            m_WeightTable(u, blitz::Range::all()) /= dTotalWeight; 
                } 
            }

            /** Retrieve a filter weight, given source and destination positions
            @param dst_pos Pixel position in destination line buffer
            @param src_pos Pixel position in source line buffer
            @return Returns the filter weight
            */
            double getWeight(int dst_pos, int src_pos) {
                    return m_WeightTable(dst_pos,src_pos);
            }

            /** Retrieve left boundary of source line buffer
            @param dst_pos Pixel position in destination line buffer
            @return Returns the left boundary of source line buffer
            */
            int getLeftBoundary(int dst_pos) {
                    return index(dst_pos)[0];
            }

            /** Retrieve right boundary of source line buffer
            @param dst_pos Pixel position in destination line buffer
            @return Returns the right boundary of source line buffer
            */
            int getRightBoundary(int dst_pos) {
                    return index(dst_pos)[1];
            }
        private:
            const RealType m_Scale;
            /// Length of line (no. or rows / cols) 
            const int m_LineLength;

            /// original width of filter -- keeping in mind scale
            const RealType m_Width;

            /// Filter window size (of affecting source pixels) -- the number of sampled pixels
            const int m_WindowSize;

            /// Row (or column) of contribution weights, first column is equal to number of rows/columns
            /// second column is equal to 2*filter.width()+1
            blitz::Array<RealType,2> m_WeightTable;

            /// leftIndex and rightIndex array
            blitz::Array<IndexType,1> index;

    };
// }}}
    
    template <class T>
    struct ValueBounded {// {{{
        ValueBounded(const T& lbound, const T& ubound):
            lbound(lbound), ubound(ubound)
        {}
        T operator()(const T& a) const {
            return (blitz::MyMin(blitz::MyMax(lbound, a), ubound));
        }
        private:
            const T lbound, ubound;
    };// }}}
    struct ValueUnbounded {// {{{
        template <class T> 
        const T& operator()(const T& a) const {
            return a;
        }
    };// }}}

    // {{{ horizontal/vertical filtering
    /// Performs horizontal image filtering
    template<class RealType, class ElementTypeA, class ElementTypeB, class BoundType>
    static void horizontalFilter(
            const BilinearFilter<RealType>& filter,
            const blitz::Array<ElementTypeA, 2>& src,
            blitz::Array<ElementTypeB,2>& dst,
            const BoundType& bound) 
    { 
        typedef typename blitz::promote_trait<
            ElementTypeA, ElementTypeB>::T_promote    ElementType;
        typedef blitz::TinyVector<int,2>              IndexType;

        IndexType dst_extent = dst.extent();
        IndexType src_extent = src.extent();

	if(dst_extent[0] == src_extent[0]) {
            std::copy(src.begin(),src.end(),dst.begin());
            //for (blitz::DomainIter<2> i(0, dst_extent-1); i; ++i)
            //    dst(*i) = static_cast<ElementTypeB> (src(*i)); // no scaling required, just copy
	} else {
            // allocate and calculate the contributions
            WeightTable<RealType> weightTable(filter, dst_extent[0], src_extent[0]); 

            for(int y = 0; y < dst_extent[1]; ++y) { // step through rows            
            for(int x = 0; x < dst_extent[0]; ++x) { // scale each row 
                    // loop through row
                    ElementType value; value = 0;
                    RealType sumweight=0; 
                    const int iLeft = weightTable.getLeftBoundary(x);    // retrieve left boundary
                    const int iRight = weightTable.getRightBoundary(x);  // retrieve right boundary

                    for(int i = iLeft; i <= iRight; i++) {
                            // scan between boundaries
                            // accumulate weighted effect of each neighboring pixel
                            RealType weight= weightTable.getWeight(x, i-iLeft);
                            value += weight * src(i,y); 
                            sumweight+=weight;
                    } 
                    if (sumweight)
                        dst(x,y) = static_cast<ElementTypeB> (bound(value/sumweight));
                    else
                        dst(x,y)=0;
                    // place result in destination pixel
                    //dst(x,y) = static_cast<ElementTypeB> (bound(value));
            } 
            }
	}
    } 

    /// Performs vertical image filtering
    template<class RealType, class ElementTypeA, class ElementTypeB, class BoundType>
    static void verticalFilter(
            const BilinearFilter<RealType>& filter,
            const blitz::Array<ElementTypeA, 2>& src,
            blitz::Array<ElementTypeB,2>& dst,
            const BoundType& bound) 
    {
        typedef typename blitz::promote_trait<
            ElementTypeA, ElementTypeB>::T_promote    ElementType;
        typedef blitz::TinyVector<int,2>              IndexType;

        IndexType dst_extent = dst.extent();
        IndexType src_extent = src.extent();

	if(dst_extent[1] == src_extent[1]) {
            std::copy(src.begin(),src.end(),dst.begin());
            //for (blitz::DomainIter<2> i(0, dst_extent-1); i; ++i)
            //    // no scaling required, just copy
            //    dst(*i) = static_cast<ElementTypeB> (src(*i)); 
	} else {
            // allocate and calculate the contributions
            WeightTable<RealType> weightTable(filter, dst_extent[1], src_extent[1]); 

            for(int x = 0; x < dst_extent[0]; ++x) { // step through columns
            for(int y = 0; y < dst_extent[1]; ++y) { // scale each column
                    // loop through column
                    ElementType value; value = 0;
                    RealType sumweight=0; 

                    // retrieve left boundary
                    int iLeft = weightTable.getLeftBoundary(y);
                    // retrieve right boundary
                    int iRight = weightTable.getRightBoundary(y);  

                    for(int i = iLeft; i <= iRight; i++) {
                        // scan between boundaries
                        // accumulate weighted effect of each neighboring pixel
                        RealType weight = weightTable.getWeight(y, i-iLeft);
                        value += weight * src(x,i);
                        sumweight+=weight;
                    } 
                    if (sumweight)
                        dst(x,y) = static_cast<ElementTypeB> (bound(value/sumweight));
                    else
                        dst(x,y)=0;
                    //dst(x,y) = static_cast<ElementTypeB> (bound(value));
            }
            }
	}
    } 
    // }}}

    }

    /**
     * Rescale the image to the supplied destination size using the supplied filter
     */
    template<class ElementType, class BoundType>
    blitz::Array<ElementType,2> rescale(
            const blitz::Array<ElementType,2>& src, 
            const int dst_width, const int dst_height,
            const BoundType& bound) 
    { // {{{ rescale
        typedef typename blitz::ExtNumericTraits<ElementType>::T_basictype     BasicType;
        typedef typename blitz::ExtNumericTraits<BasicType  >::T_floattype     RealType;
        typedef typename blitz::ExtNumericTraits<ElementType>::T_floattype     RealElementType;

        typedef blitz::TinyVector<int,2>                        IndexType;
        typedef blitz::Array<ElementType,2>                     ArrayType;
        typedef blitz::Array<RealElementType,2>                 RealArrayType;

        IndexType src_extent = src.extent();
        if (dst_width == src_extent[0] && dst_height == src_extent[1])
            return src.copy();

	if ((dst_width <= 0) || (dst_height <= 0)) {
            throw Exception("rescale", "Destination width or height is <= 0");
	}
	BilinearFilter<RealType> filter;


	// allocate the dst array
	ArrayType dst(dst_width, dst_height);

	// decide which filtering order (xy or yx) is faster for this mapping by
	// counting convolution multiplies
	if (dst_width*src_extent[1] <= dst_height*src_extent[0]) {
            // xy filtering

            // allocate a temporary image
            RealArrayType tmp(dst_width, src_extent[1]);

            // scale source image horizontally into temporary image
            detail::horizontalFilter(filter, src, tmp, detail::ValueUnbounded());

            // scale temporary image vertically into result image    
            detail::verticalFilter(filter, tmp, dst, bound);

	} else {
            // yx filtering

            // allocate a temporary image
            RealArrayType tmp(src_extent[0], dst_height);

            // scale source image vertically into temporary image
            detail::verticalFilter(filter, src, tmp, detail::ValueUnbounded());

            // scale temporary image horizontally into result image    
            detail::horizontalFilter(filter, tmp, dst, bound);
	}
        return dst;
    } // }}}
    template<class ElementType>
    blitz::Array<ElementType,2> rescale(
            const blitz::Array<ElementType,2>& src, 
            const int dst_width, const int dst_height)
    {
        typedef typename blitz::ExtNumericTraits<ElementType>::T_floattype     RealElementType;
        return rescale(src, dst_width, dst_height, detail::ValueBounded<RealElementType>(0.0,255.0));
    }
    template<class ElementType, class BoundType>
    blitz::Array<ElementType,2> rescale(
            const blitz::Array<ElementType,2>& src, 
            const blitz::TinyVector<int,2> size,
            const BoundType& bound) 
    {
        return rescale(src,size[0],size[1],bound);
    }
    template<class ElementType>
    blitz::Array<ElementType,2> rescale(
            const blitz::Array<ElementType,2>& src, 
            const blitz::TinyVector<int,2> size)
    {
        return rescale(src,size[0],size[1]);
    }

    /** Use bilinear interpolation. Supports only downscaling an image
     */
    template<class ElementType, class RealType_>
    blitz::Array<ElementType,2> fastrescale(
        blitz::Array<ElementType,2>& src, const RealType_ scale) 
    {// {{{
        if (std::abs(scale-1)<1e-8)
            return src;// scale of 1
        typedef typename blitz::ExtNumericTraits<ElementType>::T_basictype     BasicType;
        typedef typename blitz::ExtNumericTraits<BasicType  >::T_floattype     RealType;

        typedef blitz::TinyVector<int,2>        IndexType;
        typedef blitz::TinyVector<RealType,2>   Real2DType;
        IndexType in_ex=src.extent();
        IndexType out_ex(in_ex*scale); 

        blitz::Array<ElementType,2> dest(out_ex);
#ifdef _SPLIT_LOOP_
        for (int i= 0; i< out_ex[0]; ++i) {
            RealType x=i/scale;
            int fx=(int)x;
            RealType ux(x-fx), lx(1-ux);

            for (int j= 0; j< out_ex[1]; ++j) {
                RealType y=j/scale;
                int fy=(int)y;
                RealType uy(y-fy), ly(1-uy);

                dest(i,j)=
                    src(fx  ,fy  )*lx*ly 
                    +  src(fx+1,fy  )*ux*ly 
                    +  src(fx  ,fy+1)*lx*uy 
                    +  src(fx+1,fy+1)*ux*uy ;
            }
        }
#else
        for (int i= 0; i< out_ex[0]; ++i)
        for (int j= 0; j< out_ex[1]; ++j) {
            Real2DType xy(i/scale,j/scale);
            IndexType fxy(xy);
            Real2DType uxy(xy-fxy); Real2DType lxy(1-uxy);
            dest(i,j)=
                src(fxy[0]  ,fxy[1]  )*lxy[0]*lxy[1] 
                +  src(fxy[0]+1,fxy[1]  )*uxy[0]*lxy[1] 
                +  src(fxy[0]  ,fxy[1]+1)*lxy[0]*uxy[1] 
                +  src(fxy[0]+1,fxy[1]+1)*uxy[0]*uxy[1] ;
        }
#endif
        return dest;
    }// }}}
}

#endif // _LEAR_RESCALE_H_
