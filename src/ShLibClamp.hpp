#ifndef SHLIBCLAMP_HPP
#define SHLIBCLAMP_HPP

#include "ShGeneric.hpp"
#include "ShLib.hpp"

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

/** Float modulus. 
 * The result is always positive.
 */
template<int N, typename T>
ShGeneric<N, T> mod(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right);
template<int N, typename T>
ShGeneric<N, T> mod(const ShGeneric<N, T>& left, const ShGeneric<1, T>& right);
template<typename T>
ShGeneric<1, T> mod(const ShGeneric<1, T>& left, const ShGeneric<1, T>& right);
template<int N, typename T>
ShGeneric<N, T> operator%(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right);
template<int N, typename T>
ShGeneric<N, T> operator%(const ShGeneric<N, T>& left, const ShGeneric<1, T>& right);
template<typename T>
ShGeneric<1, T> operator%(const ShGeneric<1, T>& left, const ShGeneric<1, T>& right);

SH_SHLIB_CONST_SCALAR_OP(mod);
SH_SHLIB_CONST_N_OP_LEFT(mod);

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
ShGeneric<N,  T> pos(const ShGeneric<N, T>& x);

/** Maximum.
 * Creates a tuple of componentwise maximums of a pair of input tuples.
 */
template<int N, typename T>
ShGeneric<N,  T> max(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right);

SH_SHLIB_CONST_SCALAR_OP(max);

/** Minimum.
 * Creates a tuple of componentwise minimums of a pair of input tuples.
 */
template<int N, typename T>
ShGeneric<N,  T> min(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right);

SH_SHLIB_CONST_SCALAR_OP(min);

/** Componentwise clamping.
 * Clamps a between b and c.
 */
template<int N, typename T>
ShGeneric<N, T> clamp(const ShGeneric<N, T>& a,
                      const ShGeneric<N, T>& b, const ShGeneric<N, T>& c);
template<int N, typename T>
ShGeneric<N, T> clamp(const ShGeneric<N, T>& a,
                      const ShGeneric<1, T>& b, const ShGeneric<1, T>& c);
template<typename T>
ShGeneric<1, T> clamp(const ShGeneric<1, T>& a,
                      const ShGeneric<1, T>& b, const ShGeneric<1, T>& c);

SH_SHLIB_CONST_TRINARY_OP_011(clamp);

/** Componentwise saturation.
 * Equivalent to componentwise minimum with 1.
 */
template<int N, typename T>
ShGeneric<N, T> sat(const ShGeneric<N, T>& a);

/*@}*/

}

#include "ShLibClampImpl.hpp"

#endif
