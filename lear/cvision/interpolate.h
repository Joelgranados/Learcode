#ifndef _LEAR_INTERPOLATE_H_
#define _LEAR_INTERPOLATE_H_

#include <blitz/array.h>
#include <blitz/tinyvec.h>
#include <blitz/tinyvec-et.h>

#include <cmath>

namespace lear {

template<int N>
struct Interpolate {
};
template<>
struct Interpolate<1> {// {{{
    enum {N = 1};
    typedef int IndexVector;

    template<class RealTypeA, class RealTypeB>
    static void linear(
        blitz::Array<RealTypeA,N>& array, 
        const RealTypeA value,
        const RealTypeB d,
        const IndexVector lowerI,
        const IndexVector upperI)
    {
        array(lowerI) = static_cast<RealTypeA>((1-d)*value);
        array(upperI) = static_cast<RealTypeA>(d*value);
    }
    template<class RealTypeA, class RealTypeB>
    static void linear(
        blitz::Array<RealTypeA,N>& array, 
        const RealTypeA value,
        const blitz::TinyVector<RealTypeB,1> d,
        const blitz::TinyVector<IndexVector,1> lowerI,
        const blitz::TinyVector<IndexVector,1> upperI)
    {
        linear(array,value,d[0],lowerI[0],upperI[0]);
    }

    /**
     * Performs safe linear interoplation on a 1D data.
     * Safe here means that if bounds are reached for 
     * upper index, array is not incremented.
     */
    template<class RealTypeA, class RealTypeB>
    static void linear(
        blitz::Array<RealTypeA,N>& array, 
        const RealTypeA value,
        const RealTypeB d,
        const IndexVector lowerI,
        const IndexVector upperI,
        const bool lowerValid,
        const bool upperValid)
    {
        const RealTypeB c = 1 - d;
        for (int i= 0; i< 2; ++i) {
            if ((i==0 && !lowerValid) || (i && !upperValid))
                continue;
            RealTypeB iwt = i ? d : c;
            int ipos = i ? upperI : lowerI;

            array(ipos) += static_cast<RealTypeA>(value*iwt);
        }
    }
    template<class RealTypeA, class RealTypeB>
    static void linear(
        blitz::Array<RealTypeA,N>& array, 
        const RealTypeA value,
        const blitz::TinyVector<RealTypeB,1> d,
        const blitz::TinyVector<IndexVector,1> lowerI,
        const blitz::TinyVector<IndexVector,1> upperI,
        const blitz::TinyVector<bool,1> lowerValid,
        const blitz::TinyVector<bool,1> upperValid)
    {
        linear(array,value,d[0],lowerI[0],upperI[0],
                lowerValid[0],upperValid[0]);
    }
};// }}}
template<>
struct Interpolate<2> {// {{{
    enum {N = 2};
    typedef blitz::TinyVector<int,N> IndexVector;

    /**
     * Performs linear interpolation on a 2D data.
     */
    template<class RealTypeA, class RealTypeB>
    static void linear(
        blitz::Array<RealTypeA,N>& array, 
        const RealTypeA& value,
        const blitz::TinyVector<RealTypeB,N> d,
        const IndexVector lowerI,
        const IndexVector upperI)
    {
        const blitz::TinyVector<RealTypeB,N> c = 1 - d;
        for (int i= 0; i< 2; ++i) {
            RealTypeB iwt = i ? d[0] : c[0];
            int ipos = i ? upperI[0] : lowerI[0];

            for (int j= 0; j< 2; ++j) {
                RealTypeB jwt = iwt*(j ? d[1] : c[1]);
                int jpos = j ? upperI[1] : lowerI[1];

                array(ipos,jpos) += static_cast<RealTypeA>(value*jwt);
            }
        }
    }

    /**
     * Performs safe linear interoplation on a 2D data.
     * Safe here means that if bounds are reached for 
     * upper index, array is not incremented.
     */
    template<class RealTypeA, class RealTypeB>
    static void linear(
        blitz::Array<RealTypeA,N>& array, 
        const RealTypeA& value,
        const blitz::TinyVector<RealTypeB,N> d,
        const IndexVector lowerI,
        const IndexVector upperI,
        const blitz::TinyVector<bool,N> lowerValid,
        const blitz::TinyVector<bool,N> upperValid)
    {
        const blitz::TinyVector<RealTypeB,N> c = 1 - d;
        for (int i= 0; i< 2; ++i) {
            if ((i==0 && !lowerValid[0]) || (i && !upperValid[0]))
                continue;
            RealTypeB iwt = i ? d[0] : c[0];
            int ipos = i ? upperI[0] : lowerI[0];

            for (int j= 0; j< 2; ++j) {
                if ((j==0 && !lowerValid[1]) || (j && !upperValid[1]))
                    continue;
                RealTypeB jwt = iwt*(j ? d[1] : c[1]);
                int jpos = j ? upperI[1] : lowerI[1];

                array(ipos,jpos) += static_cast<RealTypeA>(value*jwt);
            }
        }
    }
    /**
     * Performs safe linear interoplation on a 2D data.
     * Safe here means that if bounds are reached for 
     * upper index, array is not incremented.
     */
    template<class RealTypeA, class RealTypeB>
    static void linear(
        blitz::Array<RealTypeA,N>& array, 
        blitz::Array<RealTypeB,N>& weight, 
        const RealTypeA& value,
        const blitz::TinyVector<RealTypeB,N> d,
        const IndexVector lowerI,
        const IndexVector upperI,
        const blitz::TinyVector<bool,N> lowerValid,
        const blitz::TinyVector<bool,N> upperValid)
    {
        const blitz::TinyVector<RealTypeB,N> c = 1 - d;
        for (int i= 0; i< 2; ++i) {
            if ((i==0 && !lowerValid[0]) || (i && !upperValid[0]))
                continue;
            RealTypeB iwt = i ? d[0] : c[0];
            int ipos = i ? upperI[0] : lowerI[0];

            for (int j= 0; j< 2; ++j) {
                if ((j==0 && !lowerValid[1]) || (j && !upperValid[1]))
                    continue;
                RealTypeB jwt = iwt*(j ? d[1] : c[1]);
                int jpos = j ? upperI[1] : lowerI[1];

                array(ipos,jpos) += static_cast<RealTypeA>(value*jwt);
                weight(ipos,jpos) += jwt;
            }
        }
    }
    /**
     * Performs reverse linear interpolation on a 2D data.
     */
    template<class RealType, class RealTypeA, class RealTypeB>
    static RealType linear_wgted_t(
        const blitz::Array<RealTypeA,N>& array, 
        const blitz::TinyVector<RealTypeB,N> d,
        const IndexVector lowerI,
        const IndexVector upperI)
    {
        RealType result = 0;
        const blitz::TinyVector<RealTypeB,N> c = 1 - d;
        for (int i= 0; i< 2; ++i) {
            RealTypeB iwt = i ? d[0] : c[0];
            int ipos = i ? upperI[0] : lowerI[0];

            for (int j= 0; j< 2; ++j) {
                RealTypeB jwt = iwt*(j ? d[1] : c[1]);
                int jpos = j ? upperI[1] : lowerI[1];

                result += static_cast<RealType>(jwt*array(ipos,jpos));
            }
        }
        return result;
    }
    template<class RealTypeA, class RealTypeB>
    static RealTypeA linear_wgted(
        const blitz::Array<RealTypeA,N>& array, 
        const blitz::TinyVector<RealTypeB,N> d,
        const IndexVector lowerI,
        const IndexVector upperI)
    {
        return linear_wgted_t<RealTypeA>(array,d, lowerI,upperI);
    }
    /**
     * Performs safe reverse linear interpolation on a 2D data.
     */
    template<class RealType, class RealTypeA, class RealTypeB>
    static RealType linear_wgted_t(
        const blitz::Array<RealTypeA,N>& array, 
        const blitz::TinyVector<RealTypeB,N> d,
        const IndexVector lowerI,
        const IndexVector upperI,
        const blitz::TinyVector<bool,N> lowerValid,
        const blitz::TinyVector<bool,N> upperValid)
    {
        RealType result = 0;
        bool wtsum_valid=false;
        RealTypeB wtsum = 0;
        const blitz::TinyVector<RealTypeB,N> c = 1 - d;
        for (int i= 0; i< 2; ++i) {
            if ((i==0 && !lowerValid[0]) || (i && !upperValid[0]))
                continue;
            RealTypeB iwt = i ? d[0] : c[0];
            int ipos = i ? upperI[0] : lowerI[0];

            for (int j= 0; j< 2; ++j) {
                if ((j==0 && !lowerValid[1]) || (j && !upperValid[1]))
                    continue;
                RealTypeB jwt = iwt*(j ? d[1] : c[1]);
                int jpos = j ? upperI[1] : lowerI[1];

                wtsum += jwt;
                wtsum_valid=true;
                result += static_cast<RealType>(jwt*array(ipos,jpos));
            }
        }
        if (wtsum_valid)
            result /= static_cast<RealType>(wtsum);
        return result;
    }
    template<class RealTypeA, class RealTypeB>
    static RealTypeA linear_wgted(
        const blitz::Array<RealTypeA,N>& array, 
        const blitz::TinyVector<RealTypeB,N> d,
        const IndexVector lowerI,
        const IndexVector upperI,
        const blitz::TinyVector<bool,N> lowerValid,
        const blitz::TinyVector<bool,N> upperValid)
    {
        return linear_wgted_t<RealTypeA>(array,d,
                lowerI,upperI,lowerValid,upperValid);
    }

};// }}}
template<>
struct Interpolate<3> {// {{{
    enum {N = 3};
    typedef blitz::TinyVector<int,N> IndexVector;

    /**
     * Performs linear interpolation on a 3D data.
     */
    template<class RealTypeA, class RealTypeB>
    static void linear(
        blitz::Array<RealTypeA,N>& array, 
        const RealTypeA& value,
        const blitz::TinyVector<RealTypeB,N>& d,
        const IndexVector& lowerI,
        const IndexVector& upperI)
    {
        const blitz::TinyVector<RealTypeB,N> c = 1 - d;
        for (int i= 0; i< 2; ++i) {
            RealTypeB iwt = i ? d[0] : c[0];
            int ipos = i ? upperI[0] : lowerI[0];

            for (int j= 0; j< 2; ++j) {
                RealTypeB jwt = iwt*(j ? d[1] : c[1]);
                int jpos = j ? upperI[1] : lowerI[1];

                for (int k= 0; k< 2; ++k) {
                    RealTypeB kwt = jwt*(k ? d[2] : c[2]);
                    int kpos = k ? upperI[2] : lowerI[2];
                    {
                    array(ipos,jpos,kpos) += 
                        static_cast<RealTypeA>(value*kwt);
                    }
                }
            }
        }
    }

    /**
     * Performs safe linear interoplation on a 3D data.
     * Safe here means that if bounds are reached for 
     * upper index, array is not incremented.
     */
    template<class RealTypeA, class RealTypeB>
    static void linear(
        blitz::Array<RealTypeA,N>& array, 
        const RealTypeA& value,
        const blitz::TinyVector<RealTypeB,N>& d,
        const IndexVector& lowerI,
        const IndexVector& upperI,
        const blitz::TinyVector<bool,N>& lowerValid,
        const blitz::TinyVector<bool,N>& upperValid)
    {
        const blitz::TinyVector<RealTypeB,N> c = 1 - d;
        for (int i= 0; i< 2; ++i) {
            if ((i==0 && !lowerValid[0]) || (i && !upperValid[0]))
                continue;
            RealTypeB iwt = i ? d[0] : c[0];
            int ipos = i ? upperI[0] : lowerI[0];

            for (int j= 0; j< 2; ++j) {
                if ((j==0 && !lowerValid[1]) || (j && !upperValid[1]))
                    continue;
                RealTypeB jwt = iwt*(j ? d[1] : c[1]);
                int jpos = j ? upperI[1] : lowerI[1];

                for (int k= 0; k< 2; ++k) {
                    if ((k==0 && !lowerValid[2]) || (k && !upperValid[2]))
                        continue;
                    RealTypeB kwt = jwt*(k ? d[2] : c[2]);
                    int kpos = k ? upperI[2] : lowerI[2];
                    {
                    array(ipos,jpos,kpos) += 
                        static_cast<RealTypeA>(value*kwt);
                    }
                }
            }
        }
    }
    /**
     * Performs reverse linear interpolation on a 3D data.
     */
    template<class RealType, class RealTypeA, class RealTypeB>
    static RealType linear_wgted_t(
        const blitz::Array<RealTypeA,N>& array, 
        const blitz::TinyVector<RealTypeB,N> d,
        const IndexVector lowerI,
        const IndexVector upperI)
    {
        RealType result = 0;
        const blitz::TinyVector<RealTypeB,N> c = 1 - d;
        for (int i= 0; i< 2; ++i) {
            RealTypeB iwt = i ? d[0] : c[0];
            int ipos = i ? upperI[0] : lowerI[0];

            for (int j= 0; j< 2; ++j) {
                RealTypeB jwt = iwt*(j ? d[1] : c[1]);
                int jpos = j ? upperI[1] : lowerI[1];

                for (int k= 0; k< 2; ++k) {
                    RealTypeB kwt = jwt*(k ? d[2] : c[2]);
                    int kpos = k ? upperI[2] : lowerI[2];

                    result += static_cast<RealType>(kwt*array(ipos,jpos,kpos));
                }
            }
        }
        return result;
    }
    template<class RealTypeA, class RealTypeB>
    static RealTypeA linear_wgted(
        const blitz::Array<RealTypeA,N>& array, 
        const blitz::TinyVector<RealTypeB,N> d,
        const IndexVector lowerI,
        const IndexVector upperI)
    {
        return linear_wgted_t<RealTypeA>(array,d, lowerI,upperI);
    }
    /**
     * Performs safe reverse linear interpolation on a 2D data.
     */
    template<class RealType, class RealTypeA, class RealTypeB>
    static RealType linear_wgted_t(
        const blitz::Array<RealTypeA,N>& array, 
        const blitz::TinyVector<RealTypeB,N> d,
        const IndexVector lowerI,
        const IndexVector upperI,
        const blitz::TinyVector<bool,N> lowerValid,
        const blitz::TinyVector<bool,N> upperValid)
    {
        RealType result = 0;
        bool wtsum_valid=false;
        RealTypeB wtsum = 0;
        const blitz::TinyVector<RealTypeB,N> c = 1 - d;
        for (int i= 0; i< 2; ++i) {
            if ((i==0 && !lowerValid[0]) || (i && !upperValid[0]))
                continue;
            RealTypeB iwt = i ? d[0] : c[0];
            int ipos = i ? upperI[0] : lowerI[0];

            for (int j= 0; j< 2; ++j) {
                if ((j==0 && !lowerValid[1]) || (j && !upperValid[1]))
                    continue;
                RealTypeB jwt = iwt*(j ? d[1] : c[1]);
                int jpos = j ? upperI[1] : lowerI[1];

                for (int k= 0; k< 2; ++k) {
                    if ((k==0 && !lowerValid[2]) || (k && !upperValid[2]))
                        continue;
                    RealTypeB kwt = iwt*(k ? d[2] : c[2]);
                    int kpos = k ? upperI[2] : lowerI[2];

                    wtsum += kwt;
                    wtsum_valid=true;
                    result += static_cast<RealType>(kwt*array(ipos,jpos));
                }
            }
        }
        if (wtsum_valid)
            result /= static_cast<RealType>(wtsum);
        return result;
    }
    template<class RealTypeA, class RealTypeB>
    static RealTypeA linear_wgted(
        const blitz::Array<RealTypeA,N>& array, 
        const blitz::TinyVector<RealTypeB,N> d,
        const IndexVector lowerI,
        const IndexVector upperI,
        const blitz::TinyVector<bool,N> lowerValid,
        const blitz::TinyVector<bool,N> upperValid)
    {
        return linear_wgted_t<RealTypeA>(array,d,
                lowerI,upperI,lowerValid,upperValid);
    }
 
};// }}}
template<>
struct Interpolate<5> {// {{{
    enum {N = 5};
    typedef blitz::TinyVector<int,N> IndexVector;

    /**
     * Performs linear interpolation on a 3D data.
     */
    template<class RealTypeA, class RealTypeB>
    static void linear(
        blitz::Array<RealTypeA,N>& array, 
        const RealTypeA& value,
        const blitz::TinyVector<RealTypeB,N>& d,
        const IndexVector& lowerI,
        const IndexVector& upperI)
    {
        const blitz::TinyVector<RealTypeB,N> c = 1 - d;
        const blitz::TinyVector<int,N> index;
        for (int i= 0; i< 2; ++i) {
            RealTypeB iwt = i ? d[0] : c[0];
            int ipos = i ? upperI[0] : lowerI[0];

            for (int j= 0; j< 2; ++j) {
                RealTypeB jwt = iwt*(j ? d[1] : c[1]);
                int jpos = j ? upperI[1] : lowerI[1];

                for (int k= 0; k< 2; ++k) {
                    RealTypeB kwt = jwt*(k ? d[2] : c[2]);
                    int kpos = k ? upperI[2] : lowerI[2];

                    for (int l= 0; l< 2; ++l) {
                        RealTypeB lwt = kwt*(l ? d[3] : c[3]);
                        int lpos = l ? upperI[3] : lowerI[3];

                        for (int m= 0; m< 2; ++m) {
                            RealTypeB mwt = lwt*(m ? d[4] : c[4]);
                            int mpos = m ? upperI[4] : lowerI[4];
                            {
                            array(ipos,jpos,kpos, lpos, mpos) += 
                                static_cast<RealTypeA>(value*mwt);
                            }
                        }
                    }
                }
            }
        }
    }

    /**
     * Performs safe linear interoplation on a 3D data.
     * Safe here means that if bounds are reached for 
     * upper index, array is not incremented.
     */
    template<class RealTypeA, class RealTypeB>
    static void linear(
        blitz::Array<RealTypeA,N>& array, 
        const RealTypeA& value,
        const blitz::TinyVector<RealTypeB,N>& d,
        const IndexVector& lowerI,
        const IndexVector& upperI,
        const blitz::TinyVector<bool,N>& lowerValid,
        const blitz::TinyVector<bool,N>& upperValid)
    {
        const blitz::TinyVector<RealTypeB,N> c = 1 - d;
        for (int i= 0; i< 2; ++i) {
            if ((i==0 && !lowerValid[0]) || (i && !upperValid[0]))
                continue;
            RealTypeB iwt = i ? d[0] : c[0];
            int ipos = i ? upperI[0] : lowerI[0];

            for (int j= 0; j< 2; ++j) {
                if ((j==0 && !lowerValid[1]) || (j && !upperValid[1]))
                    continue;
                RealTypeB jwt = iwt*(j ? d[1] : c[1]);
                int jpos = j ? upperI[1] : lowerI[1];

                for (int k= 0; k< 2; ++k) {
                    if ((k==0 && !lowerValid[2]) || (k && !upperValid[2]))
                        continue;
                    RealTypeB kwt = jwt*(k ? d[2] : c[2]);
                    int kpos = k ? upperI[2] : lowerI[2];

                    for (int l= 0; l< 2; ++l) {
                        if ((l==0 && !lowerValid[3]) || (l && !upperValid[3]))
                            continue;
                        RealTypeB lwt = kwt*(l ? d[3] : c[3]);
                        int lpos = l ? upperI[3] : lowerI[3];

                        for (int m= 0; m< 2; ++m) {
                            if ((m==0 && !lowerValid[4]) || (m && !upperValid[4]))
                                continue;
                            RealTypeB mwt = lwt*(m ? d[4] : c[4]);
                            int mpos = m ? upperI[4] : lowerI[4];

                            {
                            array(ipos,jpos,kpos, lpos, mpos) += 
                                static_cast<RealTypeA>(value*mwt);
                            }
                        }
                    }
                }
            }
        }
    }
};// }}}

}
#endif // _LEAR_INTERPOLATE_H_
