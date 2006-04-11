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
#include "DllExport.hpp"
#include "RefCount.hpp"
#include "TypeInfo.hpp"

namespace SH {

class Variant;

struct 
SH_DLLEXPORT 
VariantFactory {
  virtual ~VariantFactory() {}

  /// Creates a DataVariant object with N components 
  virtual Variant* generate(int N) const = 0; 

  /// Creates a DataVariant object by using the 
  // decode method from the Variant type corresponding
  // to this factory
  virtual Variant* generate(std::string s) const = 0;

  /// Creates an DataVariant object with the existing
  /// array as data
  /// @param managed Set to true iff this should make a copy
  //                 rather than using the given array internally.
  virtual Variant* generate(int N, void *data, bool managed = true) const = 0;  

  /// Creates an DataVariant object with N elements set to zero.
  virtual Variant* generateZero(int N = 1) const = 0;

  /// Creates an DataVariant object with N elements set to one. 
  virtual Variant* generateOne(int N = 1) const = 0;
};

template<typename T, DataType DT>
struct DataVariantFactory: public VariantFactory {
  Variant* generate(int N) const;

  Variant* generate(std::string s) const; 

  Variant* generate(int N, void *data, bool managed = true) const;  

  Variant* generateZero(int N = 1) const;
  Variant* generateOne(int N = 1) const;

  static const DataVariantFactory* instance();

  protected:
    static DataVariantFactory *m_instance;

    DataVariantFactory();
};


}

#include "VariantFactoryImpl.hpp"

#endif
