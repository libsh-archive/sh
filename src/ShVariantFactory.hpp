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
#ifndef SHVARIANTFACTORY_HPP
#define SHVARIANTFACTORY_HPP

#include <string>
#include "ShDllExport.hpp"
#include "ShVariant.hpp"

namespace SH {


struct 
SH_DLLEXPORT ShVariantFactory: public ShRefCountable {
  /// Creates a ShVariant object with N components 
  virtual ShVariantPtr generate(int N) const = 0; 

  /// Creates a ShVariant object by using the 
  // decode method from the Variant type corresponding
  // to this factory
  virtual ShVariantPtr generate(std::string s) const = 0;

  /// Creates two ShVariants object with N default low
  // range values for the given ShSemanticType
  virtual ShVariantPtr generateLowBound(int N, ShSemanticType type) const = 0; 
  virtual ShVariantPtr generateHighBound(int N, ShSemanticType type) const = 0; 
};

typedef ShPointer<ShVariantFactory> ShVariantFactoryPtr;
typedef ShPointer<const ShVariantFactory> ShVariantFactoryCPtr;

template<typename T>
struct 
SH_DLLEXPORT ShDataVariantFactory: public ShVariantFactory {
  ShVariantPtr generate(int N) const;

  /// generates a ShVariant by using  
  ShVariantPtr generate(std::string s) const; 

  ShVariantPtr generateLowBound(int N, ShSemanticType type) const; 
  ShVariantPtr generateHighBound(int N, ShSemanticType type) const; 
};


}

#include "ShVariantFactoryImpl.hpp"

#endif
