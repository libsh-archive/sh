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

/** \file ShLib.hpp
 * \brief The Sh library functions.
 * If you add or remove functions in this file be sure to modify the
 * other ShLib*.hpp (e.g. ShLibVector.hpp) files appropriately. See
 * the bottom of the file for a list.
 */

namespace SH {

/** Utility template to check whether dimensions are equal or scalar.
 * If ScalarLHS or ScalarRHS are true, then the left or right
 * arguments respectively are allowed to be 1.
 *
 * This is kind of ugly, but it saves us having to define several
 *operator*, operator/, etc. later on.
 */
template<int N, bool ScalarLHS, int M, bool ScalarRHS>
class ShCheckDims 
{
private:
  ShCheckDims();
};
template<int N> class ShCheckDims<N, false, N, false>
{public: ShCheckDims() {}};
template<int N> class ShCheckDims<N, false, N, true>
{public: ShCheckDims() {}};
template<int N> class ShCheckDims<N, true, N, false>
{public: ShCheckDims() {}};
template<int N> class ShCheckDims<N, true, N, true>
{public: ShCheckDims() {}};
template<int N, bool ScalarRHS> class ShCheckDims<1, true, N, ScalarRHS>
{public: ShCheckDims() {}};
template<int N, bool ScalarLHS> class ShCheckDims<N, ScalarLHS, 1, true>
{public: ShCheckDims() {}};
template<> class ShCheckDims<1, false, 1, false>
{public: ShCheckDims() {}};
template<> class ShCheckDims<1, false, 1, true>
{public: ShCheckDims() {}};
template<> class ShCheckDims<1, true, 1, false>
{public: ShCheckDims() {}};
template<> class ShCheckDims<1, true, 1, true>
{public: ShCheckDims() {}};

/// Addition
template<int N, typename T>
ShVariableN<N, T> operator+(const ShVariableN<N, T>& left, const ShVariableN<N, T>& right)
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
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_ADD, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

/// Subtraction
template<int N, typename T>
ShVariableN<N, T> operator-(const ShVariableN<N, T>& left, const ShVariableN<N, T>& right)
{
  return left + (-right);
}

/// Componentwise/scalar multiplication
template<int N, int M, typename T>
ShVariableN<N, T> operator*(const ShVariableN<N, T>& left, const ShVariableN<M, T>& right)
{
  ShCheckDims<N, true, M, true>();
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues());
    assert(right.hasValues());
    T lvals[N];
    left.getValues(lvals);
    T rvals[M];
    right.getValues(rvals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = lvals[i] * rvals[M == 1 ? 0 : i];
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_MUL, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);

    return t;
  }
}

template<int M, typename T>
ShVariableN<M, T> operator*(const ShVariableN<1, T>& left, const ShVariableN<M, T>& right)
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
    return ShConstant<M, T>(result);
  } else {
    ShAttrib<M, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_MUL, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);

    return t;
  }
}

/// Componentwise/scalar division
template<int N, int M, typename T>
ShVariableN<N, T> operator/(const ShVariableN<N, T>& left, const ShVariableN<M, T>& right)
{
  ShCheckDims<N, false, M, true>();
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues());
    assert(right.hasValues());
    T lvals[N];
    left.getValues(lvals);
    T rvals[M];
    right.getValues(rvals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = lvals[i] / rvals[M == 1 ? 0 : i];
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_DIV, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    
    return t;
  }
}

/// Conventional power operation.
template<int N, typename T>
ShVariableN<N, T> operator^(const ShVariableN<N, T>& left, const ShVariableN<N, T>& right)
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
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_POW, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

// "Boolean" operators

template<int N, typename T>
ShVariableN<N, T> operator<(const ShVariableN<N, T>& left, const ShVariableN<1, T>& right)
{
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues());
    assert(right.hasValues());
    T lvals[N];
    left.getValues(lvals);
    T rvals[1];
    right.getValues(rvals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = (lvals[i] < rvals[0] ? 1.0 : 0.0);
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_SLT, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

template<int N, typename T>
ShVariableN<N, T> operator<=(const ShVariableN<N, T>& left, const ShVariableN<1, T>& right)
{
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues());
    assert(right.hasValues());
    T lvals[N];
    left.getValues(lvals);
    T rvals[1];
    right.getValues(rvals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = (lvals[i] <= rvals[0] ? 1.0 : 0.0);
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_SLE, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

template<int N, typename T>
ShVariableN<N, T> operator>(const ShVariableN<N, T>& left, const ShVariableN<1, T>& right)
{
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues());
    assert(right.hasValues());
    T lvals[N];
    left.getValues(lvals);
    T rvals[1];
    right.getValues(rvals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = (lvals[i] > rvals[0] ? 1.0 : 0.0);
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_SGT, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

template<int N, typename T>
ShVariableN<N, T> operator>=(const ShVariableN<N, T>& left, const ShVariableN<1, T>& right)
{
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues());
    assert(right.hasValues());
    T lvals[N];
    left.getValues(lvals);
    T rvals[1];
    right.getValues(rvals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = (lvals[i] >= rvals[0] ? 1.0 : 0.0);
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_SGE, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

template<int N, typename T>
ShVariableN<N, T> operator==(const ShVariableN<N, T>& left, const ShVariableN<1, T>& right)
{
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues());
    assert(right.hasValues());
    T lvals[N];
    left.getValues(lvals);
    T rvals[1];
    right.getValues(rvals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = (lvals[i] == rvals[0] ? 1.0 : 0.0);
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_SEQ, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

template<int N, typename T>
ShVariableN<N, T> operator!=(const ShVariableN<N, T>& left, const ShVariableN<1, T>& right)
{
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues());
    assert(right.hasValues());
    T lvals[N];
    left.getValues(lvals);
    T rvals[1];
    right.getValues(rvals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = (lvals[i] != rvals[0] ? 1.0 : 0.0);
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_SNE, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

/** Conditional assignment (COND/CMP)
 *  dest[i] = (src1[i] > 0.0 ? src2[i] : src3[i])
 *  Note: CMP in the ARB_{vertex,fragment}_program spec has
 *  src1[i] < 0.0, not greater than.
 */
template<int N, typename T>
ShVariableN<N, T> cond(const ShVariableN<N, T>& condition, const ShVariableN<N, T>& left, const ShVariableN<N, T>& right)
{
  if (!ShEnvironment::insideShader) {
    assert(condition.hasValues());
    assert(left.hasValues());
    assert(right.hasValues());
    T cvals[N];
    condition.getValues(cvals);
    T lvals[N];
    left.getValues(lvals);
    T rvals[N];
    right.getValues(rvals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = (cvals[i] > 0.0 ? lvals[i] : rvals[i]);
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_COND, condition, left, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

// Fragment killing
template<int N, typename T>
void kill(const ShVariableN<N, T>& c)
{
  assert(ShEnvironment::insideShader);
  ShStatement stmt(SH_OP_KIL, c);
  ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
}

//--- Mathematics Library

/// Absolute value
template<int N, typename T>
ShVariableN<N, T> abs(const ShVariableN<N, T>& var)
{
  if (!ShEnvironment::insideShader) {
    assert(var.hasValues());
    T vals[N];
    var.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = (vals[i] >= 0.0 ? vals[i] : -vals[i]);
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_ABS, var);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    
    return t;
  }
}

/// Arccosine of x. x in [-1, 1], result in [-pi/2, pi/2]
template<int N, typename T>
ShVariableN<N, T> acos(const ShVariableN<N, T>& var)
{
  if (!ShEnvironment::insideShader) {
    assert(var.hasValues());
    T vals[N];
    var.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = std::acos(vals[i]) - M_PI/2;
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_ACOS, var);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    
    return t;
  }
}

/// Arcsine of x. x in [-1, 1]. Result in [0, pi]
template<int N, typename T>
ShVariableN<N, T> asin(const ShVariableN<N, T>& var)
{
  if (!ShEnvironment::insideShader) {
    assert(var.hasValues());
    T vals[N];
    var.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = std::asin(vals[i]) + M_PI/2;
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_ASIN, var);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    
    return t;
  }
}

/// Cosine of x.
template<int N, typename T>
ShVariableN<N, T> cos(const ShVariableN<N, T>& var)
{
  if (!ShEnvironment::insideShader) {
    assert(var.hasValues());
    T vals[N];
    var.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = std::cos(vals[i]);
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_COS, var);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    
    return t;
  }
}


// Dot product
template<int N, typename T>
ShVariableN<1,  T> dot(const ShVariableN<N, T>& left, const ShVariableN<N, T>& right)
{
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues());
    T lvals[N];
    left.getValues(lvals);
    T rvals[N];
    right.getValues(rvals);
    T result = 0.0;
    for (int i = 0; i < N; i++) result += lvals[i] * rvals[i];
    return ShConstant<1, T>(result);
  } else {
    ShAttrib<1, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_DOT, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

/// Fractional part.
template<int N, typename T>
ShVariableN<N, T> frac(const ShVariableN<N, T>& var)
{
  if (!ShEnvironment::insideShader) {
    assert(var.hasValues());
    T vals[N];
    var.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = fmod(vals[i], 1.0); // TODO: Check that this is correct
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_FRAC, var);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

/// Componentwise maximum
template<int N, typename T>
ShVariableN<N,  T> max(const ShVariableN<N, T>& left, const ShVariableN<N, T>& right)
{
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues());
    T lvals[N];
    left.getValues(lvals);
    T rvals[N];
    right.getValues(rvals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = (lvals[i] > rvals[i] ? lvals[i] : rvals[i]);
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_MAX, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

/// Componentwise minimum
template<int N, typename T>
ShVariableN<N,  T> min(const ShVariableN<N, T>& left, const ShVariableN<N, T>& right)
{
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues());
    T lvals[N];
    left.getValues(lvals);
    T rvals[N];
    right.getValues(rvals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = (lvals[i] < rvals[i] ? lvals[i] : rvals[i]);
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_MIN, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}


/// Sine of x.
template<int N, typename T>
ShVariableN<N, T> sin(const ShVariableN<N, T>& var)
{
  if (!ShEnvironment::insideShader) {
    assert(var.hasValues());
    T vals[N];
    var.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = std::sin(vals[i]);
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_SIN, var);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    
    return t;
  }
}

// Square root.
template<int N, typename T>
ShVariableN<N, T> sqrt(const ShVariableN<N, T>& var)
{
  if (!ShEnvironment::insideShader) {
    assert(var.hasValues());
    T vals[N];
    var.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = std::sqrt(vals[i]);
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_SQRT, var);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

// Geometrical operations

template<int N, typename T>
ShVariableN<N, T> normalize(const ShVariableN<N, T>& var)
{
  if (!ShEnvironment::insideShader) {
    ShAttrib1f sizeVar = sqrt(dot(var, var));
    double size;
    sizeVar.getValues(&size);
    assert(var.hasValues());
    T vals[N];
    var.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = vals[i]/size;
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_NORM, var);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

// Operations on matrices

/// Transpose of a matrix
template<int Rows, int Cols, int Kind, typename T>
ShMatrix<Cols, Rows, SH_VAR_TEMP, T>
transpose(const ShMatrix<Rows, Cols, Kind, T>& m)
{
  ShMatrix<Cols, Rows, SH_VAR_TEMP, T> result;
  for (int i = 0; i < Cols; i++) {
    for (int j = 0; j < Rows; j++) {
      result[i][j] = m[j][i];
    }
  }
  return result;
}

/// Matrix multiplication
template<int M, int N, int P, int Kind, int Kind2, typename T>
ShMatrix<M, P, SH_VAR_TEMP, T>
operator|(const ShMatrix<M, N, Kind, T>& a,
          const ShMatrix<N, P, Kind2, T>& b)
{
  ShMatrix<P, N, SH_VAR_TEMP, T> tb = transpose(b);

  ShMatrix<M, P, SH_VAR_TEMP, T> result;
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < P; j++) {
      result[i][j] = dot(a[i], tb[j]);
    }
  }
  return result;
}

/// Treat a variable as a column vector and multiply it with a matrix
template<int M, int N, int Kind, typename T>
ShVariableN<M, T> operator|(const ShMatrix<M, N, Kind, T>& a, const ShVariableN<N, T>& b)
{
  ShAttrib<M, SH_VAR_TEMP, T> ret;
  for (int i = 0; i < M; i++) {
    ret[i] = dot(a[i], b);
  }
  return ret;
}

}

#define SH_SHLIB_UNARY_RETTYPE_OPERATION(libtype, libop, librettype, libretsize) \
template<int N, int K1, typename T, bool S1> \
librettype<libretsize, SH_VAR_TEMP, T, false> \
libop(const libtype<N, K1, T, S1>& var) \
{ \
  ShVariableN<libretsize, T> t = libop(static_cast< ShVariableN<N, T> >(var)); \
  return librettype<libretsize, SH_VAR_TEMP, T, false>(t.node(), t.swizzle(), t.neg()); \
}

#define SH_SHLIB_UNARY_OPERATION(libtype, libop, libretsize) \
  SH_SHLIB_UNARY_RETTYPE_OPERATION(libtype, libop, libtype, libretsize)

#define SH_SHLIB_BINARY_RETTYPE_OPERATION(libtype, libop, librettype, libretsize) \
template<int N, int K1, int K2, typename T, bool S1, bool S2> \
librettype<libretsize, SH_VAR_TEMP, T, false> \
libop(const libtype<N, K1, T, S1>& left, const libtype<N, K2, T, S2>& right) \
{ \
  ShVariableN<libretsize, T> t = libop(static_cast< ShVariableN<N, T> >(left), \
                                       static_cast< ShVariableN<N, T> >(right)); \
  return librettype<libretsize, SH_VAR_TEMP, T, false>(t.node(), t.swizzle(), t.neg()); \
}

#define SH_SHLIB_BINARY_OPERATION(libtype, libop, libretsize) \
  SH_SHLIB_BINARY_RETTYPE_OPERATION(libtype, libop, libtype, libretsize)

#define SH_SHLIB_UNEQ_BINARY_RETTYPE_OPERATION(libtype, libop, librettype, libretsize) \
template<int N, int M, int K1, int K2, typename T, bool S1, bool S2> \
librettype<libretsize, SH_VAR_TEMP, T, false> \
libop(const libtype<N, K1, T, S1>& left, const libtype<M, K2, T, S2>& right) \
{ \
  ShVariableN<libretsize, T> t = libop(static_cast< ShVariableN<N, T> >(left), \
                                       static_cast< ShVariableN<M, T> >(right)); \
  return librettype<libretsize, SH_VAR_TEMP, T, false>(t.node(), t.swizzle(), t.neg()); \
}

#define SH_SHLIB_UNEQ_BINARY_OPERATION(libtype, libop, libretsize) \
  SH_SHLIB_UNEQ_BINARY_RETTYPE_OPERATION(libtype, libop, libtype, libretsize)

#define SH_SHLIB_LEFT_SCALAR_RETTYPE_OPERATION(libtype, libop, librettype) \
template<int M, int K1, int K2, typename T, bool S1, bool S2> \
librettype<M, SH_VAR_TEMP, T, false> \
libop(const libtype<1, K2, T, S2>& left, const libtype<M, K1, T, S1>& right) \
{ \
  ShVariableN<M, T> t = libop(static_cast< ShVariableN<1, T> >(left), \
                              static_cast< ShVariableN<M, T> >(right)); \
  return librettype<M, SH_VAR_TEMP, T, false>(t.node(), t.swizzle(), t.neg()); \
}

#define SH_SHLIB_LEFT_SCALAR_OPERATION(libtype, libop) \
  SH_SHLIB_LEFT_SCALAR_RETTYPE_OPERATION(libtype, libop, libtype)

#define SH_SHLIB_LEFT_MATRIX_RETTYPE_OPERATION(libtype, libop, librettype, libretsize) \
template<int M, int N, int K1, int K2, typename T, bool S1> \
librettype<libretsize, SH_VAR_TEMP, T, false> libop(const ShMatrix<M, N, K1, T>& a, \
                                                    const libtype<N, K2, T, S1>& b) \
{ \
  ShVariableN<libretsize, T> t = libop(a, \
                                       static_cast< ShVariableN<N, T> >(b)); \
  return librettype<libretsize, K1, T, S1>(t.node(), t.swizzle(), t.neg()); \
}

#define SH_SHLIB_LEFT_MATRIX_OPERATION(libtype, libop, libretsize) \
  SH_SHLIB_LEFT_MATRIX_RETTYPE_OPERATION(libtype, libop, libtype, libretsize)

#include "ShLibAttrib.hpp"
#include "ShLibVector.hpp"
#include "ShLibPoint.hpp"
#include "ShLibColor.hpp"
#include "ShLibTexCoord.hpp"
#include "ShLibNormal.hpp"
#include "ShLibPosition.hpp"

#endif
