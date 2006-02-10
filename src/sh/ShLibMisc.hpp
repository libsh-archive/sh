// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
//////////////////////////////////////////////////////////////////////////////
#include "ShLib.hpp" // ShLibMisc needs to be included from within ShLib

#ifndef SHLIBMISC_HPP
#define SHLIBMISC_HPP

#include "ShGeneric.hpp"

#ifndef _WIN32
namespace SH {

/** \defgroup lib_misc Miscellaneous operations
 * @ingroup library
 * Some special versions of type-cast operators are defined to be able to
 * deal with tuple size changes in various useful ways.
 * @{
 */

/** Casting.
 * Casts ShGeneric<N, T> to ShGeneric<M, T>.
 * If M > N, pads remaining components with 0s (on right).
 * Otherwise, discards extra components.
 */
template<int M, int N, typename T> 
ShGeneric<M, T> cast(const ShGeneric<N, T>& a);
template<int M> 
ShGeneric<M, double> cast(double a); // @todo type do a cpp type -> value type map

/** Fill Casting.
 * Casts ShGeneric<N, T> to ShGeneric<M, T>.
 * If M > N, copies last component to fill extra slots.
 * Otherwise, discards extra components.
 */
template<int M, int N, typename T> 
ShGeneric<M, T> fillcast(const ShGeneric<N, T>& a);
template<int M> 
ShGeneric<M, double> fillcast(double a); // @todo type do a cpp type -> value type map

/** Join two tuples 
 * Creates an M+N tuple with components of a first then b.
 * @{ */
template<int M, int N, typename T1, typename T2> 
ShGeneric<M+N, CT1T2> 
join(const ShGeneric<M, T1>& a, const ShGeneric<N, T2> &b);
template<int M, typename T> 
ShGeneric<M+1, T> 
join(const T& a, const ShGeneric<M, T>& b);
template<int M, typename T> 
ShGeneric<M+1, T> 
join(const ShGeneric<M, T>& a, const T& b);
// @}

/** Join three tuples 
 * Creates an M+N+O tuple with components of a first then b and c.
 */
template<int M, int N, int O, typename T1, typename T2, typename T3> 
ShGeneric<M+N+O, CT1T2T3> 
join(const ShGeneric<M, T1>& a, const ShGeneric<N, T2> &b, const ShGeneric<O, T3> &c);

/** Join four tuples 
 * Creates an M+N+O+P tuple with components of a first then b, c and d.
 */
template<int M, int N, int O, int P, typename T1, typename T2, typename T3, typename T4> 
ShGeneric<M+N+O+P, CT1T2T3T4> 
join(const ShGeneric<M, T1>& a, const ShGeneric<N, T2> &b, const ShGeneric<O, T3> &c, const ShGeneric<P, T4> &d);

/** Fragment discard. Only for fragment programs.
 * Discards the current fragment if any(c) > 0.
 */
template<int N, typename T>
void discard(const ShGeneric<N, T>& c);

/** Fragment killing.
 * @deprecated Use discard instead.
 */
template<int N, typename T>
void kill(const ShGeneric<N, T>& c);

/** Sort components of a in increasing order.
 *
 * Creates an N tuple with the components of a * in sorted increasing
 * order using an even-odd transposition sort.
 */
template<int N, typename T> 
ShGeneric<N, T> sort(const ShGeneric<N, T>& a);

/** \brief Sorts groups of components v[i](j), 0 <= i < S
  * by the components in v[0](j) 0 <= j < N.
  * Uses an even-odd transposition sort.
  */
template<int S, typename T>
void groupsort(T v[]);

/** Uniform freezing.
 *
 * Replace uses of the given uniform in the given program with a
 * constant containing its current value, and return the resulting
 * program.
 */
template<typename T>
ShProgram freeze(const ShProgram& p,
                 const T& uniform);

/* Evaluation of polynomial of order M at a using coefficients in b.
 */
template<int N, int M, typename T1, typename T2>
ShGeneric<N, CT1T2>
poly(const ShGeneric<N, T1>& a, const ShGeneric<M, T2>& b);

/*@}*/

}
#endif

#include "ShLibMiscImpl.hpp"

#endif
