#ifndef SHLIBARITHIMPL_HPP
#define SHLIBARITHIMPL_HPP

#include "ShLibArith.hpp"
#include "ShInstructions.hpp"

namespace SH {

template<int N, typename T>
inline
ShGeneric<N, T> operator+(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shADD(t, left, right);
  return t;
}

template<int N, typename T>
inline
ShGeneric<N, T> operator-(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shADD(t, left, -right);
  return t;
}
template<int N, typename T>
inline
ShGeneric<N, T> operator*(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shMUL(t, left, right);
  return t;
}
template<int N, typename T>
inline
ShGeneric<N, T> operator*(const ShGeneric<1, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shMUL(t, left, right);
  return t;
}
template<int N, typename T>
inline
ShGeneric<N, T> operator*(const ShGeneric<N, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shMUL(t, left, right);
  return t;
}
template<typename T>
inline
ShGeneric<1, T> operator*(const ShGeneric<1, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<1, SH_TEMP, T> t;
  shMUL(t, left, right);
  return t;
}

template<int N, typename T>
inline
ShGeneric<N, T> operator/(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shDIV(t, left, right);
  return t;
}
template<int N, typename T>
inline
ShGeneric<N, T> operator/(const ShGeneric<N, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shDIV(t, left, right);
  return t;
}
template<typename T>
inline
ShGeneric<1, T> operator/(const ShGeneric<1, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<1, SH_TEMP, T> t;
  shDIV(t, left, right);
  return t;
}

template<int N, typename T>
inline
ShGeneric<N, T> pow(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shPOW(t, left, right);
  return t;
}
template<int N, typename T>
inline
ShGeneric<N, T> pow(const ShGeneric<N, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shPOW(t, left, right);
  return t;
}
template<typename T>
inline
ShGeneric<1, T> pow(const ShGeneric<1, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<1, SH_TEMP, T> t;
  shPOW(t, left, right);
  return t;
}

template<int N, typename T>
inline
ShGeneric<N, T> mad(const ShGeneric<N, T>& m1, const ShGeneric<N, T>& m2, 
                    const ShGeneric<N, T>& a)
{
  ShAttrib<N, SH_TEMP, T> t;
  shMAD(t, m1, m2, a);
  return t;
}
template<int N, typename T>
inline
ShGeneric<N, T> mad(const ShGeneric<N, T>& m1, const ShGeneric<1, T>& m2, 
                    const ShGeneric<N, T>& a)
{
  ShAttrib<N, SH_TEMP, T> t;
  shMAD(t, m1, m2, a);
  return t;
}
template<int N, typename T>
inline
ShGeneric<N, T> mad(const ShGeneric<1, T>& m1, const ShGeneric<N, T>& m2, 
                    const ShGeneric<N, T>& a)
{
  ShAttrib<N, SH_TEMP, T> t;
  shMAD(t, m1, m2, a);
  return t;
}
template<typename T>
inline
ShGeneric<1, T> mad(const ShGeneric<1, T>& m1, const ShGeneric<1, T>& m2, 
                    const ShGeneric<1, T>& a)
{
  ShAttrib<1, SH_TEMP, T> t;
  shMAD(t, m1, m2, a);
  return t;
}

template<int N, typename T> 
inline
ShGeneric<N, T> mad(T m1, const ShGeneric<N, T>& m2, const ShGeneric<N, T>& a)
{
  ShAttrib<N, SH_TEMP, T> t;
  shMAD(t, ShAttrib<1, SH_CONST, T>(m1), m2, a);
  return t;
}
template<int N, typename T> 
inline
ShGeneric<N, T> mad(double m1, const ShGeneric<N, T>& m2, const ShGeneric<N, T>& a)
{
  ShAttrib<N, SH_TEMP, T> t;
  shMAD(t, ShAttrib<1, SH_CONST, T>(m1), m2, a);
  return t;
}
template<int N, typename T> 
inline
ShGeneric<N, T> mad(const ShGeneric<N, T>& m1, T m2, const ShGeneric<N, T>& a)
{
  ShAttrib<N, SH_TEMP, T> t;
  shMAD(t, m1, ShAttrib<1, SH_CONST, T>(m2), a);
  return t;
}
template<int N, typename T> 
inline
ShGeneric<N, T> mad(const ShGeneric<N, T>& m1, double m2, const ShGeneric<N, T>& a)
{
  ShAttrib<N, SH_TEMP, T> t;
  shMAD(t, m1, ShAttrib<1, SH_CONST, T>(m2), a);
  return t;
}

template<int N, typename T>
inline
ShGeneric<N, T> sqrt(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSQRT(t, var);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> lerp(const ShGeneric<N, T>& f, const ShGeneric<N, T>& a, 
                     const ShGeneric<N, T>& b)
{
  ShAttrib<N, SH_TEMP, T> t;
  shLRP(t, f, a, b);
  return t;
}
template<int N, typename T>
ShGeneric<N, T> lerp(const ShGeneric<1, T>& f, const ShGeneric<N, T>& a, 
                     const ShGeneric<N, T>& b)
{
  ShAttrib<N, SH_TEMP, T> t;
  shLRP(t, f, a, b);
  return t;
}
template<typename T>
ShGeneric<1, T> lerp(const ShGeneric<1, T>& f, const ShGeneric<1, T>& a, 
                     const ShGeneric<1, T>& b)
{
  ShAttrib<1, SH_TEMP, T> t;
  shLRP(t, f, a, b);
  return t;
}
template<int N, typename T>
ShGeneric<N, T> lerp(T f, const ShGeneric<N, T>& a, 
                     const ShGeneric<N, T>& b)
{
  ShAttrib<N, SH_TEMP, T> t;
  shLRP(t, f, a, b);
  return t;
}

}

#endif
