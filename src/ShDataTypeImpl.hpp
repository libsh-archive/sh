// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////
#ifndef SHDATATYPEIMPL_HPP
#define SHDATATYPEIMPL_HPP

#include <climits>
#include <cmath>
#include <algorithm>
#include "ShDataType.hpp"

namespace SH {

/** Returns the boolean cond in the requested data type */
template<ShValueType V, ShDataType DT>
inline
typename ShDataTypeCppType<V, DT>::type shDataTypeCond(bool cond) 
{
  return cond ? ShDataTypeConstant<V, DT>::One : ShDataTypeConstant<V, DT>::Zero;
}

/** Returns a whether the two values are exactly the same.
 * This is is useful for the range types.
 * @{
 */
template<typename T>
inline
bool shDataTypeEqual(const T &a, 
                     const T &b) 
{
  return a == b;
}

template<typename T>
inline
bool shDataTypeEqual(const ShInterval<T> &a, const ShInterval<T> &b)
{
  return (a.lo() == b.lo()) && (a.hi() == b.hi()); 
}
// @}

/** Returns whether the value is always greater than zero (i.e. true) 
 * @{
 */
template<typename T>
inline
bool shDataTypeIsPositive(const T &a)
{
  return a > 0; 
}

template<typename T>
inline
bool shDataTypeIsPositive(const ShInterval<T> &a)
{
  return (a.lo() > 0); 
}

//@}

/** Casts one data type to another data type 
 *@{ */
template<ShValueType V1, ShDataType DT1, ShValueType V2, ShDataType DT2>
struct ShDataTypeCast {
  typedef typename ShDataTypeCppType<V1, DT1>::type desttype; 
  typedef typename ShDataTypeCppType<V2, DT2>::type srctype; 
  static inline void doit(desttype &dest, const srctype &src) {
    dest = (desttype)(src);
  }
};

/** Partial specialization for fractional types on host
 *@{ */
#define SH_DATATYPECAST_FRAC(V1, minval, maxval)\
template<ShValueType V2>\
struct ShDataTypeCast<V1, SH_HOST, V2, SH_HOST>\
{\
  typedef typename ShDataTypeCppType<V1, SH_HOST>::type desttype; \
  typedef typename ShDataTypeCppType<V2, SH_HOST>::type srctype; \
  static inline void doit(desttype &dest, const srctype &src) {\
    dest = (desttype)(src); \
    dest = std::min(std::max(dest, (desttype)(minval)), (desttype)(maxval)); \
  } \
};

SH_DATATYPECAST_FRAC(SH_FRAC_INT, -1, 1);
SH_DATATYPECAST_FRAC(SH_FRAC_SHORT, -1, 1);
SH_DATATYPECAST_FRAC(SH_FRAC_BYTE, -1, 1);
SH_DATATYPECAST_FRAC(SH_FRAC_UINT, 0, 1);
SH_DATATYPECAST_FRAC(SH_FRAC_USHORT, 0, 1);
SH_DATATYPECAST_FRAC(SH_FRAC_UBYTE, 0, 1);
// @}

/** Partial specialization for memory <-> host for fractional types 
 *
 * Going from host -> memory, multiply by scale and round
 * @todo type rounding
 * Going from memory -> host, divide by scale 
 * (assume host type is float and memory type is an int)
 *@{ */
#define SH_DATATYPECAST_FRAC_MEMHOST(V, scale)\
template<>\
struct ShDataTypeCast<V, SH_HOST, V, SH_MEM>\
{\
  typedef ShDataTypeCppType<V, SH_HOST>::type desttype; \
  typedef ShDataTypeCppType<V, SH_MEM>::type srctype; \
  static inline void doit(desttype &dest, const srctype &src) {\
    dest = (desttype)((double)src / scale); \
  } \
};\
template<>\
struct ShDataTypeCast<V, SH_MEM, V, SH_HOST>\
{\
  typedef ShDataTypeCppType<V, SH_MEM>::type desttype; \
  typedef ShDataTypeCppType<V, SH_HOST>::type srctype; \
  static inline void doit(desttype &dest, const srctype &src) {\
    dest = (desttype)((double)src * scale); \
  } \
};

SH_DATATYPECAST_FRAC_MEMHOST(SH_FRAC_INT, INT_MAX);  
SH_DATATYPECAST_FRAC_MEMHOST(SH_FRAC_SHORT, SHRT_MAX);  
SH_DATATYPECAST_FRAC_MEMHOST(SH_FRAC_BYTE, SCHAR_MAX);  
SH_DATATYPECAST_FRAC_MEMHOST(SH_FRAC_UINT, UINT_MAX);  
SH_DATATYPECAST_FRAC_MEMHOST(SH_FRAC_USHORT, USHRT_MAX);  
SH_DATATYPECAST_FRAC_MEMHOST(SH_FRAC_UBYTE, UCHAR_MAX);  
// @}

/** Partial specialization for memory <-> host for half float types
 * These have been unit tested on all values except INFs and NaN.
 */
template<>
struct ShDataTypeCast<SH_HALF, SH_HOST, SH_HALF, SH_MEM> 
{
  static inline void doit(float &dest, const unsigned short &src) 
  {
    // @todo more efficient conversion using bit rep for the float var. 
    // This one is just an attempt at correctness                                       
    short sign = -((src >> 14) & 2) + 1; // -1 for negative, +1 for positive                   
    short exponent = (src >> 10) & 0x1F;
    short mantissa = src & 0x3FF;
    float fraction = sign * ((exponent ? 1 : 0) + mantissa / (float)(1 << 10));

    dest = ldexp(fraction, (exponent ? exponent - 15 : -14));
  } 
};
template<>
struct ShDataTypeCast<SH_HALF, SH_MEM, SH_HALF, SH_HOST> 
{
  static inline void doit(unsigned short &dest, const float &src) 
  {
    int exponent;
    float fraction = frexpf(src, &exponent);
    short sign = fraction < 0;
    fraction = sign ? -fraction : fraction;

    dest = (sign << 15); 
    if(fraction == 0) { // zero
    } else if(fabsf(src) > 65504) { // INF
      dest |= 31 << 10; 
    } else if(exponent < -13) { // denormalized 
      int mantissa = (int)(fraction * (1 << (exponent + 24)));
      dest |= mantissa; 
    } else { // normalized 
      int mantissa = (int)((fraction - 0.5) * (1 << 11));
      dest |= ((exponent + 14) << 10) | mantissa; 
    }
  }
};
//@}

template<ShValueType V1, ShDataType DT1, ShValueType V2, ShDataType DT2>
void shDataTypeCast(typename ShDataTypeCppType<V1, DT1>::type &dest,
                    const typename ShDataTypeCppType<V2, DT2>::type &src)
{
  ShDataTypeCast<V1, DT1, V2, DT2>::doit(dest, src);
}
// @}

}

#endif
