// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Viberiu S. Popa,
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
 * template<int N, ShValueType V, ShBindingType B1, ShBindingType B2,
 *          bool S1, bool S2>
 * ShAttrib<N, SH_TEMP, V> add(const ShAttrib<N, B1, V, S1>& a,
 *                             const ShAttrib<N, B2, V, S2>& b);
 *
 * With Generic:
 *
 * template<int N, ShValueType V>
 * ShAttrib<N, SH_TEMP, V> add(const ShGeneric<N, V>& a,
 *                             const ShGeneric<N, V>& b);
 *
 * This class is explicitly instantiated for T = float with 1 <= N <= 4.
 */
template<int N, ShValueType V>
class ShGeneric : public ShVariable 
{
public:
  static const ShValueType value_type = V;
  typedef typename ShHostType<V>::type H; 
  typedef H HostType; 
  typedef typename ShMemType<V>::type MemType; 
  static const int typesize = N;

  typedef ShDataVariant<V, SH_HOST> VariantType; 
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
  // ShGeneric(const ShGeneric<N, V> &other);

  template<ShValueType V2>
  ShGeneric(const ShGeneric<N, V2> &other);
  // @}

  // This is needed because the templated assignment op is 
  // non-default, hence C++ spec 12.8.10 says the default
  // is implicitly defined.  Here that doesn't work so well. 
  ShGeneric& operator=(const ShGeneric& other);

  template<ShValueType V2>
  ShGeneric& operator=(const ShGeneric<N, V2>& other);

  ShGeneric& operator=(const ShProgram& other);
  
  template<ShValueType V2>
  ShGeneric& operator+=(const ShGeneric<N, V2>& right);

  template<ShValueType V2>
  ShGeneric& operator-=(const ShGeneric<N, V2>& right);

  template<ShValueType V2>
  ShGeneric& operator*=(const ShGeneric<N, V2>& right);

  template<ShValueType V2>
  ShGeneric& operator/=(const ShGeneric<N, V2>& right);

  template<ShValueType V2>
  ShGeneric& operator%=(const ShGeneric<N, V2>& right);

  template<ShValueType V2>
  ShGeneric& operator+=(const ShGeneric<1, V2>& right);

  template<ShValueType V2>
  ShGeneric& operator-=(const ShGeneric<1, V2>& right);

  template<ShValueType V2>
  ShGeneric& operator*=(const ShGeneric<1, V2>& right);

  template<ShValueType V2>
  ShGeneric& operator/=(const ShGeneric<1, V2>& right);

  template<ShValueType V2>
  ShGeneric& operator%=(const ShGeneric<1, V2>& right);

  ShGeneric& operator+=(H);
  ShGeneric& operator-=(H);
  ShGeneric& operator*=(H);
  ShGeneric& operator/=(H);
  ShGeneric& operator%=(H);

  ShGeneric operator-() const;

  ShGeneric operator()() const; ///< Identity swizzle
  ShGeneric<1, V> operator()(int) const;
  ShGeneric<1, V> operator[](int) const;
  ShGeneric<2, V> operator()(int, int) const;
  ShGeneric<3, V> operator()(int, int, int) const;
  ShGeneric<4, V> operator()(int, int, int, int) const;

  /// Range Metadata
  void range(H low, H high);

  VariantType lowBound() const; 
  H lowBound(int index) const;

  VariantType highBound() const;
  H highBound(int index) const;
  
  // Arbitrary Swizzle
  template<int N2>
  ShGeneric<N2, V> swiz(int indices[]) const;

  /// Get the values of this variable, with swizzling taken into account
  void getValues(H dest[]) const;
  H getValue(int index) const;
  
  /// Set the values of this variable, using the swizzle as a
  /// writemask.
  void setValue(int index, const H &value); 
  void setValues(const H values[]);


protected:
  ShGeneric(const ShVariableNodePtr& node);

};

template<ShValueType V>
class ShGeneric<1, V> : public ShVariable 
{
public:
  static const ShValueType value_type = V;
  typedef typename ShHostType<V>::type H; 
  typedef H HostType;
  typedef typename ShMemType<V>::type MemType; 
  static const int typesize = 1;
  typedef ShDataVariant<V, SH_HOST> VariantType; 
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
  // ShGeneric(const ShGeneric<1, V> &other);

  template<ShValueType V2>
  ShGeneric(const ShGeneric<1, V2> &other);
  // @}

  ShGeneric& operator=(const ShGeneric<1, V>& other);

  template<ShValueType V2>
  ShGeneric& operator=(const ShGeneric<1, V2>& other);

  ShGeneric& operator=(H);
  ShGeneric& operator=(const ShProgram& other);
  
  template<ShValueType V2>
  ShGeneric& operator+=(const ShGeneric<1, V2>& right);

  template<ShValueType V2>
  ShGeneric& operator-=(const ShGeneric<1, V2>& right);

  template<ShValueType V2>
  ShGeneric& operator*=(const ShGeneric<1, V2>& right);

  template<ShValueType V2>
  ShGeneric& operator/=(const ShGeneric<1, V2>& right);

  template<ShValueType V2>
  ShGeneric& operator%=(const ShGeneric<1, V2>& right);

  ShGeneric& operator+=(H);
  ShGeneric& operator-=(H);
  ShGeneric& operator*=(H);
  ShGeneric& operator/=(H);
  ShGeneric& operator%=(H);

  ShGeneric operator-() const;

  ShGeneric operator()() const; ///< Identity swizzle
  ShGeneric<1, V> operator()(int) const;
  ShGeneric<1, V> operator[](int) const;
  ShGeneric<2, V> operator()(int, int) const;
  ShGeneric<3, V> operator()(int, int, int) const;
  ShGeneric<4, V> operator()(int, int, int, int) const;

  /// Range Metadata
  void range(H low, H high);

  VariantType lowBound() const; 
  H lowBound(int index) const;

  VariantType highBound() const;
  H highBound(int index) const;
  
  // Arbitrary Swizzle
  template<int N2>
  ShGeneric<N2, V> swiz(int indices[]) const;

  /// Get the values of this variable, with swizzling taken into account
  void getValues(H dest[]) const;
  H getValue(int index) const;
  
  /// Set the values of this variable, using the swizzle as a
  /// writemask.
  void setValue(int index, const H &value); 
  void setValues(const H values[]);

protected:
  ShGeneric(const ShVariableNodePtr& node);

};

}

// This is a hack for ShAttrib.hpp in particular.
// A little dirty, but it works well.
#ifndef SH_DO_NOT_INCLUDE_GENERIC_IMPL
#include "ShGenericImpl.hpp"
#endif

#endif
