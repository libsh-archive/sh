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
  
  template<typename T2>
  ShNormal(const ShGeneric<N, T2>& other);
  ShNormal(const ShNormal<N, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShNormal(const ShNormal<N, Binding, T2, Swizzled>& other);
  ShNormal(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShNormal(T data[N]);
  
  ~ShNormal();

  
  template<typename T2>
  ShNormal& operator=(const ShGeneric<N, T2>& other);
  
  template<typename T2>
  ShNormal& operator=(const ShNormal<N, Binding, T2, Swizzled>& other);
  ShNormal& operator=(const ShNormal<N, Binding, T, Swizzled>& other);

  
  template<typename T2>
  ShNormal& operator+=(const ShGeneric<N, T2>& right);
  
  template<typename T2>
  ShNormal& operator-=(const ShGeneric<N, T2>& right);
  
  template<typename T2>
  ShNormal& operator*=(const ShGeneric<N, T2>& right);
  
  template<typename T2>
  ShNormal& operator/=(const ShGeneric<N, T2>& right);
  
  template<typename T2>
  ShNormal& operator%=(const ShGeneric<N, T2>& right);
  ShNormal& operator*=(T);
  ShNormal& operator/=(T);
  ShNormal& operator%=(T);
  ShNormal& operator+=(T);
  ShNormal& operator-=(T);
  
  template<typename T2>
  ShNormal& operator+=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShNormal& operator-=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShNormal& operator*=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShNormal& operator/=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShNormal& operator%=(const ShGeneric<1, T2>&);
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
  
  template<typename T2>
  ShNormal(const ShGeneric<1, T2>& other);
  ShNormal(const ShNormal<1, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShNormal(const ShNormal<1, Binding, T2, Swizzled>& other);
  ShNormal(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShNormal(T data[1]);
  
  ShNormal(T);
  
  ~ShNormal();

  
  template<typename T2>
  ShNormal& operator=(const ShGeneric<1, T2>& other);
  
  template<typename T2>
  ShNormal& operator=(const ShNormal<1, Binding, T2, Swizzled>& other);
  ShNormal& operator=(const ShNormal<1, Binding, T, Swizzled>& other);

  ShNormal& operator=(T other);

  
  template<typename T2>
  ShNormal& operator+=(const ShGeneric<1, T2>& right);
  
  template<typename T2>
  ShNormal& operator-=(const ShGeneric<1, T2>& right);
  
  template<typename T2>
  ShNormal& operator*=(const ShGeneric<1, T2>& right);
  
  template<typename T2>
  ShNormal& operator/=(const ShGeneric<1, T2>& right);
  
  template<typename T2>
  ShNormal& operator%=(const ShGeneric<1, T2>& right);
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
  
  template<typename T2>
  ShNormal(const ShGeneric<2, T2>& other);
  ShNormal(const ShNormal<2, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShNormal(const ShNormal<2, Binding, T2, Swizzled>& other);
  ShNormal(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShNormal(T data[2]);
  
  ShNormal(T, T);
  template<typename T2, typename T3>
  ShNormal(const ShGeneric<1, T2>&, const ShGeneric<1, T3>&);
  
  ~ShNormal();

  
  template<typename T2>
  ShNormal& operator=(const ShGeneric<2, T2>& other);
  
  template<typename T2>
  ShNormal& operator=(const ShNormal<2, Binding, T2, Swizzled>& other);
  ShNormal& operator=(const ShNormal<2, Binding, T, Swizzled>& other);

  
  template<typename T2>
  ShNormal& operator+=(const ShGeneric<2, T2>& right);
  
  template<typename T2>
  ShNormal& operator-=(const ShGeneric<2, T2>& right);
  
  template<typename T2>
  ShNormal& operator*=(const ShGeneric<2, T2>& right);
  
  template<typename T2>
  ShNormal& operator/=(const ShGeneric<2, T2>& right);
  
  template<typename T2>
  ShNormal& operator%=(const ShGeneric<2, T2>& right);
  ShNormal& operator*=(T);
  ShNormal& operator/=(T);
  ShNormal& operator%=(T);
  ShNormal& operator+=(T);
  ShNormal& operator-=(T);
  
  template<typename T2>
  ShNormal& operator+=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShNormal& operator-=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShNormal& operator*=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShNormal& operator/=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShNormal& operator%=(const ShGeneric<1, T2>&);
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
  
  template<typename T2>
  ShNormal(const ShGeneric<3, T2>& other);
  ShNormal(const ShNormal<3, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShNormal(const ShNormal<3, Binding, T2, Swizzled>& other);
  ShNormal(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShNormal(T data[3]);
  
  ShNormal(T, T, T);
  template<typename T2, typename T3, typename T4>
  ShNormal(const ShGeneric<1, T2>&, const ShGeneric<1, T3>&, const ShGeneric<1, T4>&);
  
  ~ShNormal();

  
  template<typename T2>
  ShNormal& operator=(const ShGeneric<3, T2>& other);
  
  template<typename T2>
  ShNormal& operator=(const ShNormal<3, Binding, T2, Swizzled>& other);
  ShNormal& operator=(const ShNormal<3, Binding, T, Swizzled>& other);

  
  template<typename T2>
  ShNormal& operator+=(const ShGeneric<3, T2>& right);
  
  template<typename T2>
  ShNormal& operator-=(const ShGeneric<3, T2>& right);
  
  template<typename T2>
  ShNormal& operator*=(const ShGeneric<3, T2>& right);
  
  template<typename T2>
  ShNormal& operator/=(const ShGeneric<3, T2>& right);
  
  template<typename T2>
  ShNormal& operator%=(const ShGeneric<3, T2>& right);
  ShNormal& operator*=(T);
  ShNormal& operator/=(T);
  ShNormal& operator%=(T);
  ShNormal& operator+=(T);
  ShNormal& operator-=(T);
  
  template<typename T2>
  ShNormal& operator+=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShNormal& operator-=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShNormal& operator*=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShNormal& operator/=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShNormal& operator%=(const ShGeneric<1, T2>&);
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
  
  template<typename T2>
  ShNormal(const ShGeneric<4, T2>& other);
  ShNormal(const ShNormal<4, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShNormal(const ShNormal<4, Binding, T2, Swizzled>& other);
  ShNormal(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShNormal(T data[4]);
  
  ShNormal(T, T, T, T);
  template<typename T2, typename T3, typename T4, typename T5>
  ShNormal(const ShGeneric<1, T2>&, const ShGeneric<1, T3>&, const ShGeneric<1, T4>&, const ShGeneric<1, T5>&);
  
  ~ShNormal();

  
  template<typename T2>
  ShNormal& operator=(const ShGeneric<4, T2>& other);
  
  template<typename T2>
  ShNormal& operator=(const ShNormal<4, Binding, T2, Swizzled>& other);
  ShNormal& operator=(const ShNormal<4, Binding, T, Swizzled>& other);

  
  template<typename T2>
  ShNormal& operator+=(const ShGeneric<4, T2>& right);
  
  template<typename T2>
  ShNormal& operator-=(const ShGeneric<4, T2>& right);
  
  template<typename T2>
  ShNormal& operator*=(const ShGeneric<4, T2>& right);
  
  template<typename T2>
  ShNormal& operator/=(const ShGeneric<4, T2>& right);
  
  template<typename T2>
  ShNormal& operator%=(const ShGeneric<4, T2>& right);
  ShNormal& operator*=(T);
  ShNormal& operator/=(T);
  ShNormal& operator%=(T);
  ShNormal& operator+=(T);
  ShNormal& operator-=(T);
  
  template<typename T2>
  ShNormal& operator+=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShNormal& operator-=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShNormal& operator*=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShNormal& operator/=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShNormal& operator%=(const ShGeneric<1, T2>&);
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

typedef ShNormal<1, SH_INPUT, ShInterval<double> > ShInputNormal1i_d;
typedef ShNormal<1, SH_OUTPUT, ShInterval<double> > ShOutputNormal1i_d;
typedef ShNormal<1, SH_INOUT, ShInterval<double> > ShInOutNormal1i_d;
typedef ShNormal<1, SH_TEMP, ShInterval<double> > ShNormal1i_d;
typedef ShNormal<1, SH_CONST, ShInterval<double> > ShConstNormal1i_d;
typedef ShNormal<2, SH_INPUT, ShInterval<double> > ShInputNormal2i_d;
typedef ShNormal<2, SH_OUTPUT, ShInterval<double> > ShOutputNormal2i_d;
typedef ShNormal<2, SH_INOUT, ShInterval<double> > ShInOutNormal2i_d;
typedef ShNormal<2, SH_TEMP, ShInterval<double> > ShNormal2i_d;
typedef ShNormal<2, SH_CONST, ShInterval<double> > ShConstNormal2i_d;
typedef ShNormal<3, SH_INPUT, ShInterval<double> > ShInputNormal3i_d;
typedef ShNormal<3, SH_OUTPUT, ShInterval<double> > ShOutputNormal3i_d;
typedef ShNormal<3, SH_INOUT, ShInterval<double> > ShInOutNormal3i_d;
typedef ShNormal<3, SH_TEMP, ShInterval<double> > ShNormal3i_d;
typedef ShNormal<3, SH_CONST, ShInterval<double> > ShConstNormal3i_d;
typedef ShNormal<4, SH_INPUT, ShInterval<double> > ShInputNormal4i_d;
typedef ShNormal<4, SH_OUTPUT, ShInterval<double> > ShOutputNormal4i_d;
typedef ShNormal<4, SH_INOUT, ShInterval<double> > ShInOutNormal4i_d;
typedef ShNormal<4, SH_TEMP, ShInterval<double> > ShNormal4i_d;
typedef ShNormal<4, SH_CONST, ShInterval<double> > ShConstNormal4i_d;


typedef ShNormal<1, SH_INPUT, short> ShInputNormal1s;
typedef ShNormal<1, SH_OUTPUT, short> ShOutputNormal1s;
typedef ShNormal<1, SH_INOUT, short> ShInOutNormal1s;
typedef ShNormal<1, SH_TEMP, short> ShNormal1s;
typedef ShNormal<1, SH_CONST, short> ShConstNormal1s;
typedef ShNormal<2, SH_INPUT, short> ShInputNormal2s;
typedef ShNormal<2, SH_OUTPUT, short> ShOutputNormal2s;
typedef ShNormal<2, SH_INOUT, short> ShInOutNormal2s;
typedef ShNormal<2, SH_TEMP, short> ShNormal2s;
typedef ShNormal<2, SH_CONST, short> ShConstNormal2s;
typedef ShNormal<3, SH_INPUT, short> ShInputNormal3s;
typedef ShNormal<3, SH_OUTPUT, short> ShOutputNormal3s;
typedef ShNormal<3, SH_INOUT, short> ShInOutNormal3s;
typedef ShNormal<3, SH_TEMP, short> ShNormal3s;
typedef ShNormal<3, SH_CONST, short> ShConstNormal3s;
typedef ShNormal<4, SH_INPUT, short> ShInputNormal4s;
typedef ShNormal<4, SH_OUTPUT, short> ShOutputNormal4s;
typedef ShNormal<4, SH_INOUT, short> ShInOutNormal4s;
typedef ShNormal<4, SH_TEMP, short> ShNormal4s;
typedef ShNormal<4, SH_CONST, short> ShConstNormal4s;


typedef ShNormal<1, SH_INPUT, int> ShInputNormal1i;
typedef ShNormal<1, SH_OUTPUT, int> ShOutputNormal1i;
typedef ShNormal<1, SH_INOUT, int> ShInOutNormal1i;
typedef ShNormal<1, SH_TEMP, int> ShNormal1i;
typedef ShNormal<1, SH_CONST, int> ShConstNormal1i;
typedef ShNormal<2, SH_INPUT, int> ShInputNormal2i;
typedef ShNormal<2, SH_OUTPUT, int> ShOutputNormal2i;
typedef ShNormal<2, SH_INOUT, int> ShInOutNormal2i;
typedef ShNormal<2, SH_TEMP, int> ShNormal2i;
typedef ShNormal<2, SH_CONST, int> ShConstNormal2i;
typedef ShNormal<3, SH_INPUT, int> ShInputNormal3i;
typedef ShNormal<3, SH_OUTPUT, int> ShOutputNormal3i;
typedef ShNormal<3, SH_INOUT, int> ShInOutNormal3i;
typedef ShNormal<3, SH_TEMP, int> ShNormal3i;
typedef ShNormal<3, SH_CONST, int> ShConstNormal3i;
typedef ShNormal<4, SH_INPUT, int> ShInputNormal4i;
typedef ShNormal<4, SH_OUTPUT, int> ShOutputNormal4i;
typedef ShNormal<4, SH_INOUT, int> ShInOutNormal4i;
typedef ShNormal<4, SH_TEMP, int> ShNormal4i;
typedef ShNormal<4, SH_CONST, int> ShConstNormal4i;


typedef ShNormal<1, SH_INPUT, double> ShInputNormal1d;
typedef ShNormal<1, SH_OUTPUT, double> ShOutputNormal1d;
typedef ShNormal<1, SH_INOUT, double> ShInOutNormal1d;
typedef ShNormal<1, SH_TEMP, double> ShNormal1d;
typedef ShNormal<1, SH_CONST, double> ShConstNormal1d;
typedef ShNormal<2, SH_INPUT, double> ShInputNormal2d;
typedef ShNormal<2, SH_OUTPUT, double> ShOutputNormal2d;
typedef ShNormal<2, SH_INOUT, double> ShInOutNormal2d;
typedef ShNormal<2, SH_TEMP, double> ShNormal2d;
typedef ShNormal<2, SH_CONST, double> ShConstNormal2d;
typedef ShNormal<3, SH_INPUT, double> ShInputNormal3d;
typedef ShNormal<3, SH_OUTPUT, double> ShOutputNormal3d;
typedef ShNormal<3, SH_INOUT, double> ShInOutNormal3d;
typedef ShNormal<3, SH_TEMP, double> ShNormal3d;
typedef ShNormal<3, SH_CONST, double> ShConstNormal3d;
typedef ShNormal<4, SH_INPUT, double> ShInputNormal4d;
typedef ShNormal<4, SH_OUTPUT, double> ShOutputNormal4d;
typedef ShNormal<4, SH_INOUT, double> ShInOutNormal4d;
typedef ShNormal<4, SH_TEMP, double> ShNormal4d;
typedef ShNormal<4, SH_CONST, double> ShConstNormal4d;


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


typedef ShNormal<1, SH_INPUT, char> ShInputNormal1b;
typedef ShNormal<1, SH_OUTPUT, char> ShOutputNormal1b;
typedef ShNormal<1, SH_INOUT, char> ShInOutNormal1b;
typedef ShNormal<1, SH_TEMP, char> ShNormal1b;
typedef ShNormal<1, SH_CONST, char> ShConstNormal1b;
typedef ShNormal<2, SH_INPUT, char> ShInputNormal2b;
typedef ShNormal<2, SH_OUTPUT, char> ShOutputNormal2b;
typedef ShNormal<2, SH_INOUT, char> ShInOutNormal2b;
typedef ShNormal<2, SH_TEMP, char> ShNormal2b;
typedef ShNormal<2, SH_CONST, char> ShConstNormal2b;
typedef ShNormal<3, SH_INPUT, char> ShInputNormal3b;
typedef ShNormal<3, SH_OUTPUT, char> ShOutputNormal3b;
typedef ShNormal<3, SH_INOUT, char> ShInOutNormal3b;
typedef ShNormal<3, SH_TEMP, char> ShNormal3b;
typedef ShNormal<3, SH_CONST, char> ShConstNormal3b;
typedef ShNormal<4, SH_INPUT, char> ShInputNormal4b;
typedef ShNormal<4, SH_OUTPUT, char> ShOutputNormal4b;
typedef ShNormal<4, SH_INOUT, char> ShInOutNormal4b;
typedef ShNormal<4, SH_TEMP, char> ShNormal4b;
typedef ShNormal<4, SH_CONST, char> ShConstNormal4b;


typedef ShNormal<1, SH_INPUT, ShInterval<float> > ShInputNormal1i_f;
typedef ShNormal<1, SH_OUTPUT, ShInterval<float> > ShOutputNormal1i_f;
typedef ShNormal<1, SH_INOUT, ShInterval<float> > ShInOutNormal1i_f;
typedef ShNormal<1, SH_TEMP, ShInterval<float> > ShNormal1i_f;
typedef ShNormal<1, SH_CONST, ShInterval<float> > ShConstNormal1i_f;
typedef ShNormal<2, SH_INPUT, ShInterval<float> > ShInputNormal2i_f;
typedef ShNormal<2, SH_OUTPUT, ShInterval<float> > ShOutputNormal2i_f;
typedef ShNormal<2, SH_INOUT, ShInterval<float> > ShInOutNormal2i_f;
typedef ShNormal<2, SH_TEMP, ShInterval<float> > ShNormal2i_f;
typedef ShNormal<2, SH_CONST, ShInterval<float> > ShConstNormal2i_f;
typedef ShNormal<3, SH_INPUT, ShInterval<float> > ShInputNormal3i_f;
typedef ShNormal<3, SH_OUTPUT, ShInterval<float> > ShOutputNormal3i_f;
typedef ShNormal<3, SH_INOUT, ShInterval<float> > ShInOutNormal3i_f;
typedef ShNormal<3, SH_TEMP, ShInterval<float> > ShNormal3i_f;
typedef ShNormal<3, SH_CONST, ShInterval<float> > ShConstNormal3i_f;
typedef ShNormal<4, SH_INPUT, ShInterval<float> > ShInputNormal4i_f;
typedef ShNormal<4, SH_OUTPUT, ShInterval<float> > ShOutputNormal4i_f;
typedef ShNormal<4, SH_INOUT, ShInterval<float> > ShInOutNormal4i_f;
typedef ShNormal<4, SH_TEMP, ShInterval<float> > ShNormal4i_f;
typedef ShNormal<4, SH_CONST, ShInterval<float> > ShConstNormal4i_f;



} // namespace SH
#include "ShNormalImpl.hpp"

#endif // SH_SHNORMAL_HPP
