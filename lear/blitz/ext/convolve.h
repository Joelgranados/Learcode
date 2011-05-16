// {{{ Copyright notice
// }}}
// {{{ file documentation
/** 
 * @file
 * @brief Convolution operations for blitz 1D, 2D and 3D arrays.
 *
 * Defines convolution operations for blitz arrays. Currently,
 * 1D, 2D and 3D blitz arrays are supported.
 *
 * @warning Functions are defined using for loops and does not
 * use Expression Templates and Meta Programming techniques of
 * blitz arrays.
 */
// }}}

#ifndef _LEAR_CONVOLVE_H_
#define _LEAR_CONVOLVE_H_

// {{{ headers
#include <blitz/array.h>
#include <blitz/tinyvec.h>
#include <blitz/tinyvec-et.h>
// }}}

BZ_NAMESPACE(blitz)

// {{{ CPolicy_Mirror
/** 
 * @defgroup overloaded convolveMirrorExact functions
 * @brief Convolution operationeds limited to array boundaries only
 * (for example Quite handy for convolving images)
 * Array is mirrored accross boundaries. 
 *
 * Define convolution of two arrays. 
 * Array B and C are convolved with result in A. 
 * Array B is mirrored accross boundaries.  Return array has same size as array B.
 *
 * @param Array B (input Array)
 * @param Array C (convolution kernel)
 * @param Array A (convolution result)
 *
 * @warning Array C is assumed to be centered around 0. Also, size of B must be
 * greater than half the size of C.
 */
struct CPolicy_Mirror {
    /// @brief 1D convolution.
    template<class TB, class TC> inline
    blitz::Array<typename blitz::promote_trait<TB,TC>::T_promote, 1> 
        operator()(
            const blitz::Array<TB,1>& B,
            const blitz::Array<TC,1>& C,
            blitz::Array<typename blitz::promote_trait<TB,TC>::T_promote,1>& A )
        const
    {// {{{
        using namespace blitz;
        typedef typename blitz::promote_trait<TB,TC>::T_promote TA;

        const int Bl = B.lbound(0), Bh = B.ubound(0);
        const int Cl = C.lbound(0), Ch = C.ubound(0);

        for (int i=Bl; i <= Bh; ++i)
        {
            TA result = 0;
            int tj=0;
            for (int j=i-Ch; j <= i-Cl; ++j)
            {
                if (j<Bl)
                        tj = 2*Bl - j;
                else if (j>Bh)
                        tj = 2*Bh - j;
                else
                        tj =j;

                result += B(tj) * C(i-j);
            }

            A(i) = result;
        }

        return A;
    }// }}}

    /// @brief 1D convolution at a point.
    template<class TB, class TC> inline
    typename blitz::promote_trait<TB,TC>::T_promote 
        operator()(
            const blitz::Array<TB,1>& B,
            const blitz::Array<TC,1>& C,
            const int i)
        const
    {// {{{
        using namespace blitz;
        typedef typename blitz::promote_trait<TB,TC>::T_promote TA;

        const int Bl = B.lbound(0), Bh = B.ubound(0);
        const int Cl = C.lbound(0), Ch = C.ubound(0);

        TA result = 0;
        int tj=0;
        for (int j=i-Ch; j <= i-Cl; ++j)
        {
            if (j<Bl)
                    tj = 2*Bl - j;
            else if (j>Bh)
                    tj = 2*Bh - j;
            else
                    tj =j;

            result += B(tj) * C(i-j);
        }
        return result;
    }// }}}

    /// @brief Convolve 2D array with 1D array at a point.
    template<class TB, class TC> inline
    typename blitz::promote_trait<TB,TC>::T_promote 
        operator() (
            const blitz::Array<TB,2>& B,
            const blitz::Array<TC,1>& C,
            const blitz::TinyVector<int,2>& i)
        const
    {// {{{
        using namespace blitz;
        typedef typename blitz::promote_trait<TB,TC>::T_promote TA;

        Range all = Range::all();

        Array<TA,1> tmp (C.lbound(),C.extent());

        const int Bl = B.lbound(1), Bh = B.ubound(1);
        const int Cl = C.lbound(0), Ch = C.ubound(0);

        tmp =0;
        int tj=0;
        for (int j=i[1]-Ch, k=Ch; j <= i[1]-Cl; ++j, --k)
        {
            if (j<Bl)
                    tj = 2*Bl - j;
            else if (j>Bh)
                    tj = 2*Bh - j;
            else
                    tj =j;

            tmp(k) = (*this)( B(all,tj), C, i[0] );
        }

        return sum(tmp*C);
    }// }}}

    /// @brief Convolve 2D array with 1D array at a point.
    template<class TB, class TC> inline
    typename blitz::promote_trait<TB,TC>::T_promote 
        operator() (
            const blitz::Array<TB,2>& B,
            const blitz::Array<TC,1>& CX,
            const blitz::Array<TC,1>& CY,
            const blitz::TinyVector<int,2>& i)
        const
    {// {{{
        using namespace blitz;
        typedef typename blitz::promote_trait<TB,TC>::T_promote TA;

        Range all = Range::all();

        Array<TA,1> tmp (CY.lbound(),CY.extent());

        const int Bl = B.lbound(1), Bh = B.ubound(1);
        const int Cl = CY.lbound(0), Ch = CY.ubound(0);

        tmp =0;
        int tj=0;
        for (int j=i[1]-Ch, k=Ch; j <= i[1]-Cl; ++j, --k)
        {
            if (j<Bl)
                    tj = 2*Bl - j;
            else if (j>Bh)
                    tj = 2*Bh - j;
            else
                    tj =j;

            tmp(k) = (*this)( B(all,tj), CX, i[0] );
        }

        return sum(tmp*CY);
    }// }}}
};
// }}}

//{{{ CPolicy_Chop
/** 
* @brief Convolution is chopped off at array boundary.
*
* Define convolution of two arrays. 
* Array B and C are convolved with result in A. 
* Array B is chopped off at boundaries.  Return array has same size as array B.
*
* @param Array B (input Array)
* @param Array C (convolution kernel)
* @param Array A (convolution result)
*
* @warning size of B must be greater than half the size of C.
*/
struct CPolicy_Chop {
    /// @brief 1D convolution.
    template<class TB, class TC> inline
    blitz::Array<typename blitz::promote_trait<TB,TC>::T_promote, 1> 
        operator()(
        const blitz::Array<TB,1>& B,
        const blitz::Array<TC,1>& C,
        blitz::Array<typename blitz::promote_trait<TB,TC>::T_promote,1>& A ) 
        const
    {// {{{
        using namespace blitz;
        typedef typename blitz::promote_trait<TB,TC>::T_promote TA;

        const int Bl = B.lbound(0), Bh = B.ubound(0);
        const int Cl = C.lbound(0), Ch = C.ubound(0);

        for (int i=Bl; i <= Bh; ++i)
        {
            int jl = i - Ch;
            if (jl < Bl)
                jl = Bl;

            int jh = i - Cl;
            if (jh > Bh)
                jh = Bh;

            TA result;
            result = 0;
            for (int j=jl; j <= jh; ++j)
                result += B(j) * C(i-j);

            A(i) = result;
        }

        return A;
    }// }}}

    /// @brief 1D convolution at a point.
    template<class TB, class TC> inline
    typename blitz::promote_trait<TB,TC>::T_promote 
        operator()(
            const blitz::Array<TB,1>& B,
            const blitz::Array<TC,1>& C,
            const int i)
        const
    {// {{{
        using namespace blitz;
        typedef typename blitz::promote_trait<TB,TC>::T_promote TA;

        const int Bl = B.lbound(0), Bh = B.ubound(0);
        const int Cl = C.lbound(0), Ch = C.ubound(0);

        int jl = i - Ch;
        if (jl < Bl)
            jl = Bl;

        int jh = i - Cl;
        if (jh > Bh)
            jh = Bh;

        TA result = 0;
        for (int j=jl; j <= jh; ++j)
            result += B(j) * C(i-j);

        return result;
    }// }}}

    ///@brief Convolve 2D array with 1D array at a point.
    template<class TB, class TC> inline
    typename blitz::promote_trait<TB,TC>::T_promote 
        operator()(
            const blitz::Array<TB,2>& B,
            const blitz::Array<TC,1>& C,
            const blitz::TinyVector<int,2>& i)
        const
    {// {{{
        using namespace blitz;
        typedef typename blitz::promote_trait<TB,TC>::T_promote TA;

        Range all = Range::all();

        Array<TA,1> tmp (C.lbound(),C.extent());

        const int Bl = B.lbound(1), Bh = B.ubound(1);
        const int Cl = C.lbound(0), Ch = C.ubound(0);

        int jl = i[1] - Ch;
        if (jl < Bl)
            jl = Bl;

        int jh = i[1] - Cl;
        if (jh > Bh)
            jh = Bh;

        tmp =0;
        for (int j= jl; j<= jh; ++j) {
            tmp(i[1] - j) = (*this)( B(all,j), C, i[0] );
        }
        return sum(tmp*C);
    }// }}}
    ///@brief Convolve 2D array with 1D array at a point.
    /**
     * Use CX along X-axis and CY along y-axis for convolving.
     */
    template<class TB, class TC> inline
    typename blitz::promote_trait<TB,TC>::T_promote 
        operator()(
            const blitz::Array<TB,2>& B,
            const blitz::Array<TC,1>& CX,
            const blitz::Array<TC,1>& CY,
            const blitz::TinyVector<int,2>& i)
        const
    {// {{{
        using namespace blitz;
        typedef typename blitz::promote_trait<TB,TC>::T_promote TA;

        Range all = Range::all();

        Array<TA,1> tmp (CY.lbound(),CY.extent());

        const int Bl = B.lbound(1), Bh = B.ubound(1);
        const int Cl = CY.lbound(0), Ch = CY.ubound(0);

        int jl = i[1] - Ch;
        if (jl < Bl)
            jl = Bl;

        int jh = i[1] - Cl;
        if (jh > Bh)
            jh = Bh;

        tmp =0;
        for (int j= jl; j<= jh; ++j) {
            tmp(i[1] - j) = (*this)( B(all,j), CX, i[0] );
        }
        return sum(tmp*CY);
    }// }}}
};
//}}}

//{{{ CPolicy_Stretch
/** 
* @brief Convolution array is stretched at array boundary.
*
* Define convolution of two arrays. 
* Array B and C are convolved with result in A. 
* Array B is stretched at boundaries.  Return array has same size as array B.
*
* @param Array B (input Array)
* @param Array C (convolution kernel)
* @param Array A (convolution result)
*
* @warning size of B must be greater than half the size of C.
*/
struct CPolicy_Stretch {
    /// @brief 1D convolution.
    template<class TB, class TC> inline
    blitz::Array<typename blitz::promote_trait<TB,TC>::T_promote, 1> 
        operator()(
        const blitz::Array<TB,1>& B,
        const blitz::Array<TC,1>& C,
        blitz::Array<typename blitz::promote_trait<TB,TC>::T_promote,1>& A ) 
        const
    {// {{{
        using namespace blitz;
        typedef typename blitz::promote_trait<TB,TC>::T_promote TA;

        const int Bl = B.lbound(0), Bh = B.ubound(0);
        const int Cl = C.lbound(0), Ch = C.ubound(0);

        for (int i=Bl; i <= Bh; ++i)
        {
            TA result;
            result = 0;
            int tj=0;
            for (int j=i-Ch; j <= i-Cl; ++j)
            {
                if (j<Bl)
                        tj = Bl;
                else if (j>Bh)
                        tj = Bh;
                else
                        tj =j;

                result += B(tj) * C(i-j);
            }

            A(i) = result;
        }

        return A;
    }// }}}

    /// @brief 1D convolution at a point.
    template<class TB, class TC> inline
    typename blitz::promote_trait<TB,TC>::T_promote 
        operator()(
            const blitz::Array<TB,1>& B,
            const blitz::Array<TC,1>& C,
            const int i)
        const
    {// {{{
        using namespace blitz;
        typedef typename blitz::promote_trait<TB,TC>::T_promote TA;

        const int Bl = B.lbound(0), Bh = B.ubound(0);
        const int Cl = C.lbound(0), Ch = C.ubound(0);

        TA result = 0;
        int tj=0;
        for (int j=i-Ch; j <= i-Cl; ++j)
        {
            if (j<Bl)
                    tj = Bl;
            else if (j>Bh)
                    tj = Bh;
            else
                    tj =j;

            result += B(tj) * C(i-j);
        }
        return result;
    }// }}}

    ///@brief Convolve 2D array with 1D array at a point.
    template<class TB, class TC> inline
    typename blitz::promote_trait<TB,TC>::T_promote 
        operator()(
            const blitz::Array<TB,2>& B,
            const blitz::Array<TC,1>& C,
            const blitz::TinyVector<int,2>& i)
        const
    {// {{{
        using namespace blitz;
        typedef typename blitz::promote_trait<TB,TC>::T_promote TA;

        Range all = Range::all();

        Array<TA,1> tmp (C.lbound(),C.extent());

        const int Bl = B.lbound(1), Bh = B.ubound(1);
        const int Cl = C.lbound(0), Ch = C.ubound(0);

        tmp =0;
        int tj=0;
        for (int j=i[1]-Ch, k=Ch; j <= i[1]-Cl; ++j, --k)
        {
            if (j<Bl)
                    tj = Bl;
            else if (j>Bh)
                    tj = Bh;
            else
                    tj =j;

            tmp(k) = (*this)( B(all,tj), C, i[0] );
        }

        return sum(tmp*C);
    }// }}}
    ///@brief Convolve 2D array with 1D array at a point.
    /**
     * Use CX along X-axis and CY along y-axis for convolving.
     */
    template<class TB, class TC> inline
    typename blitz::promote_trait<TB,TC>::T_promote 
        operator()(
            const blitz::Array<TB,2>& B,
            const blitz::Array<TC,1>& CX,
            const blitz::Array<TC,1>& CY,
            const blitz::TinyVector<int,2>& i)
        const
    {// {{{
        using namespace blitz;
        typedef typename blitz::promote_trait<TB,TC>::T_promote TA;

        Range all = Range::all();

        Array<TA,1> tmp (CY.lbound(),CY.extent());

        const int Bl = B.lbound(1), Bh = B.ubound(1);
        const int Cl = CY.lbound(0), Ch = CY.ubound(0);

        tmp =0;
        int tj=0;
        for (int j=i[1]-Ch, k=Ch; j <= i[1]-Cl; ++j, --k)
        {
            if (j<Bl)
                    tj = Bl;
            else if (j>Bh)
                    tj = Bh;
            else
                    tj =j;

            tmp(k) = (*this)( B(all,tj), CX, i[0] );
        }
        return sum(tmp*CY);
    }// }}}
};
//}}}

// {{{ CPolicy_Loop
/** 
 * @defgroup overloaded convolveLoopExact functions
 * @brief Convolution operationeds limited to array boundaries only
 * (for example Quite handy for convolving images)
 * Array is Looped accross boundaries. 
 *
 * Define convolution of two arrays. 
 * Array B and C are convolved with result in A. 
 * Array B is Looped accross boundaries.  Return array has same size as array B.
 *
 * @param Array B (input Array)
 * @param Array C (convolution kernel)
 * @param Array A (convolution result)
 *
 * @warning Array C is assumed to be centered around 0. Also, size of B must be
 * greater than half the size of C.
 */
struct CPolicy_Loop {
    /// @brief 1D convolution.
    template<class TB, class TC> inline
    blitz::Array<typename blitz::promote_trait<TB,TC>::T_promote, 1> 
        operator()(
            const blitz::Array<TB,1>& B,
            const blitz::Array<TC,1>& C,
            blitz::Array<typename blitz::promote_trait<TB,TC>::T_promote,1>& A )
        const
    {// {{{
        using namespace blitz;
        typedef typename blitz::promote_trait<TB,TC>::T_promote TA;

        const int Bl = B.lbound(0), Bh = B.ubound(0);
        const int Cl = C.lbound(0), Ch = C.ubound(0);

        for (int i=Bl; i <= Bh; ++i)
        {
            TA result = 0;
            int tj=0;
            for (int j=i-Ch; j <= i-Cl; ++j)
            {
                if (j<Bl)
                        tj = Bh + j +1;
                else if (j>Bh)
                        tj = Bl - Bh + j -1;
                else
                        tj =j;

                result += B(tj) * C(i-j);
            }

            A(i) = result;
        }

        return A;
    }// }}}

    /// @brief 1D convolution at a point.
    template<class TB, class TC> inline
    typename blitz::promote_trait<TB,TC>::T_promote 
        operator()(
            const blitz::Array<TB,1>& B,
            const blitz::Array<TC,1>& C,
            const int i)
        const
    {// {{{
        using namespace blitz;
        typedef typename blitz::promote_trait<TB,TC>::T_promote TA;

        const int Bl = B.lbound(0), Bh = B.ubound(0);
        const int Cl = C.lbound(0), Ch = C.ubound(0);

        TA result = 0;
        int tj=0;
        for (int j=i-Ch; j <= i-Cl; ++j)
        {
                if (j<Bl)
                        tj = Bh + j +1;
                else if (j>Bh)
                        tj = Bl - Bh + j -1;
                else
                        tj =j;

            result += B(tj) * C(i-j);
        }
        return result;
    }// }}}

    /// @brief Convolve 2D array with 1D array at a point.
    template<class TB, class TC> inline
    typename blitz::promote_trait<TB,TC>::T_promote 
        operator() (
            const blitz::Array<TB,2>& B,
            const blitz::Array<TC,1>& C,
            const blitz::TinyVector<int,2>& i)
        const
    {// {{{
        using namespace blitz;
        typedef typename blitz::promote_trait<TB,TC>::T_promote TA;

        Range all = Range::all();

        Array<TA,1> tmp (C.lbound(),C.extent());

        const int Bl = B.lbound(1), Bh = B.ubound(1);
        const int Cl = C.lbound(0), Ch = C.ubound(0);

        tmp =0;
        int tj=0;
        for (int j=i[1]-Ch, k=Ch; j <= i[1]-Cl; ++j, --k)
        {
            if (j<Bl)
                    tj = Bh + j +1;
            else if (j>Bh)
                    tj = Bl - Bh + j -1;
            else
                    tj =j;

            tmp(k) = (*this)( B(all,tj), C, i[0] );
        }

        return sum(tmp*C);
    }// }}}
    /// @brief Convolve 2D array with 1D array at a point.
    template<class TB, class TC> inline
    typename blitz::promote_trait<TB,TC>::T_promote 
        operator() (
            const blitz::Array<TB,2>& B,
            const blitz::Array<TC,1>& CX,
            const blitz::Array<TC,1>& CY,
            const blitz::TinyVector<int,2>& i)
        const
    {// {{{
        using namespace blitz;
        typedef typename blitz::promote_trait<TB,TC>::T_promote TA;

        Range all = Range::all();

        Array<TA,1> tmp (CY.lbound(),CY.extent());

        const int Bl = B.lbound(1), Bh = B.ubound(1);
        const int Cl = CY.lbound(0), Ch = CY.ubound(0);

        tmp =0;
        int tj=0;
        for (int j=i[1]-Ch, k=Ch; j <= i[1]-Cl; ++j, --k)
        {
            if (j<Bl)
                    tj = Bh + j +1;
            else if (j>Bh)
                    tj = Bl - Bh + j -1;
            else
                    tj =j;

            tmp(k) = (*this)( B(all,tj), CX, i[0] );
        }

        return sum(tmp*CY);
    }// }}}
};
// }}}

template<class Policy>
struct ConvolveExact {
    /** @brief 1D convolution.
    */
    template<class TB, class TC> inline
    blitz::Array<typename blitz::promote_trait<TB,TC>::T_promote, 1> 
        operator()(
            const blitz::Array<TB,1>& B,
            const blitz::Array<TC,1>& C,
            blitz::Array<typename blitz::promote_trait<TB,TC>::T_promote,1>& A)
        const
    {
        Policy method; 
        return method(B,C,A);
    }
    /**
    * @brief Convolve 2D array with 1D array at a point.
    */
    template<class TB, class TC, int N> inline
    typename blitz::promote_trait<TB,TC>::T_promote 
        operator()(
            const blitz::Array<TB,N>& B,
            const blitz::Array<TC,1>& C,
            const blitz::TinyVector<int,N>& i)
        const
    {
        Policy method;
        return method(B,C,i);
    }
    template<class TB, class TC, int N> inline
    typename blitz::promote_trait<TB,TC>::T_promote 
        operator()(
            const blitz::Array<TB,N>& B,
            const blitz::Array<TC,1>& CX,
            const blitz::Array<TC,1>& CY,
            const blitz::TinyVector<int,N>& i)
        const
    {
        Policy method;
        return method(B,CX,CY,i);
    }

    /**
    * @brief Convolve 2D array with 1D array along X-axis.
    */
    template<class TB, class TC> inline
    blitz::Array<typename blitz::promote_trait<TB,TC>::T_promote, 2>& 
        dim1(
            const blitz::Array<TB,2>& B,
            const blitz::Array<TC,1>& C,
            blitz::Array<typename blitz::promote_trait<TB,TC>::T_promote, 2>& A)
        const
    {// {{{
        using namespace blitz;

        Range all = Range::all();

        for (int i=B.lbound(1); i <= B.ubound(1); ++i)
        {
            Array<typename promote_trait<TB,TC>::T_promote, 1> tmp = A(all,i);
            (*this)( B(all,i), C,tmp);
        }
        return A;
    }// }}}

    /**
    * @brief Convolve 2D array with 1D array along X-axis.
    *  Overloaded function. Creates new array for result.
    */
    template<class TB, class TC> inline
    blitz::Array<typename blitz::promote_trait<TB,TC>::T_promote, 2> 
        dim1(
            const blitz::Array<TB,2>& B,
            const blitz::Array<TC,1>& C)
        const
    {// {{{
        typedef typename blitz::promote_trait<TB,TC>::T_promote TA;
        blitz::Array<TA,2> A (B.lbound(),B.extent()) ;
        return dim1(B,C,A);
    }// }}}

    /**
    * @brief Convolve 2D array with 1D array along X-axis at a point.
    */
    template<class TB, class TC> inline
    typename blitz::promote_trait<TB,TC>::T_promote 
        dim1(
            const blitz::Array<TB,2>& B,
            const blitz::Array<TC,1>& C,
            const blitz::TinyVector<int,2>& i)
        const
    {// {{{
        blitz::Range all = blitz::Range::all();

        return (*this)( B(all,i[1]), C, i[0]);
    }// }}}

    /**
    * @brief Convolve 2D array with 1D array along Y-axis.
    */
    template<class TB, class TC> inline
    blitz::Array<typename blitz::promote_trait<TB,TC>::T_promote, 2>& 
        dim2(
            const blitz::Array<TB,2>& B,
            const blitz::Array<TC,1>& C,
            blitz::Array<typename blitz::promote_trait<TB,TC>::T_promote, 2>& A)
        const
    {// {{{
        using namespace blitz;

        Range all = Range::all();

        for (int i=B.lbound(0); i <= B.ubound(0); ++i)
        {
            Array<typename promote_trait<TB,TC>::T_promote, 1> tmp = A(i,all);
            (*this)( B(i,all), C, tmp);
        }
        return A;
    }// }}}

    /**
    * @brief Convolve 2D array with 1D array along Y-axis.
    * Overloaded function. Creates new array for result.
    */
    template<class TB, class TC> inline
    blitz::Array<typename blitz::promote_trait<TB,TC>::T_promote, 2> 
        dim2(
            const blitz::Array<TB,2>& B,
            const blitz::Array<TC,1>& C)
        const
    {// {{{
        typedef typename blitz::promote_trait<TB,TC>::T_promote TA;
        blitz::Array<TA,2> A (B.lbound(),B.extent()) ;
        return (*this)(B,C,A);
    }// }}}

    /**
    * @brief Convolve 2D array with 1D array along Y-axis at a point.
    */
    template<class TB, class TC> inline
    typename blitz::promote_trait<TB,TC>::T_promote 
        dim2(
        const blitz::Array<TB,2>& B,
        const blitz::Array<TC,1>& C,
        const blitz::TinyVector<int,2>& i)
    {// {{{
        blitz::Range all = blitz::Range::all();

        return (*this)( B(i[0],all), C, i[1]);
    }// }}}

    /**
    * @brief Convolve 2D array with 1D array.
    */
    template<class TB, class TC> inline
    blitz::Array<typename blitz::promote_trait<TB,TC>::T_promote, 2>& 
        operator()(
        const blitz::Array<TB,2>& B,
        const blitz::Array<TC,1>& C,
        blitz::Array<typename blitz::promote_trait<TB,TC>::T_promote, 2>& A)
        const
    {// {{{
        typedef typename blitz::promote_trait<TB,TC>::T_promote TA;
        blitz::Array<TA,2> tmp (B.lbound(),B.extent()) ;

        dim1(B, C, tmp );
        dim2(tmp, C, A );
        return A;
    }// }}}
    
    /**
    * @brief Convolve 3D array with 1D array.
    */
    template<class TB, class TC> inline
    blitz::Array<typename blitz::promote_trait<TB,TC>::T_promote, 3>& 
        operator()(
        const blitz::Array<TB,3>& B, 
        const blitz::Array<TC,1>& C,
        blitz::Array<typename blitz::promote_trait<TB,TC>::T_promote, 3>& A) 
        const
    {// {{{
        using namespace blitz;
        typedef typename blitz::promote_trait<TB,TC>::T_promote TA;
        blitz::Array<TA,3> tmp (B.lbound(),B.extent()) ;

        Range all = Range::all();

        Array<TA,3> tmp3D (B.lbound(), B.extent());
        {
            Array<TA,2> tmp ( 
                TinyVector<int,2> (B.lbound(1), B.lbound(2)),
                TinyVector<int,2> (B.extent(1), B.extent(2)) );

            for (int i=B.lbound(0); i <= B.ubound(0); ++i)
            {
                dim1(B(i,all,all), C, tmp);
                Array<TA,2> tmp2D = tmp3D(i,all,all);
                dim2(tmp, C,  tmp2D);
            }
        }
        for (int j=B.lbound(1); j <= B.ubound(1); ++j)
        for (int k=B.lbound(2); k <= B.ubound(2); ++k) {
                (*this)(tmp3D(all,j,k),C,A(all,j,k));
        }
        return A;
    }// }}}

    /**
    * @brief Convolve 3D array with 1D arrays.
    *
    * C : smooth kernel along dimension 1 (smallest)
    * D : smooth kernel along dimension 2
    * E : smooth kernel along dimension 3 (highest)
    */
    template<class TB, class TC> inline
    blitz::Array<typename blitz::promote_trait<TB,TC>::T_promote, 3>& 
        operator()(
        const blitz::Array<TB,3>& B, 
        const blitz::Array<TC,1>& C,
        const blitz::Array<TC,1>& D,
        const blitz::Array<TC,1>& E,
        blitz::Array<typename blitz::promote_trait<TB,TC>::T_promote, 3>& A) 
        const
    {// {{{
        using namespace blitz;
        typedef typename blitz::promote_trait<TB,TC>::T_promote TA;
        Range all = Range::all();

        Array<TA,3> tmp3D (B.lbound(), B.extent());
        {
            Array<TA,2> tmp ( 
                TinyVector<int,2> (B.lbound(1), B.lbound(2)),
                TinyVector<int,2> (B.extent(1), B.extent(2)) );

            for (int i=B.lbound(0); i <= B.ubound(0); ++i)
            {
                dim1(B(i,all,all), C, tmp);
                Array<TA,2> tmp2D = tmp3D(i,all,all);
                dim2(tmp, D,  tmp2D);
            }
        }
        for (int j=B.lbound(1); j <= B.ubound(1); ++j)
        for (int k=B.lbound(2); k <= B.ubound(2); ++k) {
                blitz::Array<TA,1> tA =  A(all,j,k);
                (*this)(tmp3D(all,j,k),E,tA);
        }
        return A;
    }// }}}
    /**
    * @brief Convolve 3-D array with 1D.
    * Creates new array for result.
    */
    template<class TB, class TC, int N> inline
    blitz::Array<typename blitz::promote_trait<TB,TC>::T_promote, N> 
        operator()( 
                const blitz::Array<TB,N>& B, 
                const blitz::Array<TC,1>& C) 
        const
    {// {{{
        typedef typename blitz::promote_trait<TB,TC>::T_promote TA;
        blitz::Array<TA,N> A (B.lbound(),B.extent()) ;
        return (*this)(B,C,A);
    }// }}}
};

template<class Policy, class TB, class TC, int N> inline
blitz::Array<typename blitz::promote_trait<TB,TC>::T_promote, N> 
convolveExact(
    const blitz::Array<TB,N>& B, 
    const blitz::Array<TC,1>& C,
    blitz::Array<typename blitz::promote_trait<TB,TC>::T_promote, N>& A)
{
    ConvolveExact<Policy> op;
    return op(B,C,A);
}
template<class Policy, class TB, class TC, int N> inline
blitz::Array<typename blitz::promote_trait<TB,TC>::T_promote, N> 
convolveExact(
    const blitz::Array<TB,N>& B, 
    const blitz::Array<TC,1>& C)
{
    ConvolveExact<Policy> op;
    return op(B,C);
}

BZ_NAMESPACE_END

#endif //#ifdef _LEAR_CONVOLVE_H_

//{{{ extra code

/*template<class T>
blitz::Array<T,1> convolve(const blitz::Array<T,1>& B,
						   const blitz::Array<T,1>& C,
						   blitz::Array<T,1> A);*/


/** Defines convolution operation. Return array has same
  * shape as B
  */
/*template<class T>
blitz::Array<T,2> convolve(const blitz::Array<T,2>& B,
						   const blitz::Array<T,1>& C,
						   int dimension) ;*/

/** Defines convolution operation. Return array has same
  * shape as B
  */
/*template<class T>
blitz::Array<T,1> convolveExact(const blitz::Array<T,1>& B,
								const blitz::Array<T,1>& C,
								blitz::Array<T,1> A) ; */

/** Defines convolution operation. Return array has same
  * shape as B
  */
/*template<class T>
blitz::Array<T,2> convolveExact(const blitz::Array<T,2>& B,
								const blitz::Array<T,1>& C,
								int dimension) ;*/

/** Defines convolution operation. Array B is mirror accross boundaries.
  * Array C is assumed to be centered around 0.
  *
  * NOTE: Extent of B along all dimensions must be greater than half
  * the extent of C.
  */
/*template<class T>
blitz::Array<T,1> convolveMirror(const blitz::Array<T,1>& B,
								 const blitz::Array<T,1>& C,
								 blitz::Array<T,1> A) ; */

/** Defines convolution operation. Array B is mirror accross boundaries.
  * Array C is assumed to be centered around 0.
  *
  * NOTE: Extent of B must be greater than half
  * the extent of C.
  */
/*template<class T>
blitz::Array<T,2> convolveMirror(const blitz::Array<T,2>& B,
								 const blitz::Array<T,1>& C,
								 int dimension) ;*/

// Definitions starting now...

/*
template<class T>
blitz::Array<T,1> convolve(const blitz::Array<T,1>& B,
						   const blitz::Array<T,1>& C,
						   blitz::Array<T,1> A = blitz::Array<T,1>( Range(
							B.lbound(0)+C.lbound(0), B.ubound(0)+C.ubound(0)) ) )
{
    int Bl = B.lbound(0), Bh = B.ubound(0);
    int Cl = C.lbound(0), Ch = C.ubound(0);

    for (int i=A.lbound(0); i <= A.ubound(0); ++i)
    {
        int jl = i - Ch;
        if (jl < Bl)
            jl = Bl;

        int jh = i - Cl;
        if (jh > Bh)
            jh = Bh;

        T result = 0;
        for (int j=jl; j <= jh; ++j)
            result += B(j) * C(i-j);

        A(i) = result;
    }

    return A;
}

template<class T>
blitz::Array<T,2> convolve(const blitz::Array<T,2>& B,
						   const blitz::Array<T,1>& C,
						   int dimension)
{
	using namespace blitz;

    TinyVector<int,2> Bl = B.lbound(), Be = B.extent();
    int Cl = C.lbound(0), Cu = C.ubound(0);

    TinyVector<int,2> lbound = Bl;
	lbound[dimension] += Cl;

    TinyVector<int,2> extent = Be;
	extent[dimension] += Cu - Cl;

    Array<T,2> A(lbound,extent);

	Range all = Range::all();
	if (dimension == 0)
	{
		int l = B.lbound(1), u = B.ubound(1);
		for (int i=l; i <= u; ++i)
		{
			convolve( B(all,i), C, A(all,i));
		}
	} else //if (dimension == 1)
	{
		int l = B.lbound(0), u = B.ubound(0);
		for (int i=l; i <= u; ++i)
		{
			convolve( B(i,all), C, A(i, all));
		}
	}

    return A;
}

template<class T>
blitz::Array<T,1> convolveExact(const blitz::Array<T,1>& B,
								const blitz::Array<T,1>& C,
								blitz::Array<T,1> A =
									blitz::Array<T,1>(
									Range(B.lbound(0),B.ubound(0))  ) )
{
	using namespace blitz;

    int Bl = B.lbound(0), Bh = B.ubound(0);
    int Cl = C.lbound(0), Ch = C.ubound(0);

    for (int i=Bl; i <= Bh; ++i)
    {
        int jl = i - Ch;
        if (jl < Bl)
            jl = Bl;

        int jh = i - Cl;
        if (jh > Bh)
            jh = Bh;

        T result = 0;
        for (int j=jl; j <= jh; ++j)
            result += B(j) * C(i-j);

        A(i) = result;
    }

    return A;
}

template<class T>
blitz::Array<T,2> convolveExact(const blitz::Array<T,2>& B,
								const blitz::Array<T,1>& C,
								int dimension)
{
	using namespace blitz;

    TinyVector<int,2> Bl = B.lbound(), Be = B.extent();
    int Cl = C.lbound(0), Cu = C.ubound(0);

    Array<T,2> A(Bl,Be);


	if (dimension == 0)
	{
		Range all = Range(B.lbound(0),B.ubound(0));
		int l = B.lbound(1), u = B.ubound(1);
		for (int i=l; i <= u; ++i)
		{
			convolveExact( B(all,i), C, A(all,i));
		}
	} else //if (dimension == 1)
	{
		Range all = Range(B.lbound(1),B.ubound(1));
		int l = B.lbound(0), u = B.ubound(0);
		for (int i=l; i <= u; ++i)
		{
			convolveExact( B(i,all), C, A(i, all));
		}
	}

    return A;
}


template<class T>
blitz::Array<T,1> convolveMirror(const blitz::Array<T,1>& B,
								 const blitz::Array<T,1>& C,
								 blitz::Array<T,1> A =
									blitz::Array<T,1>(
									Range(
										B.lbound(0)+C.lbound(0),
										B.ubound(0)+C.ubound(0) )
										) )
{
	using namespace blitz;

    int Bl = B.lbound(0), Bh = B.ubound(0);
    int Cl = C.lbound(0), Ch = C.ubound(0);

    for (int i=A.lbound(0); i <= A.ubound(0); ++i)
	{
        T result = 0;

		int tj=0;
        for (int j=i-Ch; j <= i-Cl; ++j)
		{
			if (j<Bl)
				tj = 2*Bl - j;
			else if (j>Bh)
				tj = 2*Bh - j;
			else
				tj = j;

            result += B(tj) * C(i-j);
		}

        A(i) = result;
    }

    return A;
}

template<class T>
blitz::Array<T,2> convolveMirror(const blitz::Array<T,2>& B,
								 const blitz::Array<T,1>& C,
								 int dimension)
{
	using namespace blitz;

    TinyVector<int,2> Bl = B.lbound(), Be = B.extent();
    int Cl = C.lbound(0), Cu = C.ubound(0);

    TinyVector<int,2> lbound = Bl;
	lbound[dimension] += Cl;

    TinyVector<int,2> extent = Be;
	extent[dimension] += Cu - Cl;

    Array<T,2> A(lbound,extent);

	Range all = Range::all();
	if (dimension == 0)
	{
		int l = B.lbound(1), u = B.ubound(1);
		for (int i=l; i <= u; ++i)
		{
			convolveMirror( B(all,i), C, A(all,i));
		}
	} else if (dimension == 1)
	{
		int l = B.lbound(0), u = B.ubound(0);
		for (int i=l; i <= u; ++i)
		{
			convolveMirror( B(i,all), C, A(i, all));
		}
	}

    return A;
}*/


/*template<class T>
blitz::Array<T,2> convolveMirrorExact(const blitz::Array<T,2>& B,
								const blitz::Array<T,1>& C)
{
	using namespace blitz;

    TinyVector<int,2> Bl = B.lbound(), Be = B.extent();
    int Cl = C.lbound(0), Ch = C.ubound(0);

    Array<T,2> D(Bl,Be);

	for (int k=B.lbound(1); k <= B.ubound(1); ++k)
	{
		int l = B.lbound(0), h = B.ubound(0);

		for (int i=l; i <= h; ++i)
		{
			T result = 0;

			int tj=0;
			for (int j=i-Ch; j <= i-Cl; ++j)
			{
				if (j<l)
					tj = 2*l - j;
				else if (j>h)
					tj = 2*h - j;
				else
					tj =j;

				result += B(tj,k) * C(i-j);
			}

			D(i,k) = result;
		}
	}
	Array<T,2> A(Bl,Be);
	for (int k=D.lbound(0); k <= D.ubound(0); ++k)
	{
		int l = D.lbound(1), h = D.ubound(1);

		for (int i=l; i <= h; ++i)
		{
			int tj=0;
			T result = 0;
			for (int j=i-Ch; j <= i-Cl; ++j)
			{
				if (j<l)
					tj = 2*l - j;
				else if (j>h)
					tj = 2*h - j;
				else
					tj =j;

				result += D(k,tj) * C(i-j);
			}

			A(k,i) = result;
		}
	}

    return A;
}*/
 //}}}
 
