// WARNING - DO NOT EDIT THIS FILE:
// This file was automatically generated from scripts/ShColor.hpp.py.
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

#ifndef SH_SHCOLOR_HPP
#define SH_SHCOLOR_HPP

#include "ShAttrib.hpp"
namespace SH {

/** An n-color.
 * 
 *
 */
template<int N, ShBindingType Binding, typename T=float, bool Swizzled=false>
class ShColor : public ShAttrib<N, Binding, T, Swizzled> {
public:
  ShColor();
  ShColor(const ShGeneric<N, T>& other);
  ShColor(const ShColor<N, Binding, T, Swizzled>& other);
  ShColor(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShColor(T data[N]);
  
  ~ShColor();

  ShColor& operator=(const ShGeneric<N, T>& other);
  ShColor& operator=(const ShColor<N, Binding, T, Swizzled>& other);

  ShColor& operator+=(const ShGeneric<N, T>& right);
  ShColor& operator-=(const ShGeneric<N, T>& right);
  ShColor& operator*=(const ShGeneric<N, T>& right);
  ShColor& operator/=(const ShGeneric<N, T>& right);
  ShColor& operator*=(T);
  ShColor& operator/=(T);
  ShColor& operator*=(const ShGeneric<1, T>&);
  ShColor& operator/=(const ShGeneric<1, T>&);
  ShColor<1, Binding, T, true> operator()(int) const;
  ShColor<2, Binding, T, true> operator()(int, int) const;
  ShColor<3, Binding, T, true> operator()(int, int, int) const;
  ShColor<4, Binding, T, true> operator()(int, int, int, int) const;
  ShColor<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShColor<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShColor operator-() const;
  typedef T ValueType;
  static const int typesize = N;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_COLOR;

  typedef ShColor<N, SH_INPUT, T> InputType;
  typedef ShColor<N, SH_OUTPUT, T> OutputType;
  typedef ShColor<N, SH_INOUT, T> InOutType;
  typedef ShColor<N, SH_TEMP, T> TempType;
  typedef ShColor<N, SH_CONST, T> ConstType;
private:
  typedef ShAttrib<N, Binding, T, Swizzled> ParentType;
};

/** An n-color.
 * 
 *
 */
template<ShBindingType Binding, typename T, bool Swizzled>
class ShColor<1, Binding, T, Swizzled> : public ShAttrib<1, Binding, T, Swizzled> {
public:
  ShColor();
  ShColor(const ShGeneric<1, T>& other);
  ShColor(const ShColor<1, Binding, T, Swizzled>& other);
  ShColor(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShColor(T data[1]);
  
  ShColor(T);
  
  ~ShColor();

  ShColor& operator=(const ShGeneric<1, T>& other);
  ShColor& operator=(const ShColor<1, Binding, T, Swizzled>& other);

  ShColor& operator+=(const ShGeneric<1, T>& right);
  ShColor& operator-=(const ShGeneric<1, T>& right);
  ShColor& operator*=(const ShGeneric<1, T>& right);
  ShColor& operator/=(const ShGeneric<1, T>& right);
  ShColor& operator*=(T);
  ShColor& operator/=(T);
  ShColor& operator+=(T);
  ShColor& operator-=(T);
  ShColor<1, Binding, T, true> operator()(int) const;
  ShColor<2, Binding, T, true> operator()(int, int) const;
  ShColor<3, Binding, T, true> operator()(int, int, int) const;
  ShColor<4, Binding, T, true> operator()(int, int, int, int) const;
  ShColor<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShColor<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShColor operator-() const;
  typedef T ValueType;
  static const int typesize = 1;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_COLOR;

  typedef ShColor<1, SH_INPUT, T> InputType;
  typedef ShColor<1, SH_OUTPUT, T> OutputType;
  typedef ShColor<1, SH_INOUT, T> InOutType;
  typedef ShColor<1, SH_TEMP, T> TempType;
  typedef ShColor<1, SH_CONST, T> ConstType;
private:
  typedef ShAttrib<1, Binding, T, Swizzled> ParentType;
};

/** An n-color.
 * 
 *
 */
template<ShBindingType Binding, typename T, bool Swizzled>
class ShColor<2, Binding, T, Swizzled> : public ShAttrib<2, Binding, T, Swizzled> {
public:
  ShColor();
  ShColor(const ShGeneric<2, T>& other);
  ShColor(const ShColor<2, Binding, T, Swizzled>& other);
  ShColor(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShColor(T data[2]);
  
  ShColor(T, T);
  ShColor(const ShGeneric<1, T>&, const ShGeneric<1, T>&);
  
  ~ShColor();

  ShColor& operator=(const ShGeneric<2, T>& other);
  ShColor& operator=(const ShColor<2, Binding, T, Swizzled>& other);

  ShColor& operator+=(const ShGeneric<2, T>& right);
  ShColor& operator-=(const ShGeneric<2, T>& right);
  ShColor& operator*=(const ShGeneric<2, T>& right);
  ShColor& operator/=(const ShGeneric<2, T>& right);
  ShColor& operator*=(T);
  ShColor& operator/=(T);
  ShColor& operator*=(const ShGeneric<1, T>&);
  ShColor& operator/=(const ShGeneric<1, T>&);
  ShColor<1, Binding, T, true> operator()(int) const;
  ShColor<2, Binding, T, true> operator()(int, int) const;
  ShColor<3, Binding, T, true> operator()(int, int, int) const;
  ShColor<4, Binding, T, true> operator()(int, int, int, int) const;
  ShColor<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShColor<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShColor operator-() const;
  typedef T ValueType;
  static const int typesize = 2;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_COLOR;

  typedef ShColor<2, SH_INPUT, T> InputType;
  typedef ShColor<2, SH_OUTPUT, T> OutputType;
  typedef ShColor<2, SH_INOUT, T> InOutType;
  typedef ShColor<2, SH_TEMP, T> TempType;
  typedef ShColor<2, SH_CONST, T> ConstType;
private:
  typedef ShAttrib<2, Binding, T, Swizzled> ParentType;
};

/** An n-color.
 * 
 *
 */
template<ShBindingType Binding, typename T, bool Swizzled>
class ShColor<3, Binding, T, Swizzled> : public ShAttrib<3, Binding, T, Swizzled> {
public:
  ShColor();
  ShColor(const ShGeneric<3, T>& other);
  ShColor(const ShColor<3, Binding, T, Swizzled>& other);
  ShColor(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShColor(T data[3]);
  
  ShColor(T, T, T);
  ShColor(const ShGeneric<1, T>&, const ShGeneric<1, T>&, const ShGeneric<1, T>&);
  
  ~ShColor();

  ShColor& operator=(const ShGeneric<3, T>& other);
  ShColor& operator=(const ShColor<3, Binding, T, Swizzled>& other);

  ShColor& operator+=(const ShGeneric<3, T>& right);
  ShColor& operator-=(const ShGeneric<3, T>& right);
  ShColor& operator*=(const ShGeneric<3, T>& right);
  ShColor& operator/=(const ShGeneric<3, T>& right);
  ShColor& operator*=(T);
  ShColor& operator/=(T);
  ShColor& operator*=(const ShGeneric<1, T>&);
  ShColor& operator/=(const ShGeneric<1, T>&);
  ShColor<1, Binding, T, true> operator()(int) const;
  ShColor<2, Binding, T, true> operator()(int, int) const;
  ShColor<3, Binding, T, true> operator()(int, int, int) const;
  ShColor<4, Binding, T, true> operator()(int, int, int, int) const;
  ShColor<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShColor<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShColor operator-() const;
  typedef T ValueType;
  static const int typesize = 3;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_COLOR;

  typedef ShColor<3, SH_INPUT, T> InputType;
  typedef ShColor<3, SH_OUTPUT, T> OutputType;
  typedef ShColor<3, SH_INOUT, T> InOutType;
  typedef ShColor<3, SH_TEMP, T> TempType;
  typedef ShColor<3, SH_CONST, T> ConstType;
private:
  typedef ShAttrib<3, Binding, T, Swizzled> ParentType;
};

/** An n-color.
 * 
 *
 */
template<ShBindingType Binding, typename T, bool Swizzled>
class ShColor<4, Binding, T, Swizzled> : public ShAttrib<4, Binding, T, Swizzled> {
public:
  ShColor();
  ShColor(const ShGeneric<4, T>& other);
  ShColor(const ShColor<4, Binding, T, Swizzled>& other);
  ShColor(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShColor(T data[4]);
  
  ShColor(T, T, T, T);
  ShColor(const ShGeneric<1, T>&, const ShGeneric<1, T>&, const ShGeneric<1, T>&, const ShGeneric<1, T>&);
  
  ~ShColor();

  ShColor& operator=(const ShGeneric<4, T>& other);
  ShColor& operator=(const ShColor<4, Binding, T, Swizzled>& other);

  ShColor& operator+=(const ShGeneric<4, T>& right);
  ShColor& operator-=(const ShGeneric<4, T>& right);
  ShColor& operator*=(const ShGeneric<4, T>& right);
  ShColor& operator/=(const ShGeneric<4, T>& right);
  ShColor& operator*=(T);
  ShColor& operator/=(T);
  ShColor& operator*=(const ShGeneric<1, T>&);
  ShColor& operator/=(const ShGeneric<1, T>&);
  ShColor<1, Binding, T, true> operator()(int) const;
  ShColor<2, Binding, T, true> operator()(int, int) const;
  ShColor<3, Binding, T, true> operator()(int, int, int) const;
  ShColor<4, Binding, T, true> operator()(int, int, int, int) const;
  ShColor<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShColor<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShColor operator-() const;
  typedef T ValueType;
  static const int typesize = 4;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_COLOR;

  typedef ShColor<4, SH_INPUT, T> InputType;
  typedef ShColor<4, SH_OUTPUT, T> OutputType;
  typedef ShColor<4, SH_INOUT, T> InOutType;
  typedef ShColor<4, SH_TEMP, T> TempType;
  typedef ShColor<4, SH_CONST, T> ConstType;
private:
  typedef ShAttrib<4, Binding, T, Swizzled> ParentType;
};

typedef ShColor<1, SH_INPUT, float> ShInputColor1f;
typedef ShColor<1, SH_OUTPUT, float> ShOutputColor1f;
typedef ShColor<1, SH_INOUT, float> ShInOutColor1f;
typedef ShColor<1, SH_TEMP, float> ShColor1f;
typedef ShColor<1, SH_CONST, float> ShConstColor1f;
typedef ShColor<2, SH_INPUT, float> ShInputColor2f;
typedef ShColor<2, SH_OUTPUT, float> ShOutputColor2f;
typedef ShColor<2, SH_INOUT, float> ShInOutColor2f;
typedef ShColor<2, SH_TEMP, float> ShColor2f;
typedef ShColor<2, SH_CONST, float> ShConstColor2f;
typedef ShColor<3, SH_INPUT, float> ShInputColor3f;
typedef ShColor<3, SH_OUTPUT, float> ShOutputColor3f;
typedef ShColor<3, SH_INOUT, float> ShInOutColor3f;
typedef ShColor<3, SH_TEMP, float> ShColor3f;
typedef ShColor<3, SH_CONST, float> ShConstColor3f;
typedef ShColor<4, SH_INPUT, float> ShInputColor4f;
typedef ShColor<4, SH_OUTPUT, float> ShOutputColor4f;
typedef ShColor<4, SH_INOUT, float> ShInOutColor4f;
typedef ShColor<4, SH_TEMP, float> ShColor4f;
typedef ShColor<4, SH_CONST, float> ShConstColor4f;

} // namespace SH

#endif // SH_SHCOLOR_HPP
