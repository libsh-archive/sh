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
ShGeneric<N, T> refract(const ShGeneric<N, T>& v, const ShGeneric<N, T>& n,
                        const ShGeneric<1, T>& theta)
{
  ShGeneric<N, T> vn = normalize(v);
  ShGeneric<N, T> nn = normalize(n);
  ShAttrib1f c = (vn|nn);
  ShAttrib1f k = c*c - 1.0f;
  k = 1.0f + theta*theta*k;
  k = clamp(k, 0.0f, 1.0f);
  ShAttrib1f a = theta;
  ShAttrib1f b = theta*c + sqrt(k);
  return (a*vn + b*nn);
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

template<int N, typename T>
ShGeneric<1,  T> distance(const ShGeneric<N, T>& a, const ShGeneric<N, T>& b)
{
  return length(a-b);
}

template<int N, typename T>
ShGeneric<1,  T> distance_1(const ShGeneric<N, T>& a, const ShGeneric<N, T>& b)
{
  return length_1(a-b);
}

template<int N, typename T>
ShGeneric<1,  T> distance_inf(const ShGeneric<N, T>& a, const ShGeneric<N, T>& b)
{
  return length_inf(a-b);
}

template<int N, typename T>
ShGeneric<1,  T> length(const ShGeneric<N, T>& a)
{
  return sqrt(dot(a, a));
}

template<int N, typename T>
ShGeneric<1,  T> length_1(const ShGeneric<N, T>& a, const ShGeneric<N, T>& b)
{
  return sum(abs(a));
}

template<int N, typename T>
ShGeneric<1,  T> length_inf(const ShGeneric<N, T>& a, const ShGeneric<N, T>& b)
{
  return max(abs(a));
}


}

#endif
