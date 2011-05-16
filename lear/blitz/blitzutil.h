// {{{ copyright notice
// }}}
// {{{ file documentation
/**
 * @file
 * @brief Defines some common utility routines for blitz arrays
 */
// }}}

#ifndef _LEAR_BLITZUTILS_H_
#define _LEAR_BLITZUTILS_H_

// {{{ headers
#include <vector>
#include <algorithm>

#include <blitz/array.h>
// }}}

namespace lear {

//{{{ Copy boundaries
template<class T> inline 
void copyBoundaries(blitz::Array<T,1>& array, const int from= 1, const int to=0) {
    array(array.lbound(0)-to) = array(array.lbound(0)+from);
    array(array.ubound(0)-to) = array(array.ubound(0)-from);
}

template<class T> inline 
void copyBoundaries(blitz::Array<T,2>& array, const int from= 1, const int to=0) {
    using namespace blitz;
    Range all = Range::all();
    array(all,array.lbound(1)+to) = array(all,array.lbound(1)+from);
    array(all,array.ubound(1)-to) = array(all,array.ubound(1)-from);

    array(array.lbound(0)+to,all) = array(array.lbound(0)+from, all);
    array(array.ubound(0)-to,all) = array(array.ubound(0)-from, all);
}

template<class T> inline 
void copyBoundaries( blitz::Array<T,3>& array, const int from= 1, const int to=0) {
    using namespace blitz;
    Range all = Range::all();
    array(all,all,array.lbound(2)+to) = array(all,all,array.lbound(2)+from);
    array(all,all,array.ubound(2)-to) = array(all,all,array.ubound(2)-from);

    array(all,array.lbound(1)+to,all) = array(all,array.lbound(1)+from,all);
    array(all,array.ubound(1)-to,all) = array(all,array.ubound(1)-from,all);

    array(array.lbound(0)+to,all,all) = array(array.lbound(0)+from,all,all);
    array(array.ubound(0)-to,all,all) = array(array.ubound(0)-from,all,all);
}
//}}} Copy boundaries

template<class ElementType>
inline blitz::Array<ElementType,1> reduce(
        const blitz::Array<ElementType,2>& array2d, const blitz::Range& range)
{// {{{
    blitz::Array<ElementType,1> array1d( 
            blitz::Range(array2d.lbound(0),array2d.ubound(0)) );

    for (int j= array1d.lbound(0); j<= array1d.ubound(0); ++j) 
        array1d(j) = blitz::sum(array2d(j, range));

    return array1d;
} // }}}

template<class ElementType>
inline blitz::Array<ElementType,2> reduce(
        const blitz::Array<ElementType,3>& array3d, const blitz::Range& range)
{// {{{
    blitz::TinyVector<int,2> lbound (array3d.lbound(0), array3d.lbound(1));
    blitz::TinyVector<int,2> extent (array3d.extent(0), array3d.extent(1));
    blitz::Array<ElementType,2> array2d( lbound, extent);

    for (int i= array2d.lbound(0); i<= array2d.ubound(0); ++i) 
    for (int j= array2d.lbound(1); j<= array2d.ubound(1); ++j) 
        array2d(i, j) = blitz::sum(array3d(i, j, range));

    return array2d;
} // }}}

// {{{ toVector and toArray
/**
 * Convert a vector to 1D blitz array.
 */
template<class T>
void toArray(const std::vector<T>& vector, blitz::Array<T,1>& array) {
    array.resize(vector.size());
    std::copy(vector.begin(),vector.end(),array.begin());
}

/**
 * Convert a 1D blitz array to std::vector.
 */
template<class T>
void toVector(const blitz::Array<T,1>& array, std::vector<T>& vector) {
    vector.resize(array.size());
    std::copy(array.begin(),array.end(),vector.begin());
}
// }}}

// {{{ productAB, productATB, productATBT, productABT 
/**
 * Multiply two Matrices (2D arrays)
 *
 * \return A*B
 */
    template<class RealType>  inline
    blitz::Array<RealType,2> productAB(
        const blitz::Array<RealType,2>& A,
        const blitz::Array<RealType,2>& B)
    {
        using namespace blitz;
        firstIndex i; secondIndex j; thirdIndex k;

        blitz::Array<RealType,2> C(
                Range(A.lbound(0), A.ubound(0)), 
                Range(B.lbound(1), B.ubound(1)) );

        C = sum(A(i,k) * B(k,j), k);
        return C;
    }

/**
 * Multiply two Matrices (2D arrays)
 *
 * \return A*B^T
 */
    template<class RealType>  inline
    blitz::Array<RealType,2> productABT(
        const blitz::Array<RealType,2>& A,
        const blitz::Array<RealType,2>& B)
    {
        using namespace blitz;
        firstIndex i; secondIndex j; thirdIndex k;

        blitz::Array<RealType,2> C(
                Range(A.lbound(0), A.ubound(0)), 
                Range(B.lbound(0), B.ubound(0)) );

        C = sum(A(i,k) * B(j,k), k);
        return C;
    }

/**
 * Multiply two Matrices (2D arrays)
 *
 * \return A^T*B
 */
    template<class RealType>  inline
    blitz::Array<RealType,2> productATB(
        const blitz::Array<RealType,2>& A,
        const blitz::Array<RealType,2>& B)
    {
        using namespace blitz;
        firstIndex i; secondIndex j; thirdIndex k;

        blitz::Array<RealType,2> C(
                Range(A.lbound(1), A.ubound(1)), 
                Range(B.lbound(1), B.ubound(1)) );

        C = sum(A(k,i) * B(k,j), k);
        return C;
    }

/**
 * Multiply two Matrices (2D arrays)
 *
 * \return A^T*B^T
 */
    template<class RealType>  inline
    blitz::Array<RealType,2> productATBT(
        const blitz::Array<RealType,2>& A,
        const blitz::Array<RealType,2>& B)
    {
        using namespace blitz;
        firstIndex i; secondIndex j; thirdIndex k;

        blitz::Array<RealType,2> C(
                Range(A.lbound(1), A.ubound(1)), 
                Range(B.lbound(0), B.ubound(0)) );

        C = sum(A(k,i) * B(j,k), k);
        return C;
    }
// }}}
} // lear

#endif // #ifndef _LEAR_BLITZUTILS_H_
