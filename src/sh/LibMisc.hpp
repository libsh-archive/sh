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
#include "Lib.hpp" // LibMisc needs to be included from within Lib

#ifndef SHLIBMISC_HPP
#define SHLIBMISC_HPP

#include "Generic.hpp"

#ifndef _WIN32
namespace SH {

/** \defgroup lib_misc Miscellaneous operations
 * @ingroup library
 * Some special versions of type-cast operators are defined to be able to
 * deal with tuple size changes in various useful ways.
 * @{
 */

/** Casting.
 * Casts Generic<N, T> to Generic<M, T>.
 * If M > N, pads remaining components with 0s (on right).
 * Otherwise, discards extra components.
 */
template<int M, int N, typename T> 
Generic<M, T> cast(const Generic<N, T>& a);
template<int M> 
Generic<M, double> cast(double a); // @todo type do a cpp type -> value type map

/** Fill Casting.
 * Casts Generic<N, T> to Generic<M, T>.
 * If M > N, copies last component to fill extra slots.
 * Otherwise, discards extra components.
 */
template<int M, int N, typename T> 
Generic<M, T> fillcast(const Generic<N, T>& a);
template<int M> 
Generic<M, double> fillcast(double a); // @todo type do a cpp type -> value type map

/** Join two tuples 
 * Creates an M+N tuple with components of a first then b.
 * @{ */
template<int M, int N, typename T1, typename T2> 
Generic<M+N, CT1T2> 
join(const Generic<M, T1>& a, const Generic<N, T2> &b);
template<int M, typename T> 
Generic<M+1, T> 
join(const T& a, const Generic<M, T>& b);
template<int M, typename T> 
Generic<M+1, T> 
join(const Generic<M, T>& a, const T& b);
// @}

/** Join three tuples 
 * Creates an M+N+O tuple with components of a first then b and c.
 */
template<int M, int N, int O, typename T1, typename T2, typename T3> 
Generic<M+N+O, CT1T2T3> 
join(const Generic<M, T1>& a, const Generic<N, T2> &b, const Generic<O, T3> &c);

/** Join four tuples 
 * Creates an M+N+O+P tuple with components of a first then b, c and d.
 */
template<int M, int N, int O, int P, typename T1, typename T2, typename T3, typename T4> 
Generic<M+N+O+P, CT1T2T3T4> 
join(const Generic<M, T1>& a, const Generic<N, T2> &b, const Generic<O, T3> &c, const Generic<P, T4> &d);

/** Fragment discard. Only for fragment programs.
 * Discards the current fragment if any(c) > 0.
 */
template<int N, typename T>
void discard(const Generic<N, T>& c);

/** Fragment killing.
 * @deprecated Use discard instead.
 */
template<int N, typename T>
void kill(const Generic<N, T>& c);

/** Sort components of a in increasing order.
 *
 * Creates an N tuple with the components of a * in sorted increasing
 * order using an even-odd transposition sort.
 */
template<int N, typename T> 
Generic<N, T> sort(const Generic<N, T>& a);

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
Program freeze(const Program& p,
                 const T& uniform);

/* Evaluation of polynomial of order M at a using coefficients in b.
 */
template<int N, int M, typename T1, typename T2>
Generic<N, CT1T2>
poly(const Generic<N, T1>& a, const Generic<M, T2>& b);

/*@}*/

}
#endif

#include "LibMiscImpl.hpp"

#endif
