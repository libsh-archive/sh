// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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

#include "ShVariableType.hpp"
#include "ShDataType.hpp"
#include "ShVariable.hpp"
#include "ShVariant.hpp"

namespace SH {

class ShProgram;

/** A variable of length N.
 *
 * This class is provided to make definition of functions that work
 * over n-tuples of particular types easier.
 *
 * ShAttrib derives from ShGeneric. Unlike ShGeneric, which only has
 * two template parameters, ShAttrib has four template
 * parameters. This would make writing functions quite ugly. E.g.:
 *
 * Without Generic:
 *
 * template<int N, typename T, ShBindingType B1, ShBindingType B2,
 *          bool S1, bool S2>
 * ShAttrib<N, SH_TEMP, T> add(const ShAttrib<N, B1, T, S1>& a,
 *                             const ShAttrib<N, B2, T, S2>& b);
 *
 * With Generic:
 *
 * template<int N, typename T>
 * ShAttrib<N, SH_TEMP, T> add(const ShGeneric<N, T>& a,
 *                             const ShGeneric<N, T>& b);
 *
 * This class is explicitly instantiated for T = float with 1 <= N <= 4.
 */
template<int N, typename T>
class ShGeneric : public ShVariable 
{
public:
  typedef T storage_type;
  static const ShValueType value_type = ShStorageTypeInfo<T>::value_type;
  typedef typename ShHostType<T>::type host_type; 
  typedef typename ShMemType<T>::type mem_type; 
  static const int typesize = N;

  typedef ShDataVariant<T, SH_HOST> VariantType; 
  typedef ShPointer<VariantType> VariantTypePtr;
  typedef ShPointer<const VariantType> VariantTypeCPtr;


  ShGeneric(const ShVariableNodePtr& node, ShSwizzle swizzle, bool neg);
  ~ShGeneric();

  // Copy constructor 
  // This should only be used internally.
  // It generates a ShVariableNode of type SH_TEMP 
  // @{

  // @todo type get rid of this. default should be okay for 
  // internal usage
  // ShGeneric(const ShGeneric<N, T> &other);

  template<typename T2>
  ShGeneric(const ShGeneric<N, T2> &other);
  // @}

  // This is needed because the templated assignment op is 
  // non-default, hence C++ spec 12.8.10 says the default
  // is implicitly defined.  Here that doesn't work so well. 
  ShGeneric& operator=(const ShGeneric& other);

  template<typename T2>
  ShGeneric& operator=(const ShGeneric<N, T2>& other);

  ShGeneric& operator=(const ShProgram& other);

  ShGeneric& operator++();
  ShGeneric& operator--();
  
  template<typename T2>
  ShGeneric& operator+=(const ShGeneric<N, T2>& right);

  template<typename T2>
  ShGeneric& operator-=(const ShGeneric<N, T2>& right);

  template<typename T2>
  ShGeneric& operator*=(const ShGeneric<N, T2>& right);

  template<typename T2>
  ShGeneric& operator/=(const ShGeneric<N, T2>& right);

  template<typename T2>
  ShGeneric& operator%=(const ShGeneric<N, T2>& right);

  template<typename T2>
  ShGeneric& operator+=(const ShGeneric<1, T2>& right);

  template<typename T2>
  ShGeneric& operator-=(const ShGeneric<1, T2>& right);

  template<typename T2>
  ShGeneric& operator*=(const ShGeneric<1, T2>& right);

  template<typename T2>
  ShGeneric& operator/=(const ShGeneric<1, T2>& right);

  template<typename T2>
  ShGeneric& operator%=(const ShGeneric<1, T2>& right);

  ShGeneric& operator+=(host_type);
  ShGeneric& operator-=(host_type);
  ShGeneric& operator*=(host_type);
  ShGeneric& operator/=(host_type);
  ShGeneric& operator%=(host_type);

  ShGeneric operator-() const;

  ShGeneric operator()() const; ///< Identity swizzle
  ShGeneric<1, T> operator()(int) const;
  ShGeneric<1, T> operator[](int) const;
  ShGeneric<2, T> operator()(int, int) const;
  ShGeneric<3, T> operator()(int, int, int) const;
  ShGeneric<4, T> operator()(int, int, int, int) const;

  /// Range Metadata
  void range(host_type low, host_type high);

  VariantType lowBound() const; 
  host_type lowBound(int index) const;

  VariantType highBound() const;
  host_type highBound(int index) const;
  
  // Arbitrary Swizzle
  template<int N2>
  ShGeneric<N2, T> swiz(int indices[]) const;

  /// Get the values of this variable, with swizzling taken into account
  void getValues(host_type dest[]) const;
  host_type getValue(int index) const;
  
  /// Set the values of this variable, using the swizzle as a
  /// writemask.
  void setValue(int index, const host_type &value); 
  void setValues(const host_type values[]);


protected:
  ShGeneric(const ShVariableNodePtr& node);

};

template<typename T>
class ShGeneric<1, T> : public ShVariable 
{
public:
  typedef T storage_type;
  static const ShValueType value_type = ShStorageTypeInfo<T>::value_type;
  typedef typename ShHostType<T>::type host_type; 
  typedef typename ShMemType<T>::type mem_type; 
  static const int typesize = 1;

  typedef ShDataVariant<T, SH_HOST> VariantType; 
  typedef ShPointer<VariantType> VariantTypePtr;
  typedef ShPointer<const VariantType> VariantTypeCPtr;


  ShGeneric(const ShVariableNodePtr& node, ShSwizzle swizzle, bool neg);
  ~ShGeneric();

  // Copy constructor 
  // This should only be used internally.  It generates a SH_TEMP, 
  // SH_ATTRIB, with the only characteristic copied from other being
  // the storage type.
  // @{

  // @todo type get rid of this
  // ShGeneric(const ShGeneric<1, T> &other);

  template<typename T2>
  ShGeneric(const ShGeneric<1, T2> &other);
  // @}

  ShGeneric& operator=(const ShGeneric<1, T>& other);

  template<typename T2>
  ShGeneric& operator=(const ShGeneric<1, T2>& other);

  ShGeneric& operator=(host_type);
  ShGeneric& operator=(const ShProgram& other);
  
  ShGeneric& operator++();
  ShGeneric& operator--();

  template<typename T2>
  ShGeneric& operator+=(const ShGeneric<1, T2>& right);

  template<typename T2>
  ShGeneric& operator-=(const ShGeneric<1, T2>& right);

  template<typename T2>
  ShGeneric& operator*=(const ShGeneric<1, T2>& right);

  template<typename T2>
  ShGeneric& operator/=(const ShGeneric<1, T2>& right);

  template<typename T2>
  ShGeneric& operator%=(const ShGeneric<1, T2>& right);

  ShGeneric& operator+=(host_type);
  ShGeneric& operator-=(host_type);
  ShGeneric& operator*=(host_type);
  ShGeneric& operator/=(host_type);
  ShGeneric& operator%=(host_type);

  ShGeneric operator-() const;

  ShGeneric operator()() const; ///< Identity swizzle
  ShGeneric<1, T> operator()(int) const;
  ShGeneric<1, T> operator[](int) const;
  ShGeneric<2, T> operator()(int, int) const;
  ShGeneric<3, T> operator()(int, int, int) const;
  ShGeneric<4, T> operator()(int, int, int, int) const;

  /// Range Metadata
  void range(host_type low, host_type high);

  VariantType lowBound() const; 
  host_type lowBound(int index) const;

  VariantType highBound() const;
  host_type highBound(int index) const;
  
  // Arbitrary Swizzle
  template<int N2>
  ShGeneric<N2, T> swiz(int indices[]) const;

  /// Get the values of this variable, with swizzling taken into account
  void getValues(host_type dest[]) const;
  host_type getValue(int index) const;
  
  /// Set the values of this variable, using the swizzle as a
  /// writemask.
  void setValue(int index, const host_type &value); 
  void setValues(const host_type values[]);

protected:
  ShGeneric(const ShVariableNodePtr& node);

};

typedef ShGeneric<1, ShFracUShort> ShGeneric1fus;
typedef ShGeneric<2, ShFracUShort> ShGeneric2fus;
typedef ShGeneric<3, ShFracUShort> ShGeneric3fus;
typedef ShGeneric<4, ShFracUShort> ShGeneric4fus;

typedef ShGeneric<1, short> ShGeneric1s;
typedef ShGeneric<2, short> ShGeneric2s;
typedef ShGeneric<3, short> ShGeneric3s;
typedef ShGeneric<4, short> ShGeneric4s;

typedef ShGeneric<1, ShFracUInt> ShGeneric1fui;
typedef ShGeneric<2, ShFracUInt> ShGeneric2fui;
typedef ShGeneric<3, ShFracUInt> ShGeneric3fui;
typedef ShGeneric<4, ShFracUInt> ShGeneric4fui;

typedef ShGeneric<1, ShFracByte> ShGeneric1fb;
typedef ShGeneric<2, ShFracByte> ShGeneric2fb;
typedef ShGeneric<3, ShFracByte> ShGeneric3fb;
typedef ShGeneric<4, ShFracByte> ShGeneric4fb;

typedef ShGeneric<1, int> ShGeneric1i;
typedef ShGeneric<2, int> ShGeneric2i;
typedef ShGeneric<3, int> ShGeneric3i;
typedef ShGeneric<4, int> ShGeneric4i;

typedef ShGeneric<1, double> ShGeneric1d;
typedef ShGeneric<2, double> ShGeneric2d;
typedef ShGeneric<3, double> ShGeneric3d;
typedef ShGeneric<4, double> ShGeneric4d;

typedef ShGeneric<1, unsigned char> ShGeneric1ub;
typedef ShGeneric<2, unsigned char> ShGeneric2ub;
typedef ShGeneric<3, unsigned char> ShGeneric3ub;
typedef ShGeneric<4, unsigned char> ShGeneric4ub;

typedef ShGeneric<1, float> ShGeneric1f;
typedef ShGeneric<2, float> ShGeneric2f;
typedef ShGeneric<3, float> ShGeneric3f;
typedef ShGeneric<4, float> ShGeneric4f;

typedef ShGeneric<1, char> ShGeneric1b;
typedef ShGeneric<2, char> ShGeneric2b;
typedef ShGeneric<3, char> ShGeneric3b;
typedef ShGeneric<4, char> ShGeneric4b;

typedef ShGeneric<1, unsigned short> ShGeneric1us;
typedef ShGeneric<2, unsigned short> ShGeneric2us;
typedef ShGeneric<3, unsigned short> ShGeneric3us;
typedef ShGeneric<4, unsigned short> ShGeneric4us;

typedef ShGeneric<1, ShFracUByte> ShGeneric1fub;
typedef ShGeneric<2, ShFracUByte> ShGeneric2fub;
typedef ShGeneric<3, ShFracUByte> ShGeneric3fub;
typedef ShGeneric<4, ShFracUByte> ShGeneric4fub;

typedef ShGeneric<1, ShHalf> ShGeneric1h;
typedef ShGeneric<2, ShHalf> ShGeneric2h;
typedef ShGeneric<3, ShHalf> ShGeneric3h;
typedef ShGeneric<4, ShHalf> ShGeneric4h;

typedef ShGeneric<1, ShFracShort> ShGeneric1fs;
typedef ShGeneric<2, ShFracShort> ShGeneric2fs;
typedef ShGeneric<3, ShFracShort> ShGeneric3fs;
typedef ShGeneric<4, ShFracShort> ShGeneric4fs;

typedef ShGeneric<1, ShFracInt> ShGeneric1fi;
typedef ShGeneric<2, ShFracInt> ShGeneric2fi;
typedef ShGeneric<3, ShFracInt> ShGeneric3fi;
typedef ShGeneric<4, ShFracInt> ShGeneric4fi;

typedef ShGeneric<1, unsigned int> ShGeneric1ui;
typedef ShGeneric<2, unsigned int> ShGeneric2ui;
typedef ShGeneric<3, unsigned int> ShGeneric3ui;
typedef ShGeneric<4, unsigned int> ShGeneric4ui;

}

// This is a hack for ShAttrib.hpp in particular.
// A little dirty, but it works well.
#ifndef SH_DO_NOT_INCLUDE_GENERIC_IMPL
#include "ShGenericImpl.hpp"
#endif

#endif
