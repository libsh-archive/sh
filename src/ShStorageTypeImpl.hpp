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
#ifndef SHSTORAGETYPEIMPL_HPP
#define SHSTORAGETYPEIMPL_HPP

#include <climits>
#include <cmath>
#include <algorithm>
#include "ShStorageType.hpp"

namespace SH {

inline bool shIsFloat(ShValueType value_type)
{
  return !shIsInvalidValueType(value_type) && 
    (value_type & SH_VALUETYPE_TYPE_MASK) == SH_VALUETYPE_TYPE_FLOAT; 
}

inline bool shIsInteger(ShValueType value_type)
{
  return !shIsInvalidValueType(value_type) && 
    (value_type & SH_VALUETYPE_TYPE_MASK) == SH_VALUETYPE_TYPE_INT; 
}

inline bool shIsFraction(ShValueType value_type)
{
  return !shIsInvalidValueType(value_type) && 
    (value_type & SH_VALUETYPE_TYPE_MASK) == SH_VALUETYPE_TYPE_FRAC; 
}

inline bool shIsSigned(ShValueType value_type)
{
  return !shIsInvalidValueType(value_type) && 
    (value_type & SH_VALUETYPE_SIGNED_MASK) == SH_VALUETYPE_SIGNED;
}

inline bool shIsRegularValueType(ShValueType value_type)
{
  return !shIsInvalidValueType(value_type) &&
    (value_type & SH_VALUETYPE_SPECIAL_MASK) == SH_VALUETYPE_SPECIAL_NONE;
}

inline bool shIsInvalidValueType(ShValueType value_type)
{
  return value_type == SH_VALUETYPE_END; 
}

inline 
ShValueType shRegularValueType(ShValueType value_type)
{
  return shIsInvalidValueType(value_type) ? value_type :  
         (value_type & ~SH_VALUETYPE_SPECIAL_MASK); 
}

/*** Implementation of the automatic promotion tree lookup
 * This decides the argument conversions used in operators
 * and the result type given by operators (and library funcs).
 * (The tree is built explicitly in ShTypeInfoCasts.cpp as well,
 * and this should match those tree edges)
 *
 * @see ShTypeInfoCasts.cpp
 *
 * Rules (checked in order until one matches)
 * 1) If either is double, use double
 * 2) If either is float or fractional, use float
 * 3) If both are half, use half
 * 4) Use int 
 */
template<typename T1, typename T2>
struct ShCommonType {
  static const ShValueType V1 = ShStorageTypeInfo<T1>::value_type;
  static const ShValueType V2 = ShStorageTypeInfo<T2>::value_type;

  static const bool eitherDouble = V1 == SH_DOUBLE || V2 == SH_DOUBLE; 
  static const bool eitherFloat = V1 == SH_FLOAT || V2 == SH_FLOAT; 
  static const bool eitherFraction = ShIsFraction<T1>::matches || ShIsFraction<T2>::matches;
  static const bool bothHalf = V1 == SH_HALF && V2 == SH_HALF;  

  static const ShValueType value_type = 
          (eitherDouble ?
            SH_DOUBLE :
          ((eitherFloat || eitherFraction) ?
            SH_FLOAT :
          (bothHalf ?
            SH_HALF :
            SH_INT)));

  typedef typename ShValueTypeInfo<value_type>::storage_type type;
};
#undef SH_MATCH


}

#endif
