// WARNING - DO NOT EDIT THIS FILE:
// This file was automatically generated from scripts/ShAttribImpl.hpp.py.
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

#ifndef SH_SHATTRIBIMPL_HPP
#define SH_SHATTRIBIMPL_HPP

#include "ShAttrib.hpp"
#include "ShStatement.hpp"
#include "ShEnvironment.hpp"
#include "ShDebug.hpp"

namespace SH {

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<N, Binding, T, Swizzled>::ShAttrib()
  : ShGeneric<N, T>(new ShVariableNode(Binding, N))
{
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<N, Binding, T, Swizzled>::ShAttrib(const ShGeneric<N, T>& other)
  : ShGeneric<N, T>(new ShVariableNode(Binding, N))
{
  if (Binding == SH_CONST || uniform()) {
    SH_DEBUG_ASSERT(Binding == SH_CONST || !ShEnvironment::insideShader);
    SH_DEBUG_ASSERT(other.hasValues());
    T data[N];
    other.getValues(data);
    setValues(data);
  } else {
    ShStatement asn(*this, SH_OP_ASN, other);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(asn);
  }
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<N, Binding, T, Swizzled>::ShAttrib(const ShAttrib<N, Binding, T, Swizzled>& other)
  : ShGeneric<N, T>(new ShVariableNode(Binding, N))
{
  if (Binding == SH_CONST || uniform()) {
    SH_DEBUG_ASSERT(Binding == SH_CONST || !ShEnvironment::insideShader);
    SH_DEBUG_ASSERT(other.hasValues());
    T data[N];
    other.getValues(data);
    setValues(data);
  } else {
    ShStatement asn(*this, SH_OP_ASN, other);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(asn);
  }
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<N, Binding, T, Swizzled>::ShAttrib(const ShVariableNodePtr& node,
  const ShSwizzle& swizzle, bool neg)
  : ShGeneric<N, T>(node)
{
  m_swizzle = swizzle;
  m_neg = neg;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<N, Binding, T, Swizzled>::ShAttrib(T data[N])
  : ShGeneric<N, T>(new ShVariableNode(Binding, N))
{
  if (Binding == SH_CONST) {
    for (int i = 0; i < N; i++) m_node->setValue(i, data[i]);
  } else {
    (*this) = ShAttrib<N, SH_CONST, T>(data);
  }
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<N, Binding, T, Swizzled>::~ShAttrib()
{
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<N, Binding, T, Swizzled>&
ShAttrib<N, Binding, T, Swizzled>::operator=(const ShGeneric<N, T>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<N, Binding, T, Swizzled>&
ShAttrib<N, Binding, T, Swizzled>::operator=(const ShAttrib<N, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<N, Binding, T, Swizzled>&
ShAttrib<N, Binding, T, Swizzled>::operator+=(const ShGeneric<N, T>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<N, Binding, T, Swizzled>&
ShAttrib<N, Binding, T, Swizzled>::operator-=(const ShGeneric<N, T>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<N, Binding, T, Swizzled>&
ShAttrib<N, Binding, T, Swizzled>::operator*=(const ShGeneric<N, T>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<N, Binding, T, Swizzled>&
ShAttrib<N, Binding, T, Swizzled>::operator/=(const ShGeneric<N, T>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<N, Binding, T, Swizzled>&
ShAttrib<N, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<N, Binding, T, Swizzled>&
ShAttrib<N, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<N, Binding, T, Swizzled>&
ShAttrib<N, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<N, Binding, T, Swizzled>&
ShAttrib<N, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<1, Binding, T, true>
ShAttrib<N, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShAttrib<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, s0), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<2, Binding, T, true>
ShAttrib<N, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShAttrib<2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, s0, s1), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<3, Binding, T, true>
ShAttrib<N, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShAttrib<3, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, s0, s1, s2), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<4, Binding, T, true>
ShAttrib<N, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShAttrib<4, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, s0, s1, s2, s3), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShAttrib<N2, Binding, T, true>
ShAttrib<N, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShAttrib<N2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, N2, indices), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<1, Binding, T, true>
ShAttrib<N, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShAttrib<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(N, s0), m_neg);
}

template<int N, ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<N, Binding, T, Swizzled>
ShAttrib<N, Binding, T, Swizzled>::operator-() const
{
  return ShAttrib<N, Binding, T, Swizzled>(m_node, m_swizzle, !m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<1, Binding, T, Swizzled>::ShAttrib()
  : ShGeneric<1, T>(new ShVariableNode(Binding, 1))
{
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<1, Binding, T, Swizzled>::ShAttrib(const ShGeneric<1, T>& other)
  : ShGeneric<1, T>(new ShVariableNode(Binding, 1))
{
  if (Binding == SH_CONST || uniform()) {
    SH_DEBUG_ASSERT(Binding == SH_CONST || !ShEnvironment::insideShader);
    SH_DEBUG_ASSERT(other.hasValues());
    T data[1];
    other.getValues(data);
    setValues(data);
  } else {
    ShStatement asn(*this, SH_OP_ASN, other);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(asn);
  }
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<1, Binding, T, Swizzled>::ShAttrib(const ShAttrib<1, Binding, T, Swizzled>& other)
  : ShGeneric<1, T>(new ShVariableNode(Binding, 1))
{
  if (Binding == SH_CONST || uniform()) {
    SH_DEBUG_ASSERT(Binding == SH_CONST || !ShEnvironment::insideShader);
    SH_DEBUG_ASSERT(other.hasValues());
    T data[1];
    other.getValues(data);
    setValues(data);
  } else {
    ShStatement asn(*this, SH_OP_ASN, other);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(asn);
  }
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<1, Binding, T, Swizzled>::ShAttrib(const ShVariableNodePtr& node,
  const ShSwizzle& swizzle, bool neg)
  : ShGeneric<1, T>(node)
{
  m_swizzle = swizzle;
  m_neg = neg;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<1, Binding, T, Swizzled>::ShAttrib(T data[1])
  : ShGeneric<1, T>(new ShVariableNode(Binding, 1))
{
  if (Binding == SH_CONST) {
    for (int i = 0; i < 1; i++) m_node->setValue(i, data[i]);
  } else {
    (*this) = ShAttrib<1, SH_CONST, T>(data);
  }
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<1, Binding, T, Swizzled>::ShAttrib(T s0)
  : ShGeneric<1, T>(new ShVariableNode(Binding, 1))
{
  if (Binding == SH_CONST) {
    m_node->setValue(0, s0);
  } else {
    (*this)[0] = ShAttrib<1, SH_CONST, T>(s0);
  }
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<1, Binding, T, Swizzled>::~ShAttrib()
{
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<1, Binding, T, Swizzled>&
ShAttrib<1, Binding, T, Swizzled>::operator=(const ShGeneric<1, T>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<1, Binding, T, Swizzled>&
ShAttrib<1, Binding, T, Swizzled>::operator=(const ShAttrib<1, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<1, Binding, T, Swizzled>&
ShAttrib<1, Binding, T, Swizzled>::operator+=(const ShGeneric<1, T>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<1, Binding, T, Swizzled>&
ShAttrib<1, Binding, T, Swizzled>::operator-=(const ShGeneric<1, T>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<1, Binding, T, Swizzled>&
ShAttrib<1, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<1, Binding, T, Swizzled>&
ShAttrib<1, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<1, Binding, T, Swizzled>&
ShAttrib<1, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<1, Binding, T, Swizzled>&
ShAttrib<1, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<1, Binding, T, Swizzled>&
ShAttrib<1, Binding, T, Swizzled>::operator+=(T right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<1, Binding, T, Swizzled>&
ShAttrib<1, Binding, T, Swizzled>::operator-=(T right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<1, Binding, T, true>
ShAttrib<1, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShAttrib<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<2, Binding, T, true>
ShAttrib<1, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShAttrib<2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, s0, s1), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<3, Binding, T, true>
ShAttrib<1, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShAttrib<3, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, s0, s1, s2), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<4, Binding, T, true>
ShAttrib<1, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShAttrib<4, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, s0, s1, s2, s3), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShAttrib<N2, Binding, T, true>
ShAttrib<1, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShAttrib<N2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, N2, indices), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<1, Binding, T, true>
ShAttrib<1, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShAttrib<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(1, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<1, Binding, T, Swizzled>
ShAttrib<1, Binding, T, Swizzled>::operator-() const
{
  return ShAttrib<1, Binding, T, Swizzled>(m_node, m_swizzle, !m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<2, Binding, T, Swizzled>::ShAttrib()
  : ShGeneric<2, T>(new ShVariableNode(Binding, 2))
{
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<2, Binding, T, Swizzled>::ShAttrib(const ShGeneric<2, T>& other)
  : ShGeneric<2, T>(new ShVariableNode(Binding, 2))
{
  if (Binding == SH_CONST || uniform()) {
    SH_DEBUG_ASSERT(Binding == SH_CONST || !ShEnvironment::insideShader);
    SH_DEBUG_ASSERT(other.hasValues());
    T data[2];
    other.getValues(data);
    setValues(data);
  } else {
    ShStatement asn(*this, SH_OP_ASN, other);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(asn);
  }
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<2, Binding, T, Swizzled>::ShAttrib(const ShAttrib<2, Binding, T, Swizzled>& other)
  : ShGeneric<2, T>(new ShVariableNode(Binding, 2))
{
  if (Binding == SH_CONST || uniform()) {
    SH_DEBUG_ASSERT(Binding == SH_CONST || !ShEnvironment::insideShader);
    SH_DEBUG_ASSERT(other.hasValues());
    T data[2];
    other.getValues(data);
    setValues(data);
  } else {
    ShStatement asn(*this, SH_OP_ASN, other);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(asn);
  }
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<2, Binding, T, Swizzled>::ShAttrib(const ShVariableNodePtr& node,
  const ShSwizzle& swizzle, bool neg)
  : ShGeneric<2, T>(node)
{
  m_swizzle = swizzle;
  m_neg = neg;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<2, Binding, T, Swizzled>::ShAttrib(T data[2])
  : ShGeneric<2, T>(new ShVariableNode(Binding, 2))
{
  if (Binding == SH_CONST) {
    for (int i = 0; i < 2; i++) m_node->setValue(i, data[i]);
  } else {
    (*this) = ShAttrib<2, SH_CONST, T>(data);
  }
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<2, Binding, T, Swizzled>::ShAttrib(T s0, T s1)
  : ShGeneric<2, T>(new ShVariableNode(Binding, 2))
{
  if (Binding == SH_CONST) {
    m_node->setValue(0, s0);
    m_node->setValue(1, s1);
  } else {
    (*this)[0] = ShAttrib<1, SH_CONST, T>(s0);
    (*this)[1] = ShAttrib<1, SH_CONST, T>(s1);
  }
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<2, Binding, T, Swizzled>::ShAttrib(const ShGeneric<1, T>& s0, const ShGeneric<1, T>& s1)
  : ShGeneric<2, T>(new ShVariableNode(Binding, 2))
{
  if (Binding == SH_CONST) {
    SH_DEBUG_ASSERT(s0.hasValues());
    m_node->setValue(0, s0.getValue(0));
    SH_DEBUG_ASSERT(s1.hasValues());
    m_node->setValue(1, s1.getValue(0));
  } else {
    (*this)[0] = s0;
    (*this)[1] = s1;
  }
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<2, Binding, T, Swizzled>::~ShAttrib()
{
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<2, Binding, T, Swizzled>&
ShAttrib<2, Binding, T, Swizzled>::operator=(const ShGeneric<2, T>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<2, Binding, T, Swizzled>&
ShAttrib<2, Binding, T, Swizzled>::operator=(const ShAttrib<2, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<2, Binding, T, Swizzled>&
ShAttrib<2, Binding, T, Swizzled>::operator+=(const ShGeneric<2, T>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<2, Binding, T, Swizzled>&
ShAttrib<2, Binding, T, Swizzled>::operator-=(const ShGeneric<2, T>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<2, Binding, T, Swizzled>&
ShAttrib<2, Binding, T, Swizzled>::operator*=(const ShGeneric<2, T>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<2, Binding, T, Swizzled>&
ShAttrib<2, Binding, T, Swizzled>::operator/=(const ShGeneric<2, T>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<2, Binding, T, Swizzled>&
ShAttrib<2, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<2, Binding, T, Swizzled>&
ShAttrib<2, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<2, Binding, T, Swizzled>&
ShAttrib<2, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<2, Binding, T, Swizzled>&
ShAttrib<2, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<1, Binding, T, true>
ShAttrib<2, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShAttrib<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<2, Binding, T, true>
ShAttrib<2, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShAttrib<2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, s0, s1), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<3, Binding, T, true>
ShAttrib<2, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShAttrib<3, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, s0, s1, s2), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<4, Binding, T, true>
ShAttrib<2, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShAttrib<4, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, s0, s1, s2, s3), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShAttrib<N2, Binding, T, true>
ShAttrib<2, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShAttrib<N2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, N2, indices), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<1, Binding, T, true>
ShAttrib<2, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShAttrib<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(2, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<2, Binding, T, Swizzled>
ShAttrib<2, Binding, T, Swizzled>::operator-() const
{
  return ShAttrib<2, Binding, T, Swizzled>(m_node, m_swizzle, !m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<3, Binding, T, Swizzled>::ShAttrib()
  : ShGeneric<3, T>(new ShVariableNode(Binding, 3))
{
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<3, Binding, T, Swizzled>::ShAttrib(const ShGeneric<3, T>& other)
  : ShGeneric<3, T>(new ShVariableNode(Binding, 3))
{
  if (Binding == SH_CONST || uniform()) {
    SH_DEBUG_ASSERT(Binding == SH_CONST || !ShEnvironment::insideShader);
    SH_DEBUG_ASSERT(other.hasValues());
    T data[3];
    other.getValues(data);
    setValues(data);
  } else {
    ShStatement asn(*this, SH_OP_ASN, other);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(asn);
  }
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<3, Binding, T, Swizzled>::ShAttrib(const ShAttrib<3, Binding, T, Swizzled>& other)
  : ShGeneric<3, T>(new ShVariableNode(Binding, 3))
{
  if (Binding == SH_CONST || uniform()) {
    SH_DEBUG_ASSERT(Binding == SH_CONST || !ShEnvironment::insideShader);
    SH_DEBUG_ASSERT(other.hasValues());
    T data[3];
    other.getValues(data);
    setValues(data);
  } else {
    ShStatement asn(*this, SH_OP_ASN, other);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(asn);
  }
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<3, Binding, T, Swizzled>::ShAttrib(const ShVariableNodePtr& node,
  const ShSwizzle& swizzle, bool neg)
  : ShGeneric<3, T>(node)
{
  m_swizzle = swizzle;
  m_neg = neg;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<3, Binding, T, Swizzled>::ShAttrib(T data[3])
  : ShGeneric<3, T>(new ShVariableNode(Binding, 3))
{
  if (Binding == SH_CONST) {
    for (int i = 0; i < 3; i++) m_node->setValue(i, data[i]);
  } else {
    (*this) = ShAttrib<3, SH_CONST, T>(data);
  }
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<3, Binding, T, Swizzled>::ShAttrib(T s0, T s1, T s2)
  : ShGeneric<3, T>(new ShVariableNode(Binding, 3))
{
  if (Binding == SH_CONST) {
    m_node->setValue(0, s0);
    m_node->setValue(1, s1);
    m_node->setValue(2, s2);
  } else {
    (*this)[0] = ShAttrib<1, SH_CONST, T>(s0);
    (*this)[1] = ShAttrib<1, SH_CONST, T>(s1);
    (*this)[2] = ShAttrib<1, SH_CONST, T>(s2);
  }
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<3, Binding, T, Swizzled>::ShAttrib(const ShGeneric<1, T>& s0, const ShGeneric<1, T>& s1, const ShGeneric<1, T>& s2)
  : ShGeneric<3, T>(new ShVariableNode(Binding, 3))
{
  if (Binding == SH_CONST) {
    SH_DEBUG_ASSERT(s0.hasValues());
    m_node->setValue(0, s0.getValue(0));
    SH_DEBUG_ASSERT(s1.hasValues());
    m_node->setValue(1, s1.getValue(0));
    SH_DEBUG_ASSERT(s2.hasValues());
    m_node->setValue(2, s2.getValue(0));
  } else {
    (*this)[0] = s0;
    (*this)[1] = s1;
    (*this)[2] = s2;
  }
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<3, Binding, T, Swizzled>::~ShAttrib()
{
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<3, Binding, T, Swizzled>&
ShAttrib<3, Binding, T, Swizzled>::operator=(const ShGeneric<3, T>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<3, Binding, T, Swizzled>&
ShAttrib<3, Binding, T, Swizzled>::operator=(const ShAttrib<3, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<3, Binding, T, Swizzled>&
ShAttrib<3, Binding, T, Swizzled>::operator+=(const ShGeneric<3, T>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<3, Binding, T, Swizzled>&
ShAttrib<3, Binding, T, Swizzled>::operator-=(const ShGeneric<3, T>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<3, Binding, T, Swizzled>&
ShAttrib<3, Binding, T, Swizzled>::operator*=(const ShGeneric<3, T>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<3, Binding, T, Swizzled>&
ShAttrib<3, Binding, T, Swizzled>::operator/=(const ShGeneric<3, T>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<3, Binding, T, Swizzled>&
ShAttrib<3, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<3, Binding, T, Swizzled>&
ShAttrib<3, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<3, Binding, T, Swizzled>&
ShAttrib<3, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<3, Binding, T, Swizzled>&
ShAttrib<3, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<1, Binding, T, true>
ShAttrib<3, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShAttrib<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<2, Binding, T, true>
ShAttrib<3, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShAttrib<2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, s0, s1), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<3, Binding, T, true>
ShAttrib<3, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShAttrib<3, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, s0, s1, s2), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<4, Binding, T, true>
ShAttrib<3, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShAttrib<4, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, s0, s1, s2, s3), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShAttrib<N2, Binding, T, true>
ShAttrib<3, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShAttrib<N2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, N2, indices), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<1, Binding, T, true>
ShAttrib<3, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShAttrib<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(3, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<3, Binding, T, Swizzled>
ShAttrib<3, Binding, T, Swizzled>::operator-() const
{
  return ShAttrib<3, Binding, T, Swizzled>(m_node, m_swizzle, !m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<4, Binding, T, Swizzled>::ShAttrib()
  : ShGeneric<4, T>(new ShVariableNode(Binding, 4))
{
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<4, Binding, T, Swizzled>::ShAttrib(const ShGeneric<4, T>& other)
  : ShGeneric<4, T>(new ShVariableNode(Binding, 4))
{
  if (Binding == SH_CONST || uniform()) {
    SH_DEBUG_ASSERT(Binding == SH_CONST || !ShEnvironment::insideShader);
    SH_DEBUG_ASSERT(other.hasValues());
    T data[4];
    other.getValues(data);
    setValues(data);
  } else {
    ShStatement asn(*this, SH_OP_ASN, other);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(asn);
  }
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<4, Binding, T, Swizzled>::ShAttrib(const ShAttrib<4, Binding, T, Swizzled>& other)
  : ShGeneric<4, T>(new ShVariableNode(Binding, 4))
{
  if (Binding == SH_CONST || uniform()) {
    SH_DEBUG_ASSERT(Binding == SH_CONST || !ShEnvironment::insideShader);
    SH_DEBUG_ASSERT(other.hasValues());
    T data[4];
    other.getValues(data);
    setValues(data);
  } else {
    ShStatement asn(*this, SH_OP_ASN, other);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(asn);
  }
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<4, Binding, T, Swizzled>::ShAttrib(const ShVariableNodePtr& node,
  const ShSwizzle& swizzle, bool neg)
  : ShGeneric<4, T>(node)
{
  m_swizzle = swizzle;
  m_neg = neg;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<4, Binding, T, Swizzled>::ShAttrib(T data[4])
  : ShGeneric<4, T>(new ShVariableNode(Binding, 4))
{
  if (Binding == SH_CONST) {
    for (int i = 0; i < 4; i++) m_node->setValue(i, data[i]);
  } else {
    (*this) = ShAttrib<4, SH_CONST, T>(data);
  }
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<4, Binding, T, Swizzled>::ShAttrib(T s0, T s1, T s2, T s3)
  : ShGeneric<4, T>(new ShVariableNode(Binding, 4))
{
  if (Binding == SH_CONST) {
    m_node->setValue(0, s0);
    m_node->setValue(1, s1);
    m_node->setValue(2, s2);
    m_node->setValue(3, s3);
  } else {
    (*this)[0] = ShAttrib<1, SH_CONST, T>(s0);
    (*this)[1] = ShAttrib<1, SH_CONST, T>(s1);
    (*this)[2] = ShAttrib<1, SH_CONST, T>(s2);
    (*this)[3] = ShAttrib<1, SH_CONST, T>(s3);
  }
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<4, Binding, T, Swizzled>::ShAttrib(const ShGeneric<1, T>& s0, const ShGeneric<1, T>& s1, const ShGeneric<1, T>& s2, const ShGeneric<1, T>& s3)
  : ShGeneric<4, T>(new ShVariableNode(Binding, 4))
{
  if (Binding == SH_CONST) {
    SH_DEBUG_ASSERT(s0.hasValues());
    m_node->setValue(0, s0.getValue(0));
    SH_DEBUG_ASSERT(s1.hasValues());
    m_node->setValue(1, s1.getValue(0));
    SH_DEBUG_ASSERT(s2.hasValues());
    m_node->setValue(2, s2.getValue(0));
    SH_DEBUG_ASSERT(s3.hasValues());
    m_node->setValue(3, s3.getValue(0));
  } else {
    (*this)[0] = s0;
    (*this)[1] = s1;
    (*this)[2] = s2;
    (*this)[3] = s3;
  }
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<4, Binding, T, Swizzled>::~ShAttrib()
{
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<4, Binding, T, Swizzled>&
ShAttrib<4, Binding, T, Swizzled>::operator=(const ShGeneric<4, T>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<4, Binding, T, Swizzled>&
ShAttrib<4, Binding, T, Swizzled>::operator=(const ShAttrib<4, Binding, T, Swizzled>& other)
{
  ParentType::operator=(other);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<4, Binding, T, Swizzled>&
ShAttrib<4, Binding, T, Swizzled>::operator+=(const ShGeneric<4, T>& right)
{
  ParentType::operator+=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<4, Binding, T, Swizzled>&
ShAttrib<4, Binding, T, Swizzled>::operator-=(const ShGeneric<4, T>& right)
{
  ParentType::operator-=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<4, Binding, T, Swizzled>&
ShAttrib<4, Binding, T, Swizzled>::operator*=(const ShGeneric<4, T>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<4, Binding, T, Swizzled>&
ShAttrib<4, Binding, T, Swizzled>::operator/=(const ShGeneric<4, T>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<4, Binding, T, Swizzled>&
ShAttrib<4, Binding, T, Swizzled>::operator*=(T right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<4, Binding, T, Swizzled>&
ShAttrib<4, Binding, T, Swizzled>::operator/=(T right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<4, Binding, T, Swizzled>&
ShAttrib<4, Binding, T, Swizzled>::operator*=(const ShGeneric<1, T>& right)
{
  ParentType::operator*=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<4, Binding, T, Swizzled>&
ShAttrib<4, Binding, T, Swizzled>::operator/=(const ShGeneric<1, T>& right)
{
  ParentType::operator/=(right);
  return *this;
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<1, Binding, T, true>
ShAttrib<4, Binding, T, Swizzled>::operator()(int s0) const
{
  return ShAttrib<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<2, Binding, T, true>
ShAttrib<4, Binding, T, Swizzled>::operator()(int s0, int s1) const
{
  return ShAttrib<2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, s0, s1), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<3, Binding, T, true>
ShAttrib<4, Binding, T, Swizzled>::operator()(int s0, int s1, int s2) const
{
  return ShAttrib<3, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, s0, s1, s2), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<4, Binding, T, true>
ShAttrib<4, Binding, T, Swizzled>::operator()(int s0, int s1, int s2, int s3) const
{
  return ShAttrib<4, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, s0, s1, s2, s3), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
template<int N2>
ShAttrib<N2, Binding, T, true>
ShAttrib<4, Binding, T, Swizzled>::swiz(int indices[]) const
{
  return ShAttrib<N2, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, N2, indices), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<1, Binding, T, true>
ShAttrib<4, Binding, T, Swizzled>::operator[](int s0) const
{
  return ShAttrib<1, Binding, T, true>(m_node, m_swizzle * ShSwizzle(4, s0), m_neg);
}

template<ShBindingType Binding, typename T, bool Swizzled>
ShAttrib<4, Binding, T, Swizzled>
ShAttrib<4, Binding, T, Swizzled>::operator-() const
{
  return ShAttrib<4, Binding, T, Swizzled>(m_node, m_swizzle, !m_neg);
}


} // namespace SH

#endif // SH_SHATTRIBIMPL_HPP
