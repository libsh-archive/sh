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

#include "ShRefCount.hpp"
#include "ShCloakFactory.hpp"
#include "ShVariableNode.hpp"

namespace SH {

/// forward declaration of ShEval
class ShEval;

struct ShTypeInfo: ShRefCountable {
  virtual const char* name() const = 0;
  virtual ShCloakFactoryCPtr cloakFactory() = 0; 
  virtual ShPointer<const ShEval> eval()= 0;
};

typedef ShPointer<ShTypeInfo> ShTypeInfoPtr;
typedef ShPointer<const ShTypeInfo> ShTypeInfoCPtr;

// generic level
template<typename T>
struct ShConcreteTypeInfo: public ShTypeInfo  {
  public:
    static const char* m_name; 

    /// default boolean values to use for ops with boolean resoluts
    static const T trueValue;
    static const T falseValue;

    /// default range information
    // (0,1) by default
    static T defaultLo(ShSemanticType type);
    static T defaultHi(ShSemanticType type);

    const char* name() const; 
    ShCloakFactoryCPtr cloakFactory();
    ShPointer<const ShEval> eval();

  protected:
    static ShPointer<ShDataCloakFactory<T> > m_cloakFactory;
    static ShPointer<ShEval > m_eval;
};


}

#include "ShTypeInfoImpl.hpp"

#endif
