// WARNING - DO NOT EDIT THIS FILE:
// This file was automatically generated from scripts/ShNormal.hpp.py.
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

#ifndef SH_SHNORMAL_HPP
#define SH_SHNORMAL_HPP

#include "ShVector.hpp"
namespace SH {

/** An n-normal.
 * 
 *
 */
template<int N, ShBindingType Binding, typename T=float, bool Swizzled=false>
class ShNormal : public ShVector<N, Binding, T, Swizzled> {
public:
  ShNormal();
  ShNormal(const ShGeneric<N, T>& other);
  ShNormal(const ShNormal<N, Binding, T, Swizzled>& other);
  ShNormal(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShNormal(T data[N]);
  
  ~ShNormal();

  ShNormal& operator=(const ShGeneric<N, T>& other);
  ShNormal& operator=(const ShNormal<N, Binding, T, Swizzled>& other);

  ShNormal& operator+=(const ShGeneric<N, T>& right);
  ShNormal& operator-=(const ShGeneric<N, T>& right);
  ShNormal& operator*=(const ShGeneric<N, T>& right);
  ShNormal& operator/=(const ShGeneric<N, T>& right);
  ShNormal& operator%=(const ShGeneric<N, T>& right);
  ShNormal& operator*=(T);
  ShNormal& operator/=(T);
  ShNormal& operator%=(T);
  ShNormal& operator*=(const ShGeneric<1, T>&);
  ShNormal& operator/=(const ShGeneric<1, T>&);
  ShNormal& operator%=(const ShGeneric<1, T>&);
  ShNormal<1, Binding, T, true> operator()(int) const;
  ShNormal<2, Binding, T, true> operator()(int, int) const;
  ShNormal<3, Binding, T, true> operator()(int, int, int) const;
  ShNormal<4, Binding, T, true> operator()(int, int, int, int) const;
  ShNormal<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShNormal<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShNormal operator-() const;
  typedef T ValueType;
  static const int typesize = N;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_NORMAL;

  typedef ShNormal<N, SH_INPUT, T> InputType;
  typedef ShNormal<N, SH_OUTPUT, T> OutputType;
  typedef ShNormal<N, SH_INOUT, T> InOutType;
  typedef ShNormal<N, SH_TEMP, T> TempType;
  typedef ShNormal<N, SH_CONST, T> ConstType;
private:
  typedef ShVector<N, Binding, T, Swizzled> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShNormal<1, Binding, T, Swizzled> : public ShVector<1, Binding, T, Swizzled> {
public:
  ShNormal();
  ShNormal(const ShGeneric<1, T>& other);
  ShNormal(const ShNormal<1, Binding, T, Swizzled>& other);
  ShNormal(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShNormal(T data[1]);
  
  ShNormal(T);
  
  ~ShNormal();

  ShNormal& operator=(const ShGeneric<1, T>& other);
  ShNormal& operator=(const ShNormal<1, Binding, T, Swizzled>& other);

  ShNormal& operator+=(const ShGeneric<1, T>& right);
  ShNormal& operator-=(const ShGeneric<1, T>& right);
  ShNormal& operator*=(const ShGeneric<1, T>& right);
  ShNormal& operator/=(const ShGeneric<1, T>& right);
  ShNormal& operator%=(const ShGeneric<1, T>& right);
  ShNormal& operator*=(T);
  ShNormal& operator/=(T);
  ShNormal& operator%=(T);
  ShNormal& operator+=(T);
  ShNormal& operator-=(T);
  ShNormal<1, Binding, T, true> operator()(int) const;
  ShNormal<2, Binding, T, true> operator()(int, int) const;
  ShNormal<3, Binding, T, true> operator()(int, int, int) const;
  ShNormal<4, Binding, T, true> operator()(int, int, int, int) const;
  ShNormal<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShNormal<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShNormal operator-() const;
  typedef T ValueType;
  static const int typesize = 1;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_NORMAL;

  typedef ShNormal<1, SH_INPUT, T> InputType;
  typedef ShNormal<1, SH_OUTPUT, T> OutputType;
  typedef ShNormal<1, SH_INOUT, T> InOutType;
  typedef ShNormal<1, SH_TEMP, T> TempType;
  typedef ShNormal<1, SH_CONST, T> ConstType;
private:
  typedef ShVector<1, Binding, T, Swizzled> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShNormal<2, Binding, T, Swizzled> : public ShVector<2, Binding, T, Swizzled> {
public:
  ShNormal();
  ShNormal(const ShGeneric<2, T>& other);
  ShNormal(const ShNormal<2, Binding, T, Swizzled>& other);
  ShNormal(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShNormal(T data[2]);
  
  ShNormal(T, T);
  ShNormal(const ShGeneric<1, T>&, const ShGeneric<1, T>&);
  
  ~ShNormal();

  ShNormal& operator=(const ShGeneric<2, T>& other);
  ShNormal& operator=(const ShNormal<2, Binding, T, Swizzled>& other);

  ShNormal& operator+=(const ShGeneric<2, T>& right);
  ShNormal& operator-=(const ShGeneric<2, T>& right);
  ShNormal& operator*=(const ShGeneric<2, T>& right);
  ShNormal& operator/=(const ShGeneric<2, T>& right);
  ShNormal& operator%=(const ShGeneric<2, T>& right);
  ShNormal& operator*=(T);
  ShNormal& operator/=(T);
  ShNormal& operator%=(T);
  ShNormal& operator*=(const ShGeneric<1, T>&);
  ShNormal& operator/=(const ShGeneric<1, T>&);
  ShNormal& operator%=(const ShGeneric<1, T>&);
  ShNormal<1, Binding, T, true> operator()(int) const;
  ShNormal<2, Binding, T, true> operator()(int, int) const;
  ShNormal<3, Binding, T, true> operator()(int, int, int) const;
  ShNormal<4, Binding, T, true> operator()(int, int, int, int) const;
  ShNormal<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShNormal<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShNormal operator-() const;
  typedef T ValueType;
  static const int typesize = 2;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_NORMAL;

  typedef ShNormal<2, SH_INPUT, T> InputType;
  typedef ShNormal<2, SH_OUTPUT, T> OutputType;
  typedef ShNormal<2, SH_INOUT, T> InOutType;
  typedef ShNormal<2, SH_TEMP, T> TempType;
  typedef ShNormal<2, SH_CONST, T> ConstType;
private:
  typedef ShVector<2, Binding, T, Swizzled> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShNormal<3, Binding, T, Swizzled> : public ShVector<3, Binding, T, Swizzled> {
public:
  ShNormal();
  ShNormal(const ShGeneric<3, T>& other);
  ShNormal(const ShNormal<3, Binding, T, Swizzled>& other);
  ShNormal(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShNormal(T data[3]);
  
  ShNormal(T, T, T);
  ShNormal(const ShGeneric<1, T>&, const ShGeneric<1, T>&, const ShGeneric<1, T>&);
  
  ~ShNormal();

  ShNormal& operator=(const ShGeneric<3, T>& other);
  ShNormal& operator=(const ShNormal<3, Binding, T, Swizzled>& other);

  ShNormal& operator+=(const ShGeneric<3, T>& right);
  ShNormal& operator-=(const ShGeneric<3, T>& right);
  ShNormal& operator*=(const ShGeneric<3, T>& right);
  ShNormal& operator/=(const ShGeneric<3, T>& right);
  ShNormal& operator%=(const ShGeneric<3, T>& right);
  ShNormal& operator*=(T);
  ShNormal& operator/=(T);
  ShNormal& operator%=(T);
  ShNormal& operator*=(const ShGeneric<1, T>&);
  ShNormal& operator/=(const ShGeneric<1, T>&);
  ShNormal& operator%=(const ShGeneric<1, T>&);
  ShNormal<1, Binding, T, true> operator()(int) const;
  ShNormal<2, Binding, T, true> operator()(int, int) const;
  ShNormal<3, Binding, T, true> operator()(int, int, int) const;
  ShNormal<4, Binding, T, true> operator()(int, int, int, int) const;
  ShNormal<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShNormal<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShNormal operator-() const;
  typedef T ValueType;
  static const int typesize = 3;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_NORMAL;

  typedef ShNormal<3, SH_INPUT, T> InputType;
  typedef ShNormal<3, SH_OUTPUT, T> OutputType;
  typedef ShNormal<3, SH_INOUT, T> InOutType;
  typedef ShNormal<3, SH_TEMP, T> TempType;
  typedef ShNormal<3, SH_CONST, T> ConstType;
private:
  typedef ShVector<3, Binding, T, Swizzled> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShNormal<4, Binding, T, Swizzled> : public ShVector<4, Binding, T, Swizzled> {
public:
  ShNormal();
  ShNormal(const ShGeneric<4, T>& other);
  ShNormal(const ShNormal<4, Binding, T, Swizzled>& other);
  ShNormal(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShNormal(T data[4]);
  
  ShNormal(T, T, T, T);
  ShNormal(const ShGeneric<1, T>&, const ShGeneric<1, T>&, const ShGeneric<1, T>&, const ShGeneric<1, T>&);
  
  ~ShNormal();

  ShNormal& operator=(const ShGeneric<4, T>& other);
  ShNormal& operator=(const ShNormal<4, Binding, T, Swizzled>& other);

  ShNormal& operator+=(const ShGeneric<4, T>& right);
  ShNormal& operator-=(const ShGeneric<4, T>& right);
  ShNormal& operator*=(const ShGeneric<4, T>& right);
  ShNormal& operator/=(const ShGeneric<4, T>& right);
  ShNormal& operator%=(const ShGeneric<4, T>& right);
  ShNormal& operator*=(T);
  ShNormal& operator/=(T);
  ShNormal& operator%=(T);
  ShNormal& operator*=(const ShGeneric<1, T>&);
  ShNormal& operator/=(const ShGeneric<1, T>&);
  ShNormal& operator%=(const ShGeneric<1, T>&);
  ShNormal<1, Binding, T, true> operator()(int) const;
  ShNormal<2, Binding, T, true> operator()(int, int) const;
  ShNormal<3, Binding, T, true> operator()(int, int, int) const;
  ShNormal<4, Binding, T, true> operator()(int, int, int, int) const;
  ShNormal<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShNormal<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShNormal operator-() const;
  typedef T ValueType;
  static const int typesize = 4;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_NORMAL;

  typedef ShNormal<4, SH_INPUT, T> InputType;
  typedef ShNormal<4, SH_OUTPUT, T> OutputType;
  typedef ShNormal<4, SH_INOUT, T> InOutType;
  typedef ShNormal<4, SH_TEMP, T> TempType;
  typedef ShNormal<4, SH_CONST, T> ConstType;
private:
  typedef ShVector<4, Binding, T, Swizzled> ParentType;
};

typedef ShNormal<1, SH_INPUT, float> ShInputNormal1f;
typedef ShNormal<1, SH_OUTPUT, float> ShOutputNormal1f;
typedef ShNormal<1, SH_INOUT, float> ShInOutNormal1f;
typedef ShNormal<1, SH_TEMP, float> ShNormal1f;
typedef ShNormal<1, SH_CONST, float> ShConstNormal1f;
typedef ShNormal<2, SH_INPUT, float> ShInputNormal2f;
typedef ShNormal<2, SH_OUTPUT, float> ShOutputNormal2f;
typedef ShNormal<2, SH_INOUT, float> ShInOutNormal2f;
typedef ShNormal<2, SH_TEMP, float> ShNormal2f;
typedef ShNormal<2, SH_CONST, float> ShConstNormal2f;
typedef ShNormal<3, SH_INPUT, float> ShInputNormal3f;
typedef ShNormal<3, SH_OUTPUT, float> ShOutputNormal3f;
typedef ShNormal<3, SH_INOUT, float> ShInOutNormal3f;
typedef ShNormal<3, SH_TEMP, float> ShNormal3f;
typedef ShNormal<3, SH_CONST, float> ShConstNormal3f;
typedef ShNormal<4, SH_INPUT, float> ShInputNormal4f;
typedef ShNormal<4, SH_OUTPUT, float> ShOutputNormal4f;
typedef ShNormal<4, SH_INOUT, float> ShInOutNormal4f;
typedef ShNormal<4, SH_TEMP, float> ShNormal4f;
typedef ShNormal<4, SH_CONST, float> ShConstNormal4f;

} // namespace SH
#include "ShNormalImpl.hpp"

#endif // SH_SHNORMAL_HPP
