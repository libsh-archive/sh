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
#ifndef SHTYPEINFOIMPL_HPP
#define SHTYPEINFOIMPL_HPP

#include "ShTypeInfo.hpp"
#include "ShVariantFactory.hpp"
#include "ShInterval.hpp"
#include "ShEval.hpp"

namespace {

/// Used to implement the valuesEqual method.
template<typename T>
bool ShConcreteTypeEquals(const T &a, const T &b) 
{
  return a == b;
}

template<typename T>
bool ShConcreteTypeEquals(const SH::ShInterval<T> &a, const SH::ShInterval<T> &b) 
{
  return SH::boundsEqual(a, b); 
}

}

namespace SH {

// values that should usually work
template<typename T> const T ShConcreteTypeInfo<T>::TrueVal = (T) 1;
template<typename T> const T ShConcreteTypeInfo<T>::FalseVal = (T) 0;

template<typename T> const T ShConcreteTypeInfo<T>::ZERO = (T) 0;
template<typename T> const T ShConcreteTypeInfo<T>::ONE = (T) 1;

template<typename T>
ShPointer<ShVariantFactory> ShConcreteTypeInfo<T>::m_variantFactory;   

template<typename T>
ShConcreteTypeInfo<T>::ShConcreteTypeInfo()
{}

template<typename T>
T ShConcreteTypeInfo<T>::defaultLo(ShSemanticType type)
{
  switch(type) {
    case SH_POINT:
    case SH_VECTOR:
    case SH_NORMAL:
    case SH_POSITION:
      return -1;
    default:
      return 0;
  }
}

template<typename T>
T ShConcreteTypeInfo<T>::defaultHi(ShSemanticType type)
{
  return 1;
}

template<typename T>
const char* ShConcreteTypeInfo<T>::name() const 
{
  return m_name;
}

template<typename T>
bool ShConcreteTypeInfo<T>::valuesEqual(const T &a, const T &b)
{
  return ShConcreteTypeEquals(a, b);
}

template<typename T>
ShVariantFactoryCPtr ShConcreteTypeInfo<T>::variantFactory() 
{
  if(!m_variantFactory) {
    m_variantFactory = new ShDataVariantFactory<T>();
  }
  return m_variantFactory;
}

template<typename T>
T shTypeInfoCond(bool cond) 
{
  return cond ? ShConcreteTypeInfo<T>::TrueVal : ShConcreteTypeInfo<T>::FalseVal;
}

// Type precedence information 
// 
// currently available types:
// float, double, int, ShInterval<double>, ShInterval<float>
template<typename T> 
struct ShCommonType<T, T> { typedef T type; };

// @todo type this shouldn't really be just *any* T
template<typename T> 
struct ShCommonType<ShInterval<double>, T> { typedef ShInterval<double> type; };

template<typename T> 
struct ShCommonType<T, ShInterval<double> > { typedef ShInterval<double> type; };

template<> struct ShCommonType<ShInterval<float>, double> { typedef ShInterval<double> type; };
template<> struct ShCommonType<ShInterval<float>, float> { typedef ShInterval<float> type; };
template<> struct ShCommonType<ShInterval<float>, int> { typedef ShInterval<float> type; };
template<> struct ShCommonType<ShInterval<float>, short> { typedef ShInterval<float> type; };
template<> struct ShCommonType<ShInterval<float>, char> { typedef ShInterval<float> type; };
template<> struct ShCommonType<double, ShInterval<float> > { typedef ShInterval<double> type; };
template<> struct ShCommonType<float, ShInterval<float> > { typedef ShInterval<float> type; };
template<> struct ShCommonType<int, ShInterval<float> > { typedef ShInterval<float> type; };
template<> struct ShCommonType<short, ShInterval<float> > { typedef ShInterval<float> type; };
template<> struct ShCommonType<char, ShInterval<float> > { typedef ShInterval<float> type; };

template<> struct ShCommonType<double, float> { typedef double type; };
template<> struct ShCommonType<double, int> { typedef double type; };
template<> struct ShCommonType<double, short> { typedef double type; };
template<> struct ShCommonType<double, char> { typedef double type; };
template<> struct ShCommonType<float, double> { typedef double type; };
template<> struct ShCommonType<int, double> { typedef double type; };
template<> struct ShCommonType<short, double> { typedef double type; };
template<> struct ShCommonType<char, double> { typedef double type; };

template<> struct ShCommonType<float, int> { typedef float type; };
template<> struct ShCommonType<float, short> { typedef float type; };
template<> struct ShCommonType<float, char> { typedef float type; };
template<> struct ShCommonType<int, float> { typedef float type; };
template<> struct ShCommonType<short, float> { typedef float type; };
template<> struct ShCommonType<char, float> { typedef float type; };

template<> struct ShCommonType<int, short> { typedef int type; };
template<> struct ShCommonType<int, char> { typedef int type; };
template<> struct ShCommonType<short, int> { typedef int type; };
template<> struct ShCommonType<char, int> { typedef int type; };

template<> struct ShCommonType<short, char> { typedef short type; };
template<> struct ShCommonType<char, short> { typedef short type; };

}

#endif
