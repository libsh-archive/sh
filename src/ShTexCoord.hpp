// WARNING - DO NOT EDIT THIS FILE:
// This file was automatically generated from scripts/ShTexCoord.hpp.py.
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

#ifndef SH_SHTEXCOORD_HPP
#define SH_SHTEXCOORD_HPP

#include "ShAttrib.hpp"
namespace SH {

/** A texture coordinate.
 * 
 *
 */
template<int N, ShBindingType Binding, typename T=float, bool Swizzled=false>
class ShTexCoord : public ShAttrib<N, Binding, T, Swizzled> {
public:
  ShTexCoord();
  
  template<typename T2>
  ShTexCoord(const ShGeneric<N, T2>& other);
  ShTexCoord(const ShTexCoord<N, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShTexCoord(const ShTexCoord<N, Binding, T2, Swizzled>& other);
  ShTexCoord(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShTexCoord(T data[N]);
  
  ~ShTexCoord();

  
  template<typename T2>
  ShTexCoord& operator=(const ShGeneric<N, T2>& other);
  
  template<typename T2>
  ShTexCoord& operator=(const ShTexCoord<N, Binding, T2, Swizzled>& other);
  ShTexCoord& operator=(const ShTexCoord<N, Binding, T, Swizzled>& other);

  
  template<typename T2>
  ShTexCoord& operator+=(const ShGeneric<N, T2>& right);
  
  template<typename T2>
  ShTexCoord& operator-=(const ShGeneric<N, T2>& right);
  
  template<typename T2>
  ShTexCoord& operator*=(const ShGeneric<N, T2>& right);
  
  template<typename T2>
  ShTexCoord& operator/=(const ShGeneric<N, T2>& right);
  
  template<typename T2>
  ShTexCoord& operator%=(const ShGeneric<N, T2>& right);
  ShTexCoord& operator*=(T);
  ShTexCoord& operator/=(T);
  ShTexCoord& operator%=(T);
  ShTexCoord& operator+=(T);
  ShTexCoord& operator-=(T);
  
  template<typename T2>
  ShTexCoord& operator+=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShTexCoord& operator-=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShTexCoord& operator*=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShTexCoord& operator/=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShTexCoord& operator%=(const ShGeneric<1, T2>&);
  ShTexCoord<1, Binding, T, true> operator()(int) const;
  ShTexCoord<2, Binding, T, true> operator()(int, int) const;
  ShTexCoord<3, Binding, T, true> operator()(int, int, int) const;
  ShTexCoord<4, Binding, T, true> operator()(int, int, int, int) const;
  ShTexCoord<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShTexCoord<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShTexCoord operator-() const;
  typedef T ValueType;
  static const int typesize = N;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_TEXCOORD;

  typedef ShTexCoord<N, SH_INPUT, T> InputType;
  typedef ShTexCoord<N, SH_OUTPUT, T> OutputType;
  typedef ShTexCoord<N, SH_INOUT, T> InOutType;
  typedef ShTexCoord<N, SH_TEMP, T> TempType;
  typedef ShTexCoord<N, SH_CONST, T> ConstType;
private:
  typedef ShAttrib<N, Binding, T, Swizzled> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShTexCoord<1, Binding, T, Swizzled> : public ShAttrib<1, Binding, T, Swizzled> {
public:
  ShTexCoord();
  
  template<typename T2>
  ShTexCoord(const ShGeneric<1, T2>& other);
  ShTexCoord(const ShTexCoord<1, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShTexCoord(const ShTexCoord<1, Binding, T2, Swizzled>& other);
  ShTexCoord(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShTexCoord(T data[1]);
  
  ShTexCoord(T);
  
  ~ShTexCoord();

  
  template<typename T2>
  ShTexCoord& operator=(const ShGeneric<1, T2>& other);
  
  template<typename T2>
  ShTexCoord& operator=(const ShTexCoord<1, Binding, T2, Swizzled>& other);
  ShTexCoord& operator=(const ShTexCoord<1, Binding, T, Swizzled>& other);

  ShTexCoord& operator=(T other);

  
  template<typename T2>
  ShTexCoord& operator+=(const ShGeneric<1, T2>& right);
  
  template<typename T2>
  ShTexCoord& operator-=(const ShGeneric<1, T2>& right);
  
  template<typename T2>
  ShTexCoord& operator*=(const ShGeneric<1, T2>& right);
  
  template<typename T2>
  ShTexCoord& operator/=(const ShGeneric<1, T2>& right);
  
  template<typename T2>
  ShTexCoord& operator%=(const ShGeneric<1, T2>& right);
  ShTexCoord& operator*=(T);
  ShTexCoord& operator/=(T);
  ShTexCoord& operator%=(T);
  ShTexCoord& operator+=(T);
  ShTexCoord& operator-=(T);
  ShTexCoord<1, Binding, T, true> operator()(int) const;
  ShTexCoord<2, Binding, T, true> operator()(int, int) const;
  ShTexCoord<3, Binding, T, true> operator()(int, int, int) const;
  ShTexCoord<4, Binding, T, true> operator()(int, int, int, int) const;
  ShTexCoord<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShTexCoord<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShTexCoord operator-() const;
  typedef T ValueType;
  static const int typesize = 1;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_TEXCOORD;

  typedef ShTexCoord<1, SH_INPUT, T> InputType;
  typedef ShTexCoord<1, SH_OUTPUT, T> OutputType;
  typedef ShTexCoord<1, SH_INOUT, T> InOutType;
  typedef ShTexCoord<1, SH_TEMP, T> TempType;
  typedef ShTexCoord<1, SH_CONST, T> ConstType;
private:
  typedef ShAttrib<1, Binding, T, Swizzled> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShTexCoord<2, Binding, T, Swizzled> : public ShAttrib<2, Binding, T, Swizzled> {
public:
  ShTexCoord();
  
  template<typename T2>
  ShTexCoord(const ShGeneric<2, T2>& other);
  ShTexCoord(const ShTexCoord<2, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShTexCoord(const ShTexCoord<2, Binding, T2, Swizzled>& other);
  ShTexCoord(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShTexCoord(T data[2]);
  
  ShTexCoord(T, T);
  ShTexCoord(const ShGeneric<1, T>&, const ShGeneric<1, T>&);
  
  ~ShTexCoord();

  
  template<typename T2>
  ShTexCoord& operator=(const ShGeneric<2, T2>& other);
  
  template<typename T2>
  ShTexCoord& operator=(const ShTexCoord<2, Binding, T2, Swizzled>& other);
  ShTexCoord& operator=(const ShTexCoord<2, Binding, T, Swizzled>& other);

  
  template<typename T2>
  ShTexCoord& operator+=(const ShGeneric<2, T2>& right);
  
  template<typename T2>
  ShTexCoord& operator-=(const ShGeneric<2, T2>& right);
  
  template<typename T2>
  ShTexCoord& operator*=(const ShGeneric<2, T2>& right);
  
  template<typename T2>
  ShTexCoord& operator/=(const ShGeneric<2, T2>& right);
  
  template<typename T2>
  ShTexCoord& operator%=(const ShGeneric<2, T2>& right);
  ShTexCoord& operator*=(T);
  ShTexCoord& operator/=(T);
  ShTexCoord& operator%=(T);
  ShTexCoord& operator+=(T);
  ShTexCoord& operator-=(T);
  
  template<typename T2>
  ShTexCoord& operator+=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShTexCoord& operator-=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShTexCoord& operator*=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShTexCoord& operator/=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShTexCoord& operator%=(const ShGeneric<1, T2>&);
  ShTexCoord<1, Binding, T, true> operator()(int) const;
  ShTexCoord<2, Binding, T, true> operator()(int, int) const;
  ShTexCoord<3, Binding, T, true> operator()(int, int, int) const;
  ShTexCoord<4, Binding, T, true> operator()(int, int, int, int) const;
  ShTexCoord<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShTexCoord<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShTexCoord operator-() const;
  typedef T ValueType;
  static const int typesize = 2;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_TEXCOORD;

  typedef ShTexCoord<2, SH_INPUT, T> InputType;
  typedef ShTexCoord<2, SH_OUTPUT, T> OutputType;
  typedef ShTexCoord<2, SH_INOUT, T> InOutType;
  typedef ShTexCoord<2, SH_TEMP, T> TempType;
  typedef ShTexCoord<2, SH_CONST, T> ConstType;
private:
  typedef ShAttrib<2, Binding, T, Swizzled> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShTexCoord<3, Binding, T, Swizzled> : public ShAttrib<3, Binding, T, Swizzled> {
public:
  ShTexCoord();
  
  template<typename T2>
  ShTexCoord(const ShGeneric<3, T2>& other);
  ShTexCoord(const ShTexCoord<3, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShTexCoord(const ShTexCoord<3, Binding, T2, Swizzled>& other);
  ShTexCoord(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShTexCoord(T data[3]);
  
  ShTexCoord(T, T, T);
  ShTexCoord(const ShGeneric<1, T>&, const ShGeneric<1, T>&, const ShGeneric<1, T>&);
  
  ~ShTexCoord();

  
  template<typename T2>
  ShTexCoord& operator=(const ShGeneric<3, T2>& other);
  
  template<typename T2>
  ShTexCoord& operator=(const ShTexCoord<3, Binding, T2, Swizzled>& other);
  ShTexCoord& operator=(const ShTexCoord<3, Binding, T, Swizzled>& other);

  
  template<typename T2>
  ShTexCoord& operator+=(const ShGeneric<3, T2>& right);
  
  template<typename T2>
  ShTexCoord& operator-=(const ShGeneric<3, T2>& right);
  
  template<typename T2>
  ShTexCoord& operator*=(const ShGeneric<3, T2>& right);
  
  template<typename T2>
  ShTexCoord& operator/=(const ShGeneric<3, T2>& right);
  
  template<typename T2>
  ShTexCoord& operator%=(const ShGeneric<3, T2>& right);
  ShTexCoord& operator*=(T);
  ShTexCoord& operator/=(T);
  ShTexCoord& operator%=(T);
  ShTexCoord& operator+=(T);
  ShTexCoord& operator-=(T);
  
  template<typename T2>
  ShTexCoord& operator+=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShTexCoord& operator-=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShTexCoord& operator*=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShTexCoord& operator/=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShTexCoord& operator%=(const ShGeneric<1, T2>&);
  ShTexCoord<1, Binding, T, true> operator()(int) const;
  ShTexCoord<2, Binding, T, true> operator()(int, int) const;
  ShTexCoord<3, Binding, T, true> operator()(int, int, int) const;
  ShTexCoord<4, Binding, T, true> operator()(int, int, int, int) const;
  ShTexCoord<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShTexCoord<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShTexCoord operator-() const;
  typedef T ValueType;
  static const int typesize = 3;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_TEXCOORD;

  typedef ShTexCoord<3, SH_INPUT, T> InputType;
  typedef ShTexCoord<3, SH_OUTPUT, T> OutputType;
  typedef ShTexCoord<3, SH_INOUT, T> InOutType;
  typedef ShTexCoord<3, SH_TEMP, T> TempType;
  typedef ShTexCoord<3, SH_CONST, T> ConstType;
private:
  typedef ShAttrib<3, Binding, T, Swizzled> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShTexCoord<4, Binding, T, Swizzled> : public ShAttrib<4, Binding, T, Swizzled> {
public:
  ShTexCoord();
  
  template<typename T2>
  ShTexCoord(const ShGeneric<4, T2>& other);
  ShTexCoord(const ShTexCoord<4, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShTexCoord(const ShTexCoord<4, Binding, T2, Swizzled>& other);
  ShTexCoord(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShTexCoord(T data[4]);
  
  ShTexCoord(T, T, T, T);
  ShTexCoord(const ShGeneric<1, T>&, const ShGeneric<1, T>&, const ShGeneric<1, T>&, const ShGeneric<1, T>&);
  
  ~ShTexCoord();

  
  template<typename T2>
  ShTexCoord& operator=(const ShGeneric<4, T2>& other);
  
  template<typename T2>
  ShTexCoord& operator=(const ShTexCoord<4, Binding, T2, Swizzled>& other);
  ShTexCoord& operator=(const ShTexCoord<4, Binding, T, Swizzled>& other);

  
  template<typename T2>
  ShTexCoord& operator+=(const ShGeneric<4, T2>& right);
  
  template<typename T2>
  ShTexCoord& operator-=(const ShGeneric<4, T2>& right);
  
  template<typename T2>
  ShTexCoord& operator*=(const ShGeneric<4, T2>& right);
  
  template<typename T2>
  ShTexCoord& operator/=(const ShGeneric<4, T2>& right);
  
  template<typename T2>
  ShTexCoord& operator%=(const ShGeneric<4, T2>& right);
  ShTexCoord& operator*=(T);
  ShTexCoord& operator/=(T);
  ShTexCoord& operator%=(T);
  ShTexCoord& operator+=(T);
  ShTexCoord& operator-=(T);
  
  template<typename T2>
  ShTexCoord& operator+=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShTexCoord& operator-=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShTexCoord& operator*=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShTexCoord& operator/=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShTexCoord& operator%=(const ShGeneric<1, T2>&);
  ShTexCoord<1, Binding, T, true> operator()(int) const;
  ShTexCoord<2, Binding, T, true> operator()(int, int) const;
  ShTexCoord<3, Binding, T, true> operator()(int, int, int) const;
  ShTexCoord<4, Binding, T, true> operator()(int, int, int, int) const;
  ShTexCoord<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShTexCoord<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShTexCoord operator-() const;
  typedef T ValueType;
  static const int typesize = 4;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_TEXCOORD;

  typedef ShTexCoord<4, SH_INPUT, T> InputType;
  typedef ShTexCoord<4, SH_OUTPUT, T> OutputType;
  typedef ShTexCoord<4, SH_INOUT, T> InOutType;
  typedef ShTexCoord<4, SH_TEMP, T> TempType;
  typedef ShTexCoord<4, SH_CONST, T> ConstType;
private:
  typedef ShAttrib<4, Binding, T, Swizzled> ParentType;
};

typedef ShTexCoord<1, SH_INPUT, ShInterval<double> > ShInputTexCoord1i_d;
typedef ShTexCoord<1, SH_OUTPUT, ShInterval<double> > ShOutputTexCoord1i_d;
typedef ShTexCoord<1, SH_INOUT, ShInterval<double> > ShInOutTexCoord1i_d;
typedef ShTexCoord<1, SH_TEMP, ShInterval<double> > ShTexCoord1i_d;
typedef ShTexCoord<1, SH_CONST, ShInterval<double> > ShConstTexCoord1i_d;
typedef ShTexCoord<2, SH_INPUT, ShInterval<double> > ShInputTexCoord2i_d;
typedef ShTexCoord<2, SH_OUTPUT, ShInterval<double> > ShOutputTexCoord2i_d;
typedef ShTexCoord<2, SH_INOUT, ShInterval<double> > ShInOutTexCoord2i_d;
typedef ShTexCoord<2, SH_TEMP, ShInterval<double> > ShTexCoord2i_d;
typedef ShTexCoord<2, SH_CONST, ShInterval<double> > ShConstTexCoord2i_d;
typedef ShTexCoord<3, SH_INPUT, ShInterval<double> > ShInputTexCoord3i_d;
typedef ShTexCoord<3, SH_OUTPUT, ShInterval<double> > ShOutputTexCoord3i_d;
typedef ShTexCoord<3, SH_INOUT, ShInterval<double> > ShInOutTexCoord3i_d;
typedef ShTexCoord<3, SH_TEMP, ShInterval<double> > ShTexCoord3i_d;
typedef ShTexCoord<3, SH_CONST, ShInterval<double> > ShConstTexCoord3i_d;
typedef ShTexCoord<4, SH_INPUT, ShInterval<double> > ShInputTexCoord4i_d;
typedef ShTexCoord<4, SH_OUTPUT, ShInterval<double> > ShOutputTexCoord4i_d;
typedef ShTexCoord<4, SH_INOUT, ShInterval<double> > ShInOutTexCoord4i_d;
typedef ShTexCoord<4, SH_TEMP, ShInterval<double> > ShTexCoord4i_d;
typedef ShTexCoord<4, SH_CONST, ShInterval<double> > ShConstTexCoord4i_d;


typedef ShTexCoord<1, SH_INPUT, short> ShInputTexCoord1s;
typedef ShTexCoord<1, SH_OUTPUT, short> ShOutputTexCoord1s;
typedef ShTexCoord<1, SH_INOUT, short> ShInOutTexCoord1s;
typedef ShTexCoord<1, SH_TEMP, short> ShTexCoord1s;
typedef ShTexCoord<1, SH_CONST, short> ShConstTexCoord1s;
typedef ShTexCoord<2, SH_INPUT, short> ShInputTexCoord2s;
typedef ShTexCoord<2, SH_OUTPUT, short> ShOutputTexCoord2s;
typedef ShTexCoord<2, SH_INOUT, short> ShInOutTexCoord2s;
typedef ShTexCoord<2, SH_TEMP, short> ShTexCoord2s;
typedef ShTexCoord<2, SH_CONST, short> ShConstTexCoord2s;
typedef ShTexCoord<3, SH_INPUT, short> ShInputTexCoord3s;
typedef ShTexCoord<3, SH_OUTPUT, short> ShOutputTexCoord3s;
typedef ShTexCoord<3, SH_INOUT, short> ShInOutTexCoord3s;
typedef ShTexCoord<3, SH_TEMP, short> ShTexCoord3s;
typedef ShTexCoord<3, SH_CONST, short> ShConstTexCoord3s;
typedef ShTexCoord<4, SH_INPUT, short> ShInputTexCoord4s;
typedef ShTexCoord<4, SH_OUTPUT, short> ShOutputTexCoord4s;
typedef ShTexCoord<4, SH_INOUT, short> ShInOutTexCoord4s;
typedef ShTexCoord<4, SH_TEMP, short> ShTexCoord4s;
typedef ShTexCoord<4, SH_CONST, short> ShConstTexCoord4s;


typedef ShTexCoord<1, SH_INPUT, int> ShInputTexCoord1i;
typedef ShTexCoord<1, SH_OUTPUT, int> ShOutputTexCoord1i;
typedef ShTexCoord<1, SH_INOUT, int> ShInOutTexCoord1i;
typedef ShTexCoord<1, SH_TEMP, int> ShTexCoord1i;
typedef ShTexCoord<1, SH_CONST, int> ShConstTexCoord1i;
typedef ShTexCoord<2, SH_INPUT, int> ShInputTexCoord2i;
typedef ShTexCoord<2, SH_OUTPUT, int> ShOutputTexCoord2i;
typedef ShTexCoord<2, SH_INOUT, int> ShInOutTexCoord2i;
typedef ShTexCoord<2, SH_TEMP, int> ShTexCoord2i;
typedef ShTexCoord<2, SH_CONST, int> ShConstTexCoord2i;
typedef ShTexCoord<3, SH_INPUT, int> ShInputTexCoord3i;
typedef ShTexCoord<3, SH_OUTPUT, int> ShOutputTexCoord3i;
typedef ShTexCoord<3, SH_INOUT, int> ShInOutTexCoord3i;
typedef ShTexCoord<3, SH_TEMP, int> ShTexCoord3i;
typedef ShTexCoord<3, SH_CONST, int> ShConstTexCoord3i;
typedef ShTexCoord<4, SH_INPUT, int> ShInputTexCoord4i;
typedef ShTexCoord<4, SH_OUTPUT, int> ShOutputTexCoord4i;
typedef ShTexCoord<4, SH_INOUT, int> ShInOutTexCoord4i;
typedef ShTexCoord<4, SH_TEMP, int> ShTexCoord4i;
typedef ShTexCoord<4, SH_CONST, int> ShConstTexCoord4i;


typedef ShTexCoord<1, SH_INPUT, double> ShInputTexCoord1d;
typedef ShTexCoord<1, SH_OUTPUT, double> ShOutputTexCoord1d;
typedef ShTexCoord<1, SH_INOUT, double> ShInOutTexCoord1d;
typedef ShTexCoord<1, SH_TEMP, double> ShTexCoord1d;
typedef ShTexCoord<1, SH_CONST, double> ShConstTexCoord1d;
typedef ShTexCoord<2, SH_INPUT, double> ShInputTexCoord2d;
typedef ShTexCoord<2, SH_OUTPUT, double> ShOutputTexCoord2d;
typedef ShTexCoord<2, SH_INOUT, double> ShInOutTexCoord2d;
typedef ShTexCoord<2, SH_TEMP, double> ShTexCoord2d;
typedef ShTexCoord<2, SH_CONST, double> ShConstTexCoord2d;
typedef ShTexCoord<3, SH_INPUT, double> ShInputTexCoord3d;
typedef ShTexCoord<3, SH_OUTPUT, double> ShOutputTexCoord3d;
typedef ShTexCoord<3, SH_INOUT, double> ShInOutTexCoord3d;
typedef ShTexCoord<3, SH_TEMP, double> ShTexCoord3d;
typedef ShTexCoord<3, SH_CONST, double> ShConstTexCoord3d;
typedef ShTexCoord<4, SH_INPUT, double> ShInputTexCoord4d;
typedef ShTexCoord<4, SH_OUTPUT, double> ShOutputTexCoord4d;
typedef ShTexCoord<4, SH_INOUT, double> ShInOutTexCoord4d;
typedef ShTexCoord<4, SH_TEMP, double> ShTexCoord4d;
typedef ShTexCoord<4, SH_CONST, double> ShConstTexCoord4d;


typedef ShTexCoord<1, SH_INPUT, float> ShInputTexCoord1f;
typedef ShTexCoord<1, SH_OUTPUT, float> ShOutputTexCoord1f;
typedef ShTexCoord<1, SH_INOUT, float> ShInOutTexCoord1f;
typedef ShTexCoord<1, SH_TEMP, float> ShTexCoord1f;
typedef ShTexCoord<1, SH_CONST, float> ShConstTexCoord1f;
typedef ShTexCoord<2, SH_INPUT, float> ShInputTexCoord2f;
typedef ShTexCoord<2, SH_OUTPUT, float> ShOutputTexCoord2f;
typedef ShTexCoord<2, SH_INOUT, float> ShInOutTexCoord2f;
typedef ShTexCoord<2, SH_TEMP, float> ShTexCoord2f;
typedef ShTexCoord<2, SH_CONST, float> ShConstTexCoord2f;
typedef ShTexCoord<3, SH_INPUT, float> ShInputTexCoord3f;
typedef ShTexCoord<3, SH_OUTPUT, float> ShOutputTexCoord3f;
typedef ShTexCoord<3, SH_INOUT, float> ShInOutTexCoord3f;
typedef ShTexCoord<3, SH_TEMP, float> ShTexCoord3f;
typedef ShTexCoord<3, SH_CONST, float> ShConstTexCoord3f;
typedef ShTexCoord<4, SH_INPUT, float> ShInputTexCoord4f;
typedef ShTexCoord<4, SH_OUTPUT, float> ShOutputTexCoord4f;
typedef ShTexCoord<4, SH_INOUT, float> ShInOutTexCoord4f;
typedef ShTexCoord<4, SH_TEMP, float> ShTexCoord4f;
typedef ShTexCoord<4, SH_CONST, float> ShConstTexCoord4f;


typedef ShTexCoord<1, SH_INPUT, char> ShInputTexCoord1b;
typedef ShTexCoord<1, SH_OUTPUT, char> ShOutputTexCoord1b;
typedef ShTexCoord<1, SH_INOUT, char> ShInOutTexCoord1b;
typedef ShTexCoord<1, SH_TEMP, char> ShTexCoord1b;
typedef ShTexCoord<1, SH_CONST, char> ShConstTexCoord1b;
typedef ShTexCoord<2, SH_INPUT, char> ShInputTexCoord2b;
typedef ShTexCoord<2, SH_OUTPUT, char> ShOutputTexCoord2b;
typedef ShTexCoord<2, SH_INOUT, char> ShInOutTexCoord2b;
typedef ShTexCoord<2, SH_TEMP, char> ShTexCoord2b;
typedef ShTexCoord<2, SH_CONST, char> ShConstTexCoord2b;
typedef ShTexCoord<3, SH_INPUT, char> ShInputTexCoord3b;
typedef ShTexCoord<3, SH_OUTPUT, char> ShOutputTexCoord3b;
typedef ShTexCoord<3, SH_INOUT, char> ShInOutTexCoord3b;
typedef ShTexCoord<3, SH_TEMP, char> ShTexCoord3b;
typedef ShTexCoord<3, SH_CONST, char> ShConstTexCoord3b;
typedef ShTexCoord<4, SH_INPUT, char> ShInputTexCoord4b;
typedef ShTexCoord<4, SH_OUTPUT, char> ShOutputTexCoord4b;
typedef ShTexCoord<4, SH_INOUT, char> ShInOutTexCoord4b;
typedef ShTexCoord<4, SH_TEMP, char> ShTexCoord4b;
typedef ShTexCoord<4, SH_CONST, char> ShConstTexCoord4b;


typedef ShTexCoord<1, SH_INPUT, ShInterval<float> > ShInputTexCoord1i_f;
typedef ShTexCoord<1, SH_OUTPUT, ShInterval<float> > ShOutputTexCoord1i_f;
typedef ShTexCoord<1, SH_INOUT, ShInterval<float> > ShInOutTexCoord1i_f;
typedef ShTexCoord<1, SH_TEMP, ShInterval<float> > ShTexCoord1i_f;
typedef ShTexCoord<1, SH_CONST, ShInterval<float> > ShConstTexCoord1i_f;
typedef ShTexCoord<2, SH_INPUT, ShInterval<float> > ShInputTexCoord2i_f;
typedef ShTexCoord<2, SH_OUTPUT, ShInterval<float> > ShOutputTexCoord2i_f;
typedef ShTexCoord<2, SH_INOUT, ShInterval<float> > ShInOutTexCoord2i_f;
typedef ShTexCoord<2, SH_TEMP, ShInterval<float> > ShTexCoord2i_f;
typedef ShTexCoord<2, SH_CONST, ShInterval<float> > ShConstTexCoord2i_f;
typedef ShTexCoord<3, SH_INPUT, ShInterval<float> > ShInputTexCoord3i_f;
typedef ShTexCoord<3, SH_OUTPUT, ShInterval<float> > ShOutputTexCoord3i_f;
typedef ShTexCoord<3, SH_INOUT, ShInterval<float> > ShInOutTexCoord3i_f;
typedef ShTexCoord<3, SH_TEMP, ShInterval<float> > ShTexCoord3i_f;
typedef ShTexCoord<3, SH_CONST, ShInterval<float> > ShConstTexCoord3i_f;
typedef ShTexCoord<4, SH_INPUT, ShInterval<float> > ShInputTexCoord4i_f;
typedef ShTexCoord<4, SH_OUTPUT, ShInterval<float> > ShOutputTexCoord4i_f;
typedef ShTexCoord<4, SH_INOUT, ShInterval<float> > ShInOutTexCoord4i_f;
typedef ShTexCoord<4, SH_TEMP, ShInterval<float> > ShTexCoord4i_f;
typedef ShTexCoord<4, SH_CONST, ShInterval<float> > ShConstTexCoord4i_f;



} // namespace SH
#include "ShTexCoordImpl.hpp"

#endif // SH_SHTEXCOORD_HPP
