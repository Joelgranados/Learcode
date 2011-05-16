#ifndef _LEAR_TINY_VECTOR_MATRIX_EXTRA_H_
#define _LEAR_TINY_VECTOR_MATRIX_EXTRA_H_

#include <blitz/tinyvec.h>

namespace lear {

template<
        class PairType, int PairSize,
        class ElementType
    >
void conjoin( 
        int StartIndex, 
        blitz::TinyVector<PairType,PairSize>& result,
        const ElementType& input
        )
{
    result[StartIndex] = static_cast<PairType> (input);
}

template<
        class PairType, int PairSize,
        class ElementType, int ElementSize
    >
void conjoin( 
        int StartIndex, 
        blitz::TinyVector<PairType,PairSize>& result,
        const blitz::TinyVector<ElementType,ElementSize>& input
        )
{
    for (int i=0; i < ElementSize; ++i) {
        result[i+StartIndex] = static_cast<PairType> (input[i]);
    }
}

template<
        class PairType, int PairSize,
        class ElementType, int ElementSize
    >
void alienate(
        int StartIndex, 
        const blitz::TinyVector<PairType, PairSize>& input,
        blitz::TinyVector<ElementType, ElementSize>& result
        ) 
{
    for (int i=0; i < ElementSize; ++i) {
        result[i] = static_cast<ElementType> (input[i+StartIndex]);
    }
}

template<
        class PairType, int PairSize,
        class ElementType
    >
void alienate(
        int StartIndex, 
        const blitz::TinyVector<PairType, PairSize>& input,
        ElementType& result
        ) 
{
    result = static_cast<ElementType> (input[StartIndex]);
}



}
#endif // _LEAR_TINY_VECTOR_MATRIX_EXTRA_H_

