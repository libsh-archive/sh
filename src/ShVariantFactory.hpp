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
#include "ShTypeInfo.hpp"

namespace SH {


struct 
SH_DLLEXPORT 
ShVariantFactory {
  virtual ~ShVariantFactory() {}

  /// Functions that generate host storage type variants  
  //
  // @see ShHostType 
  //@{
  
  /// Creates a ShDataVariant object with N components 
  virtual ShVariantPtr generate(int N) const = 0; 

  /// Creates a ShDataVariant object by using the 
  // decode method from the Variant type corresponding
  // to this factory
  virtual ShVariantPtr generate(std::string s) const = 0;

  /// Creates an ShDataVariant object with the existing
  /// array as data
  /// @param managed Set to true iff this should make a copy
  //                 rather than using the given array internally.
  virtual ShVariantPtr generate(void *data, int N, bool managed = true) const = 0;  

  /// Creates ShDataVariant objects with N default low
  // range values for the given ShSemanticType
  virtual ShVariantPtr generateLowBound(int N, ShSemanticType type) const = 0; 
  virtual ShVariantPtr generateHighBound(int N, ShSemanticType type) const = 0; 

  /// Creates an ShDataVariant object with N elements set to zero.
  virtual ShVariantPtr generateZero(int N = 1) const = 0;

  /// Creates an ShDataVariant object with N elements set to one. 
  virtual ShVariantPtr generateOne(int N = 1) const = 0;

  /// Creates an ShDataVariant object with N elements set to true 
  virtual ShVariantPtr generateTrue(int N = 1) const = 0;

  /// Creates an ShDataVariant object with N elements set to false 
  virtual ShVariantPtr generateFalse(int N = 1) const = 0;
 
  // @}

  /// Functions that generate memory storage type variants
  //
  // @see ShMemoryType 
  //@{
  
  /// Creates a ShDataVariant object with N components 
  //
  virtual ShVariantPtr generateMemory(int N) const = 0;

  /// Creates an ShDataVariant object with the existing
  /// array as data
  /// @param managed Set to true iff this should make a copy
  //                 rather than using the given array internally.
  virtual ShVariantPtr generateMemory(void *data, int N, bool managed = true) const = 0;  

  //@}
};

template<typename T>
struct ShDataVariantFactory: public ShVariantFactory {
  typedef typename ShConcreteTypeInfo<T>::H H; // host storage type 
  typedef typename ShConcreteTypeInfo<T>::M M; // memory storage type

  ShVariantPtr generate(int N) const;

  ShVariantPtr generate(std::string s) const; 

  ShVariantPtr generate(void *data, int N, bool managed = true) const;  

  ShVariantPtr generateLowBound(int N, ShSemanticType type) const; 
  ShVariantPtr generateHighBound(int N, ShSemanticType type) const; 

  ShVariantPtr generateZero(int N = 1) const;
  ShVariantPtr generateOne(int N = 1) const;
  ShVariantPtr generateTrue(int N = 1) const;
  ShVariantPtr generateFalse(int N = 1) const;

  ShVariantPtr generateMemory(int N) const;
  ShVariantPtr generateMemory(void *data, int N, bool managed = true) const;  

  static const ShDataVariantFactory<T>* instance();

  protected:
    static ShDataVariantFactory<T> *m_instance;

    ShDataVariantFactory();
};


}

#include "ShVariantFactoryImpl.hpp"

#endif
