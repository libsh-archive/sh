// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
//////////////////////////////////////////////////////////////////////////////
#ifndef SHSTORAGETYPEIMPL_HPP
#define SHSTORAGETYPEIMPL_HPP

#include <climits>
#include <cmath>
#include <algorithm>
#include "StorageType.hpp"

namespace SH {

inline bool isFloat(ValueType value_type)
{
  return !isInvalidValueType(value_type) && 
    (value_type & VALUETYPE_TYPE_MASK) == VALUETYPE_TYPE_FLOAT; 
}

inline bool isInteger(ValueType value_type)
{
  return !isInvalidValueType(value_type) && 
    (value_type & VALUETYPE_TYPE_MASK) == VALUETYPE_TYPE_INT; 
}

inline bool isFraction(ValueType value_type)
{
  return !isInvalidValueType(value_type) && 
    (value_type & VALUETYPE_TYPE_MASK) == VALUETYPE_TYPE_FRAC; 
}

inline bool isSigned(ValueType value_type)
{
  return !isInvalidValueType(value_type) && 
    (value_type & VALUETYPE_SIGNED_MASK) == VALUETYPE_SIGNED;
}

inline bool isRegularValueType(ValueType value_type)
{
  return !isInvalidValueType(value_type) &&
    (value_type & VALUETYPE_SPECIAL_MASK) == VALUETYPE_SPECIAL_NONE;
}

inline bool isInterval(ValueType value_type)
{
  return !isInvalidValueType(value_type) && 
    (value_type & VALUETYPE_SPECIAL_MASK) == VALUETYPE_SPECIAL_I; 
}

inline bool isAffine(ValueType value_type)
{
  return !isInvalidValueType(value_type) && 
    (value_type & VALUETYPE_SPECIAL_MASK) == VALUETYPE_SPECIAL_A; 
}

inline bool isRange(ValueType value_type)
{
  // @todo range - this doesn't make it very extendable for user defined types
  return isInterval(value_type) || isAffine(value_type); 
}

inline bool isInvalidValueType(ValueType value_type)
{
  return value_type == VALUETYPE_END; 
}

inline 
ValueType intervalValueType(ValueType value_type)
{
  return regularValueType(value_type) | VALUETYPE_SPECIAL_I;
}

inline 
ValueType affineValueType(ValueType value_type)
{
  return regularValueType(value_type) | VALUETYPE_SPECIAL_A;
}

inline 
ValueType regularValueType(ValueType value_type)
{
  return isInvalidValueType(value_type) ? value_type :  
         (value_type & ~VALUETYPE_SPECIAL_MASK); 
}

/*** Implementation of the automatic promotion tree lookup
 * This decides the argument conversions used in operators
 * and the result type given by operators (and library funcs).
 * (The tree is built explicitly in TypeInfoCasts.cpp as well,
 * and this should match those tree edges)
 *
 * @see TypeInfoCasts.cpp
 *
 * Rules (checked in order until one matches)
 * 1) If either is affine, and either has base type double, use affine double
 * 2) If either is affine, use affine float
 * 3) If either is interval, and either has base type double, use interval
 * double
 * 4) If either is interval, use interval float
 * 5) If either is double, use double
 * 6) If either is float or fractional, use float
 * 7) If both are half, use half
 * 8) Use int 
 */
template<typename T1, typename T2>
struct CommonType {
  static const ValueType V1 = StorageTypeInfo<T1>::value_type;
  static const ValueType V2 = StorageTypeInfo<T2>::value_type;

  static const bool eitherAffine = IsAffine<T1>::matches || IsAffine<T2>::matches;
  static const bool eitherInterval = IsInterval<T1>::matches || IsInterval<T2>::matches;
  static const bool eitherDouble = V1 == SH_DOUBLE || V2 == SH_DOUBLE; 
  static const bool eitherFloat = V1 == SH_FLOAT || V2 == SH_FLOAT; 
  static const bool eitherFraction = IsFraction<T1>::matches || IsFraction<T2>::matches;
  static const bool bothHalf = V1 == SH_HALF && V2 == SH_HALF;  

  static const ValueType value_type = 
          (eitherAffine ?
            (eitherDouble ? 
              SH_A_DOUBLE :
            SH_A_FLOAT) :
          (eitherInterval ?
            (eitherDouble ?
              SH_I_DOUBLE :
            SH_I_FLOAT) :
          (eitherDouble ?
            SH_DOUBLE :
          ((eitherFloat || eitherFraction) ?
            SH_FLOAT :
          (bothHalf ?
            SH_HALF :
            SH_INT)))));

  typedef typename ValueTypeInfo<value_type>::storage_type type;
};
#undef MATCH


}

#endif
