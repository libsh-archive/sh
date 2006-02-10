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
#ifndef SHTYPEINFO_HPP
#define SHTYPEINFO_HPP

#include <string>
#include <vector>
#include "ShHashMap.hpp"
#include "ShVariableType.hpp"
#include "ShDataType.hpp"
#include "ShRefCount.hpp"
#include "ShFraction.hpp"
#include "ShHalf.hpp"

namespace SH {

/// forward declarations 
struct ShVariantFactory;


/** A holder of information about a data type and how to allocate it 
 * @see ShDataType.hpp
 * */ 
struct 
SH_DLLEXPORT
ShTypeInfo {
  virtual ~ShTypeInfo() {}

  /** Returns a the name of the value type */
  virtual const char* name() const = 0;

  /** Returns size of type */ 
  virtual int datasize() const = 0;

  /** Returns the factory that generates ShVariant objects of this type */
  virtual const ShVariantFactory* variantFactory() const = 0; 

  /** Initializes the variant factories, automatic promotions, and
   * other variant casters.
   */
  static void init();

  /** Returns the type info with the requested value and data types. */ 
  static const ShTypeInfo* get(ShValueType valueType, ShDataType dataType);

  typedef ShPairHashMap<ShValueType, ShDataType, const ShTypeInfo*> TypeInfoMap;
  private:
    /** Holds ShDataTypeInfo instances for all available valuetype/datatypes */
    static TypeInfoMap* m_valueTypes;

    /** Adds automatic promotion and other casts into the ShCastManager */ 
    static void addCasts();

    /** Adds ops to the ShEval class */ 
    static void addOps();
};

// generic level, singleton ShTypeInfo class holding information for
// a particular type
template<typename T, ShDataType DT>
struct ShDataTypeInfo: public ShTypeInfo {
  public:
    typedef typename ShDataTypeCppType<T, DT>::type type;
    static const type Zero;
    static const type One;

    const char* name() const; 
    int datasize() const;
    const ShVariantFactory* variantFactory() const;

    static const ShDataTypeInfo* instance();

  protected:
    static ShDataTypeInfo *m_instance;
    ShDataTypeInfo() {}
};


SH_DLLEXPORT
extern const ShTypeInfo* shTypeInfo(ShValueType valueType, ShDataType dataType = SH_HOST);

SH_DLLEXPORT
extern const ShVariantFactory* shVariantFactory(ShValueType valueType, ShDataType dataType = SH_HOST);

SH_DLLEXPORT
extern const char* shValueTypeName(ShValueType valueType);
}

#include "ShTypeInfoImpl.hpp"

#endif
