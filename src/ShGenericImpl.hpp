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
#ifndef SHGENERICIMPL_HPP
#define SHGENERICIMPL_HPP

#include "ShGeneric.hpp"
#include "ShAttrib.hpp"
#include "ShLib.hpp"
#include "ShInstructions.hpp"

namespace SH {

template<int N, typename T>
ShGeneric<N, T>::ShGeneric(const ShVariableNodePtr& node)
  : ShVariable(node)
{
}

template<int N, typename T>
ShGeneric<N, T>::ShGeneric(const ShVariableNodePtr& node, ShSwizzle swizzle, bool neg)
  : ShVariable(node)
{
  m_swizzle = swizzle;
  m_neg = neg;
}

template<int N, typename T>
ShGeneric<N, T>::~ShGeneric()
{
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator=(const ShGeneric<N, T>& other)
{
  shASN(*this, other);
  return *this;
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator+=(const ShGeneric<N, T>& right)
{
  *this = *this + right;
  return *this;
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator-=(const ShGeneric<N, T>& right)
{
  *this = *this - right;
  return *this;
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator*=(const ShGeneric<N, T>& right)
{
  *this = *this * right;
  return *this;
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator/=(const ShGeneric<N, T>& right)
{
  *this = *this / right;
  return *this;
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator%=(const ShGeneric<N, T>& right)
{
  *this = *this % right;
  return *this;
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator+=(const ShGeneric<1, T>& right)
{
  *this = *this + right;
  return *this;
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator-=(const ShGeneric<1, T>& right)
{
  *this = *this - right;
  return *this;
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator*=(const ShGeneric<1, T>& right)
{
  *this = *this * right;
  return *this;
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator/=(const ShGeneric<1, T>& right)
{
  *this = *this / right;
  return *this;
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator%=(const ShGeneric<1, T>& right)
{
  *this = *this / right;
  return *this;
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator+=(T right)
{
  *this = *this + right;
  return *this;
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator-=(T right)
{
  *this = *this - right;
  return *this;
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator*=(T right)
{
  *this = *this * right;
  return *this;
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator/=(T right)
{
  *this = *this / right;
  return *this;
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator%=(T right)
{
  *this = *this % right;
  return *this;
}

template<int N, typename T>
ShGeneric<N, T> ShGeneric<N, T>::operator-() const
{
  return ShGeneric<N, T>(m_node, m_swizzle, !m_neg);
}


template<int N, typename T>
ShGeneric<N, T> ShGeneric<N, T>::operator()() const
{
  return ShGeneric<N, T>(m_node, m_swizzle, m_neg);
}

template<int N, typename T>
ShGeneric<1, T> ShGeneric<N, T>::operator()(int i1) const
{
  return ShGeneric<1, T>(m_node, m_swizzle * ShSwizzle(size(), i1), m_neg);
}

template<int N, typename T>
ShGeneric<1, T> ShGeneric<N, T>::operator[](int i1) const
{
  return ShGeneric<1, T>(m_node, m_swizzle * ShSwizzle(size(), i1), m_neg);
}

template<int N, typename T>
ShGeneric<2, T> ShGeneric<N, T>::operator()(int i1, int i2) const
{
  return ShGeneric<2, T>(m_node, m_swizzle * ShSwizzle(size(), i1, i2), m_neg);
}

template<int N, typename T>
ShGeneric<3, T> ShGeneric<N, T>::operator()(int i1, int i2, int i3) const
{
  return ShGeneric<3, T>(m_node, m_swizzle * ShSwizzle(size(), i1, i2, i3), m_neg);
}

template<int N, typename T>
ShGeneric<4, T> ShGeneric<N, T>::operator()(int i1, int i2, int i3, int i4) const
{
  return ShGeneric<4, T>(m_node, m_swizzle * ShSwizzle(size(), i1, i2, i3, i4), m_neg);
}
  
template<int N, typename T> 
template<int N2>
ShGeneric<N2, T> ShGeneric<N, T>::swiz(int indices[]) const
{
  return ShGeneric<N2, T>(m_node, m_swizzle * ShSwizzle(N, N2, indices), m_neg);
}

template<typename T>
ShGeneric<1, T>::ShGeneric(const ShVariableNodePtr& node)
  : ShVariable(node)
{
}

template<typename T>
ShGeneric<1, T>::ShGeneric(const ShVariableNodePtr& node, ShSwizzle swizzle, bool neg)
  : ShVariable(node)
{
  m_swizzle = swizzle;
  m_neg = neg;
}

template<typename T>
ShGeneric<1, T>::~ShGeneric()
{
}

template<typename T>
ShGeneric<1, T>& ShGeneric<1, T>::operator=(const ShGeneric<1, T>& other)
{
  shASN(*this, other);
  return *this;
}

template<typename T>
ShGeneric<1, T>& ShGeneric<1, T>::operator=(T other)
{
  shASN(*this, ShAttrib<1, SH_TEMP, T>(other));
  return *this;
}

template<typename T>
ShGeneric<1, T>& ShGeneric<1, T>::operator+=(const ShGeneric<1, T>& right)
{
  *this = *this + right;
  return *this;
}

template<typename T>
ShGeneric<1, T>& ShGeneric<1, T>::operator-=(const ShGeneric<1, T>& right)
{
  *this = *this - right;
  return *this;
}

template<typename T>
ShGeneric<1, T>& ShGeneric<1, T>::operator*=(const ShGeneric<1, T>& right)
{
  *this = *this * right;
  return *this;
}

template<typename T>
ShGeneric<1, T>& ShGeneric<1, T>::operator/=(const ShGeneric<1, T>& right)
{
  *this = *this / right;
  return *this;
}

template<typename T>
ShGeneric<1, T>& ShGeneric<1, T>::operator%=(const ShGeneric<1, T>& right)
{
  *this = *this % right;
  return *this;
}

template<typename T>
ShGeneric<1, T>& ShGeneric<1, T>::operator*=(T right)
{
  *this = *this * right;
  return *this;
}

template<typename T>
ShGeneric<1, T>& ShGeneric<1, T>::operator/=(T right)
{
  *this = *this / right;
  return *this;
}

template<typename T>
ShGeneric<1, T>& ShGeneric<1, T>::operator%=(T right)
{
  *this = *this % right;
  return *this;
}

template<typename T>
ShGeneric<1, T>& ShGeneric<1, T>::operator+=(T right)
{
  *this = *this + right;
  return *this;
}

template<typename T>
ShGeneric<1, T>& ShGeneric<1, T>::operator-=(T right)
{
  *this = *this - right;
  return *this;
}

template<typename T>
ShGeneric<1, T> ShGeneric<1, T>::operator-() const
{
  return ShGeneric<1, T>(m_node, m_swizzle, !m_neg);
}


template<typename T>
ShGeneric<1, T> ShGeneric<1, T>::operator()() const
{
  return ShGeneric<1, T>(m_node, m_swizzle, m_neg);
}

template<typename T>
ShGeneric<1, T> ShGeneric<1, T>::operator()(int i1) const
{
  return ShGeneric<1, T>(m_node, m_swizzle * ShSwizzle(size(), i1), m_neg);
}

template<typename T>
ShGeneric<1, T> ShGeneric<1, T>::operator[](int i1) const
{
  return ShGeneric<1, T>(m_node, m_swizzle * ShSwizzle(size(), i1), m_neg);
}

template<typename T>
ShGeneric<2, T> ShGeneric<1, T>::operator()(int i1, int i2) const
{
  return ShGeneric<2, T>(m_node, m_swizzle * ShSwizzle(size(), i1, i2), m_neg);
}

template<typename T>
ShGeneric<3, T> ShGeneric<1, T>::operator()(int i1, int i2, int i3) const
{
  return ShGeneric<3, T>(m_node, m_swizzle * ShSwizzle(size(), i1, i2, i3), m_neg);
}

template<typename T>
ShGeneric<4, T> ShGeneric<1, T>::operator()(int i1, int i2, int i3, int i4) const
{
  return ShGeneric<4, T>(m_node, m_swizzle * ShSwizzle(size(), i1, i2, i3, i4), m_neg);
}
  
template<typename T> 
template<int N2>
ShGeneric<N2, T> ShGeneric<1, T>::swiz(int indices[]) const
{
  return ShGeneric<N2, T>(m_node, m_swizzle * ShSwizzle(1, N2, indices), m_neg);
}

}

#endif
