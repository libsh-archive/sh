#ifndef SHLIBARITH_HPP
#define SHLIBARITH_HPP

#include "ShGeneric.hpp"
#include "ShLib.hpp"

namespace SH {

/** \defgroup lib_arith Arithmetic operations
 * @{
 */

/** Addition.
 * On tuples, this operator acts componentwise.
 * @todo scalar promotion.
 */
template<int N, typename T>
ShGeneric<N, T> operator+(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right);
SH_SHLIB_CONST_SCALAR_OP(operator+);

/** Subtraction.
 * On tuples, this operator acts componentwise.
 * @todo scalar promotion.
 */
template<int N, typename T>
ShGeneric<N, T> operator-(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right);
SH_SHLIB_CONST_SCALAR_OP(operator-);

/** Multiplication.
 * On tuples, this operator acts componentwise.
 * If a scalar is multiplied by a tuple, the scalar is promoted by
 * duplication to a tuple.
 */
template<int N, typename T>
ShGeneric<N, T> operator*(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right);
template<int N, typename T>
ShGeneric<N, T> operator*(const ShGeneric<1, T>& left, const ShGeneric<N, T>& right);
template<int N, typename T>
ShGeneric<N, T> operator*(const ShGeneric<N, T>& left, const ShGeneric<1, T>& right);
template<typename T>
ShGeneric<1, T> operator*(const ShGeneric<1, T>& left, const ShGeneric<1, T>& right);

SH_SHLIB_CONST_SCALAR_OP(operator*);
SH_SHLIB_CONST_N_OP_BOTH(operator*);

/** Division.
 * On tuples, this operator acts componentwise.
 * If a tuple is divided by a scalar (or vice versa), the scalar is promoted by
 * duplication to a tuple.
 */
template<int N, typename T>
ShGeneric<N, T> operator/(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right);
template<int N, typename T>
ShGeneric<N, T> operator/(const ShGeneric<N, T>& left, const ShGeneric<1, T>& right);
template<typename T>
ShGeneric<1, T> operator/(const ShGeneric<1, T>& left, const ShGeneric<1, T>& right);

SH_SHLIB_CONST_SCALAR_OP(operator/);
SH_SHLIB_CONST_N_OP_LEFT(operator/);

/** Power.
 * Raise a tuple to a power.
 * @todo scalar promotion.
 */
template<int N, typename T>
ShGeneric<N, T> pow(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right);
template<int N, typename T>
ShGeneric<N, T> pow(const ShGeneric<N, T>& left, const ShGeneric<1, T>& right);
template<typename T>
ShGeneric<1, T> pow(const ShGeneric<1, T>& left, const ShGeneric<1, T>& right);

SH_SHLIB_CONST_SCALAR_OP(pow);
SH_SHLIB_CONST_N_OP_RIGHT(pow);

/** Multiply and add.
 * This is an intrinsic to access the assembly instruction of the same name.
 * Multiply-add is potentially cheaper than a separate multiply and
 * add.  Note: potentially.
 */
template<int N, typename T>
ShGeneric<N, T> mad(const ShGeneric<N, T>& m1, const ShGeneric<N, T>& m2, 
                    const ShGeneric<N, T>& a);
template<int N, typename T>
ShGeneric<N, T> mad(const ShGeneric<N, T>& m1, const ShGeneric<1, T>& m2, 
                    const ShGeneric<N, T>& a);
template<int N, typename T>
ShGeneric<N, T> mad(const ShGeneric<1, T>& m1, const ShGeneric<N, T>& m2, 
                    const ShGeneric<N, T>& a);
template<typename T>
ShGeneric<1, T> mad(const ShGeneric<1, T>& m1, const ShGeneric<1, T>& m2, 
                    const ShGeneric<1, T>& a);

template<int N, typename T> 
ShGeneric<N, T> mad(T m1, const ShGeneric<N, T>& m2, const ShGeneric<N, T>& a);
template<int N, typename T> 
ShGeneric<N, T> mad(double m1, const ShGeneric<N, T>& m2, const ShGeneric<N, T>& a);
template<int N, typename T> 
ShGeneric<N, T> mad(const ShGeneric<N, T>& m1, T m2, const ShGeneric<N, T>& a);
template<int N, typename T> 
ShGeneric<N, T> mad(const ShGeneric<N, T>& m1, double m2, const ShGeneric<N, T>& a);

/* Square root.
 * The square root of each component of the input is evaluated.
 * @todo should add reciprocal square root (rsqrt) as an intrinsic.
 */
template<int N, typename T>
ShGeneric<N, T> sqrt(const ShGeneric<N, T>& var);

/*@}*/

/** Linear interpolation.
 * Blend between two tuples.   The blend value can be a scalar
 * or a tuple.
 */
template<int N, typename T>
ShGeneric<N, T> lerp(const ShGeneric<N, T>& f, const ShGeneric<N, T>& a, 
                     const ShGeneric<N, T>& b);
template<int N, typename T>
ShGeneric<N, T> lerp(const ShGeneric<1, T>& f, const ShGeneric<N, T>& a, 
                     const ShGeneric<N, T>& b);
template<typename T>
ShGeneric<1, T> lerp(const ShGeneric<1, T>& f, const ShGeneric<1, T>& a, 
                     const ShGeneric<1, T>& b);
template<int N, typename T>
ShGeneric<N, T> lerp(T f, const ShGeneric<N, T>& a, 
                     const ShGeneric<N, T>& b);
}

#include "ShLibArithImpl.hpp"

#endif
