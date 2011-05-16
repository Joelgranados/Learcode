#ifndef _LEAR_STENCIL_OPS_H_
#define _LEAR_STENCIL_OPS_H_

#include <blitz/array.h>
#include <blitz/tinyvec.h>
#include <blitz/array/stencil-et.h>

BZ_NAMESPACE(blitz)

    
//{{{ Blitz util macros

// Declare Macros for stencil operators with templates. 
#define BZ_DECLARE_APPLY1 \
template<class T1, class T2, class T3, class T4, class T5, class T6, \
    class T7, class T8, class T9, class T10, class T11>         \
inline void apply(T1& R, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) const

// Declare Macros for stencil operators with templates. 
#define BZ_DECLARE_APPLY2 \
template<class T1, class T2, class T3, class T4, class T5, class T6, \
    class T7, class T8, class T9, class T10, class T11>         \
inline void apply(T1& R, T2& A, T3, T4, T5, T6, T7, T8, T9, T10, T11) const

// Declare Macros for stencil operators with templates. 
#define BZ_DECLARE_APPLY3 \
template<class T1, class T2, class T3, class T4, class T5, class T6, \
    class T7, class T8, class T9, class T10, class T11>         \
inline void apply(T1& R, T2& A, T3& B, T4, T5, T6, T7, T8, T9, T10, T11) const

//}}}

//  // {{{ derivative operators
//  template<int Dim>
//  struct derv1Order2DimN {
//      BZ_DECLARE_APPLY2(A,R)
//          R = central12(A,Dim);
//      BZ_END_STENCIL

//  template<int Dim>
//  struct derv2Order2DimN {
//      BZ_DECLARE_APPLY2(A,R)
//          R = central22(A,Dim);
//      BZ_END_STENCIL

//  template<int Dim>
//  struct derv3Order2DimN {
//      BZ_DECLARE_APPLY2(A,R)
//          R = central32(A,Dim);
//      BZ_END_STENCIL

//  template<int Dim>
//  struct derv4Order2DimN {
//      BZ_DECLARE_APPLY2(A,R)
//          R = central42(A,Dim);
//      BZ_END_STENCIL
//  // BZ_END_STENCIL_WITH_SHAPE(shape(-1,-1),shape(+1,+1))

//  // }}}
//{{{ Laplacian operator. Iso Symmetric
// defines isosymmetric laplacian. To normalize, divide by 6
BZ_DECLARE_STENCIL2(isoLaplacian2D, R, A)
	R(0,0) = -20.0 * A(0,0) +
		(A(-1,0) + A(1,0) + A(0,-1) + A(0,1) )* 4 +
		(A(-1,-1) + A(1,-1) + A(1,1) + A(-1,1) );
BZ_END_STENCIL_WITH_SHAPE(shape(-1,-1),shape(+1,+1))
//}}}

//{{{ 2nd derivative operator. angle 60
// defines 2nd order derivatives
    // 2nd derv, 0 angle
BZ_DECLARE_STENCIL2(central22_000, R, A)
    R(0,0) =  A(-1, 0) - 2.0000*A( 0, 0) + A(1,0)
    ;
BZ_END_STENCIL_WITH_SHAPE(shape(-1,-1),shape(+1,+1))

BZ_DECLARE_STENCIL2(central22_090, R, A)
    R(0,0) =  A( 0,-1) - 2.0000*A( 0, 0) + A(0,1)
    ;
BZ_END_STENCIL_WITH_SHAPE(shape(-1,-1),shape(+1,+1))

    // 2nd derv, 60 angle
BZ_DECLARE_STENCIL2(central22_060, R, A)
    R(0,0) =                   0.2990*A(-1,-1) + 0.4019*A(-1,1)         
            -0.0670*A( 0,-1) - 2.0000*A( 0, 0) - 0.0670*A(0,1)
            +0.4019*A( 1,-1) + 0.2990*A( 1, 0)
    ;
BZ_END_STENCIL_WITH_SHAPE(shape(-1,-1),shape(+1,+1))

//2nd derivative operator. angle 120
BZ_DECLARE_STENCIL2(central22_120, R, A)
    R(0,0) = +0.4019*A(-1, 1) + 0.2990*A(-1,-1)
             -0.0670*A( 0,-1) - 2.000*A( 0, 0) -0.0670*A(0,1)
                              +0.2990*A( 1, 0) + 0.4019*A( 1,-1)
    ;
BZ_END_STENCIL_WITH_SHAPE(shape(-1,-1),shape(+1,+1))
//}}}
BZ_DECLARE_STENCIL2(diag045derv1, R, A)
    R(0,0) = - A(0,0) + A( 1,1);
BZ_END_STENCIL_WITH_SHAPE(shape(0,0),shape(1,1))

BZ_DECLARE_STENCIL2(diag135derv1, R, A)
    R(0,0) = - A(1,0) + A( 0,1);
BZ_END_STENCIL_WITH_SHAPE(shape(0,0),shape(1,1))

struct CentralDerv1Order2 {// {{{
    CentralDerv1Order2(const int dimension) : dimension(dimension) {}

    BZ_DECLARE_APPLY2
    {
        R = A.shift(1,dimension) - A.shift(-1,dimension);
    }

    template<int N>
    void getExtent(TinyVector<int,N>& minb, TinyVector<int,N>& maxb) const
    {
        minb = 0; maxb = 0;
        minb[dimension] = -1;
        maxb[dimension] = 1;
    }
    enum { hasExtent = 1 }; 
    private:
        int dimension;
};// }}}
struct CentralDerv1Gap {// {{{
    CentralDerv1Gap(const int dimension, const int gap) : 
        dimension(dimension) , gap(gap)
    {}

    BZ_DECLARE_APPLY2
    {
        R = A.shift(gap,dimension) - A.shift(-gap,dimension);
    }

    template<int N>
    void getExtent(TinyVector<int,N>& minb, TinyVector<int,N>& maxb) const
    {
        minb = 0; maxb = 0;
        minb[dimension] = -gap;
        maxb[dimension] = gap;
    }
    enum { hasExtent = 1 }; 
    private:
        int dimension, gap;
};// }}}
struct CentralDerv1Order4 {// {{{
    CentralDerv1Order4(const int dimension) : dimension(dimension) {}

    BZ_DECLARE_APPLY2
    {
        //R = 0.13534f*A.shift( 2,dimension) + 0.60653f*A.shift( 1,dimension) 
         //  -0.13534f*A.shift(-2,dimension) - 0.60653f*A.shift(-1,dimension);
        R =-A.shift( 2,dimension) + 8*A.shift( 1,dimension) 
           +A.shift(-2,dimension) - 8*A.shift(-1,dimension);
    }

    template<int N>
    inline void getExtent(TinyVector<int,N>& minb, TinyVector<int,N>& maxb)const
    {
        minb = 0; maxb = 0;
        minb[dimension] = -2;
        maxb[dimension] = 2;
    }
    enum { hasExtent = 1 }; 
    private:
        int dimension;
};// }}}
struct CentralDerv2Order2 {// {{{
    CentralDerv2Order2(const int dimension) : dimension(dimension) {}

    BZ_DECLARE_APPLY2
    {
        R = A.shift(1,dimension) - 2*(*A) + A.shift(-1,dimension);
    }

    template<int N>
    void getExtent(TinyVector<int,N>& minb, TinyVector<int,N>& maxb) const
    {
        minb = 0; maxb = 0;
        minb[dimension] = -1;
        maxb[dimension] = 1;
    }
    enum { hasExtent = 1 }; 
    private:
        int dimension;
};// }}}
struct Integ1Order1{// {{{
    Integ1Order1(const int dimension) : dimension(dimension) {}

    BZ_DECLARE_APPLY2
    {
        R =A.shift( 1,dimension) + A.shift(0,dimension);
    }

    template<int N>
    inline void getExtent(TinyVector<int,N>& minb, TinyVector<int,N>& maxb)const
    {
        minb = 0; maxb = 0;
        maxb[dimension] = 1;
    }
    enum { hasExtent = 1 }; 
    private:
        int dimension;
};// }}}
struct Derv1Order1 {// {{{
    Derv1Order1(const int dimension) : dimension(dimension) {}

    BZ_DECLARE_APPLY2
    {
        R =A.shift( 1,dimension) - A.shift(0,dimension);
    }

    template<int N>
    inline void getExtent(TinyVector<int,N>& minb, TinyVector<int,N>& maxb)const
    {
        minb = 0; maxb = 0;
        maxb[dimension] = 1;
    }
    enum { hasExtent = 1 }; 
    private:
        int dimension;
};// }}}
struct CustomLap2D {// {{{
    CustomLap2D(const float wa=1, const float wb=1) : wa(wa),wb(wb/2){}

    BZ_DECLARE_APPLY2
    {
	R(0,0) = wa*(-4*A(0,0) + (A(-1, 0) + A(1, 0) + A(0,-1) + A( 0,1))) +
                 wb*(-4*A(0,0) + (A(-1,-1) + A(1,-1) + A(1, 1) + A(-1,1)));
    }

    template<int N>
    inline void getExtent(TinyVector<int,N>& minb, TinyVector<int,N>& maxb)const
    {
        minb = -1; maxb = 1;
    }
    enum { hasExtent = 1 }; 
    private:
        float wa,wb;
};// }}}

// {{{ LessThanStrict/LessThanEqual,GreaterThanEqual,GreaterThanStrict
struct LessThanEqual {
    template<class TA, class TB>
    inline static bool compare(TA a, TB b) {
        return a <= b;
    }
    template<class TA>
    inline static void decrement(TA& a) {
        ++a;
    }
    template<class TA>
    inline static void increment(TA& a) {
        --a;
    }
};
struct LessThanStrict {
    template<class TA, class TB>
    inline static bool compare(TA a, TB b) {
        return a < b;
    }
    template<class TA>
    inline static void decrement(TA& a) {
        ++a;
    }
    template<class TA>
    inline static void increment(TA& a) {
        --a;
    }
};
struct GreaterThanEqual {
    template<class TA, class TB>
    inline static bool compare(TA a, TB b) {
        return a >= b;
    }
    template<class TA>
    inline static void decrement(TA& a) {
        --a;
    }
    template<class TA>
    inline static void increment(TA& a) {
        ++a;
    }
};
struct GreaterThanStrict {
    template<class TA, class TB>
    inline static bool compare(TA a, TB b) {
        return a > b;
    }
    template<class TA>
    inline static void decrement(TA& a) {
        --a;
    }
    template<class TA>
    inline static void increment(TA& a) {
        ++a;
    }
};
// }}}

// {{{ struct localwindow_cmp start                 				
#define  LOCAL_WINDOW_COMPARE_START \
    protected:  \
    TinyVector<int,Rank> stencilWindow; \
    public:     \
    localwindow_cmp(const TinyVector<int,Rank> stencilWindow) : stencilWindow(stencilWindow){}

#define  LOCAL_WINDOW_COMPARE_END \
    template<int N> \
    void getExtent(TinyVector<int,N>& minb, TinyVector<int,N>& maxb) const \
    { \
        minb = -stencilWindow; \
        maxb = stencilWindow; \
    } \
    enum { hasExtent = 1 }; 

template<class ElementType, int Rank, class Cmp > 								
struct localwindow_cmp {                 				
    LOCAL_WINDOW_COMPARE_START
    // apply not implemented. we donot know the rank in general.
    LOCAL_WINDOW_COMPARE_END
};
// }}}
// {{{ localwindow_cmp<ElementType,3>
template<class ElementType, class Cmp> 								
struct localwindow_cmp<ElementType,3,Cmp> {
    enum {Rank = 3};

    LOCAL_WINDOW_COMPARE_START

    BZ_DECLARE_APPLY2
    {
        ElementType val = A(0,0,0);
        Cmp::decrement(A(0,0,0));
        bool ismax = true;
        for (int i= -stencilWindow[0]; i<= stencilWindow[0] ; ++i) 
        for (int j= -stencilWindow[1]; j<= stencilWindow[1] ; ++j) 
        for (int k= -stencilWindow[2]; k<= stencilWindow[2] ; ++k)  {
            ismax =Cmp::compare(val, A(i,j,k));
            if (!ismax) {
                Cmp::increment(A(0,0,0));
                R = ismax; 
                return;
            }
        }

        Cmp::increment(A(0,0,0));
        R = ismax; 
    }

    LOCAL_WINDOW_COMPARE_END
};
//}}}
// {{{ localwindow_cmp<ElementType,2>
template<class ElementType,class Cmp> 								
struct localwindow_cmp<ElementType,2,Cmp> {
    enum {Rank = 2};

    LOCAL_WINDOW_COMPARE_START

    BZ_DECLARE_APPLY2
    {
        ElementType val = A(0,0);
        Cmp::decrement(A(0,0)); // decrement A for now. this way, we avoid if i!=0 && j!=0 condition in inner for loop
        bool ismax = true;
        for (int i= -stencilWindow[0]; i<= stencilWindow[0] ; ++i) 
        for (int j= -stencilWindow[1]; j<= stencilWindow[1] ; ++j) 
        {
            ismax =Cmp::compare(val, A(i,j));
            if (!ismax) {
                Cmp::increment(A(0,0));
                R = ismax; 
                return;
            }
        }

        Cmp::increment(A(0,0));
        R = ismax; 
    }

    LOCAL_WINDOW_COMPARE_END
};
// }}}
// {{{ localwindow_cmp<ElementType,1>
template<class ElementType,class Cmp> 								
struct localwindow_cmp<ElementType,1,Cmp> {
    enum {Rank = 1};

    LOCAL_WINDOW_COMPARE_START

    BZ_DECLARE_APPLY2
    {
        ElementType val = A(0);
        Cmp::decrement(A(0));  // decrement A for now. this way, we avoid if i!=0 condition in inner for loop
        bool ismax = true;
        for (int i= -stencilWindow[0]; i<= stencilWindow[0] ; ++i) 
        {
            ismax =Cmp::compare(val, A(i));
            if (!ismax) {
                Cmp::increment(A(0));
                R = ismax; 
                return;
            }
        }

        Cmp::increment(A(0));
        R = ismax; 
    }

    LOCAL_WINDOW_COMPARE_END
};
// }}}
// {{{ struct localmin_window end                 				
#undef LOCAL_WINDOW_COMPARE_START
#undef LOCAL_WINDOW_COMPARE_END
//}}}

BZ_NAMESPACE_END
#endif	//#ifndef _LEAR_STENCIL_OPS_H_
