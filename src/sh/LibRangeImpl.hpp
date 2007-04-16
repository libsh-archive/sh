#ifndef SHLIBRANGEIMPL_HPP
#define SHLIBRANGEIMPL_HPP

#include "LibRange.hpp"

namespace SH {

template<int N, typename T>
inline
Generic<N, REGULARTYPE(T)> 
range_lo(const Generic<N, T>& var)
{
  Attrib<N, SH_TEMP, REGULARTYPE(T)> t;
  shLO(t, var);
  return t;
}

template<int N, typename T>
inline
Generic<N, REGULARTYPE(T)> 
range_hi(const Generic<N, T>& var)
{
  Attrib<N, SH_TEMP, REGULARTYPE(T)> t;
  shHI(t, var);
  return t;
}

template<int N, typename T>
inline
Generic<N, REGULARTYPE(T)> 
range_width(const Generic<N, T>& var)
{
  Attrib<N, SH_TEMP, REGULARTYPE(T)> t;
  shWIDTH(t, var);
  return t;
}

template<int N, typename T>
inline
Generic<N, REGULARTYPE(T)> 
range_radius(const Generic<N, T>& var)
{
  Attrib<N, SH_TEMP, REGULARTYPE(T)> t;
  shRADIUS(t, var);
  return t;
}

template<int N, typename T>
inline
Generic<N, REGULARTYPE(T)> 
range_center(const Generic<N, T>& var)
{
  Attrib<N, SH_TEMP, REGULARTYPE(T)> t;
  shCENTER(t, var);
  return t;
}

template<int N, typename T1, typename T2>
inline
Generic<N, IA_TYPE(CT1T2)> make_interval(const Generic<N, T1>& lo,
    const Generic<N, T2> &hi) 
{
  Attrib<N, SH_TEMP, IA_TYPE(CT1T2)> t;
  shIVAL(t, lo, hi);
  return t;
}
SHLIB_CONST_SCALAR_OP(make_interval);
SHLIB_CONST_N_OP_BOTH(make_interval);

template<int N, typename T1, typename T2>
inline
Generic<N, CT1T2> range_union(const Generic<N, T1>& a,
    const Generic<N, T2> &b) 
{
  SH_DEBUG_ASSERT(IsRange<T1>::matches && IsRange<T2>::matches);
  Attrib<N, SH_TEMP, CT1T2> t;
  shUNION(t, a, b);
  return t;
}
SHLIB_CONST_SCALAR_OP(range_union);
SHLIB_CONST_N_OP_BOTH(range_union);

template<int N, typename T1, typename T2>
inline
Generic<N, CT1T2> range_isct(const Generic<N, T1>& a,
    const Generic<N, T2> &b) 
{
  SH_DEBUG_ASSERT(IsRange<T1>::matches && IsRange<T2>::matches);
  Attrib<N, SH_TEMP, CT1T2> t;
  shISCT(t, a, b);
  return t;
}
SHLIB_CONST_SCALAR_OP(range_isct);
SHLIB_CONST_N_OP_BOTH(range_isct);

template<int N, typename T1, typename T2>
inline
Generic<N, CT1T2> range_contains(const Generic<N, T1>& a,
    const Generic<N, T2> &b) 
{
  SH_DEBUG_ASSERT(IsRange<T1>::matches && IsRange<T2>::matches);
  Attrib<N, SH_TEMP, CT1T2> t;
  shCONTAINS(t, a, b);
  return t;
}
SHLIB_CONST_SCALAR_OP(range_contains);
SHLIB_CONST_N_OP_BOTH(range_contains);

template<int N1, typename T1, int N2, typename T2>
Generic<N1, T1>
affine_errfrom(const Generic<N1, T1>& a, const Generic<N2, T2>& b)
{
  SH_DEBUG_ASSERT(IsAffine<T1>::matches && IsAffine<T2>::matches);
  Attrib<N1, SH_TEMP, T1> t;
  shERRFROM(t, a, b);
  return t;
}

template<int N1, typename T1, int N2, typename T2>
Generic<N1, REGULARTYPE(T1)>
affine_lasterr(const Generic<N1, T1>& a, const Generic<N2, T2>& b)
{
  SH_DEBUG_ASSERT(IsAffine<T1>::matches && IsAffine<T2>::matches);
  Attrib<N1, SH_TEMP, REGULARTYPE(T1)> t;
  shLASTERR(t, a, b);
  return t;
}

}

#endif
