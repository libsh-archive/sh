// WARNING - DO NOT EDIT THIS FILE:
// This file was automatically generated from scripts/ShNormal.hpp.py.
// If you wish to change it, edit that file instead.
//
// ---
//
// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
  typedef T storage_type;
  typedef typename ShHostType<T>::type host_type; 
  typedef typename ShMemType<T>::type mem_type; 
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_NORMAL;

  typedef ShNormal<N, SH_INPUT, T> InputType;
  typedef ShNormal<N, SH_OUTPUT, T> OutputType;
  typedef ShNormal<N, SH_INOUT, T> InOutType;
  typedef ShNormal<N, SH_TEMP, T> TempType;
  typedef ShNormal<N, SH_CONST, T> ConstType;
  ShNormal();
  
  template<typename T2>
  ShNormal(const ShGeneric<N, T2>& other);
  ShNormal(const ShNormal<N, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShNormal(const ShNormal<N, Binding, T2, Swizzled>& other);
  ShNormal(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShNormal(host_type data[N]);
  
  ~ShNormal();

  
  template<typename T2>
  ShNormal& operator=(const ShGeneric<N, T2>& other);
  
  template<typename T2>
  ShNormal& operator=(const ShNormal<N, Binding, T2, Swizzled>& other);
  ShNormal& operator=(const ShNormal<N, Binding, T, Swizzled>& other);

  ShNormal& operator=(const ShProgram& prg);

  
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
  ShNormal& operator*=(host_type);
  ShNormal& operator/=(host_type);
  ShNormal& operator%=(host_type);
  ShNormal& operator+=(host_type);
  ShNormal& operator-=(host_type);
  
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
  private:
    typedef ShVector<N, Binding, T, Swizzled> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShNormal<1, Binding, T, Swizzled> : public ShVector<1, Binding, T, Swizzled> {
public:
  typedef T storage_type;
  typedef typename ShHostType<T>::type host_type; 
  typedef typename ShMemType<T>::type mem_type; 
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_NORMAL;

  typedef ShNormal<1, SH_INPUT, T> InputType;
  typedef ShNormal<1, SH_OUTPUT, T> OutputType;
  typedef ShNormal<1, SH_INOUT, T> InOutType;
  typedef ShNormal<1, SH_TEMP, T> TempType;
  typedef ShNormal<1, SH_CONST, T> ConstType;
  ShNormal();
  
  template<typename T2>
  ShNormal(const ShGeneric<1, T2>& other);
  ShNormal(const ShNormal<1, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShNormal(const ShNormal<1, Binding, T2, Swizzled>& other);
  ShNormal(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShNormal(host_type data[1]);
  
  ShNormal(host_type);
  
  ~ShNormal();

  
  template<typename T2>
  ShNormal& operator=(const ShGeneric<1, T2>& other);
  
  template<typename T2>
  ShNormal& operator=(const ShNormal<1, Binding, T2, Swizzled>& other);
  ShNormal& operator=(const ShNormal<1, Binding, T, Swizzled>& other);

  ShNormal& operator=(host_type other);

  ShNormal& operator=(const ShProgram& prg);

  
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
  ShNormal& operator*=(host_type);
  ShNormal& operator/=(host_type);
  ShNormal& operator%=(host_type);
  ShNormal& operator+=(host_type);
  ShNormal& operator-=(host_type);
  ShNormal<1, Binding, T, true> operator()(int) const;
  ShNormal<2, Binding, T, true> operator()(int, int) const;
  ShNormal<3, Binding, T, true> operator()(int, int, int) const;
  ShNormal<4, Binding, T, true> operator()(int, int, int, int) const;
  ShNormal<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShNormal<N2, Binding, T, true> swiz(int indices[]) const;
  
  ShNormal operator-() const;
  private:
    typedef ShVector<1, Binding, T, Swizzled> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShNormal<2, Binding, T, Swizzled> : public ShVector<2, Binding, T, Swizzled> {
public:
  typedef T storage_type;
  typedef typename ShHostType<T>::type host_type; 
  typedef typename ShMemType<T>::type mem_type; 
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_NORMAL;

  typedef ShNormal<2, SH_INPUT, T> InputType;
  typedef ShNormal<2, SH_OUTPUT, T> OutputType;
  typedef ShNormal<2, SH_INOUT, T> InOutType;
  typedef ShNormal<2, SH_TEMP, T> TempType;
  typedef ShNormal<2, SH_CONST, T> ConstType;
  ShNormal();
  
  template<typename T2>
  ShNormal(const ShGeneric<2, T2>& other);
  ShNormal(const ShNormal<2, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShNormal(const ShNormal<2, Binding, T2, Swizzled>& other);
  ShNormal(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShNormal(host_type data[2]);
  
  ShNormal(host_type, host_type);
  template<typename T2, typename T3>
  ShNormal(const ShGeneric<1, T2>&, const ShGeneric<1, T3>&);
  
  ~ShNormal();

  
  template<typename T2>
  ShNormal& operator=(const ShGeneric<2, T2>& other);
  
  template<typename T2>
  ShNormal& operator=(const ShNormal<2, Binding, T2, Swizzled>& other);
  ShNormal& operator=(const ShNormal<2, Binding, T, Swizzled>& other);

  ShNormal& operator=(const ShProgram& prg);

  
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
  ShNormal& operator*=(host_type);
  ShNormal& operator/=(host_type);
  ShNormal& operator%=(host_type);
  ShNormal& operator+=(host_type);
  ShNormal& operator-=(host_type);
  
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
  private:
    typedef ShVector<2, Binding, T, Swizzled> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShNormal<3, Binding, T, Swizzled> : public ShVector<3, Binding, T, Swizzled> {
public:
  typedef T storage_type;
  typedef typename ShHostType<T>::type host_type; 
  typedef typename ShMemType<T>::type mem_type; 
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_NORMAL;

  typedef ShNormal<3, SH_INPUT, T> InputType;
  typedef ShNormal<3, SH_OUTPUT, T> OutputType;
  typedef ShNormal<3, SH_INOUT, T> InOutType;
  typedef ShNormal<3, SH_TEMP, T> TempType;
  typedef ShNormal<3, SH_CONST, T> ConstType;
  ShNormal();
  
  template<typename T2>
  ShNormal(const ShGeneric<3, T2>& other);
  ShNormal(const ShNormal<3, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShNormal(const ShNormal<3, Binding, T2, Swizzled>& other);
  ShNormal(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShNormal(host_type data[3]);
  
  ShNormal(host_type, host_type, host_type);
  template<typename T2, typename T3, typename T4>
  ShNormal(const ShGeneric<1, T2>&, const ShGeneric<1, T3>&, const ShGeneric<1, T4>&);
  
  ~ShNormal();

  
  template<typename T2>
  ShNormal& operator=(const ShGeneric<3, T2>& other);
  
  template<typename T2>
  ShNormal& operator=(const ShNormal<3, Binding, T2, Swizzled>& other);
  ShNormal& operator=(const ShNormal<3, Binding, T, Swizzled>& other);

  ShNormal& operator=(const ShProgram& prg);

  
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
  ShNormal& operator*=(host_type);
  ShNormal& operator/=(host_type);
  ShNormal& operator%=(host_type);
  ShNormal& operator+=(host_type);
  ShNormal& operator-=(host_type);
  
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
  private:
    typedef ShVector<3, Binding, T, Swizzled> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShNormal<4, Binding, T, Swizzled> : public ShVector<4, Binding, T, Swizzled> {
public:
  typedef T storage_type;
  typedef typename ShHostType<T>::type host_type; 
  typedef typename ShMemType<T>::type mem_type; 
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_NORMAL;

  typedef ShNormal<4, SH_INPUT, T> InputType;
  typedef ShNormal<4, SH_OUTPUT, T> OutputType;
  typedef ShNormal<4, SH_INOUT, T> InOutType;
  typedef ShNormal<4, SH_TEMP, T> TempType;
  typedef ShNormal<4, SH_CONST, T> ConstType;
  ShNormal();
  
  template<typename T2>
  ShNormal(const ShGeneric<4, T2>& other);
  ShNormal(const ShNormal<4, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShNormal(const ShNormal<4, Binding, T2, Swizzled>& other);
  ShNormal(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShNormal(host_type data[4]);
  
  ShNormal(host_type, host_type, host_type, host_type);
  template<typename T2, typename T3, typename T4, typename T5>
  ShNormal(const ShGeneric<1, T2>&, const ShGeneric<1, T3>&, const ShGeneric<1, T4>&, const ShGeneric<1, T5>&);
  
  ~ShNormal();

  
  template<typename T2>
  ShNormal& operator=(const ShGeneric<4, T2>& other);
  
  template<typename T2>
  ShNormal& operator=(const ShNormal<4, Binding, T2, Swizzled>& other);
  ShNormal& operator=(const ShNormal<4, Binding, T, Swizzled>& other);

  ShNormal& operator=(const ShProgram& prg);

  
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
  ShNormal& operator*=(host_type);
  ShNormal& operator/=(host_type);
  ShNormal& operator%=(host_type);
  ShNormal& operator+=(host_type);
  ShNormal& operator-=(host_type);
  
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
  private:
    typedef ShVector<4, Binding, T, Swizzled> ParentType;
};

typedef ShNormal<1, SH_INPUT, ShFracUShort> ShInputNormal1fus;
typedef ShNormal<1, SH_OUTPUT, ShFracUShort> ShOutputNormal1fus;
typedef ShNormal<1, SH_INOUT, ShFracUShort> ShInOutNormal1fus;
typedef ShNormal<1, SH_TEMP, ShFracUShort> ShNormal1fus;
typedef ShNormal<1, SH_CONST, ShFracUShort> ShConstNormal1fus;
typedef ShNormal<2, SH_INPUT, ShFracUShort> ShInputNormal2fus;
typedef ShNormal<2, SH_OUTPUT, ShFracUShort> ShOutputNormal2fus;
typedef ShNormal<2, SH_INOUT, ShFracUShort> ShInOutNormal2fus;
typedef ShNormal<2, SH_TEMP, ShFracUShort> ShNormal2fus;
typedef ShNormal<2, SH_CONST, ShFracUShort> ShConstNormal2fus;
typedef ShNormal<3, SH_INPUT, ShFracUShort> ShInputNormal3fus;
typedef ShNormal<3, SH_OUTPUT, ShFracUShort> ShOutputNormal3fus;
typedef ShNormal<3, SH_INOUT, ShFracUShort> ShInOutNormal3fus;
typedef ShNormal<3, SH_TEMP, ShFracUShort> ShNormal3fus;
typedef ShNormal<3, SH_CONST, ShFracUShort> ShConstNormal3fus;
typedef ShNormal<4, SH_INPUT, ShFracUShort> ShInputNormal4fus;
typedef ShNormal<4, SH_OUTPUT, ShFracUShort> ShOutputNormal4fus;
typedef ShNormal<4, SH_INOUT, ShFracUShort> ShInOutNormal4fus;
typedef ShNormal<4, SH_TEMP, ShFracUShort> ShNormal4fus;
typedef ShNormal<4, SH_CONST, ShFracUShort> ShConstNormal4fus;


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


typedef ShNormal<1, SH_INPUT, ShFracUInt> ShInputNormal1fui;
typedef ShNormal<1, SH_OUTPUT, ShFracUInt> ShOutputNormal1fui;
typedef ShNormal<1, SH_INOUT, ShFracUInt> ShInOutNormal1fui;
typedef ShNormal<1, SH_TEMP, ShFracUInt> ShNormal1fui;
typedef ShNormal<1, SH_CONST, ShFracUInt> ShConstNormal1fui;
typedef ShNormal<2, SH_INPUT, ShFracUInt> ShInputNormal2fui;
typedef ShNormal<2, SH_OUTPUT, ShFracUInt> ShOutputNormal2fui;
typedef ShNormal<2, SH_INOUT, ShFracUInt> ShInOutNormal2fui;
typedef ShNormal<2, SH_TEMP, ShFracUInt> ShNormal2fui;
typedef ShNormal<2, SH_CONST, ShFracUInt> ShConstNormal2fui;
typedef ShNormal<3, SH_INPUT, ShFracUInt> ShInputNormal3fui;
typedef ShNormal<3, SH_OUTPUT, ShFracUInt> ShOutputNormal3fui;
typedef ShNormal<3, SH_INOUT, ShFracUInt> ShInOutNormal3fui;
typedef ShNormal<3, SH_TEMP, ShFracUInt> ShNormal3fui;
typedef ShNormal<3, SH_CONST, ShFracUInt> ShConstNormal3fui;
typedef ShNormal<4, SH_INPUT, ShFracUInt> ShInputNormal4fui;
typedef ShNormal<4, SH_OUTPUT, ShFracUInt> ShOutputNormal4fui;
typedef ShNormal<4, SH_INOUT, ShFracUInt> ShInOutNormal4fui;
typedef ShNormal<4, SH_TEMP, ShFracUInt> ShNormal4fui;
typedef ShNormal<4, SH_CONST, ShFracUInt> ShConstNormal4fui;


typedef ShNormal<1, SH_INPUT, ShFracByte> ShInputNormal1fb;
typedef ShNormal<1, SH_OUTPUT, ShFracByte> ShOutputNormal1fb;
typedef ShNormal<1, SH_INOUT, ShFracByte> ShInOutNormal1fb;
typedef ShNormal<1, SH_TEMP, ShFracByte> ShNormal1fb;
typedef ShNormal<1, SH_CONST, ShFracByte> ShConstNormal1fb;
typedef ShNormal<2, SH_INPUT, ShFracByte> ShInputNormal2fb;
typedef ShNormal<2, SH_OUTPUT, ShFracByte> ShOutputNormal2fb;
typedef ShNormal<2, SH_INOUT, ShFracByte> ShInOutNormal2fb;
typedef ShNormal<2, SH_TEMP, ShFracByte> ShNormal2fb;
typedef ShNormal<2, SH_CONST, ShFracByte> ShConstNormal2fb;
typedef ShNormal<3, SH_INPUT, ShFracByte> ShInputNormal3fb;
typedef ShNormal<3, SH_OUTPUT, ShFracByte> ShOutputNormal3fb;
typedef ShNormal<3, SH_INOUT, ShFracByte> ShInOutNormal3fb;
typedef ShNormal<3, SH_TEMP, ShFracByte> ShNormal3fb;
typedef ShNormal<3, SH_CONST, ShFracByte> ShConstNormal3fb;
typedef ShNormal<4, SH_INPUT, ShFracByte> ShInputNormal4fb;
typedef ShNormal<4, SH_OUTPUT, ShFracByte> ShOutputNormal4fb;
typedef ShNormal<4, SH_INOUT, ShFracByte> ShInOutNormal4fb;
typedef ShNormal<4, SH_TEMP, ShFracByte> ShNormal4fb;
typedef ShNormal<4, SH_CONST, ShFracByte> ShConstNormal4fb;


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


typedef ShNormal<1, SH_INPUT, unsigned char> ShInputNormal1ub;
typedef ShNormal<1, SH_OUTPUT, unsigned char> ShOutputNormal1ub;
typedef ShNormal<1, SH_INOUT, unsigned char> ShInOutNormal1ub;
typedef ShNormal<1, SH_TEMP, unsigned char> ShNormal1ub;
typedef ShNormal<1, SH_CONST, unsigned char> ShConstNormal1ub;
typedef ShNormal<2, SH_INPUT, unsigned char> ShInputNormal2ub;
typedef ShNormal<2, SH_OUTPUT, unsigned char> ShOutputNormal2ub;
typedef ShNormal<2, SH_INOUT, unsigned char> ShInOutNormal2ub;
typedef ShNormal<2, SH_TEMP, unsigned char> ShNormal2ub;
typedef ShNormal<2, SH_CONST, unsigned char> ShConstNormal2ub;
typedef ShNormal<3, SH_INPUT, unsigned char> ShInputNormal3ub;
typedef ShNormal<3, SH_OUTPUT, unsigned char> ShOutputNormal3ub;
typedef ShNormal<3, SH_INOUT, unsigned char> ShInOutNormal3ub;
typedef ShNormal<3, SH_TEMP, unsigned char> ShNormal3ub;
typedef ShNormal<3, SH_CONST, unsigned char> ShConstNormal3ub;
typedef ShNormal<4, SH_INPUT, unsigned char> ShInputNormal4ub;
typedef ShNormal<4, SH_OUTPUT, unsigned char> ShOutputNormal4ub;
typedef ShNormal<4, SH_INOUT, unsigned char> ShInOutNormal4ub;
typedef ShNormal<4, SH_TEMP, unsigned char> ShNormal4ub;
typedef ShNormal<4, SH_CONST, unsigned char> ShConstNormal4ub;


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


typedef ShNormal<1, SH_INPUT, unsigned short> ShInputNormal1us;
typedef ShNormal<1, SH_OUTPUT, unsigned short> ShOutputNormal1us;
typedef ShNormal<1, SH_INOUT, unsigned short> ShInOutNormal1us;
typedef ShNormal<1, SH_TEMP, unsigned short> ShNormal1us;
typedef ShNormal<1, SH_CONST, unsigned short> ShConstNormal1us;
typedef ShNormal<2, SH_INPUT, unsigned short> ShInputNormal2us;
typedef ShNormal<2, SH_OUTPUT, unsigned short> ShOutputNormal2us;
typedef ShNormal<2, SH_INOUT, unsigned short> ShInOutNormal2us;
typedef ShNormal<2, SH_TEMP, unsigned short> ShNormal2us;
typedef ShNormal<2, SH_CONST, unsigned short> ShConstNormal2us;
typedef ShNormal<3, SH_INPUT, unsigned short> ShInputNormal3us;
typedef ShNormal<3, SH_OUTPUT, unsigned short> ShOutputNormal3us;
typedef ShNormal<3, SH_INOUT, unsigned short> ShInOutNormal3us;
typedef ShNormal<3, SH_TEMP, unsigned short> ShNormal3us;
typedef ShNormal<3, SH_CONST, unsigned short> ShConstNormal3us;
typedef ShNormal<4, SH_INPUT, unsigned short> ShInputNormal4us;
typedef ShNormal<4, SH_OUTPUT, unsigned short> ShOutputNormal4us;
typedef ShNormal<4, SH_INOUT, unsigned short> ShInOutNormal4us;
typedef ShNormal<4, SH_TEMP, unsigned short> ShNormal4us;
typedef ShNormal<4, SH_CONST, unsigned short> ShConstNormal4us;


typedef ShNormal<1, SH_INPUT, ShFracUByte> ShInputNormal1fub;
typedef ShNormal<1, SH_OUTPUT, ShFracUByte> ShOutputNormal1fub;
typedef ShNormal<1, SH_INOUT, ShFracUByte> ShInOutNormal1fub;
typedef ShNormal<1, SH_TEMP, ShFracUByte> ShNormal1fub;
typedef ShNormal<1, SH_CONST, ShFracUByte> ShConstNormal1fub;
typedef ShNormal<2, SH_INPUT, ShFracUByte> ShInputNormal2fub;
typedef ShNormal<2, SH_OUTPUT, ShFracUByte> ShOutputNormal2fub;
typedef ShNormal<2, SH_INOUT, ShFracUByte> ShInOutNormal2fub;
typedef ShNormal<2, SH_TEMP, ShFracUByte> ShNormal2fub;
typedef ShNormal<2, SH_CONST, ShFracUByte> ShConstNormal2fub;
typedef ShNormal<3, SH_INPUT, ShFracUByte> ShInputNormal3fub;
typedef ShNormal<3, SH_OUTPUT, ShFracUByte> ShOutputNormal3fub;
typedef ShNormal<3, SH_INOUT, ShFracUByte> ShInOutNormal3fub;
typedef ShNormal<3, SH_TEMP, ShFracUByte> ShNormal3fub;
typedef ShNormal<3, SH_CONST, ShFracUByte> ShConstNormal3fub;
typedef ShNormal<4, SH_INPUT, ShFracUByte> ShInputNormal4fub;
typedef ShNormal<4, SH_OUTPUT, ShFracUByte> ShOutputNormal4fub;
typedef ShNormal<4, SH_INOUT, ShFracUByte> ShInOutNormal4fub;
typedef ShNormal<4, SH_TEMP, ShFracUByte> ShNormal4fub;
typedef ShNormal<4, SH_CONST, ShFracUByte> ShConstNormal4fub;


typedef ShNormal<1, SH_INPUT, ShHalf> ShInputNormal1h;
typedef ShNormal<1, SH_OUTPUT, ShHalf> ShOutputNormal1h;
typedef ShNormal<1, SH_INOUT, ShHalf> ShInOutNormal1h;
typedef ShNormal<1, SH_TEMP, ShHalf> ShNormal1h;
typedef ShNormal<1, SH_CONST, ShHalf> ShConstNormal1h;
typedef ShNormal<2, SH_INPUT, ShHalf> ShInputNormal2h;
typedef ShNormal<2, SH_OUTPUT, ShHalf> ShOutputNormal2h;
typedef ShNormal<2, SH_INOUT, ShHalf> ShInOutNormal2h;
typedef ShNormal<2, SH_TEMP, ShHalf> ShNormal2h;
typedef ShNormal<2, SH_CONST, ShHalf> ShConstNormal2h;
typedef ShNormal<3, SH_INPUT, ShHalf> ShInputNormal3h;
typedef ShNormal<3, SH_OUTPUT, ShHalf> ShOutputNormal3h;
typedef ShNormal<3, SH_INOUT, ShHalf> ShInOutNormal3h;
typedef ShNormal<3, SH_TEMP, ShHalf> ShNormal3h;
typedef ShNormal<3, SH_CONST, ShHalf> ShConstNormal3h;
typedef ShNormal<4, SH_INPUT, ShHalf> ShInputNormal4h;
typedef ShNormal<4, SH_OUTPUT, ShHalf> ShOutputNormal4h;
typedef ShNormal<4, SH_INOUT, ShHalf> ShInOutNormal4h;
typedef ShNormal<4, SH_TEMP, ShHalf> ShNormal4h;
typedef ShNormal<4, SH_CONST, ShHalf> ShConstNormal4h;


typedef ShNormal<1, SH_INPUT, ShFracShort> ShInputNormal1fs;
typedef ShNormal<1, SH_OUTPUT, ShFracShort> ShOutputNormal1fs;
typedef ShNormal<1, SH_INOUT, ShFracShort> ShInOutNormal1fs;
typedef ShNormal<1, SH_TEMP, ShFracShort> ShNormal1fs;
typedef ShNormal<1, SH_CONST, ShFracShort> ShConstNormal1fs;
typedef ShNormal<2, SH_INPUT, ShFracShort> ShInputNormal2fs;
typedef ShNormal<2, SH_OUTPUT, ShFracShort> ShOutputNormal2fs;
typedef ShNormal<2, SH_INOUT, ShFracShort> ShInOutNormal2fs;
typedef ShNormal<2, SH_TEMP, ShFracShort> ShNormal2fs;
typedef ShNormal<2, SH_CONST, ShFracShort> ShConstNormal2fs;
typedef ShNormal<3, SH_INPUT, ShFracShort> ShInputNormal3fs;
typedef ShNormal<3, SH_OUTPUT, ShFracShort> ShOutputNormal3fs;
typedef ShNormal<3, SH_INOUT, ShFracShort> ShInOutNormal3fs;
typedef ShNormal<3, SH_TEMP, ShFracShort> ShNormal3fs;
typedef ShNormal<3, SH_CONST, ShFracShort> ShConstNormal3fs;
typedef ShNormal<4, SH_INPUT, ShFracShort> ShInputNormal4fs;
typedef ShNormal<4, SH_OUTPUT, ShFracShort> ShOutputNormal4fs;
typedef ShNormal<4, SH_INOUT, ShFracShort> ShInOutNormal4fs;
typedef ShNormal<4, SH_TEMP, ShFracShort> ShNormal4fs;
typedef ShNormal<4, SH_CONST, ShFracShort> ShConstNormal4fs;


typedef ShNormal<1, SH_INPUT, ShFracInt> ShInputNormal1fi;
typedef ShNormal<1, SH_OUTPUT, ShFracInt> ShOutputNormal1fi;
typedef ShNormal<1, SH_INOUT, ShFracInt> ShInOutNormal1fi;
typedef ShNormal<1, SH_TEMP, ShFracInt> ShNormal1fi;
typedef ShNormal<1, SH_CONST, ShFracInt> ShConstNormal1fi;
typedef ShNormal<2, SH_INPUT, ShFracInt> ShInputNormal2fi;
typedef ShNormal<2, SH_OUTPUT, ShFracInt> ShOutputNormal2fi;
typedef ShNormal<2, SH_INOUT, ShFracInt> ShInOutNormal2fi;
typedef ShNormal<2, SH_TEMP, ShFracInt> ShNormal2fi;
typedef ShNormal<2, SH_CONST, ShFracInt> ShConstNormal2fi;
typedef ShNormal<3, SH_INPUT, ShFracInt> ShInputNormal3fi;
typedef ShNormal<3, SH_OUTPUT, ShFracInt> ShOutputNormal3fi;
typedef ShNormal<3, SH_INOUT, ShFracInt> ShInOutNormal3fi;
typedef ShNormal<3, SH_TEMP, ShFracInt> ShNormal3fi;
typedef ShNormal<3, SH_CONST, ShFracInt> ShConstNormal3fi;
typedef ShNormal<4, SH_INPUT, ShFracInt> ShInputNormal4fi;
typedef ShNormal<4, SH_OUTPUT, ShFracInt> ShOutputNormal4fi;
typedef ShNormal<4, SH_INOUT, ShFracInt> ShInOutNormal4fi;
typedef ShNormal<4, SH_TEMP, ShFracInt> ShNormal4fi;
typedef ShNormal<4, SH_CONST, ShFracInt> ShConstNormal4fi;


typedef ShNormal<1, SH_INPUT, unsigned int> ShInputNormal1ui;
typedef ShNormal<1, SH_OUTPUT, unsigned int> ShOutputNormal1ui;
typedef ShNormal<1, SH_INOUT, unsigned int> ShInOutNormal1ui;
typedef ShNormal<1, SH_TEMP, unsigned int> ShNormal1ui;
typedef ShNormal<1, SH_CONST, unsigned int> ShConstNormal1ui;
typedef ShNormal<2, SH_INPUT, unsigned int> ShInputNormal2ui;
typedef ShNormal<2, SH_OUTPUT, unsigned int> ShOutputNormal2ui;
typedef ShNormal<2, SH_INOUT, unsigned int> ShInOutNormal2ui;
typedef ShNormal<2, SH_TEMP, unsigned int> ShNormal2ui;
typedef ShNormal<2, SH_CONST, unsigned int> ShConstNormal2ui;
typedef ShNormal<3, SH_INPUT, unsigned int> ShInputNormal3ui;
typedef ShNormal<3, SH_OUTPUT, unsigned int> ShOutputNormal3ui;
typedef ShNormal<3, SH_INOUT, unsigned int> ShInOutNormal3ui;
typedef ShNormal<3, SH_TEMP, unsigned int> ShNormal3ui;
typedef ShNormal<3, SH_CONST, unsigned int> ShConstNormal3ui;
typedef ShNormal<4, SH_INPUT, unsigned int> ShInputNormal4ui;
typedef ShNormal<4, SH_OUTPUT, unsigned int> ShOutputNormal4ui;
typedef ShNormal<4, SH_INOUT, unsigned int> ShInOutNormal4ui;
typedef ShNormal<4, SH_TEMP, unsigned int> ShNormal4ui;
typedef ShNormal<4, SH_CONST, unsigned int> ShConstNormal4ui;



} // namespace SH
#include "ShNormalImpl.hpp"

#endif // SH_SHNORMAL_HPP
