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
  typedef T storage_type;
  typedef typename ShHostType<T>::type host_type; 
  typedef typename ShMemType<T>::type mem_type; 
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_ATTRIB;

  typedef ShAttrib<N, SH_INPUT, T> InputType;
  typedef ShAttrib<N, SH_OUTPUT, T> OutputType;
  typedef ShAttrib<N, SH_INOUT, T> InOutType;
  typedef ShAttrib<N, SH_TEMP, T> TempType;
  typedef ShAttrib<N, SH_CONST, T> ConstType;
  ShAttrib();
  
  template<typename T2>
  ShAttrib(const ShGeneric<N, T2>& other);
  ShAttrib(const ShAttrib<N, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShAttrib(const ShAttrib<N, Binding, T2, Swizzled>& other);
  ShAttrib(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShAttrib(host_type data[N]);
  
  ~ShAttrib();

  
  template<typename T2>
  ShAttrib& operator=(const ShGeneric<N, T2>& other);
  
  template<typename T2>
  ShAttrib& operator=(const ShAttrib<N, Binding, T2, Swizzled>& other);
  ShAttrib& operator=(const ShAttrib<N, Binding, T, Swizzled>& other);

  ShAttrib& operator=(const ShProgram& prg);

  
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
  ShAttrib& operator*=(host_type);
  ShAttrib& operator/=(host_type);
  ShAttrib& operator%=(host_type);
  ShAttrib& operator+=(host_type);
  ShAttrib& operator-=(host_type);
  
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
  template<int N2>
  ShAttrib<N2, Binding, T, true> swiz(const ShSwizzle& s) const;
  
  ShAttrib operator-() const;
  private:
    typedef ShGeneric<N, T> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShAttrib<1, Binding, T, Swizzled> : public ShGeneric<1, T> {
public:
  typedef T storage_type;
  typedef typename ShHostType<T>::type host_type; 
  typedef typename ShMemType<T>::type mem_type; 
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_ATTRIB;

  typedef ShAttrib<1, SH_INPUT, T> InputType;
  typedef ShAttrib<1, SH_OUTPUT, T> OutputType;
  typedef ShAttrib<1, SH_INOUT, T> InOutType;
  typedef ShAttrib<1, SH_TEMP, T> TempType;
  typedef ShAttrib<1, SH_CONST, T> ConstType;
  ShAttrib();
  
  template<typename T2>
  ShAttrib(const ShGeneric<1, T2>& other);
  ShAttrib(const ShAttrib<1, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShAttrib(const ShAttrib<1, Binding, T2, Swizzled>& other);
  ShAttrib(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShAttrib(host_type data[1]);
  
  ShAttrib(host_type);
  
  ~ShAttrib();

  
  template<typename T2>
  ShAttrib& operator=(const ShGeneric<1, T2>& other);
  
  template<typename T2>
  ShAttrib& operator=(const ShAttrib<1, Binding, T2, Swizzled>& other);
  ShAttrib& operator=(const ShAttrib<1, Binding, T, Swizzled>& other);

  ShAttrib& operator=(host_type other);

  ShAttrib& operator=(const ShProgram& prg);

  
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
  ShAttrib& operator*=(host_type);
  ShAttrib& operator/=(host_type);
  ShAttrib& operator%=(host_type);
  ShAttrib& operator+=(host_type);
  ShAttrib& operator-=(host_type);
  ShAttrib<1, Binding, T, true> operator()(int) const;
  ShAttrib<2, Binding, T, true> operator()(int, int) const;
  ShAttrib<3, Binding, T, true> operator()(int, int, int) const;
  ShAttrib<4, Binding, T, true> operator()(int, int, int, int) const;
  ShAttrib<1, Binding, T, true> operator[](int) const;
  
  template<int N2>
  ShAttrib<N2, Binding, T, true> swiz(int indices[]) const;
  template<int N2>
  ShAttrib<N2, Binding, T, true> swiz(const ShSwizzle& s) const;
  
  ShAttrib operator-() const;
  private:
    typedef ShGeneric<1, T> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShAttrib<2, Binding, T, Swizzled> : public ShGeneric<2, T> {
public:
  typedef T storage_type;
  typedef typename ShHostType<T>::type host_type; 
  typedef typename ShMemType<T>::type mem_type; 
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_ATTRIB;

  typedef ShAttrib<2, SH_INPUT, T> InputType;
  typedef ShAttrib<2, SH_OUTPUT, T> OutputType;
  typedef ShAttrib<2, SH_INOUT, T> InOutType;
  typedef ShAttrib<2, SH_TEMP, T> TempType;
  typedef ShAttrib<2, SH_CONST, T> ConstType;
  ShAttrib();
  
  template<typename T2>
  ShAttrib(const ShGeneric<2, T2>& other);
  ShAttrib(const ShAttrib<2, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShAttrib(const ShAttrib<2, Binding, T2, Swizzled>& other);
  ShAttrib(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShAttrib(host_type data[2]);
  
  ShAttrib(host_type, host_type);
  template<typename T2, typename T3>
  ShAttrib(const ShGeneric<1, T2>&, const ShGeneric<1, T3>&);
  
  ~ShAttrib();

  
  template<typename T2>
  ShAttrib& operator=(const ShGeneric<2, T2>& other);
  
  template<typename T2>
  ShAttrib& operator=(const ShAttrib<2, Binding, T2, Swizzled>& other);
  ShAttrib& operator=(const ShAttrib<2, Binding, T, Swizzled>& other);

  ShAttrib& operator=(const ShProgram& prg);

  
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
  ShAttrib& operator*=(host_type);
  ShAttrib& operator/=(host_type);
  ShAttrib& operator%=(host_type);
  ShAttrib& operator+=(host_type);
  ShAttrib& operator-=(host_type);
  
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
  template<int N2>
  ShAttrib<N2, Binding, T, true> swiz(const ShSwizzle& s) const;
  
  ShAttrib operator-() const;
  private:
    typedef ShGeneric<2, T> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShAttrib<3, Binding, T, Swizzled> : public ShGeneric<3, T> {
public:
  typedef T storage_type;
  typedef typename ShHostType<T>::type host_type; 
  typedef typename ShMemType<T>::type mem_type; 
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_ATTRIB;

  typedef ShAttrib<3, SH_INPUT, T> InputType;
  typedef ShAttrib<3, SH_OUTPUT, T> OutputType;
  typedef ShAttrib<3, SH_INOUT, T> InOutType;
  typedef ShAttrib<3, SH_TEMP, T> TempType;
  typedef ShAttrib<3, SH_CONST, T> ConstType;
  ShAttrib();
  
  template<typename T2>
  ShAttrib(const ShGeneric<3, T2>& other);
  ShAttrib(const ShAttrib<3, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShAttrib(const ShAttrib<3, Binding, T2, Swizzled>& other);
  ShAttrib(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShAttrib(host_type data[3]);
  
  ShAttrib(host_type, host_type, host_type);
  template<typename T2, typename T3, typename T4>
  ShAttrib(const ShGeneric<1, T2>&, const ShGeneric<1, T3>&, const ShGeneric<1, T4>&);
  
  ~ShAttrib();

  
  template<typename T2>
  ShAttrib& operator=(const ShGeneric<3, T2>& other);
  
  template<typename T2>
  ShAttrib& operator=(const ShAttrib<3, Binding, T2, Swizzled>& other);
  ShAttrib& operator=(const ShAttrib<3, Binding, T, Swizzled>& other);

  ShAttrib& operator=(const ShProgram& prg);

  
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
  ShAttrib& operator*=(host_type);
  ShAttrib& operator/=(host_type);
  ShAttrib& operator%=(host_type);
  ShAttrib& operator+=(host_type);
  ShAttrib& operator-=(host_type);
  
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
  template<int N2>
  ShAttrib<N2, Binding, T, true> swiz(const ShSwizzle& s) const;
  
  ShAttrib operator-() const;
  private:
    typedef ShGeneric<3, T> ParentType;
};

template<ShBindingType Binding, typename T, bool Swizzled>
class ShAttrib<4, Binding, T, Swizzled> : public ShGeneric<4, T> {
public:
  typedef T storage_type;
  typedef typename ShHostType<T>::type host_type; 
  typedef typename ShMemType<T>::type mem_type; 
  static const ShBindingType binding_type = Binding;
  static const ShSemanticType semantic_type = SH_ATTRIB;

  typedef ShAttrib<4, SH_INPUT, T> InputType;
  typedef ShAttrib<4, SH_OUTPUT, T> OutputType;
  typedef ShAttrib<4, SH_INOUT, T> InOutType;
  typedef ShAttrib<4, SH_TEMP, T> TempType;
  typedef ShAttrib<4, SH_CONST, T> ConstType;
  ShAttrib();
  
  template<typename T2>
  ShAttrib(const ShGeneric<4, T2>& other);
  ShAttrib(const ShAttrib<4, Binding, T, Swizzled>& other);
  
  template<typename T2>
  ShAttrib(const ShAttrib<4, Binding, T2, Swizzled>& other);
  ShAttrib(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  explicit ShAttrib(host_type data[4]);
  
  ShAttrib(host_type, host_type, host_type, host_type);
  template<typename T2, typename T3, typename T4, typename T5>
  ShAttrib(const ShGeneric<1, T2>&, const ShGeneric<1, T3>&, const ShGeneric<1, T4>&, const ShGeneric<1, T5>&);
  
  ~ShAttrib();

  
  template<typename T2>
  ShAttrib& operator=(const ShGeneric<4, T2>& other);
  
  template<typename T2>
  ShAttrib& operator=(const ShAttrib<4, Binding, T2, Swizzled>& other);
  ShAttrib& operator=(const ShAttrib<4, Binding, T, Swizzled>& other);

  ShAttrib& operator=(const ShProgram& prg);

  
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
  ShAttrib& operator*=(host_type);
  ShAttrib& operator/=(host_type);
  ShAttrib& operator%=(host_type);
  ShAttrib& operator+=(host_type);
  ShAttrib& operator-=(host_type);
  
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
  template<int N2>
  ShAttrib<N2, Binding, T, true> swiz(const ShSwizzle& s) const;
  
  ShAttrib operator-() const;
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


typedef ShAttrib<1, SH_INPUT, ShFracUShort> ShInputAttrib1fus;
typedef ShAttrib<1, SH_OUTPUT, ShFracUShort> ShOutputAttrib1fus;
typedef ShAttrib<1, SH_INOUT, ShFracUShort> ShInOutAttrib1fus;
typedef ShAttrib<1, SH_TEMP, ShFracUShort> ShAttrib1fus;
typedef ShAttrib<1, SH_CONST, ShFracUShort> ShConstAttrib1fus;
typedef ShAttrib<2, SH_INPUT, ShFracUShort> ShInputAttrib2fus;
typedef ShAttrib<2, SH_OUTPUT, ShFracUShort> ShOutputAttrib2fus;
typedef ShAttrib<2, SH_INOUT, ShFracUShort> ShInOutAttrib2fus;
typedef ShAttrib<2, SH_TEMP, ShFracUShort> ShAttrib2fus;
typedef ShAttrib<2, SH_CONST, ShFracUShort> ShConstAttrib2fus;
typedef ShAttrib<3, SH_INPUT, ShFracUShort> ShInputAttrib3fus;
typedef ShAttrib<3, SH_OUTPUT, ShFracUShort> ShOutputAttrib3fus;
typedef ShAttrib<3, SH_INOUT, ShFracUShort> ShInOutAttrib3fus;
typedef ShAttrib<3, SH_TEMP, ShFracUShort> ShAttrib3fus;
typedef ShAttrib<3, SH_CONST, ShFracUShort> ShConstAttrib3fus;
typedef ShAttrib<4, SH_INPUT, ShFracUShort> ShInputAttrib4fus;
typedef ShAttrib<4, SH_OUTPUT, ShFracUShort> ShOutputAttrib4fus;
typedef ShAttrib<4, SH_INOUT, ShFracUShort> ShInOutAttrib4fus;
typedef ShAttrib<4, SH_TEMP, ShFracUShort> ShAttrib4fus;
typedef ShAttrib<4, SH_CONST, ShFracUShort> ShConstAttrib4fus;


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


typedef ShAttrib<1, SH_INPUT, ShFracUInt> ShInputAttrib1fui;
typedef ShAttrib<1, SH_OUTPUT, ShFracUInt> ShOutputAttrib1fui;
typedef ShAttrib<1, SH_INOUT, ShFracUInt> ShInOutAttrib1fui;
typedef ShAttrib<1, SH_TEMP, ShFracUInt> ShAttrib1fui;
typedef ShAttrib<1, SH_CONST, ShFracUInt> ShConstAttrib1fui;
typedef ShAttrib<2, SH_INPUT, ShFracUInt> ShInputAttrib2fui;
typedef ShAttrib<2, SH_OUTPUT, ShFracUInt> ShOutputAttrib2fui;
typedef ShAttrib<2, SH_INOUT, ShFracUInt> ShInOutAttrib2fui;
typedef ShAttrib<2, SH_TEMP, ShFracUInt> ShAttrib2fui;
typedef ShAttrib<2, SH_CONST, ShFracUInt> ShConstAttrib2fui;
typedef ShAttrib<3, SH_INPUT, ShFracUInt> ShInputAttrib3fui;
typedef ShAttrib<3, SH_OUTPUT, ShFracUInt> ShOutputAttrib3fui;
typedef ShAttrib<3, SH_INOUT, ShFracUInt> ShInOutAttrib3fui;
typedef ShAttrib<3, SH_TEMP, ShFracUInt> ShAttrib3fui;
typedef ShAttrib<3, SH_CONST, ShFracUInt> ShConstAttrib3fui;
typedef ShAttrib<4, SH_INPUT, ShFracUInt> ShInputAttrib4fui;
typedef ShAttrib<4, SH_OUTPUT, ShFracUInt> ShOutputAttrib4fui;
typedef ShAttrib<4, SH_INOUT, ShFracUInt> ShInOutAttrib4fui;
typedef ShAttrib<4, SH_TEMP, ShFracUInt> ShAttrib4fui;
typedef ShAttrib<4, SH_CONST, ShFracUInt> ShConstAttrib4fui;


typedef ShAttrib<1, SH_INPUT, ShFracByte> ShInputAttrib1fb;
typedef ShAttrib<1, SH_OUTPUT, ShFracByte> ShOutputAttrib1fb;
typedef ShAttrib<1, SH_INOUT, ShFracByte> ShInOutAttrib1fb;
typedef ShAttrib<1, SH_TEMP, ShFracByte> ShAttrib1fb;
typedef ShAttrib<1, SH_CONST, ShFracByte> ShConstAttrib1fb;
typedef ShAttrib<2, SH_INPUT, ShFracByte> ShInputAttrib2fb;
typedef ShAttrib<2, SH_OUTPUT, ShFracByte> ShOutputAttrib2fb;
typedef ShAttrib<2, SH_INOUT, ShFracByte> ShInOutAttrib2fb;
typedef ShAttrib<2, SH_TEMP, ShFracByte> ShAttrib2fb;
typedef ShAttrib<2, SH_CONST, ShFracByte> ShConstAttrib2fb;
typedef ShAttrib<3, SH_INPUT, ShFracByte> ShInputAttrib3fb;
typedef ShAttrib<3, SH_OUTPUT, ShFracByte> ShOutputAttrib3fb;
typedef ShAttrib<3, SH_INOUT, ShFracByte> ShInOutAttrib3fb;
typedef ShAttrib<3, SH_TEMP, ShFracByte> ShAttrib3fb;
typedef ShAttrib<3, SH_CONST, ShFracByte> ShConstAttrib3fb;
typedef ShAttrib<4, SH_INPUT, ShFracByte> ShInputAttrib4fb;
typedef ShAttrib<4, SH_OUTPUT, ShFracByte> ShOutputAttrib4fb;
typedef ShAttrib<4, SH_INOUT, ShFracByte> ShInOutAttrib4fb;
typedef ShAttrib<4, SH_TEMP, ShFracByte> ShAttrib4fb;
typedef ShAttrib<4, SH_CONST, ShFracByte> ShConstAttrib4fb;


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


typedef ShAttrib<1, SH_INPUT, unsigned char> ShInputAttrib1ub;
typedef ShAttrib<1, SH_OUTPUT, unsigned char> ShOutputAttrib1ub;
typedef ShAttrib<1, SH_INOUT, unsigned char> ShInOutAttrib1ub;
typedef ShAttrib<1, SH_TEMP, unsigned char> ShAttrib1ub;
typedef ShAttrib<1, SH_CONST, unsigned char> ShConstAttrib1ub;
typedef ShAttrib<2, SH_INPUT, unsigned char> ShInputAttrib2ub;
typedef ShAttrib<2, SH_OUTPUT, unsigned char> ShOutputAttrib2ub;
typedef ShAttrib<2, SH_INOUT, unsigned char> ShInOutAttrib2ub;
typedef ShAttrib<2, SH_TEMP, unsigned char> ShAttrib2ub;
typedef ShAttrib<2, SH_CONST, unsigned char> ShConstAttrib2ub;
typedef ShAttrib<3, SH_INPUT, unsigned char> ShInputAttrib3ub;
typedef ShAttrib<3, SH_OUTPUT, unsigned char> ShOutputAttrib3ub;
typedef ShAttrib<3, SH_INOUT, unsigned char> ShInOutAttrib3ub;
typedef ShAttrib<3, SH_TEMP, unsigned char> ShAttrib3ub;
typedef ShAttrib<3, SH_CONST, unsigned char> ShConstAttrib3ub;
typedef ShAttrib<4, SH_INPUT, unsigned char> ShInputAttrib4ub;
typedef ShAttrib<4, SH_OUTPUT, unsigned char> ShOutputAttrib4ub;
typedef ShAttrib<4, SH_INOUT, unsigned char> ShInOutAttrib4ub;
typedef ShAttrib<4, SH_TEMP, unsigned char> ShAttrib4ub;
typedef ShAttrib<4, SH_CONST, unsigned char> ShConstAttrib4ub;


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


typedef ShAttrib<1, SH_INPUT, ShAffine<double> > ShInputAttrib1a_d;
typedef ShAttrib<1, SH_OUTPUT, ShAffine<double> > ShOutputAttrib1a_d;
typedef ShAttrib<1, SH_INOUT, ShAffine<double> > ShInOutAttrib1a_d;
typedef ShAttrib<1, SH_TEMP, ShAffine<double> > ShAttrib1a_d;
typedef ShAttrib<1, SH_CONST, ShAffine<double> > ShConstAttrib1a_d;
typedef ShAttrib<2, SH_INPUT, ShAffine<double> > ShInputAttrib2a_d;
typedef ShAttrib<2, SH_OUTPUT, ShAffine<double> > ShOutputAttrib2a_d;
typedef ShAttrib<2, SH_INOUT, ShAffine<double> > ShInOutAttrib2a_d;
typedef ShAttrib<2, SH_TEMP, ShAffine<double> > ShAttrib2a_d;
typedef ShAttrib<2, SH_CONST, ShAffine<double> > ShConstAttrib2a_d;
typedef ShAttrib<3, SH_INPUT, ShAffine<double> > ShInputAttrib3a_d;
typedef ShAttrib<3, SH_OUTPUT, ShAffine<double> > ShOutputAttrib3a_d;
typedef ShAttrib<3, SH_INOUT, ShAffine<double> > ShInOutAttrib3a_d;
typedef ShAttrib<3, SH_TEMP, ShAffine<double> > ShAttrib3a_d;
typedef ShAttrib<3, SH_CONST, ShAffine<double> > ShConstAttrib3a_d;
typedef ShAttrib<4, SH_INPUT, ShAffine<double> > ShInputAttrib4a_d;
typedef ShAttrib<4, SH_OUTPUT, ShAffine<double> > ShOutputAttrib4a_d;
typedef ShAttrib<4, SH_INOUT, ShAffine<double> > ShInOutAttrib4a_d;
typedef ShAttrib<4, SH_TEMP, ShAffine<double> > ShAttrib4a_d;
typedef ShAttrib<4, SH_CONST, ShAffine<double> > ShConstAttrib4a_d;


typedef ShAttrib<1, SH_INPUT, ShAffine<float> > ShInputAttrib1a_f;
typedef ShAttrib<1, SH_OUTPUT, ShAffine<float> > ShOutputAttrib1a_f;
typedef ShAttrib<1, SH_INOUT, ShAffine<float> > ShInOutAttrib1a_f;
typedef ShAttrib<1, SH_TEMP, ShAffine<float> > ShAttrib1a_f;
typedef ShAttrib<1, SH_CONST, ShAffine<float> > ShConstAttrib1a_f;
typedef ShAttrib<2, SH_INPUT, ShAffine<float> > ShInputAttrib2a_f;
typedef ShAttrib<2, SH_OUTPUT, ShAffine<float> > ShOutputAttrib2a_f;
typedef ShAttrib<2, SH_INOUT, ShAffine<float> > ShInOutAttrib2a_f;
typedef ShAttrib<2, SH_TEMP, ShAffine<float> > ShAttrib2a_f;
typedef ShAttrib<2, SH_CONST, ShAffine<float> > ShConstAttrib2a_f;
typedef ShAttrib<3, SH_INPUT, ShAffine<float> > ShInputAttrib3a_f;
typedef ShAttrib<3, SH_OUTPUT, ShAffine<float> > ShOutputAttrib3a_f;
typedef ShAttrib<3, SH_INOUT, ShAffine<float> > ShInOutAttrib3a_f;
typedef ShAttrib<3, SH_TEMP, ShAffine<float> > ShAttrib3a_f;
typedef ShAttrib<3, SH_CONST, ShAffine<float> > ShConstAttrib3a_f;
typedef ShAttrib<4, SH_INPUT, ShAffine<float> > ShInputAttrib4a_f;
typedef ShAttrib<4, SH_OUTPUT, ShAffine<float> > ShOutputAttrib4a_f;
typedef ShAttrib<4, SH_INOUT, ShAffine<float> > ShInOutAttrib4a_f;
typedef ShAttrib<4, SH_TEMP, ShAffine<float> > ShAttrib4a_f;
typedef ShAttrib<4, SH_CONST, ShAffine<float> > ShConstAttrib4a_f;


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


typedef ShAttrib<1, SH_INPUT, unsigned short> ShInputAttrib1us;
typedef ShAttrib<1, SH_OUTPUT, unsigned short> ShOutputAttrib1us;
typedef ShAttrib<1, SH_INOUT, unsigned short> ShInOutAttrib1us;
typedef ShAttrib<1, SH_TEMP, unsigned short> ShAttrib1us;
typedef ShAttrib<1, SH_CONST, unsigned short> ShConstAttrib1us;
typedef ShAttrib<2, SH_INPUT, unsigned short> ShInputAttrib2us;
typedef ShAttrib<2, SH_OUTPUT, unsigned short> ShOutputAttrib2us;
typedef ShAttrib<2, SH_INOUT, unsigned short> ShInOutAttrib2us;
typedef ShAttrib<2, SH_TEMP, unsigned short> ShAttrib2us;
typedef ShAttrib<2, SH_CONST, unsigned short> ShConstAttrib2us;
typedef ShAttrib<3, SH_INPUT, unsigned short> ShInputAttrib3us;
typedef ShAttrib<3, SH_OUTPUT, unsigned short> ShOutputAttrib3us;
typedef ShAttrib<3, SH_INOUT, unsigned short> ShInOutAttrib3us;
typedef ShAttrib<3, SH_TEMP, unsigned short> ShAttrib3us;
typedef ShAttrib<3, SH_CONST, unsigned short> ShConstAttrib3us;
typedef ShAttrib<4, SH_INPUT, unsigned short> ShInputAttrib4us;
typedef ShAttrib<4, SH_OUTPUT, unsigned short> ShOutputAttrib4us;
typedef ShAttrib<4, SH_INOUT, unsigned short> ShInOutAttrib4us;
typedef ShAttrib<4, SH_TEMP, unsigned short> ShAttrib4us;
typedef ShAttrib<4, SH_CONST, unsigned short> ShConstAttrib4us;


typedef ShAttrib<1, SH_INPUT, ShFracUByte> ShInputAttrib1fub;
typedef ShAttrib<1, SH_OUTPUT, ShFracUByte> ShOutputAttrib1fub;
typedef ShAttrib<1, SH_INOUT, ShFracUByte> ShInOutAttrib1fub;
typedef ShAttrib<1, SH_TEMP, ShFracUByte> ShAttrib1fub;
typedef ShAttrib<1, SH_CONST, ShFracUByte> ShConstAttrib1fub;
typedef ShAttrib<2, SH_INPUT, ShFracUByte> ShInputAttrib2fub;
typedef ShAttrib<2, SH_OUTPUT, ShFracUByte> ShOutputAttrib2fub;
typedef ShAttrib<2, SH_INOUT, ShFracUByte> ShInOutAttrib2fub;
typedef ShAttrib<2, SH_TEMP, ShFracUByte> ShAttrib2fub;
typedef ShAttrib<2, SH_CONST, ShFracUByte> ShConstAttrib2fub;
typedef ShAttrib<3, SH_INPUT, ShFracUByte> ShInputAttrib3fub;
typedef ShAttrib<3, SH_OUTPUT, ShFracUByte> ShOutputAttrib3fub;
typedef ShAttrib<3, SH_INOUT, ShFracUByte> ShInOutAttrib3fub;
typedef ShAttrib<3, SH_TEMP, ShFracUByte> ShAttrib3fub;
typedef ShAttrib<3, SH_CONST, ShFracUByte> ShConstAttrib3fub;
typedef ShAttrib<4, SH_INPUT, ShFracUByte> ShInputAttrib4fub;
typedef ShAttrib<4, SH_OUTPUT, ShFracUByte> ShOutputAttrib4fub;
typedef ShAttrib<4, SH_INOUT, ShFracUByte> ShInOutAttrib4fub;
typedef ShAttrib<4, SH_TEMP, ShFracUByte> ShAttrib4fub;
typedef ShAttrib<4, SH_CONST, ShFracUByte> ShConstAttrib4fub;


typedef ShAttrib<1, SH_INPUT, ShHalf> ShInputAttrib1h;
typedef ShAttrib<1, SH_OUTPUT, ShHalf> ShOutputAttrib1h;
typedef ShAttrib<1, SH_INOUT, ShHalf> ShInOutAttrib1h;
typedef ShAttrib<1, SH_TEMP, ShHalf> ShAttrib1h;
typedef ShAttrib<1, SH_CONST, ShHalf> ShConstAttrib1h;
typedef ShAttrib<2, SH_INPUT, ShHalf> ShInputAttrib2h;
typedef ShAttrib<2, SH_OUTPUT, ShHalf> ShOutputAttrib2h;
typedef ShAttrib<2, SH_INOUT, ShHalf> ShInOutAttrib2h;
typedef ShAttrib<2, SH_TEMP, ShHalf> ShAttrib2h;
typedef ShAttrib<2, SH_CONST, ShHalf> ShConstAttrib2h;
typedef ShAttrib<3, SH_INPUT, ShHalf> ShInputAttrib3h;
typedef ShAttrib<3, SH_OUTPUT, ShHalf> ShOutputAttrib3h;
typedef ShAttrib<3, SH_INOUT, ShHalf> ShInOutAttrib3h;
typedef ShAttrib<3, SH_TEMP, ShHalf> ShAttrib3h;
typedef ShAttrib<3, SH_CONST, ShHalf> ShConstAttrib3h;
typedef ShAttrib<4, SH_INPUT, ShHalf> ShInputAttrib4h;
typedef ShAttrib<4, SH_OUTPUT, ShHalf> ShOutputAttrib4h;
typedef ShAttrib<4, SH_INOUT, ShHalf> ShInOutAttrib4h;
typedef ShAttrib<4, SH_TEMP, ShHalf> ShAttrib4h;
typedef ShAttrib<4, SH_CONST, ShHalf> ShConstAttrib4h;


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


typedef ShAttrib<1, SH_INPUT, ShFracShort> ShInputAttrib1fs;
typedef ShAttrib<1, SH_OUTPUT, ShFracShort> ShOutputAttrib1fs;
typedef ShAttrib<1, SH_INOUT, ShFracShort> ShInOutAttrib1fs;
typedef ShAttrib<1, SH_TEMP, ShFracShort> ShAttrib1fs;
typedef ShAttrib<1, SH_CONST, ShFracShort> ShConstAttrib1fs;
typedef ShAttrib<2, SH_INPUT, ShFracShort> ShInputAttrib2fs;
typedef ShAttrib<2, SH_OUTPUT, ShFracShort> ShOutputAttrib2fs;
typedef ShAttrib<2, SH_INOUT, ShFracShort> ShInOutAttrib2fs;
typedef ShAttrib<2, SH_TEMP, ShFracShort> ShAttrib2fs;
typedef ShAttrib<2, SH_CONST, ShFracShort> ShConstAttrib2fs;
typedef ShAttrib<3, SH_INPUT, ShFracShort> ShInputAttrib3fs;
typedef ShAttrib<3, SH_OUTPUT, ShFracShort> ShOutputAttrib3fs;
typedef ShAttrib<3, SH_INOUT, ShFracShort> ShInOutAttrib3fs;
typedef ShAttrib<3, SH_TEMP, ShFracShort> ShAttrib3fs;
typedef ShAttrib<3, SH_CONST, ShFracShort> ShConstAttrib3fs;
typedef ShAttrib<4, SH_INPUT, ShFracShort> ShInputAttrib4fs;
typedef ShAttrib<4, SH_OUTPUT, ShFracShort> ShOutputAttrib4fs;
typedef ShAttrib<4, SH_INOUT, ShFracShort> ShInOutAttrib4fs;
typedef ShAttrib<4, SH_TEMP, ShFracShort> ShAttrib4fs;
typedef ShAttrib<4, SH_CONST, ShFracShort> ShConstAttrib4fs;


typedef ShAttrib<1, SH_INPUT, ShFracInt> ShInputAttrib1fi;
typedef ShAttrib<1, SH_OUTPUT, ShFracInt> ShOutputAttrib1fi;
typedef ShAttrib<1, SH_INOUT, ShFracInt> ShInOutAttrib1fi;
typedef ShAttrib<1, SH_TEMP, ShFracInt> ShAttrib1fi;
typedef ShAttrib<1, SH_CONST, ShFracInt> ShConstAttrib1fi;
typedef ShAttrib<2, SH_INPUT, ShFracInt> ShInputAttrib2fi;
typedef ShAttrib<2, SH_OUTPUT, ShFracInt> ShOutputAttrib2fi;
typedef ShAttrib<2, SH_INOUT, ShFracInt> ShInOutAttrib2fi;
typedef ShAttrib<2, SH_TEMP, ShFracInt> ShAttrib2fi;
typedef ShAttrib<2, SH_CONST, ShFracInt> ShConstAttrib2fi;
typedef ShAttrib<3, SH_INPUT, ShFracInt> ShInputAttrib3fi;
typedef ShAttrib<3, SH_OUTPUT, ShFracInt> ShOutputAttrib3fi;
typedef ShAttrib<3, SH_INOUT, ShFracInt> ShInOutAttrib3fi;
typedef ShAttrib<3, SH_TEMP, ShFracInt> ShAttrib3fi;
typedef ShAttrib<3, SH_CONST, ShFracInt> ShConstAttrib3fi;
typedef ShAttrib<4, SH_INPUT, ShFracInt> ShInputAttrib4fi;
typedef ShAttrib<4, SH_OUTPUT, ShFracInt> ShOutputAttrib4fi;
typedef ShAttrib<4, SH_INOUT, ShFracInt> ShInOutAttrib4fi;
typedef ShAttrib<4, SH_TEMP, ShFracInt> ShAttrib4fi;
typedef ShAttrib<4, SH_CONST, ShFracInt> ShConstAttrib4fi;


typedef ShAttrib<1, SH_INPUT, unsigned int> ShInputAttrib1ui;
typedef ShAttrib<1, SH_OUTPUT, unsigned int> ShOutputAttrib1ui;
typedef ShAttrib<1, SH_INOUT, unsigned int> ShInOutAttrib1ui;
typedef ShAttrib<1, SH_TEMP, unsigned int> ShAttrib1ui;
typedef ShAttrib<1, SH_CONST, unsigned int> ShConstAttrib1ui;
typedef ShAttrib<2, SH_INPUT, unsigned int> ShInputAttrib2ui;
typedef ShAttrib<2, SH_OUTPUT, unsigned int> ShOutputAttrib2ui;
typedef ShAttrib<2, SH_INOUT, unsigned int> ShInOutAttrib2ui;
typedef ShAttrib<2, SH_TEMP, unsigned int> ShAttrib2ui;
typedef ShAttrib<2, SH_CONST, unsigned int> ShConstAttrib2ui;
typedef ShAttrib<3, SH_INPUT, unsigned int> ShInputAttrib3ui;
typedef ShAttrib<3, SH_OUTPUT, unsigned int> ShOutputAttrib3ui;
typedef ShAttrib<3, SH_INOUT, unsigned int> ShInOutAttrib3ui;
typedef ShAttrib<3, SH_TEMP, unsigned int> ShAttrib3ui;
typedef ShAttrib<3, SH_CONST, unsigned int> ShConstAttrib3ui;
typedef ShAttrib<4, SH_INPUT, unsigned int> ShInputAttrib4ui;
typedef ShAttrib<4, SH_OUTPUT, unsigned int> ShOutputAttrib4ui;
typedef ShAttrib<4, SH_INOUT, unsigned int> ShInOutAttrib4ui;
typedef ShAttrib<4, SH_TEMP, unsigned int> ShAttrib4ui;
typedef ShAttrib<4, SH_CONST, unsigned int> ShConstAttrib4ui;



} // namespace SH
#include "ShGenericImpl.hpp"
#include "ShAttribImpl.hpp"

#endif // SH_SHATTRIB_HPP
