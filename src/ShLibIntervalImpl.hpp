#ifndef SHLIBINTERVALIMPL_HPP
#define SHLIBINTERVALIMPL_HPP

#include "ShLibInterval.hpp"

namespace SH {

template<int N, ShValueType V>
inline
ShGeneric<N, V> lo(const ShGeneric<N, ShIntervalValueType<V>::type>& var)
{
  ShAttrib<N, SH_TEMP, V> t;
  shLO(t, var);
  return t;
}

template<int N, ShValueType V>
inline
ShGeneric<N, V> hi(const ShGeneric<N, ShIntervalValueType<V>::type>& var)
{
  ShAttrib<N, SH_TEMP, V> t;
  shHI(t, var);
  return t;
}

template<int N, ShValueType V1, ShValueType V2>
inline
void setlo(const ShGeneric<N, ShIntervalValueType<V1>::type>& var, const ShGeneric<N, V2> &lo)
{
  shSETLO(var, lo);
  return var;
}

template<int N, ShValueType V1, ShValueType V2>
inline
void sethi(const ShGeneric<N, ShIntervalValueType<V1>::type>& var, const ShGeneric<N, V2> &hi)
{
  shSETHI(var, hi);
  return var;
}

template<int N, ShValueType V1, ShValueType V2>
inline
ShGeneric<N, ShIntervalValueType<CV1V2>::type> makeInterval(const ShGeneric<N, V1>& lo,
    const ShGeneric<N, V2> &hi) 
{
  ShAttrib<N, SH_TEMP, ShIntervalValueType<CV1V2>::type> t;
  shSETLO(t, lo);
  shSETHI(t, hi);
  return t;
}

}

#endif
