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
#include "ShLib.hpp" // ShLibClamp needs to be included from within ShLib

#ifndef SHLIBCLAMP_HPP
#define SHLIBCLAMP_HPP

#include "ShGeneric.hpp"

#ifndef _WIN32
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
template<int N, typename T>
ShGeneric<N, T> abs(const ShGeneric<N, T>& var);

/** Ceiling.
 * Returns the least integer >= argument. 
 * Operates componentwise on tuples.
 */
template<int N, typename T>
ShGeneric<N, T> ceil(const ShGeneric<N, T>& var);

/** Floor.
 * Returns the greatest integer <= argument.
 * Operates componentwise on tuples.
 */
template<int N, typename T>
ShGeneric<N, T> floor(const ShGeneric<N, T>& var);

/** Round.
 * Returns the nearest integer to the argument.
 * Operates componentwise on tuples.
 */
template<int N, typename T>
ShGeneric<N, T> round(const ShGeneric<N, T>& var);

/** Float modulus. 
 * The result is always positive.
 */
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
mod(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right);
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
mod(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right);
template<typename T1, typename T2>
ShGeneric<1, CT1T2>
mod(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right);
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator%(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right);
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator%(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right);
template<typename T1, typename T2>
ShGeneric<1, CT1T2>
operator%(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right);

SH_SHLIB_CONST_SCALAR_OP_DECL(mod);
SH_SHLIB_CONST_N_OP_LEFT_DECL(mod);
SH_SHLIB_CONST_SCALAR_OP_DECL(operator%);
SH_SHLIB_CONST_N_OP_LEFT_DECL(operator%);

/** Fractional part.
 * Discards the integer part of each componenent in var.
 */
template<int N, typename T>
ShGeneric<N, T> frac(const ShGeneric<N, T>& var);

/** Take positive part. 
 * Clamps a value to zero if it is negative.   
 * This is useful to wrap dot products in lighting models.
 */
template<int N, typename T>
ShGeneric<N, T> pos(const ShGeneric<N, T>& x);

/** Maximum.
 * Creates a tuple of componentwise maximums of a pair of input tuples.
 */
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
max(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right);

SH_SHLIB_CONST_SCALAR_OP_DECL(max);

/** Minimum.
 * Creates a tuple of componentwise minimums of a pair of input tuples.
 */
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
min(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right);

SH_SHLIB_CONST_SCALAR_OP_DECL(min);

/** Maximum of all components
 */
template<int N, typename T>
ShGeneric<1, T> max(const ShGeneric<N, T>& a);

/** Minimum of all components
 */
template<int N, typename T>
ShGeneric<1, T> min(const ShGeneric<N, T>& a);

/** Componentwise clamping.
 * Clamps a between b and c.
 */
template<int N, typename T1, typename T2, typename T3>
ShGeneric<N, CT1T2T3> 
clamp(const ShGeneric<N, T1>& a, const ShGeneric<N, T2>& b, const ShGeneric<N, T3>& c);
template<int N, typename T1, typename T2, typename T3>
ShGeneric<N, CT1T2T3> 
clamp(const ShGeneric<N, T1>& a, const ShGeneric<1, T2>& b, const ShGeneric<1, T3>& c);
template<typename T1, typename T2, typename T3>
ShGeneric<1, CT1T2T3> 
clamp(const ShGeneric<1, T1>& a, const ShGeneric<1, T2>& b, const ShGeneric<1, T3>& c);

SH_SHLIB_CONST_TRINARY_OP_011_DECL(clamp);

/** Componentwise saturation.
 * Equivalent to componentwise minimum with 1.
 */
template<int N, typename T>
ShGeneric<N, T> sat(const ShGeneric<N, T>& a);

/** Componentwise sign.
 * Returns -1.0 if argument is less than 0.0, 1.0 if argument is greater
 * than 0.0, 0.0 otherwise.
 * Operates componentwise on tuples.
 */
template<int N, typename T>
ShGeneric<N, T> sign(const ShGeneric<N, T>& var);


/** Linear step.
 * Linear step of t centered around c with width w.
 */
template <int N, typename T>
ShGeneric<N, T> linstep(const ShGeneric<N, T>& t, const ShGeneric<N, T>& c,
                        const ShGeneric<N, T>& w);

/** \brief Cubic interpolated step between 0 and 1. 
 * Returns:
 *    0 if x < a, 
 *    1 if x > b, 
 * cubic interpolation between 0 and 1 otherwise
 */
template<int N, typename T>
ShGeneric<N, T> smoothstep(const ShGeneric<N, T>& a, const ShGeneric<N, T>& b,
                           const ShGeneric<N, T>& x);

/** Linear smooth pulse.
 * Linear smooth pulse of t between r0 and r1 with width w.
 */
template <int N, typename T>
ShGeneric<N, T> smoothpulse(const ShGeneric<N, T>& t, const ShGeneric<N, T>& r0, 
                            const ShGeneric<N, T>& r1, const ShGeneric<N, T>& w);

/*@}*/

}
#endif

#include "ShLibClampImpl.hpp"

#endif
