#ifndef SHLIBINTERVALIMPL_HPP
#define SHLIBINTERVALIMPL_HPP

#include "ShLibInterval.hpp"

namespace SH {

template<int N, typename T>
inline
ShGeneric<N, T> lo(const ShGeneric<N, ShInterval<T> >& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shLO(t, var);
  return t;
}

template<int N, typename T>
inline
ShGeneric<N, T> hi(const ShGeneric<N, ShInterval<T> >& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shHI(t, var);
  return t;
}

template<int N, typename T1, typename T2>
inline
void setlo(const ShGeneric<N, ShInterval<T1> >& var, const ShGeneric<N, T2> &lo)
{
  shSETLO(var, lo);
  return var;
}

template<int N, typename T1, typename T2>
inline
void sethi(const ShGeneric<N, ShInterval<T1> >& var, const ShGeneric<N, T2> &hi)
{
  shSETHI(var, hi);
  return var;
}

template<int N, typename T1, typename T2>
inline
ShGeneric<N, ShInterval<CT1T2> > makeInterval(const ShGeneric<N, T1>& lo,
    const ShGeneric<N, T2> &hi) 
{
  ShAttrib<N, SH_TEMP, ShInterval<CT1T2> > t;
  shSETLO(t, lo);
  shSETHI(t, hi);
  return t;
}

}

#endif
