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

}

#endif
