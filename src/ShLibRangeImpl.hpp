#ifndef SHLIBRANGEIMPL_HPP
#define SHLIBRANGEIMPL_HPP

#include "ShLibRange.hpp"

namespace SH {

template<int N, typename T>
inline
ShGeneric<N, SH_REGULARTYPE(T)> 
range_lo(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, SH_REGULARTYPE(T)> t;
  shLO(t, var);
  return t;
}

template<int N, typename T>
inline
ShGeneric<N, SH_REGULARTYPE(T)> 
range_hi(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, SH_REGULARTYPE(T)> t;
  shHI(t, var);
  return t;
}

template<int N, typename T>
inline
ShGeneric<N, SH_REGULARTYPE(T)> 
range_width(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, SH_REGULARTYPE(T)> t;
  shWIDTH(t, var);
  return t;
}

template<int N, typename T>
inline
ShGeneric<N, SH_REGULARTYPE(T)> 
range_center(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, SH_REGULARTYPE(T)> t;
  shCENTER(t, var);
  return t;
}

template<int N, typename T1, typename T2>
inline
ShGeneric<N, SH_IA_TYPE(CT1T2)> make_interval(const ShGeneric<N, T1>& lo,
    const ShGeneric<N, T2> &hi) 
{
  ShAttrib<N, SH_TEMP, SH_IA_TYPE(CT1T2)> t;
  shIVAL(t, lo, hi);
  return t;
}
SH_SHLIB_CONST_SCALAR_OP(make_interval);
SH_SHLIB_CONST_N_OP_BOTH(make_interval);

template<int N, typename T1, typename T2>
inline
ShGeneric<N, CT1T2> range_union(const ShGeneric<N, T1>& a,
    const ShGeneric<N, T2> &b) 
{
  SH_DEBUG_ASSERT(ShIsRange<T1>::matches && ShIsRange<T2>::matches);
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shUNION(t, a, b);
  return t;
}
SH_SHLIB_CONST_SCALAR_OP(range_union);
SH_SHLIB_CONST_N_OP_BOTH(range_union);

template<int N, typename T1, typename T2>
inline
ShGeneric<N, CT1T2> range_isct(const ShGeneric<N, T1>& a,
    const ShGeneric<N, T2> &b) 
{
  SH_DEBUG_ASSERT(ShIsRange<T1>::matches && ShIsRange<T2>::matches);
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shISCT(t, a, b);
  return t;
}
SH_SHLIB_CONST_SCALAR_OP(range_isct);
SH_SHLIB_CONST_N_OP_BOTH(range_isct);

template<int N, typename T1, typename T2>
inline
ShGeneric<N, CT1T2> range_contains(const ShGeneric<N, T1>& a,
    const ShGeneric<N, T2> &b) 
{
  SH_DEBUG_ASSERT(ShIsRange<T1>::matches && ShIsRange<T2>::matches);
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shCONTAINS(t, a, b);
  return t;
}
SH_SHLIB_CONST_SCALAR_OP(range_contains);
SH_SHLIB_CONST_N_OP_BOTH(range_contains);

}

#endif
