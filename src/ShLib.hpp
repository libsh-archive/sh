#ifndef SHLIB_HPP
#define SHLIB_HPP

namespace SH {

#include "ShAttrib.hpp"

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
ShAttrib<N, SH_VAR_TEMP, T, false> operator+(const ShVariableN<N, T>& left, const ShVariableN<N, T>& right)
{
  ShAttrib<N, SH_VAR_TEMP, T, false> t;
  ShStatement stmt(t, left, SH_OP_ADD, right);
  ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
  
  return t;
}

/// Subtraction
template<int N, typename T>
ShAttrib<N, SH_VAR_TEMP, T, false> operator-(const ShVariableN<N, T>& left, const ShVariableN<N, T>& right)
{
  return left + (-right);
}

/// Componentwise/scalar multiplication
template<int N, int M, typename T>
ShAttrib<N, SH_VAR_TEMP, T, false> operator*(const ShVariableN<N, T>& left, const ShVariableN<M, T>& right)
{
  ShCheckDims<N, true, M, true>();
  ShAttrib<N, SH_VAR_TEMP, T, false> t;
  ShStatement stmt(t, left, SH_OP_MUL, right);
  ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);

  return t;
}

/// Componentwise/scalar division
template<int N, int M, typename T>
ShAttrib<N, SH_VAR_TEMP, T, false> operator/(const ShVariableN<N, T>& left, const ShVariableN<M, T>& right)
{
  ShCheckDims<N, false, M, true>();
  ShAttrib<N, SH_VAR_TEMP, T, false> t;
  ShStatement stmt(t, left, SH_OP_DIV, right);
  ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);

  return t;
}

}


#endif
