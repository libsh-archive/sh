#ifndef SHLIBTRIGIMPL_HPP
#define SHLIBTRIGIMPL_HPP

#include "ShLibTrig.hpp"
#include "ShAttrib.hpp"
#include "ShInstructions.hpp"

namespace SH {

template<int N, typename T>
inline
ShGeneric<N, T> acos(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shACOS(t, var);
  return t;
}

template<int N, typename T>
inline
ShGeneric<N, T> asin(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shASIN(t, var);
  return t;
}

template<int N, typename T>
inline
ShGeneric<N, T> atan(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shATAN(t, var);
  return t;
}

template<int N, typename T>
inline
ShGeneric<N, T> atan2(const ShGeneric<N, T>& y, const ShGeneric<N, T>& x)
{
  ShAttrib<N, SH_TEMP, T> t;
  shATAN2(t, y, x);
  return t;
}

template<int N, typename T>
inline
ShGeneric<N, T> cos(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shCOS(t, var);
  return t;
}

template<int N, typename T>
inline
ShGeneric<N, T> sin(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSIN(t, var);
  return t;
}

template<int N, typename T>
inline
ShGeneric<N, T> tan(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shTAN(t, var);
  return t;
}

}

#endif
