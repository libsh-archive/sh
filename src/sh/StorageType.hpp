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
#ifndef SHSTORAGETYPE_HPP
#define SHSTORAGETYPE_HPP

#include "Utility.hpp"
#include "VariableType.hpp"
#include "Affine.hpp"
#include "Interval.hpp"
#include "Half.hpp"
#include "Fraction.hpp"

/** @file DataType.hpp
 * Defines the host computation and memory storage c++ types associated with
 * each Sh value type.
 *
 * Also defines   
 */
namespace SH {

/** A special C++ type used to represent an invalid storage type */
struct InvalidStorageType {};

/**  The various value type integers correspond to storage types. 
 *
 * Sh internally uses unsigned integers < 2^16.  User-defined types must use
 * values >= 2^16. 
 *
 * This is idea behind the current set of value types:
 * i) bits 0-2 = B, where 1 << B = sizeof type in bytes 
 * ii) bit 3 = 0 for signed type, 1 for unsigned
 * iii) bits 4-7 = 0 for floating point, 1 for integer, 2 for fraction 
 * iv) bits 8-14 = 0 for regular, 1 for interval, 2 for affine 
 * v) bit 15 = 0 for valid type, 1 for invalid type
 * @{ */
typedef unsigned int ValueType;
enum __ValueTypeEnum {
  SH_HALF       = 0x0001, 
  SH_FLOAT      = 0x0002, 
  SH_DOUBLE     = 0x0003, 

  SH_BYTE       = 0x0010,
  SH_SHORT      = 0x0011,
  SH_INT        = 0x0012,

  SH_UBYTE      = 0x0018,
  SH_USHORT     = 0x0019,
  SH_UINT       = 0x001A,

  SH_FBYTE      = 0x0020,
  SH_FSHORT     = 0x0021,
  SH_FINT       = 0x0022,

  SH_FUBYTE     = 0x0028,
  SH_FUSHORT    = 0x0029,
  SH_FUINT      = 0x002A,

  SH_I_HALF    = 0x0101,
  SH_I_FLOAT   = 0x0102,
  SH_I_DOUBLE  = 0x0103,

  SH_A_HALF    = 0x0201,
  SH_A_FLOAT   = 0x0202,
  SH_A_DOUBLE  = 0x0203,

  VALUETYPE_SIZE_MASK = 0x0007,

  VALUETYPE_SIGNED_MASK = 0x0008,
  VALUETYPE_SIGNED = 0x0000,

  VALUETYPE_TYPE_MASK = 0x00F0,
  VALUETYPE_TYPE_FLOAT = 0x0000,
  VALUETYPE_TYPE_INT   = 0x0010,
  VALUETYPE_TYPE_FRAC  = 0x0020,

  VALUETYPE_SPECIAL_MASK = 0x7F00, 
  VALUETYPE_SPECIAL_NONE = 0x0000, 
  VALUETYPE_SPECIAL_I    = 0x0100, 
  VALUETYPE_SPECIAL_A    = 0x0200, 

  VALUETYPE_END = 0xFFFF
};
// @}

/** Functions to retrieve traits about storage types from their ValueType 
 *
 * @todo range - these should check for user defined value types later
 * and return false.
 * @{ */
bool isFloat(ValueType value_type);
bool isInteger(ValueType value_type);
bool isFraction(ValueType value_type);

bool isSigned(ValueType value_type);

bool isRegularValueType(ValueType value_type);
bool isInterval(ValueType value_type);
bool isAffine(ValueType value_type);
bool isRange(ValueType value_type);

bool isInvalidValueType(ValueType value_type);
// @}

/** Mappings from value type to storage type and back. 
 * @{ */ 
template<ValueType V> struct __ValueToStorageType { typedef InvalidStorageType type; };
template<typename T> struct __StorageToValueType { static const ValueType type = VALUETYPE_END; };

#define VALUE_STORAGE_TYPE_MAPPING(V, T)\
  template<> struct __ValueToStorageType<V >    { typedef T type; }; \
  template<> struct __StorageToValueType<T >    { static const ValueType type = V; }; 

VALUE_STORAGE_TYPE_MAPPING(SH_HALF,    Half); 
VALUE_STORAGE_TYPE_MAPPING(SH_FLOAT,   float); 
VALUE_STORAGE_TYPE_MAPPING(SH_DOUBLE,  double); 

VALUE_STORAGE_TYPE_MAPPING(SH_BYTE,    char); 
VALUE_STORAGE_TYPE_MAPPING(SH_SHORT,   short); 
VALUE_STORAGE_TYPE_MAPPING(SH_INT,     int); 
VALUE_STORAGE_TYPE_MAPPING(SH_UBYTE,   unsigned char); 
VALUE_STORAGE_TYPE_MAPPING(SH_USHORT,  unsigned short); 
VALUE_STORAGE_TYPE_MAPPING(SH_UINT,    unsigned int); 

VALUE_STORAGE_TYPE_MAPPING(SH_FBYTE,   FracByte); 
VALUE_STORAGE_TYPE_MAPPING(SH_FSHORT,  FracShort); 
VALUE_STORAGE_TYPE_MAPPING(SH_FINT,    FracInt); 
VALUE_STORAGE_TYPE_MAPPING(SH_FUBYTE,  FracUByte); 
VALUE_STORAGE_TYPE_MAPPING(SH_FUSHORT, FracUShort); 
VALUE_STORAGE_TYPE_MAPPING(SH_FUINT,   FracUInt); 

//VALUE_STORAGE_TYPE_MAPPING(SH_I_HALF,  Interval<Half>); 
VALUE_STORAGE_TYPE_MAPPING(SH_I_FLOAT, Interval<float>); 
VALUE_STORAGE_TYPE_MAPPING(SH_I_DOUBLE,Interval<double>); 

//VALUE_STORAGE_TYPE_MAPPING(SH_A_HALF,  Affine<Half>); 
VALUE_STORAGE_TYPE_MAPPING(SH_A_FLOAT, Affine<float>); 
VALUE_STORAGE_TYPE_MAPPING(SH_A_DOUBLE,Affine<double>); 
// @}

/** Mapping from Storage Type to a name 
 * @{*/
template<typename T> struct __StorageTypeName { static const char* name; };
template<typename T> const char* __StorageTypeName<T>::name = "unknown";

#define STORAGETYPE_NAME_SPEC(T)\
  template<> struct SH_DLLEXPORT __StorageTypeName<T > { static const char* name; };

STORAGETYPE_NAME_SPEC(Half);
STORAGETYPE_NAME_SPEC(float);
STORAGETYPE_NAME_SPEC(double);

STORAGETYPE_NAME_SPEC(char);
STORAGETYPE_NAME_SPEC(short);
STORAGETYPE_NAME_SPEC(int);
STORAGETYPE_NAME_SPEC(unsigned char);
STORAGETYPE_NAME_SPEC(unsigned short);
STORAGETYPE_NAME_SPEC(unsigned int);

STORAGETYPE_NAME_SPEC(FracByte);
STORAGETYPE_NAME_SPEC(FracShort);
STORAGETYPE_NAME_SPEC(FracInt);
STORAGETYPE_NAME_SPEC(FracUByte);
STORAGETYPE_NAME_SPEC(FracUShort);
STORAGETYPE_NAME_SPEC(FracUInt);

//STORAGETYPE_NAME_SPEC(Interval<Half>);
STORAGETYPE_NAME_SPEC(Interval<float>);
STORAGETYPE_NAME_SPEC(Interval<double>);

//STORAGETYPE_NAME_SPEC(Affine<Half>);
STORAGETYPE_NAME_SPEC(Affine<float>);
STORAGETYPE_NAME_SPEC(Affine<double>);

#undef STORAGETYPE_NAME_SPEC
//@}


/** Returns whether a type is an interval type
 * @{ */
template<typename T>
struct IsInterval: public MatchTemplateType<T, Interval> {};
//@}

/** Returns whether a type is an affine type
 * @{ */
template<typename T>
struct IsAffine: public MatchTemplateType<T, Affine> {};
//@}

/** Returns whether a type is a range arithmetic type (interval or affine at
 * this point) 
 * @{ */
template<typename T>
struct IsRange 
{
  static const bool matches = IsAffine<T>::matches || IsInterval<T>::matches;
};
//@}

/** Returns whether a type is a fraction type
 * @{ */
template<typename T>
struct IsFraction: public MatchTemplateType<T, Fraction> {};
//@}

/** Returns the regular value type corresponding to a special templated value type
 * (interval or affine) so far 
 * @{ */
template<ValueType V>
struct __RegularValueType 
{
  static const ValueType type = (V & ~VALUETYPE_SPECIAL_MASK); 
};

template<typename T>
struct __RegularStorageType
{
  static const ValueType range_value_type = __StorageToValueType<T>::type;
  static const ValueType value_type = __RegularValueType<range_value_type>::type; 
  typedef typename __ValueToStorageType<value_type>::type type; 
};

inline 
ValueType regularValueType(ValueType value_type); 
// @}

/** Returns an interval value type corresponding to a type,
 * or SH_VALUETYPE_NONE if no such type is defined; 
 * @{ */

template<typename T>
struct __IntervalStorageType
{
  typedef Interval<typename __RegularStorageType<T>::type> type;
};

inline 
ValueType intervalValueType(ValueType value_type); 
// @}

/** Returns an affine value type corresponding to a type,
 * or SH_VALUETYPE_NONE if no such type is defined; 
 * @{ */

template<typename T>
struct __AffineStorageType
{
  typedef Affine<typename __RegularStorageType<T>::type> type;
};

inline 
ValueType affineValueType(ValueType value_type); 
// @}


/** Provides a least common ancestor in the automatic promotion tree
 * for use in immediate mode
 * @{ */
template<typename T1, typename T2>
struct CommonType;

template<typename T1, typename T2, typename T3>
struct CommonType3 {
  typedef typename CommonType<typename CommonType<T1, T2>::type, T3>::type type; 
};

template<typename T1, typename T2, typename T3, typename T4>
struct CommonType4 {
  typedef typename CommonType<typename CommonType<T1, T2>::type, 
                                typename CommonType<T3, T4>::type>::type type; 
};
// @}

/** Holds much of the above information in one place.
 * This is the class to specialize for user-defined types
 * @{ */
template<typename T>
struct StorageTypeInfo {
  typedef T storage_type;
  static const ValueType value_type = __StorageToValueType<T>::type; 


  /** Non-special storage type corresponding to T */
  typedef typename __RegularStorageType<T>::type RegularType;
  static const ValueType RegularValueType = __StorageToValueType<RegularType>::type;

  // @todo not sure we want all of these here, since there could be user-defined 
  // special types too.  This might be too restrictive if we depend on these. 

  /** Interval storage type corresponding to T (either T itself or uses T as its
   * bounds).  May be InvalidStorageType if no proper interval type exists */ 
  typedef typename __IntervalStorageType<T>::type IntervalType; 
  static const ValueType IntervalValueType = __StorageToValueType<IntervalType>::type;

  /** Affine storage type corresponding to T (either T itself or uses T as its
   * bounds).  May be InvalidStorageType if no proper interval type exists */ 
  typedef typename __AffineStorageType<T>::type AffineType; 
  static const ValueType AffineValueType = __StorageToValueType<AffineType>::type;

  static const char* name; 

  // @todo type include here the ability to set available operations 

  // @todo type include here Programs for transforming from this
  // storage type to a set of different ones, allowing the transformers
  // to choose the best conversions.

  // once the above two things are complete, than user-defined types should
  // work just fine.
};

template<typename T>
const char* StorageTypeInfo<T>::name = __StorageTypeName<T>::name;


//@}

/** Subclass of StorageTypeInfo so we can lookup the same things with a
 * ValueType.  This is probably not necessary though since most of the time
 * ValueType is used only internally in the bowels of Sh. */
template<ValueType V>
struct ValueTypeInfo: public StorageTypeInfo<typename __ValueToStorageType<V>::type> {
};

}

#include "StorageTypeImpl.hpp"

#endif
