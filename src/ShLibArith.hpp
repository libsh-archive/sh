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
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator+(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right);
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator+(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right);
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator+(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right);
template<typename T1, typename T2>
ShGeneric<1, CT1T2> operator+(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right);
SH_SHLIB_CONST_SCALAR_OP(operator+);
SH_SHLIB_CONST_N_OP_BOTH(operator+);

/** Subtraction.
 * On tuples, this operator acts componentwise.
 * @todo scalar promotion.
 */
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator-(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right);
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator-(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right);
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator-(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right);
template<typename T1, typename T2>
ShGeneric<1, CT1T2> operator-(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right);
SH_SHLIB_CONST_SCALAR_OP(operator-);
SH_SHLIB_CONST_N_OP_BOTH(operator-);

/** Multiplication.
 * On tuples, this operator acts componentwise.
 * If a scalar is multiplied by a tuple, the scalar is promoted by
 * duplication to a tuple.
 */
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator*(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right);
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator*(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right);
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator*(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right);
template<typename T1, typename T2>
ShGeneric<1, CT1T2> operator*(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right);
SH_SHLIB_CONST_SCALAR_OP(operator*);
SH_SHLIB_CONST_N_OP_BOTH(operator*);

/** Division.
 * On tuples, this operator acts componentwise.
 * If a tuple is divided by a scalar (or vice versa), the scalar is promoted by
 * duplication to a tuple.
 */
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator/(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right);
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator/(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right);
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator/(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right);
template<typename T1, typename T2>
ShGeneric<1, CT1T2> operator/(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right);
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

/** Power.
 * Raise a tuple to a power.
 * @todo scalar promotion.
 */
template<int N, typename T1, typename T2, typename T3>
ShGeneric<N, CT1T2>
pow(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right);
template<int N, typename T1, typename T2, typename T3>
ShGeneric<N, CT1T2>
pow(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right);
template<typename T1, typename T2, typename T3>
ShGeneric<1, CT1T2> pow(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right);

SH_SHLIB_CONST_SCALAR_OP(pow);
SH_SHLIB_CONST_N_OP_RIGHT(pow);

/** Multiply and add.
 * This is an intrinsic to access the assembly instruction of the same name.
 * Multiply-add is potentially cheaper than a separate multiply and
 * add.  Note: potentially.
 */
template<int N, typename T1, typename T2, typename T3>
ShGeneric<N, CT1T2T3>
mad(const ShGeneric<N, T1>& m1, const ShGeneric<N, T2>& m2, 
                    const ShGeneric<N, T3>& a);
template<int N, typename T1, typename T2, typename T3>
ShGeneric<N, CT1T2T3>
mad(const ShGeneric<N, T1>& m1, const ShGeneric<1, T2>& m2, 
                    const ShGeneric<N, T3>& a);
template<int N, typename T1, typename T2, typename T3>
ShGeneric<N, CT1T2T3>
mad(const ShGeneric<1, T1>& m1, const ShGeneric<N, T2>& m2, 
                    const ShGeneric<N, T3>& a);
template<typename T1, typename T2, typename T3>
ShGeneric<1, CT1T2T3> mad(const ShGeneric<1, T1>& m1, const ShGeneric<1, T2>& m2, 
                    const ShGeneric<1, T3>& a);

//@todo type should not use double here, but overloading problems need to be
//resolved
template<int N, typename T1, typename T2> 
ShGeneric<N, CT1T2>
mad(double m1, const ShGeneric<N, T1>& m2, const ShGeneric<N, T2>& a);
template<int N, typename T1, typename T2> 
ShGeneric<N, CT1T2>
mad(const ShGeneric<N, T1>& m1, double m2, const ShGeneric<N, T2>& a);

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

/*@}*/

/** Linear interpolation.
 * Blend between two tuples.   The blend value can be a scalar
 * or a tuple.
 */
template<int N, typename T1, typename T2, typename T3>
ShGeneric<N, CT1T2T3>
lerp(const ShGeneric<N, T1>& f, const ShGeneric<N, T2>& a, 
                     const ShGeneric<N, T3>& b);
template<int N, typename T1, typename T2, typename T3>
ShGeneric<N, CT1T2T3>
lerp(const ShGeneric<1, T1>& f, const ShGeneric<N, T2>& a, 
                     const ShGeneric<N, T3>& b);
template<typename T1, typename T2, typename T3>
ShGeneric<1, CT1T2T3> 
lerp(const ShGeneric<1, T1>& f, const ShGeneric<1, T2>& a, 
     const ShGeneric<1, T3>& b);
//@todo type fix double f to a templated type (there are overload resolution
//problems, e.g. if first arg is ShAttrib this function matches better
//than the above functions)
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
lerp(double f, const ShGeneric<N, T1>& a, const ShGeneric<N, T2>& b);
}

#include "ShLibArithImpl.hpp"

#endif
