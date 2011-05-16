/*
 * =====================================================================================
 * 
 *       Filename:  interpolateposition.h
 * 
 *    Description:  Compute interpolated positions. Given a coarse estimate,
 *    find the fine floating point precision location.
 * 
 * =====================================================================================
 */

#include <blitz/array.h>
#include <blitz/tinyvec.h>
#include <blitz/tinymat.h>
#include <lear/numericutil/gauss.h>
#include <lear/blitz/ext/convolve.h>
#include <lear/numericutil/linalg.h>
#include <lear/blitz/ext/stencilop.h>

namespace lear {
    /**
     * Used to compute location of maximums in N-space with floating point
     * precision.
     */
    template<class RealType, int N>
    struct InterpolatePosition {
    };

    template<class RealType_>
    struct InterpolatePosition<RealType_,3> {
        enum {N=3};
        typedef RealType_                       RealType;
        typedef blitz::TinyVector<int,N>        IndexType;
        typedef blitz::TinyVector<RealType,N>   PointType;
        typedef blitz::TinyMatrix<RealType,N,N> MatrixType;
        typedef blitz::Array<RealType,N>        ArrayType;

        InterpolatePosition(const ArrayType& array) :
            array(array) 
        {}

        bool operator() (const IndexType index, PointType& newpoint, RealType& score) const {
            PointType grad; MatrixType hessian;

            using namespace blitz;
            blitz::RectDomain<N> W(index-WIN,index+WIN);
            IndexType extent(2*WIN+1);
            ArrayType cropped(array(W));

            ArrayType dervX(extent), dervY(extent);
            //ArrayType dervZ(extent);

            // Evaluate dervX and dervY operator.
            // use custom central operators because blitz crashes in 
            // central12 function if PixelType is TinyVector.
            applyStencil(CentralDerv1Order2(firstDim), dervX, cropped);
            applyStencil(CentralDerv1Order2(secondDim),dervY, cropped);
            //applyStencil(CentralDerv1Order2(thirdDim), dervZ, cropped);

            IndexType ip, in;
            for (int i= 0; i< N; ++i) {
                ip = WIN; in = WIN; --ip[i]; ++in[i];
                hessian(i,i) = cropped(in) + cropped(ip) -2*cropped(WIN);
            }
            /*
            ip = WIN; in = WIN; --ip[1]; ++in[1];
            hessian(0,1) = dervX(in) - dervX(ip);

            ip = WIN; in = WIN; --ip[0]; ++in[0];
            hessian(1,0) = dervY(in) - dervY(ip);

            ip = WIN; in = WIN; --ip[2]; ++in[2];
            hessian(0,2) = dervX(in) - dervX(ip);

            ip = WIN; in = WIN; --ip[0]; ++in[0];
            hessian(2,0) = dervZ(in) - dervZ(ip);

            ip = WIN; in = WIN; --ip[2]; ++in[2];
            hessian(1,2) = dervY(in) - dervY(ip);

            ip = WIN; in = WIN; --ip[1]; ++in[1];
            hessian(2,1) = dervZ(in) - dervZ(ip);
            */
            ip = WIN; in = WIN; --ip[0]; ++in[0];
            hessian(0,1) = hessian(1,0) = (dervY(in) - dervY(ip));

            ip = WIN; in = WIN; --ip[2]; ++in[2];
            hessian(0,2) = hessian(2,0) = (dervX(in) - dervX(ip));

            ip = WIN; in = WIN; --ip[2]; ++in[2];
            hessian(1,2) = hessian(2,1) = (dervY(in) - dervY(ip));

            ip = WIN;// use center element
            grad[0] = dervX(ip); grad[1] = dervY(ip); 
            ip = WIN; in = WIN; --ip[2]; ++in[2];
            grad[2] = cropped(in) - cropped(ip);

            newpoint=product(numericutil::invert(hessian),grad);
            RealType s = dot(newpoint,grad)/2;
            score +=s; newpoint+=index;
            if (s<0)
                return false;
            else {
                //std::cout << "Hessian " << hessian << std::endl;
                //std::cout << "Grad " << grad << std::endl;
                return true;
            }
        }
        bool inbound(IndexType index) const {
            return !(blitz::sum(index < WIN) || 
                     blitz::sum(index > array.ubound()-WIN)) ;
        }

        private:
            const ArrayType& array;
            static const int WIN=2;
    };
}

