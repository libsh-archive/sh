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

template<int N, typename T>
inline
ShGeneric<N, T> fmod(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shFMOD(t, left, right);
  return t;
}
template<int N, typename T>
inline
ShGeneric<N, T> fmod(const ShGeneric<N, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shFMOD(t, left, right);
  return t;
}
template<typename T>
inline
ShGeneric<1, T> fmod(const ShGeneric<1, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<1, SH_TEMP, T> t;
  shFMOD(t, left, right);
  return t;
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

template<int N, typename T>
inline
ShGeneric<N,  T> max(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shMAX(t, left, right);
  return t;
}

template<int N, typename T>
inline
ShGeneric<N,  T> min(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shMIN(t, left, right);
  return t;
}

}

#endif
