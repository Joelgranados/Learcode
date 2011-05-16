// {{{
/**
 * @file
 * @brief Warp an image using homography provided.
 */
// }}}

#ifndef _LEAR_WARPIMAGE_H__
#define _LEAR_WARPIMAGE_H__

// {{{ headers
#include <iostream>
#include <blitz/array.h>
#include <blitz/tinyvec.h>
#include <blitz/tinymat.h>

#include <blitz/tinyvec-et.h>

#include <lear/blitz/ext/tvmutil.h>
// }}}

namespace lear {

// {{{ warp Image 

/** 
 * Applies 2D Projective transformation to an image and returns the transformed
 * image. Since, a projective tranformation can stretch an image arbitrairly, 
 * returned image size is limited by extent.
 *
 * If extent is 0, returned image is resized such that no pixel of returned 
 * mage goes out of bound. 
 * If no extent is specified, return image is of same size as input image
 * Bilinear interpolation is used to interpolate image.
 *
 * @todo warpImage will not work well with images represented as int or unsigned int.
 * Do we really need it?
 *
 */
template<class PixelType, class RealType> inline
blitz::Array<PixelType,2> warpImage ( const blitz::Array<PixelType,2>& in, 
        const blitz::TinyMatrix<RealType,3,3>& homography, 
        blitz::TinyVector<int,2> extent = -1,
        blitz::TinyVector<RealType,2> translate = 0, bool centerit=false)
{
    using namespace blitz;
    typedef Array<PixelType,2> Image;
    typedef TinyMatrix<RealType,3,3> Homography;
    typedef TinyVector<RealType,3> Point;
    typedef TinyVector<int,2> IndexType;

    IndexType inLbound = in.lbound();
    IndexType inUbound = in.ubound();
    IndexType inExtent = in.extent();

    IndexType outLbound = inLbound;
    IndexType outExtent = inExtent;
    IndexType outUbound = inUbound;

    if (isZero(extent) || centerit) {
        Array<Point,1> endpts(4);
        endpts(0) = product(homography, Point(inLbound[0], inLbound[1], 1));
        endpts(1) = product(homography, Point(inUbound[0], inLbound[1], 1));
        endpts(2) = product(homography, Point(inUbound[0], inUbound[1], 1));
        endpts(3) = product(homography, Point(inLbound[0], inUbound[1], 1));

        endpts[0] /= endpts[2];
        endpts[1] /= endpts[2];

        TinyVector<RealType,2> pt;
        pt[0] = min(endpts[0]); pt[1] = min(endpts[1]);
        outLbound = floor(pt);

        pt[0] = max(endpts[0]); pt[1] = max(endpts[1]);
        outUbound  = ceil(pt);

        outExtent = (outUbound-outLbound + 1);
        if (centerit) {
            if (sum(extent != -1)==2) {
                translate = extent/2.0 - 0.5f;

                outLbound = 0;
                outExtent = extent;
                outUbound = extent-1;
                /*
                outLbound += (outExtent- extent)/2;
                outUbound -= (outExtent- extent)/2;
                outExtent = (outUbound-outLbound + 1);
                */
            }
        }
    } else if (sum(extent != -1)==2) {
        outLbound = 0;
        outExtent = extent;
        outUbound = extent-1;
    }
    // output image
    Image out(outLbound , outExtent); 

    // temporary variable. Used to store contribution
    Array<RealType,2> contribs(outLbound, outExtent);

    out = 0.0;
    contribs = 0;

    RealType x, y;
    for (int i=inLbound(0); i <= inUbound(0); ++i) {
        for (int j=inLbound(1); j <= inUbound(1); ++j) {
            if (isZero(in(i,j)))
                continue;

            Point pt = product(homography, Point(i,j,1) );
            x = pt(0)/pt(2) + translate[0];
            y = pt(1)/pt(2) + translate[1];
            int  x1 = (int) x;
            int  y1 = (int) y;
            if (x >= outLbound(0) && x <= outUbound(0) &&
                y >= outLbound(1) && y <= outUbound(1)) 
            {

                PixelType color = in(i,j);
                // interpolate bilinear
                RealType u = x-x1;
                RealType v = y-y1;
                
                RealType compU = 1-u;
                RealType compV = 1-v;
                
                RealType w;
                
                w = compU*compV;
                contribs(x1  , y1  ) += w;
                out     (x1  , y1  ) += color*w;
                
                if (x1 < outUbound(0)) {
                    w = u*compV;
                    contribs(x1+1, y1  ) += w;
                    out     (x1+1, y1  ) += color*w;
                }
                
                if (x1 < outUbound(0) && y1 < outUbound(1) ) {
                    w = u*v;
                    contribs(x1+1, y1+1) += w;
                    out     (x1+1, y1+1) += color*w;
                }
                
                if (y1 < outUbound(1) ) {
                    w = compU*v;
                    contribs(x1  , y1+1) += w;
                    out     (x1  , y1+1) += color*w;
                }
            }
        }
    }

    for (int i=outLbound(0); i <= outUbound(0); ++i) {
        for (int j=outLbound(1); j <= outUbound(1); ++j) {
            if (contribs(i,j) > 1e-10)
                out(i,j) /= contribs(i,j);
        }
    }
    return out;
} 
// }}}

} // lear

#endif // _LEAR_WARPIMAGE_H__
