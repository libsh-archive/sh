// WARNING - DO NOT EDIT THIS FILE:
// This file was automatically generated from scripts/ShVector.hpp.py.
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

#ifndef SH_SHVECTOR_HPP
#define SH_SHVECTOR_HPP

#include "ShAttrib.hpp"
namespace SH {

/** A (geometric) n-vector.
 * 
 * By "vector" we mean a geometric vector, not just a tuple or an
 * array (unlike the unfortunate terminology in the standard
 * library).
 *
 */
template<int N, ShBindingType Binding, typename T=float, bool Swizzled=false>
class ShVector : public ShAttrib<N, Binding, T, Swizzled> {
public:
  typedef T ValueType;
  typedef typename ShHostType<T>::type H; 
  typedef H HostType; 
  typedef typename ShMemoryType<T>::type MemoryType; 
  static const int typesize = N;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_VECTOR;

  typedef ShVector<N, SH_INPUT, T> InputType;
  typedef ShVector<N, SH_OUTPUT, T> OutputType;
  typedef ShVector<N, SH_INOUT, T> InOutType;
  typedef ShVector<N, SH_TEMP, T> TempType;
  typedef ShVector<N, SH_CONST, T> ConstType;
  ShVector();
  
  template<typename T2>
  ShVector(const ShGeneric<N, T2>& other);
  ShVector(const ShVector<N, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShVector(const ShVector<N, Binding, T2, Swizzled>& other);
  ShVector(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShVector(H data[N]);
  
  ~ShVector();

  
  template<typename T2>
  ShVector& operator=(const ShGeneric<N, T2>& other);
  
  template<typename T2>
  ShVector& operator=(const ShVector<N, Binding, T2, Swizzled>& other);
  ShVector& operator=(const ShVector<N, Binding, T, Swizzled>& other);

  ShVector& operator=(const ShProgram& prg);

  
  template<typename T2>
  ShVector& operator+=(const ShGeneric<N, T2>& right);
  
  template<typename T2>
  ShVector& operator-=(const ShGeneric<N, T2>& right);
  
  template<typename T2>
  ShVector& operator*=(const ShGeneric<N, T2>& right);
  
  template<typename T2>
  ShVector& operator/=(const ShGeneric<N, T2>& right);
  
  template<typename T2>
  ShVector& operator%=(const ShGeneric<N, T2>& right);
  ShVector& operator*=(H);
  ShVector& operator/=(H);
  ShVector& operator%=(H);
  ShVector& operator+=(H);
  ShVector& operator-=(H);
  
  template<typename T2>
  ShVector& operator+=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShVector& operator-=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShVector& operator*=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShVector& operator/=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShVector& operator%=(const ShGeneric<1, T2>&);
  ShVector<1, Binding, T, true> operator()(int) const;
  ShVector<2, Binding, T, true> operator()(int, int) const;
  ShVector<3, Binding, T, true> operator()(int, int, int) const;
  ShVector<4, Binding, T, true> operator()(int, int, int, int) const;
  ShVector<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShVector<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShVector operator-() const;
  private:
    typedef ShAttrib<N, Binding, T, Swizzled> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShVector<1, Binding, T, Swizzled> : public ShAttrib<1, Binding, T, Swizzled> {
public:
  typedef T ValueType;
  typedef typename ShHostType<T>::type H; 
  typedef H HostType; 
  typedef typename ShMemoryType<T>::type MemoryType; 
  static const int typesize = 1;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_VECTOR;

  typedef ShVector<1, SH_INPUT, T> InputType;
  typedef ShVector<1, SH_OUTPUT, T> OutputType;
  typedef ShVector<1, SH_INOUT, T> InOutType;
  typedef ShVector<1, SH_TEMP, T> TempType;
  typedef ShVector<1, SH_CONST, T> ConstType;
  ShVector();
  
  template<typename T2>
  ShVector(const ShGeneric<1, T2>& other);
  ShVector(const ShVector<1, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShVector(const ShVector<1, Binding, T2, Swizzled>& other);
  ShVector(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShVector(H data[1]);
  
  ShVector(H);
  
  ~ShVector();

  
  template<typename T2>
  ShVector& operator=(const ShGeneric<1, T2>& other);
  
  template<typename T2>
  ShVector& operator=(const ShVector<1, Binding, T2, Swizzled>& other);
  ShVector& operator=(const ShVector<1, Binding, T, Swizzled>& other);

  ShVector& operator=(H other);

  ShVector& operator=(const ShProgram& prg);

  
  template<typename T2>
  ShVector& operator+=(const ShGeneric<1, T2>& right);
  
  template<typename T2>
  ShVector& operator-=(const ShGeneric<1, T2>& right);
  
  template<typename T2>
  ShVector& operator*=(const ShGeneric<1, T2>& right);
  
  template<typename T2>
  ShVector& operator/=(const ShGeneric<1, T2>& right);
  
  template<typename T2>
  ShVector& operator%=(const ShGeneric<1, T2>& right);
  ShVector& operator*=(H);
  ShVector& operator/=(H);
  ShVector& operator%=(H);
  ShVector& operator+=(H);
  ShVector& operator-=(H);
  ShVector<1, Binding, T, true> operator()(int) const;
  ShVector<2, Binding, T, true> operator()(int, int) const;
  ShVector<3, Binding, T, true> operator()(int, int, int) const;
  ShVector<4, Binding, T, true> operator()(int, int, int, int) const;
  ShVector<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShVector<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShVector operator-() const;
  private:
    typedef ShAttrib<1, Binding, T, Swizzled> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShVector<2, Binding, T, Swizzled> : public ShAttrib<2, Binding, T, Swizzled> {
public:
  typedef T ValueType;
  typedef typename ShHostType<T>::type H; 
  typedef H HostType; 
  typedef typename ShMemoryType<T>::type MemoryType; 
  static const int typesize = 2;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_VECTOR;

  typedef ShVector<2, SH_INPUT, T> InputType;
  typedef ShVector<2, SH_OUTPUT, T> OutputType;
  typedef ShVector<2, SH_INOUT, T> InOutType;
  typedef ShVector<2, SH_TEMP, T> TempType;
  typedef ShVector<2, SH_CONST, T> ConstType;
  ShVector();
  
  template<typename T2>
  ShVector(const ShGeneric<2, T2>& other);
  ShVector(const ShVector<2, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShVector(const ShVector<2, Binding, T2, Swizzled>& other);
  ShVector(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShVector(H data[2]);
  
  ShVector(H, H);
  template<typename T2, typename T3>
  ShVector(const ShGeneric<1, T2>&, const ShGeneric<1, T3>&);
  
  ~ShVector();

  
  template<typename T2>
  ShVector& operator=(const ShGeneric<2, T2>& other);
  
  template<typename T2>
  ShVector& operator=(const ShVector<2, Binding, T2, Swizzled>& other);
  ShVector& operator=(const ShVector<2, Binding, T, Swizzled>& other);

  ShVector& operator=(const ShProgram& prg);

  
  template<typename T2>
  ShVector& operator+=(const ShGeneric<2, T2>& right);
  
  template<typename T2>
  ShVector& operator-=(const ShGeneric<2, T2>& right);
  
  template<typename T2>
  ShVector& operator*=(const ShGeneric<2, T2>& right);
  
  template<typename T2>
  ShVector& operator/=(const ShGeneric<2, T2>& right);
  
  template<typename T2>
  ShVector& operator%=(const ShGeneric<2, T2>& right);
  ShVector& operator*=(H);
  ShVector& operator/=(H);
  ShVector& operator%=(H);
  ShVector& operator+=(H);
  ShVector& operator-=(H);
  
  template<typename T2>
  ShVector& operator+=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShVector& operator-=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShVector& operator*=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShVector& operator/=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShVector& operator%=(const ShGeneric<1, T2>&);
  ShVector<1, Binding, T, true> operator()(int) const;
  ShVector<2, Binding, T, true> operator()(int, int) const;
  ShVector<3, Binding, T, true> operator()(int, int, int) const;
  ShVector<4, Binding, T, true> operator()(int, int, int, int) const;
  ShVector<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShVector<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShVector operator-() const;
  private:
    typedef ShAttrib<2, Binding, T, Swizzled> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShVector<3, Binding, T, Swizzled> : public ShAttrib<3, Binding, T, Swizzled> {
public:
  typedef T ValueType;
  typedef typename ShHostType<T>::type H; 
  typedef H HostType; 
  typedef typename ShMemoryType<T>::type MemoryType; 
  static const int typesize = 3;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_VECTOR;

  typedef ShVector<3, SH_INPUT, T> InputType;
  typedef ShVector<3, SH_OUTPUT, T> OutputType;
  typedef ShVector<3, SH_INOUT, T> InOutType;
  typedef ShVector<3, SH_TEMP, T> TempType;
  typedef ShVector<3, SH_CONST, T> ConstType;
  ShVector();
  
  template<typename T2>
  ShVector(const ShGeneric<3, T2>& other);
  ShVector(const ShVector<3, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShVector(const ShVector<3, Binding, T2, Swizzled>& other);
  ShVector(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShVector(H data[3]);
  
  ShVector(H, H, H);
  template<typename T2, typename T3, typename T4>
  ShVector(const ShGeneric<1, T2>&, const ShGeneric<1, T3>&, const ShGeneric<1, T4>&);
  
  ~ShVector();

  
  template<typename T2>
  ShVector& operator=(const ShGeneric<3, T2>& other);
  
  template<typename T2>
  ShVector& operator=(const ShVector<3, Binding, T2, Swizzled>& other);
  ShVector& operator=(const ShVector<3, Binding, T, Swizzled>& other);

  ShVector& operator=(const ShProgram& prg);

  
  template<typename T2>
  ShVector& operator+=(const ShGeneric<3, T2>& right);
  
  template<typename T2>
  ShVector& operator-=(const ShGeneric<3, T2>& right);
  
  template<typename T2>
  ShVector& operator*=(const ShGeneric<3, T2>& right);
  
  template<typename T2>
  ShVector& operator/=(const ShGeneric<3, T2>& right);
  
  template<typename T2>
  ShVector& operator%=(const ShGeneric<3, T2>& right);
  ShVector& operator*=(H);
  ShVector& operator/=(H);
  ShVector& operator%=(H);
  ShVector& operator+=(H);
  ShVector& operator-=(H);
  
  template<typename T2>
  ShVector& operator+=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShVector& operator-=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShVector& operator*=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShVector& operator/=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShVector& operator%=(const ShGeneric<1, T2>&);
  ShVector<1, Binding, T, true> operator()(int) const;
  ShVector<2, Binding, T, true> operator()(int, int) const;
  ShVector<3, Binding, T, true> operator()(int, int, int) const;
  ShVector<4, Binding, T, true> operator()(int, int, int, int) const;
  ShVector<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShVector<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShVector operator-() const;
  private:
    typedef ShAttrib<3, Binding, T, Swizzled> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShVector<4, Binding, T, Swizzled> : public ShAttrib<4, Binding, T, Swizzled> {
public:
  typedef T ValueType;
  typedef typename ShHostType<T>::type H; 
  typedef H HostType; 
  typedef typename ShMemoryType<T>::type MemoryType; 
  static const int typesize = 4;
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_VECTOR;

  typedef ShVector<4, SH_INPUT, T> InputType;
  typedef ShVector<4, SH_OUTPUT, T> OutputType;
  typedef ShVector<4, SH_INOUT, T> InOutType;
  typedef ShVector<4, SH_TEMP, T> TempType;
  typedef ShVector<4, SH_CONST, T> ConstType;
  ShVector();
  
  template<typename T2>
  ShVector(const ShGeneric<4, T2>& other);
  ShVector(const ShVector<4, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShVector(const ShVector<4, Binding, T2, Swizzled>& other);
  ShVector(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShVector(H data[4]);
  
  ShVector(H, H, H, H);
  template<typename T2, typename T3, typename T4, typename T5>
  ShVector(const ShGeneric<1, T2>&, const ShGeneric<1, T3>&, const ShGeneric<1, T4>&, const ShGeneric<1, T5>&);
  
  ~ShVector();

  
  template<typename T2>
  ShVector& operator=(const ShGeneric<4, T2>& other);
  
  template<typename T2>
  ShVector& operator=(const ShVector<4, Binding, T2, Swizzled>& other);
  ShVector& operator=(const ShVector<4, Binding, T, Swizzled>& other);

  ShVector& operator=(const ShProgram& prg);

  
  template<typename T2>
  ShVector& operator+=(const ShGeneric<4, T2>& right);
  
  template<typename T2>
  ShVector& operator-=(const ShGeneric<4, T2>& right);
  
  template<typename T2>
  ShVector& operator*=(const ShGeneric<4, T2>& right);
  
  template<typename T2>
  ShVector& operator/=(const ShGeneric<4, T2>& right);
  
  template<typename T2>
  ShVector& operator%=(const ShGeneric<4, T2>& right);
  ShVector& operator*=(H);
  ShVector& operator/=(H);
  ShVector& operator%=(H);
  ShVector& operator+=(H);
  ShVector& operator-=(H);
  
  template<typename T2>
  ShVector& operator+=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShVector& operator-=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShVector& operator*=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShVector& operator/=(const ShGeneric<1, T2>&);
  
  template<typename T2>
  ShVector& operator%=(const ShGeneric<1, T2>&);
  ShVector<1, Binding, T, true> operator()(int) const;
  ShVector<2, Binding, T, true> operator()(int, int) const;
  ShVector<3, Binding, T, true> operator()(int, int, int) const;
  ShVector<4, Binding, T, true> operator()(int, int, int, int) const;
  ShVector<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShVector<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShVector operator-() const;
  private:
    typedef ShAttrib<4, Binding, T, Swizzled> ParentType;
};

typedef ShVector<1, SH_INPUT, ShInterval<double> > ShInputVector1i_d;
typedef ShVector<1, SH_OUTPUT, ShInterval<double> > ShOutputVector1i_d;
typedef ShVector<1, SH_INOUT, ShInterval<double> > ShInOutVector1i_d;
typedef ShVector<1, SH_TEMP, ShInterval<double> > ShVector1i_d;
typedef ShVector<1, SH_CONST, ShInterval<double> > ShConstVector1i_d;
typedef ShVector<2, SH_INPUT, ShInterval<double> > ShInputVector2i_d;
typedef ShVector<2, SH_OUTPUT, ShInterval<double> > ShOutputVector2i_d;
typedef ShVector<2, SH_INOUT, ShInterval<double> > ShInOutVector2i_d;
typedef ShVector<2, SH_TEMP, ShInterval<double> > ShVector2i_d;
typedef ShVector<2, SH_CONST, ShInterval<double> > ShConstVector2i_d;
typedef ShVector<3, SH_INPUT, ShInterval<double> > ShInputVector3i_d;
typedef ShVector<3, SH_OUTPUT, ShInterval<double> > ShOutputVector3i_d;
typedef ShVector<3, SH_INOUT, ShInterval<double> > ShInOutVector3i_d;
typedef ShVector<3, SH_TEMP, ShInterval<double> > ShVector3i_d;
typedef ShVector<3, SH_CONST, ShInterval<double> > ShConstVector3i_d;
typedef ShVector<4, SH_INPUT, ShInterval<double> > ShInputVector4i_d;
typedef ShVector<4, SH_OUTPUT, ShInterval<double> > ShOutputVector4i_d;
typedef ShVector<4, SH_INOUT, ShInterval<double> > ShInOutVector4i_d;
typedef ShVector<4, SH_TEMP, ShInterval<double> > ShVector4i_d;
typedef ShVector<4, SH_CONST, ShInterval<double> > ShConstVector4i_d;


typedef ShVector<1, SH_INPUT, short> ShInputVector1s;
typedef ShVector<1, SH_OUTPUT, short> ShOutputVector1s;
typedef ShVector<1, SH_INOUT, short> ShInOutVector1s;
typedef ShVector<1, SH_TEMP, short> ShVector1s;
typedef ShVector<1, SH_CONST, short> ShConstVector1s;
typedef ShVector<2, SH_INPUT, short> ShInputVector2s;
typedef ShVector<2, SH_OUTPUT, short> ShOutputVector2s;
typedef ShVector<2, SH_INOUT, short> ShInOutVector2s;
typedef ShVector<2, SH_TEMP, short> ShVector2s;
typedef ShVector<2, SH_CONST, short> ShConstVector2s;
typedef ShVector<3, SH_INPUT, short> ShInputVector3s;
typedef ShVector<3, SH_OUTPUT, short> ShOutputVector3s;
typedef ShVector<3, SH_INOUT, short> ShInOutVector3s;
typedef ShVector<3, SH_TEMP, short> ShVector3s;
typedef ShVector<3, SH_CONST, short> ShConstVector3s;
typedef ShVector<4, SH_INPUT, short> ShInputVector4s;
typedef ShVector<4, SH_OUTPUT, short> ShOutputVector4s;
typedef ShVector<4, SH_INOUT, short> ShInOutVector4s;
typedef ShVector<4, SH_TEMP, short> ShVector4s;
typedef ShVector<4, SH_CONST, short> ShConstVector4s;


typedef ShVector<1, SH_INPUT, int> ShInputVector1i;
typedef ShVector<1, SH_OUTPUT, int> ShOutputVector1i;
typedef ShVector<1, SH_INOUT, int> ShInOutVector1i;
typedef ShVector<1, SH_TEMP, int> ShVector1i;
typedef ShVector<1, SH_CONST, int> ShConstVector1i;
typedef ShVector<2, SH_INPUT, int> ShInputVector2i;
typedef ShVector<2, SH_OUTPUT, int> ShOutputVector2i;
typedef ShVector<2, SH_INOUT, int> ShInOutVector2i;
typedef ShVector<2, SH_TEMP, int> ShVector2i;
typedef ShVector<2, SH_CONST, int> ShConstVector2i;
typedef ShVector<3, SH_INPUT, int> ShInputVector3i;
typedef ShVector<3, SH_OUTPUT, int> ShOutputVector3i;
typedef ShVector<3, SH_INOUT, int> ShInOutVector3i;
typedef ShVector<3, SH_TEMP, int> ShVector3i;
typedef ShVector<3, SH_CONST, int> ShConstVector3i;
typedef ShVector<4, SH_INPUT, int> ShInputVector4i;
typedef ShVector<4, SH_OUTPUT, int> ShOutputVector4i;
typedef ShVector<4, SH_INOUT, int> ShInOutVector4i;
typedef ShVector<4, SH_TEMP, int> ShVector4i;
typedef ShVector<4, SH_CONST, int> ShConstVector4i;


typedef ShVector<1, SH_INPUT, double> ShInputVector1d;
typedef ShVector<1, SH_OUTPUT, double> ShOutputVector1d;
typedef ShVector<1, SH_INOUT, double> ShInOutVector1d;
typedef ShVector<1, SH_TEMP, double> ShVector1d;
typedef ShVector<1, SH_CONST, double> ShConstVector1d;
typedef ShVector<2, SH_INPUT, double> ShInputVector2d;
typedef ShVector<2, SH_OUTPUT, double> ShOutputVector2d;
typedef ShVector<2, SH_INOUT, double> ShInOutVector2d;
typedef ShVector<2, SH_TEMP, double> ShVector2d;
typedef ShVector<2, SH_CONST, double> ShConstVector2d;
typedef ShVector<3, SH_INPUT, double> ShInputVector3d;
typedef ShVector<3, SH_OUTPUT, double> ShOutputVector3d;
typedef ShVector<3, SH_INOUT, double> ShInOutVector3d;
typedef ShVector<3, SH_TEMP, double> ShVector3d;
typedef ShVector<3, SH_CONST, double> ShConstVector3d;
typedef ShVector<4, SH_INPUT, double> ShInputVector4d;
typedef ShVector<4, SH_OUTPUT, double> ShOutputVector4d;
typedef ShVector<4, SH_INOUT, double> ShInOutVector4d;
typedef ShVector<4, SH_TEMP, double> ShVector4d;
typedef ShVector<4, SH_CONST, double> ShConstVector4d;


typedef ShVector<1, SH_INPUT, float> ShInputVector1f;
typedef ShVector<1, SH_OUTPUT, float> ShOutputVector1f;
typedef ShVector<1, SH_INOUT, float> ShInOutVector1f;
typedef ShVector<1, SH_TEMP, float> ShVector1f;
typedef ShVector<1, SH_CONST, float> ShConstVector1f;
typedef ShVector<2, SH_INPUT, float> ShInputVector2f;
typedef ShVector<2, SH_OUTPUT, float> ShOutputVector2f;
typedef ShVector<2, SH_INOUT, float> ShInOutVector2f;
typedef ShVector<2, SH_TEMP, float> ShVector2f;
typedef ShVector<2, SH_CONST, float> ShConstVector2f;
typedef ShVector<3, SH_INPUT, float> ShInputVector3f;
typedef ShVector<3, SH_OUTPUT, float> ShOutputVector3f;
typedef ShVector<3, SH_INOUT, float> ShInOutVector3f;
typedef ShVector<3, SH_TEMP, float> ShVector3f;
typedef ShVector<3, SH_CONST, float> ShConstVector3f;
typedef ShVector<4, SH_INPUT, float> ShInputVector4f;
typedef ShVector<4, SH_OUTPUT, float> ShOutputVector4f;
typedef ShVector<4, SH_INOUT, float> ShInOutVector4f;
typedef ShVector<4, SH_TEMP, float> ShVector4f;
typedef ShVector<4, SH_CONST, float> ShConstVector4f;


typedef ShVector<1, SH_INPUT, char> ShInputVector1b;
typedef ShVector<1, SH_OUTPUT, char> ShOutputVector1b;
typedef ShVector<1, SH_INOUT, char> ShInOutVector1b;
typedef ShVector<1, SH_TEMP, char> ShVector1b;
typedef ShVector<1, SH_CONST, char> ShConstVector1b;
typedef ShVector<2, SH_INPUT, char> ShInputVector2b;
typedef ShVector<2, SH_OUTPUT, char> ShOutputVector2b;
typedef ShVector<2, SH_INOUT, char> ShInOutVector2b;
typedef ShVector<2, SH_TEMP, char> ShVector2b;
typedef ShVector<2, SH_CONST, char> ShConstVector2b;
typedef ShVector<3, SH_INPUT, char> ShInputVector3b;
typedef ShVector<3, SH_OUTPUT, char> ShOutputVector3b;
typedef ShVector<3, SH_INOUT, char> ShInOutVector3b;
typedef ShVector<3, SH_TEMP, char> ShVector3b;
typedef ShVector<3, SH_CONST, char> ShConstVector3b;
typedef ShVector<4, SH_INPUT, char> ShInputVector4b;
typedef ShVector<4, SH_OUTPUT, char> ShOutputVector4b;
typedef ShVector<4, SH_INOUT, char> ShInOutVector4b;
typedef ShVector<4, SH_TEMP, char> ShVector4b;
typedef ShVector<4, SH_CONST, char> ShConstVector4b;


typedef ShVector<1, SH_INPUT, ShInterval<float> > ShInputVector1i_f;
typedef ShVector<1, SH_OUTPUT, ShInterval<float> > ShOutputVector1i_f;
typedef ShVector<1, SH_INOUT, ShInterval<float> > ShInOutVector1i_f;
typedef ShVector<1, SH_TEMP, ShInterval<float> > ShVector1i_f;
typedef ShVector<1, SH_CONST, ShInterval<float> > ShConstVector1i_f;
typedef ShVector<2, SH_INPUT, ShInterval<float> > ShInputVector2i_f;
typedef ShVector<2, SH_OUTPUT, ShInterval<float> > ShOutputVector2i_f;
typedef ShVector<2, SH_INOUT, ShInterval<float> > ShInOutVector2i_f;
typedef ShVector<2, SH_TEMP, ShInterval<float> > ShVector2i_f;
typedef ShVector<2, SH_CONST, ShInterval<float> > ShConstVector2i_f;
typedef ShVector<3, SH_INPUT, ShInterval<float> > ShInputVector3i_f;
typedef ShVector<3, SH_OUTPUT, ShInterval<float> > ShOutputVector3i_f;
typedef ShVector<3, SH_INOUT, ShInterval<float> > ShInOutVector3i_f;
typedef ShVector<3, SH_TEMP, ShInterval<float> > ShVector3i_f;
typedef ShVector<3, SH_CONST, ShInterval<float> > ShConstVector3i_f;
typedef ShVector<4, SH_INPUT, ShInterval<float> > ShInputVector4i_f;
typedef ShVector<4, SH_OUTPUT, ShInterval<float> > ShOutputVector4i_f;
typedef ShVector<4, SH_INOUT, ShInterval<float> > ShInOutVector4i_f;
typedef ShVector<4, SH_TEMP, ShInterval<float> > ShVector4i_f;
typedef ShVector<4, SH_CONST, ShInterval<float> > ShConstVector4i_f;



} // namespace SH
#include "ShVectorImpl.hpp"

#endif // SH_SHVECTOR_HPP
