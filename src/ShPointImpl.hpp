// WARNING - DO NOT EDIT THIS FILE:
// This file was automatically generated from scripts/ShPointImpl.hpp.py.
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

#ifndef SH_SHPOINTIMPL_HPP
#define SH_SHPOINTIMPL_HPP

#include "ShPoint.hpp"

namespace SH {

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<N, Binding, T, Swizzled>::ShPoint()
{
  this->m_node->specialType(SH_POINT);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<N, Binding, T, Swizzled>::ShPoint(const ShGeneric<N, T>& other)
  : ParentType(other)
{
  this->m_node->specialType(SH_POINT);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<N, Binding, T, Swizzled>::ShPoint(const ShPoint<N, Binding, T, Swizzled>& other)
  : ParentType(other)
{
  this->m_node->specialType(SH_POINT);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<N, Binding, T, Swizzled>::ShPoint(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg)
  : ParentType(node, swizzle, neg)
{
  this->m_node->specialType(SH_POINT);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<N, Binding, T, Swizzled>::ShPoint(T data[N])
  : ParentType(data)
{
  this->m_node->specialType(SH_POINT);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<N, Binding, T, Swizzled>::~ShPoint()
{
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<N, Binding, T, Swizzled>&
ShPoint<N, Binding, T, Swizzled>::operator=(const ShGeneric<N, T>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<N, Binding, T, Swizzled>&
ShPoint<N, Binding, T, Swizzled>::operator=(const ShPoint<N, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<N, Binding, T, Swizzled>&
ShPoint<N, Binding, T, Swizzled>::operator=(const ShProgram& prg)
{
  ParentType::operator=(prg);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<N, Binding, T, Swizzled>&
ShPoint<N, Binding, T, Swizzled>::operator+=(const ShGeneric<N, T>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<N, Binding, T, Swizzled>&
ShPoint<N, Binding, T, Swizzled>::operator-=(const ShGeneric<N, T>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<N, Binding, T, Swizzled>&
ShPoint<N, Binding, T, Swizzled>::operator*=(const ShGeneric<N, T>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<N, Binding, T, Swizzled>&
ShPoint<N, Binding, T, Swizzled>::operator/=(const ShGeneric<N, T>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<N, Binding, T, Swizzled>&
ShPoint<N, Binding, T, Swizzled>::operator%=(const ShGeneric<N, T>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<N, Binding, T, Swizzled>&
ShPoint<N, Binding, T, Swizzled>::operator+=(T right)
{
  ParentType::operator+=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<N, Binding, T, Swizzled>&
ShPoint<N, Binding, T, Swizzled>::operator-=(T right)
{
  ParentType::operator-=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<N, Binding, T, Swizzled>&
ShPoint<N, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<N, Binding, T, Swizzled>&
ShPoint<N, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<N, Binding, T, Swizzled>&
ShPoint<N, Binding, T, Swizzled>::operator%=(T right)
{
  ParentType::operator%=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<N, Binding, T, Swizzled>&
ShPoint<N, Binding, T, Swizzled>::operator+=(const ShGeneric<1, T>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<N, Binding, T, Swizzled>&
ShPoint<N, Binding, T, Swizzled>::operator-=(const ShGeneric<1, T>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<N, Binding, T, Swizzled>&
ShPoint<N, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<N, Binding, T, Swizzled>&
ShPoint<N, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<N, Binding, T, Swizzled>&
ShPoint<N, Binding, T, Swizzled>::operator%=(const ShGeneric<1, T>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, true>
ShPoint<N, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShPoint<1, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(N, s0), this->m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, true>
ShPoint<N, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShPoint<2, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(N, s0, s1), this->m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, true>
ShPoint<N, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShPoint<3, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(N, s0, s1, s2), this->m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, true>
ShPoint<N, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShPoint<4, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(N, s0, s1, s2, s3), this->m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShPoint<N2, Binding, T, true>
ShPoint<N, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShPoint<N2, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(N, N2, indices), this->m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, true>
ShPoint<N, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShPoint<1, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(N, s0), this->m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShPoint<N, Binding, T, Swizzled>
ShPoint<N, Binding, T, Swizzled>::operator-() const
{
  return ShPoint<N, Binding, T, Swizzled>(this->m_node, this->m_swizzle, !this->m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, Swizzled>::ShPoint()
{
  this->m_node->specialType(SH_POINT);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, Swizzled>::ShPoint(const ShGeneric<1, T>& other)
  : ParentType(other)
{
  this->m_node->specialType(SH_POINT);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, Swizzled>::ShPoint(const ShPoint<1, Binding, T, Swizzled>& other)
  : ParentType(other)
{
  this->m_node->specialType(SH_POINT);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, Swizzled>::ShPoint(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg)
  : ParentType(node, swizzle, neg)
{
  this->m_node->specialType(SH_POINT);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, Swizzled>::ShPoint(T data[1])
  : ParentType(data)
{
  this->m_node->specialType(SH_POINT);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, Swizzled>::ShPoint(T s0)
  : ParentType(s0)
{
  this->m_node->specialType(SH_POINT);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, Swizzled>::~ShPoint()
{
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, Swizzled>&
ShPoint<1, Binding, T, Swizzled>::operator=(const ShGeneric<1, T>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, Swizzled>&
ShPoint<1, Binding, T, Swizzled>::operator=(const ShPoint<1, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, Swizzled>&
ShPoint<1, Binding, T, Swizzled>::operator=(T other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, Swizzled>&
ShPoint<1, Binding, T, Swizzled>::operator=(const ShProgram& prg)
{
  ParentType::operator=(prg);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, Swizzled>&
ShPoint<1, Binding, T, Swizzled>::operator+=(const ShGeneric<1, T>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, Swizzled>&
ShPoint<1, Binding, T, Swizzled>::operator-=(const ShGeneric<1, T>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, Swizzled>&
ShPoint<1, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, Swizzled>&
ShPoint<1, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, Swizzled>&
ShPoint<1, Binding, T, Swizzled>::operator%=(const ShGeneric<1, T>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, Swizzled>&
ShPoint<1, Binding, T, Swizzled>::operator+=(T right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, Swizzled>&
ShPoint<1, Binding, T, Swizzled>::operator-=(T right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, Swizzled>&
ShPoint<1, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, Swizzled>&
ShPoint<1, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, Swizzled>&
ShPoint<1, Binding, T, Swizzled>::operator%=(T right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, true>
ShPoint<1, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShPoint<1, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(1, s0), this->m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, true>
ShPoint<1, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShPoint<2, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(1, s0, s1), this->m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, true>
ShPoint<1, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShPoint<3, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(1, s0, s1, s2), this->m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, true>
ShPoint<1, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShPoint<4, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(1, s0, s1, s2, s3), this->m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShPoint<N2, Binding, T, true>
ShPoint<1, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShPoint<N2, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(1, N2, indices), this->m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, true>
ShPoint<1, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShPoint<1, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(1, s0), this->m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPoint<1, Binding, T, Swizzled>
ShPoint<1, Binding, T, Swizzled>::operator-() const
{
  return ShPoint<1, Binding, T, Swizzled>(this->m_node, this->m_swizzle, !this->m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, Swizzled>::ShPoint()
{
  this->m_node->specialType(SH_POINT);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, Swizzled>::ShPoint(const ShGeneric<2, T>& other)
  : ParentType(other)
{
  this->m_node->specialType(SH_POINT);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, Swizzled>::ShPoint(const ShPoint<2, Binding, T, Swizzled>& other)
  : ParentType(other)
{
  this->m_node->specialType(SH_POINT);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, Swizzled>::ShPoint(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg)
  : ParentType(node, swizzle, neg)
{
  this->m_node->specialType(SH_POINT);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, Swizzled>::ShPoint(T data[2])
  : ParentType(data)
{
  this->m_node->specialType(SH_POINT);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, Swizzled>::ShPoint(T s0, T s1)
  : ParentType(s0, s1)
{
  this->m_node->specialType(SH_POINT);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, Swizzled>::ShPoint(const ShGeneric<1, T>& s0, const ShGeneric<1, T>& s1)
  : ParentType(s0, s1)
{
  this->m_node->specialType(SH_POINT);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, Swizzled>::~ShPoint()
{
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, Swizzled>&
ShPoint<2, Binding, T, Swizzled>::operator=(const ShGeneric<2, T>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, Swizzled>&
ShPoint<2, Binding, T, Swizzled>::operator=(const ShPoint<2, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, Swizzled>&
ShPoint<2, Binding, T, Swizzled>::operator=(const ShProgram& prg)
{
  ParentType::operator=(prg);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, Swizzled>&
ShPoint<2, Binding, T, Swizzled>::operator+=(const ShGeneric<2, T>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, Swizzled>&
ShPoint<2, Binding, T, Swizzled>::operator-=(const ShGeneric<2, T>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, Swizzled>&
ShPoint<2, Binding, T, Swizzled>::operator*=(const ShGeneric<2, T>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, Swizzled>&
ShPoint<2, Binding, T, Swizzled>::operator/=(const ShGeneric<2, T>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, Swizzled>&
ShPoint<2, Binding, T, Swizzled>::operator%=(const ShGeneric<2, T>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, Swizzled>&
ShPoint<2, Binding, T, Swizzled>::operator+=(T right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, Swizzled>&
ShPoint<2, Binding, T, Swizzled>::operator-=(T right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, Swizzled>&
ShPoint<2, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, Swizzled>&
ShPoint<2, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, Swizzled>&
ShPoint<2, Binding, T, Swizzled>::operator%=(T right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, Swizzled>&
ShPoint<2, Binding, T, Swizzled>::operator+=(const ShGeneric<1, T>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, Swizzled>&
ShPoint<2, Binding, T, Swizzled>::operator-=(const ShGeneric<1, T>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, Swizzled>&
ShPoint<2, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, Swizzled>&
ShPoint<2, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, Swizzled>&
ShPoint<2, Binding, T, Swizzled>::operator%=(const ShGeneric<1, T>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, true>
ShPoint<2, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShPoint<1, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(2, s0), this->m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, true>
ShPoint<2, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShPoint<2, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(2, s0, s1), this->m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, true>
ShPoint<2, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShPoint<3, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(2, s0, s1, s2), this->m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, true>
ShPoint<2, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShPoint<4, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(2, s0, s1, s2, s3), this->m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShPoint<N2, Binding, T, true>
ShPoint<2, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShPoint<N2, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(2, N2, indices), this->m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, true>
ShPoint<2, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShPoint<1, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(2, s0), this->m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPoint<2, Binding, T, Swizzled>
ShPoint<2, Binding, T, Swizzled>::operator-() const
{
  return ShPoint<2, Binding, T, Swizzled>(this->m_node, this->m_swizzle, !this->m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, Swizzled>::ShPoint()
{
  this->m_node->specialType(SH_POINT);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, Swizzled>::ShPoint(const ShGeneric<3, T>& other)
  : ParentType(other)
{
  this->m_node->specialType(SH_POINT);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, Swizzled>::ShPoint(const ShPoint<3, Binding, T, Swizzled>& other)
  : ParentType(other)
{
  this->m_node->specialType(SH_POINT);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, Swizzled>::ShPoint(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg)
  : ParentType(node, swizzle, neg)
{
  this->m_node->specialType(SH_POINT);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, Swizzled>::ShPoint(T data[3])
  : ParentType(data)
{
  this->m_node->specialType(SH_POINT);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, Swizzled>::ShPoint(T s0, T s1, T s2)
  : ParentType(s0, s1, s2)
{
  this->m_node->specialType(SH_POINT);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, Swizzled>::ShPoint(const ShGeneric<1, T>& s0, const ShGeneric<1, T>& s1, const ShGeneric<1, T>& s2)
  : ParentType(s0, s1, s2)
{
  this->m_node->specialType(SH_POINT);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, Swizzled>::~ShPoint()
{
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, Swizzled>&
ShPoint<3, Binding, T, Swizzled>::operator=(const ShGeneric<3, T>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, Swizzled>&
ShPoint<3, Binding, T, Swizzled>::operator=(const ShPoint<3, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, Swizzled>&
ShPoint<3, Binding, T, Swizzled>::operator=(const ShProgram& prg)
{
  ParentType::operator=(prg);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, Swizzled>&
ShPoint<3, Binding, T, Swizzled>::operator+=(const ShGeneric<3, T>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, Swizzled>&
ShPoint<3, Binding, T, Swizzled>::operator-=(const ShGeneric<3, T>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, Swizzled>&
ShPoint<3, Binding, T, Swizzled>::operator*=(const ShGeneric<3, T>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, Swizzled>&
ShPoint<3, Binding, T, Swizzled>::operator/=(const ShGeneric<3, T>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, Swizzled>&
ShPoint<3, Binding, T, Swizzled>::operator%=(const ShGeneric<3, T>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, Swizzled>&
ShPoint<3, Binding, T, Swizzled>::operator+=(T right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, Swizzled>&
ShPoint<3, Binding, T, Swizzled>::operator-=(T right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, Swizzled>&
ShPoint<3, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, Swizzled>&
ShPoint<3, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, Swizzled>&
ShPoint<3, Binding, T, Swizzled>::operator%=(T right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, Swizzled>&
ShPoint<3, Binding, T, Swizzled>::operator+=(const ShGeneric<1, T>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, Swizzled>&
ShPoint<3, Binding, T, Swizzled>::operator-=(const ShGeneric<1, T>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, Swizzled>&
ShPoint<3, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, Swizzled>&
ShPoint<3, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, Swizzled>&
ShPoint<3, Binding, T, Swizzled>::operator%=(const ShGeneric<1, T>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, true>
ShPoint<3, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShPoint<1, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(3, s0), this->m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, true>
ShPoint<3, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShPoint<2, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(3, s0, s1), this->m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, true>
ShPoint<3, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShPoint<3, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(3, s0, s1, s2), this->m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, true>
ShPoint<3, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShPoint<4, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(3, s0, s1, s2, s3), this->m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShPoint<N2, Binding, T, true>
ShPoint<3, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShPoint<N2, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(3, N2, indices), this->m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, true>
ShPoint<3, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShPoint<1, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(3, s0), this->m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPoint<3, Binding, T, Swizzled>
ShPoint<3, Binding, T, Swizzled>::operator-() const
{
  return ShPoint<3, Binding, T, Swizzled>(this->m_node, this->m_swizzle, !this->m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, Swizzled>::ShPoint()
{
  this->m_node->specialType(SH_POINT);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, Swizzled>::ShPoint(const ShGeneric<4, T>& other)
  : ParentType(other)
{
  this->m_node->specialType(SH_POINT);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, Swizzled>::ShPoint(const ShPoint<4, Binding, T, Swizzled>& other)
  : ParentType(other)
{
  this->m_node->specialType(SH_POINT);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, Swizzled>::ShPoint(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg)
  : ParentType(node, swizzle, neg)
{
  this->m_node->specialType(SH_POINT);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, Swizzled>::ShPoint(T data[4])
  : ParentType(data)
{
  this->m_node->specialType(SH_POINT);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, Swizzled>::ShPoint(T s0, T s1, T s2, T s3)
  : ParentType(s0, s1, s2, s3)
{
  this->m_node->specialType(SH_POINT);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, Swizzled>::ShPoint(const ShGeneric<1, T>& s0, const ShGeneric<1, T>& s1, const ShGeneric<1, T>& s2, const ShGeneric<1, T>& s3)
  : ParentType(s0, s1, s2, s3)
{
  this->m_node->specialType(SH_POINT);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, Swizzled>::~ShPoint()
{
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, Swizzled>&
ShPoint<4, Binding, T, Swizzled>::operator=(const ShGeneric<4, T>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, Swizzled>&
ShPoint<4, Binding, T, Swizzled>::operator=(const ShPoint<4, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, Swizzled>&
ShPoint<4, Binding, T, Swizzled>::operator=(const ShProgram& prg)
{
  ParentType::operator=(prg);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, Swizzled>&
ShPoint<4, Binding, T, Swizzled>::operator+=(const ShGeneric<4, T>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, Swizzled>&
ShPoint<4, Binding, T, Swizzled>::operator-=(const ShGeneric<4, T>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, Swizzled>&
ShPoint<4, Binding, T, Swizzled>::operator*=(const ShGeneric<4, T>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, Swizzled>&
ShPoint<4, Binding, T, Swizzled>::operator/=(const ShGeneric<4, T>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, Swizzled>&
ShPoint<4, Binding, T, Swizzled>::operator%=(const ShGeneric<4, T>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, Swizzled>&
ShPoint<4, Binding, T, Swizzled>::operator+=(T right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, Swizzled>&
ShPoint<4, Binding, T, Swizzled>::operator-=(T right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, Swizzled>&
ShPoint<4, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, Swizzled>&
ShPoint<4, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, Swizzled>&
ShPoint<4, Binding, T, Swizzled>::operator%=(T right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, Swizzled>&
ShPoint<4, Binding, T, Swizzled>::operator+=(const ShGeneric<1, T>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, Swizzled>&
ShPoint<4, Binding, T, Swizzled>::operator-=(const ShGeneric<1, T>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, Swizzled>&
ShPoint<4, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, Swizzled>&
ShPoint<4, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, Swizzled>&
ShPoint<4, Binding, T, Swizzled>::operator%=(const ShGeneric<1, T>& right)
{
  ParentType::operator%=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, true>
ShPoint<4, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShPoint<1, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(4, s0), this->m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<2, Binding, T, true>
ShPoint<4, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShPoint<2, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(4, s0, s1), this->m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<3, Binding, T, true>
ShPoint<4, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShPoint<3, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(4, s0, s1, s2), this->m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<4, Binding, T, true>
ShPoint<4, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShPoint<4, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(4, s0, s1, s2, s3), this->m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShPoint<N2, Binding, T, true>
ShPoint<4, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShPoint<N2, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(4, N2, indices), this->m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
inline
ShPoint<1, Binding, T, true>
ShPoint<4, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShPoint<1, Binding, T, true>(this->m_node, this->m_swizzle * ShSwizzle(4, s0), this->m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShPoint<4, Binding, T, Swizzled>
ShPoint<4, Binding, T, Swizzled>::operator-() const
{
  return ShPoint<4, Binding, T, Swizzled>(this->m_node, this->m_swizzle, !this->m_neg);
}


} // namespace SH

#endif // SH_SHPOINTIMPL_HPP
