#ifndef SHATTRIBIMPL_HPP
#define SHATTRIBIMPL_HPP

#include "ShAttrib.hpp"
#include "ShConstant.hpp"
#include "ShStatement.hpp"
#include "ShEnvironment.hpp"
#include "ShLib.hpp"

namespace SH {

template<int N, int Kind, typename T, bool Swizzled>
ShAttrib<N, Kind, T, Swizzled>::ShAttrib()
  : ShVariableN<N, T>(new ShVariableNode(static_cast<ShVariableKind>(Kind), N))
{
}

template<int N, int Kind, typename T, bool Swizzled>
ShAttrib<N, Kind, T, Swizzled>::ShAttrib(const ShVariableN<N, T>& other)
  : ShVariableN<N, T>(new ShVariableNode(static_cast<ShVariableKind>(Kind), N))
{
  if (uniform()) {
    assert(!ShEnvironment::insideShader);
    assert(other.hasValues());
    T data[N];
    other.getValues(data);
    setValues(data);
  } else {
    ShStatement asn(*this, SH_OP_ASN, other);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(asn);
  }
}

template<int N, int Kind, typename T, bool Swizzled>
ShAttrib<N, Kind, T, Swizzled>::ShAttrib(const ShAttrib<N, Kind, T, Swizzled>& other)
  : ShVariableN<N, T>(new ShVariableNode(static_cast<ShVariableKind>(Kind), N))
{
  if (uniform()) {
    assert(!ShEnvironment::insideShader);
    assert(other.hasValues());
    T data[N];
    other.getValues(data);
    setValues(data);
  } else {
    ShStatement asn(*this, SH_OP_ASN, other);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(asn);
  }
}

template<int N, int Kind, typename T, bool Swizzled>
ShAttrib<N, Kind, T, Swizzled>::ShAttrib(const ShVariableNodePtr& node,
                                         const ShSwizzle& swizzle,
                                         bool neg)
  : ShVariableN<N, T>(node)
{
  m_swizzle = swizzle;
  m_neg = neg;
}

template<int N, int Kind, typename T, bool Swizzled>
ShAttrib<N, Kind, T, Swizzled>::ShAttrib(T v1)
  : ShVariableN<N, T>(new ShVariableNode(static_cast<ShVariableKind>(Kind), N))
{
  if (uniform()) {
    m_node->setValue(0, v1);
  } else {
    ShConstant<1, T> value(v1);
    ShStatement asn(*this, SH_OP_ASN, value);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(asn);
  }
}

template<int N, int Kind, typename T, bool Swizzled>
ShAttrib<N, Kind, T, Swizzled>::ShAttrib(T v1, T v2)
  : ShVariableN<N, T>(new ShVariableNode(static_cast<ShVariableKind>(Kind), N))
{
  if (uniform()) {
    m_node->setValue(0, v1);
    m_node->setValue(1, v2);
  } else {
    T values[2] = {v1, v2};
    ShConstant<2, T> value(values);
    ShStatement asn(*this, SH_OP_ASN, value);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(asn);
  }
}

template<int N, int Kind, typename T, bool Swizzled>
ShAttrib<N, Kind, T, Swizzled>::ShAttrib(T v1, T v2, T v3)
  : ShVariableN<N, T>(new ShVariableNode(static_cast<ShVariableKind>(Kind), N))
{
  if (uniform()) {
    m_node->setValue(0, v1);
    m_node->setValue(1, v2);
    m_node->setValue(2, v3);
  } else {
    T values[3] = {v1, v2, v3};
    ShConstant<3, T> value(values);
    ShStatement asn(*this, SH_OP_ASN, value);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(asn);
  }
}

template<int N, int Kind, typename T, bool Swizzled>
ShAttrib<N, Kind, T, Swizzled>::ShAttrib(T v1, T v2, T v3, T v4)
  : ShVariableN<N, T>(new ShVariableNode(static_cast<ShVariableKind>(Kind), N))
{
  if (uniform()) {
    m_node->setValue(0, v1);
    m_node->setValue(1, v2);
    m_node->setValue(2, v3);
    m_node->setValue(3, v4);
  } else {
    T values[4] = {v1, v2, v3, v4};
    ShConstant<4, T> value(values);
    ShStatement asn(*this, SH_OP_ASN, value);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(asn);
  }
}

template<int N, int Kind, typename T, bool Swizzled>
ShAttrib<N, Kind, T, Swizzled>&
ShAttrib<N, Kind, T, Swizzled>::operator=(const ShVariableN<N, T>& other)
{
  if (uniform()) {
    assert(!ShEnvironment::insideShader);
    assert(other.hasValues());
    T data[N];
    other.getValues(data);
    setValues(data);
  } else {
    ShStatement asn(*this, SH_OP_ASN, other);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(asn);
  }
  return *this;
}

template<int N, int Kind, typename T, bool Swizzled>
ShAttrib<N, Kind, T, Swizzled>&
ShAttrib<N, Kind, T, Swizzled>::operator=(const ShAttrib<N, Kind, T, Swizzled>& other)
{
  if (uniform()) {
    assert(!ShEnvironment::insideShader);
    assert(other.hasValues());
    T data[N];
    other.getValues(data);
    setValues(data);
  } else {
    ShStatement asn(*this, SH_OP_ASN, other);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(asn);
  }
  return *this;
}

/// In-place addition
template<int N, int Kind, typename T, bool Swizzled>
ShAttrib<N, Kind, T, Swizzled>&
ShAttrib<N, Kind, T, Swizzled>::operator+=(const ShVariableN<N, T>& right)
{
  *this = *this + right;

  return *this;
}

/// In-place subtraction
template<int N, int Kind, typename T, bool Swizzled>
ShAttrib<N, Kind, T, Swizzled>&
ShAttrib<N, Kind, T, Swizzled>::operator-=(const ShVariableN<N, T>& right)
{
  *this = *this - right;

  return *this;
}

/// In-place multiplication
template<int N, int Kind, typename T, bool Swizzled>
template<typename T2>
ShAttrib<N, Kind, T, Swizzled>&
ShAttrib<N, Kind, T, Swizzled>::operator*=(const T2& right)
{
  *this = *this * right;

  return *this;
}

/// In-place division
template<int N, int Kind, typename T, bool Swizzled>
template<typename T2>
ShAttrib<N, Kind, T, Swizzled>&
ShAttrib<N, Kind, T, Swizzled>::operator/=(const T2& right)
{
  *this = *this / right;

  return *this;
}

template<int N, int Kind, typename T, bool Swizzled>
ShAttrib<N, Kind, T, Swizzled>::~ShAttrib()
{
}

template<int N, int Kind, typename T, bool Swizzled>
ShAttrib<N, Kind, T, true> ShAttrib<N, Kind, T, Swizzled>::operator()() const
{
  return ShAttrib<N, Kind, T, true>(m_node, m_swizzle, m_neg);
}

template<int N, int Kind, typename T, bool Swizzled>
ShAttrib<1, Kind, T, true> ShAttrib<N, Kind, T, Swizzled>::operator()(int i1) const
{
  return ShAttrib<1, Kind, T, true>(m_node, m_swizzle * ShSwizzle(N, i1), m_neg);
}

template<int N, int Kind, typename T, bool Swizzled>
ShAttrib<1, Kind, T, true> ShAttrib<N, Kind, T, Swizzled>::operator[](int i1) const
{
  return ShAttrib<1, Kind, T, true>(m_node, m_swizzle * ShSwizzle(N, i1), m_neg);
}

template<int N, int Kind, typename T, bool Swizzled>
ShAttrib<2, Kind, T, true> ShAttrib<N, Kind, T, Swizzled>::operator()(int i1, int i2) const
{
  return ShAttrib<2, Kind, T, true>(m_node, m_swizzle * ShSwizzle(N, i1, i2), m_neg);
}

template<int N, int Kind, typename T, bool Swizzled>
ShAttrib<3, Kind, T, true> ShAttrib<N, Kind, T, Swizzled>::operator()(int i1, int i2, int i3) const
{
  return ShAttrib<3, Kind, T, true>(m_node, m_swizzle * ShSwizzle(N, i1, i2, i3), m_neg);
}

template<int N, int Kind, typename T, bool Swizzled>
ShAttrib<4, Kind, T, true> ShAttrib<N, Kind, T, Swizzled>::operator()(int i1, int i2, int i3, int i4) const
{
  return ShAttrib<4, Kind, T, true>(m_node, m_swizzle * ShSwizzle(N, i1, i2, i3, i4), m_neg);
}

template<int N, int Kind, typename T, bool Swizzled>
ShAttrib<N, Kind, T, Swizzled>
ShAttrib<N, Kind, T, Swizzled>::operator-() const
{
  return ShAttrib<N, Kind, T, Swizzled>(m_node, m_swizzle, !m_neg);
}

}

#endif
