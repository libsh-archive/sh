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
#ifndef SHVARIABLETYPE_HPP
#define SHVARIABLETYPE_HPP

#include "DllExport.hpp"

/** @file VariableType.hpp
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
 * @see TypeInfo.hpp
 */

namespace SH {

/** The various ways variables can be bound.
 */
enum BindingType {
  INPUT, 
  OUTPUT, 
  INOUT,
  TEMP,
  CONST, 
  TEXTURE, 
  STREAM, 
  PALETTE, 

  BINDINGTYPE_END
};

DLLEXPORT extern const char* bindingTypeName[];

/** The various ways semantic types for variables.
 */
enum SemanticType {
  ATTRIB,
  POINT,
  VECTOR,
  NORMAL,
  COLOR,
  TEXCOORD,
  POSITION,
  PLANE,
  
  SEMANTICTYPE_END
};

DLLEXPORT extern const char* semanticTypeName[];


}
#endif
