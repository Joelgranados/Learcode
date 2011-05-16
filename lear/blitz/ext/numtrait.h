// {{{ file documentation
/**
 * @brief extension to blitz util in blitz namespace
 * @warning This file extends blitz's name space and doesnot use lear namespace
 */
// }}}

#ifndef _LEAR_EXT_NUMTRAIT_H_
#define _LEAR_EXT_NUMTRAIT_H_

#include <blitz/blitz.h>
#include <blitz/array.h>
#include <blitz/tinyvec.h>
#include <blitz/numtrait.h>

BZ_NAMESPACE(blitz)

#ifdef BZ_USE_NUMTRAIT

template<class P_numtype>
class ExtNumericTraits :public NumericTypeTraits<P_numtype> {
public:
    typedef P_numtype T_realtype;  // Type to be used for real valued calculations

    typedef P_numtype T_basictype;        // type 
    enum { numElements };          // number of elements in P_numtype 
    enum { hasNumElements = 0 };   // support number of elements
    
};

#define TMP_BZDECLNUMTRAIT(X,Y,Z,W,U,V)                             \
    template<>                                                      \
    class ExtNumericTraits<X> {                                     \
    public:                                                         \
        typedef Y T_sumtype;                                        \
        typedef Z T_difftype;                                       \
        typedef W T_floattype;                                      \
        typedef U T_signedtype;                                     \
                                                                    \
        typedef V T_realtype;                                       \
        enum { hasTrivialCtor = 1 };                                \
                                                                    \
        typedef X   T_basictype;                                    \
        enum { numElements  = 1};                                   \
        enum { hasNumElements = 1 };                                \
    }                                                               

#ifdef BZ_BOOL
    TMP_BZDECLNUMTRAIT(bool,unsigned,int,float,int, double);
#endif

TMP_BZDECLNUMTRAIT(char,int,int,float,char, double);
TMP_BZDECLNUMTRAIT(unsigned char, unsigned, int, float,int, double);
TMP_BZDECLNUMTRAIT(short int, int, int, float, short int, double);
TMP_BZDECLNUMTRAIT(short unsigned int, unsigned int, int, float, int, double);
TMP_BZDECLNUMTRAIT(int, long, int, float, int, double);
TMP_BZDECLNUMTRAIT(unsigned int, unsigned long, int, float, long, double);
TMP_BZDECLNUMTRAIT(long, long, long, double, long, double);
TMP_BZDECLNUMTRAIT(unsigned long, unsigned long, long, double, long, double);
// NOTE:: in blitz, float's sumtype and realtype is defined to be double
// we will use float as float's sumtype
TMP_BZDECLNUMTRAIT(float, float, float, float, float, float);
TMP_BZDECLNUMTRAIT(double, double, double, double, double, double);

#undef TMP_BZDECLNUMTRAIT

template<class T, int N>
class ExtNumericTraits<TinyVector<T,N> > {
    public:                                                         
    typedef TinyVector<typename ExtNumericTraits<T>::T_sumtype,   N > T_sumtype; 
    typedef TinyVector<typename ExtNumericTraits<T>::T_difftype,  N > T_difftype; 
    typedef TinyVector<typename ExtNumericTraits<T>::T_floattype, N > T_floattype; 
    typedef TinyVector<typename ExtNumericTraits<T>::T_signedtype,N > T_signedtype; 
    typedef TinyVector<typename ExtNumericTraits<T>::T_realtype,  N > T_realtype; 
    enum { hasTrivialCtor = 1 };

    typedef T   T_basictype; 
    enum { numElements  = N};
    enum { hasNumElements = 1 };
};

template<class T, int N>
class ExtNumericTraits<Array<T,N> > {
    public:                                                         
    typedef Array<typename ExtNumericTraits<T>::T_sumtype,   N > T_sumtype; 
    typedef Array<typename ExtNumericTraits<T>::T_difftype,  N > T_difftype; 
    typedef Array<typename ExtNumericTraits<T>::T_floattype, N > T_floattype; 
    typedef Array<typename ExtNumericTraits<T>::T_signedtype,N > T_signedtype; 
    typedef Array<typename ExtNumericTraits<T>::T_realtype,  N > T_realtype; 
    enum { hasTrivialCtor = 1 };

    typedef T   T_basictype; 
    enum { numElements  = N};
    enum { hasNumElements = 1 };
};

#endif // BZ_USE_NUMTRAIT

BZ_NAMESPACE_END

#endif // _LEAR_EXT_NUMTRAIT_H_
