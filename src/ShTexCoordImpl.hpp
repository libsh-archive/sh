// WARNING - DO NOT EDIT THIS FILE:
// This file was automatically generated from scripts/ShTexCoordImpl.hpp.py.
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

#ifndef SH_SHTEXCOORDIMPL_HPP
#define SH_SHTEXCOORDIMPL_HPP

#include "ShTexCoord.hpp"

namespace SH {

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<N, Binding, T, Swizzled>::ShTexCoord()
{
  m_node->specialType(SH_TEXCOORD);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<N, Binding, T, Swizzled>::ShTexCoord(const ShGeneric<N, T2>& other)
  : ParentType(other)
{
  m_node->specialType(SH_TEXCOORD);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<N, Binding, T, Swizzled>::ShTexCoord(const ShTexCoord<N, Binding, T, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_TEXCOORD);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<N, Binding, T, Swizzled>::ShTexCoord(const ShTexCoord<N, Binding, T2, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_TEXCOORD);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<N, Binding, T, Swizzled>::ShTexCoord(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg)
  : ParentType(node, swizzle, neg)
{
  m_node->specialType(SH_TEXCOORD);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<N, Binding, T, Swizzled>::ShTexCoord(T data[N])
  : ParentType(data)
{
  m_node->specialType(SH_TEXCOORD);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<N, Binding, T, Swizzled>::~ShTexCoord()
{
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<N, Binding, T, Swizzled>&
ShTexCoord<N, Binding, T, Swizzled>::operator=(const ShGeneric<N, T2>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<N, Binding, T, Swizzled>&
ShTexCoord<N, Binding, T, Swizzled>::operator=(const ShTexCoord<N, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<N, Binding, T, Swizzled>&
ShTexCoord<N, Binding, T, Swizzled>::operator=(const ShTexCoord<N, Binding, T2, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<N, Binding, T, Swizzled>&
ShTexCoord<N, Binding, T, Swizzled>::operator+=(const ShGeneric<N, T2>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<N, Binding, T, Swizzled>&
ShTexCoord<N, Binding, T, Swizzled>::operator-=(const ShGeneric<N, T2>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<N, Binding, T, Swizzled>&
ShTexCoord<N, Binding, T, Swizzled>::operator*=(const ShGeneric<N, T2>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<N, Binding, T, Swizzled>&
ShTexCoord<N, Binding, T, Swizzled>::operator/=(const ShGeneric<N, T2>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<N, Binding, T, Swizzled>&
ShTexCoord<N, Binding, T, Swizzled>::operator%=(const ShGeneric<N, T2>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<N, Binding, T, Swizzled>&
ShTexCoord<N, Binding, T, Swizzled>::operator+=(T right)
{
  ParentType::operator+=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<N, Binding, T, Swizzled>&
ShTexCoord<N, Binding, T, Swizzled>::operator-=(T right)
{
  ParentType::operator-=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<N, Binding, T, Swizzled>&
ShTexCoord<N, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<N, Binding, T, Swizzled>&
ShTexCoord<N, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<N, Binding, T, Swizzled>&
ShTexCoord<N, Binding, T, Swizzled>::operator%=(T right)
{
  ParentType::operator%=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<N, Binding, T, Swizzled>&
ShTexCoord<N, Binding, T, Swizzled>::operator+=(const ShGeneric<1, T2>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<N, Binding, T, Swizzled>&
ShTexCoord<N, Binding, T, Swizzled>::operator-=(const ShGeneric<1, T2>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<N, Binding, T, Swizzled>&
ShTexCoord<N, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T2>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<N, Binding, T, Swizzled>&
ShTexCoord<N, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T2>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<N, Binding, T, Swizzled>&
ShTexCoord<N, Binding, T, Swizzled>::operator%=(const ShGeneric<1, T2>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<1, Binding, T, true>
ShTexCoord<N, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShTexCoord<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, s0), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<2, Binding, T, true>
ShTexCoord<N, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShTexCoord<2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, s0, s1), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<3, Binding, T, true>
ShTexCoord<N, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShTexCoord<3, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, s0, s1, s2), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<4, Binding, T, true>
ShTexCoord<N, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShTexCoord<4, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, s0, s1, s2, s3), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShTexCoord<N2, Binding, T, true>
ShTexCoord<N, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShTexCoord<N2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, N2, indices), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<1, Binding, T, true>
ShTexCoord<N, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShTexCoord<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, s0), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<N, Binding, T, Swizzled>
ShTexCoord<N, Binding, T, Swizzled>::operator-() const
{
  return ShTexCoord<N, Binding, T, Swizzled>(m_node, m_swizzle, !m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<1, Binding, T, Swizzled>::ShTexCoord()
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<1, Binding, T, Swizzled>::ShTexCoord(const ShGeneric<1, T2>& other)
  : ParentType(other)
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<1, Binding, T, Swizzled>::ShTexCoord(const ShTexCoord<1, Binding, T, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<1, Binding, T, Swizzled>::ShTexCoord(const ShTexCoord<1, Binding, T2, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<1, Binding, T, Swizzled>::ShTexCoord(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg)
  : ParentType(node, swizzle, neg)
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<1, Binding, T, Swizzled>::ShTexCoord(T data[1])
  : ParentType(data)
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<1, Binding, T, Swizzled>::ShTexCoord(T s0)
  : ParentType(s0)
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<1, Binding, T, Swizzled>::~ShTexCoord()
{
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<1, Binding, T, Swizzled>&
ShTexCoord<1, Binding, T, Swizzled>::operator=(const ShGeneric<1, T2>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<1, Binding, T, Swizzled>&
ShTexCoord<1, Binding, T, Swizzled>::operator=(const ShTexCoord<1, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<1, Binding, T, Swizzled>&
ShTexCoord<1, Binding, T, Swizzled>::operator=(const ShTexCoord<1, Binding, T2, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<1, Binding, T, Swizzled>&
ShTexCoord<1, Binding, T, Swizzled>::operator=(T other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<1, Binding, T, Swizzled>&
ShTexCoord<1, Binding, T, Swizzled>::operator+=(const ShGeneric<1, T2>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<1, Binding, T, Swizzled>&
ShTexCoord<1, Binding, T, Swizzled>::operator-=(const ShGeneric<1, T2>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<1, Binding, T, Swizzled>&
ShTexCoord<1, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T2>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<1, Binding, T, Swizzled>&
ShTexCoord<1, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T2>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<1, Binding, T, Swizzled>&
ShTexCoord<1, Binding, T, Swizzled>::operator%=(const ShGeneric<1, T2>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<1, Binding, T, Swizzled>&
ShTexCoord<1, Binding, T, Swizzled>::operator+=(T right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<1, Binding, T, Swizzled>&
ShTexCoord<1, Binding, T, Swizzled>::operator-=(T right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<1, Binding, T, Swizzled>&
ShTexCoord<1, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<1, Binding, T, Swizzled>&
ShTexCoord<1, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<1, Binding, T, Swizzled>&
ShTexCoord<1, Binding, T, Swizzled>::operator%=(T right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<1, Binding, T, true>
ShTexCoord<1, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShTexCoord<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<2, Binding, T, true>
ShTexCoord<1, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShTexCoord<2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, s0, s1), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<3, Binding, T, true>
ShTexCoord<1, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShTexCoord<3, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, s0, s1, s2), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<4, Binding, T, true>
ShTexCoord<1, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShTexCoord<4, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, s0, s1, s2, s3), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShTexCoord<N2, Binding, T, true>
ShTexCoord<1, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShTexCoord<N2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, N2, indices), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<1, Binding, T, true>
ShTexCoord<1, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShTexCoord<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<1, Binding, T, Swizzled>
ShTexCoord<1, Binding, T, Swizzled>::operator-() const
{
  return ShTexCoord<1, Binding, T, Swizzled>(m_node, m_swizzle, !m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<2, Binding, T, Swizzled>::ShTexCoord()
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<2, Binding, T, Swizzled>::ShTexCoord(const ShGeneric<2, T2>& other)
  : ParentType(other)
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<2, Binding, T, Swizzled>::ShTexCoord(const ShTexCoord<2, Binding, T, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<2, Binding, T, Swizzled>::ShTexCoord(const ShTexCoord<2, Binding, T2, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<2, Binding, T, Swizzled>::ShTexCoord(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg)
  : ParentType(node, swizzle, neg)
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<2, Binding, T, Swizzled>::ShTexCoord(T data[2])
  : ParentType(data)
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<2, Binding, T, Swizzled>::ShTexCoord(T s0, T s1)
  : ParentType(s0, s1)
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2, typename T3>
ShTexCoord<2, Binding, T, Swizzled>::ShTexCoord(const ShGeneric<1, T2>& s0, const ShGeneric<1, T3>& s1)
  : ParentType(s0, s1)
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<2, Binding, T, Swizzled>::~ShTexCoord()
{
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<2, Binding, T, Swizzled>&
ShTexCoord<2, Binding, T, Swizzled>::operator=(const ShGeneric<2, T2>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<2, Binding, T, Swizzled>&
ShTexCoord<2, Binding, T, Swizzled>::operator=(const ShTexCoord<2, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<2, Binding, T, Swizzled>&
ShTexCoord<2, Binding, T, Swizzled>::operator=(const ShTexCoord<2, Binding, T2, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<2, Binding, T, Swizzled>&
ShTexCoord<2, Binding, T, Swizzled>::operator+=(const ShGeneric<2, T2>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<2, Binding, T, Swizzled>&
ShTexCoord<2, Binding, T, Swizzled>::operator-=(const ShGeneric<2, T2>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<2, Binding, T, Swizzled>&
ShTexCoord<2, Binding, T, Swizzled>::operator*=(const ShGeneric<2, T2>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<2, Binding, T, Swizzled>&
ShTexCoord<2, Binding, T, Swizzled>::operator/=(const ShGeneric<2, T2>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<2, Binding, T, Swizzled>&
ShTexCoord<2, Binding, T, Swizzled>::operator%=(const ShGeneric<2, T2>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<2, Binding, T, Swizzled>&
ShTexCoord<2, Binding, T, Swizzled>::operator+=(T right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<2, Binding, T, Swizzled>&
ShTexCoord<2, Binding, T, Swizzled>::operator-=(T right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<2, Binding, T, Swizzled>&
ShTexCoord<2, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<2, Binding, T, Swizzled>&
ShTexCoord<2, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<2, Binding, T, Swizzled>&
ShTexCoord<2, Binding, T, Swizzled>::operator%=(T right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<2, Binding, T, Swizzled>&
ShTexCoord<2, Binding, T, Swizzled>::operator+=(const ShGeneric<1, T2>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<2, Binding, T, Swizzled>&
ShTexCoord<2, Binding, T, Swizzled>::operator-=(const ShGeneric<1, T2>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<2, Binding, T, Swizzled>&
ShTexCoord<2, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T2>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<2, Binding, T, Swizzled>&
ShTexCoord<2, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T2>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<2, Binding, T, Swizzled>&
ShTexCoord<2, Binding, T, Swizzled>::operator%=(const ShGeneric<1, T2>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<1, Binding, T, true>
ShTexCoord<2, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShTexCoord<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<2, Binding, T, true>
ShTexCoord<2, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShTexCoord<2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, s0, s1), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<3, Binding, T, true>
ShTexCoord<2, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShTexCoord<3, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, s0, s1, s2), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<4, Binding, T, true>
ShTexCoord<2, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShTexCoord<4, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, s0, s1, s2, s3), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShTexCoord<N2, Binding, T, true>
ShTexCoord<2, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShTexCoord<N2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, N2, indices), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<1, Binding, T, true>
ShTexCoord<2, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShTexCoord<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<2, Binding, T, Swizzled>
ShTexCoord<2, Binding, T, Swizzled>::operator-() const
{
  return ShTexCoord<2, Binding, T, Swizzled>(m_node, m_swizzle, !m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<3, Binding, T, Swizzled>::ShTexCoord()
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<3, Binding, T, Swizzled>::ShTexCoord(const ShGeneric<3, T2>& other)
  : ParentType(other)
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<3, Binding, T, Swizzled>::ShTexCoord(const ShTexCoord<3, Binding, T, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<3, Binding, T, Swizzled>::ShTexCoord(const ShTexCoord<3, Binding, T2, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<3, Binding, T, Swizzled>::ShTexCoord(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg)
  : ParentType(node, swizzle, neg)
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<3, Binding, T, Swizzled>::ShTexCoord(T data[3])
  : ParentType(data)
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<3, Binding, T, Swizzled>::ShTexCoord(T s0, T s1, T s2)
  : ParentType(s0, s1, s2)
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2, typename T3, typename T4>
ShTexCoord<3, Binding, T, Swizzled>::ShTexCoord(const ShGeneric<1, T2>& s0, const ShGeneric<1, T3>& s1, const ShGeneric<1, T4>& s2)
  : ParentType(s0, s1, s2)
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<3, Binding, T, Swizzled>::~ShTexCoord()
{
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<3, Binding, T, Swizzled>&
ShTexCoord<3, Binding, T, Swizzled>::operator=(const ShGeneric<3, T2>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<3, Binding, T, Swizzled>&
ShTexCoord<3, Binding, T, Swizzled>::operator=(const ShTexCoord<3, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<3, Binding, T, Swizzled>&
ShTexCoord<3, Binding, T, Swizzled>::operator=(const ShTexCoord<3, Binding, T2, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<3, Binding, T, Swizzled>&
ShTexCoord<3, Binding, T, Swizzled>::operator+=(const ShGeneric<3, T2>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<3, Binding, T, Swizzled>&
ShTexCoord<3, Binding, T, Swizzled>::operator-=(const ShGeneric<3, T2>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<3, Binding, T, Swizzled>&
ShTexCoord<3, Binding, T, Swizzled>::operator*=(const ShGeneric<3, T2>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<3, Binding, T, Swizzled>&
ShTexCoord<3, Binding, T, Swizzled>::operator/=(const ShGeneric<3, T2>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<3, Binding, T, Swizzled>&
ShTexCoord<3, Binding, T, Swizzled>::operator%=(const ShGeneric<3, T2>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<3, Binding, T, Swizzled>&
ShTexCoord<3, Binding, T, Swizzled>::operator+=(T right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<3, Binding, T, Swizzled>&
ShTexCoord<3, Binding, T, Swizzled>::operator-=(T right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<3, Binding, T, Swizzled>&
ShTexCoord<3, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<3, Binding, T, Swizzled>&
ShTexCoord<3, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<3, Binding, T, Swizzled>&
ShTexCoord<3, Binding, T, Swizzled>::operator%=(T right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<3, Binding, T, Swizzled>&
ShTexCoord<3, Binding, T, Swizzled>::operator+=(const ShGeneric<1, T2>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<3, Binding, T, Swizzled>&
ShTexCoord<3, Binding, T, Swizzled>::operator-=(const ShGeneric<1, T2>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<3, Binding, T, Swizzled>&
ShTexCoord<3, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T2>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<3, Binding, T, Swizzled>&
ShTexCoord<3, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T2>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<3, Binding, T, Swizzled>&
ShTexCoord<3, Binding, T, Swizzled>::operator%=(const ShGeneric<1, T2>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<1, Binding, T, true>
ShTexCoord<3, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShTexCoord<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<2, Binding, T, true>
ShTexCoord<3, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShTexCoord<2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, s0, s1), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<3, Binding, T, true>
ShTexCoord<3, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShTexCoord<3, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, s0, s1, s2), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<4, Binding, T, true>
ShTexCoord<3, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShTexCoord<4, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, s0, s1, s2, s3), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShTexCoord<N2, Binding, T, true>
ShTexCoord<3, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShTexCoord<N2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, N2, indices), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<1, Binding, T, true>
ShTexCoord<3, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShTexCoord<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<3, Binding, T, Swizzled>
ShTexCoord<3, Binding, T, Swizzled>::operator-() const
{
  return ShTexCoord<3, Binding, T, Swizzled>(m_node, m_swizzle, !m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<4, Binding, T, Swizzled>::ShTexCoord()
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<4, Binding, T, Swizzled>::ShTexCoord(const ShGeneric<4, T2>& other)
  : ParentType(other)
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<4, Binding, T, Swizzled>::ShTexCoord(const ShTexCoord<4, Binding, T, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<4, Binding, T, Swizzled>::ShTexCoord(const ShTexCoord<4, Binding, T2, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<4, Binding, T, Swizzled>::ShTexCoord(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg)
  : ParentType(node, swizzle, neg)
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<4, Binding, T, Swizzled>::ShTexCoord(T data[4])
  : ParentType(data)
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<4, Binding, T, Swizzled>::ShTexCoord(T s0, T s1, T s2, T s3)
  : ParentType(s0, s1, s2, s3)
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2, typename T3, typename T4, typename T5>
ShTexCoord<4, Binding, T, Swizzled>::ShTexCoord(const ShGeneric<1, T2>& s0, const ShGeneric<1, T3>& s1, const ShGeneric<1, T4>& s2, const ShGeneric<1, T5>& s3)
  : ParentType(s0, s1, s2, s3)
{
  m_node->specialType(SH_TEXCOORD);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<4, Binding, T, Swizzled>::~ShTexCoord()
{
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<4, Binding, T, Swizzled>&
ShTexCoord<4, Binding, T, Swizzled>::operator=(const ShGeneric<4, T2>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<4, Binding, T, Swizzled>&
ShTexCoord<4, Binding, T, Swizzled>::operator=(const ShTexCoord<4, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<4, Binding, T, Swizzled>&
ShTexCoord<4, Binding, T, Swizzled>::operator=(const ShTexCoord<4, Binding, T2, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<4, Binding, T, Swizzled>&
ShTexCoord<4, Binding, T, Swizzled>::operator+=(const ShGeneric<4, T2>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<4, Binding, T, Swizzled>&
ShTexCoord<4, Binding, T, Swizzled>::operator-=(const ShGeneric<4, T2>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<4, Binding, T, Swizzled>&
ShTexCoord<4, Binding, T, Swizzled>::operator*=(const ShGeneric<4, T2>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<4, Binding, T, Swizzled>&
ShTexCoord<4, Binding, T, Swizzled>::operator/=(const ShGeneric<4, T2>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<4, Binding, T, Swizzled>&
ShTexCoord<4, Binding, T, Swizzled>::operator%=(const ShGeneric<4, T2>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<4, Binding, T, Swizzled>&
ShTexCoord<4, Binding, T, Swizzled>::operator+=(T right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<4, Binding, T, Swizzled>&
ShTexCoord<4, Binding, T, Swizzled>::operator-=(T right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<4, Binding, T, Swizzled>&
ShTexCoord<4, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<4, Binding, T, Swizzled>&
ShTexCoord<4, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<4, Binding, T, Swizzled>&
ShTexCoord<4, Binding, T, Swizzled>::operator%=(T right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<4, Binding, T, Swizzled>&
ShTexCoord<4, Binding, T, Swizzled>::operator+=(const ShGeneric<1, T2>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<4, Binding, T, Swizzled>&
ShTexCoord<4, Binding, T, Swizzled>::operator-=(const ShGeneric<1, T2>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<4, Binding, T, Swizzled>&
ShTexCoord<4, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T2>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<4, Binding, T, Swizzled>&
ShTexCoord<4, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T2>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShTexCoord<4, Binding, T, Swizzled>&
ShTexCoord<4, Binding, T, Swizzled>::operator%=(const ShGeneric<1, T2>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<1, Binding, T, true>
ShTexCoord<4, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShTexCoord<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<2, Binding, T, true>
ShTexCoord<4, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShTexCoord<2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, s0, s1), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<3, Binding, T, true>
ShTexCoord<4, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShTexCoord<3, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, s0, s1, s2), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<4, Binding, T, true>
ShTexCoord<4, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShTexCoord<4, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, s0, s1, s2, s3), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShTexCoord<N2, Binding, T, true>
ShTexCoord<4, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShTexCoord<N2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, N2, indices), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<1, Binding, T, true>
ShTexCoord<4, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShTexCoord<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShTexCoord<4, Binding, T, Swizzled>
ShTexCoord<4, Binding, T, Swizzled>::operator-() const
{
  return ShTexCoord<4, Binding, T, Swizzled>(m_node, m_swizzle, !m_neg);
}


} // namespace SH

#endif // SH_SHTEXCOORDIMPL_HPP
