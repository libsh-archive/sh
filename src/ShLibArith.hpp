// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Viberiu S. Popa,
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
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator+(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right);
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator+(const ShGeneric<1, V1>& left, const ShGeneric<N, V2>& right);
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator+(const ShGeneric<N, V1>& left, const ShGeneric<1, V2>& right);
template<ShValueType V1, ShValueType V2>
ShGeneric<1, CV1V2> operator+(const ShGeneric<1, V1>& left, const ShGeneric<1, V2>& right);
SH_SHLIB_CONST_SCALAR_OP(operator+);
SH_SHLIB_CONST_N_OP_BOTH(operator+);

/** Subtraction.
 * On tuples, this operator acts componentwise.
 * @todo scalar promotion.
 */
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator-(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right);
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator-(const ShGeneric<1, V1>& left, const ShGeneric<N, V2>& right);
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator-(const ShGeneric<N, V1>& left, const ShGeneric<1, V2>& right);
template<ShValueType V1, ShValueType V2>
ShGeneric<1, CV1V2> operator-(const ShGeneric<1, V1>& left, const ShGeneric<1, V2>& right);
SH_SHLIB_CONST_SCALAR_OP(operator-);
SH_SHLIB_CONST_N_OP_BOTH(operator-);

/** Multiplication.
 * On tuples, this operator acts componentwise.
 * If a scalar is multiplied by a tuple, the scalar is promoted by
 * duplication to a tuple.
 */
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator*(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right);
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator*(const ShGeneric<1, V1>& left, const ShGeneric<N, V2>& right);
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator*(const ShGeneric<N, V1>& left, const ShGeneric<1, V2>& right);
template<ShValueType V1, ShValueType V2>
ShGeneric<1, CV1V2> operator*(const ShGeneric<1, V1>& left, const ShGeneric<1, V2>& right);
SH_SHLIB_CONST_SCALAR_OP(operator*);
SH_SHLIB_CONST_N_OP_BOTH(operator*);

/** Division.
 * On tuples, this operator acts componentwise.
 * If a tuple is divided by a scalar (or vice versa), the scalar is promoted by
 * duplication to a tuple.
 */
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator/(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right);
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator/(const ShGeneric<N, V1>& left, const ShGeneric<1, V2>& right);
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator/(const ShGeneric<1, V1>& left, const ShGeneric<N, V2>& right);
template<ShValueType V1, ShValueType V2>
ShGeneric<1, CV1V2> operator/(const ShGeneric<1, V1>& left, const ShGeneric<1, V2>& right);
SH_SHLIB_CONST_SCALAR_OP(operator/);
SH_SHLIB_CONST_N_OP_LEFT(operator/);


/** Natural Exponent.
 * Operates componentwise on tuples.
 * Returns the natural exponent of x.
 */
template<int N, ShValueType V>
ShGeneric<N, V> exp(const ShGeneric<N, V>& var);

/** Exponent base 2.
 * Operates componentwise on tuples.
 * Returns the exponent of x using base 2.
 */
template<int N, ShValueType V>
ShGeneric<N, V> exp2(const ShGeneric<N, V>& var);

/** Exponent base 10.
 * Operates componentwise on tuples.
 * Returns the exponent of x using base 10.
 */
template<int N, ShValueType V>
ShGeneric<N, V> exp(const ShGeneric<N, V>& var);

/** Minus-one Exponent base 10.
 * Operates componentwise on tuples.
 * Returns the exponent of x - 1 using base 10.
 */
template<int N, ShValueType V>
ShGeneric<N, V> expm1(const ShGeneric<N, V>& x);

/** Natural Logarithm.
 * Operates componentwise on tuples.
 * Returns the natural logarithm of x.
 */
template<int N, ShValueType V>
ShGeneric<N, V> log(const ShGeneric<N, V>& var);

/** Logarithm base 2.
 * Operates componentwise on tuples.
 * Returns the logarithm of x using base 2.
 */
template<int N, ShValueType V>
ShGeneric<N, V> log2(const ShGeneric<N, V>& var);

/** Logarithm base 10.
 * Operates componentwise on tuples.
 * Returns the logarithm of x using base 10.
 */
template<int N, ShValueType V>
ShGeneric<N, V> log(const ShGeneric<N, V>& var);

/** Plus-One Logarithm base 10.
 * Operates componentwise on tuples.
 * Returns the logarithm of x + 1 using base 10.
 */
template<int N, ShValueType V>
ShGeneric<N, V> logp1(const ShGeneric<N, V>& x);

/** Power.
 * Raise a tuple to a power.
 * @todo scalar promotion.
 */
template<int N, ShValueType V1, ShValueType V2, ShValueType V3>
ShGeneric<N, CV1V2>
pow(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right);
template<int N, ShValueType V1, ShValueType V2, ShValueType V3>
ShGeneric<N, CV1V2>
pow(const ShGeneric<N, V1>& left, const ShGeneric<1, V2>& right);
template<ShValueType V1, ShValueType V2, ShValueType V3>
ShGeneric<1, CV1V2> pow(const ShGeneric<1, V1>& left, const ShGeneric<1, V2>& right);

SH_SHLIB_CONST_SCALAR_OP(pow);
SH_SHLIB_CONST_N_OP_RIGHT(pow);

/** Multiply and add.
 * This is an intrinsic to access the assembly instruction of the same name.
 * Multiply-add is potentially cheaper than a separate multiply and
 * add.  Note: potentially.
 */
template<int N, ShValueType V1, ShValueType V2, ShValueType V3>
ShGeneric<N, CV1V2V3>
mad(const ShGeneric<N, V1>& m1, const ShGeneric<N, V2>& m2, 
                    const ShGeneric<N, V3>& a);
template<int N, ShValueType V1, ShValueType V2, ShValueType V3>
ShGeneric<N, CV1V2V3>
mad(const ShGeneric<N, V1>& m1, const ShGeneric<1, V2>& m2, 
                    const ShGeneric<N, V3>& a);
template<int N, ShValueType V1, ShValueType V2, ShValueType V3>
ShGeneric<N, CV1V2V3>
mad(const ShGeneric<1, V1>& m1, const ShGeneric<N, V2>& m2, 
                    const ShGeneric<N, V3>& a);
template<ShValueType V1, ShValueType V2, ShValueType V3>
ShGeneric<1, CV1V2V3> mad(const ShGeneric<1, V1>& m1, const ShGeneric<1, V2>& m2, 
                    const ShGeneric<1, V3>& a);

//@todo type should not use double here, but overloading problems need to be
//resolved
//template<int N, ShValueType V> 
//ShGeneric<N, V> 
//mad(T m1, const ShGeneric<N, V>& m2, const ShGeneric<N, V>& a);
//template<int N, ShValueType V> 
//ShGeneric<N, V>
//mad(const ShGeneric<N, V>& m1, V m2, const ShGeneric<N, V>& a);

//@todo type not sure these are a good idea
template<int N, ShValueType V1, ShValueType V2> 
ShGeneric<N, CV1V2> 
mad(double m1, const ShGeneric<N, V1>& m2, const ShGeneric<N, V2>& a);
template<int N, ShValueType V1, ShValueType V2> 
ShGeneric<N, CV1V2>
mad(const ShGeneric<N, V1>& m1, double m2, const ShGeneric<N, V2>& a);

/* Reciprocal
 * One divided by the given value, for each component.
 */
template<int N, ShValueType V>
ShGeneric<N, V> rcp(const ShGeneric<N, V>& var);

/* Square root.
 * The square root of each component of the input is evaluated.
 */
template<int N, ShValueType V>
ShGeneric<N, V> sqrt(const ShGeneric<N, V>& var);

/* Reciprocal square root.
 * The inverse of the square root of each component of the input is evaluated.
 */
template<int N, ShValueType V>
ShGeneric<N, V> rsqrt(const ShGeneric<N, V>& var);

/* Cube root.
 * The cube root of each component of the input is evaluated.
 */
template<int N, ShValueType V>
ShGeneric<N, V> cbrt(const ShGeneric<N, V>& var);

/*@}*/

/** Linear interpolation.
 * Blend between two tuples.   The blend value can be a scalar
 * or a tuple.
 */
template<int N, ShValueType V1, ShValueType V2, ShValueType V3>
ShGeneric<N, CV1V2V3>
lerp(const ShGeneric<N, V1>& f, const ShGeneric<N, V2>& a, 
                     const ShGeneric<N, V3>& b);
template<int N, ShValueType V1, ShValueType V2, ShValueType V3>
ShGeneric<N, CV1V2V3>
lerp(const ShGeneric<1, V1>& f, const ShGeneric<N, V2>& a, 
                     const ShGeneric<N, V3>& b);
template<ShValueType V1, ShValueType V2, ShValueType V3>
ShGeneric<1, CV1V2V3> 
lerp(const ShGeneric<1, V1>& f, const ShGeneric<1, V2>& a, 
     const ShGeneric<1, V3>& b);

template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2>
lerp(double f, const ShGeneric<N, V1>& a, const ShGeneric<N, V2>& b);


/* Sum of components.
 * Addition of all components into a single result.
 */
template<int N, ShValueType V>
ShGeneric<1, V> sum(const ShGeneric<N, V>& var);

/* Product of components.
 * Multiplication of all components into a single result.
 */
template<int N, ShValueType V>
ShGeneric<1, V> prod(const ShGeneric<N, V>& var);

}

#include "ShLibArithImpl.hpp"

#endif
