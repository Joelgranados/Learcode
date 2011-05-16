// {{{ file documentation
/**
 * @brief extension to blitz promote util in blitz namespace
 * @warning This file extends blitz's name space and doesnot use lear namespace
 */
// }}}

#ifndef _LEAR_EXT_PROMOTE_H_
#define _LEAR_EXT_PROMOTE_H_

#include <blitz/tinyvec.h>
#include <blitz/promote.h>

BZ_NAMESPACE(blitz)

// specialization of blitz promote_trait for tinyvector
template<class T_a, class T_b, int N>
struct promote_trait< TinyVector<T_a,N>, TinyVector<T_b,N> > {

    typedef TinyVector< tyename promote_trait<T_a, T_b>::T_promote, N> T_promote;
}

BZ_NAMESPACE_END

#endif // _LEAR_EXT_PROMOTE_H_
