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
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2>
operator<(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right);
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2>
operator<(const ShGeneric<N, V1>& left, const ShGeneric<1, V2>& right);
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2>
operator<(const ShGeneric<1, V1>& left, const ShGeneric<N, V2>& right);
template<ShValueType V1, ShValueType V2>
ShGeneric<1, CV1V2>
operator<(const ShGeneric<1, V1>& left, const ShGeneric<1, V2>& right);
SH_SHLIB_CONST_SCALAR_OP(operator<);
SH_SHLIB_CONST_N_OP_BOTH(operator<);

/// Componentwise less-than-or-equal
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2>
operator<=(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right);
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2>
operator<=(const ShGeneric<N, V1>& left, const ShGeneric<1, V2>& right);
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2>
operator<=(const ShGeneric<1, V1>& left, const ShGeneric<N, V2>& right);
template<ShValueType V1, ShValueType V2>
ShGeneric<1, CV1V2>
operator<=(const ShGeneric<1, V1>& left, const ShGeneric<1, V2>& right);
SH_SHLIB_CONST_SCALAR_OP(operator<=);
SH_SHLIB_CONST_N_OP_BOTH(operator<=);

/// Componentwise greater-than
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2>
operator>(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right);
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2>
operator>(const ShGeneric<N, V1>& left, const ShGeneric<1, V2>& right);
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2>
operator>(const ShGeneric<1, V1>& left, const ShGeneric<N, V2>& right);
template<ShValueType V1, ShValueType V2>
ShGeneric<1, CV1V2>
operator>(const ShGeneric<1, V1>& left, const ShGeneric<1, V2>& right);
SH_SHLIB_CONST_SCALAR_OP(operator>);
SH_SHLIB_CONST_N_OP_BOTH(operator>);

/// Componentwise greater-than-or-equal
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2>
operator>=(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right);
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2>
operator>=(const ShGeneric<N, V1>& left, const ShGeneric<1, V2>& right);
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2>
operator>=(const ShGeneric<1, V1>& left, const ShGeneric<N, V2>& right);
template<ShValueType V1, ShValueType V2>
ShGeneric<1, CV1V2>
operator>=(const ShGeneric<1, V1>& left, const ShGeneric<1, V2>& right);
SH_SHLIB_CONST_SCALAR_OP(operator>=);
SH_SHLIB_CONST_N_OP_BOTH(operator>=);

/// Componentwise equal
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2>
operator==(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right);
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2>
operator==(const ShGeneric<N, V1>& left, const ShGeneric<1, V2>& right);
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2>
operator==(const ShGeneric<1, V1>& left, const ShGeneric<N, V2>& right);
template<ShValueType V1, ShValueType V2>
ShGeneric<1, CV1V2>
operator==(const ShGeneric<1, V1>& left, const ShGeneric<1, V2>& right);
SH_SHLIB_CONST_SCALAR_OP(operator==);
SH_SHLIB_CONST_N_OP_BOTH(operator==);

/// Componentwise not equal
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2>
operator!=(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right);
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2>
operator!=(const ShGeneric<N, V1>& left, const ShGeneric<1, V2>& right);
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2>
operator!=(const ShGeneric<1, V1>& left, const ShGeneric<N, V2>& right);
template<ShValueType V1, ShValueType V2>
ShGeneric<1, CV1V2>
operator!=(const ShGeneric<1, V1>& left, const ShGeneric<1, V2>& right);
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
template<int N, ShValueType V1, ShValueType V2, ShValueType V3>
ShGeneric<N, CV1V2V3> 
cond(const ShGeneric<N, V1>& condition, const ShGeneric<N, V2>& left,
                     const ShGeneric<N, V3>& right);
template<int N, ShValueType V1, ShValueType V2, ShValueType V3>
ShGeneric<N, CV1V2V3> 
cond(const ShGeneric<1, V1>& condition, const ShGeneric<N, V2>& left,
                     const ShGeneric<N, V3>& right);
template<int N, ShValueType V1, ShValueType V2, ShValueType V3>
ShGeneric<N, CV1V2V3> 
cond(const ShGeneric<1, V1>& condition, const ShGeneric<1, V2>& left,
                     const ShGeneric<1, V3>& right);

/// Boolean negation
template<int N, ShValueType V>
ShGeneric<N, V> operator!(const ShGeneric<N, V>& a);

/// Componentwise minimum
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator&&(const ShGeneric<N, V1>& a, const ShGeneric<N, V2>& b);

/// Componentwise maximum
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator||(const ShGeneric<N, V1>& a, const ShGeneric<N, V2>& b);

/// Maximum of all components
template<int N, ShValueType V>
ShGeneric<1, V> any(const ShGeneric<N, V>& a);

/// Minimum of all components
template<int N, ShValueType V>
ShGeneric<1, V> all(const ShGeneric<N, V>& a);

/*@}*/

}

#include "ShLibBooleanImpl.hpp"

#endif
