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
#ifndef SHLIB_HPP
#define SHLIB_HPP

#include <cmath>
#include <cassert>
#include "ShVariable.hpp"
#include "ShAttrib.hpp"
#include "ShMatrix.hpp"
#include "ShEnvironment.hpp"

/** \file ShLib.hpp
 * \brief The Sh library functions.
 * If you add or remove functions in this file be sure to modify the
 * other ShLib*.hpp (e.g. ShLibVector.hpp) files appropriately. See
 * the bottom of the file for a list.
 */

#define SH_SHLIB_CONST_SCALAR_OP(operation) \
template<typename T> \
ShAttrib<1, SH_TEMP, T> operation(const ShGeneric<1, T>& left, T right) \
{ \
  return operation(left, ShAttrib<1, SH_CONST, T>(right)); \
} \
template<typename T> \
ShGeneric<1, T> operation(T left, const ShGeneric<1, T>& right) \
{ \
  return operation(ShAttrib<1, SH_CONST, T>(left), right); \
} \
template<typename T> \
ShGeneric<1, T> operation(const ShGeneric<1, T>& left, double right) \
{ \
  return operation(left, ShAttrib<1, SH_CONST, T>(right)); \
} \
template<typename T> \
ShGeneric<1, T> operation(double left, const ShGeneric<1, T>& right) \
{ \
  return operation(ShAttrib<1, SH_CONST, T>(left), right); \
}

#define SH_SHLIB_CONST_N_OP_RETSIZE_LEFT(operation, retsize) \
template<int N, typename T> \
ShGeneric<retsize, T> operation(const ShGeneric<N, T>& left, T right) \
{ \
  return operation(left, ShAttrib<1, SH_CONST, T>(right)); \
} \
template<int N, typename T> \
ShGeneric<retsize, T> operation(const ShGeneric<N, T>& left, double right) \
{ \
  return operation(left, ShAttrib<1, SH_CONST, T>(right)); \
}

#define SH_SHLIB_CONST_N_OP_RETSIZE_RIGHT(operation, retsize) \
template<int N, typename T> \
ShGeneric<retsize, T> operation(T left, const ShGeneric<N, T>& right) \
{ \
  return operation(ShAttrib<1, SH_CONST, T>(left), right); \
} \
template<int N, typename T> \
ShGeneric<retsize, T> operation(double left, const ShGeneric<N, T>& right) \
{ \
  return operation(ShAttrib<1, SH_CONST, T>(left), right); \
}

#define SH_SHLIB_CONST_N_OP_LEFT(operation) \
  SH_SHLIB_CONST_N_OP_RETSIZE_LEFT(operation, N);

#define SH_SHLIB_CONST_N_OP_RIGHT(operation) \
  SH_SHLIB_CONST_N_OP_RETSIZE_RIGHT(operation, N);

#define SH_SHLIB_CONST_N_OP_BOTH(operation) \
SH_SHLIB_CONST_N_OP_LEFT(operation); \
SH_SHLIB_CONST_N_OP_RIGHT(operation);

#define SH_SHLIB_CONST_N_OP_RETSIZE_BOTH(operation, retsize) \
SH_SHLIB_CONST_N_OP_RETSIZE_LEFT(operation, retsize); \
SH_SHLIB_CONST_N_OP_RETSIZE_RIGHT(operation, retsize);

namespace SH {

/** Arithmetic Operations.
 */ ///@{

/** Addition.
 * On tuples, this operator acts componentwise.
 * TODO: scalar promotion.
 */
template<int N, typename T>
ShGeneric<N, T> operator+(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right)
{
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues());
    assert(right.hasValues());
    T lvals[N];
    left.getValues(lvals);
    T rvals[N];
    right.getValues(rvals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = lvals[i] + rvals[i];
    return ShAttrib<N, SH_CONST, T>(result);
  } else {
    ShAttrib<N, SH_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_ADD, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

SH_SHLIB_CONST_SCALAR_OP(operator+);

/** Subtraction.
 * On tuples, this operator acts componentwise.
 * TODO: scalar promotion.
 */
template<int N, typename T>
ShGeneric<N, T> operator-(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right)
{
  return left + (-right);
}

SH_SHLIB_CONST_SCALAR_OP(operator-);

/** Multiplication.
 * On tuples, this operator acts componentwise.
 * If a scalar is multiplied by a tuple, the scalar is promoted by
 * duplication to a tuple.
 */
template<int N, int M, typename T>
ShGeneric<N, T> operator*(const ShGeneric<N, T>& left, const ShGeneric<M, T>& right)
{
  SH_STATIC_CHECK(N == M || N == 1 || M == 1, Multiplication_Components_Do_Not_Match);
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues());
    assert(right.hasValues());
    T lvals[N];
    left.getValues(lvals);
    T rvals[M];
    right.getValues(rvals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = lvals[i] * rvals[M == 1 ? 0 : i];
    return ShAttrib<N, SH_CONST, T>(result);
  } else {
    ShAttrib<N, SH_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_MUL, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);

    return t;
  }
}

template<int M, typename T>
ShGeneric<M, T> operator*(const ShGeneric<1, T>& left, const ShGeneric<M, T>& right)
{
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues());
    assert(right.hasValues());
    T lvals[1];
    left.getValues(lvals);
    T rvals[M];
    right.getValues(rvals);
    T result[M];
    for (int i = 0; i < M; i++) result[i] = lvals[0] * rvals[i];
    return ShAttrib<M, SH_CONST, T>(result);
  } else {
    ShAttrib<M, SH_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_MUL, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);

    return t;
  }
}

SH_SHLIB_CONST_SCALAR_OP(operator*);
SH_SHLIB_CONST_N_OP_BOTH(operator*);

/** Division.
 * On tuples, this operator acts componentwise.
 * If a tuple is divided by a scalar (or vice versa), the scalar is promoted by
 * duplication to a tuple.
 */
template<int N, int M, typename T>
ShGeneric<N, T> operator/(const ShGeneric<N, T>& left, const ShGeneric<M, T>& right)
{
  SH_STATIC_CHECK(M == N || M == 1, Division_Components_Do_Not_Match);
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues());
    assert(right.hasValues());
    T lvals[N];
    left.getValues(lvals);
    T rvals[M];
    right.getValues(rvals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = lvals[i] / rvals[M == 1 ? 0 : i];
    return ShAttrib<N, SH_CONST, T>(result);
  } else {
    ShAttrib<N, SH_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_DIV, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    
    return t;
  }
}

SH_SHLIB_CONST_SCALAR_OP(operator/);
SH_SHLIB_CONST_N_OP_LEFT(operator/);

/** Power.
 * Raise a tuple to a power.
 * TODO: scalar promotion.
 */
template<int N, typename T>
ShGeneric<N, T> pow(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right)
{
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues());
    assert(right.hasValues());
    T lvals[N];
    left.getValues(lvals);
    T rvals[N];
    right.getValues(rvals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = std::pow(lvals[i], rvals[i]);
    return ShAttrib<N, SH_CONST, T>(result);
  } else {
    ShAttrib<N, SH_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_POW, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

SH_SHLIB_CONST_SCALAR_OP(pow);
SH_SHLIB_CONST_N_OP_RIGHT(pow);

/** Power operator.
 * TODO: Not so sure this a good idea.   Might be better to use this opeator for
 * cross product, would be more consistent with usual C usage (or rather,
 * would not conflict so much with it!)
 */
template<int N, typename T>
ShGeneric<N, T> operator^(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right) {
  return pow(left, right);
}

SH_SHLIB_CONST_SCALAR_OP(operator^);
SH_SHLIB_CONST_N_OP_RIGHT(operator^);

/** Multiply and add.
 * This is an intrinsic to access the assembly instruction of the same name.
 * Multiply-add is potentially cheaper than a separate multiply and
 * add.  Note: potentially.
 */
template<int N, int M, int P, typename T>
ShGeneric<P, T> mad(const ShGeneric<M, T>& m1, const ShGeneric<N, T>& m2, 
    const ShGeneric<P,T>& a)
{
  SH_STATIC_CHECK((M == N && N == P) || (M == 1 && N == P) || (N == 1 && M == P),
                  Multiply_And_Add_Components_Do_Not_Match);
  if (!ShEnvironment::insideShader) {
    assert(m1.hasValues()); 
    assert(m2.hasValues());
    assert(a.hasValues());
    //TODO better error message.  can this method be cleaner?
    T m1vals[M], m2vals[N], avals[P];
    m1.getValues(m1vals);
    m2.getValues(m2vals);
    a.getValues(avals);
    T result[P];
    for (int i = 0; i < P; i++) {
        result[i] = ( M == 1 ? m1vals[0] : m1vals[i] ) * ( N == 1 ? m2vals[0] : m2vals[i] ) + avals[i]; 
    }
    return ShAttrib<P, SH_CONST, T>(result);
  } else {
    ShAttrib<P, SH_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_MAD, m1, m2, a); 
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

// Use template specialization for the N, 1, N case

template<int N, typename T> 
ShGeneric<N, T> mad(T m1, const ShGeneric<N, T>& m2, const ShGeneric<N, T>& a)
{ 
  return mad(ShAttrib<1, SH_CONST, T>(m1), m2, a); 
}

template<int N, typename T> 
ShGeneric<N, T> mad(double m1, const ShGeneric<N, T>& m2, const ShGeneric<N, T>& a)
{ 
  return mad(ShAttrib<1, SH_CONST, T>(m1), m2, a); 
}

template<int N, typename T> 
ShGeneric<N, T> mad(const ShGeneric<N, T>& m1, T m2, const ShGeneric<N, T>& a)
{ 
  return mad(m1, ShAttrib<1, SH_CONST, T>(m2), a); 
}

template<int N, typename T> 
ShGeneric<N, T> mad(const ShGeneric<N, T>& m1, double m2, const ShGeneric<N, T>& a)
{ 
  return mad(m1, ShAttrib<1, SH_CONST, T>(m2), a); 
}

/* Square root.
 * The square root of each component of the input is evaluated.
 * TODO: should add reciprocal square root (rsqrt) as an intrinsic.
 */
template<int N, typename T>
ShGeneric<N, T> sqrt(const ShGeneric<N, T>& var)
{
  if (!ShEnvironment::insideShader) {
    assert(var.hasValues());
    T vals[N];
    var.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = std::sqrt(vals[i]);
    return ShAttrib<N, SH_CONST, T>(result);
  } else {
    ShAttrib<N, SH_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_SQRT, var);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

///@}

/** Conditionals.
 * TODO: where are && and || defined?   And how can we add documentation
 * for all the operators defined using macros?
 */
///@{

#define SH_SHLIB_BOOLEAN_OP(opfunc, op, shop) \
template<int N, int M, typename T>\
ShGeneric<N, T> opfunc(const ShGeneric<N, T>& left, const ShGeneric<M, T>& right)\
{\
  SH_STATIC_CHECK(N == M || N == 1 || M == 1, Boolean_Operation_Components_Do_Not_match); \
  if (!ShEnvironment::insideShader) {\
    assert(left.hasValues());\
    assert(right.hasValues());\
    T lvals[N];\
    left.getValues(lvals);\
    T rvals[M];\
    right.getValues(rvals);\
    T result[N];\
    for (int i = 0; i < N; i++) result[i] = (lvals[i] op rvals[(M == 1) ? 0 : i] ? 1.0 : 0.0);\
    return ShAttrib<N, SH_CONST, T>(result);\
  } else {\
    ShAttrib<N, SH_TEMP, T, false> t;\
    ShStatement stmt(t, left, shop, right);\
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);\
    return t;\
  }\
}\
\
template<int M, typename T>\
ShGeneric<M, T> opfunc(const ShGeneric<1, T>& left, const ShGeneric<M, T>& right)\
{\
  if (!ShEnvironment::insideShader) {\
    assert(left.hasValues());\
    assert(right.hasValues());\
    T lvals[1];\
    left.getValues(lvals);\
    T rvals[M];\
    right.getValues(rvals);\
    T result[M];\
    for (int i = 0; i < M; i++) result[i] = (lvals[0] op rvals[i] ? 1.0 : 0.0);\
    return ShAttrib<M, SH_CONST, T>(result);\
  } else {\
    ShAttrib<M, SH_TEMP, T, false> t;\
    ShStatement stmt(t, left, shop, right);\
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);\
    return t;\
  }\
}\
SH_SHLIB_CONST_SCALAR_OP(opfunc);\
SH_SHLIB_CONST_N_OP_BOTH(opfunc);

SH_SHLIB_BOOLEAN_OP(operator<, <, SH_OP_SLT); 
SH_SHLIB_BOOLEAN_OP(operator<=, <=, SH_OP_SLE); 
SH_SHLIB_BOOLEAN_OP(operator>, >, SH_OP_SGT); 
SH_SHLIB_BOOLEAN_OP(operator>=, >=, SH_OP_SGE); 
SH_SHLIB_BOOLEAN_OP(operator==, ==, SH_OP_SEQ); 
SH_SHLIB_BOOLEAN_OP(operator!=, !=, SH_OP_SNE); 

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
template<int M, int N, typename T>
ShGeneric<N, T> cond(const ShGeneric<M, T>& condition, const ShGeneric<N, T>& left, const ShGeneric<N, T>& right)
{
  SH_STATIC_CHECK(N == M || M == 1, Conditional_Assignment_Components_Do_Not_Match);
  if (!ShEnvironment::insideShader) {
    assert(condition.hasValues());
    assert(left.hasValues());
    assert(right.hasValues());
    T cvals[M];
    condition.getValues(cvals);
    T lvals[N];
    left.getValues(lvals);
    T rvals[N];
    right.getValues(rvals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = (cvals[M == 1 ? 0 : i] > 0.0 ? lvals[i] : rvals[i]);
    return ShAttrib<N, SH_CONST, T>(result);
  } else {
    ShAttrib<N, SH_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_COND, condition, left, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

///@}

/** Casts.
 * Some special versions of type-cast operators are defined to be able to
 * deal with tuple size changes in various useful ways.
 */
///@{

/** Casting.
 * Casts ShGeneric<N, T> to ShGeneric<M, T>.
 * If M > N, pads remaining components with 0s (on right).
 * Otherwise, discards extra components.
 */
template<int M, int N, typename T> 
ShGeneric<M, T> cast( const ShGeneric<N, T> &a ) {
  int copySize = std::min(M, N);
  ShAttrib<M, SH_TEMP, T, false> result;

  int indices[copySize];
  for(int i = 0; i < copySize; ++i) indices[i] = i;
  if(M < N) {
    result = a.template swiz<M>(indices);
  } else if( M > N ) {
    result.template swiz<N>(indices) = a;
  } else { // M == N
    ShStatement stmt(result, SH_OP_ASN, a);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
  }
  return result;
}

/** Fill Casting.
 * Casts ShGeneric<N, T> to ShGeneric<M, T>.
 * If M > N, copies last component to fill extra slots.
 * Otherwise, discards extra components.
 */
template<int M, int N, typename T> 
ShGeneric<M, T> fillcast( const ShGeneric<N, T> &a ) {
  if( M <= N ) return cast<M>(a);
  int indices[M];
  for(int i = 0; i < M; ++i) indices[i] = i >= N ? N - 1 : i;
  return a.template swiz<M>(indices);
}

/** Fragment killing.
 * TODO: This should probably be a "control construct", and should
 * be called "SH_DISCARD(cond)".   
 */
template<int N, typename T>
void kill(const ShGeneric<N, T>& c)
{
  assert(ShEnvironment::insideShader);
  ShStatement stmt(c, SH_OP_KIL, c);
  ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
}

///@}

/** Clamping.
 * Various operations that clamp values to specific ranges or
 * values or that manipulate the sign.
 */ ///@{

/** Absolute value.
 * Returns the magnitude.
 * Operates componentwise on tuples.
 */
template<int N, typename T>
ShGeneric<N, T> abs(const ShGeneric<N, T>& var)
{
  if (!ShEnvironment::insideShader) {
    assert(var.hasValues());
    T vals[N];
    var.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = (vals[i] >= 0.0 ? vals[i] : -vals[i]);
    return ShAttrib<N, SH_CONST, T>(result);
  } else {
    ShAttrib<N, SH_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_ABS, var);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    
    return t;
  }
}

/** Ceiling.
 * Returns the least integer <= argument. 
 * Operates componentwise on tuples.
 */
template<int N, typename T>
ShGeneric<N, T> ceil(const ShGeneric<N, T>& var)
{
  if (!ShEnvironment::insideShader) {
    assert(var.hasValues());
    T vals[N];
    var.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = std::ceil(vals[i]);
    return ShAttrib<N, SH_CONST, T>(result);
  } else {
    ShAttrib<N, SH_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_CEIL, var);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

/** Floor.
 * Returns the smallest integer >= argument.
 * Operates componentwise on tuples.
 */
template<int N, typename T>
ShGeneric<N, T> floor(const ShGeneric<N, T>& var)
{
  if (!ShEnvironment::insideShader) {
    assert(var.hasValues());
    T vals[N];
    var.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = std::floor(vals[i]);
    return ShAttrib<N, SH_CONST, T>(result);
  } else {
    ShAttrib<N, SH_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_FLR, var);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

/** Float modulus. 
 * Given a value f, returns f - floor(f).   Note that this is the
 * same as frac (the fractional part) for positive values, but is
 * equal to 1-frac for negative values.   The result is always 
 * postive.
 */
template<int N, int M, typename T>
ShGeneric<N,  T> fmod(const ShGeneric<N, T>& left, const ShGeneric<M, T>& right)
{
  SH_STATIC_CHECK(N == M || M == 1, Fmod_Components_Do_Not_Match);
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues()); 
    assert(right.hasValues());
    T lvals[N];
    left.getValues(lvals);
    T rvals[N];
    right.getValues(rvals);
    T result[N]; 
    for (int i = 0; i < N; i++) result[i] = 
      std::fmod( lvals[i], rvals[(M == 1 ? 0 : i)] ); 
    return ShAttrib<N, SH_CONST, T>(result);
  } else {
    ShAttrib<N, SH_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_FMOD, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

SH_SHLIB_CONST_SCALAR_OP(fmod);
SH_SHLIB_CONST_N_OP_LEFT(fmod);

/** Fractional part.
 * Discards the integer part.
 * TODO: is this equivalent to fmod or not?   How does this behave on negative 
 * numbers?
 */
template<int N, typename T>
ShGeneric<N, T> frac(const ShGeneric<N, T>& var)
{
  if (!ShEnvironment::insideShader) {
    assert(var.hasValues());
    T vals[N];
    var.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = fmodf(vals[i], 1.0f); // TODO: Check that this is correct
    return ShAttrib<N, SH_CONST, T>(result);
  } else {
    ShAttrib<N, SH_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_FRAC, var);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

/** Take positive part. 
 * Clamps a value to zero if it is negative.   
 * This is useful to wrap dot products in lighting models.
 */
template<int N, typename T>
ShGeneric<N,  T> pos(const ShGeneric<N, T>& x) 
{
  if (!ShEnvironment::insideShader) {
    assert(x.hasValues());
    T vals[N];
    x.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = vals[i] > 0 ? vals[i] : 0; 
    return ShAttrib<N, SH_CONST, T>(result);
  } else {
    T vals[N];
    for( int i = 0; i < N; ++i ) vals[i] = 0;
    ShAttrib<N, SH_CONST, T> zero(vals);
    ShAttrib<N, SH_TEMP, T, false> t;
    ShStatement stmt(t, x, SH_OP_MAX, zero);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

/** Maximum.
 * Creates a tuple of componentwise maximums of a pair of input tuples.
 * TODO: would be nice to be able to apply this to a single tuple.
 */
template<int N, typename T>
ShGeneric<N,  T> max(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right)
{
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues());
    T lvals[N];
    left.getValues(lvals);
    T rvals[N];
    right.getValues(rvals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = (lvals[i] > rvals[i] ? lvals[i] : rvals[i]);
    return ShAttrib<N, SH_CONST, T>(result);
  } else {
    ShAttrib<N, SH_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_MAX, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}
SH_SHLIB_CONST_SCALAR_OP(max);

/** Minimum.
 * Creates a tuple of componentwise minimums of a pair of input tuples.
 */
template<int N, typename T>
ShGeneric<N,  T> min(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right)
{
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues());
    T lvals[N];
    left.getValues(lvals);
    T rvals[N];
    right.getValues(rvals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = (lvals[i] < rvals[i] ? lvals[i] : rvals[i]);
    return ShAttrib<N, SH_CONST, T>(result);
  } else {
    ShAttrib<N, SH_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_MIN, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

SH_SHLIB_CONST_SCALAR_OP(min);

///@}

/** Trigonometry.
 * TODO: tan, atan, atan2, hyperbolic functions, secant, cosecant, etc.
 */ ///@{

/** Arccosine. 
 * Operates componentwise on tuples.
 * A value of x in [-1, 1] gives a result in [0, pi].
 * Input values outside the range [-1,1] will give undefined results.
 */
template<int N, typename T>
ShGeneric<N, T> acos(const ShGeneric<N, T>& var)
{
  if (!ShEnvironment::insideShader) {
    assert(var.hasValues());
    T vals[N];
    var.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = std::acos(vals[i]);
    return ShAttrib<N, SH_CONST, T>(result);
  } else {
    ShAttrib<N, SH_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_ACOS, var);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    
    return t;
  }
}

/** Arcsine. 
 * Operates componentwise on tuples.
 * A value of x in [-1, 1] gives a result in [-pi/2, pi/2].
 * Input values outside the range [-1,1] will give undefined results.
 */
template<int N, typename T>
ShGeneric<N, T> asin(const ShGeneric<N, T>& var)
{
  if (!ShEnvironment::insideShader) {
    assert(var.hasValues());
    T vals[N];
    var.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = std::asin(vals[i]);
    return ShAttrib<N, SH_CONST, T>(result);
  } else {
    ShAttrib<N, SH_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_ASIN, var);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    
    return t;
  }
}

/** Cosine.
 * Operates componentwise on tuples.
 * Returns the cosine of x.   Any value of x gives a result
 * in the range [-1,1].
 */
template<int N, typename T>
ShGeneric<N, T> cos(const ShGeneric<N, T>& var)
{
  if (!ShEnvironment::insideShader) {
    assert(var.hasValues());
    T vals[N];
    var.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = std::cos(vals[i]);
    return ShAttrib<N, SH_CONST, T>(result);
  } else {
    ShAttrib<N, SH_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_COS, var);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    
    return t;
  }
}

/** Sine.
 * Operates componentwise on tuples.
 * Returns the sine of x.   Any value of x gives a result
 * in the range [-1,1].
 */
template<int N, typename T>
ShGeneric<N, T> sin(const ShGeneric<N, T>& var)
{
  if (!ShEnvironment::insideShader) {
    assert(var.hasValues());
    T vals[N];
    var.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = std::sin(vals[i]);
    return ShAttrib<N, SH_CONST, T>(result);
  } else {
    ShAttrib<N, SH_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_SIN, var);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    
    return t;
  }
}

///@}

/** Interpolation and Approximation.
 */ ///@{

/** Linear interpolation.
 * Blend between two tuples.   The blend value can be a scalar
 * or a tuple.
 */
template<int N, int M, typename T>
ShGeneric<N, T> lerp(const ShGeneric<M, T>& f, const ShGeneric<N, T>& a, 
    const ShGeneric<N,T>& b)
{
  SH_STATIC_CHECK(M == N || M == 1, Linear_Interpolation_Components_Do_Not_Match);
  if (!ShEnvironment::insideShader) {
    assert(a.hasValues()); 
    assert(b.hasValues());
    assert(f.hasValues());
    T avals[N], bvals[N], fvals[M];
    a.getValues(avals);
    b.getValues(bvals);
    f.getValues(fvals);
    T result[N];
    for (int i = 0; i < N; i++) {
        T fi = ( M == 1 ? fvals[0] : fvals[i] ); 
        result[i] = fi * avals[i] + (1 - fi) * bvals[i]; 
    }
    return ShAttrib<N, SH_CONST, T>(result);
  } else {
    ShAttrib<N, SH_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_LRP, f, a, b); 
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

template<int N, typename T> 
ShGeneric<N, T> lerp(T f, const ShGeneric<N, T>& a, const ShGeneric<N, T>& b)
{ 
  return lerp(ShAttrib<1, SH_CONST, T>(f), a, b); 
}

///@}

/** Geometric operations.
 * TODO: distance, sqdistance, length, rlength, rsqlength, sqlength, perp,
 * perpdot, projnorm should be implemented too.   Different distance functions
 * also possible: L1, L2, Linf.
 */
///@{

/** Take the cross product of two 3-tuples.
 * Note that this operation is not limited to vectors (although it
 * probably should be).
 * TODO: should really be restricted to vectors and normals.
 */
template<typename T>
ShGeneric<3, T> cross(const ShGeneric<3, T>& left, const ShGeneric<3, T>& right)
{
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues());
    assert(right.hasValues());
    T lvals[3];
    left.getValues(lvals);
    T rvals[3];
    right.getValues(rvals);
    T result[3];
    result[0] = lvals[1] * rvals[2] - lvals[2] * rvals[1];
    result[1] = -(lvals[0] * rvals[2] - lvals[2] * rvals[0]);
    result[2] = lvals[0] * rvals[1] - lvals[1] * rvals[0];
    return ShAttrib<3, SH_CONST, T>(result);
  } else {
    ShAttrib<3, SH_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_XPD, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

/** Normalize an n-tuple to unit length.
 * Divides an n-tuple by its Euclidean length.   
 * TODO: should really be restricted to vectors and normals.
 * Will operate incorrectly on homogeneous points, for instance.
 */
template<int N, typename T>
ShGeneric<N, T> normalize(const ShGeneric<N, T>& var)
{
  if (!ShEnvironment::insideShader) {
    ShAttrib1f sizeVar = sqrt(dot(var, var));
    T size;
    sizeVar.getValues(&size);
    assert(var.hasValues());
    T vals[N];
    var.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = vals[i]/size;
    return ShAttrib<N, SH_CONST, T>(result);
  } else {
    ShAttrib<N, SH_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_NORM, var);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

///@}

/** Linear Algebra.
 * Matrix operations and the dot product.   To do (or incorporate):
 * matrix inverse, matrix adjoint, determinant, trace.
 * Also, functions to construct matrices from tuples by column or
 * row would be useful (for small numbers of tuples, and arrays of
 * tuples).   Although... perhaps these should be constructors?
 */ ///@{

/** Inner (dot) product.
 * TODO: why the macro?   Should just use an inline function to define
 * one of these in terms of the other.   Documentation not going to 
 * work this way.   Also, should only work on tuples of the same size...
 * scalar promotion doesn't make any sense here.
 */
#define SH_LIB_DOTPRODUCT(opfunc) \
template<int M, int N, typename T>\
ShGeneric<1,  T> opfunc(const ShGeneric<M, T>& left, const ShGeneric<N, T>& right)\
{\
  SH_STATIC_CHECK(M == N || M == 1 || N == 1, Dot_Product_Components_Do_Not_Match);\
  if (!ShEnvironment::insideShader) {\
    assert(left.hasValues());\
    T lvals[M];\
    left.getValues(lvals);\
    T rvals[N];\
    right.getValues(rvals);\
    T result = 0.0;\
    for (int i = 0; i < M || i < N; i++) result += \
        ( M == 1 ? lvals[0] : lvals[i] ) * ( N == 1 ? rvals[0] : rvals[i] );\
    return ShAttrib<1, SH_CONST, T>(result);\
  } else {\
    ShAttrib<1, SH_TEMP, T, false> t;\
    ShStatement stmt(t, left, SH_OP_DOT, right);\
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);\
    return t;\
  }\
}

SH_LIB_DOTPRODUCT(dot);
SH_LIB_DOTPRODUCT(operator|);

SH_SHLIB_CONST_N_OP_RETSIZE_BOTH(dot, 1);

/** Transpose of a matrix.
 * TODO: this implementation is not very smart.   Really, the compiler should
 * keep track of transpose state and swizzle references as needed, so that
 * a copy of the matrix does not need to be formed.   But this needs to be
 * overridden if the transpose of a matrix is taken and then modified
 * (which, however, does not happen very often).
 */
template<int Rows, int Cols, ShBindingType Binding, typename T>
ShMatrix<Cols, Rows, SH_TEMP, T>
transpose(const ShMatrix<Rows, Cols, Binding, T>& m)
{
  ShMatrix<Cols, Rows, SH_TEMP, T> result;
  for (int i = 0; i < Cols; i++) {
    for (int j = 0; j < Rows; j++) {
      result[i][j] = m[j][i];
    }
  }
  return result;
}

/** Matrix multiplication.
 * Only works on matrices of compatible sizes.
 * TODO: should also define * and mmult to mean the same thing.
 */
template<int M, int N, int P, ShBindingType Binding, ShBindingType Binding2, typename T>
ShMatrix<M, P, SH_TEMP, T>
operator|(const ShMatrix<M, N, Binding, T>& a,
          const ShMatrix<N, P, Binding2, T>& b)
{
  ShMatrix<P, N, SH_TEMP, T> tb = transpose(b);

  ShMatrix<M, P, SH_TEMP, T> result;
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < P; j++) {
      result[i][j] = dot(a[i], tb[j]);
    }
  }
  return result;
}

/** Matrix-tuple multiplication.
 * Treats the tuple as a column vector.
 * TODO: should also define * and mmult to mean the same thing.
 */
template<int M, int N, ShBindingType Binding, typename T>
ShGeneric<M, T> operator|(const ShMatrix<M, N, Binding, T>& a, const ShGeneric<N, T>& b)
{
  ShAttrib<M, SH_TEMP, T> ret;
  for (int i = 0; i < M; i++) {
    ret[i] = dot(a[i], b);
  }
  return ret;
}

///@}

}

#define SH_SHLIB_UNARY_RETTYPE_OPERATION(libtype, libop, librettype, libretsize) \
template<int N, ShBindingType K1, typename T, bool S1> \
librettype<libretsize, SH_TEMP, T, false> \
libop(const libtype<N, K1, T, S1>& var) \
{ \
  ShGeneric<libretsize, T> t = libop(static_cast< ShGeneric<N, T> >(var)); \
  return librettype<libretsize, SH_TEMP, T, false>(t.node(), t.swizzle(), t.neg()); \
}

#define SH_SHLIB_UNARY_OPERATION(libtype, libop, libretsize) \
  SH_SHLIB_UNARY_RETTYPE_OPERATION(libtype, libop, libtype, libretsize)

#define SH_SHLIB_BINARY_RETTYPE_OPERATION(libtype, libop, librettype, libretsize) \
template<int N, ShBindingType K1, ShBindingType K2, typename T, bool S1, bool S2> \
librettype<libretsize, SH_TEMP, T, false> \
libop(const libtype<N, K1, T, S1>& left, const libtype<N, K2, T, S2>& right) \
{ \
  ShGeneric<libretsize, T> t = libop(static_cast< ShGeneric<N, T> >(left), \
                                       static_cast< ShGeneric<N, T> >(right)); \
  return librettype<libretsize, SH_TEMP, T, false>(t.node(), t.swizzle(), t.neg()); \
}

#define SH_SHLIB_BINARY_OPERATION(libtype, libop, libretsize) \
  SH_SHLIB_BINARY_RETTYPE_OPERATION(libtype, libop, libtype, libretsize)

#define SH_SHLIB_UNEQ_BINARY_RETTYPE_OPERATION(libtype, libop, librettype, libretsize) \
template<int N, int M, ShBindingType K1, ShBindingType K2, typename T, bool S1, bool S2> \
librettype<libretsize, SH_TEMP, T, false> \
libop(const libtype<N, K1, T, S1>& left, const libtype<M, K2, T, S2>& right) \
{ \
  ShGeneric<libretsize, T> t = libop(static_cast< ShGeneric<N, T> >(left), \
                                       static_cast< ShGeneric<M, T> >(right)); \
  return librettype<libretsize, SH_TEMP, T, false>(t.node(), t.swizzle(), t.neg()); \
}

#define SH_SHLIB_UNEQ_BINARY_OPERATION(libtype, libop, libretsize) \
  SH_SHLIB_UNEQ_BINARY_RETTYPE_OPERATION(libtype, libop, libtype, libretsize)

#define SH_SHLIB_LEFT_SCALAR_RETTYPE_OPERATION(libtype, libop, librettype) \
template<int M, ShBindingType K1, ShBindingType K2, typename T, bool S1, bool S2> \
librettype<M, SH_TEMP, T, false> \
libop(const libtype<1, K2, T, S2>& left, const libtype<M, K1, T, S1>& right) \
{ \
  ShGeneric<M, T> t = libop(static_cast< ShGeneric<1, T> >(left), \
                              static_cast< ShGeneric<M, T> >(right)); \
  return librettype<M, SH_TEMP, T, false>(t.node(), t.swizzle(), t.neg()); \
}

#define SH_SHLIB_LEFT_SCALAR_OPERATION(libtype, libop) \
  SH_SHLIB_LEFT_SCALAR_RETTYPE_OPERATION(libtype, libop, libtype)

#define SH_SHLIB_LEFT_MATRIX_RETTYPE_OPERATION(libtype, libop, librettype, libretsize) \
template<int M, int N, ShBindingType K1, ShBindingType K2, typename T, bool S1> \
librettype<libretsize, SH_TEMP, T, false> libop(const ShMatrix<M, N, K1, T>& a, \
                                                    const libtype<N, K2, T, S1>& b) \
{ \
  ShGeneric<libretsize, T> t = libop(a, \
                                       static_cast< ShGeneric<N, T> >(b)); \
  return librettype<libretsize, K1, T, S1>(t.node(), t.swizzle(), t.neg()); \
}

#define SH_SHLIB_LEFT_MATRIX_OPERATION(libtype, libop, libretsize) \
  SH_SHLIB_LEFT_MATRIX_RETTYPE_OPERATION(libtype, libop, libtype, libretsize)

// All the scalar constant stuff

#define SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(libtype, libop, librettype, libretsize) \
template<ShBindingType K1, typename T, bool S1> \
librettype<libretsize, SH_TEMP, T, false> \
libop(const libtype<1, K1, T, S1>& left, T right) \
{ \
  return libop(left, ShAttrib<1, SH_CONST, T>(right)); \
} \
template<ShBindingType K1, typename T, bool S1> \
librettype<libretsize, SH_TEMP, T, false> \
libop(T left, const libtype<1, K1, T, S1>& right) \
{ \
  return libop(ShAttrib<1, SH_CONST, T>(left), right); \
} \
template<ShBindingType K1, typename T, bool S1> \
librettype<libretsize, SH_TEMP, T, false> \
libop(const libtype<1, K1, T, S1>& left, double right) \
{ \
  return libop(left, ShAttrib<1, SH_CONST, T>(right)); \
} \
template<ShBindingType K1, typename T, bool S1> \
librettype<libretsize, SH_TEMP, T, false> \
libop(double left, const libtype<1, K1, T, S1>& right) \
{ \
  return libop(ShAttrib<1, SH_CONST, T>(left), right); \
}

#define SH_SHLIB_SPECIAL_CONST_SCALAR_OP(libtype, libop) \
  SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(libtype, libop, libtype, 1)

#define SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_LEFT(libtype, libop, librettype, libretsize) \
template<int N, ShBindingType K1, typename T, bool S1> \
librettype<libretsize, SH_TEMP, T, false> \
libop(const libtype<N, K1, T, S1>& left, T right) \
{ \
  return libop(left, ShAttrib<1, SH_CONST, T>(right)); \
} \
template<int N, ShBindingType K1, typename T, bool S1> \
librettype<libretsize, SH_TEMP, T, false> \
libop(const libtype<N, K1, T, S1>& left, double right) \
{ \
  return libop(left, ShAttrib<1, SH_CONST, T>(right)); \
}
#define SH_SHLIB_SPECIAL_CONST_N_OP_LEFT(libtype, libop) \
  SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_LEFT(libtype, libop, libtype, N)

#define SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_RIGHT(libtype, libop, librettype, libretsize) \
template<int N, ShBindingType K1, typename T, bool S1> \
librettype<libretsize, SH_TEMP, T, false> \
libop(T left, const libtype<N, K1, T, S1>& right) \
{ \
  return libop(ShAttrib<1, SH_CONST, T>(left), right); \
} \
template<int N, ShBindingType K1, typename T, bool S1> \
librettype<libretsize, SH_TEMP, T, false> \
libop(double left, const libtype<N, K1, T, S1>& right) \
{ \
  return libop(ShAttrib<1, SH_CONST, T>(left), right); \
}
#define SH_SHLIB_SPECIAL_CONST_N_OP_RIGHT(libtype, libop) \
  SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_RIGHT(libtype, libop, libtype, N)

#define SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_BOTH(libtype, operation, librettype, libretsize) \
SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_LEFT(libtype, operation, librettype, libretsize); \
SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_RIGHT(libtype, operation, librettype, libretsize);

#define SH_SHLIB_SPECIAL_CONST_N_OP_BOTH(libtype, operation) \
SH_SHLIB_SPECIAL_CONST_N_OP_LEFT(libtype, operation); \
SH_SHLIB_SPECIAL_CONST_N_OP_RIGHT(libtype, operation);

// Standard stuff

#define SH_SHLIB_USUAL_OPERATIONS(type) \
  SH_SHLIB_USUAL_OPERATIONS_RETTYPE(type, type)

#define SH_SHLIB_USUAL_OPERATIONS_RETTYPE(type, rettype) \
  SH_SHLIB_USUAL_NON_UNIT_OPS_RETTYPE(type, rettype)\
  SH_SHLIB_UNARY_RETTYPE_OPERATION(type, normalize, rettype, N);       \
  SH_SHLIB_UNARY_RETTYPE_OPERATION(type, abs, rettype, N);           

#define SH_SHLIB_USUAL_NON_UNIT_OPS_RETTYPE(type, rettype) \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, operator+, rettype, N);      \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator+, rettype, 1);  \
                                                        \
SH_SHLIB_UNEQ_BINARY_RETTYPE_OPERATION(type, operator*, rettype, N); \
SH_SHLIB_LEFT_SCALAR_RETTYPE_OPERATION(type, operator*, rettype);    \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator*, rettype, 1);  \
SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_BOTH(type, operator*, rettype, N);  \
                                                        \
SH_SHLIB_UNEQ_BINARY_RETTYPE_OPERATION(type, operator/, rettype, N); \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator/, rettype, 1);  \
SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_LEFT(type, operator/, rettype, N);  \
                                                        \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, pow, rettype, N);            \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, pow, rettype, 1);        \
SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_RIGHT(type, pow, rettype, N);       \
                                                        \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, operator^, rettype, N);      \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator^, rettype, 1);  \
SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_RIGHT(type, operator^, rettype, N); \
                                                        \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, operator<, rettype, N);      \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator<, rettype, 1);  \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, operator<=, rettype, N);     \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator<=, rettype, 1); \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, operator>, rettype, N);      \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator>, rettype, 1);  \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, operator>=, rettype, N);     \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator>=, rettype, 1); \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, operator==, rettype, N);     \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator==, rettype, 1); \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, operator!=, rettype, N);     \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator!=, rettype, 1); \
                                                        \
SH_SHLIB_UNARY_RETTYPE_OPERATION(type, acos, rettype, N);            \
SH_SHLIB_UNARY_RETTYPE_OPERATION(type, asin, rettype, N);            \
SH_SHLIB_UNARY_RETTYPE_OPERATION(type, cos, rettype, N);             \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, dot, rettype, 1);           \
SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_BOTH(type, dot, rettype, 1);           \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, fmod, rettype, N);           \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, fmod, rettype, 1);       \
                                                        \
SH_SHLIB_UNARY_RETTYPE_OPERATION(type, frac, rettype, N);            \
SH_SHLIB_UNARY_RETTYPE_OPERATION(type, sin, rettype, N);             \
SH_SHLIB_UNARY_RETTYPE_OPERATION(type, sqrt, rettype, N);            \
                                                        \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, min, rettype, N);            \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, min, rettype, 1);        \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, pos, rettype, N);            \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, pos, rettype, 1);        \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, max, rettype, N);            \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, max, rettype, 1);

// Points have different subtraction, so we don't include them in our "usuals"
#define SH_SHLIB_USUAL_SUBTRACT(type) \
SH_SHLIB_BINARY_OPERATION(type, operator-, N);      \
SH_SHLIB_SPECIAL_CONST_SCALAR_OP(type, operator-);  \

#endif
