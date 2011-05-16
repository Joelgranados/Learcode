#ifndef _LEAR_EDGES_H_
#define _LEAR_EDGES_H_
// {{{ headers
#if TIMER > 2
#include <boost/timer.hpp>
#include <iostream>
#include <iomanip>
#endif

#include <cmath>
#include <blitz/array.h>
#include <blitz/tinyvec.h>
#include <blitz/tinyvec-et.h>
#include <blitz/array/stencil-et.h>

#include <lear/blitz/blitzutil.h>
#include <lear/image/colorconversion.h>

#include <lear/blitz/ext/stdfuncs.h>
#include <lear/blitz/ext/convolve.h>
#include <lear/blitz/ext/stencilop.h>
#include <lear/blitz/ext/numtrait.h>
#include <lear/blitz/ext/domainiter.h>

#include <lear/util/lookup.h>
#include <lear/numericutil/gauss.h>
//#include <lear/image/imageio.h>
// }}}

namespace lear {
// {{{ SuppressNonMaxEdge
/**
 * Code adapted from Vigra's Image processing library.
 */
namespace detail {

#define SUPPRESS_NON_MAX_EDGE_END   \
template<int N>                     \
void getExtent(                     \
    blitz::TinyVector<int,N>& minb, \
    blitz::TinyVector<int,N>& maxb) \
const \
{                                   \
    minb = -1;                      \
    maxb = 1;                       \
}                                   \
enum { hasExtent = 1 }; 

template <class RealType_>
struct SuppressNonMaxEdge_usingDxDy {// {{{
    typedef RealType_                       RealType;
    typedef RealType_                       PixelType;
    typedef blitz::Array<PixelType,2>       ImageType;
    typedef blitz::TinyVector<int,2>        IndexType;

    static ImageType apply(const ImageType& mag, const ImageType& dX, const ImageType& dY) {
        const PixelType zero = 0;
        const PixelType tan22_5 = static_cast<PixelType> (0.4142135623730951); // sqrt(2) - 1
        
        blitz::Range all = blitz::Range::all();

        IndexType ub = mag.ubound();
        IndexType lb = mag.lbound();

        ImageType res(lb,mag.extent());
        res = -1;
        // copy boundaries
        res(all,lb(1)) = 0; res(all,ub(1)) = 0;
        res(lb(0),all) = 0; res(ub(0),all) = 0;

        lb+=1; ub-=1;
        for (int i= lb[0]; i<= ub[0]; ++i) 
        for (int j= lb[1]; j<= ub[1]; ++j) {
            const RealType m = mag(i,j);
            const RealType dx = dX(i,j);
            const RealType dy = dY(i,j);
            RealType& r = res(i,j);

            r = zero;
            if (m) {
                // find out quadrant
                if(std::abs(dy) < tan22_5*std::abs(dx)) { // north-south edge
                    if(mag(i-1,  j) < m && mag(i+1,  j) < m) 
                        r = m;
                } else if(std::abs(dx) < tan22_5*std::abs(dy)) { // west-east edge
                    if(mag(  i,j-1) < m && mag(  i,j+1) < m) 
                        r = m;
                } else if(dx*dy < zero) { // south-east edge
                    if(mag(i+1,j-1) < m && mag(i-1,j+1) < m) 
                        r = m;
                } else if(dx*dy > zero){ // south-west edge
                    if(mag(i-1,j-1) < m && mag(i+1,j+1) < m) 
                        r = m;
                } else {
                        r = m;
                }
            }
        }
        if (min(res) < 0) std::cerr << "Min <0" << std::endl;
        return res;
    }
};// }}}
template <class RealType_, int M>
struct SuppressNonMaxEdge_usingDxDy<blitz::TinyVector<RealType_, M> > {// {{{

    typedef RealType_                       RealType;
    typedef blitz::TinyVector<RealType,M>   PixelType; 
    typedef blitz::Array<PixelType,2>       ImageType;
    typedef blitz::TinyVector<int,2>        IndexType;

    static ImageType apply(const ImageType& mag, const ImageType& dX, const ImageType& dY) {
        const PixelType zero = 0;
        const RealType tan22_5 = static_cast<RealType> (0.4142135623730951); // sqrt(2) - 1
        
        blitz::Range all = blitz::Range::all();

        IndexType ub = mag.ubound();
        IndexType lb = mag.lbound();

        ImageType res(lb,mag.extent());
        // copy boundaries
        res(all,lb(1)) = zero; res(all,ub(1)) = zero;
        res(lb(0),all) = zero; res(ub(0),all) = zero;

        lb+=1; ub-=1;
        for (int i= lb[0]; i<= ub[0]; ++i) 
        for (int j= lb[1]; j<= ub[1]; ++j) {
            res(i,j) = zero;
            for (int k= 0; k<M ; ++k) {
                const RealType m = mag(i,j)[k];
                const RealType dx = dX(i,j)[k];
                const RealType dy = dY(i,j)[k];
                RealType& r = res(i,j)[k];

                if (m) {
                    // find out quadrant
                    if(std::abs(dy) < tan22_5*std::abs(dx)) { // north-south edge
                        if(mag(i-1,  j)[k] < m && mag(i+1,  j)[k] < m) 
                            r = m;
                    } else if(std::abs(dx) < tan22_5*std::abs(dy)) { // west-east edge
                        if(mag(  i,j-1)[k] < m && mag(  i,j+1)[k] < m) 
                            r = m;
                    } else if(dx*dy < 0) { // south-east edge
                        if(mag(i+1,j-1)[k] < m && mag(i-1,j+1)[k] < m) 
                            r = m;
                    } else if(dx*dy > 0){ // south-west edge
                        if(mag(i-1,j-1)[k] < m && mag(i+1,j+1)[k] < m) 
                            r = m;
                    } else {
                            r = m;
                    }
                }
            }
        }
        // copy boundaries
        return res;
    }
};// }}}

template <class RealType>
struct SuppressNonMaxEdge_usingAngle {// {{{
    template<class T1, class T2, class T3, class T4, class T5, class T6, 
        class T7, class T8, class T9, class T10, class T11>         
    inline void apply(T1& r, T2& m, T3& o, T4, T5, T6, T7, T8, T9, T10, T11)  const
    {
        const RealType zero = 0;
        const RealType tan22_5 = MathConst<RealType>::PI_Value/8;
        const RealType tan67_5 = 3*tan22_5;
        
        if (m(0,0)) {
            // find out quadrant
            if(std::abs(o) < tan22_5) { // north-south edge
                if(m(-1, 0) < m && m( 1, 0) < m) 
                    r(0,0) = m(0,0);
            } else if(std::abs(o) > tan67_5) { // west-east edge
                if(m( 0,-1) < m && m( 0, 1) < m) 
                    r(0,0) = m(0,0);
            } else if(o < -tan22_5) { // south-east edge
                if(m( 1,-1) < m && m(-1, 1) < m) 
                    r(0,0) = m(0,0);
            } else if(o > tan22_5){ // south-west edge
                if(m(-1,-1) < m && m( 1, 1) < m) 
                    r(0,0) = m(0,0);
            } else {
                    r(0,0) = zero;
            }
        } else {
                r(0,0) = zero;
        }
    }
    SUPPRESS_NON_MAX_EDGE_END   
};// }}}
#undef SUPPRESS_NON_MAX_EDGE_END   
} // end namespace detail
// }}}

struct AnglePolicy_atan {// {{{ 
    template< class T>
        T operator()(const T y, const T x)const {
            return atan(y/x);
        }
    template< class T, int N>
        void operator()(
                const blitz::Array<T,N>& y,
                const blitz::Array<T,N>& x, 
                blitz::Array<typename blitz::ExtNumericTraits<T>::T_floattype,N>& r) const 
        {
            r=blitz::atan(y/x);
        }
//      BZ_DECLARE_FUNCTOR(AnglePolicy_atan)
};// }}}
struct AnglePolicy_atan2 {// {{{
    template< class T>
        T operator()(const T y, const T x)const {
            return atan2(y,x);
        }
    template< class T, int N>
        void operator()(
                const blitz::Array<T,N>& y,
                const blitz::Array<T,N>& x, 
                blitz::Array<typename blitz::ExtNumericTraits<T>::T_floattype,N>& r) const 
        {
            r=blitz::atan2(y,x);
        }
//      BZ_DECLARE_FUNCTOR(AnglePolicy_atan2)
};// }}}

// {{{ hysthresh
/** Compute hysteresis threshold of an image
 * Original idea is from Canny. The principle is very simple:
 * for all pixles > threshold High,
 *      add this pixel to a stack
 * for all pixels on stack
 *      if connected pixels > threshold Low
 *          add this pixel to stack and mark as edge
 * @param res       thresholded image, must be same size as input image
 * @param inp      input image
 * @param tHi      threshold high
 * @param tLo      threshold Low
 *
 * @warning inp parameter should not have any boundry pixel greater
 * than tLo. A if condition on bounds is avoided for speed improvement
 */
template<class BoolType, class PixelType>
blitz::Array<BoolType,2>& hysthreshold(
        blitz::Array<BoolType,2>& res,
        const blitz::Array<PixelType,2>& inp, 
        const PixelType tHi, 
        const PixelType tLo) // = PixelType(0.4*tHi)
{
    using namespace blitz;
    typedef Array<PixelType,2>      ArrayType;
    typedef TinyVector<int,2>       IndexType;

    int                 stptr = -1;
    Array<IndexType,1>  stack(product(inp.extent()));

    //increment to avoid testing boundaries
    const IndexType lbound ( inp.lbound()+1);
    //decrement to avoid testing boundaries
    const IndexType ubound ( inp.ubound()-1);
    
    res = false;
    for (DomainIter<2> i(lbound,ubound); i; ++i) {
        if (inp(*i) > tHi) {
            stack(++stptr) = *i;
            res(*i) = true;
        } 
    }
    while (stptr >=0) {
        IndexType l(stack(stptr)-1);
        IndexType u(stack(stptr)+1);
        --stptr;

        for (DomainIter<2> j(l,u); j; ++j) {
            if (inp(*j) > tLo && !res(*j)) {
                res(*j) = true;
                stack(++stptr) = *j;
            }
        }
    }
    return res;
}
template< class PixelType>
blitz::Array<bool,2> hysthreshold(
        const blitz::Array<PixelType,2>& inp, 
        const PixelType tHi, 
        const PixelType tLo) // = PixelType(0.4*tHi)
{
    blitz::Array<bool,2> res(inp.lbound(),inp.extent());
    return hysthreshold(res,inp,tHi,tLo);
}
// }}}

// {{{ gradientXY
/** Computes the xy gradient of an image, using the kernel provided.
 * Return value is std::pair, with 
 * pair::first      gradient X, and 
 * pair::second     gradient Y 
 */
template<class ConvMethod, class PixelType, class RealType>
inline std::pair<blitz::Array<PixelType,2>, blitz::Array<PixelType,2> > 
gradientXY(
    const blitz::Array<PixelType,2>& signal,
    const blitz::Array<RealType,1>& smoothKernel)
{
    using namespace blitz;
    typedef TinyVector<int,2>   IndexType;
    typedef Array<PixelType,2>  ImageType;

    // storage for smoothed signals
    const IndexType lbound =signal.lbound();
    const IndexType ubound =signal.ubound();
    const IndexType extent =signal.extent();

    ImageType dervX(lbound,extent), dervY(lbound,extent), mag(lbound,extent);

    // use mag, dervX and dervY as temporaries for a while
    ConvMethod conv;

    // Input signal smoothed in X direction
    conv.dim1(signal, smoothKernel, dervX); 
    // Input signal smoothed in Y direction
    conv.dim2(dervX, smoothKernel, mag);

    // Evaluate dervX and dervY operator.
    // use custom central operators because blitz crashes in 
    // central12 function if PixelType is TinyVector.
    applyStencil(CentralDerv1Order2(firstDim), dervX, mag);
    applyStencil(CentralDerv1Order2(secondDim),dervY, mag);

    // copy boundaries
    Range all = Range::all();
    dervY(all,lbound(1)) = dervY(all,lbound(1)+1);
    dervY(all,ubound(1)) = dervY(all,ubound(1)-1);

    // copy boundaries
    dervX(lbound(0),all) = dervX(lbound(0)+1, all);
    dervX(ubound(0),all) = dervX(ubound(0)-1, all);

    return std::make_pair(dervX, dervY);;
}
template< class PixelType, class RealType>
inline std::pair<blitz::Array<PixelType,2>, blitz::Array<PixelType,2> > 
gradientXY(
    const blitz::Array<PixelType,2>& signal,
    const blitz::Array<RealType,1>& smoothKernel)
{
    using namespace blitz;
    return gradientXY<ConvolveExact<CPolicy_Stretch> >(signal,smoothKernel);
}
// }}}
// {{{ gradient
/** Computes the gradient of an image, using the kernel provided.
 * Return value is std::pair, with 
 * pair::first      gradient magnitude, and 
 * pair::second     gradient angle (in radians)
 *
 * Range of gradient angle policy can be specified using AnglePolicy.
 * Two possible functions provided for AnglePolicy
 * AnglePolicy_atan         - use std::atan to estimate angle, thus range +/-PI/2
 * AnglePolicy_atan2        - use std::atan2 to estimate angle, thus range +/-PI
 *
 * Default AnglePolicy is AnglePolicy_atan. User can also implement his
 * own AnglePolicy for e.g. to convert Angle's from radians to degrees.
 */
template<class PixelType, class RealType, class AnglePolicy> inline
std::pair<blitz::Array<PixelType,2>, blitz::Array<PixelType,2> > gradient(
    const blitz::Array<PixelType,2>& signal,
    const blitz::Array<RealType,1>& smoothKernel,
    const AnglePolicy angle = AnglePolicy_atan())
{
    using namespace blitz;
    typedef Array<PixelType,2>  ImageType;

    std::pair<ImageType,ImageType> dervXY = gradientXY(signal,smoothKernel);
    // create temporary references
    ImageType& dervX = dervXY.first; ImageType& dervY = dervXY.second; 

    ImageType mag(signal.lbound(),signal.extent()); // magnitude of derv
    // compute magnitude and orientations
    mag = sqrt(dervX*dervX+dervY*dervY);
    angle(dervY,dervX, dervX);// dervX now has angle values
    return std::make_pair(mag, dervXY.first);
}
// }}}
// {{{ gradient
/** Computes the gradient of an image, using the scale provided.
 * Return value is std::pair, with 
 * pair::first      gradient magnitude, and 
 * pair::second     gradient angle (in radians)
 *
 * Range of gradient angle policy can be specified using AnglePolicy.
 * Two possible functions provided for AnglePolicy
 * AnglePolicy_atan         - use std::atan to estimate angle, thus range +/-PI/2
 * AnglePolicy_atan2        - use std::atan2 to estimate angle, thus range +/-PI
 *
 * Default AnglePolicy is AnglePolicy_atan. User can also implement his
 * own AnglePolicy for e.g. to convert Angle's from radians to degrees.
 */
template<class PixelType, class RealType, class AnglePolicy> inline
std::pair<blitz::Array<PixelType,2>, blitz::Array<PixelType,2> > gradient(
    const blitz::Array<PixelType,2>& signal,
    const RealType scale,
    const AnglePolicy angle = AnglePolicy_atan(),
    const int support = Gauss::KERNEL_SUPPORT)
{
    // 1D Gaussian kernel of specified scale
    blitz::Array<RealType,1> smoothKernel = 
        Gauss::discrete(scale,support);

    std::pair<blitz::Array<PixelType,2>, blitz::Array<PixelType,2> > g = 
                                gradient(signal,smoothKernel,angle);
    g.first *= scale; 

    return g;
}
template<class PixelType, class RealType> inline
std::pair<blitz::Array<PixelType,2>, blitz::Array<PixelType,2> > gradient(
    const blitz::Array<PixelType,2>& signal,
    const RealType scale,
    const int support = Gauss::KERNEL_SUPPORT)
{
    return gradient(signal, scale, AnglePolicy_atan(), support);
}
// }}}

// {{{ nosmoothgradientXY
/** Computes the gradient of an image, using the kernel provided.
 * Return value is std::pair, with 
 * pair::first      gradient X, and 
 * pair::second     gradient Y
 */
template<class PixelType> inline
std::pair<
    blitz::Array<typename blitz::ExtNumericTraits<PixelType>::T_difftype,2>, 
    blitz::Array<typename blitz::ExtNumericTraits<PixelType>::T_difftype,2>      
    > 
    nosmoothgradientXY(const blitz::Array<PixelType,2>& signal)
{
    using namespace blitz;
    typedef TinyVector<int,2>   IndexType;
    typedef Array<PixelType,2>  ImageType;
    typedef Array<typename blitz::ExtNumericTraits<PixelType>::T_difftype,2>  
                                RImageType;

    // storage for smoothed signals
    const IndexType lbound =signal.lbound();
    const IndexType ubound =signal.ubound();
    const IndexType extent =signal.extent();

    RImageType dervX(lbound,extent);
    RImageType dervY(lbound,extent);

    // use custom central operators because blitz crashes in 
    // central12 function if PixelType is TinyVector.
    applyStencil(CentralDerv1Order2(firstDim), dervX, signal);
    applyStencil(CentralDerv1Order2(secondDim),dervY, signal);

    // copy boundaries
    Range all = Range::all();
    dervY(all,lbound(1)) = dervY(all,lbound(1)+1);
    dervY(all,ubound(1)) = dervY(all,ubound(1)-1);

    // copy boundaries
    dervX(lbound(0),all) = dervX(lbound(0)+1, all);
    dervX(ubound(0),all) = dervX(ubound(0)-1, all);

    return std::make_pair(dervX,dervY);
}
// }}}
// {{{ nosmoothgradient
/** Computes the gradient of an image, using the kernel provided.
 * Return value is std::pair, with 
 * pair::first      gradient magnitude, and 
 * pair::second     gradient angle (in radians)
 *
 * Range of gradient angle policy can be specified using AnglePolicy.
 * Two possible functions provided for AnglePolicy
 * AnglePolicy_atan         - use std::atan to estimate angle, thus range +/-PI/2
 * AnglePolicy_atan2        - use std::atan2 to estimate angle, thus range +/-PI
 *
 * Default AnglePolicy is AnglePolicy_atan. User can also implement his
 * own AnglePolicy for e.g. to convert Angle's from radians to degrees.
 */
template<class PixelType, class AnglePolicy> inline
std::pair<
    blitz::Array<typename blitz::ExtNumericTraits<PixelType>::T_floattype,2>,
    blitz::Array<typename blitz::ExtNumericTraits<PixelType>::T_floattype,2>  
    > 
    nosmoothgradient( 
        const blitz::Array<PixelType,2>& signal,
        const AnglePolicy angle = AnglePolicy_atan())
{
    using namespace blitz;
    typedef Array<PixelType,2>  ImageType;
    typedef Array<typename blitz::ExtNumericTraits<PixelType>::T_difftype,2>  
                                DImageType;
    typedef Array<typename blitz::ExtNumericTraits<PixelType>::T_floattype,2>  
                                RImageType;

    std::pair<DImageType,DImageType> dervXY = nosmoothgradientXY(signal);
    // create temporary references
    DImageType& dervX = dervXY.first; DImageType& dervY = dervXY.second; 

    RImageType mag(signal.lbound(),signal.extent()); // magnitude of derv
    // compute magnitude and orientations
    mag = sqrt(dervX*dervX+dervY*dervY);
    RImageType ang(signal.lbound(),signal.extent());
    angle(dervY,dervX, ang);// dervX now has angle values
//      for (DomainIter<2> i (lbound, ubound); i; ++i) {
//          mag(*i) = sqrt(dervX(*i)*dervX(*i)+dervY(*i)*dervY(*i));
//          dervX(*i) = angle(dervY(*i),dervX(*i));
//      }

    return std::make_pair(mag, ang);
}
// }}}

// {{{ nonmaxSuppress
/** NonMaximum Suppression of gradient magnitude.
 * Normally, output result of gradient should be passed as input 
 * paramteters to gradient magnitude. 
 *
 * @param mag       Gradient magnitude array
 * @param ori       Gradient angle (must be bewteen +- PI/2 radians)
 */
template<class PixelType> inline
blitz::Array<PixelType,2> nonmaxSuppress(
    const blitz::Array<PixelType,2>& mag,
    const blitz::Array<PixelType,2>& ori)
{
    typedef blitz::TinyVector<int,2>   IndexType;

    const IndexType lbound =mag.lbound();
    const IndexType ubound =mag.ubound();
    const IndexType extent =mag.extent();

    blitz::Array<PixelType,2> edge (lbound, extent); 
    applyStencil(detail::SuppressNonMaxEdge_usingAngle<PixelType>(), 
            edge, mag, ori);

    // copy boundaries
    blitz::Range all = blitz::Range::all();
    edge(all,lbound(1)) = 0;
    edge(all,ubound(1)) = 0;

    edge(lbound(0),all) = 0;
    edge(ubound(0),all) = 0;

    return edge;
}
// }}}

}

#endif // #ifndef _LEAR_EDGES_H_
