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
#ifndef SHLIBCLAMP_HPP
#define SHLIBCLAMP_HPP

#include "ShGeneric.hpp"
#include "ShLib.hpp"

#ifndef WIN32
namespace SH {


/** \defgroup lib_clamping Clamping
 * @ingroup library
 * Various operations that reduce information in values somehow,
 * e.g. clamping them to ranges, throwing away their sign, or
 * selecting one out of two tuples.
 * @{
 */

/** Absolute value.
 * Returns the magnitude.
 * Operates componentwise on tuples.
 */
template<int N, ShValueType V>
ShGeneric<N, V> abs(const ShGeneric<N, V>& var);

/** Ceiling.
 * Returns the least integer >= argument. 
 * Operates componentwise on tuples.
 */
template<int N, ShValueType V>
ShGeneric<N, V> ceil(const ShGeneric<N, V>& var);

/** Floor.
 * Returns the greatest integer <= argument.
 * Operates componentwise on tuples.
 */
template<int N, ShValueType V>
ShGeneric<N, V> floor(const ShGeneric<N, V>& var);

/** Round.
 * Returns the nearest integer to the argument.
 * Operates componentwise on tuples.
 */
template<int N, ShValueType V>
ShGeneric<N, V> round(const ShGeneric<N, V>& var);

/** Float modulus. 
 * The result is always positive.
 */
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2>
mod(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right);
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2>
mod(const ShGeneric<N, V1>& left, const ShGeneric<1, V2>& right);
template<ShValueType V1, ShValueType V2>
ShGeneric<1, CV1V2>
mod(const ShGeneric<1, V1>& left, const ShGeneric<1, V2>& right);
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2>
operator%(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right);
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2>
operator%(const ShGeneric<N, V1>& left, const ShGeneric<1, V2>& right);
template<ShValueType V1, ShValueType V2>
ShGeneric<1, CV1V2>
operator%(const ShGeneric<1, V1>& left, const ShGeneric<1, V2>& right);

SH_SHLIB_CONST_SCALAR_OP_DECL(mod);
SH_SHLIB_CONST_N_OP_LEFT_DECL(mod);
SH_SHLIB_CONST_SCALAR_OP_DECL(operator%);
SH_SHLIB_CONST_N_OP_LEFT_DECL(operator%);

/** Fractional part.
 * Discards the integer part of each componenent in var.
 */
template<int N, ShValueType V>
ShGeneric<N, V> frac(const ShGeneric<N, V>& var);

/** Take positive part. 
 * Clamps a value to zero if it is negative.   
 * This is useful to wrap dot products in lighting models.
 */
template<int N, ShValueType V>
ShGeneric<N, V> pos(const ShGeneric<N, V>& x);

/** Maximum.
 * Creates a tuple of componentwise maximums of a pair of input tuples.
 */
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2>
max(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right);

SH_SHLIB_CONST_SCALAR_OP_DECL(max);

/** Minimum.
 * Creates a tuple of componentwise minimums of a pair of input tuples.
 */
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2>
min(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right);

SH_SHLIB_CONST_SCALAR_OP_DECL(min);

/** Maximum of all components
 */
template<int N, ShValueType V>
ShGeneric<1, V> max(const ShGeneric<N, V>& a);

/** Minimum of all components
 */
template<int N, ShValueType V>
ShGeneric<1, V> min(const ShGeneric<N, V>& a);

/** Componentwise clamping.
 * Clamps a between b and c.
 */
template<int N, ShValueType V1, ShValueType V2, ShValueType V3>
ShGeneric<N, CV1V2V3> 
clamp(const ShGeneric<N, V1>& a, const ShGeneric<N, V2>& b, const ShGeneric<N, V3>& c);
template<int N, ShValueType V1, ShValueType V2, ShValueType V3>
ShGeneric<N, CV1V2V3> 
clamp(const ShGeneric<N, V1>& a, const ShGeneric<1, V2>& b, const ShGeneric<1, V3>& c);
template<ShValueType V1, ShValueType V2, ShValueType V3>
ShGeneric<1, CV1V2V3> 
clamp(const ShGeneric<1, V1>& a, const ShGeneric<1, V2>& b, const ShGeneric<1, V3>& c);

SH_SHLIB_CONST_TRINARY_OP_011_DECL(clamp);

/** Componentwise saturation.
 * Equivalent to componentwise minimum with 1.
 */
template<int N, ShValueType V>
ShGeneric<N, V> sat(const ShGeneric<N, V>& a);

/** Componentwise sign.
 * Returns -1.0 if argument is less than 0.0, 1.0 if argument is greater
 * than 0.0, 0.0 otherwise.
 * Operates componentwise on tuples.
 */
template<int N, ShValueType V>
ShGeneric<N, V> sign(const ShGeneric<N, V>& var);

/*@}*/

}
#endif

#include "ShLibClampImpl.hpp"

#endif
