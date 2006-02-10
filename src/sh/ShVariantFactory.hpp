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
#ifndef SHVARIANTFACTORY_HPP
#define SHVARIANTFACTORY_HPP

#include <string>
#include "ShDllExport.hpp"
#include "ShRefCount.hpp"
#include "ShTypeInfo.hpp"

namespace SH {

class ShVariant;

struct 
SH_DLLEXPORT 
ShVariantFactory {
  virtual ~ShVariantFactory() {}

  /// Creates a ShDataVariant object with N components 
  virtual ShVariant* generate(int N) const = 0; 

  /// Creates a ShDataVariant object by using the 
  // decode method from the Variant type corresponding
  // to this factory
  virtual ShVariant* generate(std::string s) const = 0;

  /// Creates an ShDataVariant object with the existing
  /// array as data
  /// @param managed Set to true iff this should make a copy
  //                 rather than using the given array internally.
  virtual ShVariant* generate(int N, void *data, bool managed = true) const = 0;  

  /// Creates an ShDataVariant object with N elements set to zero.
  virtual ShVariant* generateZero(int N = 1) const = 0;

  /// Creates an ShDataVariant object with N elements set to one. 
  virtual ShVariant* generateOne(int N = 1) const = 0;
};

template<typename T, ShDataType DT>
struct ShDataVariantFactory: public ShVariantFactory {
  ShVariant* generate(int N) const;

  ShVariant* generate(std::string s) const; 

  ShVariant* generate(int N, void *data, bool managed = true) const;  

  ShVariant* generateZero(int N = 1) const;
  ShVariant* generateOne(int N = 1) const;

  static const ShDataVariantFactory* instance();

  protected:
    static ShDataVariantFactory *m_instance;

    ShDataVariantFactory();
};


}

#include "ShVariantFactoryImpl.hpp"

#endif
