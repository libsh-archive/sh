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

#ifndef SH_DO_NOT_INCLUDE_GENERIC_IMPL
#  define SH_DO_NOT_INCLUDE_GENERIC_IMPL
#  include "ShGeneric.hpp"
#  undef SH_DO_NOT_INCLUDE_GENERIC_IMPL
#else
#  include "ShGeneric.hpp"
#endif
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
  
  template<typename T2>
  ShAttrib(const ShGeneric<N, T2>& other);
  ShAttrib(const ShAttrib<N, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShAttrib(const ShAttrib<N, Binding, T2, Swizzled>& other);
  ShAttrib(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShAttrib(T data[N]);
  
  ~ShAttrib();

  
  template<typename T2>
  ShAttrib& operator=(const ShGeneric<N, T2>& other);
  
  template<typename T2>
  ShAttrib& operator=(const ShAttrib<N, Binding, T2, Swizzled>& other);
  ShAttrib& operator=(const ShAttrib<N, Binding, T, Swizzled>& other);

  
  template<typename T2>
  ShAttrib& operator+=(const ShGeneric<N, T2>& right);
  
  template<typename T2>
  ShAttrib& operator-=(const ShGeneric<N, T2>& right);
  
  template<typename T2>
  ShAttrib& operator*=(const ShGeneric<N, T2>& right);
  
  template<typename T2>
  ShAttrib& operator/=(const ShGeneric<N, T2>& right);
  
  template<typename T2>
  ShAttrib& operator%=(const ShGeneric<N, T2>& right);
  ShAttrib& operator*=(T);
  ShAttrib& operator/=(T);
  ShAttrib& operator%=(T);
  ShAttrib& operator+=(T);
  ShAttrib& operator-=(T);
  
  template<typename T2>
  ShAttrib& operator+=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShAttrib& operator-=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShAttrib& operator*=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShAttrib& operator/=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShAttrib& operator%=(const ShGeneric<1, T2>&);
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
  
  template<typename T2>
  ShAttrib(const ShGeneric<1, T2>& other);
  ShAttrib(const ShAttrib<1, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShAttrib(const ShAttrib<1, Binding, T2, Swizzled>& other);
  ShAttrib(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShAttrib(T data[1]);
  
  ShAttrib(T);
  
  ~ShAttrib();

  
  template<typename T2>
  ShAttrib& operator=(const ShGeneric<1, T2>& other);
  
  template<typename T2>
  ShAttrib& operator=(const ShAttrib<1, Binding, T2, Swizzled>& other);
  ShAttrib& operator=(const ShAttrib<1, Binding, T, Swizzled>& other);

  ShAttrib& operator=(T other);

  
  template<typename T2>
  ShAttrib& operator+=(const ShGeneric<1, T2>& right);
  
  template<typename T2>
  ShAttrib& operator-=(const ShGeneric<1, T2>& right);
  
  template<typename T2>
  ShAttrib& operator*=(const ShGeneric<1, T2>& right);
  
  template<typename T2>
  ShAttrib& operator/=(const ShGeneric<1, T2>& right);
  
  template<typename T2>
  ShAttrib& operator%=(const ShGeneric<1, T2>& right);
  ShAttrib& operator*=(T);
  ShAttrib& operator/=(T);
  ShAttrib& operator%=(T);
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
  
  template<typename T2>
  ShAttrib(const ShGeneric<2, T2>& other);
  ShAttrib(const ShAttrib<2, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShAttrib(const ShAttrib<2, Binding, T2, Swizzled>& other);
  ShAttrib(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShAttrib(T data[2]);
  
  ShAttrib(T, T);
  template<typename T2, typename T3>
  ShAttrib(const ShGeneric<1, T2>&, const ShGeneric<1, T3>&);
  
  ~ShAttrib();

  
  template<typename T2>
  ShAttrib& operator=(const ShGeneric<2, T2>& other);
  
  template<typename T2>
  ShAttrib& operator=(const ShAttrib<2, Binding, T2, Swizzled>& other);
  ShAttrib& operator=(const ShAttrib<2, Binding, T, Swizzled>& other);

  
  template<typename T2>
  ShAttrib& operator+=(const ShGeneric<2, T2>& right);
  
  template<typename T2>
  ShAttrib& operator-=(const ShGeneric<2, T2>& right);
  
  template<typename T2>
  ShAttrib& operator*=(const ShGeneric<2, T2>& right);
  
  template<typename T2>
  ShAttrib& operator/=(const ShGeneric<2, T2>& right);
  
  template<typename T2>
  ShAttrib& operator%=(const ShGeneric<2, T2>& right);
  ShAttrib& operator*=(T);
  ShAttrib& operator/=(T);
  ShAttrib& operator%=(T);
  ShAttrib& operator+=(T);
  ShAttrib& operator-=(T);
  
  template<typename T2>
  ShAttrib& operator+=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShAttrib& operator-=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShAttrib& operator*=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShAttrib& operator/=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShAttrib& operator%=(const ShGeneric<1, T2>&);
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
  
  template<typename T2>
  ShAttrib(const ShGeneric<3, T2>& other);
  ShAttrib(const ShAttrib<3, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShAttrib(const ShAttrib<3, Binding, T2, Swizzled>& other);
  ShAttrib(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShAttrib(T data[3]);
  
  ShAttrib(T, T, T);
  template<typename T2, typename T3, typename T4>
  ShAttrib(const ShGeneric<1, T2>&, const ShGeneric<1, T3>&, const ShGeneric<1, T4>&);
  
  ~ShAttrib();

  
  template<typename T2>
  ShAttrib& operator=(const ShGeneric<3, T2>& other);
  
  template<typename T2>
  ShAttrib& operator=(const ShAttrib<3, Binding, T2, Swizzled>& other);
  ShAttrib& operator=(const ShAttrib<3, Binding, T, Swizzled>& other);

  
  template<typename T2>
  ShAttrib& operator+=(const ShGeneric<3, T2>& right);
  
  template<typename T2>
  ShAttrib& operator-=(const ShGeneric<3, T2>& right);
  
  template<typename T2>
  ShAttrib& operator*=(const ShGeneric<3, T2>& right);
  
  template<typename T2>
  ShAttrib& operator/=(const ShGeneric<3, T2>& right);
  
  template<typename T2>
  ShAttrib& operator%=(const ShGeneric<3, T2>& right);
  ShAttrib& operator*=(T);
  ShAttrib& operator/=(T);
  ShAttrib& operator%=(T);
  ShAttrib& operator+=(T);
  ShAttrib& operator-=(T);
  
  template<typename T2>
  ShAttrib& operator+=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShAttrib& operator-=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShAttrib& operator*=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShAttrib& operator/=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShAttrib& operator%=(const ShGeneric<1, T2>&);
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
  
  template<typename T2>
  ShAttrib(const ShGeneric<4, T2>& other);
  ShAttrib(const ShAttrib<4, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShAttrib(const ShAttrib<4, Binding, T2, Swizzled>& other);
  ShAttrib(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShAttrib(T data[4]);
  
  ShAttrib(T, T, T, T);
  template<typename T2, typename T3, typename T4, typename T5>
  ShAttrib(const ShGeneric<1, T2>&, const ShGeneric<1, T3>&, const ShGeneric<1, T4>&, const ShGeneric<1, T5>&);
  
  ~ShAttrib();

  
  template<typename T2>
  ShAttrib& operator=(const ShGeneric<4, T2>& other);
  
  template<typename T2>
  ShAttrib& operator=(const ShAttrib<4, Binding, T2, Swizzled>& other);
  ShAttrib& operator=(const ShAttrib<4, Binding, T, Swizzled>& other);

  
  template<typename T2>
  ShAttrib& operator+=(const ShGeneric<4, T2>& right);
  
  template<typename T2>
  ShAttrib& operator-=(const ShGeneric<4, T2>& right);
  
  template<typename T2>
  ShAttrib& operator*=(const ShGeneric<4, T2>& right);
  
  template<typename T2>
  ShAttrib& operator/=(const ShGeneric<4, T2>& right);
  
  template<typename T2>
  ShAttrib& operator%=(const ShGeneric<4, T2>& right);
  ShAttrib& operator*=(T);
  ShAttrib& operator/=(T);
  ShAttrib& operator%=(T);
  ShAttrib& operator+=(T);
  ShAttrib& operator-=(T);
  
  template<typename T2>
  ShAttrib& operator+=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShAttrib& operator-=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShAttrib& operator*=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShAttrib& operator/=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShAttrib& operator%=(const ShGeneric<1, T2>&);
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

typedef ShAttrib<1, SH_INPUT, ShInterval<double> > ShInputAttrib1i_d;
typedef ShAttrib<1, SH_OUTPUT, ShInterval<double> > ShOutputAttrib1i_d;
typedef ShAttrib<1, SH_INOUT, ShInterval<double> > ShInOutAttrib1i_d;
typedef ShAttrib<1, SH_TEMP, ShInterval<double> > ShAttrib1i_d;
typedef ShAttrib<1, SH_CONST, ShInterval<double> > ShConstAttrib1i_d;
typedef ShAttrib<2, SH_INPUT, ShInterval<double> > ShInputAttrib2i_d;
typedef ShAttrib<2, SH_OUTPUT, ShInterval<double> > ShOutputAttrib2i_d;
typedef ShAttrib<2, SH_INOUT, ShInterval<double> > ShInOutAttrib2i_d;
typedef ShAttrib<2, SH_TEMP, ShInterval<double> > ShAttrib2i_d;
typedef ShAttrib<2, SH_CONST, ShInterval<double> > ShConstAttrib2i_d;
typedef ShAttrib<3, SH_INPUT, ShInterval<double> > ShInputAttrib3i_d;
typedef ShAttrib<3, SH_OUTPUT, ShInterval<double> > ShOutputAttrib3i_d;
typedef ShAttrib<3, SH_INOUT, ShInterval<double> > ShInOutAttrib3i_d;
typedef ShAttrib<3, SH_TEMP, ShInterval<double> > ShAttrib3i_d;
typedef ShAttrib<3, SH_CONST, ShInterval<double> > ShConstAttrib3i_d;
typedef ShAttrib<4, SH_INPUT, ShInterval<double> > ShInputAttrib4i_d;
typedef ShAttrib<4, SH_OUTPUT, ShInterval<double> > ShOutputAttrib4i_d;
typedef ShAttrib<4, SH_INOUT, ShInterval<double> > ShInOutAttrib4i_d;
typedef ShAttrib<4, SH_TEMP, ShInterval<double> > ShAttrib4i_d;
typedef ShAttrib<4, SH_CONST, ShInterval<double> > ShConstAttrib4i_d;


typedef ShAttrib<1, SH_INPUT, short> ShInputAttrib1s;
typedef ShAttrib<1, SH_OUTPUT, short> ShOutputAttrib1s;
typedef ShAttrib<1, SH_INOUT, short> ShInOutAttrib1s;
typedef ShAttrib<1, SH_TEMP, short> ShAttrib1s;
typedef ShAttrib<1, SH_CONST, short> ShConstAttrib1s;
typedef ShAttrib<2, SH_INPUT, short> ShInputAttrib2s;
typedef ShAttrib<2, SH_OUTPUT, short> ShOutputAttrib2s;
typedef ShAttrib<2, SH_INOUT, short> ShInOutAttrib2s;
typedef ShAttrib<2, SH_TEMP, short> ShAttrib2s;
typedef ShAttrib<2, SH_CONST, short> ShConstAttrib2s;
typedef ShAttrib<3, SH_INPUT, short> ShInputAttrib3s;
typedef ShAttrib<3, SH_OUTPUT, short> ShOutputAttrib3s;
typedef ShAttrib<3, SH_INOUT, short> ShInOutAttrib3s;
typedef ShAttrib<3, SH_TEMP, short> ShAttrib3s;
typedef ShAttrib<3, SH_CONST, short> ShConstAttrib3s;
typedef ShAttrib<4, SH_INPUT, short> ShInputAttrib4s;
typedef ShAttrib<4, SH_OUTPUT, short> ShOutputAttrib4s;
typedef ShAttrib<4, SH_INOUT, short> ShInOutAttrib4s;
typedef ShAttrib<4, SH_TEMP, short> ShAttrib4s;
typedef ShAttrib<4, SH_CONST, short> ShConstAttrib4s;


typedef ShAttrib<1, SH_INPUT, int> ShInputAttrib1i;
typedef ShAttrib<1, SH_OUTPUT, int> ShOutputAttrib1i;
typedef ShAttrib<1, SH_INOUT, int> ShInOutAttrib1i;
typedef ShAttrib<1, SH_TEMP, int> ShAttrib1i;
typedef ShAttrib<1, SH_CONST, int> ShConstAttrib1i;
typedef ShAttrib<2, SH_INPUT, int> ShInputAttrib2i;
typedef ShAttrib<2, SH_OUTPUT, int> ShOutputAttrib2i;
typedef ShAttrib<2, SH_INOUT, int> ShInOutAttrib2i;
typedef ShAttrib<2, SH_TEMP, int> ShAttrib2i;
typedef ShAttrib<2, SH_CONST, int> ShConstAttrib2i;
typedef ShAttrib<3, SH_INPUT, int> ShInputAttrib3i;
typedef ShAttrib<3, SH_OUTPUT, int> ShOutputAttrib3i;
typedef ShAttrib<3, SH_INOUT, int> ShInOutAttrib3i;
typedef ShAttrib<3, SH_TEMP, int> ShAttrib3i;
typedef ShAttrib<3, SH_CONST, int> ShConstAttrib3i;
typedef ShAttrib<4, SH_INPUT, int> ShInputAttrib4i;
typedef ShAttrib<4, SH_OUTPUT, int> ShOutputAttrib4i;
typedef ShAttrib<4, SH_INOUT, int> ShInOutAttrib4i;
typedef ShAttrib<4, SH_TEMP, int> ShAttrib4i;
typedef ShAttrib<4, SH_CONST, int> ShConstAttrib4i;


typedef ShAttrib<1, SH_INPUT, double> ShInputAttrib1d;
typedef ShAttrib<1, SH_OUTPUT, double> ShOutputAttrib1d;
typedef ShAttrib<1, SH_INOUT, double> ShInOutAttrib1d;
typedef ShAttrib<1, SH_TEMP, double> ShAttrib1d;
typedef ShAttrib<1, SH_CONST, double> ShConstAttrib1d;
typedef ShAttrib<2, SH_INPUT, double> ShInputAttrib2d;
typedef ShAttrib<2, SH_OUTPUT, double> ShOutputAttrib2d;
typedef ShAttrib<2, SH_INOUT, double> ShInOutAttrib2d;
typedef ShAttrib<2, SH_TEMP, double> ShAttrib2d;
typedef ShAttrib<2, SH_CONST, double> ShConstAttrib2d;
typedef ShAttrib<3, SH_INPUT, double> ShInputAttrib3d;
typedef ShAttrib<3, SH_OUTPUT, double> ShOutputAttrib3d;
typedef ShAttrib<3, SH_INOUT, double> ShInOutAttrib3d;
typedef ShAttrib<3, SH_TEMP, double> ShAttrib3d;
typedef ShAttrib<3, SH_CONST, double> ShConstAttrib3d;
typedef ShAttrib<4, SH_INPUT, double> ShInputAttrib4d;
typedef ShAttrib<4, SH_OUTPUT, double> ShOutputAttrib4d;
typedef ShAttrib<4, SH_INOUT, double> ShInOutAttrib4d;
typedef ShAttrib<4, SH_TEMP, double> ShAttrib4d;
typedef ShAttrib<4, SH_CONST, double> ShConstAttrib4d;


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


typedef ShAttrib<1, SH_INPUT, char> ShInputAttrib1b;
typedef ShAttrib<1, SH_OUTPUT, char> ShOutputAttrib1b;
typedef ShAttrib<1, SH_INOUT, char> ShInOutAttrib1b;
typedef ShAttrib<1, SH_TEMP, char> ShAttrib1b;
typedef ShAttrib<1, SH_CONST, char> ShConstAttrib1b;
typedef ShAttrib<2, SH_INPUT, char> ShInputAttrib2b;
typedef ShAttrib<2, SH_OUTPUT, char> ShOutputAttrib2b;
typedef ShAttrib<2, SH_INOUT, char> ShInOutAttrib2b;
typedef ShAttrib<2, SH_TEMP, char> ShAttrib2b;
typedef ShAttrib<2, SH_CONST, char> ShConstAttrib2b;
typedef ShAttrib<3, SH_INPUT, char> ShInputAttrib3b;
typedef ShAttrib<3, SH_OUTPUT, char> ShOutputAttrib3b;
typedef ShAttrib<3, SH_INOUT, char> ShInOutAttrib3b;
typedef ShAttrib<3, SH_TEMP, char> ShAttrib3b;
typedef ShAttrib<3, SH_CONST, char> ShConstAttrib3b;
typedef ShAttrib<4, SH_INPUT, char> ShInputAttrib4b;
typedef ShAttrib<4, SH_OUTPUT, char> ShOutputAttrib4b;
typedef ShAttrib<4, SH_INOUT, char> ShInOutAttrib4b;
typedef ShAttrib<4, SH_TEMP, char> ShAttrib4b;
typedef ShAttrib<4, SH_CONST, char> ShConstAttrib4b;


typedef ShAttrib<1, SH_INPUT, ShInterval<float> > ShInputAttrib1i_f;
typedef ShAttrib<1, SH_OUTPUT, ShInterval<float> > ShOutputAttrib1i_f;
typedef ShAttrib<1, SH_INOUT, ShInterval<float> > ShInOutAttrib1i_f;
typedef ShAttrib<1, SH_TEMP, ShInterval<float> > ShAttrib1i_f;
typedef ShAttrib<1, SH_CONST, ShInterval<float> > ShConstAttrib1i_f;
typedef ShAttrib<2, SH_INPUT, ShInterval<float> > ShInputAttrib2i_f;
typedef ShAttrib<2, SH_OUTPUT, ShInterval<float> > ShOutputAttrib2i_f;
typedef ShAttrib<2, SH_INOUT, ShInterval<float> > ShInOutAttrib2i_f;
typedef ShAttrib<2, SH_TEMP, ShInterval<float> > ShAttrib2i_f;
typedef ShAttrib<2, SH_CONST, ShInterval<float> > ShConstAttrib2i_f;
typedef ShAttrib<3, SH_INPUT, ShInterval<float> > ShInputAttrib3i_f;
typedef ShAttrib<3, SH_OUTPUT, ShInterval<float> > ShOutputAttrib3i_f;
typedef ShAttrib<3, SH_INOUT, ShInterval<float> > ShInOutAttrib3i_f;
typedef ShAttrib<3, SH_TEMP, ShInterval<float> > ShAttrib3i_f;
typedef ShAttrib<3, SH_CONST, ShInterval<float> > ShConstAttrib3i_f;
typedef ShAttrib<4, SH_INPUT, ShInterval<float> > ShInputAttrib4i_f;
typedef ShAttrib<4, SH_OUTPUT, ShInterval<float> > ShOutputAttrib4i_f;
typedef ShAttrib<4, SH_INOUT, ShInterval<float> > ShInOutAttrib4i_f;
typedef ShAttrib<4, SH_TEMP, ShInterval<float> > ShAttrib4i_f;
typedef ShAttrib<4, SH_CONST, ShInterval<float> > ShConstAttrib4i_f;



} // namespace SH
#include "ShGenericImpl.hpp"
#include "ShAttribImpl.hpp"

#endif // SH_SHATTRIB_HPP
