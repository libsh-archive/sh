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
#ifndef SHDATATYPE_HPP
#define SHDATATYPE_HPP

#include "Utility.hpp"
#include "VariableType.hpp"
#include "Half.hpp"
#include "Fraction.hpp"
#include "StorageType.hpp"

/** @file DataType.hpp
 * Defines the host computation and memory storage c++ types associated with
 * each Sh value type.
 *
 * Also defines   
 */
namespace SH {
/**  Used to denote the kinds of C++ data types associated with a Value type. 
 */
enum DataType {
  HOST,  //< computation data type on the host
  MEM,  //< data type stored in memory on the host
  DATATYPE_END
};

DLLEXPORT
extern const char* dataTypeName[];

/** Sets the actual host computation and memory data types for a given Value type.
 *  The default is computation and memory types equal the Value type.
 *  Defined below are the exceptions to this rule
 * 
 *  half floats - memory type is 1 sign bit, 5 exponent bits, 10 mantissa bits
 *                see ATI_pixel_format_float for specification
 *                (NV_half_float is similar, except it has NaN and INF)
 * 
 * @{ */
template<typename T, DataType DT> struct DataTypeCppType; 

template<typename T> struct DataTypeCppType<T, HOST> { typedef T type; }; 
template<typename T> struct DataTypeCppType<T, MEM> { typedef T type; }; 

// define special cases here
#define VALUETYPE_DATATYPE(T, hostType, memType)\
  template<> struct DataTypeCppType<T, HOST> { typedef hostType type; }; \
  template<> struct DataTypeCppType<T, MEM> { typedef memType type; }; 

VALUETYPE_DATATYPE(Half, float, Half); 

template<typename T> struct HostType { typedef typename DataTypeCppType<T, HOST>::type type; };
template<typename T> struct MemType { typedef typename DataTypeCppType<T, MEM>::type type; };
// @}

/**  Sets the constant values for a given data type.
 * Currently only the additive and multiplicative inverses are here.
 * And with all current types, Zero is a false value and One is a true value
 * (although usually not the only ones).
 * @{ */
template<typename T, DataType DT> 
struct DataTypeConstant {
    typedef typename DataTypeCppType<T, DT>::type type;
    static const type Zero; /* additive identity and also a true value */ \
    static const type One; /* multiplicative identity also a false value */ \
};

template<typename T, DataType DT>
const typename DataTypeCppType<T, DT>::type DataTypeConstant<T, DT>::Zero = 
  (typename DataTypeCppType<T, DT>::type)(0.0); 

template<typename T, DataType DT>
const typename DataTypeCppType<T, DT>::type DataTypeConstant<T, DT>::One = 
  (typename DataTypeCppType<T, DT>::type)(1.0); 
// @}

/** Returns the boolean cond in the requested data type */
template<typename T, DataType DT>
inline
typename DataTypeCppType<T, DT>::type dataTypeCond(bool cond);

/** Returns a whether the two values are exactly the same.
 * This is is useful for the range types.
 * @{ */
template<typename T>
inline
bool dataTypeEqual(const T &a, const T &b);
// @}

/** Returns whether the value is always greater than zero (i.e. true) 
 */
template<typename T>
inline
bool dataTypeIsPositive(const T &a);


/** Casts one data type to another data type 
 */
template<typename T1, DataType DT1, typename T2, DataType DT2>
void dataTypeCast(typename DataTypeCppType<T1, DT1>::type &dest,
                    const typename DataTypeCppType<T2, DT2>::type &src);

}

#include "DataTypeImpl.hpp"

#endif
