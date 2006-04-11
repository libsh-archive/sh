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
#include "Lib.hpp" // LibArith needs to be included from within Lib

#ifndef SHLIBARITH_HPP
#define SHLIBARITH_HPP

#include "Generic.hpp"

#ifndef _WIN32
namespace SH {

/** \defgroup lib_arith Arithmetic operations
 * @ingroup library
 * @{
 */

/** Addition.
 * On tuples, this operator acts componentwise.
 * @todo scalar promotion.
 */
template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator+(const Generic<N, T1>& left, const Generic<N, T2>& right);
template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator+(const Generic<1, T1>& left, const Generic<N, T2>& right);
template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator+(const Generic<N, T1>& left, const Generic<1, T2>& right);
template<typename T1, typename T2>
Generic<1, CT1T2> operator+(const Generic<1, T1>& left, const Generic<1, T2>& right);
SHLIB_CONST_SCALAR_OP_DECL(operator+);
SHLIB_CONST_N_OP_BOTH_DECL(operator+);

/** Subtraction.
 * On tuples, this operator acts componentwise.
 * @todo scalar promotion.
 */
template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator-(const Generic<N, T1>& left, const Generic<N, T2>& right);
template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator-(const Generic<1, T1>& left, const Generic<N, T2>& right);
template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator-(const Generic<N, T1>& left, const Generic<1, T2>& right);
template<typename T1, typename T2>
Generic<1, CT1T2> operator-(const Generic<1, T1>& left, const Generic<1, T2>& right);
SHLIB_CONST_SCALAR_OP_DECL(operator-);
SHLIB_CONST_N_OP_BOTH_DECL(operator-);

/** Multiplication.
 * On tuples, this operator acts componentwise.
 * If a scalar is multiplied by a tuple, the scalar is promoted by
 * duplication to a tuple.
 */
template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator*(const Generic<N, T1>& left, const Generic<N, T2>& right);
template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator*(const Generic<1, T1>& left, const Generic<N, T2>& right);
template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator*(const Generic<N, T1>& left, const Generic<1, T2>& right);
template<typename T1, typename T2>
Generic<1, CT1T2> operator*(const Generic<1, T1>& left, const Generic<1, T2>& right);
SHLIB_CONST_SCALAR_OP_DECL(operator*);
SHLIB_CONST_N_OP_BOTH_DECL(operator*);

/** Division.
 * On tuples, this operator acts componentwise.
 * If a tuple is divided by a scalar (or vice versa), the scalar is promoted by
 * duplication to a tuple.
 */
template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator/(const Generic<N, T1>& left, const Generic<N, T2>& right);
template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator/(const Generic<N, T1>& left, const Generic<1, T2>& right);
template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator/(const Generic<1, T1>& left, const Generic<N, T2>& right);
template<typename T1, typename T2>
Generic<1, CT1T2> operator/(const Generic<1, T1>& left, const Generic<1, T2>& right);
SHLIB_CONST_SCALAR_OP_DECL(operator/);
SHLIB_CONST_N_OP_LEFT_DECL(operator/);


/** Natural Exponent.
 * Operates componentwise on tuples.
 * Returns the natural exponent of x.
 */
template<int N, typename T>
Generic<N, T> exp(const Generic<N, T>& var);

/** Exponent base 2.
 * Operates componentwise on tuples.
 * Returns the exponent of x using base 2.
 */
template<int N, typename T>
Generic<N, T> exp2(const Generic<N, T>& var);

/** Exponent base 10.
 * Operates componentwise on tuples.
 * Returns the exponent of x using base 10.
 */
template<int N, typename T>
Generic<N, T> exp(const Generic<N, T>& var);

/** Minus-one Exponent base 10.
 * Operates componentwise on tuples.
 * Returns the exponent of x - 1 using base 10.
 */
template<int N, typename T>
Generic<N, T> expm1(const Generic<N, T>& x);

/** Natural Logarithm.
 * Operates componentwise on tuples.
 * Returns the natural logarithm of x.
 */
template<int N, typename T>
Generic<N, T> log(const Generic<N, T>& var);

/** Logarithm base 2.
 * Operates componentwise on tuples.
 * Returns the logarithm of x using base 2.
 */
template<int N, typename T>
Generic<N, T> log2(const Generic<N, T>& var);

/** Logarithm base 10.
 * Operates componentwise on tuples.
 * Returns the logarithm of x using base 10.
 */
template<int N, typename T>
Generic<N, T> log(const Generic<N, T>& var);

/** Plus-One Logarithm base 10.
 * Operates componentwise on tuples.
 * Returns the logarithm of x + 1 using base 10.
 */
template<int N, typename T>
Generic<N, T> logp1(const Generic<N, T>& x);

/** Power.
 * Raise a tuple to a power.
 * @todo scalar promotion.
 */
template<int N, typename T1, typename T2, typename T3>
Generic<N, CT1T2>
pow(const Generic<N, T1>& left, const Generic<N, T2>& right);
template<int N, typename T1, typename T2, typename T3>
Generic<N, CT1T2>
pow(const Generic<N, T1>& left, const Generic<1, T2>& right);
template<typename T1, typename T2, typename T3>
Generic<1, CT1T2> pow(const Generic<1, T1>& left, const Generic<1, T2>& right);

SHLIB_CONST_SCALAR_OP_DECL(pow);
SHLIB_CONST_N_OP_RIGHT_DECL(pow);

/** Multiply and add.
 * This is an intrinsic to access the assembly instruction of the same name.
 * Multiply-add is potentially cheaper than a separate multiply and
 * add.  Note: potentially.
 */
template<int N, typename T1, typename T2, typename T3>
Generic<N, CT1T2T3>
mad(const Generic<N, T1>& m1, const Generic<N, T2>& m2, 
    const Generic<N, T3>& a);
template<int N, typename T1, typename T2, typename T3>
Generic<N, CT1T2T3>
mad(const Generic<N, T1>& m1, const Generic<N, T2>& m2, 
    const Generic<1, T3>& a);
template<int N, typename T1, typename T2, typename T3>
Generic<N, CT1T2T3>
mad(const Generic<N, T1>& m1, const Generic<1, T2>& m2, 
    const Generic<N, T3>& a);
template<int N, typename T1, typename T2, typename T3>
Generic<N, CT1T2T3>
mad(const Generic<N, T1>& m1, const Generic<1, T2>& m2, 
    const Generic<1, T3>& a);
template<int N, typename T1, typename T2, typename T3>
Generic<N, CT1T2T3>
mad(const Generic<1, T1>& m1, const Generic<N, T2>& m2, 
    const Generic<N, T3>& a);
template<int N, typename T1, typename T2, typename T3>
Generic<N, CT1T2T3>
mad(const Generic<1, T1>& m1, const Generic<N, T2>& m2, 
    const Generic<1, T3>& a);
template<int N, typename T1, typename T2, typename T3>
Generic<N, CT1T2T3>
mad(const Generic<1, T1>& m1, const Generic<1, T2>& m2, 
    const Generic<N, T3>& a);
template<typename T1, typename T2, typename T3>
Generic<1, CT1T2T3>
mad(const Generic<1, T1>& m1, const Generic<1, T2>& m2, 
    const Generic<1, T3>& a);

template<int N, typename T1, typename T2>
Generic<N, CT1T2> 
mad(const CT1T2& m1, const Generic<N, T1>& m2, const Generic<N, T2>& a);
template<int N, typename T1, typename T2> 
Generic<N, CT1T2>
mad(const Generic<N, T1>& m1, const CT1T2& m2, const Generic<N, T2>& a);

/* Reciprocal
 * One divided by the given value, for each component.
 */
template<int N, typename T>
Generic<N, T> rcp(const Generic<N, T>& var);

/* Square root.
 * The square root of each component of the input is evaluated.
 */
template<int N, typename T>
Generic<N, T> sqrt(const Generic<N, T>& var);

/* Reciprocal square root.
 * The inverse of the square root of each component of the input is evaluated.
 */
template<int N, typename T>
Generic<N, T> rsqrt(const Generic<N, T>& var);

/* Cube root.
 * The cube root of each component of the input is evaluated.
 */
template<int N, typename T>
Generic<N, T> cbrt(const Generic<N, T>& var);

/*@}*/

/** Linear interpolation.
 * Blend between two tuples.   The blend value can be a scalar
 * or a tuple.
 */
template<int N, typename T1, typename T2, typename T3>
Generic<N, CT1T2T3>
lerp(const Generic<N, T1>& f, const Generic<N, T2>& a, 
                     const Generic<N, T3>& b);
template<int N, typename T1, typename T2, typename T3>
Generic<N, CT1T2T3>
lerp(const Generic<1, T1>& f, const Generic<N, T2>& a, 
                     const Generic<N, T3>& b);
template<typename T1, typename T2, typename T3>
Generic<1, CT1T2T3> 
lerp(const Generic<1, T1>& f, const Generic<1, T2>& a, 
     const Generic<1, T3>& b);

template<int N, typename T1, typename T2>
Generic<N, CT1T2>
lerp(const CT1T2& f, const Generic<N, T1>& a, const Generic<N, T2>& b);


/* Sum of components.
 * Addition of all components into a single result.
 */
template<int N, typename T>
Generic<1, T> sum(const Generic<N, T>& var);

/* Product of components.
 * Multiplication of all components into a single result.
 */
template<int N, typename T>
Generic<1, T> prod(const Generic<N, T>& var);

}
#endif

#include "LibArithImpl.hpp"

#endif
