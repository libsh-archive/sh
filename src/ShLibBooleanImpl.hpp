#ifndef SHLIBBOOLEANIMPL_HPP
#define SHLIBBOOLEANIMPL_HPP

#include "ShLibBoolean.hpp"
#include "ShInstructions.hpp"
#include "ShAttrib.hpp"

namespace SH {

template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator<(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSLT(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator<(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSLT(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator<(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSLT(t, left, right);
  return t;
}
template<typename T1, typename T2>
ShGeneric<1, CT1T2> operator<(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<1, SH_TEMP, CT1T2> t;
  shSLT(t, left, right);
  return t;
}

template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator<=(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSLE(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator<=(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSLE(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator<=(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSLE(t, left, right);
  return t;
}
template<typename T1, typename T2>
ShGeneric<1, CT1T2> operator<=(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<1, SH_TEMP, CT1T2> t;
  shSLE(t, left, right);
  return t;
}

template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator>(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSGT(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator>(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSGT(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator>(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSGT(t, left, right);
  return t;
}
template<typename T1, typename T2>
ShGeneric<1, CT1T2> operator>(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<1, SH_TEMP, CT1T2> t;
  shSGT(t, left, right);
  return t;
}

template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator>=(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSGE(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator>=(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSGE(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator>=(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSGE(t, left, right);
  return t;
}
template<typename T1, typename T2>
ShGeneric<1, CT1T2> operator>=(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<1, SH_TEMP, CT1T2> t;
  shSGE(t, left, right);
  return t;
}

template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator==(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSEQ(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator==(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSEQ(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator==(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSEQ(t, left, right);
  return t;
}
template<typename T1, typename T2>
ShGeneric<1, CT1T2> operator==(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<1, SH_TEMP, CT1T2> t;
  shSEQ(t, left, right);
  return t;
}

template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator!=(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSNE(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator!=(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSNE(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator!=(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSNE(t, left, right);
  return t;
}
template<typename T1, typename T2>
ShGeneric<1, CT1T2> operator!=(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<1, SH_TEMP, CT1T2> t;
  shSNE(t, left, right);
  return t;
}

template<int N, typename T1, typename T2, typename T3>
ShGeneric<N, CT1T2T3> cond(const ShGeneric<N, T1>& condition, const ShGeneric<N, T2>& left,
                     const ShGeneric<N, T3>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2T3> t;
  shCOND(t, condition, left, right);
  return t;
}
template<int N, typename T1, typename T2, typename T3>
ShGeneric<N, CT1T2T3> cond(const ShGeneric<1, T1>& condition, const ShGeneric<N, T2>& left,
                     const ShGeneric<N, T3>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2T3> t;
  shCOND(t, condition, left, right);
  return t;
}
template<typename T1, typename T2, typename T3>
ShGeneric<1, CT1T2T3> cond(const ShGeneric<1, T1>& condition, const ShGeneric<1, T2>& left,
                     const ShGeneric<1, T3>& right)
{
  ShAttrib<1, SH_TEMP, CT1T2T3> t;
  shCOND(t, condition, left, right);
  return t;
}

// TODO

template<int N, typename T>
ShGeneric<N, T> operator!(const ShGeneric<N, T>& a)
{
  return 1.0f - (a > 0.0f);
}


template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator&&(const ShGeneric<N, T1>& a, const ShGeneric<N, T2>& b)
{
  return min(a,b);
}

template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator||(const ShGeneric<N, T1>& a, const ShGeneric<N, T2>& b)
{
  return max(a,b);
}

template<int N, typename T>
ShGeneric<1, T> any(const ShGeneric<N, T>& a)
{
  ShAttrib<1, SH_TEMP, T> t = a(0);
  for (int i = 1; i < N; i++) {
    t = t || a(i);
  }
  return t;
}

template<int N, typename T>
ShGeneric<1, T> all(const ShGeneric<N, T>& a)
{
  ShAttrib<1, SH_TEMP, T> t = a(0);
  for (int i = 1; i < N; i++) {
    t = t && a(i);
  }
  return t;
}

}

#endif
