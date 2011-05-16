// {{{ file documentation
/**
 * @file
 * @brief Gauss kernel utilities.
 */
// }}}

#ifndef _LEAR_GAUSS_H__
#define _LEAR_GAUSS_H__

// {{{ headers
#include <blitz/array.h>
#include <boost/math/special_functions/bessel.hpp>
// }}}

namespace lear {

struct Gauss { 
    /**
    * Kernel support size for Gauss Kernels.
    *
    * DiscreteGauss uses Gauss::KERNEL_SUPPORT as default support size of 
    * all its kernel.
    */
    static const int KERNEL_SUPPORT = 3;

    /**
    * Creates a centered Gauss Kernel using exponential.
    */
    template<class T>
    static blitz::Array<T,1> conventional(
            const T stdDeviation, const int support = KERNEL_SUPPORT) 
    {
        using namespace blitz;
        T variance = stdDeviation*stdDeviation;
        int dim = static_cast<int>(std::ceil(stdDeviation*support));

        // Define discrete Gauss kernel centered at 0
        Array<T,1> gauss(Range(-dim,dim));
        // initialize first half
        gauss(Range(-dim,0)) = exp(-tensor::i*tensor::i/variance);

        // initialize second half -- using symmetric property of Gaussians
        if (dim > 0)
            gauss(Range(1,dim,1)) = gauss(Range(-1,-dim,-1));

        T s = sum(gauss);
        gauss/=s;
        return gauss;
    }

    /**
    * Creates a centered Discrete Gauss Kernel.
    *
    * Scaled regular modified cylindrical Bessel function Bessel_In(n,var)
    * are used as argued by Tony Lindeberg in his Scale Space theory.
    */
    template<class T>
    static blitz::Array<T,1> discrete(
            const T stdDeviation, const int support = KERNEL_SUPPORT) 
    {
        using namespace blitz;
        T variance = stdDeviation*stdDeviation;
        int dim = static_cast<int>(std::ceil(stdDeviation*support));

        // Define discrete Gauss kernel centered at 0
        Array<T,1> gauss(Range(-dim,dim));
        // initialize first half
        T norm = exp(-variance);
        for (int i= 0; i<= dim; ++i) 
        {
            gauss(i) = norm*boost::math::cyl_bessel_i(i, variance);
        }
        // initialize second half -- using symmetric property of Gaussians
        for (int i= -dim; i< 0; ++i) 
        {
            gauss(i) = gauss(-i);
        }

        return gauss;
    }
};

} // lear

#endif // _LEAR_GAUSS_H__
