#ifndef SHLIBDERIVIMPL_HPP
#define SHLIBDERIVIMPL_HPP

#include "ShLibDeriv.hpp"

namespace SH {

template<int N, typename T>
inline
ShGeneric<N, T> dx(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shDX(t, var);
  return t;
}

template<int N, typename T>
inline
ShGeneric<N, T> dy(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shDY(t, var);
  return t;
}

template<int N, typename T>
inline
ShGeneric<N, T> fwidth(const ShGeneric<N, T>& var)
{
  return max(abs(dx(var)), abs(dy(var)));
}

template<typename T>
inline
ShGeneric<2, T> gradient(const ShGeneric<1, T>& var)
{
  return ShAttrib2f(dx(var), dy(var));
}

template<int N, typename T>
inline
ShMatrix<2, N, SH_TEMP, T> jacobian(const ShGeneric<N, T>& var)
{
  ShMatrix<2, N, SH_TEMP, T> ret;
  ret[0] = dx(var);
  ret[1] = dy(var);
  return ret;
}



}

#endif
