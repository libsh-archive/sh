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

#include "ShVariableType.hpp"
#include "ShRefCount.hpp"

namespace SH {

/// forward declarations 
class ShCloakFactory;

struct ShTypeInfo: ShRefCountable {
  /** Returns a unique string representation for this type */
  virtual const char* name() const = 0;

  /** Returns the factory that generates ShCloak objects of this type */
  virtual ShPointer<const ShCloakFactory> cloakFactory() = 0; 
};

typedef ShPointer<ShTypeInfo> ShTypeInfoPtr;
typedef ShPointer<const ShTypeInfo> ShTypeInfoCPtr;

// generic level
template<typename T>
struct ShConcreteTypeInfo: public ShTypeInfo {
  public:
    static const char* m_name; 

    /// default boolean values to use for ops with boolean results
    static const T TrueVal;
    static const T FalseVal;

    /// default values for additive/multiplicative identities 
    static const T ZERO;
    static const T ONE;

    ShConcreteTypeInfo();

    /// default range information
    // (0,1) by default
    static T defaultLo(ShSemanticType type);
    static T defaultHi(ShSemanticType type);

    const char* name() const; 
    ShPointer<const ShCloakFactory> cloakFactory();

  protected:
    static ShPointer<ShCloakFactory> m_cloakFactory;
};

//default initialization (adds concrete type infos and creates 
// eval providers for core Sh types
void shTypeInfoInit(); 

// Given a type T, returns ShConcreteTypeInfo<T>::TrueVal if the arg is true
// ::FalseVal otherwise
template<typename T>
T shTypeInfoCond(bool cond);

// Provides a least common ancestor in the type tree for 
// a given pair of types in a typedef named type 
template<typename T1, typename T2>
struct ShCommonType; 

template<typename T1, typename T2, typename T3>
struct ShCommonType3 {
  typedef typename ShCommonType<typename ShCommonType<T1, T2>::type, T3>::type type;
};


// TODO could make this into a tree traversal to reduce amount
// of code when we hav 12+ base types
//
// But template metapgrogramming that might also lead to very
// obfuscated code...

}

#include "ShTypeInfoImpl.hpp"

#endif
