#ifndef SHLIBARITHIMPL_HPP
#define SHLIBARITHIMPL_HPP

#include "ShLibArith.hpp"
#include "ShInstructions.hpp"

namespace SH {

template<int N, typename T1, typename T2>
inline
ShGeneric<N, CT1T2>
operator+(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shADD(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
inline
ShGeneric<N, CT1T2>
operator+(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shADD(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
inline
ShGeneric<N, CT1T2>
operator+(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shADD(t, left, right);
  return t;
}
template<typename T1, typename T2>
inline
ShGeneric<1, CT1T2>
operator+(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<1, SH_TEMP, CT1T2> t;
  shADD(t, left, right);
  return t;
}

template<int N, typename T1, typename T2>
inline
ShGeneric<N, CT1T2>
operator-(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shADD(t, left, -right);
  return t;
}
template<int N, typename T1, typename T2>
inline
ShGeneric<N, CT1T2>
operator-(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shADD(t, left, -right);
  return t;
}
template<int N, typename T1, typename T2>
inline
ShGeneric<N, CT1T2>
operator-(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shADD(t, left, -right);
  return t;
}
template<typename T1, typename T2>
inline
ShGeneric<1, CT1T2>
operator-(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<1, SH_TEMP, CT1T2> t;
  shADD(t, left, -right);
  return t;
}

template<int N, typename T1, typename T2>
inline
ShGeneric<N, CT1T2>
operator*(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shMUL(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
inline
ShGeneric<N, CT1T2>
operator*(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shMUL(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
inline
ShGeneric<N, CT1T2>
operator*(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shMUL(t, left, right);
  return t;
}
template<typename T1, typename T2>
inline
ShGeneric<1, CT1T2>
operator*(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<1, SH_TEMP, CT1T2> t;
  shMUL(t, left, right);
  return t;
}

template<int N, typename T1, typename T2>
inline
ShGeneric<N, CT1T2>
operator/(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shDIV(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
inline
ShGeneric<N, CT1T2>
operator/(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shDIV(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
inline
ShGeneric<N, CT1T2>
operator/(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shDIV(t, left, right);
  return t;
}
template<typename T1, typename T2>
inline
ShGeneric<1, CT1T2>
operator/(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<1, SH_TEMP, CT1T2> t;
  shDIV(t, left, right);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> exp(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shEXP(t, var);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> exp2(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shEXP2(t, var);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> exp10(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shEXP10(t, var);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> log(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shLOG(t, var);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> log2(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shLOG2(t, var);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> log10(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shLOG10(t, var);
  return t;
}

template<int N, typename T1, typename T2>
inline
ShGeneric<N, CT1T2> pow(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shPOW(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
inline
ShGeneric<N, CT1T2> pow(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shPOW(t, left, right);
  return t;
}
template<typename T1, typename T2>
inline
ShGeneric<1, CT1T2> pow(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<1, SH_TEMP, CT1T2> t;
  shPOW(t, left, right);
  return t;
}

template<int N, typename T1, typename T2, typename T3>
inline
ShGeneric<N, CT1T2T3> mad(const ShGeneric<N, T1>& m1, const ShGeneric<N, T2>& m2, 
                    const ShGeneric<N, T3>& a)
{
  ShAttrib<N, SH_TEMP, CT1T2T3> t;
  shMAD(t, m1, m2, a);
  return t;
}
template<int N, typename T1, typename T2, typename T3>
inline
ShGeneric<N, CT1T2T3> mad(const ShGeneric<N, T1>& m1, const ShGeneric<1, T2>& m2, 
                    const ShGeneric<N, T3>& a)
{
  ShAttrib<N, SH_TEMP, CT1T2T3> t;
  shMAD(t, m1, m2, a);
  return t;
}
template<int N, typename T1, typename T2, typename T3>
inline
ShGeneric<N, CT1T2T3> mad(const ShGeneric<1, T1>& m1, const ShGeneric<N, T2>& m2, 
                    const ShGeneric<N, T3>& a)
{
  ShAttrib<N, SH_TEMP, CT1T2T3> t;
  shMAD(t, m1, m2, a);
  return t;
}
template<int N, typename T1, typename T2, typename T3>
inline
ShGeneric<1, CT1T2T3> mad(const ShGeneric<1, T1>& m1, const ShGeneric<1, T2>& m2, 
                    const ShGeneric<1, T3>& a)
{
  ShAttrib<1, SH_TEMP, CT1T2T3> t;
  shMAD(t, m1, m2, a);
  return t;
}

template<int N, typename T1, typename T2>
inline
ShGeneric<N, CT1T2> mad(double m1, const ShGeneric<N, T1>& m2, const ShGeneric<N, T2>& a)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shMAD(t, ShAttrib<1, SH_CONST, double>(m1), m2, a);
  return t;
}
template<int N, typename T1, typename T2>
inline
ShGeneric<N, CT1T2> mad(const ShGeneric<N, T1>& m1, double m2, const ShGeneric<N, T2>& a)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shMAD(t, m1, ShAttrib<1, SH_CONST, double>(m2), a);
  return t;
}

template<int N, typename T>
inline
ShGeneric<N, T> rcp(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shRCP(t, var);
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
inline
ShGeneric<N, T> rsqrt(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shRSQ(t, var);
  return t;
}

template<int N, typename T1, typename T2, typename T3>
ShGeneric<N, CT1T2T3> lerp(const ShGeneric<N, T1>& f, const ShGeneric<N, T2>& a, 
                     const ShGeneric<N, T3>& b)
{
  ShAttrib<N, SH_TEMP, CT1T2T3> t;
  shLRP(t, f, a, b);
  return t;
}
template<int N, typename T1, typename T2, typename T3>
ShGeneric<N, CT1T2T3> lerp(const ShGeneric<1, T1>& f, const ShGeneric<N, T2>& a, 
                     const ShGeneric<N, T3>& b)
{
  ShAttrib<N, SH_TEMP, CT1T2T3> t;
  shLRP(t, f, a, b);
  return t;
}
template<typename T1, typename T2, typename T3>
ShGeneric<1, CT1T2T3> lerp(const ShGeneric<1, T1>& f, const ShGeneric<1, T2>& a, 
                     const ShGeneric<1, T3>& b)
{
  ShAttrib<1, SH_TEMP, CT1T2T3> t;
  shLRP(t, f, a, b);
  return t;
}
//@todo type see explanation in LibArith.hpp file
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> lerp(double f, const ShGeneric<N, T1>& a, 
                     const ShGeneric<N, T2>& b)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shLRP(t, f, a, b);
  return t;
}

}

#endif
