//{{{ License - this file is modified version of std-pair class
// Modified by Navneet Dalal - Sept 04, 2003

// It implements triplet class modeled after std::pair class


// Triplet implementation -*- C++ -*-

// Copyright (C) 2001 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU General Public License.

/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */
 //}}}

#ifndef _LEAR_TRIPLET_H
#define _LEAR_TRIPLET_H

namespace lear
{
// {{{ Triplet class 
/** triplet holds three objects of arbitrary type.
 */
template <class _T1, class _T2, class _T3>
struct triplet {
  typedef _T1 first_type;    ///<  @c first_type is the first bound type
  typedef _T2 second_type;   ///<  @c second_type is the second bound type
  typedef _T3 third_type;   ///<  @c third_type is the second bound type

  _T1 first;                 ///< @c first is a copy of the first object
  _T2 second;                ///< @c second is a copy of the second object
  _T3 third;                ///< @c third is a copy of the second object

  /** The default constructor creates @c first, @c second and @c thrid using their
   *  respective default constructors.  */
  triplet() : first(), second(), third() {}

  /** Three objects may be passed to a @c triplet constructor to be copied.  */
  triplet(const _T1& __a, const _T2& __b, const _T3& __c) :
  	first(__a), second(__b), third(__c) {}

  /** There is also a templated copy ctor for the @c triplet class itself.  */
  template <class _U1, class _U2, class _U3>
  triplet(const triplet<_U1, _U2, _U3 >& __p) :
  	first(__p.first), second(__p.second), third(__p.third) {}

};
// }}}

// Operators overloaded for triplet class {{{
/// Two triplets of the same type are equal iff their members are equal.
template <class _T1, class _T2, class _T3>
inline bool operator==(const triplet<_T1, _T2, _T3>& __x, const triplet<_T1, _T2, _T3>& __y)
{
  return __x.first == __y.first && __x.second == __y.second && __x.third == __y.third;
}

/// <http://gcc.gnu.org/onlinedocs/libtdc++/20_util/howto.html#pairlt>
template <class _T1, class _T2, class _T3>
inline bool operator<(const triplet<_T1, _T2, _T3>& __x, const triplet<_T1, _T2, _T3>& __y)
{
  return __x.first < __y.first ||
         (!(__y.first < __x.first) &&   __x.second < __y.second) ||
	 (!(__y.second < __x.second) && ( __x.third < __y.third) )   ;
}

/// Uses @c operator== to find the result.
template <class _T1, class _T2, class _T3>
inline bool operator!=(const triplet<_T1, _T2, _T3>& __x, const triplet<_T1, _T2, _T3>& __y) {
  return !(__x == __y);
}

/// Uses @c operator< to find the result.
template <class _T1, class _T2, class _T3>
inline bool operator>(const triplet<_T1, _T2, _T3>& __x, const triplet<_T1, _T2, _T3>& __y) {
  return __y < __x;
}

/// Uses @c operator< to find the result.
template <class _T1, class _T2, class _T3>
inline bool operator<=(const triplet<_T1, _T2, _T3>& __x, const triplet<_T1, _T2, _T3>& __y) {
  return !(__y < __x);
}

/// Uses @c operator< to find the result.
template <class _T1, class _T2, class _T3>
inline bool operator>=(const triplet<_T1, _T2, _T3>& __x, const triplet<_T1, _T2, _T3>& __y) {
  return !(__x < __y);
}
// }}}

// triplet wraper {{{
/**
 *  @brief A convenience wrapper for creating a triplet from two objects.
 *  @param  __x  The first object.
 *  @param  __y  The second object.
 *  @param  __z  The third object.
 *  @return   A newly-constructed triplet<> object of the appropriate type.
 *
 *  The standard requires that the objects be passed by reference-to-const,
 *  but LWG issue #181 says they should be passed by const value.  We follow
 *  the LWG by default.
*/
template <class _T1, class _T2, class _T3>
//181.  make_pair() unintended behavior
inline triplet<_T1, _T2, _T3> make_triplet(const _T1 __x, const _T2 __y, const _T3 __z)
// inline triplet<_T1, _T2, _T3> make_triplet(const _T1& __x, const _T2& __y, const _T3& __z)
{
  return triplet<_T1, _T2, _T3>(__x, __y, __z);
}
//}}}

} // namespace lear

#endif /* _LEAR_TRIPLET_H */

