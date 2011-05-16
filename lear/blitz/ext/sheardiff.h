#ifndef _LEAR_SHEAR_DIFF_OPS_H_
#define _LEAR_SHEAR_DIFF_OPS_H_

#include <blitz/array.h>
#include <blitz/tinyvec.h>
#include <blitz/array/stencil-et.h>

BZ_NAMESPACE(blitz)
    
BZ_DECLARE_STENCIL3(diffTime,R,A,B)
    R(0,0) = A(0,0) - B(0,0);
BZ_END_STENCIL_WITH_SHAPE(shape(0,0),shape(0,0))

BZ_DECLARE_STENCIL3(diffTop,R,A,B)
    R(0,0) = A(0,0) - B(0,-1);
BZ_END_STENCIL_WITH_SHAPE(shape(0,-1),shape(0,0))

BZ_DECLARE_STENCIL3(diffBottom,R,A,B)
    R(0,0) = A(0,0) - B(0,+1);
BZ_END_STENCIL_WITH_SHAPE(shape(0,0),shape(0,1))

BZ_DECLARE_STENCIL3(diffLeft,R,A,B)
    R(0,0) = A(0,0) - B(-1,0);
BZ_END_STENCIL_WITH_SHAPE(shape(-1,0),shape(0,0))

BZ_DECLARE_STENCIL3(diffRight,R,A,B)
    R(0,0) = A(0,0) - B(1,0);
BZ_END_STENCIL_WITH_SHAPE(shape(0,0),shape(1,0))

BZ_DECLARE_STENCIL3(diffTopLeft,R,A,B)
    R(0,0) = A(0,0) - B(-1,-1);
BZ_END_STENCIL_WITH_SHAPE(shape(-1,-1),shape(0,0))

BZ_DECLARE_STENCIL3(diffTopRight,R,A,B)
    R(0,0) = A(0,0) - B(1,-1);
BZ_END_STENCIL_WITH_SHAPE(shape(0,-1),shape(1,0))

BZ_DECLARE_STENCIL3(diffBottomRight,R,A,B)
    R(0,0) = A(0,0) - B(1,1);
BZ_END_STENCIL_WITH_SHAPE(shape(0,0),shape(1,1))

BZ_DECLARE_STENCIL3(diffBottomLeft,R,A,B)
    R(0,0) = A(0,0) - B(-1,1);
BZ_END_STENCIL_WITH_SHAPE(shape(-1,0),shape(0,1))

BZ_NAMESPACE_END
#endif	//#ifndef _LEAR_SHEAR_DIFF_OPS_H_
