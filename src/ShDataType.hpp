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
#ifndef SHDATATYPE_HPP
#define SHDATATYPE_HPP

#include "ShVariableType.hpp"
#include "ShInterval.hpp"

/** @file ShDataType.hpp
 * Defines the host computation and memory storage c++ types associated with
 * each Sh value type.
 *
 * Also defines   
 */
namespace SH {

/// Used to denote the kinds of C++ data types associated with a Value type. 
enum ShDataType {
  SH_HOST, ///< computation data type on the host
  SH_MEM, ///< data type stored in memory on the host
  SH_DATATYPE_END
};

SH_DLLEXPORT
extern const char* dataTypeName[];

/// Sets the actual host computation and memory data types for a given Value type.
/// The default is computation and memory types equal the Value type.
/// Defined below are the exceptions to this rule
///
/// half floats - memory type is 1 sign bit, 5 exponent bits, 10 mantissa bits
///               see ATI_pixel_format_float for specification
///               (NV_half_float is similar, except it has NaN and INF)
///
// @{
template<ShValueType V, ShDataType DT>
struct ShDataTypeCppType;

#define SH_VALUETYPE_DATATYPE(V, hostType, memType)\
  template<> struct ShDataTypeCppType<V, SH_HOST> { typedef hostType type; }; \
  template<> struct ShDataTypeCppType<V, SH_MEM> { typedef memType type; }; 

SH_VALUETYPE_DATATYPE(SH_INTERVAL_DOUBLE, ShInterval<double>, ShInterval<double>);
SH_VALUETYPE_DATATYPE(SH_INTERVAL_FLOAT, ShInterval<float>, ShInterval<float>);

SH_VALUETYPE_DATATYPE(SH_DOUBLE, double, double);
SH_VALUETYPE_DATATYPE(SH_FLOAT, float, float);
SH_VALUETYPE_DATATYPE(SH_HALF, float, unsigned short);

SH_VALUETYPE_DATATYPE(SH_INT, int, int);
SH_VALUETYPE_DATATYPE(SH_SHORT, short, short);
SH_VALUETYPE_DATATYPE(SH_BYTE, char, char);
SH_VALUETYPE_DATATYPE(SH_UINT, unsigned int, unsigned int);
SH_VALUETYPE_DATATYPE(SH_USHORT, unsigned short, unsigned short);
SH_VALUETYPE_DATATYPE(SH_UBYTE, unsigned char, unsigned char);

SH_VALUETYPE_DATATYPE(SH_FRAC_INT, float, int);
SH_VALUETYPE_DATATYPE(SH_FRAC_SHORT, float, short);
SH_VALUETYPE_DATATYPE(SH_FRAC_BYTE, float, char);
SH_VALUETYPE_DATATYPE(SH_FRAC_UINT, float, unsigned int);
SH_VALUETYPE_DATATYPE(SH_FRAC_USHORT, float, unsigned short);
SH_VALUETYPE_DATATYPE(SH_FRAC_UBYTE, float, unsigned char);

template<ShValueType V> struct ShHostType { typedef typename ShDataTypeCppType<V, SH_HOST>::type type; };
template<ShValueType V> struct ShMemType { typedef typename ShDataTypeCppType<V, SH_MEM>::type type; };
// @}

/// Sets the constant values for a given data type.
// Currently only the additive and multiplicative inverses are here.
// And with all current types, Zero is a false value and One is a true value
// (although usually not the only ones).
template<ShValueType V, ShDataType DT> 
struct ShDataTypeConstant {
    typedef typename ShDataTypeCppType<V, DT>::type type;
    static const type Zero; /* additive identity and also a true value */ \
    static const type One; /* multiplicative identity also a false value */ \
};

template<ShValueType V, ShDataType DT>
const typename ShDataTypeCppType<V, DT>::type ShDataTypeConstant<V, DT>::Zero = 0; 

template<ShValueType V, ShDataType DT>
const typename ShDataTypeCppType<V, DT>::type ShDataTypeConstant<V, DT>::One = 1; 

/** Special cases
 * @{  */
#define SH_DTC_SPEC(V, DT) \
template<> struct ShDataTypeConstant<V, DT> { \
    typedef ShDataTypeCppType<V, DT>::type type; \
    static const type Zero; /* additive identity and also a true value */ \
    static const type One; /* multiplicative identity also a false value */ \
};

SH_DTC_SPEC(SH_HALF, SH_MEM);
SH_DTC_SPEC(SH_FRAC_INT, SH_MEM);
SH_DTC_SPEC(SH_FRAC_SHORT, SH_MEM);
SH_DTC_SPEC(SH_FRAC_BYTE, SH_MEM);
SH_DTC_SPEC(SH_FRAC_UINT, SH_MEM);
SH_DTC_SPEC(SH_FRAC_USHORT, SH_MEM);
SH_DTC_SPEC(SH_FRAC_UBYTE, SH_MEM);
// @}

/** Returns the boolean cond in the requested data type */
template<ShValueType V, ShDataType DT>
inline
typename ShDataTypeCppType<V, DT>::type shDataTypeCond(bool cond);

/** Returns a whether the two values are exactly the same.
 * This is is useful for the range types.
 * @{
 */
template<typename T>
inline
bool shDataTypeEqual(const T &a, const T &b);
// @}

/** Returns whether the value is always greater than zero (i.e. true) 
 * @{
 */
template<typename T>
inline
bool shDataTypeIsPositive(const T &a);

//@}

/** Casts one data type to another data type 
 *@{ */
template<ShValueType V1, ShDataType DT1, ShValueType V2, ShDataType DT2>
void shDataTypeCast(typename ShDataTypeCppType<V1, DT1>::type &dest,
                    const typename ShDataTypeCppType<V2, DT2>::type &src);

}

#include "ShDataTypeImpl.hpp"

#endif
