// WARNING - DO NOT EDIT THIS FILE:
// This file was automatically generated from scripts/ShAttrib.hpp.py.
// If you wish to change it, edit that file instead.
//
// ---
//
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

#ifndef SH_SHATTRIB_HPP
#define SH_SHATTRIB_HPP

#include "ShVariable.hpp"
namespace SH {

/** A generic attribute (or parameter) holding N values.
 * 
 * The reason we have the Swizzle template argument is so that
 * swizzled variables, which need to be copied (i.e. have an ASN
 * statement generated) when another variable is initialized through
 * them, may otherwise be simply placed into new variables (through
 * copy constructor eliding, as per paragraph 12.8.15 of the C++
 * standard), causing variables to share ShVariableNodes when they
 * shouldn't. By making swizzled variables separate types we enforce
 * calling a conversion constructor instead, which cannot be elided.
 * If this paragraph confuses you, and you're not modifying Sh
 * internals, you may safely ignore it.
 *
 */
template<int N, ShBindingType Binding, typename T=float, bool Swizzled=false>
class ShAttrib : public ShGeneric<N, T> {
public:
  ShAttrib();
  ShAttrib(const ShGeneric<N, T>& other);
  ShAttrib(const ShAttrib<N, Binding, T, Swizzled>& other);
  ShAttrib(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShAttrib(T data[N]);
  
  ~ShAttrib();

  ShAttrib& operator=(const ShGeneric<N, T>& other);
  ShAttrib& operator=(const ShAttrib<N, Binding, T, Swizzled>& other);

  ShAttrib& operator+=(const ShGeneric<N, T>& right);
  ShAttrib& operator-=(const ShGeneric<N, T>& right);
  ShAttrib& operator*=(const ShGeneric<N, T>& right);
  ShAttrib& operator/=(const ShGeneric<N, T>& right);
  ShAttrib& operator*=(T);
  ShAttrib& operator/=(T);
  ShAttrib& operator*=(const ShGeneric<1, T>&);
  ShAttrib& operator/=(const ShGeneric<1, T>&);
  ShAttrib<1, Binding, T, true> operator()(int) const;
  ShAttrib<2, Binding, T, true> operator()(int, int) const;
  ShAttrib<3, Binding, T, true> operator()(int, int, int) const;
  ShAttrib<4, Binding, T, true> operator()(int, int, int, int) const;
  ShAttrib<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShAttrib<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShAttrib operator-() const;
  typedef T ValueType;
  static const int typesize = N;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_ATTRIB;

  typedef ShAttrib<N, SH_INPUT, T> InputType;
  typedef ShAttrib<N, SH_OUTPUT, T> OutputType;
  typedef ShAttrib<N, SH_INOUT, T> InOutType;
  typedef ShAttrib<N, SH_TEMP, T> TempType;
  typedef ShAttrib<N, SH_CONST, T> ConstType;
private:
  typedef ShGeneric<N, T> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShAttrib<1, Binding, T, Swizzled> : public ShGeneric<1, T> {
public:
  ShAttrib();
  ShAttrib(const ShGeneric<1, T>& other);
  ShAttrib(const ShAttrib<1, Binding, T, Swizzled>& other);
  ShAttrib(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShAttrib(T data[1]);
  
  ShAttrib(T);
  
  ~ShAttrib();

  ShAttrib& operator=(const ShGeneric<1, T>& other);
  ShAttrib& operator=(const ShAttrib<1, Binding, T, Swizzled>& other);

  ShAttrib& operator+=(const ShGeneric<1, T>& right);
  ShAttrib& operator-=(const ShGeneric<1, T>& right);
  ShAttrib& operator*=(const ShGeneric<1, T>& right);
  ShAttrib& operator/=(const ShGeneric<1, T>& right);
  ShAttrib& operator*=(T);
  ShAttrib& operator/=(T);
  ShAttrib& operator+=(T);
  ShAttrib& operator-=(T);
  ShAttrib<1, Binding, T, true> operator()(int) const;
  ShAttrib<2, Binding, T, true> operator()(int, int) const;
  ShAttrib<3, Binding, T, true> operator()(int, int, int) const;
  ShAttrib<4, Binding, T, true> operator()(int, int, int, int) const;
  ShAttrib<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShAttrib<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShAttrib operator-() const;
  typedef T ValueType;
  static const int typesize = 1;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_ATTRIB;

  typedef ShAttrib<1, SH_INPUT, T> InputType;
  typedef ShAttrib<1, SH_OUTPUT, T> OutputType;
  typedef ShAttrib<1, SH_INOUT, T> InOutType;
  typedef ShAttrib<1, SH_TEMP, T> TempType;
  typedef ShAttrib<1, SH_CONST, T> ConstType;
private:
  typedef ShGeneric<1, T> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShAttrib<2, Binding, T, Swizzled> : public ShGeneric<2, T> {
public:
  ShAttrib();
  ShAttrib(const ShGeneric<2, T>& other);
  ShAttrib(const ShAttrib<2, Binding, T, Swizzled>& other);
  ShAttrib(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShAttrib(T data[2]);
  
  ShAttrib(T, T);
  ShAttrib(const ShGeneric<1, T>&, const ShGeneric<1, T>&);
  
  ~ShAttrib();

  ShAttrib& operator=(const ShGeneric<2, T>& other);
  ShAttrib& operator=(const ShAttrib<2, Binding, T, Swizzled>& other);

  ShAttrib& operator+=(const ShGeneric<2, T>& right);
  ShAttrib& operator-=(const ShGeneric<2, T>& right);
  ShAttrib& operator*=(const ShGeneric<2, T>& right);
  ShAttrib& operator/=(const ShGeneric<2, T>& right);
  ShAttrib& operator*=(T);
  ShAttrib& operator/=(T);
  ShAttrib& operator*=(const ShGeneric<1, T>&);
  ShAttrib& operator/=(const ShGeneric<1, T>&);
  ShAttrib<1, Binding, T, true> operator()(int) const;
  ShAttrib<2, Binding, T, true> operator()(int, int) const;
  ShAttrib<3, Binding, T, true> operator()(int, int, int) const;
  ShAttrib<4, Binding, T, true> operator()(int, int, int, int) const;
  ShAttrib<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShAttrib<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShAttrib operator-() const;
  typedef T ValueType;
  static const int typesize = 2;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_ATTRIB;

  typedef ShAttrib<2, SH_INPUT, T> InputType;
  typedef ShAttrib<2, SH_OUTPUT, T> OutputType;
  typedef ShAttrib<2, SH_INOUT, T> InOutType;
  typedef ShAttrib<2, SH_TEMP, T> TempType;
  typedef ShAttrib<2, SH_CONST, T> ConstType;
private:
  typedef ShGeneric<2, T> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShAttrib<3, Binding, T, Swizzled> : public ShGeneric<3, T> {
public:
  ShAttrib();
  ShAttrib(const ShGeneric<3, T>& other);
  ShAttrib(const ShAttrib<3, Binding, T, Swizzled>& other);
  ShAttrib(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShAttrib(T data[3]);
  
  ShAttrib(T, T, T);
  ShAttrib(const ShGeneric<1, T>&, const ShGeneric<1, T>&, const ShGeneric<1, T>&);
  
  ~ShAttrib();

  ShAttrib& operator=(const ShGeneric<3, T>& other);
  ShAttrib& operator=(const ShAttrib<3, Binding, T, Swizzled>& other);

  ShAttrib& operator+=(const ShGeneric<3, T>& right);
  ShAttrib& operator-=(const ShGeneric<3, T>& right);
  ShAttrib& operator*=(const ShGeneric<3, T>& right);
  ShAttrib& operator/=(const ShGeneric<3, T>& right);
  ShAttrib& operator*=(T);
  ShAttrib& operator/=(T);
  ShAttrib& operator*=(const ShGeneric<1, T>&);
  ShAttrib& operator/=(const ShGeneric<1, T>&);
  ShAttrib<1, Binding, T, true> operator()(int) const;
  ShAttrib<2, Binding, T, true> operator()(int, int) const;
  ShAttrib<3, Binding, T, true> operator()(int, int, int) const;
  ShAttrib<4, Binding, T, true> operator()(int, int, int, int) const;
  ShAttrib<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShAttrib<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShAttrib operator-() const;
  typedef T ValueType;
  static const int typesize = 3;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_ATTRIB;

  typedef ShAttrib<3, SH_INPUT, T> InputType;
  typedef ShAttrib<3, SH_OUTPUT, T> OutputType;
  typedef ShAttrib<3, SH_INOUT, T> InOutType;
  typedef ShAttrib<3, SH_TEMP, T> TempType;
  typedef ShAttrib<3, SH_CONST, T> ConstType;
private:
  typedef ShGeneric<3, T> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShAttrib<4, Binding, T, Swizzled> : public ShGeneric<4, T> {
public:
  ShAttrib();
  ShAttrib(const ShGeneric<4, T>& other);
  ShAttrib(const ShAttrib<4, Binding, T, Swizzled>& other);
  ShAttrib(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShAttrib(T data[4]);
  
  ShAttrib(T, T, T, T);
  ShAttrib(const ShGeneric<1, T>&, const ShGeneric<1, T>&, const ShGeneric<1, T>&, const ShGeneric<1, T>&);
  
  ~ShAttrib();

  ShAttrib& operator=(const ShGeneric<4, T>& other);
  ShAttrib& operator=(const ShAttrib<4, Binding, T, Swizzled>& other);

  ShAttrib& operator+=(const ShGeneric<4, T>& right);
  ShAttrib& operator-=(const ShGeneric<4, T>& right);
  ShAttrib& operator*=(const ShGeneric<4, T>& right);
  ShAttrib& operator/=(const ShGeneric<4, T>& right);
  ShAttrib& operator*=(T);
  ShAttrib& operator/=(T);
  ShAttrib& operator*=(const ShGeneric<1, T>&);
  ShAttrib& operator/=(const ShGeneric<1, T>&);
  ShAttrib<1, Binding, T, true> operator()(int) const;
  ShAttrib<2, Binding, T, true> operator()(int, int) const;
  ShAttrib<3, Binding, T, true> operator()(int, int, int) const;
  ShAttrib<4, Binding, T, true> operator()(int, int, int, int) const;
  ShAttrib<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShAttrib<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShAttrib operator-() const;
  typedef T ValueType;
  static const int typesize = 4;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_ATTRIB;

  typedef ShAttrib<4, SH_INPUT, T> InputType;
  typedef ShAttrib<4, SH_OUTPUT, T> OutputType;
  typedef ShAttrib<4, SH_INOUT, T> InOutType;
  typedef ShAttrib<4, SH_TEMP, T> TempType;
  typedef ShAttrib<4, SH_CONST, T> ConstType;
private:
  typedef ShGeneric<4, T> ParentType;
};

typedef ShAttrib<1, SH_INPUT, float> ShInputAttrib1f;
typedef ShAttrib<1, SH_OUTPUT, float> ShOutputAttrib1f;
typedef ShAttrib<1, SH_INOUT, float> ShInOutAttrib1f;
typedef ShAttrib<1, SH_TEMP, float> ShAttrib1f;
typedef ShAttrib<1, SH_CONST, float> ShConstAttrib1f;
typedef ShAttrib<2, SH_INPUT, float> ShInputAttrib2f;
typedef ShAttrib<2, SH_OUTPUT, float> ShOutputAttrib2f;
typedef ShAttrib<2, SH_INOUT, float> ShInOutAttrib2f;
typedef ShAttrib<2, SH_TEMP, float> ShAttrib2f;
typedef ShAttrib<2, SH_CONST, float> ShConstAttrib2f;
typedef ShAttrib<3, SH_INPUT, float> ShInputAttrib3f;
typedef ShAttrib<3, SH_OUTPUT, float> ShOutputAttrib3f;
typedef ShAttrib<3, SH_INOUT, float> ShInOutAttrib3f;
typedef ShAttrib<3, SH_TEMP, float> ShAttrib3f;
typedef ShAttrib<3, SH_CONST, float> ShConstAttrib3f;
typedef ShAttrib<4, SH_INPUT, float> ShInputAttrib4f;
typedef ShAttrib<4, SH_OUTPUT, float> ShOutputAttrib4f;
typedef ShAttrib<4, SH_INOUT, float> ShInOutAttrib4f;
typedef ShAttrib<4, SH_TEMP, float> ShAttrib4f;
typedef ShAttrib<4, SH_CONST, float> ShConstAttrib4f;

} // namespace SH

#endif // SH_SHATTRIB_HPP
