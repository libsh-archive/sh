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
#include "ShRefCount.hpp"

namespace SH {

/// forward declarations 
class ShVariantFactory;

/// empty structs for non-native host types (half-float, fracs)
/// @{
struct ShHalf {};
struct ShFracInt {};
struct ShFracShort {};
struct ShFracByte {};
struct ShFracUInt {};
struct ShFracUShort {};
struct ShFracUByte {};
// @}

/// Sets the actual host computation and memory storage types for a given type.
// @{
template<typename T> struct ShHostType { typedef T type; };
template<> struct ShHostType<ShHalf> { typedef float type; };
template<> struct ShHostType<ShFracInt> { typedef float type; };
template<> struct ShHostType<ShFracShort> { typedef float type; };
template<> struct ShHostType<ShFracByte> { typedef float type; };
template<> struct ShHostType<ShFracUInt> { typedef float type; };
template<> struct ShHostType<ShFracUShort> { typedef float type; };
template<> struct ShHostType<ShFracUByte> { typedef float type; };

template<typename T> struct ShMemoryType { typedef T type; };
template<> struct ShMemoryType<ShHalf> { typedef float type; };
template<> struct ShMemoryType<ShFracInt> { typedef float type; };
template<> struct ShMemoryType<ShFracShort> { typedef float type; };
template<> struct ShMemoryType<ShFracByte> { typedef float type; };
template<> struct ShMemoryType<ShFracUInt> { typedef float type; };
template<> struct ShMemoryType<ShFracUShort> { typedef float type; };
template<> struct ShMemoryType<ShFracUByte> { typedef float type; };
// @}


struct 
SH_DLLEXPORT
ShTypeInfo {
  virtual ~ShTypeInfo() {}

  /** Returns a unique string representation for this type */
  virtual const char* name() const = 0;

  /** Returns size of type when stored in host memory */ 
  virtual int datasize() const = 0;

  /** Returns the factory that generates ShVariant objects of this type */
  virtual const ShVariantFactory* variantFactory() const = 0; 
};

// generic level, singleton ShTypeInfo class holding information for
// a particular type
template<typename T>
struct ShConcreteTypeInfo: public ShTypeInfo {
  public:
    typedef typename ShHostType<T>::type H; 
    typedef typename ShMemoryType<T>::type M;

    static const char* m_name; 

    /// default boolean values to use for ops with boolean results
    static const H TrueVal;
    static const H FalseVal;

    /// default values for additive/multiplicative identities 
    static const H ZERO;
    static const H ONE;

    /// default range information
    // (0,1) by default
    static H defaultLo(ShSemanticType type);
    static H defaultHi(ShSemanticType type);

    /// Equality comparison that returns true iff
    // the values are equal. 
    static bool valuesEqual(const H &a, const H &b); 

    const char* name() const; 
    int datasize() const;
    const ShVariantFactory* variantFactory() const;

    static const ShConcreteTypeInfo<T>* instance();

  protected:
    static ShConcreteTypeInfo<T> *m_instance;

    ShConcreteTypeInfo();
};

//default initialization (adds concrete type infos and creates 
// eval providers for core Sh types
SH_DLLEXPORT
void shTypeInfoInit(); 

/// Returns the number of storage types
// This means that type indices 0 through result - 1 are all
// occupied.
SH_DLLEXPORT
int shNumTypes();

SH_DLLEXPORT
const ShTypeInfo* shTypeInfo(int typeIndex);  

/// Returns the type index of type T in the current context
template<typename T>
int shTypeIndex();

// Given a type T, returns ShConcreteTypeInfo<T>::TrueVal if the arg is true
// ::FalseVal otherwise
template<typename T>
typename ShConcreteTypeInfo<T>::H shTypeInfoCond(bool cond);

// Provides a least common ancestor in the type tree for 
// a given pair of types in a typedef named type 
template<typename T1, typename T2>
struct ShCommonType; 

template<typename T1, typename T2, typename T3>
struct ShCommonType3 {
  typedef typename ShCommonType<typename ShCommonType<T1, T2>::type, T3>::type type;
};

template<typename T1, typename T2, typename T3, typename T4>
struct ShCommonType4 {
  typedef typename ShCommonType<typename ShCommonType<T1, T2>::type, 
                                typename ShCommonType<T3, T4>::type>::type type;
};


// TODO could make this into a tree traversal to reduce amount
// of code when we hav 12+ base types
//
// But template metapgrogramming that might also lead to very
// obfuscated code...

}

#include "ShTypeInfoImpl.hpp"

#endif
