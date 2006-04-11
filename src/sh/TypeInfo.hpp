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
#include "HashMap.hpp"
#include "VariableType.hpp"
#include "DataType.hpp"
#include "RefCount.hpp"
#include "Fraction.hpp"
#include "Half.hpp"

namespace SH {

/// forward declarations 
struct VariantFactory;


/** A holder of information about a data type and how to allocate it 
 * @see DataType.hpp
 * */ 
struct 
SH_DLLEXPORT
TypeInfo {
  virtual ~TypeInfo() {}

  /** Returns a the name of the value type */
  virtual const char* name() const = 0;

  /** Returns size of type */ 
  virtual int datasize() const = 0;

  /** Returns the factory that generates Variant objects of this type */
  virtual const VariantFactory* variantFactory() const = 0; 

  /** Initializes the variant factories, automatic promotions, and
   * other variant casters.
   */
  static void init();

  /** Returns the type info with the requested value and data types. */ 
  static const TypeInfo* get(ValueType valueType, DataType dataType);

  typedef PairHashMap<ValueType, DataType, const TypeInfo*> TypeInfoMap;
  private:
    /** Holds DataTypeInfo instances for all available valuetype/datatypes */
    static TypeInfoMap* m_valueTypes;

    /** Adds automatic promotion and other casts into the CastManager */ 
    static void addCasts();

    /** Adds ops to the Eval class */ 
    static void addOps();
};

// generic level, singleton TypeInfo class holding information for
// a particular type
template<typename T, DataType DT>
struct DataTypeInfo: public TypeInfo {
  public:
    typedef typename DataTypeCppType<T, DT>::type type;
    static const type Zero;
    static const type One;

    const char* name() const; 
    int datasize() const;
    const VariantFactory* variantFactory() const;

    static const DataTypeInfo* instance();

  protected:
    static DataTypeInfo *m_instance;
    DataTypeInfo() {}
};


SH_DLLEXPORT
extern const TypeInfo* typeInfo(ValueType valueType, DataType dataType = HOST);

SH_DLLEXPORT
extern const VariantFactory* variantFactory(ValueType valueType, DataType dataType = HOST);

SH_DLLEXPORT
extern const char* valueTypeName(ValueType valueType);
}

#include "TypeInfoImpl.hpp"

#endif
