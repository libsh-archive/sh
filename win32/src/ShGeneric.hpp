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
#ifndef SHGENERIC_HPP
#define SHGENERIC_HPP

#include "ShVariable.hpp"

namespace SH {

/** A variable of length N.
 *
 * This class is provided to make definition of functions that work
 * over n-tuples of particular types easier.
 *
 * ShAttrib derives from ShGeneric. Unlike ShGeneric, which only has
 * two template parameters, ShAttrib has four template
 * parameters. This would make writing functions quite ugly. E.g.:
 *
 * Without Generic:
 *
 * template<int N, typename T, ShBindingType B1, ShBindingType B2,
 *          bool S1, bool S2>
 * ShAttrib<N, SH_TEMP, T> add(const ShAttrib<N, B1, T, S1>& a,
 *                             const ShAttrib<N, B2, T, S2>& b);
 *
 * With Generic:
 *
 * template<int N, typename T>
 * ShAttrib<N, SH_TEMP, T> add(const ShGeneric<N, T>& a,
 *                             const ShGeneric<N, T>& b);
 *
 * This class is explicitly instantiated for T = float with 1 <= N <= 4.
 */
template<int N, typename T>
class ShGeneric : public ShVariable 
{
public:
  ShGeneric(const ShVariableNodePtr& node, ShSwizzle swizzle, bool neg);
  ~ShGeneric();

  ShGeneric& operator=(const ShGeneric& other);
  
  ShGeneric& operator+=(const ShGeneric& right);
  ShGeneric& operator-=(const ShGeneric& right);
  ShGeneric& operator*=(const ShGeneric& right);
  ShGeneric& operator/=(const ShGeneric& right);
  ShGeneric& operator%=(const ShGeneric& right);
  ShGeneric& operator+=(const ShGeneric<1, T>& right);
  ShGeneric& operator-=(const ShGeneric<1, T>& right);
  ShGeneric& operator*=(const ShGeneric<1, T>& right);
  ShGeneric& operator/=(const ShGeneric<1, T>& right);
  ShGeneric& operator%=(const ShGeneric<1, T>& right);
  ShGeneric& operator+=(T);
  ShGeneric& operator-=(T);
  ShGeneric& operator*=(T);
  ShGeneric& operator/=(T);
  ShGeneric& operator%=(T);

  ShGeneric operator-() const;

  ShGeneric operator()() const; ///< Identity swizzle
  ShGeneric<1, T> operator()(int) const;
  ShGeneric<1, T> operator[](int) const;
  ShGeneric<2, T> operator()(int, int) const;
  ShGeneric<3, T> operator()(int, int, int) const;
  ShGeneric<4, T> operator()(int, int, int, int) const;
  
  // Arbitrary Swizzle
  template<int N2>
  ShGeneric<N2, T> swiz(int indices[]) const;

  typedef T ValueType;
  static const int typesize = N;

protected:
  ShGeneric(const ShVariableNodePtr& node);
};

template<typename T>
class ShGeneric<1, T> : public ShVariable 
{
public:
  ShGeneric(const ShVariableNodePtr& node, ShSwizzle swizzle, bool neg);
  ~ShGeneric();

  ShGeneric& operator=(const ShGeneric& other);
  ShGeneric& operator=(T);
  
  ShGeneric& operator+=(const ShGeneric& right);
  ShGeneric& operator-=(const ShGeneric& right);
  ShGeneric& operator*=(const ShGeneric& right);
  ShGeneric& operator/=(const ShGeneric& right);
  ShGeneric& operator%=(const ShGeneric& right);
  ShGeneric& operator+=(T);
  ShGeneric& operator-=(T);
  ShGeneric& operator*=(T);
  ShGeneric& operator/=(T);
  ShGeneric& operator%=(T);

  ShGeneric operator-() const;

  ShGeneric operator()() const; ///< Identity swizzle
  ShGeneric<1, T> operator()(int) const;
  ShGeneric<1, T> operator[](int) const;
  ShGeneric<2, T> operator()(int, int) const;
  ShGeneric<3, T> operator()(int, int, int) const;
  ShGeneric<4, T> operator()(int, int, int, int) const;
  
  // Arbitrary Swizzle
  template<int N2>
  ShGeneric<N2, T> swiz(int indices[]) const;

  typedef T ValueType;
  static const int typesize = 1;

protected:
  ShGeneric(const ShVariableNodePtr& node);
};

}

// This is a hack for ShAttrib.hpp in particular.
// A little dirty, but it works well.
#ifndef SH_DO_NOT_INCLUDE_GENERIC_IMPL
#include "ShGenericImpl.hpp"
#endif

#endif
