#ifndef SHLIBGEOMETRYIMPL_HPP
#define SHLIBGEOMETRYIMPL_HPP

#include "ShLibClamp.hpp"
#include "ShAttrib.hpp"
#include "ShInstructions.hpp"

namespace SH {

template<typename T>
inline
ShGeneric<3, T> cross(const ShGeneric<3, T>& left, const ShGeneric<3, T>& right)
{
  ShAttrib<3, SH_TEMP, T> t;
  shXPD(t, left, right);
  return t;
}

template<typename T>
inline
ShGeneric<3, T> operator^(const ShGeneric<3, T>& left, const ShGeneric<3, T>& right)
{
  return cross(left, right);
}

template<int N, typename T>
inline
ShGeneric<N, T> normalize(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shNORM(t, var);
  return t;
}

template<int N, typename T>
inline
ShGeneric<1,  T> dot(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<1, SH_TEMP, T> t;
  shDOT(t, left, right);
  return t;
}

template<int N, typename T>
inline
ShGeneric<1,  T> operator|(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right)
{
  return dot(left, right);
}

template<int N, typename T>
inline
ShGeneric<N, T> reflect(const ShGeneric<N, T>& a, const ShGeneric<N, T>& b)
{
  ShGeneric<N, T> bn = normalize(b);
  return 2.0 * dot(a, b) * b - a;
}

template<int N, typename T>
inline
ShGeneric<N, T> faceforward(const ShGeneric<N, T>& a, const ShGeneric<N, T>& b)
{
  return (2.0 * (dot(a, b) > 0.0) - 1.0) * b;
}

template<typename T>
inline
ShGeneric<4, T> lit(const ShGeneric<1, T>& a,
                    const ShGeneric<1, T>& b,
                    const ShGeneric<1, T>& c)
{
  ShAttrib<4, SH_TEMP, T> r;
  r(0,3) = ShConstAttrib2f(1.0, 1.0);
  r(1) = pos(a);
  r(2) = (a < 0 && b < 0) * pow(b, c);
  return r;
}

}

#endif
