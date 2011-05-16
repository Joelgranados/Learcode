// {{{
/**
 * @file
 * @brief Rotate an image around its center
 */
// }}}

#ifndef _LEAR_ROTATE_H_
#define _LEAR_ROTATE_H_

// {{{ headers
#include <iostream>
#include <blitz/array.h>
#include <blitz/tinyvec.h>
#include <blitz/tinymat.h>

#include <blitz/tinyvec-et.h>

#include <lear/blitz/ext/tvmutil.h>
#include <lear/blitz/ext/domainiter.h>
// }}}

namespace lear {

/** 
 * Rotate a 2D image around its center. If \param resize is true, then resizes 
 * the image not to loose any pixel.  Bilinear interpolation is used to 
 * interpolate image.
 *
 * @param in        input image
 * @param angle     rotation angle (in radians)
 * @param resize    Set true to fit the output image exactly to rotated image
 * @param translate If resize is true, returns the translated value
 *
 * @todo rotateImage will not work well with images represented as int or unsigned int.
 * Do we really need it?
 *
 */
//   * Returned image size is limited by width X height (or if default argument is used, 
//   * output image is same size as input image).
template<class PixelType, class RealType> inline
blitz::Array<PixelType,2> rotate ( const blitz::Array<PixelType,2>& in, 
        const RealType angle, 
        const bool resize = false,
        blitz::TinyVector<int,2>* translate = 0)
{
    using namespace blitz;
    using namespace std;
    typedef TinyVector<int,2> IndexType;

    typedef Array<PixelType,2> Image;
    typedef TinyVector<RealType,2> Point;
    typedef TinyMatrix<RealType,2,2> Rotation;

    const IndexType inLbound = in.lbound();
    const IndexType inUbound = in.ubound();
    const IndexType inExtent = in.extent();

    const IndexType origin = inLbound + inExtent/2;

    Rotation rotate;
    rotate(0,0) = rotate(1,1) = std::cos(angle);
    rotate(1,0) = std::sin(angle);
    rotate(0,1) = -rotate(1,0);

    const IndexType outLbound = inLbound;
    IndexType outExtent = inExtent;
    blitz::TinyVector<int,2> t_translate = 0;
    if (!translate) translate = &t_translate;// set only if zero. we dont want a crash
    if (resize) {

        Array<Point,1> endpts(4);
        endpts(0) = product(rotate, Point(inLbound - origin) ) + origin;
        endpts(1) = product(rotate, Point(Point(inUbound[0],inLbound[1]) - origin) ) + origin;
        endpts(2) = product(rotate, Point(inUbound - origin) ) + origin;
        endpts(3) = product(rotate, Point(Point(inLbound[0],inUbound[1]) - origin) ) + origin;

        Point pt;
        pt[0] = min(endpts[0]); pt[1] = min(endpts[1]);
        *translate  = -floor(pt);

        pt[0] = max(endpts[0]); pt[1] = max(endpts[1]);
        outExtent  = ceil(pt) + *translate;
    }
    const IndexType outUbound = outLbound + outExtent -1;

    // output image
    Image out(outLbound , outExtent); 
    out = 0;

    // temporary variable. Used to store contribution
    Array<RealType,2> contribs(outLbound, outExtent);
    contribs = 0;

    const IndexType tu = outUbound -1;
    for (DomainIter<2> i(inLbound,inUbound); i; ++i) {
        if (isZero(in(*i)))
            continue;

        Point pt = product(rotate, Point(*i - origin) ) + origin + *translate;
        if (isInRange(pt, outLbound, tu)) {
            PixelType color = in(*i);
            // interpolate bilinear
            IndexType xy = pt;
            Point uv = pt - xy;
            Point compuv = 1-uv;

            RealType w;
            
            w = product(compuv);
            contribs(xy) += w;
            out     (xy) += color*w;
            
            w = uv[0]*compuv[1];
            ++xy[0];
            contribs(xy) += w;
            out     (xy) += color*w;
            
            w = product(uv);
            ++xy[1];
            contribs(xy) += w;
            out     (xy) += color*w;
            
            w = compuv[0]*uv[1];
            --xy[1];
            contribs(xy) += w;
            out     (xy) += color*w;
        }
    }

    for (DomainIter<2> i(outLbound,outUbound); i; ++i) {
        if (contribs(*i) !=0)
            out(*i) /= contribs(*i);
    }
    return out;
} 


} // lear

#endif // _LEAR_ROTATE_H_
