#ifndef SHLIB_HPP
#define SHLIB_HPP

#include <cmath>
#include <cassert>
#include "ShVariable.hpp"
#include "ShAttrib.hpp"

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

//--- Library calls "ported" from Cg

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


}


#endif
