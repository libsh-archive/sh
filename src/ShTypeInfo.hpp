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
#ifndef SHTYPEINFO_HPP
#define SHTYPEINFO_HPP

#include <string>
#include <vector>
#include "ShVariableType.hpp"
#include "ShDataType.hpp"
#include "ShRefCount.hpp"
#include "ShInterval.hpp"

namespace SH {

/// forward declarations 
class ShVariantFactory;


/** A holder of information about a type and how to allocate it */ 
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

  private:
    /** Holds ShDataTypeInfo instances for all available valuetype/datatypes */
    static const ShTypeInfo* m_valueTypes[SH_VALUETYPE_END][SH_DATATYPE_END]; 

    /** Adds automatic promotion and other casts into the ShCastManager */ 
    static void addCasts();

    /** Adds ops to the ShEval class */ 
    static void addOps();
};

// generic level, singleton ShTypeInfo class holding information for
// a particular type
template<ShValueType V, ShDataType DT>
struct ShDataTypeInfo: public ShTypeInfo {
  public:
    typedef typename ShDataTypeCppType<V, DT>::type type;
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

/// Returns the number of storage types
// This means that type indices 0 through result - 1 are all
// occupied.
//SH_DLLEXPORT
//int shNumTypes();

SH_DLLEXPORT
extern const ShTypeInfo* shTypeInfo(ShValueType valueType, ShDataType dataType = SH_HOST);

SH_DLLEXPORT
extern const ShVariantFactory* shVariantFactory(ShValueType valueType, ShDataType dataType = SH_HOST);

// Provides a least common ancestor in the type tree for 
// a given pair of types in a typedef named type 
template<ShValueType V1, ShValueType V2>
struct ShCommonType;


template<ShValueType V1, ShValueType V2, ShValueType V3>
struct ShCommonType3 {
  static const ShValueType valueType = 
    ShCommonType<ShCommonType<V1, V2>::valueType, V3>::valueType;
};

template<ShValueType V1, ShValueType V2, ShValueType V3, ShValueType V4>
struct ShCommonType4 {
  static const ShValueType valueType = 
    ShCommonType<ShCommonType<T1, T2>::valueType, 
      ShCommonType<T3, T4>::valueType>::valueType; 
};

}

#include "ShTypeInfoImpl.hpp"

#endif
