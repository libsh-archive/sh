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
#ifndef SHVARIABLETYPE_HPP
#define SHVARIABLETYPE_HPP

#include "ShDllExport.hpp"

/** @file ShVariableType.hpp
 * 
 * Lists the binding, semantic, and Value types available in Sh
 * and their associated C++ data types for host computation and 
 * holding in memory.  Each enum is zero-indexed. 
 * 
 * For some Value types, the host data type and memory data types are
 * different since the first must be efficient in host computation and the
 * second needs to satisfy space constraints and special data formats used in some
 * backends. 
 * 
 * For more information on operations dealing with the data types such as
 * allocation of arrays, automatic promotions, and casting.
 * @see ShTypeInfo.hpp
 */

namespace SH {

/** The various ways variables can be bound.
 */
enum ShBindingType {
  SH_INPUT, 
  SH_OUTPUT, 
  SH_INOUT,
  SH_TEMP,
  SH_CONST, 
  SH_TEXTURE, 
  SH_STREAM, 
  SH_PALETTE, 
};

SH_DLLEXPORT extern const char* bindingTypeName[];

/** The various ways semantic types for variables.
 */
enum ShSemanticType {
  SH_ATTRIB,
  SH_POINT,
  SH_VECTOR,
  SH_NORMAL,
  SH_COLOR,
  SH_TEXCOORD,
  SH_POSITION,
};

SH_DLLEXPORT extern const char* semanticTypeName[];

/**  The various variable value types.
 */
enum ShValueType {
  SH_INTERVAL_DOUBLE,
  SH_INTERVAL_FLOAT,
  SH_DOUBLE,
  SH_FLOAT,
  SH_HALF,

  SH_INT,
  SH_SHORT,
  SH_BYTE,
  SH_UINT,
  SH_USHORT,
  SH_UBYTE,

  SH_FRAC_INT,
  SH_FRAC_SHORT,
  SH_FRAC_BYTE,
  SH_FRAC_UINT,
  SH_FRAC_USHORT,
  SH_FRAC_UBYTE,

  SH_VALUETYPE_END ///< only denotes end of list/number of Value type 
};

SH_DLLEXPORT extern const char* valueTypeName[]; 

/** type holds the interval value type that uses the given value type V 
 * for its bounds. or SH_VALUETYPE_END if no such interval type exists.
 */
template<ShValueType V>
struct ShIntervalValueType 
{
  static const ShValueType type = (SH_DOUBLE <= V) && (V <= SH_FLOAT) ? (ShValueType)(V - 2) : SH_VALUETYPE_END;
};

/** type holds the the regular type used as a bounds for the interval type V 
 * or SH_VALUETYPE_END if V is not an interval type */
template<ShValueType V>
struct ShRegularValueType
{
  static const ShValueType type = (SH_INTERVAL_DOUBLE <= V) && (V <= SH_INTERVAL_FLOAT) ? (ShValueType)((int)V + 2) : SH_VALUETYPE_END;
};

inline
bool shIsIntervalValueType(ShValueType valueType)
{
  return (SH_INTERVAL_DOUBLE <= valueType) && (valueType <= SH_INTERVAL_FLOAT); 
}

inline
ShValueType shIntervalValueType(ShValueType valueType) 
{
  if(shIsIntervalValueType(valueType)) {
    return (ShValueType)((int)valueType - 2); 
  }
  return SH_VALUETYPE_END;
}

inline
ShValueType shRegularValueType(ShValueType valueType) 
{
  if(SH_INTERVAL_DOUBLE <= valueType && valueType <= SH_INTERVAL_FLOAT) {
    return (ShValueType)((int)valueType + 2); 
  }
  return SH_VALUETYPE_END;
}



}
#endif
