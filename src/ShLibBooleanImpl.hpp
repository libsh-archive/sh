#ifndef SHLIBBOOLEANIMPL_HPP
#define SHLIBBOOLEANIMPL_HPP

#include "ShLibBoolean.hpp"
#include "ShInstructions.hpp"
#include "ShAttrib.hpp"

namespace SH {

template<int N, typename T>
ShGeneric<N, T> operator<(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSLT(t, left, right);
  return t;
}
template<int N, typename T>
ShGeneric<N, T> operator<(const ShGeneric<N, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSLT(t, left, right);
  return t;
}
template<int N, typename T>
ShGeneric<N, T> operator<(const ShGeneric<1, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSLT(t, left, right);
  return t;
}
template<typename T>
ShGeneric<1, T> operator<(const ShGeneric<1, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<1, SH_TEMP, T> t;
  shSLT(t, left, right);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> operator<=(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSLE(t, left, right);
  return t;
}
template<int N, typename T>
ShGeneric<N, T> operator<=(const ShGeneric<N, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSLE(t, left, right);
  return t;
}
template<int N, typename T>
ShGeneric<N, T> operator<=(const ShGeneric<1, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSLE(t, left, right);
  return t;
}
template<typename T>
ShGeneric<1, T> operator<=(const ShGeneric<1, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<1, SH_TEMP, T> t;
  shSLE(t, left, right);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> operator>(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSGT(t, left, right);
  return t;
}
template<int N, typename T>
ShGeneric<N, T> operator>(const ShGeneric<N, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSGT(t, left, right);
  return t;
}
template<int N, typename T>
ShGeneric<N, T> operator>(const ShGeneric<1, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSGT(t, left, right);
  return t;
}
template<typename T>
ShGeneric<1, T> operator>(const ShGeneric<1, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<1, SH_TEMP, T> t;
  shSGT(t, left, right);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> operator>=(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSGE(t, left, right);
  return t;
}
template<int N, typename T>
ShGeneric<N, T> operator>=(const ShGeneric<N, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSGE(t, left, right);
  return t;
}
template<int N, typename T>
ShGeneric<N, T> operator>=(const ShGeneric<1, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSGE(t, left, right);
  return t;
}
template<typename T>
ShGeneric<1, T> operator>=(const ShGeneric<1, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<1, SH_TEMP, T> t;
  shSGE(t, left, right);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> operator==(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSEQ(t, left, right);
  return t;
}
template<int N, typename T>
ShGeneric<N, T> operator==(const ShGeneric<N, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSEQ(t, left, right);
  return t;
}
template<int N, typename T>
ShGeneric<N, T> operator==(const ShGeneric<1, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSEQ(t, left, right);
  return t;
}
template<typename T>
ShGeneric<1, T> operator==(const ShGeneric<1, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<1, SH_TEMP, T> t;
  shSEQ(t, left, right);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> operator!=(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSNE(t, left, right);
  return t;
}
template<int N, typename T>
ShGeneric<N, T> operator!=(const ShGeneric<N, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSNE(t, left, right);
  return t;
}
template<int N, typename T>
ShGeneric<N, T> operator!=(const ShGeneric<1, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSNE(t, left, right);
  return t;
}
template<typename T>
ShGeneric<1, T> operator!=(const ShGeneric<1, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<1, SH_TEMP, T> t;
  shSNE(t, left, right);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> cond(const ShGeneric<N, T>& condition, const ShGeneric<N, T>& left,
                     const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shCOND(t, condition, left, right);
  return t;
}
template<int N, typename T>
ShGeneric<N, T> cond(const ShGeneric<1, T>& condition, const ShGeneric<N, T>& left,
                     const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shCOND(t, condition, left, right);
  return t;
}
template<typename T>
ShGeneric<1, T> cond(const ShGeneric<1, T>& condition, const ShGeneric<1, T>& left,
                     const ShGeneric<1, T>& right)
{
  ShAttrib<1, SH_TEMP, T> t;
  shCOND(t, condition, left, right);
  return t;
}

// TODO

template<int N, typename T>
ShGeneric<N, T> operator!(const ShGeneric<N, T>& a)
{
  return 1.0f - (a > 0.0f);
}


template<int N, typename T>
ShGeneric<N, T> operator&&(const ShGeneric<N, T>& a, const ShGeneric<N, T>& b)
{
  return min(a,b);
}

template<int N, typename T>
ShGeneric<N, T> operator||(const ShGeneric<N, T>& a, const ShGeneric<N, T>& b)
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
