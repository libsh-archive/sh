#ifndef SHLIBBOOLEAN_HPP
#define SHLIBBOOLEAN_HPP

#include "ShGeneric.hpp"
#include "ShLib.hpp"


namespace SH {

/** \defgroup lib_boolean Boolean and conditional functions
 * @ingroup library
 * @{
 */

/// Componentwise less-than
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator<(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right);
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator<(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right);
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator<(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right);
template<typename T1, typename T2>
ShGeneric<1, CT1T2>
operator<(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right);
SH_SHLIB_CONST_SCALAR_OP(operator<);
SH_SHLIB_CONST_N_OP_BOTH(operator<);

/// Componentwise less-than-or-equal
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator<=(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right);
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator<=(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right);
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator<=(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right);
template<typename T1, typename T2>
ShGeneric<1, CT1T2>
operator<=(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right);
SH_SHLIB_CONST_SCALAR_OP(operator<=);
SH_SHLIB_CONST_N_OP_BOTH(operator<=);

/// Componentwise greater-than
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator>(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right);
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator>(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right);
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator>(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right);
template<typename T1, typename T2>
ShGeneric<1, CT1T2>
operator>(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right);
SH_SHLIB_CONST_SCALAR_OP(operator>);
SH_SHLIB_CONST_N_OP_BOTH(operator>);

/// Componentwise greater-than-or-equal
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator>=(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right);
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator>=(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right);
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator>=(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right);
template<typename T1, typename T2>
ShGeneric<1, CT1T2>
operator>=(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right);
SH_SHLIB_CONST_SCALAR_OP(operator>=);
SH_SHLIB_CONST_N_OP_BOTH(operator>=);

/// Componentwise equal
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator==(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right);
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator==(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right);
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator==(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right);
template<typename T1, typename T2>
ShGeneric<1, CT1T2>
operator==(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right);
SH_SHLIB_CONST_SCALAR_OP(operator==);
SH_SHLIB_CONST_N_OP_BOTH(operator==);

/// Componentwise not equal
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator!=(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right);
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator!=(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right);
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator!=(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right);
template<typename T1, typename T2>
ShGeneric<1, CT1T2>
operator!=(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right);
SH_SHLIB_CONST_SCALAR_OP(operator!=);
SH_SHLIB_CONST_N_OP_BOTH(operator!=);

/** Conditional assignment.
 *  dest[i] = ((src[0][i] > 0.0) ? src[1][i] : src[2][i])
 *  Note: CMP in the ARB_{vertex,fragment}_program spec has
 *  src[0][i] < 0.0, not greater than.   The semantics used here
 *  follows that in Scheme and the ?: operator (which, unfortunately,
 *  cannot be overloaded in C++ right now).   The boolean argument
 *  is a tuple, and so this is really a kind of conditional writemask
 *  if used with a tuple of booleans, although a scalar boolean may
 *  also be used in the first argument, in which case it applies to 
 *  all elements.
 */
template<int N, typename T1, typename T2, typename T3>
ShGeneric<N, CT1T2T3> 
cond(const ShGeneric<N, T1>& condition, const ShGeneric<N, T2>& left,
                     const ShGeneric<N, T3>& right);
template<int N, typename T1, typename T2, typename T3>
ShGeneric<N, CT1T2T3> 
cond(const ShGeneric<1, T1>& condition, const ShGeneric<N, T2>& left,
                     const ShGeneric<N, T3>& right);
template<int N, typename T1, typename T2, typename T3>
ShGeneric<N, CT1T2T3> 
cond(const ShGeneric<1, T1>& condition, const ShGeneric<1, T2>& left,
                     const ShGeneric<1, T3>& right);

/// Boolean negation
template<int N, typename T>
ShGeneric<N, T> operator!(const ShGeneric<N, T>& a);

/// Componentwise minimum
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator&&(const ShGeneric<N, T1>& a, const ShGeneric<N, T2>& b);

/// Componentwise maximum
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator||(const ShGeneric<N, T1>& a, const ShGeneric<N, T2>& b);

/// Maximum of all components
template<int N, typename T>
ShGeneric<1, T> any(const ShGeneric<N, T>& a);

/// Minimum of all components
template<int N, typename T>
ShGeneric<1, T> all(const ShGeneric<N, T>& a);

/*@}*/

}

#include "ShLibBooleanImpl.hpp"

#endif
