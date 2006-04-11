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
#include "Lib.hpp" // LibClamp needs to be included from within Lib

#ifndef SHLIBCLAMP_HPP
#define SHLIBCLAMP_HPP

#include "Generic.hpp"

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
Generic<N, T> abs(const Generic<N, T>& var);

/** Ceiling.
 * Returns the least integer >= argument. 
 * Operates componentwise on tuples.
 */
template<int N, typename T>
Generic<N, T> ceil(const Generic<N, T>& var);

/** Floor.
 * Returns the greatest integer <= argument.
 * Operates componentwise on tuples.
 */
template<int N, typename T>
Generic<N, T> floor(const Generic<N, T>& var);

/** Round.
 * Returns the nearest integer to the argument.
 * Operates componentwise on tuples.
 */
template<int N, typename T>
Generic<N, T> round(const Generic<N, T>& var);

/** Float modulus. 
 * The result is always positive.
 */
template<int N, typename T1, typename T2>
Generic<N, CT1T2>
mod(const Generic<N, T1>& left, const Generic<N, T2>& right);
template<int N, typename T1, typename T2>
Generic<N, CT1T2>
mod(const Generic<N, T1>& left, const Generic<1, T2>& right);
template<typename T1, typename T2>
Generic<1, CT1T2>
mod(const Generic<1, T1>& left, const Generic<1, T2>& right);
template<int N, typename T1, typename T2>
Generic<N, CT1T2>
operator%(const Generic<N, T1>& left, const Generic<N, T2>& right);
template<int N, typename T1, typename T2>
Generic<N, CT1T2>
operator%(const Generic<N, T1>& left, const Generic<1, T2>& right);
template<typename T1, typename T2>
Generic<1, CT1T2>
operator%(const Generic<1, T1>& left, const Generic<1, T2>& right);

SHLIB_CONST_SCALAR_OP_DECL(mod);
SHLIB_CONST_N_OP_LEFT_DECL(mod);
SHLIB_CONST_SCALAR_OP_DECL(operator%);
SHLIB_CONST_N_OP_LEFT_DECL(operator%);

/** Fractional part.
 * Discards the integer part of each componenent in var.
 */
template<int N, typename T>
Generic<N, T> frac(const Generic<N, T>& var);

/** Take positive part. 
 * Clamps a value to zero if it is negative.   
 * This is useful to wrap dot products in lighting models.
 */
template<int N, typename T>
Generic<N, T> pos(const Generic<N, T>& x);

/** Maximum.
 * Creates a tuple of componentwise maximums of a pair of input tuples.
 */
template<int N, typename T1, typename T2>
Generic<N, CT1T2>
max(const Generic<N, T1>& left, const Generic<N, T2>& right);

SHLIB_CONST_SCALAR_OP_DECL(max);

/** Minimum.
 * Creates a tuple of componentwise minimums of a pair of input tuples.
 */
template<int N, typename T1, typename T2>
Generic<N, CT1T2>
min(const Generic<N, T1>& left, const Generic<N, T2>& right);

SHLIB_CONST_SCALAR_OP_DECL(min);

/** Maximum of all components
 */
template<int N, typename T>
Generic<1, T> max(const Generic<N, T>& a);

/** Minimum of all components
 */
template<int N, typename T>
Generic<1, T> min(const Generic<N, T>& a);

/** Componentwise clamping.
 * Clamps a between b and c.
 */
template<int N, typename T1, typename T2, typename T3>
Generic<N, CT1T2T3> 
clamp(const Generic<N, T1>& a, const Generic<N, T2>& b, const Generic<N, T3>& c);
template<int N, typename T1, typename T2, typename T3>
Generic<N, CT1T2T3> 
clamp(const Generic<N, T1>& a, const Generic<1, T2>& b, const Generic<1, T3>& c);
template<typename T1, typename T2, typename T3>
Generic<1, CT1T2T3> 
clamp(const Generic<1, T1>& a, const Generic<1, T2>& b, const Generic<1, T3>& c);

SHLIB_CONST_TRINARY_OP_011_DECL(clamp);

/** Componentwise saturation.
 * Equivalent to componentwise minimum with 1.
 */
template<int N, typename T>
Generic<N, T> sat(const Generic<N, T>& a);

/** Componentwise sign.
 * Returns -1.0 if argument is less than 0.0, 1.0 if argument is greater
 * than 0.0, 0.0 otherwise.
 * Operates componentwise on tuples.
 */
template<int N, typename T>
Generic<N, T> sign(const Generic<N, T>& var);


/** Linear step.
 * Linear step of t centered around c with width w.
 */
template <int N, typename T>
Generic<N, T> linstep(const Generic<N, T>& t, const Generic<N, T>& c,
                        const Generic<N, T>& w);

/** \brief Cubic interpolated step between 0 and 1. 
 * Returns:
 *    0 if x < a, 
 *    1 if x > b, 
 * cubic interpolation between 0 and 1 otherwise
 */
template<int N, typename T>
Generic<N, T> smoothstep(const Generic<N, T>& a, const Generic<N, T>& b,
                           const Generic<N, T>& x);

/** Linear smooth pulse.
 * Linear smooth pulse of t between r0 and r1 with width w.
 */
template <int N, typename T>
Generic<N, T> smoothpulse(const Generic<N, T>& t, const Generic<N, T>& r0, 
                            const Generic<N, T>& r1, const Generic<N, T>& w);

/*@}*/

}
#endif

#include "LibClampImpl.hpp"

#endif
