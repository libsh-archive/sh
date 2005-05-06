// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////
#ifndef SHLIBMISC_HPP
#define SHLIBMISC_HPP

#include "ShGeneric.hpp"
#include "ShLib.hpp"

#ifndef WIN32
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
 */
template<int M, int N, typename T1, typename T2> 
ShGeneric<M+N, CT1T2> 
join(const ShGeneric<M, T1>& a, const ShGeneric<N, T2> &b);

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
template<int S, int N, typename T>
void groupsort(ShGeneric<N, T> v[]);

/** Uniform freezing.
 *
 * Replace uses of the given uniform in the given program with a
 * constant containing its current value, and return the resulting
 * program.
 */
template<typename T>
ShProgram freeze(const ShProgram& p,
                 const T& uniform);

/** Debugging op 
 *
 * Replace the first color output with the given variable. 
 * (If there are multiple calls to this, the last one executed
 * takes precedence) 
 */
template<int N, typename T>
void dbg_output(const ShGeneric<N, T>& a);

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
