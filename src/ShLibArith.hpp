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
#ifndef SHLIBARITH_HPP
#define SHLIBARITH_HPP

#include "ShGeneric.hpp"
#include "ShLib.hpp"

namespace SH {

/** \defgroup lib_arith Arithmetic operations
 * @ingroup library
 * @{
 */

/** Addition.
 * On tuples, this operator acts componentwise.
 * @todo scalar promotion.
 */
template<int N, typename T>
ShGeneric<N, T> operator+(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right);
template<int N, typename T>
ShGeneric<N, T> operator+(const ShGeneric<1, T>& left, const ShGeneric<N, T>& right);
template<int N, typename T>
ShGeneric<N, T> operator+(const ShGeneric<N, T>& left, const ShGeneric<1, T>& right);
template<typename T>
ShGeneric<1, T> operator+(const ShGeneric<1, T>& left, const ShGeneric<1, T>& right);
SH_SHLIB_CONST_SCALAR_OP(operator+);
SH_SHLIB_CONST_N_OP_BOTH(operator+);

/** Subtraction.
 * On tuples, this operator acts componentwise.
 * @todo scalar promotion.
 */
template<int N, typename T>
ShGeneric<N, T> operator-(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right);
template<int N, typename T>
ShGeneric<N, T> operator-(const ShGeneric<1, T>& left, const ShGeneric<N, T>& right);
template<int N, typename T>
ShGeneric<N, T> operator-(const ShGeneric<N, T>& left, const ShGeneric<1, T>& right);
template<typename T>
ShGeneric<1, T> operator-(const ShGeneric<1, T>& left, const ShGeneric<1, T>& right);
SH_SHLIB_CONST_SCALAR_OP(operator-);
SH_SHLIB_CONST_N_OP_BOTH(operator-);

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
template<int N, typename T>
ShGeneric<N, T> operator/(const ShGeneric<1, T>& left, const ShGeneric<N, T>& right);
template<typename T>
ShGeneric<1, T> operator/(const ShGeneric<1, T>& left, const ShGeneric<1, T>& right);
SH_SHLIB_CONST_SCALAR_OP(operator/);
SH_SHLIB_CONST_N_OP_LEFT(operator/);


/** Natural Exponent.
 * Operates componentwise on tuples.
 * Returns the natural exponent of x.
 */
template<int N, typename T>
ShGeneric<N, T> exp(const ShGeneric<N, T>& var);

/** Exponent base 2.
 * Operates componentwise on tuples.
 * Returns the exponent of x using base 2.
 */
template<int N, typename T>
ShGeneric<N, T> exp2(const ShGeneric<N, T>& var);

/** Exponent base 10.
 * Operates componentwise on tuples.
 * Returns the exponent of x using base 10.
 */
template<int N, typename T>
ShGeneric<N, T> exp(const ShGeneric<N, T>& var);

/** Minus-one Exponent base 10.
 * Operates componentwise on tuples.
 * Returns the exponent of x - 1 using base 10.
 */
template<int N, typename T>
ShGeneric<N, T> expm1(const ShGeneric<N, T>& x);

/** Natural Logarithm.
 * Operates componentwise on tuples.
 * Returns the natural logarithm of x.
 */
template<int N, typename T>
ShGeneric<N, T> log(const ShGeneric<N, T>& var);

/** Logarithm base 2.
 * Operates componentwise on tuples.
 * Returns the logarithm of x using base 2.
 */
template<int N, typename T>
ShGeneric<N, T> log2(const ShGeneric<N, T>& var);

/** Logarithm base 10.
 * Operates componentwise on tuples.
 * Returns the logarithm of x using base 10.
 */
template<int N, typename T>
ShGeneric<N, T> log(const ShGeneric<N, T>& var);

/** Plus-One Logarithm base 10.
 * Operates componentwise on tuples.
 * Returns the logarithm of x + 1 using base 10.
 */
template<int N, typename T>
ShGeneric<N, T> logp1(const ShGeneric<N, T>& x);

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

/* Reciprocal
 * One divided by the given value, for each component.
 */
template<int N, typename T>
ShGeneric<N, T> rcp(const ShGeneric<N, T>& var);

/* Square root.
 * The square root of each component of the input is evaluated.
 */
template<int N, typename T>
ShGeneric<N, T> sqrt(const ShGeneric<N, T>& var);

/* Reciprocal square root.
 * The inverse of the square root of each component of the input is evaluated.
 */
template<int N, typename T>
ShGeneric<N, T> rsqrt(const ShGeneric<N, T>& var);

/* Cube root.
 * The cube root of each component of the input is evaluated.
 */
template<int N, typename T>
ShGeneric<N, T> cbrt(const ShGeneric<N, T>& var);

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


/* Sum of components.
 * Addition of all components into a single result.
 */
template<int N, typename T>
ShGeneric<1, T> sum(const ShGeneric<N, T>& var);

/* Product of components.
 * Multiplication of all components into a single result.
 */
template<int N, typename T>
ShGeneric<1, T> prod(const ShGeneric<N, T>& var);

}

#include "ShLibArithImpl.hpp"

#endif
