// {{{ file documentation
/**
 * @file 
 * @author Navneet Dalal
 * @brief Defines some useful utilities for TinyVector and TinyMatrix. 
 *
 * @warning Some functions may not be defined using blitz Expression Templates
 * and Meta Programming techniques.
 */
// }}}

#ifndef _LEAR_EXT_TVMUTILS_H_
#define _LEAR_EXT_TVMUTILS_H_

// {{{ headers
#include <blitz/array.h>
#include <blitz/tinyvec.h>
#include <blitz/tinymat.h>
// }}}

BZ_NAMESPACE(blitz)

//{{{ isInRange functions
template<class TypeA, class TypeB, class TypeC, int Rank>
inline bool isInRange(
        const TinyVector<TypeA, Rank>& p, 
        const TinyVector<TypeB, Rank>& cMin, 
        const TinyVector<TypeC, Rank>& cMax) 
{
    for (int i= 0; i< Rank; ++i) 
        if (p[i] < cMin[i] || p[i] > cMax[i])
            return false;
    return true;
}

#define TMP_ISINRANGE(TYPE)                                                 \
inline bool isInRange(const TYPE p, const TYPE cMin, const TYPE cMax){      \
    if (p < cMin || p > cMax)                                               \
            return false;                                                   \
    return true;                                                            \
} 

TMP_ISINRANGE(double)
TMP_ISINRANGE(long double)
TMP_ISINRANGE(float)
TMP_ISINRANGE(int)
TMP_ISINRANGE(unsigned int)
TMP_ISINRANGE(short)
TMP_ISINRANGE(unsigned short)
TMP_ISINRANGE(long)
TMP_ISINRANGE(unsigned long)
TMP_ISINRANGE(char)
TMP_ISINRANGE(unsigned char)

#undef TMP_ISINRANGE
//}}}

// {{{ dot
/* 
 * @def dot
 * @brief Enables dot to be called for blitz array.
 *
 * For e.g. if A , B and C are blitz arrays, and we want to call dot product for each element,
 * we can simply use this statment
 * A = dot(B,C);
 * instead of writing for loops.
 */
// @{

#define TMP_SUM(TYPE)           \
inline double dot(const TYPE a, const TYPE b){  \
    return a*b;                   \
}

TMP_SUM(double)
TMP_SUM(long double)
TMP_SUM(float)
TMP_SUM(int)
TMP_SUM(unsigned int)
TMP_SUM(short)
TMP_SUM(unsigned short)
TMP_SUM(long)
TMP_SUM(unsigned long)
TMP_SUM(char)
TMP_SUM(unsigned char)

#undef TMP_SUM

template<class T>
inline double dist(const T a) {
    return dot(a,a);
}
BZ_DECLARE_FUNCTION2_RET(dot,double)
BZ_DECLARE_FUNCTION_RET(dist,double)
// }}}
// template<class T, int N>
// inline TinyVector<T,N> sqrt(const TinyVector<T,N>& a) {
//     TinyVector<T,N> val;
//     for (int i= 0; i<N ; ++i) 
//         val(i) = sqrt(a(i));
//     return val;
// }
// BZ_DECLARE_FUNCTION_RET(dist,TinyVector<T,N>)

// {{{ isEqual functions
/** 
 * @defgroup isEqual_Functions Overloaded isEqual functions
 * @brief Defines isEqual function for int, double and TinyVector.
 *
 * isEqual compares element by element and returns true only if all elements are equal.
 *
 */
// @{ 
#define TMP_ISEQUAL(TYPE)                                                  \
inline bool isEqual(const TYPE a, const TYPE b) {                           \
    return a == b;                                                          \
}

TMP_ISEQUAL(double)
TMP_ISEQUAL(long double)
TMP_ISEQUAL(float)
TMP_ISEQUAL(int)
TMP_ISEQUAL(unsigned int)
TMP_ISEQUAL(short)
TMP_ISEQUAL(unsigned short)
TMP_ISEQUAL(long)
TMP_ISEQUAL(unsigned long)
TMP_ISEQUAL(char)
TMP_ISEQUAL(unsigned char)

#undef TMP_ISEQUAL

template<class T1, class T2, int N>
inline bool isEqual(const TinyVector<T1,N>&a, const TinyVector<T2,N>&b) {
    for (int i= 0; i<N ; ++i) 
        if (a[i] != b[i])
            return false;
    return true;
}

BZ_DECLARE_FUNCTION2_RET(isEqual,bool)

// @}
// }}}
// {{{ isNotEqual functions
/** 
 * @defgroup isNotEqual Overloaded isNotEqual functions
 * @brief Defines isNotEqual function for int and TinyVector.
 *
 * isNotEqual compares element by element and returns true only if all elements are equal.
 *
 */
// @{ 
#define TMP_ISNOTEQUAL(TYPE)                                                  \
inline bool isNotEqual(const TYPE a, const TYPE b) {                           \
    return a != b;                                                          \
}

TMP_ISNOTEQUAL(int)
TMP_ISNOTEQUAL(unsigned int)
TMP_ISNOTEQUAL(short)
TMP_ISNOTEQUAL(unsigned short)
TMP_ISNOTEQUAL(long)
TMP_ISNOTEQUAL(unsigned long)
TMP_ISNOTEQUAL(char)
TMP_ISNOTEQUAL(unsigned char)

#undef TMP_ISNOTEQUAL

template<class T1, class T2, int N>
inline bool isNotEqual(const TinyVector<T1,N>&a, const TinyVector<T2,N>&b) {
    for (int i= 0; i<N ; ++i) 
        if (a[i] != b[i])
            return true;
    return false;
}

BZ_DECLARE_FUNCTION2_RET(isNotEqual,bool)

// @}
// }}}
// {{{ isGreater functions
/** 
 * @defgroup isGreater Overloaded isEqual functions
 * @brief Defines isGreater function for int, double and TinyVector.
 *
 * isGreater compares element by element and returns true only if all elements 
 * of a are are Greater than b.
 *
 */
// @{ 
#define TMP_ISGREATER(TYPE)                                     \
inline bool isGreater(const TYPE a, const TYPE b) {             \
    return a > b;                                               \
}

TMP_ISGREATER(double)
TMP_ISGREATER(long double)
TMP_ISGREATER(float)
TMP_ISGREATER(int)
TMP_ISGREATER(unsigned int)
TMP_ISGREATER(short)
TMP_ISGREATER(unsigned short)
TMP_ISGREATER(long)
TMP_ISGREATER(unsigned long)
TMP_ISGREATER(char)
TMP_ISGREATER(unsigned char)

#undef TMP_ISGREATER

template<class T1, class T2, int N>
inline bool isGreater(const TinyVector<T1,N>&a, const TinyVector<T2,N>&b) {
    for (int i= 0; i<N ; ++i) 
        if (a[i] <= b[i])
            return false;
    return true;
}

BZ_DECLARE_FUNCTION2_RET(isGreater,bool)

// @}
// }}}
// {{{ isGreaterEqual functions
/** 
 * @defgroup isGreaterEqual Overloaded isGreaterEqual functions
 * @brief Defines isGreaterEqual function for int, double and TinyVector.
 *
 * isGreaterEqual compares element by element and returns true only if all elements 
 * of a are GreaterEqual than b.
 *
 */
// @{ 
#define TMP_ISGREATEREQUAL(TYPE)                                          \
inline bool isGreaterEqual(const TYPE a, const TYPE b) {                  \
    return a >= b;                                                         \
}

TMP_ISGREATEREQUAL(double)
TMP_ISGREATEREQUAL(long double)
TMP_ISGREATEREQUAL(float)
TMP_ISGREATEREQUAL(int)
TMP_ISGREATEREQUAL(unsigned int)
TMP_ISGREATEREQUAL(short)
TMP_ISGREATEREQUAL(unsigned short)
TMP_ISGREATEREQUAL(long)
TMP_ISGREATEREQUAL(unsigned long)
TMP_ISGREATEREQUAL(char)
TMP_ISGREATEREQUAL(unsigned char)

#undef TMP_ISGREATEREQUAL 

template<class T1, class T2, int N>
inline bool isGreaterEqual(const TinyVector<T1,N>&a, const TinyVector<T2,N>&b) {
    for (int i= 0; i<N ; ++i) 
        if (a[i] < b[i])
            return false;
    return true;
}

BZ_DECLARE_FUNCTION2_RET(isGreaterEqual,bool)
// @}
// }}}
// {{{ isLess functions
/** 
 * @defgroup isLess Overloaded isLess functions
 * @brief Defines isLess function for int, double and TinyVector.
 *
 * isLess compares element by element and returns true only if all elements 
 * of a are are Greater than b.
 *
 */
// @{ 
#define TMP_ISLESS(TYPE)                                     \
inline bool isLess(const TYPE a, const TYPE b) {             \
    return a < b;                                               \
}

TMP_ISLESS(double)
TMP_ISLESS(long double)
TMP_ISLESS(float)
TMP_ISLESS(int)
TMP_ISLESS(unsigned int)
TMP_ISLESS(short)
TMP_ISLESS(unsigned short)
TMP_ISLESS(long)
TMP_ISLESS(unsigned long)
TMP_ISLESS(char)
TMP_ISLESS(unsigned char)

#undef TMP_ISGREATER

template<class T1, class T2, int N>
inline bool isLess(const TinyVector<T1,N>&a, const TinyVector<T2,N>&b) {
    for (int i= 0; i<N ; ++i) 
        if (a[i] >= b[i])
            return false;
    return true;
}

BZ_DECLARE_FUNCTION2_RET(isLess,bool)

// @}
// }}}
// {{{ isLessEqual functions
/** 
 * @defgroup isLessEqual Overloaded isLessEqual functions
 * @brief Defines isLessEqual function for int, double and TinyVector.
 *
 * isLessEqual compares element by element and returns true only if all elements 
 * of a are GreaterEqual than b.
 *
 */
// @{ 
#define TMP_ISLESSEQUAL(TYPE)                                          \
inline bool isLessEqual(const TYPE a, const TYPE b) {                  \
    return a <= b;                                                         \
}

TMP_ISLESSEQUAL(double)
TMP_ISLESSEQUAL(long double)
TMP_ISLESSEQUAL(float)
TMP_ISLESSEQUAL(int)
TMP_ISLESSEQUAL(unsigned int)
TMP_ISLESSEQUAL(short)
TMP_ISLESSEQUAL(unsigned short)
TMP_ISLESSEQUAL(long)
TMP_ISLESSEQUAL(unsigned long)
TMP_ISLESSEQUAL(char)
TMP_ISLESSEQUAL(unsigned char)

#undef TMP_ISLESSEQUAL 

template<class T1, class T2, int N>
inline bool isLessEqual(const TinyVector<T1,N>&a, const TinyVector<T2,N>&b) {
    for (int i= 0; i<N ; ++i) 
        if (a[i] > b[i])
            return false;
    return true;
}

BZ_DECLARE_FUNCTION2_RET(isLessEqual,bool)
// @}
// }}}

// {{{ isZero functions
/**
 * @defgroup isZero_Functions Overloaded isZero functions
 * @brief Defines isZero function for int, double and TinyVector.
 *
 * isZero compares element by element and returns true only if all elements are zero.
 *
 */
// @{

#define TMP_ISZERO(TYPE)                                                  \
inline bool isZero(const TYPE a) {                           \
    return a == 0;                                                          \
}

TMP_ISZERO(double)
TMP_ISZERO(long double)
TMP_ISZERO(float)
TMP_ISZERO(int)
TMP_ISZERO(unsigned int)
TMP_ISZERO(short)
TMP_ISZERO(unsigned short)
TMP_ISZERO(long)
TMP_ISZERO(unsigned long)
TMP_ISZERO(char)
TMP_ISZERO(unsigned char)

#undef TMP_ISZERO

template<class T, int N>
inline bool isZero(const TinyVector<T,N>&a) {
    for (int i=0; i<N; ++i) 
        if (a[i] != 0)
            return false;
    return true;
}

BZ_DECLARE_FUNCTION_RET(isZero,bool)

// @}
// }}}

// {{{ sum functions
/**
 * @defgroup sum_Functions Overloaded sum functions
 * @brief Defines sum function for int, double and TinyVector.
 *
 * Add elements of int, double and TinyVector and returns the sum of all elements.
 *
 */
// @{

#define TMP_SUM(TYPE)           \
inline TYPE sum(const TYPE a){  \
    return a;                   \
}

TMP_SUM(double)
TMP_SUM(long double)
TMP_SUM(float)
TMP_SUM(int)
TMP_SUM(unsigned int)
TMP_SUM(short)
TMP_SUM(unsigned short)
TMP_SUM(long)
TMP_SUM(unsigned long)
TMP_SUM(char)
TMP_SUM(unsigned char)

#undef TMP_SUM
// @}
// }}}
// {{{ product functions
/**
 * @defgroup product_Functions Overloaded product functions
 * @brief Defines product function for int, double and TinyVector.
 *
 * Add elements of int, double and TinyVector and returns the product of all elements.
 *
 */
// @{

#define TMP_PRODUCT(TYPE)           \
inline TYPE product(const TYPE a){  \
    return a;                   \
}

TMP_PRODUCT(double)
TMP_PRODUCT(long double)
TMP_PRODUCT(float)
TMP_PRODUCT(int)
TMP_PRODUCT(unsigned int)
TMP_PRODUCT(short)
TMP_PRODUCT(unsigned short)
TMP_PRODUCT(long)
TMP_PRODUCT(unsigned long)
TMP_PRODUCT(char)
TMP_PRODUCT(unsigned char)

#undef TMP_PRODUCT
// 
// template<class T, int N>
// inline T product(const TinyVector<T,N>&a) {
//     T value=1;
//     for (int i= 0; i<N ; ++i) 
//         value *= a[i];
//     return value;
// }
// @}
// }}}
// {{{ max min functions
/**
 * @defgroup Max_Min Overloaded Max Min functions
 * @brief Defines Max, Min function for int, double and TinyVector.
 *
 */
// @{

#define TMP_MAXMIN(TYPE)           \
inline TYPE MyMax(const TYPE a, const TYPE b){  \
    return std::max(a,b); \
} \
inline TYPE MyMin(const TYPE a, const TYPE b){  \
    return std::min(a,b); \
}

TMP_MAXMIN(double)
TMP_MAXMIN(long double)
TMP_MAXMIN(float)
TMP_MAXMIN(int)
TMP_MAXMIN(unsigned int)
TMP_MAXMIN(short)
TMP_MAXMIN(unsigned short)
TMP_MAXMIN(long)
TMP_MAXMIN(unsigned long)
TMP_MAXMIN(char)
TMP_MAXMIN(unsigned char)

#undef TMP_MAXMIN

template<class T, int N>
inline TinyVector<T,N> MyMax(const TinyVector<T,N>&a, const TinyVector<T,N>&b) {
    return max(a,b);
}
template<class T, int N>
inline TinyVector<T,N> MyMin(const TinyVector<T,N>&a, const TinyVector<T,N>&b) {
    return min(a,b);
}
// @}
// }}}


//{{{ Round functions
/** 
 * @defgroup round Overloaded round functions
 * @brief Defines round function for basic types and and TinyVector.
 *
 * round compares element by element and returns true only if all elements are equal.
 *
 */
// @{ 
// template<class T>
// inline T round(const T& a) {
//     return a;
// }

#define TMP_ROUND(TYPE)         \
inline int round(const TYPE f){ \
    int i = (int)f;             \
    if (f>0) {                  \
        if (f - i >= 0.5F)      \
                ++i;            \
        return i;               \
    } else {                    \
        if (i - f >= 0.5F)      \
                --i;            \
        return i;               \
    }                           \
}
TMP_ROUND(double)
TMP_ROUND(long double)
TMP_ROUND(float)
#undef TMP_ROUND

#define TMP_ROUND(TYPE)                 \
template<int N>                         \
inline TinyVector<int,N> round(  \
    const TinyVector<TYPE,N>&f)  \
{                                       \
    /* implicit casting*/               \
    TinyVector<int,N> i (f);            \
    for (int j= 0; j< N; ++j)           \
        if (f[j] > 0) {                 \
            if (f[j] - i[j] >= 0.5F)    \
                    ++i[j];             \
        } else {                        \
            if (i[j] - f[j] >= 0.5F)    \
                    --i[j];             \
        }                               \
    return i;                           \
}

TMP_ROUND(double)
TMP_ROUND(long double)
TMP_ROUND(float)
#undef TMP_ROUND

// @}
//}}} 

// {{{ identity functions
/**
 * @brief Reinitialize a matrix to an identity matrix.
 *
 * @param m Matrix to be reinitialized
 * @param x Initial value of diagonal elements
 */
template<class T, int N, int M >
inline TinyMatrix<T,N,M>& identity(TinyMatrix<T,N,M>& m, const T x=1)
{
    for (int i=0; i < N; ++i)
    for (int j=0; j < M; ++j)
            m(i,j) = (i==j) ? x : 0;
    return m;
}
// }}}

// {{{ diagonalise TinyMatrix
/**
 * @brief Initialize a matrix to a diagonal matrix with diagonal elements equal to tinyvector elements
 *
 * @param m Matrix to be reinitialized
 * @param x Initial value of diagonal elements
 */
template<class TM, class TV, int N>
inline TinyMatrix<TM,N,N>& diagonal(TinyMatrix<TM,N,N>& m, const TinyVector<TV,N>& x)
{
    m =0;
    for (int i=0; i < N; ++i)
            m(i,i) = x[i];
    return m;
}
template<class TV, int N>
inline TinyMatrix<TV,N,N> diagonal(const TinyVector<TV,N>& x)
{
    TinyMatrix<TV,N,N> m(0);
    for (int i=0; i < N; ++i)
            m(i,i) = x[i];
    return m;
}

// }}}
// {{{ outerProduct
/**
 * Create a outer product of two vectors.
 * If A and B are two column vectors, OuterProduct = A*B^T.
 * and is [size(A)] X [size(B)] matrix
 */
template<class T1, class T2, int N, int M>
inline TinyMatrix<typename promote_trait<T1,T2>::T_promote, N, M>
outerProduct(const TinyVector<T1, N> & x, const TinyVector<T2, M> & y)
{
    TinyMatrix<typename promote_trait<T1,T2>::T_promote, N, M> matrix;
    for (int i=0; i < N; ++i)
    for (int j=0; j < M; ++j)
            matrix(i,j) = x(i)*y(j);
    return matrix;
}
// }}}

// {{{ transpose and transpose self
template<class T, int N, int M>
inline TinyMatrix<T,M,N> transpose(const TinyMatrix<T,N,M>& x)
{
    TinyMatrix<T,M,N> m;
    for (int i=0; i < N; ++i)
    for (int j=0; j < M; ++j)
            m(i,j) = x(j,i);
    return m;
}

template<class T, int N>
inline TinyMatrix<T,N,N>& transposeSelf(TinyMatrix<T,N,N>& x)
{
    for (int i=0; i < N; ++i)
    for (int j=i; j < N; ++j)
            swap(x(j,i), x(i,j));
    return x;
}
// }}}
// {{{ product
/**
 * Compute product of matrices.
 * If A and B are two matrices , product = A*B.
 * and is [row(A)] X [column(B)] matrix
 */
/*
template<class T1, class T2, int N, int O, int M>
inline TinyMatrix<typename promote_trait<T1,T2>::T_promote, N, M>
product(const TinyMatrix<T1, N, O> & x, const TinyMatrix<T2, O, M> & y)
{
    TinyMatrix<typename promote_trait<T1,T2>::T_promote, N, M> matrix=0;
    for (int i=0; i < N; ++i)
    for (int j=0; j < M; ++j)
    for (int k=0; k < O; ++k)
            matrix(i,j) += x(i,k)*y(k,j);
    return matrix;
}*/
// }}}

BZ_NAMESPACE_END

#endif // _LEAR_EXT_TVMUTILS_H__
