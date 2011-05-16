// -*- C++ -*-
// blitz/funcs.h            Function objects for math functions
#ifndef _LEAR_BLITZ_STD_FUNCS_H_
#define _LEAR_BLITZ_STD_FUNCS_H_

#include <blitz/funcs.h>
#include <blitz/tinyvec.h>

BZ_NAMESPACE(blitz)
    
#define BZ_TV_MATHFN_SCOPE(x) blitz::x
/* Unary functions that return same type as argument */
    
#define BZ_DEFINE_TV_UNARY_FUNC(name,fun)                      \
template<typename InType, int N>                               \
struct name< TinyVector<InType,N> > {                          \
    typedef TinyVector<InType,N>  T_numtype1;                  \
    typedef T_numtype1 T_numtype;                              \
                                                               \
    static inline T_numtype                                    \
    apply(T_numtype1 a)                                        \
    { return fun(a); }                                         \
							       \
    template<typename T1>                                      \
    static inline void prettyPrint(BZ_STD_SCOPE(string) &str,  \
        prettyPrintFormat& format, const T1& t1)               \
    {                                                          \
        str += #fun;                                           \
        str += "(";                                            \
        t1.prettyPrint(str, format);                           \
        str += ")";                                            \
    }                                                          \
};
BZ_DEFINE_TV_UNARY_FUNC(Fn_acos,BZ_TV_MATHFN_SCOPE(acos))
BZ_DEFINE_TV_UNARY_FUNC(Fn_asin,BZ_TV_MATHFN_SCOPE(asin))
BZ_DEFINE_TV_UNARY_FUNC(Fn_atan,BZ_TV_MATHFN_SCOPE(atan))
BZ_DEFINE_TV_UNARY_FUNC(Fn_ceil,BZ_TV_MATHFN_SCOPE(ceil))
BZ_DEFINE_TV_UNARY_FUNC(Fn_cos,BZ_TV_MATHFN_SCOPE(cos))
BZ_DEFINE_TV_UNARY_FUNC(Fn_cosh,BZ_TV_MATHFN_SCOPE(cosh))
BZ_DEFINE_TV_UNARY_FUNC(Fn_exp,BZ_TV_MATHFN_SCOPE(exp))
BZ_DEFINE_TV_UNARY_FUNC(Fn_fabs,BZ_TV_MATHFN_SCOPE(fabs))
BZ_DEFINE_TV_UNARY_FUNC(Fn_floor,BZ_TV_MATHFN_SCOPE(floor))
BZ_DEFINE_TV_UNARY_FUNC(Fn_log,BZ_TV_MATHFN_SCOPE(log))
BZ_DEFINE_TV_UNARY_FUNC(Fn_log10,BZ_TV_MATHFN_SCOPE(log10))
BZ_DEFINE_TV_UNARY_FUNC(Fn_sin,BZ_TV_MATHFN_SCOPE(sin))
BZ_DEFINE_TV_UNARY_FUNC(Fn_sinh,BZ_TV_MATHFN_SCOPE(sinh))
BZ_DEFINE_TV_UNARY_FUNC(Fn_sqrt,BZ_TV_MATHFN_SCOPE(sqrt))
BZ_DEFINE_TV_UNARY_FUNC(Fn_tan,BZ_TV_MATHFN_SCOPE(tan))
BZ_DEFINE_TV_UNARY_FUNC(Fn_tanh,BZ_TV_MATHFN_SCOPE(tanh))

#ifdef BZ_HAVE_IEEE_MATH
BZ_DEFINE_TV_UNARY_FUNC(Fn_acosh,BZ_TV_MATHFN_SCOPE(acosh))
BZ_DEFINE_TV_UNARY_FUNC(Fn_asinh,BZ_TV_MATHFN_SCOPE(asinh))
BZ_DEFINE_TV_UNARY_FUNC(Fn_atanh,BZ_TV_MATHFN_SCOPE(atanh))
BZ_DEFINE_TV_UNARY_FUNC(Fn_cbrt,BZ_TV_MATHFN_SCOPE(cbrt))
BZ_DEFINE_TV_UNARY_FUNC(Fn_erf,BZ_TV_MATHFN_SCOPE(erf))
BZ_DEFINE_TV_UNARY_FUNC(Fn_erfc,BZ_TV_MATHFN_SCOPE(erfc))
BZ_DEFINE_TV_UNARY_FUNC(Fn_expm1,BZ_TV_MATHFN_SCOPE(expm1))
BZ_DEFINE_TV_UNARY_FUNC(Fn_j0,BZ_TV_MATHFN_SCOPE(j0))
BZ_DEFINE_TV_UNARY_FUNC(Fn_j1,BZ_TV_MATHFN_SCOPE(j1))
BZ_DEFINE_TV_UNARY_FUNC(Fn_lgamma,BZ_TV_MATHFN_SCOPE(lgamma))
BZ_DEFINE_TV_UNARY_FUNC(Fn_logb,BZ_TV_MATHFN_SCOPE(logb))
BZ_DEFINE_TV_UNARY_FUNC(Fn_log1p,BZ_TV_MATHFN_SCOPE(log1p))
BZ_DEFINE_TV_UNARY_FUNC(Fn_rint,BZ_TV_MATHFN_SCOPE(rint))
BZ_DEFINE_TV_UNARY_FUNC(Fn_y0,BZ_TV_MATHFN_SCOPE(y0))
BZ_DEFINE_TV_UNARY_FUNC(Fn_y1,BZ_TV_MATHFN_SCOPE(y1))
#endif
    
#ifdef BZ_HAVE_SYSTEM_V_MATH
BZ_DEFINE_TV_UNARY_FUNC(Fn__class,BZ_TV_MATHFN_SCOPE(_class))
BZ_DEFINE_TV_UNARY_FUNC(Fn_nearest,BZ_TV_MATHFN_SCOPE(nearest))
BZ_DEFINE_TV_UNARY_FUNC(Fn_rsqrt,BZ_TV_MATHFN_SCOPE(rsqrt))
#endif
    
/* Unary functions that return a specified type */
    
#define BZ_DEFINE_TV_UNARY_FUNC_RET(name,fun,ret)              \
template<typename InType, int N>                               \
struct name< TinyVector<InType,N> > {                          \
    typedef TinyVector<InType,N>  T_numtype1;                  \
    typedef TinyVector<ret,N>     T_numtype;                   \
                                                               \
    static inline T_numtype                                    \
    apply(T_numtype1 a)                                        \
    { return fun(a); }                                         \
                                                               \
    template<typename T1>                                      \
    static inline void prettyPrint(BZ_STD_SCOPE(string) &str,  \
        prettyPrintFormat& format, const T1& t1)               \
    {                                                          \
        str += #fun;                                           \
        str += "(";                                            \
        t1.prettyPrint(str, format);                           \
        str += ")";                                            \
    }                                                          \
};

#ifdef BZ_HAVE_IEEE_MATH
BZ_DEFINE_TV_UNARY_FUNC_RET(Fn_ilogb,BZ_TV_MATHFN_SCOPE(ilogb),int)
#endif
    
#ifdef BZ_HAVE_SYSTEM_V_MATH
BZ_DEFINE_TV_UNARY_FUNC_RET(Fn_itrunc,BZ_TV_MATHFN_SCOPE(itrunc),int)
BZ_DEFINE_TV_UNARY_FUNC_RET(Fn_uitrunc,BZ_TV_MATHFN_SCOPE(uitrunc),unsigned int)
#endif
    
    
/* Binary functions that return type based on type promotion */

#define BZ_DEFINE_TV_BINARY_FUNC(name,fun)                        \
template<typename InType1, typename InType2, int N>               \
struct name < TinyVector<InType1, N>, TinyVector<InType2, N> > {  \
    typedef BZ_PROMOTE(InType1, InType2) InType;                  \
    typedef TinyVector<InType1,N>  T_numtype1;                    \
    typedef TinyVector<InType2,N>  T_numtype2;                    \
    typedef TinyVector<InType ,N>  T_numtype ;                    \
                                                                  \
    static inline T_numtype                                       \
    apply(T_numtype1 a, T_numtype2 b)                             \
    { return fun(a,b); }                                          \
                                                                  \
    template<typename T1, typename T2>                            \
    static inline void prettyPrint(BZ_STD_SCOPE(string) &str,     \
        prettyPrintFormat& format, const T1& t1,                  \
        const T2& t2)                                             \
    {                                                             \
        str += #fun;                                              \
        str += "(";                                               \
        t1.prettyPrint(str, format);                              \
        str += ",";                                               \
        t2.prettyPrint(str, format);                              \
        str += ")";                                               \
    }                                                             \
};

BZ_DEFINE_TV_BINARY_FUNC(Fn_atan2,BZ_TV_MATHFN_SCOPE(atan2))
BZ_DEFINE_TV_BINARY_FUNC(Fn_fmod,BZ_TV_MATHFN_SCOPE(fmod))
BZ_DEFINE_TV_BINARY_FUNC(Fn_pow,BZ_TV_MATHFN_SCOPE(pow))
    
#ifdef BZ_HAVE_SYSTEM_V_MATH
BZ_DEFINE_TV_BINARY_FUNC(Fn_copysign,BZ_TV_MATHFN_SCOPE(copysign))
BZ_DEFINE_TV_BINARY_FUNC(Fn_drem,BZ_TV_MATHFN_SCOPE(drem))
BZ_DEFINE_TV_BINARY_FUNC(Fn_hypot,BZ_TV_MATHFN_SCOPE(hypot))
BZ_DEFINE_TV_BINARY_FUNC(Fn_nextafter,BZ_TV_MATHFN_SCOPE(nextafter))
BZ_DEFINE_TV_BINARY_FUNC(Fn_remainder,BZ_TV_MATHFN_SCOPE(remainder))
BZ_DEFINE_TV_BINARY_FUNC(Fn_scalb,BZ_TV_MATHFN_SCOPE(scalb))
#endif
    
/* Binary functions that return a specified type */
    
#define BZ_DEFINE_TV_BINARY_FUNC_RET(name,fun,ret)                \
template<typename InType1, typename InType2, int N>               \
struct name < TinyVector<InType1, N>, TinyVector<InType2, N> > {  \
    typedef ret InType;                                           \
    typedef TinyVector<InType1,N>  T_numtype1;                    \
    typedef TinyVector<InType2,N>  T_numtype2;                    \
    typedef TinyVector<InType ,N>  T_numtype ;                    \
                                                                  \
    static inline T_numtype                                       \
    apply(T_numtype1 a, T_numtype2 b)                             \
    { return fun(a,b); }                                          \
                                                                  \
    template<typename T1, typename T2>                            \
    static inline void prettyPrint(BZ_STD_SCOPE(string) &str,     \
        prettyPrintFormat& format, const T1& t1,                  \
        const T2& t2)                                             \
    {                                                             \
        str += #fun;                                              \
        str += "(";                                               \
        t1.prettyPrint(str, format);                              \
        str += ",";                                               \
        t2.prettyPrint(str, format);                              \
        str += ")";                                               \
    }                                                             \
};

#ifdef BZ_HAVE_SYSTEM_V_MATH
BZ_DEFINE_TV_BINARY_FUNC_RET(Fn_unordered,BZ_TV_MATHFN_SCOPE(unordered),int)
#endif
    
    
/* These functions don't quite fit the usual patterns */
    
// abs()    Absolute value
template<typename InType, int N >
struct Fn_abs< TinyVector<InType,N > > {
    typedef TinyVector<InType,N> T_numtype1;
    typedef T_numtype1 T_numtype;
    
    static inline T_numtype
    apply(T_numtype1 a)
    { return BZ_TV_MATHFN_SCOPE(abs)(a); }
    
    template<typename T1>
    static inline void prettyPrint(BZ_STD_SCOPE(string) &str,
        prettyPrintFormat& format, const T1& t1)
    {
        str += "abs";
        str += "(";
        t1.prettyPrint(str, format);
        str += ")";
    }
};

#ifdef BZ_HAVE_IEEE_MATH
// isnan()    Nonzero if NaNS or NaNQ
template<typename InType, int N>
struct Fn_isnan < TinyVector<InType,N> > {
    typedef TinyVector<InType,N> T_numtype1;
    typedef TinyVector<int,N> T_numtype;
    
    static inline T_numtype
    apply(T_numtype1 a)
    {
#ifdef isnan
        // Some platforms define isnan as a macro, which causes the
        // BZ_TV_MATHFN_SCOPE macro to break.
        return isnan(a); 
#else
#ifdef BZ_ISNAN_IN_NAMESPACE_STD
        return BZ_STD_SCOPE(isnan)(a);
#else
        return BZ_IEEEMATHFN_SCOPE(isnan)(a);
#endif
#endif
    }
    
    template<typename T1>
    static inline void prettyPrint(BZ_STD_SCOPE(string) &str,
        prettyPrintFormat& format, const T1& t1)
    {
        str += "isnan";
        str += "(";
        t1.prettyPrint(str, format);
        str += ")";
    }
};
#endif // BZ_HAVE_IEEE_MATH


BZ_NAMESPACE_END

#endif // _LEAR_BLITZ_STD_FUNCS_H_
