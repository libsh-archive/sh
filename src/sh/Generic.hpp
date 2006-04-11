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
#ifndef SHGENERIC_HPP
#define SHGENERIC_HPP

#include "VariableType.hpp"
#include "DataType.hpp"
#include "Variable.hpp"
#include "Variant.hpp"

namespace SH {

class Program;

/** A variable of length N.
 *
 * This class is provided to make definition of functions that work
 * over n-tuples of particular types easier.
 *
 * Attrib derives from Generic. Unlike Generic, which only has
 * two template parameters, Attrib has four template
 * parameters. This would make writing functions quite ugly. E.g.:
 *
 * Without Generic:
 *
 * template<int N, typename T, BindingType B1, BindingType B2,
 *          bool S1, bool S2>
 * Attrib<N, TEMP, T> add(const Attrib<N, B1, T, S1>& a,
 *                             const Attrib<N, B2, T, S2>& b);
 *
 * With Generic:
 *
 * template<int N, typename T>
 * Attrib<N, TEMP, T> add(const Generic<N, T>& a,
 *                             const Generic<N, T>& b);
 *
 * This class is explicitly instantiated for T = float with 1 <= N <= 4.
 */
template<int N, typename T>
class Generic : public Variable 
{
public:
  typedef T storage_type;
  static const ValueType value_type = StorageTypeInfo<T>::value_type;
  typedef typename HostType<T>::type host_type; 
  typedef typename MemType<T>::type mem_type; 
  static const int typesize = N;

  typedef DataVariant<T, HOST> VariantType; 
  typedef Pointer<VariantType> VariantTypePtr;
  typedef Pointer<const VariantType> VariantTypeCPtr;


  Generic(const VariableNodePtr& node, Swizzle swizzle, bool neg);
  ~Generic();

  // Copy constructor 
  // This should only be used internally.
  // It generates a VariableNode of type TEMP 
  // @{

  // @todo type get rid of this. default should be okay for 
  // internal usage
  // Generic(const Generic<N, T> &other);

  template<typename T2>
  Generic(const Generic<N, T2> &other);
  // @}

  // This is needed because the templated assignment op is 
  // non-default, hence C++ spec 12.8.10 says the default
  // is implicitly defined.  Here that doesn't work so well. 
  Generic& operator=(const Generic& other);

  template<typename T2>
  Generic& operator=(const Generic<N, T2>& other);

  Generic& operator=(const Program& other);

  Generic& operator++();
  Generic& operator--();
  
  template<typename T2>
  Generic& operator+=(const Generic<N, T2>& right);

  template<typename T2>
  Generic& operator-=(const Generic<N, T2>& right);

  template<typename T2>
  Generic& operator*=(const Generic<N, T2>& right);

  template<typename T2>
  Generic& operator/=(const Generic<N, T2>& right);

  template<typename T2>
  Generic& operator%=(const Generic<N, T2>& right);

  template<typename T2>
  Generic& operator+=(const Generic<1, T2>& right);

  template<typename T2>
  Generic& operator-=(const Generic<1, T2>& right);

  template<typename T2>
  Generic& operator*=(const Generic<1, T2>& right);

  template<typename T2>
  Generic& operator/=(const Generic<1, T2>& right);

  template<typename T2>
  Generic& operator%=(const Generic<1, T2>& right);

  Generic& operator+=(host_type);
  Generic& operator-=(host_type);
  Generic& operator*=(host_type);
  Generic& operator/=(host_type);
  Generic& operator%=(host_type);

  Generic operator-() const;

  Generic operator()() const; ///< Identity swizzle
  Generic<1, T> operator()(int) const;
  Generic<1, T> operator[](int) const;
  Generic<2, T> operator()(int, int) const;
  Generic<3, T> operator()(int, int, int) const;
  Generic<4, T> operator()(int, int, int, int) const;

  /// Range Metadata
  void range(host_type low, host_type high);

  VariantType lowBound() const; 
  host_type lowBound(int index) const;

  VariantType highBound() const;
  host_type highBound(int index) const;
  
  // Arbitrary Swizzle
  template<int N2>
  Generic<N2, T> swiz(int indices[]) const;

  /// Get the values of this variable, with swizzling taken into account
  void getValues(host_type dest[]) const;
  host_type getValue(int index) const;
  
  /// Set the values of this variable, using the swizzle as a
  /// writemask.
  void setValue(int index, const host_type &value); 
  void setValues(const host_type values[]);


protected:
  Generic(const VariableNodePtr& node);

};

template<typename T>
class Generic<1, T> : public Variable 
{
public:
  typedef T storage_type;
  static const ValueType value_type = StorageTypeInfo<T>::value_type;
  typedef typename HostType<T>::type host_type; 
  typedef typename MemType<T>::type mem_type; 
  static const int typesize = 1;

  typedef DataVariant<T, HOST> VariantType; 
  typedef Pointer<VariantType> VariantTypePtr;
  typedef Pointer<const VariantType> VariantTypeCPtr;


  Generic(const VariableNodePtr& node, Swizzle swizzle, bool neg);
  ~Generic();

  // Copy constructor 
  // This should only be used internally.  It generates a TEMP, 
  // ATTRIB, with the only characteristic copied from other being
  // the storage type.
  // @{

  // @todo type get rid of this
  // Generic(const Generic<1, T> &other);

  template<typename T2>
  Generic(const Generic<1, T2> &other);
  // @}

  Generic& operator=(const Generic<1, T>& other);

  template<typename T2>
  Generic& operator=(const Generic<1, T2>& other);

  Generic& operator=(host_type);
  Generic& operator=(const Program& other);
  
  Generic& operator++();
  Generic& operator--();

  template<typename T2>
  Generic& operator+=(const Generic<1, T2>& right);

  template<typename T2>
  Generic& operator-=(const Generic<1, T2>& right);

  template<typename T2>
  Generic& operator*=(const Generic<1, T2>& right);

  template<typename T2>
  Generic& operator/=(const Generic<1, T2>& right);

  template<typename T2>
  Generic& operator%=(const Generic<1, T2>& right);

  Generic& operator+=(host_type);
  Generic& operator-=(host_type);
  Generic& operator*=(host_type);
  Generic& operator/=(host_type);
  Generic& operator%=(host_type);

  Generic operator-() const;

  Generic operator()() const; ///< Identity swizzle
  Generic<1, T> operator()(int) const;
  Generic<1, T> operator[](int) const;
  Generic<2, T> operator()(int, int) const;
  Generic<3, T> operator()(int, int, int) const;
  Generic<4, T> operator()(int, int, int, int) const;

  /// Range Metadata
  void range(host_type low, host_type high);

  VariantType lowBound() const; 
  host_type lowBound(int index) const;

  VariantType highBound() const;
  host_type highBound(int index) const;
  
  // Arbitrary Swizzle
  template<int N2>
  Generic<N2, T> swiz(int indices[]) const;

  /// Get the values of this variable, with swizzling taken into account
  void getValues(host_type dest[]) const;
  host_type getValue(int index) const;
  
  /// Set the values of this variable, using the swizzle as a
  /// writemask.
  void setValue(int index, const host_type &value); 
  void setValues(const host_type values[]);

protected:
  Generic(const VariableNodePtr& node);

};

typedef Generic<1, FracUShort> Generic1fus;
typedef Generic<2, FracUShort> Generic2fus;
typedef Generic<3, FracUShort> Generic3fus;
typedef Generic<4, FracUShort> Generic4fus;

typedef Generic<1, short> Generic1s;
typedef Generic<2, short> Generic2s;
typedef Generic<3, short> Generic3s;
typedef Generic<4, short> Generic4s;

typedef Generic<1, FracUInt> Generic1fui;
typedef Generic<2, FracUInt> Generic2fui;
typedef Generic<3, FracUInt> Generic3fui;
typedef Generic<4, FracUInt> Generic4fui;

typedef Generic<1, FracByte> Generic1fb;
typedef Generic<2, FracByte> Generic2fb;
typedef Generic<3, FracByte> Generic3fb;
typedef Generic<4, FracByte> Generic4fb;

typedef Generic<1, int> Generic1i;
typedef Generic<2, int> Generic2i;
typedef Generic<3, int> Generic3i;
typedef Generic<4, int> Generic4i;

typedef Generic<1, double> Generic1d;
typedef Generic<2, double> Generic2d;
typedef Generic<3, double> Generic3d;
typedef Generic<4, double> Generic4d;

typedef Generic<1, unsigned char> Generic1ub;
typedef Generic<2, unsigned char> Generic2ub;
typedef Generic<3, unsigned char> Generic3ub;
typedef Generic<4, unsigned char> Generic4ub;

typedef Generic<1, float> Generic1f;
typedef Generic<2, float> Generic2f;
typedef Generic<3, float> Generic3f;
typedef Generic<4, float> Generic4f;

typedef Generic<1, char> Generic1b;
typedef Generic<2, char> Generic2b;
typedef Generic<3, char> Generic3b;
typedef Generic<4, char> Generic4b;

typedef Generic<1, unsigned short> Generic1us;
typedef Generic<2, unsigned short> Generic2us;
typedef Generic<3, unsigned short> Generic3us;
typedef Generic<4, unsigned short> Generic4us;

typedef Generic<1, FracUByte> Generic1fub;
typedef Generic<2, FracUByte> Generic2fub;
typedef Generic<3, FracUByte> Generic3fub;
typedef Generic<4, FracUByte> Generic4fub;

typedef Generic<1, Half> Generic1h;
typedef Generic<2, Half> Generic2h;
typedef Generic<3, Half> Generic3h;
typedef Generic<4, Half> Generic4h;

typedef Generic<1, FracShort> Generic1fs;
typedef Generic<2, FracShort> Generic2fs;
typedef Generic<3, FracShort> Generic3fs;
typedef Generic<4, FracShort> Generic4fs;

typedef Generic<1, FracInt> Generic1fi;
typedef Generic<2, FracInt> Generic2fi;
typedef Generic<3, FracInt> Generic3fi;
typedef Generic<4, FracInt> Generic4fi;

typedef Generic<1, unsigned int> Generic1ui;
typedef Generic<2, unsigned int> Generic2ui;
typedef Generic<3, unsigned int> Generic3ui;
typedef Generic<4, unsigned int> Generic4ui;

}

// This is a hack for Attrib.hpp in particular.
// A little dirty, but it works well.
#ifndef SH_DO_NOT_INCLUDE_GENERIC_IMPL
#include "GenericImpl.hpp"
#endif

#endif
