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

}

#endif
