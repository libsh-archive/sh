// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
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
 * Casts ShGeneric<N, V> to ShGeneric<M, V>.
 * If M > N, pads remaining components with 0s (on right).
 * Otherwise, discards extra components.
 */
template<int M, int N, ShValueType V> 
ShGeneric<M, V> cast(const ShGeneric<N, V>& a);
template<int M> 
ShGeneric<M, SH_DOUBLE> cast(double a); // @todo type do a cpp type -> value type map

/** Fill Casting.
 * Casts ShGeneric<N, V> to ShGeneric<M, V>.
 * If M > N, copies last component to fill extra slots.
 * Otherwise, discards extra components.
 */
template<int M, int N, ShValueType V> 
ShGeneric<M, V> fillcast(const ShGeneric<N, V>& a);
template<int M> 
ShGeneric<M, SH_DOUBLE> fillcast(double a); // @todo type do a cpp type -> value type map

/** Join two tuples 
 * Creates an M+N tuple with components of a first then b.
 */
template<int M, int N, ShValueType V1, ShValueType V2> 
ShGeneric<M+N, CV1V2> 
join(const ShGeneric<M, V1>& a, const ShGeneric<N, V2> &b);

/** Fragment discard. Only for fragment programs.
 * Discards the current fragment if any(c) > 0.
 */
template<int N, ShValueType V>
void discard(const ShGeneric<N, V>& c);

/** Fragment killing.
 * @deprecated Use discard instead.
 */
template<int N, ShValueType V>
void kill(const ShGeneric<N, V>& c);

/** Uniform freezing.
 *
 * Replace uses of the given uniform in the given program with a
 * constant containing its current value, and return the resulting
 * program.
 */
template<typename T>
ShProgram freeze(const ShProgram& p,
                 const T& uniform);

/*@}*/

}
#endif

#include "ShLibMiscImpl.hpp"

#endif
