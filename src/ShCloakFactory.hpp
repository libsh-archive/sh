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
#ifndef SHCLOAKFACTORY_HPP
#define SHCLOAKFACTORY_HPP

#include <string>
#include "ShCloak.hpp"
#include "ShVariableNode.hpp"

namespace SH {


struct ShCloakFactory: public ShRefCountable {
  /// Creates a ShCloak object with N components 
  virtual ShCloakPtr generate(int N) const = 0; 

  /// Creates a ShCloak object by using the 
  // decode method from the Cloak type corresponding
  // to this factory
  virtual ShCloakPtr generate(std::string s) const = 0;

  /// Creates two ShCloaks object with N default low
  // range values for the given ShSemanticType
  virtual ShCloakPtr generateLowBound(int N, ShSemanticType type) const = 0; 
  virtual ShCloakPtr generateHighBound(int N, ShSemanticType type) const = 0; 
};

typedef ShPointer<ShCloakFactory> ShCloakFactoryPtr;
typedef ShPointer<const ShCloakFactory> ShCloakFactoryCPtr;

template<typename T>
struct ShDataCloakFactory: public ShCloakFactory {
  ShCloakPtr generate(int N) const;

  /// generates a ShCloak by using  
  ShCloakPtr generate(string s) const; 

  ShCloakPtr generateLowBound(int N, ShSemanticType type) const; 
  ShCloakPtr generateHighBound(int N, ShSemanticType type) const; 
};


}

#include "ShCloakFactoryImpl.hpp"

#endif
