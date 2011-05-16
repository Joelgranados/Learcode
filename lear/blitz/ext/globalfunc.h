#ifndef _LEAR_GLOBAL_FUNCS_H_
#define _LEAR_GLOBAL_FUNCS_H_

#include <blitz/array.h>
#include <blitz/tinyvec.h>
#include <blitz/tinyvec-et.h>

BZ_NAMESPACE(blitz)
// {{{ centerofgravity
    /**
     * Compute the center of gravity of a N-D array. The result is promoted to nearest
     * real typed value (using NumericTypeTraits provided in blitz). So any lost
     * in precision is avoided.
     */

template<class ElementType, int N>
class CenterOfGravity {
    public:
        typedef TinyVector<typename NumericTypeTraits<ElementType>::T_floattype, N> ReturnType;

        static ReturnType get(const Array<ElementType,N>& array) 
        {
            ReturnType result(0);
            for (typename Array<ElementType,N>::const_iterator iter = array.begin();
                    iter != array.end(); ++iter) 
            {
                result += (*iter)*(iter.position());
            }
            const typename NumericTypeTraits<ElementType>::T_floattype sumwt = sum(array);
            return result/sumwt;
        }
};

template<class ElementType, int N>
TinyVector<typename NumericTypeTraits<ElementType>::T_floattype, N> centerofgravity(
        const Array<ElementType,N>& array) 
{
    return CenterOfGravity<ElementType,N>::get(array);
}
// }}}


template<class ElementType>
Array<ElementType,2> extendBorder(
        const Array<ElementType,2>& input, 
        const TinyVector<int,2> margin_topleft, 
        const TinyVector<int,2> margin_bottomright, 
        const TinyVector<int,2> center, 
        // ideally should be 0, but blitz-0.9 have issue
        const ElementType color=0.0)// in case ElementType is an int or something, 
                        //it will give warning and cast it. But it takes care of tinyvector annoying memory pointer initialization.
{
    typedef TinyVector<int,2>       IndexType;
    typedef Array<ElementType,2>    ImageType;

    IndexType cimageEx (margin_topleft+margin_bottomright+1); 

    ImageType cimage(cimageEx);
    cimage = color;

    IndexType lb (center - margin_topleft);

    // refer to top-left in input
    IndexType dlb = blitz::max(lb,input.lbound()); 

    IndexType ub (center + margin_bottomright);

    // refer to bottom-right in input
    IndexType dub = blitz::min(ub,input.ubound());

    // refer to input range
    blitz::RectDomain<2> domain(dlb, dub);
    
    // refer to top-left in cimage
    lb = dlb - lb;
    // refer to bottom-right in cimage
    ub = dub - dlb + lb;

    // refer to centered image range
    blitz::RectDomain<2> range(lb, ub);

    cimage(range) = input(domain);

    blitz::Range all = blitz::Range::all();

    for (int i= 0; i< lb[0]; ++i) 
        cimage(i,all) = cimage(lb[0],all);
    for (int i= 0; i< lb[1]; ++i) 
        cimage(all,i) = cimage(all,lb[1]);

    for (int i= ub[0]; i< cimageEx[0]; ++i)
        cimage(i,all) = cimage(i-1,all);
    for (int i= ub[1]; i< cimageEx[1]; ++i) 
        cimage(all,i) = cimage(all,i -1);

    return cimage;
}

BZ_NAMESPACE_END
#endif	//#ifndef _LEAR_GLOBAL_FUNCS_H_
