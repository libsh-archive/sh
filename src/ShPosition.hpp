// WARNING - DO NOT EDIT THIS FILE:
// This file was automatically generated from scripts/ShPosition.hpp.py.
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

#ifndef SH_SHPOSITION_HPP
#define SH_SHPOSITION_HPP

#include "ShPoint.hpp"
namespace SH {

/** A vertex or fragment position.
 * 
 * This is semantically a point, but is bound preferentially to the position
 * input and output of the rasterizer or to the vertex position when
 * used in vertex and fragment shaders.
 * 
 *
 */
template<int N, ShBindingType Binding, typename T=float, bool Swizzled=false>
class ShPosition : public ShPoint<N, Binding, T, Swizzled> {
public:
  ShPosition();
  
  template<typename T2>
  ShPosition(const ShGeneric<N, T2>& other);
  ShPosition(const ShPosition<N, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShPosition(const ShPosition<N, Binding, T2, Swizzled>& other);
  ShPosition(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShPosition(T data[N]);
  
  ~ShPosition();

  
  template<typename T2>
  ShPosition& operator=(const ShGeneric<N, T2>& other);
  
  template<typename T2>
  ShPosition& operator=(const ShPosition<N, Binding, T2, Swizzled>& other);
  ShPosition& operator=(const ShPosition<N, Binding, T, Swizzled>& other);

  
  template<typename T2>
  ShPosition& operator+=(const ShGeneric<N, T2>& right);
  
  template<typename T2>
  ShPosition& operator-=(const ShGeneric<N, T2>& right);
  
  template<typename T2>
  ShPosition& operator*=(const ShGeneric<N, T2>& right);
  
  template<typename T2>
  ShPosition& operator/=(const ShGeneric<N, T2>& right);
  
  template<typename T2>
  ShPosition& operator%=(const ShGeneric<N, T2>& right);
  ShPosition& operator*=(T);
  ShPosition& operator/=(T);
  ShPosition& operator%=(T);
  ShPosition& operator+=(T);
  ShPosition& operator-=(T);
  
  template<typename T2>
  ShPosition& operator+=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShPosition& operator-=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShPosition& operator*=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShPosition& operator/=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShPosition& operator%=(const ShGeneric<1, T2>&);
  ShPosition<1, Binding, T, true> operator()(int) const;
  ShPosition<2, Binding, T, true> operator()(int, int) const;
  ShPosition<3, Binding, T, true> operator()(int, int, int) const;
  ShPosition<4, Binding, T, true> operator()(int, int, int, int) const;
  ShPosition<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShPosition<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShPosition operator-() const;
  typedef T ValueType;
  static const int typesize = N;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_POSITION;

  typedef ShPosition<N, SH_INPUT, T> InputType;
  typedef ShPosition<N, SH_OUTPUT, T> OutputType;
  typedef ShPosition<N, SH_INOUT, T> InOutType;
  typedef ShPosition<N, SH_TEMP, T> TempType;
  typedef ShPosition<N, SH_CONST, T> ConstType;
private:
  typedef ShPoint<N, Binding, T, Swizzled> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShPosition<1, Binding, T, Swizzled> : public ShPoint<1, Binding, T, Swizzled> {
public:
  ShPosition();
  
  template<typename T2>
  ShPosition(const ShGeneric<1, T2>& other);
  ShPosition(const ShPosition<1, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShPosition(const ShPosition<1, Binding, T2, Swizzled>& other);
  ShPosition(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShPosition(T data[1]);
  
  ShPosition(T);
  
  ~ShPosition();

  
  template<typename T2>
  ShPosition& operator=(const ShGeneric<1, T2>& other);
  
  template<typename T2>
  ShPosition& operator=(const ShPosition<1, Binding, T2, Swizzled>& other);
  ShPosition& operator=(const ShPosition<1, Binding, T, Swizzled>& other);

  ShPosition& operator=(T other);

  
  template<typename T2>
  ShPosition& operator+=(const ShGeneric<1, T2>& right);
  
  template<typename T2>
  ShPosition& operator-=(const ShGeneric<1, T2>& right);
  
  template<typename T2>
  ShPosition& operator*=(const ShGeneric<1, T2>& right);
  
  template<typename T2>
  ShPosition& operator/=(const ShGeneric<1, T2>& right);
  
  template<typename T2>
  ShPosition& operator%=(const ShGeneric<1, T2>& right);
  ShPosition& operator*=(T);
  ShPosition& operator/=(T);
  ShPosition& operator%=(T);
  ShPosition& operator+=(T);
  ShPosition& operator-=(T);
  ShPosition<1, Binding, T, true> operator()(int) const;
  ShPosition<2, Binding, T, true> operator()(int, int) const;
  ShPosition<3, Binding, T, true> operator()(int, int, int) const;
  ShPosition<4, Binding, T, true> operator()(int, int, int, int) const;
  ShPosition<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShPosition<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShPosition operator-() const;
  typedef T ValueType;
  static const int typesize = 1;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_POSITION;

  typedef ShPosition<1, SH_INPUT, T> InputType;
  typedef ShPosition<1, SH_OUTPUT, T> OutputType;
  typedef ShPosition<1, SH_INOUT, T> InOutType;
  typedef ShPosition<1, SH_TEMP, T> TempType;
  typedef ShPosition<1, SH_CONST, T> ConstType;
private:
  typedef ShPoint<1, Binding, T, Swizzled> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShPosition<2, Binding, T, Swizzled> : public ShPoint<2, Binding, T, Swizzled> {
public:
  ShPosition();
  
  template<typename T2>
  ShPosition(const ShGeneric<2, T2>& other);
  ShPosition(const ShPosition<2, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShPosition(const ShPosition<2, Binding, T2, Swizzled>& other);
  ShPosition(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShPosition(T data[2]);
  
  ShPosition(T, T);
  ShPosition(const ShGeneric<1, T>&, const ShGeneric<1, T>&);
  
  ~ShPosition();

  
  template<typename T2>
  ShPosition& operator=(const ShGeneric<2, T2>& other);
  
  template<typename T2>
  ShPosition& operator=(const ShPosition<2, Binding, T2, Swizzled>& other);
  ShPosition& operator=(const ShPosition<2, Binding, T, Swizzled>& other);

  
  template<typename T2>
  ShPosition& operator+=(const ShGeneric<2, T2>& right);
  
  template<typename T2>
  ShPosition& operator-=(const ShGeneric<2, T2>& right);
  
  template<typename T2>
  ShPosition& operator*=(const ShGeneric<2, T2>& right);
  
  template<typename T2>
  ShPosition& operator/=(const ShGeneric<2, T2>& right);
  
  template<typename T2>
  ShPosition& operator%=(const ShGeneric<2, T2>& right);
  ShPosition& operator*=(T);
  ShPosition& operator/=(T);
  ShPosition& operator%=(T);
  ShPosition& operator+=(T);
  ShPosition& operator-=(T);
  
  template<typename T2>
  ShPosition& operator+=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShPosition& operator-=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShPosition& operator*=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShPosition& operator/=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShPosition& operator%=(const ShGeneric<1, T2>&);
  ShPosition<1, Binding, T, true> operator()(int) const;
  ShPosition<2, Binding, T, true> operator()(int, int) const;
  ShPosition<3, Binding, T, true> operator()(int, int, int) const;
  ShPosition<4, Binding, T, true> operator()(int, int, int, int) const;
  ShPosition<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShPosition<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShPosition operator-() const;
  typedef T ValueType;
  static const int typesize = 2;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_POSITION;

  typedef ShPosition<2, SH_INPUT, T> InputType;
  typedef ShPosition<2, SH_OUTPUT, T> OutputType;
  typedef ShPosition<2, SH_INOUT, T> InOutType;
  typedef ShPosition<2, SH_TEMP, T> TempType;
  typedef ShPosition<2, SH_CONST, T> ConstType;
private:
  typedef ShPoint<2, Binding, T, Swizzled> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShPosition<3, Binding, T, Swizzled> : public ShPoint<3, Binding, T, Swizzled> {
public:
  ShPosition();
  
  template<typename T2>
  ShPosition(const ShGeneric<3, T2>& other);
  ShPosition(const ShPosition<3, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShPosition(const ShPosition<3, Binding, T2, Swizzled>& other);
  ShPosition(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShPosition(T data[3]);
  
  ShPosition(T, T, T);
  ShPosition(const ShGeneric<1, T>&, const ShGeneric<1, T>&, const ShGeneric<1, T>&);
  
  ~ShPosition();

  
  template<typename T2>
  ShPosition& operator=(const ShGeneric<3, T2>& other);
  
  template<typename T2>
  ShPosition& operator=(const ShPosition<3, Binding, T2, Swizzled>& other);
  ShPosition& operator=(const ShPosition<3, Binding, T, Swizzled>& other);

  
  template<typename T2>
  ShPosition& operator+=(const ShGeneric<3, T2>& right);
  
  template<typename T2>
  ShPosition& operator-=(const ShGeneric<3, T2>& right);
  
  template<typename T2>
  ShPosition& operator*=(const ShGeneric<3, T2>& right);
  
  template<typename T2>
  ShPosition& operator/=(const ShGeneric<3, T2>& right);
  
  template<typename T2>
  ShPosition& operator%=(const ShGeneric<3, T2>& right);
  ShPosition& operator*=(T);
  ShPosition& operator/=(T);
  ShPosition& operator%=(T);
  ShPosition& operator+=(T);
  ShPosition& operator-=(T);
  
  template<typename T2>
  ShPosition& operator+=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShPosition& operator-=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShPosition& operator*=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShPosition& operator/=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShPosition& operator%=(const ShGeneric<1, T2>&);
  ShPosition<1, Binding, T, true> operator()(int) const;
  ShPosition<2, Binding, T, true> operator()(int, int) const;
  ShPosition<3, Binding, T, true> operator()(int, int, int) const;
  ShPosition<4, Binding, T, true> operator()(int, int, int, int) const;
  ShPosition<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShPosition<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShPosition operator-() const;
  typedef T ValueType;
  static const int typesize = 3;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_POSITION;

  typedef ShPosition<3, SH_INPUT, T> InputType;
  typedef ShPosition<3, SH_OUTPUT, T> OutputType;
  typedef ShPosition<3, SH_INOUT, T> InOutType;
  typedef ShPosition<3, SH_TEMP, T> TempType;
  typedef ShPosition<3, SH_CONST, T> ConstType;
private:
  typedef ShPoint<3, Binding, T, Swizzled> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShPosition<4, Binding, T, Swizzled> : public ShPoint<4, Binding, T, Swizzled> {
public:
  ShPosition();
  
  template<typename T2>
  ShPosition(const ShGeneric<4, T2>& other);
  ShPosition(const ShPosition<4, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShPosition(const ShPosition<4, Binding, T2, Swizzled>& other);
  ShPosition(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShPosition(T data[4]);
  
  ShPosition(T, T, T, T);
  ShPosition(const ShGeneric<1, T>&, const ShGeneric<1, T>&, const ShGeneric<1, T>&, const ShGeneric<1, T>&);
  
  ~ShPosition();

  
  template<typename T2>
  ShPosition& operator=(const ShGeneric<4, T2>& other);
  
  template<typename T2>
  ShPosition& operator=(const ShPosition<4, Binding, T2, Swizzled>& other);
  ShPosition& operator=(const ShPosition<4, Binding, T, Swizzled>& other);

  
  template<typename T2>
  ShPosition& operator+=(const ShGeneric<4, T2>& right);
  
  template<typename T2>
  ShPosition& operator-=(const ShGeneric<4, T2>& right);
  
  template<typename T2>
  ShPosition& operator*=(const ShGeneric<4, T2>& right);
  
  template<typename T2>
  ShPosition& operator/=(const ShGeneric<4, T2>& right);
  
  template<typename T2>
  ShPosition& operator%=(const ShGeneric<4, T2>& right);
  ShPosition& operator*=(T);
  ShPosition& operator/=(T);
  ShPosition& operator%=(T);
  ShPosition& operator+=(T);
  ShPosition& operator-=(T);
  
  template<typename T2>
  ShPosition& operator+=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShPosition& operator-=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShPosition& operator*=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShPosition& operator/=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShPosition& operator%=(const ShGeneric<1, T2>&);
  ShPosition<1, Binding, T, true> operator()(int) const;
  ShPosition<2, Binding, T, true> operator()(int, int) const;
  ShPosition<3, Binding, T, true> operator()(int, int, int) const;
  ShPosition<4, Binding, T, true> operator()(int, int, int, int) const;
  ShPosition<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShPosition<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShPosition operator-() const;
  typedef T ValueType;
  static const int typesize = 4;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_POSITION;

  typedef ShPosition<4, SH_INPUT, T> InputType;
  typedef ShPosition<4, SH_OUTPUT, T> OutputType;
  typedef ShPosition<4, SH_INOUT, T> InOutType;
  typedef ShPosition<4, SH_TEMP, T> TempType;
  typedef ShPosition<4, SH_CONST, T> ConstType;
private:
  typedef ShPoint<4, Binding, T, Swizzled> ParentType;
};

typedef ShPosition<1, SH_INPUT, ShInterval<double> > ShInputPosition1i_d;
typedef ShPosition<1, SH_OUTPUT, ShInterval<double> > ShOutputPosition1i_d;
typedef ShPosition<1, SH_INOUT, ShInterval<double> > ShInOutPosition1i_d;
typedef ShPosition<1, SH_TEMP, ShInterval<double> > ShPosition1i_d;
typedef ShPosition<1, SH_CONST, ShInterval<double> > ShConstPosition1i_d;
typedef ShPosition<2, SH_INPUT, ShInterval<double> > ShInputPosition2i_d;
typedef ShPosition<2, SH_OUTPUT, ShInterval<double> > ShOutputPosition2i_d;
typedef ShPosition<2, SH_INOUT, ShInterval<double> > ShInOutPosition2i_d;
typedef ShPosition<2, SH_TEMP, ShInterval<double> > ShPosition2i_d;
typedef ShPosition<2, SH_CONST, ShInterval<double> > ShConstPosition2i_d;
typedef ShPosition<3, SH_INPUT, ShInterval<double> > ShInputPosition3i_d;
typedef ShPosition<3, SH_OUTPUT, ShInterval<double> > ShOutputPosition3i_d;
typedef ShPosition<3, SH_INOUT, ShInterval<double> > ShInOutPosition3i_d;
typedef ShPosition<3, SH_TEMP, ShInterval<double> > ShPosition3i_d;
typedef ShPosition<3, SH_CONST, ShInterval<double> > ShConstPosition3i_d;
typedef ShPosition<4, SH_INPUT, ShInterval<double> > ShInputPosition4i_d;
typedef ShPosition<4, SH_OUTPUT, ShInterval<double> > ShOutputPosition4i_d;
typedef ShPosition<4, SH_INOUT, ShInterval<double> > ShInOutPosition4i_d;
typedef ShPosition<4, SH_TEMP, ShInterval<double> > ShPosition4i_d;
typedef ShPosition<4, SH_CONST, ShInterval<double> > ShConstPosition4i_d;


typedef ShPosition<1, SH_INPUT, short> ShInputPosition1s;
typedef ShPosition<1, SH_OUTPUT, short> ShOutputPosition1s;
typedef ShPosition<1, SH_INOUT, short> ShInOutPosition1s;
typedef ShPosition<1, SH_TEMP, short> ShPosition1s;
typedef ShPosition<1, SH_CONST, short> ShConstPosition1s;
typedef ShPosition<2, SH_INPUT, short> ShInputPosition2s;
typedef ShPosition<2, SH_OUTPUT, short> ShOutputPosition2s;
typedef ShPosition<2, SH_INOUT, short> ShInOutPosition2s;
typedef ShPosition<2, SH_TEMP, short> ShPosition2s;
typedef ShPosition<2, SH_CONST, short> ShConstPosition2s;
typedef ShPosition<3, SH_INPUT, short> ShInputPosition3s;
typedef ShPosition<3, SH_OUTPUT, short> ShOutputPosition3s;
typedef ShPosition<3, SH_INOUT, short> ShInOutPosition3s;
typedef ShPosition<3, SH_TEMP, short> ShPosition3s;
typedef ShPosition<3, SH_CONST, short> ShConstPosition3s;
typedef ShPosition<4, SH_INPUT, short> ShInputPosition4s;
typedef ShPosition<4, SH_OUTPUT, short> ShOutputPosition4s;
typedef ShPosition<4, SH_INOUT, short> ShInOutPosition4s;
typedef ShPosition<4, SH_TEMP, short> ShPosition4s;
typedef ShPosition<4, SH_CONST, short> ShConstPosition4s;


typedef ShPosition<1, SH_INPUT, int> ShInputPosition1i;
typedef ShPosition<1, SH_OUTPUT, int> ShOutputPosition1i;
typedef ShPosition<1, SH_INOUT, int> ShInOutPosition1i;
typedef ShPosition<1, SH_TEMP, int> ShPosition1i;
typedef ShPosition<1, SH_CONST, int> ShConstPosition1i;
typedef ShPosition<2, SH_INPUT, int> ShInputPosition2i;
typedef ShPosition<2, SH_OUTPUT, int> ShOutputPosition2i;
typedef ShPosition<2, SH_INOUT, int> ShInOutPosition2i;
typedef ShPosition<2, SH_TEMP, int> ShPosition2i;
typedef ShPosition<2, SH_CONST, int> ShConstPosition2i;
typedef ShPosition<3, SH_INPUT, int> ShInputPosition3i;
typedef ShPosition<3, SH_OUTPUT, int> ShOutputPosition3i;
typedef ShPosition<3, SH_INOUT, int> ShInOutPosition3i;
typedef ShPosition<3, SH_TEMP, int> ShPosition3i;
typedef ShPosition<3, SH_CONST, int> ShConstPosition3i;
typedef ShPosition<4, SH_INPUT, int> ShInputPosition4i;
typedef ShPosition<4, SH_OUTPUT, int> ShOutputPosition4i;
typedef ShPosition<4, SH_INOUT, int> ShInOutPosition4i;
typedef ShPosition<4, SH_TEMP, int> ShPosition4i;
typedef ShPosition<4, SH_CONST, int> ShConstPosition4i;


typedef ShPosition<1, SH_INPUT, double> ShInputPosition1d;
typedef ShPosition<1, SH_OUTPUT, double> ShOutputPosition1d;
typedef ShPosition<1, SH_INOUT, double> ShInOutPosition1d;
typedef ShPosition<1, SH_TEMP, double> ShPosition1d;
typedef ShPosition<1, SH_CONST, double> ShConstPosition1d;
typedef ShPosition<2, SH_INPUT, double> ShInputPosition2d;
typedef ShPosition<2, SH_OUTPUT, double> ShOutputPosition2d;
typedef ShPosition<2, SH_INOUT, double> ShInOutPosition2d;
typedef ShPosition<2, SH_TEMP, double> ShPosition2d;
typedef ShPosition<2, SH_CONST, double> ShConstPosition2d;
typedef ShPosition<3, SH_INPUT, double> ShInputPosition3d;
typedef ShPosition<3, SH_OUTPUT, double> ShOutputPosition3d;
typedef ShPosition<3, SH_INOUT, double> ShInOutPosition3d;
typedef ShPosition<3, SH_TEMP, double> ShPosition3d;
typedef ShPosition<3, SH_CONST, double> ShConstPosition3d;
typedef ShPosition<4, SH_INPUT, double> ShInputPosition4d;
typedef ShPosition<4, SH_OUTPUT, double> ShOutputPosition4d;
typedef ShPosition<4, SH_INOUT, double> ShInOutPosition4d;
typedef ShPosition<4, SH_TEMP, double> ShPosition4d;
typedef ShPosition<4, SH_CONST, double> ShConstPosition4d;


typedef ShPosition<1, SH_INPUT, float> ShInputPosition1f;
typedef ShPosition<1, SH_OUTPUT, float> ShOutputPosition1f;
typedef ShPosition<1, SH_INOUT, float> ShInOutPosition1f;
typedef ShPosition<1, SH_TEMP, float> ShPosition1f;
typedef ShPosition<1, SH_CONST, float> ShConstPosition1f;
typedef ShPosition<2, SH_INPUT, float> ShInputPosition2f;
typedef ShPosition<2, SH_OUTPUT, float> ShOutputPosition2f;
typedef ShPosition<2, SH_INOUT, float> ShInOutPosition2f;
typedef ShPosition<2, SH_TEMP, float> ShPosition2f;
typedef ShPosition<2, SH_CONST, float> ShConstPosition2f;
typedef ShPosition<3, SH_INPUT, float> ShInputPosition3f;
typedef ShPosition<3, SH_OUTPUT, float> ShOutputPosition3f;
typedef ShPosition<3, SH_INOUT, float> ShInOutPosition3f;
typedef ShPosition<3, SH_TEMP, float> ShPosition3f;
typedef ShPosition<3, SH_CONST, float> ShConstPosition3f;
typedef ShPosition<4, SH_INPUT, float> ShInputPosition4f;
typedef ShPosition<4, SH_OUTPUT, float> ShOutputPosition4f;
typedef ShPosition<4, SH_INOUT, float> ShInOutPosition4f;
typedef ShPosition<4, SH_TEMP, float> ShPosition4f;
typedef ShPosition<4, SH_CONST, float> ShConstPosition4f;


typedef ShPosition<1, SH_INPUT, char> ShInputPosition1b;
typedef ShPosition<1, SH_OUTPUT, char> ShOutputPosition1b;
typedef ShPosition<1, SH_INOUT, char> ShInOutPosition1b;
typedef ShPosition<1, SH_TEMP, char> ShPosition1b;
typedef ShPosition<1, SH_CONST, char> ShConstPosition1b;
typedef ShPosition<2, SH_INPUT, char> ShInputPosition2b;
typedef ShPosition<2, SH_OUTPUT, char> ShOutputPosition2b;
typedef ShPosition<2, SH_INOUT, char> ShInOutPosition2b;
typedef ShPosition<2, SH_TEMP, char> ShPosition2b;
typedef ShPosition<2, SH_CONST, char> ShConstPosition2b;
typedef ShPosition<3, SH_INPUT, char> ShInputPosition3b;
typedef ShPosition<3, SH_OUTPUT, char> ShOutputPosition3b;
typedef ShPosition<3, SH_INOUT, char> ShInOutPosition3b;
typedef ShPosition<3, SH_TEMP, char> ShPosition3b;
typedef ShPosition<3, SH_CONST, char> ShConstPosition3b;
typedef ShPosition<4, SH_INPUT, char> ShInputPosition4b;
typedef ShPosition<4, SH_OUTPUT, char> ShOutputPosition4b;
typedef ShPosition<4, SH_INOUT, char> ShInOutPosition4b;
typedef ShPosition<4, SH_TEMP, char> ShPosition4b;
typedef ShPosition<4, SH_CONST, char> ShConstPosition4b;


typedef ShPosition<1, SH_INPUT, ShInterval<float> > ShInputPosition1i_f;
typedef ShPosition<1, SH_OUTPUT, ShInterval<float> > ShOutputPosition1i_f;
typedef ShPosition<1, SH_INOUT, ShInterval<float> > ShInOutPosition1i_f;
typedef ShPosition<1, SH_TEMP, ShInterval<float> > ShPosition1i_f;
typedef ShPosition<1, SH_CONST, ShInterval<float> > ShConstPosition1i_f;
typedef ShPosition<2, SH_INPUT, ShInterval<float> > ShInputPosition2i_f;
typedef ShPosition<2, SH_OUTPUT, ShInterval<float> > ShOutputPosition2i_f;
typedef ShPosition<2, SH_INOUT, ShInterval<float> > ShInOutPosition2i_f;
typedef ShPosition<2, SH_TEMP, ShInterval<float> > ShPosition2i_f;
typedef ShPosition<2, SH_CONST, ShInterval<float> > ShConstPosition2i_f;
typedef ShPosition<3, SH_INPUT, ShInterval<float> > ShInputPosition3i_f;
typedef ShPosition<3, SH_OUTPUT, ShInterval<float> > ShOutputPosition3i_f;
typedef ShPosition<3, SH_INOUT, ShInterval<float> > ShInOutPosition3i_f;
typedef ShPosition<3, SH_TEMP, ShInterval<float> > ShPosition3i_f;
typedef ShPosition<3, SH_CONST, ShInterval<float> > ShConstPosition3i_f;
typedef ShPosition<4, SH_INPUT, ShInterval<float> > ShInputPosition4i_f;
typedef ShPosition<4, SH_OUTPUT, ShInterval<float> > ShOutputPosition4i_f;
typedef ShPosition<4, SH_INOUT, ShInterval<float> > ShInOutPosition4i_f;
typedef ShPosition<4, SH_TEMP, ShInterval<float> > ShPosition4i_f;
typedef ShPosition<4, SH_CONST, ShInterval<float> > ShConstPosition4i_f;



} // namespace SH
#include "ShPositionImpl.hpp"

#endif // SH_SHPOSITION_HPP
