// WARNING - DO NOT EDIT THIS FILE:
// This file was automatically generated from scripts/ShColorImpl.hpp.py.
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

#ifndef SH_SHCOLORIMPL_HPP
#define SH_SHCOLORIMPL_HPP

#include "ShColor.hpp"

namespace SH {

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShColor<N, Binding, T, Swizzled>::ShColor()
{
  m_node->specialType(SH_COLOR);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<N, Binding, T, Swizzled>::ShColor(const ShGeneric<N, T2>& other)
  : ParentType(other)
{
  m_node->specialType(SH_COLOR);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShColor<N, Binding, T, Swizzled>::ShColor(const ShColor<N, Binding, T, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_COLOR);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<N, Binding, T, Swizzled>::ShColor(const ShColor<N, Binding, T2, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_COLOR);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShColor<N, Binding, T, Swizzled>::ShColor(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg)
  : ParentType(node, swizzle, neg)
{
  m_node->specialType(SH_COLOR);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShColor<N, Binding, T, Swizzled>::ShColor(T data[N])
  : ParentType(data)
{
  m_node->specialType(SH_COLOR);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShColor<N, Binding, T, Swizzled>::~ShColor()
{
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<N, Binding, T, Swizzled>&
ShColor<N, Binding, T, Swizzled>::operator=(const ShGeneric<N, T2>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShColor<N, Binding, T, Swizzled>&
ShColor<N, Binding, T, Swizzled>::operator=(const ShColor<N, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<N, Binding, T, Swizzled>&
ShColor<N, Binding, T, Swizzled>::operator=(const ShColor<N, Binding, T2, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShColor<N, Binding, T, Swizzled>&
ShColor<N, Binding, T, Swizzled>::operator=(const ShProgram& prg)
{
  ParentType::operator=(prg);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<N, Binding, T, Swizzled>&
ShColor<N, Binding, T, Swizzled>::operator+=(const ShGeneric<N, T2>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<N, Binding, T, Swizzled>&
ShColor<N, Binding, T, Swizzled>::operator-=(const ShGeneric<N, T2>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<N, Binding, T, Swizzled>&
ShColor<N, Binding, T, Swizzled>::operator*=(const ShGeneric<N, T2>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<N, Binding, T, Swizzled>&
ShColor<N, Binding, T, Swizzled>::operator/=(const ShGeneric<N, T2>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<N, Binding, T, Swizzled>&
ShColor<N, Binding, T, Swizzled>::operator%=(const ShGeneric<N, T2>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShColor<N, Binding, T, Swizzled>&
ShColor<N, Binding, T, Swizzled>::operator+=(T right)
{
  ParentType::operator+=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShColor<N, Binding, T, Swizzled>&
ShColor<N, Binding, T, Swizzled>::operator-=(T right)
{
  ParentType::operator-=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShColor<N, Binding, T, Swizzled>&
ShColor<N, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShColor<N, Binding, T, Swizzled>&
ShColor<N, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShColor<N, Binding, T, Swizzled>&
ShColor<N, Binding, T, Swizzled>::operator%=(T right)
{
  ParentType::operator%=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<N, Binding, T, Swizzled>&
ShColor<N, Binding, T, Swizzled>::operator+=(const ShGeneric<1, T2>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<N, Binding, T, Swizzled>&
ShColor<N, Binding, T, Swizzled>::operator-=(const ShGeneric<1, T2>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<N, Binding, T, Swizzled>&
ShColor<N, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T2>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<N, Binding, T, Swizzled>&
ShColor<N, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T2>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<N, Binding, T, Swizzled>&
ShColor<N, Binding, T, Swizzled>::operator%=(const ShGeneric<1, T2>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShColor<1, Binding, T, true>
ShColor<N, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShColor<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, s0), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShColor<2, Binding, T, true>
ShColor<N, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShColor<2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, s0, s1), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShColor<3, Binding, T, true>
ShColor<N, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShColor<3, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, s0, s1, s2), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShColor<4, Binding, T, true>
ShColor<N, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShColor<4, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, s0, s1, s2, s3), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShColor<N2, Binding, T, true>
ShColor<N, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShColor<N2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, N2, indices), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShColor<1, Binding, T, true>
ShColor<N, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShColor<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, s0), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShColor<N, Binding, T, Swizzled>
ShColor<N, Binding, T, Swizzled>::operator-() const
{
  return ShColor<N, Binding, T, Swizzled>(m_node, m_swizzle, !m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<1, Binding, T, Swizzled>::ShColor()
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<1, Binding, T, Swizzled>::ShColor(const ShGeneric<1, T2>& other)
  : ParentType(other)
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<1, Binding, T, Swizzled>::ShColor(const ShColor<1, Binding, T, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<1, Binding, T, Swizzled>::ShColor(const ShColor<1, Binding, T2, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<1, Binding, T, Swizzled>::ShColor(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg)
  : ParentType(node, swizzle, neg)
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<1, Binding, T, Swizzled>::ShColor(T data[1])
  : ParentType(data)
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<1, Binding, T, Swizzled>::ShColor(T s0)
  : ParentType(s0)
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<1, Binding, T, Swizzled>::~ShColor()
{
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<1, Binding, T, Swizzled>&
ShColor<1, Binding, T, Swizzled>::operator=(const ShGeneric<1, T2>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<1, Binding, T, Swizzled>&
ShColor<1, Binding, T, Swizzled>::operator=(const ShColor<1, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<1, Binding, T, Swizzled>&
ShColor<1, Binding, T, Swizzled>::operator=(const ShColor<1, Binding, T2, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<1, Binding, T, Swizzled>&
ShColor<1, Binding, T, Swizzled>::operator=(T other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<1, Binding, T, Swizzled>&
ShColor<1, Binding, T, Swizzled>::operator=(const ShProgram& prg)
{
  ParentType::operator=(prg);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<1, Binding, T, Swizzled>&
ShColor<1, Binding, T, Swizzled>::operator+=(const ShGeneric<1, T2>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<1, Binding, T, Swizzled>&
ShColor<1, Binding, T, Swizzled>::operator-=(const ShGeneric<1, T2>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<1, Binding, T, Swizzled>&
ShColor<1, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T2>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<1, Binding, T, Swizzled>&
ShColor<1, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T2>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<1, Binding, T, Swizzled>&
ShColor<1, Binding, T, Swizzled>::operator%=(const ShGeneric<1, T2>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<1, Binding, T, Swizzled>&
ShColor<1, Binding, T, Swizzled>::operator+=(T right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<1, Binding, T, Swizzled>&
ShColor<1, Binding, T, Swizzled>::operator-=(T right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<1, Binding, T, Swizzled>&
ShColor<1, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<1, Binding, T, Swizzled>&
ShColor<1, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<1, Binding, T, Swizzled>&
ShColor<1, Binding, T, Swizzled>::operator%=(T right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<1, Binding, T, true>
ShColor<1, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShColor<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<2, Binding, T, true>
ShColor<1, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShColor<2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, s0, s1), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<3, Binding, T, true>
ShColor<1, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShColor<3, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, s0, s1, s2), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<4, Binding, T, true>
ShColor<1, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShColor<4, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, s0, s1, s2, s3), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShColor<N2, Binding, T, true>
ShColor<1, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShColor<N2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, N2, indices), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<1, Binding, T, true>
ShColor<1, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShColor<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<1, Binding, T, Swizzled>
ShColor<1, Binding, T, Swizzled>::operator-() const
{
  return ShColor<1, Binding, T, Swizzled>(m_node, m_swizzle, !m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<2, Binding, T, Swizzled>::ShColor()
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<2, Binding, T, Swizzled>::ShColor(const ShGeneric<2, T2>& other)
  : ParentType(other)
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<2, Binding, T, Swizzled>::ShColor(const ShColor<2, Binding, T, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<2, Binding, T, Swizzled>::ShColor(const ShColor<2, Binding, T2, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<2, Binding, T, Swizzled>::ShColor(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg)
  : ParentType(node, swizzle, neg)
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<2, Binding, T, Swizzled>::ShColor(T data[2])
  : ParentType(data)
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<2, Binding, T, Swizzled>::ShColor(T s0, T s1)
  : ParentType(s0, s1)
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2, typename T3>
ShColor<2, Binding, T, Swizzled>::ShColor(const ShGeneric<1, T2>& s0, const ShGeneric<1, T3>& s1)
  : ParentType(s0, s1)
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<2, Binding, T, Swizzled>::~ShColor()
{
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<2, Binding, T, Swizzled>&
ShColor<2, Binding, T, Swizzled>::operator=(const ShGeneric<2, T2>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<2, Binding, T, Swizzled>&
ShColor<2, Binding, T, Swizzled>::operator=(const ShColor<2, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<2, Binding, T, Swizzled>&
ShColor<2, Binding, T, Swizzled>::operator=(const ShColor<2, Binding, T2, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<2, Binding, T, Swizzled>&
ShColor<2, Binding, T, Swizzled>::operator=(const ShProgram& prg)
{
  ParentType::operator=(prg);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<2, Binding, T, Swizzled>&
ShColor<2, Binding, T, Swizzled>::operator+=(const ShGeneric<2, T2>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<2, Binding, T, Swizzled>&
ShColor<2, Binding, T, Swizzled>::operator-=(const ShGeneric<2, T2>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<2, Binding, T, Swizzled>&
ShColor<2, Binding, T, Swizzled>::operator*=(const ShGeneric<2, T2>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<2, Binding, T, Swizzled>&
ShColor<2, Binding, T, Swizzled>::operator/=(const ShGeneric<2, T2>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<2, Binding, T, Swizzled>&
ShColor<2, Binding, T, Swizzled>::operator%=(const ShGeneric<2, T2>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<2, Binding, T, Swizzled>&
ShColor<2, Binding, T, Swizzled>::operator+=(T right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<2, Binding, T, Swizzled>&
ShColor<2, Binding, T, Swizzled>::operator-=(T right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<2, Binding, T, Swizzled>&
ShColor<2, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<2, Binding, T, Swizzled>&
ShColor<2, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<2, Binding, T, Swizzled>&
ShColor<2, Binding, T, Swizzled>::operator%=(T right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<2, Binding, T, Swizzled>&
ShColor<2, Binding, T, Swizzled>::operator+=(const ShGeneric<1, T2>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<2, Binding, T, Swizzled>&
ShColor<2, Binding, T, Swizzled>::operator-=(const ShGeneric<1, T2>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<2, Binding, T, Swizzled>&
ShColor<2, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T2>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<2, Binding, T, Swizzled>&
ShColor<2, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T2>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<2, Binding, T, Swizzled>&
ShColor<2, Binding, T, Swizzled>::operator%=(const ShGeneric<1, T2>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<1, Binding, T, true>
ShColor<2, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShColor<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<2, Binding, T, true>
ShColor<2, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShColor<2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, s0, s1), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<3, Binding, T, true>
ShColor<2, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShColor<3, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, s0, s1, s2), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<4, Binding, T, true>
ShColor<2, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShColor<4, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, s0, s1, s2, s3), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShColor<N2, Binding, T, true>
ShColor<2, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShColor<N2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, N2, indices), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<1, Binding, T, true>
ShColor<2, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShColor<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<2, Binding, T, Swizzled>
ShColor<2, Binding, T, Swizzled>::operator-() const
{
  return ShColor<2, Binding, T, Swizzled>(m_node, m_swizzle, !m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<3, Binding, T, Swizzled>::ShColor()
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<3, Binding, T, Swizzled>::ShColor(const ShGeneric<3, T2>& other)
  : ParentType(other)
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<3, Binding, T, Swizzled>::ShColor(const ShColor<3, Binding, T, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<3, Binding, T, Swizzled>::ShColor(const ShColor<3, Binding, T2, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<3, Binding, T, Swizzled>::ShColor(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg)
  : ParentType(node, swizzle, neg)
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<3, Binding, T, Swizzled>::ShColor(T data[3])
  : ParentType(data)
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<3, Binding, T, Swizzled>::ShColor(T s0, T s1, T s2)
  : ParentType(s0, s1, s2)
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2, typename T3, typename T4>
ShColor<3, Binding, T, Swizzled>::ShColor(const ShGeneric<1, T2>& s0, const ShGeneric<1, T3>& s1, const ShGeneric<1, T4>& s2)
  : ParentType(s0, s1, s2)
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<3, Binding, T, Swizzled>::~ShColor()
{
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<3, Binding, T, Swizzled>&
ShColor<3, Binding, T, Swizzled>::operator=(const ShGeneric<3, T2>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<3, Binding, T, Swizzled>&
ShColor<3, Binding, T, Swizzled>::operator=(const ShColor<3, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<3, Binding, T, Swizzled>&
ShColor<3, Binding, T, Swizzled>::operator=(const ShColor<3, Binding, T2, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<3, Binding, T, Swizzled>&
ShColor<3, Binding, T, Swizzled>::operator=(const ShProgram& prg)
{
  ParentType::operator=(prg);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<3, Binding, T, Swizzled>&
ShColor<3, Binding, T, Swizzled>::operator+=(const ShGeneric<3, T2>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<3, Binding, T, Swizzled>&
ShColor<3, Binding, T, Swizzled>::operator-=(const ShGeneric<3, T2>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<3, Binding, T, Swizzled>&
ShColor<3, Binding, T, Swizzled>::operator*=(const ShGeneric<3, T2>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<3, Binding, T, Swizzled>&
ShColor<3, Binding, T, Swizzled>::operator/=(const ShGeneric<3, T2>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<3, Binding, T, Swizzled>&
ShColor<3, Binding, T, Swizzled>::operator%=(const ShGeneric<3, T2>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<3, Binding, T, Swizzled>&
ShColor<3, Binding, T, Swizzled>::operator+=(T right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<3, Binding, T, Swizzled>&
ShColor<3, Binding, T, Swizzled>::operator-=(T right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<3, Binding, T, Swizzled>&
ShColor<3, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<3, Binding, T, Swizzled>&
ShColor<3, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<3, Binding, T, Swizzled>&
ShColor<3, Binding, T, Swizzled>::operator%=(T right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<3, Binding, T, Swizzled>&
ShColor<3, Binding, T, Swizzled>::operator+=(const ShGeneric<1, T2>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<3, Binding, T, Swizzled>&
ShColor<3, Binding, T, Swizzled>::operator-=(const ShGeneric<1, T2>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<3, Binding, T, Swizzled>&
ShColor<3, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T2>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<3, Binding, T, Swizzled>&
ShColor<3, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T2>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<3, Binding, T, Swizzled>&
ShColor<3, Binding, T, Swizzled>::operator%=(const ShGeneric<1, T2>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<1, Binding, T, true>
ShColor<3, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShColor<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<2, Binding, T, true>
ShColor<3, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShColor<2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, s0, s1), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<3, Binding, T, true>
ShColor<3, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShColor<3, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, s0, s1, s2), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<4, Binding, T, true>
ShColor<3, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShColor<4, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, s0, s1, s2, s3), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShColor<N2, Binding, T, true>
ShColor<3, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShColor<N2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, N2, indices), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<1, Binding, T, true>
ShColor<3, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShColor<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<3, Binding, T, Swizzled>
ShColor<3, Binding, T, Swizzled>::operator-() const
{
  return ShColor<3, Binding, T, Swizzled>(m_node, m_swizzle, !m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<4, Binding, T, Swizzled>::ShColor()
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<4, Binding, T, Swizzled>::ShColor(const ShGeneric<4, T2>& other)
  : ParentType(other)
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<4, Binding, T, Swizzled>::ShColor(const ShColor<4, Binding, T, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<4, Binding, T, Swizzled>::ShColor(const ShColor<4, Binding, T2, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<4, Binding, T, Swizzled>::ShColor(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg)
  : ParentType(node, swizzle, neg)
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<4, Binding, T, Swizzled>::ShColor(T data[4])
  : ParentType(data)
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<4, Binding, T, Swizzled>::ShColor(T s0, T s1, T s2, T s3)
  : ParentType(s0, s1, s2, s3)
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2, typename T3, typename T4, typename T5>
ShColor<4, Binding, T, Swizzled>::ShColor(const ShGeneric<1, T2>& s0, const ShGeneric<1, T3>& s1, const ShGeneric<1, T4>& s2, const ShGeneric<1, T5>& s3)
  : ParentType(s0, s1, s2, s3)
{
  m_node->specialType(SH_COLOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<4, Binding, T, Swizzled>::~ShColor()
{
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<4, Binding, T, Swizzled>&
ShColor<4, Binding, T, Swizzled>::operator=(const ShGeneric<4, T2>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<4, Binding, T, Swizzled>&
ShColor<4, Binding, T, Swizzled>::operator=(const ShColor<4, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<4, Binding, T, Swizzled>&
ShColor<4, Binding, T, Swizzled>::operator=(const ShColor<4, Binding, T2, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<4, Binding, T, Swizzled>&
ShColor<4, Binding, T, Swizzled>::operator=(const ShProgram& prg)
{
  ParentType::operator=(prg);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<4, Binding, T, Swizzled>&
ShColor<4, Binding, T, Swizzled>::operator+=(const ShGeneric<4, T2>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<4, Binding, T, Swizzled>&
ShColor<4, Binding, T, Swizzled>::operator-=(const ShGeneric<4, T2>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<4, Binding, T, Swizzled>&
ShColor<4, Binding, T, Swizzled>::operator*=(const ShGeneric<4, T2>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<4, Binding, T, Swizzled>&
ShColor<4, Binding, T, Swizzled>::operator/=(const ShGeneric<4, T2>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<4, Binding, T, Swizzled>&
ShColor<4, Binding, T, Swizzled>::operator%=(const ShGeneric<4, T2>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<4, Binding, T, Swizzled>&
ShColor<4, Binding, T, Swizzled>::operator+=(T right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<4, Binding, T, Swizzled>&
ShColor<4, Binding, T, Swizzled>::operator-=(T right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<4, Binding, T, Swizzled>&
ShColor<4, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<4, Binding, T, Swizzled>&
ShColor<4, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<4, Binding, T, Swizzled>&
ShColor<4, Binding, T, Swizzled>::operator%=(T right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<4, Binding, T, Swizzled>&
ShColor<4, Binding, T, Swizzled>::operator+=(const ShGeneric<1, T2>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<4, Binding, T, Swizzled>&
ShColor<4, Binding, T, Swizzled>::operator-=(const ShGeneric<1, T2>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<4, Binding, T, Swizzled>&
ShColor<4, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T2>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<4, Binding, T, Swizzled>&
ShColor<4, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T2>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<typename T2>
ShColor<4, Binding, T, Swizzled>&
ShColor<4, Binding, T, Swizzled>::operator%=(const ShGeneric<1, T2>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<1, Binding, T, true>
ShColor<4, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShColor<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<2, Binding, T, true>
ShColor<4, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShColor<2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, s0, s1), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<3, Binding, T, true>
ShColor<4, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShColor<3, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, s0, s1, s2), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<4, Binding, T, true>
ShColor<4, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShColor<4, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, s0, s1, s2, s3), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShColor<N2, Binding, T, true>
ShColor<4, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShColor<N2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, N2, indices), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<1, Binding, T, true>
ShColor<4, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShColor<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShColor<4, Binding, T, Swizzled>
ShColor<4, Binding, T, Swizzled>::operator-() const
{
  return ShColor<4, Binding, T, Swizzled>(m_node, m_swizzle, !m_neg);
}


} // namespace SH

#endif // SH_SHCOLORIMPL_HPP
