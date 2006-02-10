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

#include "ShUtility.hpp"
#include "ShVariableType.hpp"
#include "ShHalf.hpp"
#include "ShFraction.hpp"
#include "ShStorageType.hpp"

/** @file ShDataType.hpp
 * Defines the host computation and memory storage c++ types associated with
 * each Sh value type.
 *
 * Also defines   
 */
namespace SH {
/**  Used to denote the kinds of C++ data types associated with a Value type. 
 */
enum ShDataType {
  SH_HOST,  //< computation data type on the host
  SH_MEM,  //< data type stored in memory on the host
  SH_DATATYPE_END
};

SH_DLLEXPORT
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
template<typename T, ShDataType DT> struct ShDataTypeCppType; 

template<typename T> struct ShDataTypeCppType<T, SH_HOST> { typedef T type; }; 
template<typename T> struct ShDataTypeCppType<T, SH_MEM> { typedef T type; }; 

// define special cases here
#define SH_VALUETYPE_DATATYPE(T, hostType, memType)\
  template<> struct ShDataTypeCppType<T, SH_HOST> { typedef hostType type; }; \
  template<> struct ShDataTypeCppType<T, SH_MEM> { typedef memType type; }; 

SH_VALUETYPE_DATATYPE(ShHalf, float, ShHalf); 

template<typename T> struct ShHostType { typedef typename ShDataTypeCppType<T, SH_HOST>::type type; };
template<typename T> struct ShMemType { typedef typename ShDataTypeCppType<T, SH_MEM>::type type; };
// @}

/**  Sets the constant values for a given data type.
 * Currently only the additive and multiplicative inverses are here.
 * And with all current types, Zero is a false value and One is a true value
 * (although usually not the only ones).
 * @{ */
template<typename T, ShDataType DT> 
struct ShDataTypeConstant {
    typedef typename ShDataTypeCppType<T, DT>::type type;
    static const type Zero; /* additive identity and also a true value */ \
    static const type One; /* multiplicative identity also a false value */ \
};

template<typename T, ShDataType DT>
const typename ShDataTypeCppType<T, DT>::type ShDataTypeConstant<T, DT>::Zero = 
  (typename ShDataTypeCppType<T, DT>::type)(0.0); 

template<typename T, ShDataType DT>
const typename ShDataTypeCppType<T, DT>::type ShDataTypeConstant<T, DT>::One = 
  (typename ShDataTypeCppType<T, DT>::type)(1.0); 
// @}

/** Returns the boolean cond in the requested data type */
template<typename T, ShDataType DT>
inline
typename ShDataTypeCppType<T, DT>::type shDataTypeCond(bool cond);

/** Returns a whether the two values are exactly the same.
 * This is is useful for the range types.
 * @{ */
template<typename T>
inline
bool shDataTypeEqual(const T &a, const T &b);
// @}

/** Returns whether the value is always greater than zero (i.e. true) 
 */
template<typename T>
inline
bool shDataTypeIsPositive(const T &a);


/** Casts one data type to another data type 
 */
template<typename T1, ShDataType DT1, typename T2, ShDataType DT2>
void shDataTypeCast(typename ShDataTypeCppType<T1, DT1>::type &dest,
                    const typename ShDataTypeCppType<T2, DT2>::type &src);

}

#include "ShDataTypeImpl.hpp"

#endif
