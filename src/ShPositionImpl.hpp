// WARNING - DO NOT EDIT THIS FILE:
// This file was automatically generated from scripts/ShPositionImpl.hpp.py.
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

#ifndef SH_SHPOSITIONIMPL_HPP
#define SH_SHPOSITIONIMPL_HPP

#include "ShPosition.hpp"

namespace SH {

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShPosition<N, Binding, T, Swizzled>::ShPosition()
{
  m_node->specialType(SH_POSITION);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<N, Binding, T, Swizzled>::ShPosition(const ShGeneric<N, T2>& other)
  : ParentType(other)
{
  m_node->specialType(SH_POSITION);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShPosition<N, Binding, T, Swizzled>::ShPosition(const ShPosition<N, Binding, T, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_POSITION);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<N, Binding, T, Swizzled>::ShPosition(const ShPosition<N, Binding, T2, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_POSITION);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShPosition<N, Binding, T, Swizzled>::ShPosition(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg)
  : ParentType(node, swizzle, neg)
{
  m_node->specialType(SH_POSITION);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShPosition<N, Binding, T, Swizzled>::ShPosition(T data[N])
  : ParentType(data)
{
  m_node->specialType(SH_POSITION);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShPosition<N, Binding, T, Swizzled>::~ShPosition()
{
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<N, Binding, T, Swizzled>&
ShPosition<N, Binding, T, Swizzled>::operator=(const ShGeneric<N, T2>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShPosition<N, Binding, T, Swizzled>&
ShPosition<N, Binding, T, Swizzled>::operator=(const ShPosition<N, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<N, Binding, T, Swizzled>&
ShPosition<N, Binding, T, Swizzled>::operator=(const ShPosition<N, Binding, T2, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<N, Binding, T, Swizzled>&
ShPosition<N, Binding, T, Swizzled>::operator+=(const ShGeneric<N, T2>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<N, Binding, T, Swizzled>&
ShPosition<N, Binding, T, Swizzled>::operator-=(const ShGeneric<N, T2>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<N, Binding, T, Swizzled>&
ShPosition<N, Binding, T, Swizzled>::operator*=(const ShGeneric<N, T2>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<N, Binding, T, Swizzled>&
ShPosition<N, Binding, T, Swizzled>::operator/=(const ShGeneric<N, T2>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<N, Binding, T, Swizzled>&
ShPosition<N, Binding, T, Swizzled>::operator%=(const ShGeneric<N, T2>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShPosition<N, Binding, T, Swizzled>&
ShPosition<N, Binding, T, Swizzled>::operator+=(T right)
{
  ParentType::operator+=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShPosition<N, Binding, T, Swizzled>&
ShPosition<N, Binding, T, Swizzled>::operator-=(T right)
{
  ParentType::operator-=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShPosition<N, Binding, T, Swizzled>&
ShPosition<N, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShPosition<N, Binding, T, Swizzled>&
ShPosition<N, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShPosition<N, Binding, T, Swizzled>&
ShPosition<N, Binding, T, Swizzled>::operator%=(T right)
{
  ParentType::operator%=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<N, Binding, T, Swizzled>&
ShPosition<N, Binding, T, Swizzled>::operator+=(const ShGeneric<1, T2>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<N, Binding, T, Swizzled>&
ShPosition<N, Binding, T, Swizzled>::operator-=(const ShGeneric<1, T2>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<N, Binding, T, Swizzled>&
ShPosition<N, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T2>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<N, Binding, T, Swizzled>&
ShPosition<N, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T2>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<N, Binding, T, Swizzled>&
ShPosition<N, Binding, T, Swizzled>::operator%=(const ShGeneric<1, T2>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShPosition<1, Binding, T, true>
ShPosition<N, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShPosition<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, s0), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShPosition<2, Binding, T, true>
ShPosition<N, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShPosition<2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, s0, s1), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShPosition<3, Binding, T, true>
ShPosition<N, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShPosition<3, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, s0, s1, s2), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShPosition<4, Binding, T, true>
ShPosition<N, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShPosition<4, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, s0, s1, s2, s3), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShPosition<N2, Binding, T, true>
ShPosition<N, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShPosition<N2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, N2, indices), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShPosition<1, Binding, T, true>
ShPosition<N, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShPosition<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, s0), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShPosition<N, Binding, T, Swizzled>
ShPosition<N, Binding, T, Swizzled>::operator-() const
{
  return ShPosition<N, Binding, T, Swizzled>(m_node, m_swizzle, !m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<1, Binding, T, Swizzled>::ShPosition()
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<1, Binding, T, Swizzled>::ShPosition(const ShGeneric<1, T2>& other)
  : ParentType(other)
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<1, Binding, T, Swizzled>::ShPosition(const ShPosition<1, Binding, T, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<1, Binding, T, Swizzled>::ShPosition(const ShPosition<1, Binding, T2, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<1, Binding, T, Swizzled>::ShPosition(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg)
  : ParentType(node, swizzle, neg)
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<1, Binding, T, Swizzled>::ShPosition(T data[1])
  : ParentType(data)
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<1, Binding, T, Swizzled>::ShPosition(T s0)
  : ParentType(s0)
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<1, Binding, T, Swizzled>::~ShPosition()
{
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<1, Binding, T, Swizzled>&
ShPosition<1, Binding, T, Swizzled>::operator=(const ShGeneric<1, T2>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<1, Binding, T, Swizzled>&
ShPosition<1, Binding, T, Swizzled>::operator=(const ShPosition<1, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<1, Binding, T, Swizzled>&
ShPosition<1, Binding, T, Swizzled>::operator=(const ShPosition<1, Binding, T2, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<1, Binding, T, Swizzled>&
ShPosition<1, Binding, T, Swizzled>::operator=(T other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<1, Binding, T, Swizzled>&
ShPosition<1, Binding, T, Swizzled>::operator+=(const ShGeneric<1, T2>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<1, Binding, T, Swizzled>&
ShPosition<1, Binding, T, Swizzled>::operator-=(const ShGeneric<1, T2>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<1, Binding, T, Swizzled>&
ShPosition<1, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T2>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<1, Binding, T, Swizzled>&
ShPosition<1, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T2>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<1, Binding, T, Swizzled>&
ShPosition<1, Binding, T, Swizzled>::operator%=(const ShGeneric<1, T2>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<1, Binding, T, Swizzled>&
ShPosition<1, Binding, T, Swizzled>::operator+=(T right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<1, Binding, T, Swizzled>&
ShPosition<1, Binding, T, Swizzled>::operator-=(T right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<1, Binding, T, Swizzled>&
ShPosition<1, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<1, Binding, T, Swizzled>&
ShPosition<1, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<1, Binding, T, Swizzled>&
ShPosition<1, Binding, T, Swizzled>::operator%=(T right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<1, Binding, T, true>
ShPosition<1, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShPosition<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<2, Binding, T, true>
ShPosition<1, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShPosition<2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, s0, s1), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<3, Binding, T, true>
ShPosition<1, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShPosition<3, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, s0, s1, s2), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<4, Binding, T, true>
ShPosition<1, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShPosition<4, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, s0, s1, s2, s3), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShPosition<N2, Binding, T, true>
ShPosition<1, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShPosition<N2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, N2, indices), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<1, Binding, T, true>
ShPosition<1, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShPosition<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<1, Binding, T, Swizzled>
ShPosition<1, Binding, T, Swizzled>::operator-() const
{
  return ShPosition<1, Binding, T, Swizzled>(m_node, m_swizzle, !m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<2, Binding, T, Swizzled>::ShPosition()
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<2, Binding, T, Swizzled>::ShPosition(const ShGeneric<2, T2>& other)
  : ParentType(other)
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<2, Binding, T, Swizzled>::ShPosition(const ShPosition<2, Binding, T, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<2, Binding, T, Swizzled>::ShPosition(const ShPosition<2, Binding, T2, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<2, Binding, T, Swizzled>::ShPosition(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg)
  : ParentType(node, swizzle, neg)
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<2, Binding, T, Swizzled>::ShPosition(T data[2])
  : ParentType(data)
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<2, Binding, T, Swizzled>::ShPosition(T s0, T s1)
  : ParentType(s0, s1)
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2, typename T3>
ShPosition<2, Binding, T, Swizzled>::ShPosition(const ShGeneric<1, T2>& s0, const ShGeneric<1, T3>& s1)
  : ParentType(s0, s1)
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<2, Binding, T, Swizzled>::~ShPosition()
{
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<2, Binding, T, Swizzled>&
ShPosition<2, Binding, T, Swizzled>::operator=(const ShGeneric<2, T2>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<2, Binding, T, Swizzled>&
ShPosition<2, Binding, T, Swizzled>::operator=(const ShPosition<2, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<2, Binding, T, Swizzled>&
ShPosition<2, Binding, T, Swizzled>::operator=(const ShPosition<2, Binding, T2, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<2, Binding, T, Swizzled>&
ShPosition<2, Binding, T, Swizzled>::operator+=(const ShGeneric<2, T2>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<2, Binding, T, Swizzled>&
ShPosition<2, Binding, T, Swizzled>::operator-=(const ShGeneric<2, T2>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<2, Binding, T, Swizzled>&
ShPosition<2, Binding, T, Swizzled>::operator*=(const ShGeneric<2, T2>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<2, Binding, T, Swizzled>&
ShPosition<2, Binding, T, Swizzled>::operator/=(const ShGeneric<2, T2>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<2, Binding, T, Swizzled>&
ShPosition<2, Binding, T, Swizzled>::operator%=(const ShGeneric<2, T2>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<2, Binding, T, Swizzled>&
ShPosition<2, Binding, T, Swizzled>::operator+=(T right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<2, Binding, T, Swizzled>&
ShPosition<2, Binding, T, Swizzled>::operator-=(T right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<2, Binding, T, Swizzled>&
ShPosition<2, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<2, Binding, T, Swizzled>&
ShPosition<2, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<2, Binding, T, Swizzled>&
ShPosition<2, Binding, T, Swizzled>::operator%=(T right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<2, Binding, T, Swizzled>&
ShPosition<2, Binding, T, Swizzled>::operator+=(const ShGeneric<1, T2>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<2, Binding, T, Swizzled>&
ShPosition<2, Binding, T, Swizzled>::operator-=(const ShGeneric<1, T2>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<2, Binding, T, Swizzled>&
ShPosition<2, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T2>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<2, Binding, T, Swizzled>&
ShPosition<2, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T2>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<2, Binding, T, Swizzled>&
ShPosition<2, Binding, T, Swizzled>::operator%=(const ShGeneric<1, T2>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<1, Binding, T, true>
ShPosition<2, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShPosition<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<2, Binding, T, true>
ShPosition<2, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShPosition<2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, s0, s1), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<3, Binding, T, true>
ShPosition<2, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShPosition<3, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, s0, s1, s2), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<4, Binding, T, true>
ShPosition<2, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShPosition<4, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, s0, s1, s2, s3), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShPosition<N2, Binding, T, true>
ShPosition<2, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShPosition<N2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, N2, indices), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<1, Binding, T, true>
ShPosition<2, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShPosition<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<2, Binding, T, Swizzled>
ShPosition<2, Binding, T, Swizzled>::operator-() const
{
  return ShPosition<2, Binding, T, Swizzled>(m_node, m_swizzle, !m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<3, Binding, T, Swizzled>::ShPosition()
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<3, Binding, T, Swizzled>::ShPosition(const ShGeneric<3, T2>& other)
  : ParentType(other)
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<3, Binding, T, Swizzled>::ShPosition(const ShPosition<3, Binding, T, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<3, Binding, T, Swizzled>::ShPosition(const ShPosition<3, Binding, T2, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<3, Binding, T, Swizzled>::ShPosition(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg)
  : ParentType(node, swizzle, neg)
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<3, Binding, T, Swizzled>::ShPosition(T data[3])
  : ParentType(data)
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<3, Binding, T, Swizzled>::ShPosition(T s0, T s1, T s2)
  : ParentType(s0, s1, s2)
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2, typename T3, typename T4>
ShPosition<3, Binding, T, Swizzled>::ShPosition(const ShGeneric<1, T2>& s0, const ShGeneric<1, T3>& s1, const ShGeneric<1, T4>& s2)
  : ParentType(s0, s1, s2)
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<3, Binding, T, Swizzled>::~ShPosition()
{
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<3, Binding, T, Swizzled>&
ShPosition<3, Binding, T, Swizzled>::operator=(const ShGeneric<3, T2>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<3, Binding, T, Swizzled>&
ShPosition<3, Binding, T, Swizzled>::operator=(const ShPosition<3, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<3, Binding, T, Swizzled>&
ShPosition<3, Binding, T, Swizzled>::operator=(const ShPosition<3, Binding, T2, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<3, Binding, T, Swizzled>&
ShPosition<3, Binding, T, Swizzled>::operator+=(const ShGeneric<3, T2>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<3, Binding, T, Swizzled>&
ShPosition<3, Binding, T, Swizzled>::operator-=(const ShGeneric<3, T2>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<3, Binding, T, Swizzled>&
ShPosition<3, Binding, T, Swizzled>::operator*=(const ShGeneric<3, T2>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<3, Binding, T, Swizzled>&
ShPosition<3, Binding, T, Swizzled>::operator/=(const ShGeneric<3, T2>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<3, Binding, T, Swizzled>&
ShPosition<3, Binding, T, Swizzled>::operator%=(const ShGeneric<3, T2>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<3, Binding, T, Swizzled>&
ShPosition<3, Binding, T, Swizzled>::operator+=(T right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<3, Binding, T, Swizzled>&
ShPosition<3, Binding, T, Swizzled>::operator-=(T right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<3, Binding, T, Swizzled>&
ShPosition<3, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<3, Binding, T, Swizzled>&
ShPosition<3, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<3, Binding, T, Swizzled>&
ShPosition<3, Binding, T, Swizzled>::operator%=(T right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<3, Binding, T, Swizzled>&
ShPosition<3, Binding, T, Swizzled>::operator+=(const ShGeneric<1, T2>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<3, Binding, T, Swizzled>&
ShPosition<3, Binding, T, Swizzled>::operator-=(const ShGeneric<1, T2>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<3, Binding, T, Swizzled>&
ShPosition<3, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T2>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<3, Binding, T, Swizzled>&
ShPosition<3, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T2>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<3, Binding, T, Swizzled>&
ShPosition<3, Binding, T, Swizzled>::operator%=(const ShGeneric<1, T2>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<1, Binding, T, true>
ShPosition<3, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShPosition<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<2, Binding, T, true>
ShPosition<3, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShPosition<2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, s0, s1), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<3, Binding, T, true>
ShPosition<3, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShPosition<3, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, s0, s1, s2), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<4, Binding, T, true>
ShPosition<3, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShPosition<4, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, s0, s1, s2, s3), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShPosition<N2, Binding, T, true>
ShPosition<3, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShPosition<N2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, N2, indices), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<1, Binding, T, true>
ShPosition<3, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShPosition<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<3, Binding, T, Swizzled>
ShPosition<3, Binding, T, Swizzled>::operator-() const
{
  return ShPosition<3, Binding, T, Swizzled>(m_node, m_swizzle, !m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<4, Binding, T, Swizzled>::ShPosition()
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<4, Binding, T, Swizzled>::ShPosition(const ShGeneric<4, T2>& other)
  : ParentType(other)
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<4, Binding, T, Swizzled>::ShPosition(const ShPosition<4, Binding, T, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<4, Binding, T, Swizzled>::ShPosition(const ShPosition<4, Binding, T2, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<4, Binding, T, Swizzled>::ShPosition(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg)
  : ParentType(node, swizzle, neg)
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<4, Binding, T, Swizzled>::ShPosition(T data[4])
  : ParentType(data)
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<4, Binding, T, Swizzled>::ShPosition(T s0, T s1, T s2, T s3)
  : ParentType(s0, s1, s2, s3)
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2, typename T3, typename T4, typename T5>
ShPosition<4, Binding, T, Swizzled>::ShPosition(const ShGeneric<1, T2>& s0, const ShGeneric<1, T3>& s1, const ShGeneric<1, T4>& s2, const ShGeneric<1, T5>& s3)
  : ParentType(s0, s1, s2, s3)
{
  m_node->specialType(SH_POSITION);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<4, Binding, T, Swizzled>::~ShPosition()
{
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<4, Binding, T, Swizzled>&
ShPosition<4, Binding, T, Swizzled>::operator=(const ShGeneric<4, T2>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<4, Binding, T, Swizzled>&
ShPosition<4, Binding, T, Swizzled>::operator=(const ShPosition<4, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<4, Binding, T, Swizzled>&
ShPosition<4, Binding, T, Swizzled>::operator=(const ShPosition<4, Binding, T2, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<4, Binding, T, Swizzled>&
ShPosition<4, Binding, T, Swizzled>::operator+=(const ShGeneric<4, T2>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<4, Binding, T, Swizzled>&
ShPosition<4, Binding, T, Swizzled>::operator-=(const ShGeneric<4, T2>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<4, Binding, T, Swizzled>&
ShPosition<4, Binding, T, Swizzled>::operator*=(const ShGeneric<4, T2>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<4, Binding, T, Swizzled>&
ShPosition<4, Binding, T, Swizzled>::operator/=(const ShGeneric<4, T2>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<4, Binding, T, Swizzled>&
ShPosition<4, Binding, T, Swizzled>::operator%=(const ShGeneric<4, T2>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<4, Binding, T, Swizzled>&
ShPosition<4, Binding, T, Swizzled>::operator+=(T right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<4, Binding, T, Swizzled>&
ShPosition<4, Binding, T, Swizzled>::operator-=(T right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<4, Binding, T, Swizzled>&
ShPosition<4, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<4, Binding, T, Swizzled>&
ShPosition<4, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<4, Binding, T, Swizzled>&
ShPosition<4, Binding, T, Swizzled>::operator%=(T right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<4, Binding, T, Swizzled>&
ShPosition<4, Binding, T, Swizzled>::operator+=(const ShGeneric<1, T2>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<4, Binding, T, Swizzled>&
ShPosition<4, Binding, T, Swizzled>::operator-=(const ShGeneric<1, T2>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<4, Binding, T, Swizzled>&
ShPosition<4, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T2>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<4, Binding, T, Swizzled>&
ShPosition<4, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T2>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShPosition<4, Binding, T, Swizzled>&
ShPosition<4, Binding, T, Swizzled>::operator%=(const ShGeneric<1, T2>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<1, Binding, T, true>
ShPosition<4, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShPosition<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<2, Binding, T, true>
ShPosition<4, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShPosition<2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, s0, s1), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<3, Binding, T, true>
ShPosition<4, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShPosition<3, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, s0, s1, s2), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<4, Binding, T, true>
ShPosition<4, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShPosition<4, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, s0, s1, s2, s3), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShPosition<N2, Binding, T, true>
ShPosition<4, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShPosition<N2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, N2, indices), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<1, Binding, T, true>
ShPosition<4, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShPosition<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPosition<4, Binding, T, Swizzled>
ShPosition<4, Binding, T, Swizzled>::operator-() const
{
  return ShPosition<4, Binding, T, Swizzled>(m_node, m_swizzle, !m_neg);
}


} // namespace SH

#endif // SH_SHPOSITIONIMPL_HPP
