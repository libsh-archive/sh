#ifndef SHLIBMISC_HPP
#define SHLIBMISC_HPP

#include "ShGeneric.hpp"
#include "ShLib.hpp"

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
ShGeneric<M, float> cast(float a);

/** Fill Casting.
 * Casts ShGeneric<N, T> to ShGeneric<M, T>.
 * If M > N, copies last component to fill extra slots.
 * Otherwise, discards extra components.
 */
template<int M, int N, typename T> 
ShGeneric<M, T> fillcast(const ShGeneric<N, T>& a);
template<int M> 
ShGeneric<M, float> fillcast(float a);

/** Join two tuples 
 * Creates an M+N tuple with components of a first then b.
 */
template<int M, int N, typename T1, typename T2> 
ShGeneric<M+N, CT1T2> 
join(const ShGeneric<M, T1>& a, const ShGeneric<N, T2> &b);

/** Fragment killing. Only for fragment programs.
 * Kills the current fragment if any(c) > 0.
 */
template<int N, typename T>
void kill(const ShGeneric<N, T>& c);

/*@}*/

}

#include "ShLibMiscImpl.hpp"

#endif
