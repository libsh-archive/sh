// WARNING - DO NOT EDIT THIS FILE:
// This file was automatically generated from scripts/ShVectorImpl.hpp.py.
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

#ifndef SH_SHVECTORIMPL_HPP
#define SH_SHVECTORIMPL_HPP

#include "ShVector.hpp"

namespace SH {

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<N, Binding, T, Swizzled>::ShVector()
{
  m_node->specialType(SH_VECTOR);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<N, Binding, T, Swizzled>::ShVector(const ShGeneric<N, T>& other)
  : ParentType(other)
{
  m_node->specialType(SH_VECTOR);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<N, Binding, T, Swizzled>::ShVector(const ShVector<N, Binding, T, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_VECTOR);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<N, Binding, T, Swizzled>::ShVector(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg)
  : ParentType(node, swizzle, neg)
{
  m_node->specialType(SH_VECTOR);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<N, Binding, T, Swizzled>::ShVector(T data[N])
  : ParentType(data)
{
  m_node->specialType(SH_VECTOR);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<N, Binding, T, Swizzled>::~ShVector()
{
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<N, Binding, T, Swizzled>&
ShVector<N, Binding, T, Swizzled>::operator=(const ShGeneric<N, T>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<N, Binding, T, Swizzled>&
ShVector<N, Binding, T, Swizzled>::operator=(const ShVector<N, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<N, Binding, T, Swizzled>&
ShVector<N, Binding, T, Swizzled>::operator=(const ShProgram& prg)
{
  ParentType::operator=(prg);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<N, Binding, T, Swizzled>&
ShVector<N, Binding, T, Swizzled>::operator+=(const ShGeneric<N, T>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<N, Binding, T, Swizzled>&
ShVector<N, Binding, T, Swizzled>::operator-=(const ShGeneric<N, T>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<N, Binding, T, Swizzled>&
ShVector<N, Binding, T, Swizzled>::operator*=(const ShGeneric<N, T>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<N, Binding, T, Swizzled>&
ShVector<N, Binding, T, Swizzled>::operator/=(const ShGeneric<N, T>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<N, Binding, T, Swizzled>&
ShVector<N, Binding, T, Swizzled>::operator%=(const ShGeneric<N, T>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<N, Binding, T, Swizzled>&
ShVector<N, Binding, T, Swizzled>::operator+=(T right)
{
  ParentType::operator+=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<N, Binding, T, Swizzled>&
ShVector<N, Binding, T, Swizzled>::operator-=(T right)
{
  ParentType::operator-=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<N, Binding, T, Swizzled>&
ShVector<N, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<N, Binding, T, Swizzled>&
ShVector<N, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<N, Binding, T, Swizzled>&
ShVector<N, Binding, T, Swizzled>::operator%=(T right)
{
  ParentType::operator%=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<N, Binding, T, Swizzled>&
ShVector<N, Binding, T, Swizzled>::operator+=(const ShGeneric<1, T>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<N, Binding, T, Swizzled>&
ShVector<N, Binding, T, Swizzled>::operator-=(const ShGeneric<1, T>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<N, Binding, T, Swizzled>&
ShVector<N, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<N, Binding, T, Swizzled>&
ShVector<N, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<N, Binding, T, Swizzled>&
ShVector<N, Binding, T, Swizzled>::operator%=(const ShGeneric<1, T>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, true>
ShVector<N, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShVector<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, s0), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, true>
ShVector<N, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShVector<2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, s0, s1), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, true>
ShVector<N, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShVector<3, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, s0, s1, s2), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, true>
ShVector<N, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShVector<4, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, s0, s1, s2, s3), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShVector<N2, Binding, T, true>
ShVector<N, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShVector<N2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, N2, indices), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, true>
ShVector<N, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShVector<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, s0), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShVector<N, Binding, T, Swizzled>
ShVector<N, Binding, T, Swizzled>::operator-() const
{
  return ShVector<N, Binding, T, Swizzled>(m_node, m_swizzle, !m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, Swizzled>::ShVector()
{
  m_node->specialType(SH_VECTOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, Swizzled>::ShVector(const ShGeneric<1, T>& other)
  : ParentType(other)
{
  m_node->specialType(SH_VECTOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, Swizzled>::ShVector(const ShVector<1, Binding, T, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_VECTOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, Swizzled>::ShVector(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg)
  : ParentType(node, swizzle, neg)
{
  m_node->specialType(SH_VECTOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, Swizzled>::ShVector(T data[1])
  : ParentType(data)
{
  m_node->specialType(SH_VECTOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, Swizzled>::ShVector(T s0)
  : ParentType(s0)
{
  m_node->specialType(SH_VECTOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, Swizzled>::~ShVector()
{
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, Swizzled>&
ShVector<1, Binding, T, Swizzled>::operator=(const ShGeneric<1, T>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, Swizzled>&
ShVector<1, Binding, T, Swizzled>::operator=(const ShVector<1, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, Swizzled>&
ShVector<1, Binding, T, Swizzled>::operator=(T other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, Swizzled>&
ShVector<1, Binding, T, Swizzled>::operator=(const ShProgram& prg)
{
  ParentType::operator=(prg);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, Swizzled>&
ShVector<1, Binding, T, Swizzled>::operator+=(const ShGeneric<1, T>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, Swizzled>&
ShVector<1, Binding, T, Swizzled>::operator-=(const ShGeneric<1, T>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, Swizzled>&
ShVector<1, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, Swizzled>&
ShVector<1, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, Swizzled>&
ShVector<1, Binding, T, Swizzled>::operator%=(const ShGeneric<1, T>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, Swizzled>&
ShVector<1, Binding, T, Swizzled>::operator+=(T right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, Swizzled>&
ShVector<1, Binding, T, Swizzled>::operator-=(T right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, Swizzled>&
ShVector<1, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, Swizzled>&
ShVector<1, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, Swizzled>&
ShVector<1, Binding, T, Swizzled>::operator%=(T right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, true>
ShVector<1, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShVector<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, true>
ShVector<1, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShVector<2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, s0, s1), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, true>
ShVector<1, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShVector<3, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, s0, s1, s2), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, true>
ShVector<1, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShVector<4, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, s0, s1, s2, s3), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShVector<N2, Binding, T, true>
ShVector<1, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShVector<N2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, N2, indices), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, true>
ShVector<1, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShVector<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, Swizzled>
ShVector<1, Binding, T, Swizzled>::operator-() const
{
  return ShVector<1, Binding, T, Swizzled>(m_node, m_swizzle, !m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, Swizzled>::ShVector()
{
  m_node->specialType(SH_VECTOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, Swizzled>::ShVector(const ShGeneric<2, T>& other)
  : ParentType(other)
{
  m_node->specialType(SH_VECTOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, Swizzled>::ShVector(const ShVector<2, Binding, T, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_VECTOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, Swizzled>::ShVector(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg)
  : ParentType(node, swizzle, neg)
{
  m_node->specialType(SH_VECTOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, Swizzled>::ShVector(T data[2])
  : ParentType(data)
{
  m_node->specialType(SH_VECTOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, Swizzled>::ShVector(T s0, T s1)
  : ParentType(s0, s1)
{
  m_node->specialType(SH_VECTOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, Swizzled>::ShVector(const ShGeneric<1, T>& s0, const ShGeneric<1, T>& s1)
  : ParentType(s0, s1)
{
  m_node->specialType(SH_VECTOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, Swizzled>::~ShVector()
{
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, Swizzled>&
ShVector<2, Binding, T, Swizzled>::operator=(const ShGeneric<2, T>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, Swizzled>&
ShVector<2, Binding, T, Swizzled>::operator=(const ShVector<2, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, Swizzled>&
ShVector<2, Binding, T, Swizzled>::operator=(const ShProgram& prg)
{
  ParentType::operator=(prg);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, Swizzled>&
ShVector<2, Binding, T, Swizzled>::operator+=(const ShGeneric<2, T>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, Swizzled>&
ShVector<2, Binding, T, Swizzled>::operator-=(const ShGeneric<2, T>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, Swizzled>&
ShVector<2, Binding, T, Swizzled>::operator*=(const ShGeneric<2, T>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, Swizzled>&
ShVector<2, Binding, T, Swizzled>::operator/=(const ShGeneric<2, T>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, Swizzled>&
ShVector<2, Binding, T, Swizzled>::operator%=(const ShGeneric<2, T>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, Swizzled>&
ShVector<2, Binding, T, Swizzled>::operator+=(T right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, Swizzled>&
ShVector<2, Binding, T, Swizzled>::operator-=(T right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, Swizzled>&
ShVector<2, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, Swizzled>&
ShVector<2, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, Swizzled>&
ShVector<2, Binding, T, Swizzled>::operator%=(T right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, Swizzled>&
ShVector<2, Binding, T, Swizzled>::operator+=(const ShGeneric<1, T>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, Swizzled>&
ShVector<2, Binding, T, Swizzled>::operator-=(const ShGeneric<1, T>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, Swizzled>&
ShVector<2, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, Swizzled>&
ShVector<2, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, Swizzled>&
ShVector<2, Binding, T, Swizzled>::operator%=(const ShGeneric<1, T>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, true>
ShVector<2, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShVector<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, true>
ShVector<2, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShVector<2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, s0, s1), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, true>
ShVector<2, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShVector<3, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, s0, s1, s2), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, true>
ShVector<2, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShVector<4, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, s0, s1, s2, s3), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShVector<N2, Binding, T, true>
ShVector<2, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShVector<N2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, N2, indices), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, true>
ShVector<2, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShVector<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, Swizzled>
ShVector<2, Binding, T, Swizzled>::operator-() const
{
  return ShVector<2, Binding, T, Swizzled>(m_node, m_swizzle, !m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, Swizzled>::ShVector()
{
  m_node->specialType(SH_VECTOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, Swizzled>::ShVector(const ShGeneric<3, T>& other)
  : ParentType(other)
{
  m_node->specialType(SH_VECTOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, Swizzled>::ShVector(const ShVector<3, Binding, T, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_VECTOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, Swizzled>::ShVector(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg)
  : ParentType(node, swizzle, neg)
{
  m_node->specialType(SH_VECTOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, Swizzled>::ShVector(T data[3])
  : ParentType(data)
{
  m_node->specialType(SH_VECTOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, Swizzled>::ShVector(T s0, T s1, T s2)
  : ParentType(s0, s1, s2)
{
  m_node->specialType(SH_VECTOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, Swizzled>::ShVector(const ShGeneric<1, T>& s0, const ShGeneric<1, T>& s1, const ShGeneric<1, T>& s2)
  : ParentType(s0, s1, s2)
{
  m_node->specialType(SH_VECTOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, Swizzled>::~ShVector()
{
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, Swizzled>&
ShVector<3, Binding, T, Swizzled>::operator=(const ShGeneric<3, T>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, Swizzled>&
ShVector<3, Binding, T, Swizzled>::operator=(const ShVector<3, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, Swizzled>&
ShVector<3, Binding, T, Swizzled>::operator=(const ShProgram& prg)
{
  ParentType::operator=(prg);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, Swizzled>&
ShVector<3, Binding, T, Swizzled>::operator+=(const ShGeneric<3, T>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, Swizzled>&
ShVector<3, Binding, T, Swizzled>::operator-=(const ShGeneric<3, T>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, Swizzled>&
ShVector<3, Binding, T, Swizzled>::operator*=(const ShGeneric<3, T>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, Swizzled>&
ShVector<3, Binding, T, Swizzled>::operator/=(const ShGeneric<3, T>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, Swizzled>&
ShVector<3, Binding, T, Swizzled>::operator%=(const ShGeneric<3, T>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, Swizzled>&
ShVector<3, Binding, T, Swizzled>::operator+=(T right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, Swizzled>&
ShVector<3, Binding, T, Swizzled>::operator-=(T right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, Swizzled>&
ShVector<3, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, Swizzled>&
ShVector<3, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, Swizzled>&
ShVector<3, Binding, T, Swizzled>::operator%=(T right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, Swizzled>&
ShVector<3, Binding, T, Swizzled>::operator+=(const ShGeneric<1, T>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, Swizzled>&
ShVector<3, Binding, T, Swizzled>::operator-=(const ShGeneric<1, T>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, Swizzled>&
ShVector<3, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, Swizzled>&
ShVector<3, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, Swizzled>&
ShVector<3, Binding, T, Swizzled>::operator%=(const ShGeneric<1, T>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, true>
ShVector<3, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShVector<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, true>
ShVector<3, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShVector<2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, s0, s1), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, true>
ShVector<3, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShVector<3, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, s0, s1, s2), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, true>
ShVector<3, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShVector<4, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, s0, s1, s2, s3), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShVector<N2, Binding, T, true>
ShVector<3, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShVector<N2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, N2, indices), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, true>
ShVector<3, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShVector<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, Swizzled>
ShVector<3, Binding, T, Swizzled>::operator-() const
{
  return ShVector<3, Binding, T, Swizzled>(m_node, m_swizzle, !m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, Swizzled>::ShVector()
{
  m_node->specialType(SH_VECTOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, Swizzled>::ShVector(const ShGeneric<4, T>& other)
  : ParentType(other)
{
  m_node->specialType(SH_VECTOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, Swizzled>::ShVector(const ShVector<4, Binding, T, Swizzled>& other)
  : ParentType(other)
{
  m_node->specialType(SH_VECTOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, Swizzled>::ShVector(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg)
  : ParentType(node, swizzle, neg)
{
  m_node->specialType(SH_VECTOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, Swizzled>::ShVector(T data[4])
  : ParentType(data)
{
  m_node->specialType(SH_VECTOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, Swizzled>::ShVector(T s0, T s1, T s2, T s3)
  : ParentType(s0, s1, s2, s3)
{
  m_node->specialType(SH_VECTOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, Swizzled>::ShVector(const ShGeneric<1, T>& s0, const ShGeneric<1, T>& s1, const ShGeneric<1, T>& s2, const ShGeneric<1, T>& s3)
  : ParentType(s0, s1, s2, s3)
{
  m_node->specialType(SH_VECTOR);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, Swizzled>::~ShVector()
{
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, Swizzled>&
ShVector<4, Binding, T, Swizzled>::operator=(const ShGeneric<4, T>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, Swizzled>&
ShVector<4, Binding, T, Swizzled>::operator=(const ShVector<4, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, Swizzled>&
ShVector<4, Binding, T, Swizzled>::operator=(const ShProgram& prg)
{
  ParentType::operator=(prg);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, Swizzled>&
ShVector<4, Binding, T, Swizzled>::operator+=(const ShGeneric<4, T>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, Swizzled>&
ShVector<4, Binding, T, Swizzled>::operator-=(const ShGeneric<4, T>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, Swizzled>&
ShVector<4, Binding, T, Swizzled>::operator*=(const ShGeneric<4, T>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, Swizzled>&
ShVector<4, Binding, T, Swizzled>::operator/=(const ShGeneric<4, T>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, Swizzled>&
ShVector<4, Binding, T, Swizzled>::operator%=(const ShGeneric<4, T>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, Swizzled>&
ShVector<4, Binding, T, Swizzled>::operator+=(T right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, Swizzled>&
ShVector<4, Binding, T, Swizzled>::operator-=(T right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, Swizzled>&
ShVector<4, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, Swizzled>&
ShVector<4, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, Swizzled>&
ShVector<4, Binding, T, Swizzled>::operator%=(T right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, Swizzled>&
ShVector<4, Binding, T, Swizzled>::operator+=(const ShGeneric<1, T>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, Swizzled>&
ShVector<4, Binding, T, Swizzled>::operator-=(const ShGeneric<1, T>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, Swizzled>&
ShVector<4, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, Swizzled>&
ShVector<4, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, Swizzled>&
ShVector<4, Binding, T, Swizzled>::operator%=(const ShGeneric<1, T>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, true>
ShVector<4, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShVector<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<2, Binding, T, true>
ShVector<4, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShVector<2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, s0, s1), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<3, Binding, T, true>
ShVector<4, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShVector<3, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, s0, s1, s2), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, true>
ShVector<4, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShVector<4, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, s0, s1, s2, s3), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShVector<N2, Binding, T, true>
ShVector<4, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShVector<N2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, N2, indices), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<1, Binding, T, true>
ShVector<4, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShVector<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShVector<4, Binding, T, Swizzled>
ShVector<4, Binding, T, Swizzled>::operator-() const
{
  return ShVector<4, Binding, T, Swizzled>(m_node, m_swizzle, !m_neg);
}


} // namespace SH

#endif // SH_SHVECTORIMPL_HPP
