// WARNING - DO NOT EDIT THIS FILE:
// This file was automatically generated from scripts/ShPoint.hpp.py.
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

#ifndef SH_SHPOINT_HPP
#define SH_SHPOINT_HPP

#include "ShAttrib.hpp"
namespace SH {

/** An n-point.
 * 
 *
 */
template<int N, ShBindingType Binding, typename T=float, bool Swizzled=false>
class ShPoint : public ShAttrib<N, Binding, T, Swizzled> {
public:
  ShPoint();
  ShPoint(const ShGeneric<N, T>& other);
  ShPoint(const ShPoint<N, Binding, T, Swizzled>& other);
  ShPoint(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShPoint(T data[N]);
  
  ~ShPoint();

  ShPoint& operator=(const ShGeneric<N, T>& other);
  ShPoint& operator=(const ShPoint<N, Binding, T, Swizzled>& other);

  ShPoint& operator+=(const ShGeneric<N, T>& right);
  ShPoint& operator-=(const ShGeneric<N, T>& right);
  ShPoint& operator*=(const ShGeneric<N, T>& right);
  ShPoint& operator/=(const ShGeneric<N, T>& right);
  ShPoint& operator*=(T);
  ShPoint& operator/=(T);
  ShPoint& operator*=(const ShGeneric<1, T>&);
  ShPoint& operator/=(const ShGeneric<1, T>&);
  ShPoint<1, Binding, T, true> operator()(int) const;
  ShPoint<2, Binding, T, true> operator()(int, int) const;
  ShPoint<3, Binding, T, true> operator()(int, int, int) const;
  ShPoint<4, Binding, T, true> operator()(int, int, int, int) const;
  ShPoint<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShPoint<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShPoint operator-() const;
  typedef T ValueType;
  static const int typesize = N;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_POINT;

  typedef ShPoint<N, SH_INPUT, T> InputType;
  typedef ShPoint<N, SH_OUTPUT, T> OutputType;
  typedef ShPoint<N, SH_INOUT, T> InOutType;
  typedef ShPoint<N, SH_TEMP, T> TempType;
  typedef ShPoint<N, SH_CONST, T> ConstType;
private:
  typedef ShAttrib<N, Binding, T, Swizzled> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShPoint<1, Binding, T, Swizzled> : public ShAttrib<1, Binding, T, Swizzled> {
public:
  ShPoint();
  ShPoint(const ShGeneric<1, T>& other);
  ShPoint(const ShPoint<1, Binding, T, Swizzled>& other);
  ShPoint(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShPoint(T data[1]);
  
  ShPoint(T);
  
  ~ShPoint();

  ShPoint& operator=(const ShGeneric<1, T>& other);
  ShPoint& operator=(const ShPoint<1, Binding, T, Swizzled>& other);

  ShPoint& operator+=(const ShGeneric<1, T>& right);
  ShPoint& operator-=(const ShGeneric<1, T>& right);
  ShPoint& operator*=(const ShGeneric<1, T>& right);
  ShPoint& operator/=(const ShGeneric<1, T>& right);
  ShPoint& operator*=(T);
  ShPoint& operator/=(T);
  ShPoint& operator+=(T);
  ShPoint& operator-=(T);
  ShPoint<1, Binding, T, true> operator()(int) const;
  ShPoint<2, Binding, T, true> operator()(int, int) const;
  ShPoint<3, Binding, T, true> operator()(int, int, int) const;
  ShPoint<4, Binding, T, true> operator()(int, int, int, int) const;
  ShPoint<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShPoint<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShPoint operator-() const;
  typedef T ValueType;
  static const int typesize = 1;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_POINT;

  typedef ShPoint<1, SH_INPUT, T> InputType;
  typedef ShPoint<1, SH_OUTPUT, T> OutputType;
  typedef ShPoint<1, SH_INOUT, T> InOutType;
  typedef ShPoint<1, SH_TEMP, T> TempType;
  typedef ShPoint<1, SH_CONST, T> ConstType;
private:
  typedef ShAttrib<1, Binding, T, Swizzled> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShPoint<2, Binding, T, Swizzled> : public ShAttrib<2, Binding, T, Swizzled> {
public:
  ShPoint();
  ShPoint(const ShGeneric<2, T>& other);
  ShPoint(const ShPoint<2, Binding, T, Swizzled>& other);
  ShPoint(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShPoint(T data[2]);
  
  ShPoint(T, T);
  ShPoint(const ShGeneric<1, T>&, const ShGeneric<1, T>&);
  
  ~ShPoint();

  ShPoint& operator=(const ShGeneric<2, T>& other);
  ShPoint& operator=(const ShPoint<2, Binding, T, Swizzled>& other);

  ShPoint& operator+=(const ShGeneric<2, T>& right);
  ShPoint& operator-=(const ShGeneric<2, T>& right);
  ShPoint& operator*=(const ShGeneric<2, T>& right);
  ShPoint& operator/=(const ShGeneric<2, T>& right);
  ShPoint& operator*=(T);
  ShPoint& operator/=(T);
  ShPoint& operator*=(const ShGeneric<1, T>&);
  ShPoint& operator/=(const ShGeneric<1, T>&);
  ShPoint<1, Binding, T, true> operator()(int) const;
  ShPoint<2, Binding, T, true> operator()(int, int) const;
  ShPoint<3, Binding, T, true> operator()(int, int, int) const;
  ShPoint<4, Binding, T, true> operator()(int, int, int, int) const;
  ShPoint<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShPoint<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShPoint operator-() const;
  typedef T ValueType;
  static const int typesize = 2;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_POINT;

  typedef ShPoint<2, SH_INPUT, T> InputType;
  typedef ShPoint<2, SH_OUTPUT, T> OutputType;
  typedef ShPoint<2, SH_INOUT, T> InOutType;
  typedef ShPoint<2, SH_TEMP, T> TempType;
  typedef ShPoint<2, SH_CONST, T> ConstType;
private:
  typedef ShAttrib<2, Binding, T, Swizzled> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShPoint<3, Binding, T, Swizzled> : public ShAttrib<3, Binding, T, Swizzled> {
public:
  ShPoint();
  ShPoint(const ShGeneric<3, T>& other);
  ShPoint(const ShPoint<3, Binding, T, Swizzled>& other);
  ShPoint(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShPoint(T data[3]);
  
  ShPoint(T, T, T);
  ShPoint(const ShGeneric<1, T>&, const ShGeneric<1, T>&, const ShGeneric<1, T>&);
  
  ~ShPoint();

  ShPoint& operator=(const ShGeneric<3, T>& other);
  ShPoint& operator=(const ShPoint<3, Binding, T, Swizzled>& other);

  ShPoint& operator+=(const ShGeneric<3, T>& right);
  ShPoint& operator-=(const ShGeneric<3, T>& right);
  ShPoint& operator*=(const ShGeneric<3, T>& right);
  ShPoint& operator/=(const ShGeneric<3, T>& right);
  ShPoint& operator*=(T);
  ShPoint& operator/=(T);
  ShPoint& operator*=(const ShGeneric<1, T>&);
  ShPoint& operator/=(const ShGeneric<1, T>&);
  ShPoint<1, Binding, T, true> operator()(int) const;
  ShPoint<2, Binding, T, true> operator()(int, int) const;
  ShPoint<3, Binding, T, true> operator()(int, int, int) const;
  ShPoint<4, Binding, T, true> operator()(int, int, int, int) const;
  ShPoint<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShPoint<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShPoint operator-() const;
  typedef T ValueType;
  static const int typesize = 3;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_POINT;

  typedef ShPoint<3, SH_INPUT, T> InputType;
  typedef ShPoint<3, SH_OUTPUT, T> OutputType;
  typedef ShPoint<3, SH_INOUT, T> InOutType;
  typedef ShPoint<3, SH_TEMP, T> TempType;
  typedef ShPoint<3, SH_CONST, T> ConstType;
private:
  typedef ShAttrib<3, Binding, T, Swizzled> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShPoint<4, Binding, T, Swizzled> : public ShAttrib<4, Binding, T, Swizzled> {
public:
  ShPoint();
  ShPoint(const ShGeneric<4, T>& other);
  ShPoint(const ShPoint<4, Binding, T, Swizzled>& other);
  ShPoint(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShPoint(T data[4]);
  
  ShPoint(T, T, T, T);
  ShPoint(const ShGeneric<1, T>&, const ShGeneric<1, T>&, const ShGeneric<1, T>&, const ShGeneric<1, T>&);
  
  ~ShPoint();

  ShPoint& operator=(const ShGeneric<4, T>& other);
  ShPoint& operator=(const ShPoint<4, Binding, T, Swizzled>& other);

  ShPoint& operator+=(const ShGeneric<4, T>& right);
  ShPoint& operator-=(const ShGeneric<4, T>& right);
  ShPoint& operator*=(const ShGeneric<4, T>& right);
  ShPoint& operator/=(const ShGeneric<4, T>& right);
  ShPoint& operator*=(T);
  ShPoint& operator/=(T);
  ShPoint& operator*=(const ShGeneric<1, T>&);
  ShPoint& operator/=(const ShGeneric<1, T>&);
  ShPoint<1, Binding, T, true> operator()(int) const;
  ShPoint<2, Binding, T, true> operator()(int, int) const;
  ShPoint<3, Binding, T, true> operator()(int, int, int) const;
  ShPoint<4, Binding, T, true> operator()(int, int, int, int) const;
  ShPoint<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShPoint<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShPoint operator-() const;
  typedef T ValueType;
  static const int typesize = 4;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_POINT;

  typedef ShPoint<4, SH_INPUT, T> InputType;
  typedef ShPoint<4, SH_OUTPUT, T> OutputType;
  typedef ShPoint<4, SH_INOUT, T> InOutType;
  typedef ShPoint<4, SH_TEMP, T> TempType;
  typedef ShPoint<4, SH_CONST, T> ConstType;
private:
  typedef ShAttrib<4, Binding, T, Swizzled> ParentType;
};

typedef ShPoint<1, SH_INPUT, float> ShInputPoint1f;
typedef ShPoint<1, SH_OUTPUT, float> ShOutputPoint1f;
typedef ShPoint<1, SH_INOUT, float> ShInOutPoint1f;
typedef ShPoint<1, SH_TEMP, float> ShPoint1f;
typedef ShPoint<1, SH_CONST, float> ShConstPoint1f;
typedef ShPoint<2, SH_INPUT, float> ShInputPoint2f;
typedef ShPoint<2, SH_OUTPUT, float> ShOutputPoint2f;
typedef ShPoint<2, SH_INOUT, float> ShInOutPoint2f;
typedef ShPoint<2, SH_TEMP, float> ShPoint2f;
typedef ShPoint<2, SH_CONST, float> ShConstPoint2f;
typedef ShPoint<3, SH_INPUT, float> ShInputPoint3f;
typedef ShPoint<3, SH_OUTPUT, float> ShOutputPoint3f;
typedef ShPoint<3, SH_INOUT, float> ShInOutPoint3f;
typedef ShPoint<3, SH_TEMP, float> ShPoint3f;
typedef ShPoint<3, SH_CONST, float> ShConstPoint3f;
typedef ShPoint<4, SH_INPUT, float> ShInputPoint4f;
typedef ShPoint<4, SH_OUTPUT, float> ShOutputPoint4f;
typedef ShPoint<4, SH_INOUT, float> ShInOutPoint4f;
typedef ShPoint<4, SH_TEMP, float> ShPoint4f;
typedef ShPoint<4, SH_CONST, float> ShConstPoint4f;

} // namespace SH

#endif // SH_SHPOINT_HPP
