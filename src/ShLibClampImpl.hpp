#ifndef SHLIBCLAMPIMPL_HPP
#define SHLIBCLAMPIMPL_HPP

#include "ShLibClamp.hpp"
#include "ShInstructions.hpp"
#include "ShAttrib.hpp"
#include "ShLibMiscImpl.hpp"

namespace SH {

template<int N, typename T>
inline
ShGeneric<N, T> abs(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shABS(t, var);
  return t;
}

template<int N, typename T>
inline
ShGeneric<N, T> ceil(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shCEIL(t, var);
  return t;
}

template<int N, typename T>
inline
ShGeneric<N, T> floor(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shFLR(t, var);
  return t;
}

template<int N, typename T1, typename T2>
inline
ShGeneric<N, CT1T2> mod(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shMOD(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
inline
ShGeneric<N, CT1T2> mod(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shMOD(t, left, right);
  return t;
}
template<typename T1, typename T2>
inline
ShGeneric<1, CT1T2> mod(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<1, SH_TEMP, CT1T2> t;
  shMOD(t, left, right);
  return t;
}

template<int N, typename T1, typename T2>
inline
ShGeneric<N, CT1T2> operator%(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right)
{
  return mod(left, right);
}
template<int N, typename T1, typename T2>
inline
ShGeneric<N, CT1T2> operator%(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right)
{
  return mod(left, right);
}
template<typename T1, typename T2>
inline
ShGeneric<1, CT1T2> operator%(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right)
{
  return mod(left, right);
}

template<int N, typename T>
inline
ShGeneric<N, T> frac(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shFRAC(t, var);
  return t;
}

template<int N, typename T>
inline
ShGeneric<N,  T> pos(const ShGeneric<N, T>& var)
{
  return max(var, fillcast<N>(0.0f));
}

template<int N, typename T1, typename T2>
inline
ShGeneric<N,  CT1T2> max(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shMAX(t, left, right);
  return t;
}

template<int N, typename T1, typename T2>
inline
ShGeneric<N,  CT1T2> min(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shMIN(t, left, right);
  return t;
}

template<int N, typename T1, typename T2, typename T3>
inline
ShGeneric<N, CT1T2T3> clamp(const ShGeneric<N, T1>& a,
                      const ShGeneric<N, T2>& b, const ShGeneric<N, T3>& c)
{
  return min(max(a, b), c);
}
template<int N, typename T1, typename T2, typename T3>
inline
ShGeneric<N, CT1T2T3> clamp(const ShGeneric<N, T1>& a,
                      const ShGeneric<1, T2>& b, const ShGeneric<1, T3>& c)
{
  return min(max(a, fillcast<N>(b)), fillcast<N>(c));
}

template<typename T1, typename T2, typename T3>
inline
ShGeneric<1, CT1T2T3> clamp(const ShGeneric<1, T1>& a,
                      const ShGeneric<1, T2>& b, const ShGeneric<1, T3>& c)
{
  return min(max(a, b), c);
}

template<int N, typename T>
inline
ShGeneric<N, T> sat(const ShGeneric<N, T>& var)
{
  return min(var, fillcast<N>(ShConstAttrib1f(1.0)));
}

template<int N, typename T>
inline
ShGeneric<N, T> sign(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSGN(t, var);
  return t;
}

}

#endif
