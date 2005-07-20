// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
//////////////////////////////////////////////////////////////////////////////
#ifndef SHGENERICIMPL_HPP
#define SHGENERICIMPL_HPP

#include "ShGeneric.hpp"
#include "ShAttrib.hpp"
#include "ShInstructions.hpp"
#include "ShDebug.hpp"
#include "ShProgram.hpp"

namespace SH {

template<int N, typename T>
ShGeneric<N, T>::ShGeneric(const ShVariableNodePtr& node)
  : ShVariable(node)
{
  SH_DEBUG_ASSERT(node); // DEBUG
}

template<int N, typename T>
ShGeneric<N, T>::ShGeneric(const ShVariableNodePtr& node, ShSwizzle swizzle, bool neg)
  : ShVariable(node)
{
  m_swizzle = swizzle;
  m_neg = neg;
  SH_DEBUG_ASSERT(node); // DEBUG
}

template<int N, typename T>
ShGeneric<N, T>::~ShGeneric()
{
}

template<int N, typename T>
template<typename T2>
ShGeneric<N, T>::ShGeneric(const ShGeneric<N, T2>& other)
  : ShVariable(new ShVariableNode(SH_TEMP, N, value_type, 
        other.node()->specialType()))
{
  SH_DEBUG_ASSERT(other.node());
  SH_DEBUG_ASSERT(m_node);
  shASN(*this, other);
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator=(const ShProgram& prg)
{
  this->ShVariable::operator=(prg);
  return *this;
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator=(const ShGeneric<N, T>& other)
{
  shASN(*this, other);
  return *this;
}

template<int N, typename T>
template<typename T2>
ShGeneric<N, T>& ShGeneric<N, T>::operator=(const ShGeneric<N, T2>& other)
{
  shASN(*this, other);
  return *this;
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator++()
{
  *this += 1;
  return *this;
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator--()
{
  *this -= 1;
  return *this;
}

template<int N, typename T>
template<typename T2>
ShGeneric<N, T>& ShGeneric<N, T>::operator+=(const ShGeneric<N, T2>& right)
{
  *this = *this + right;
  return *this;
}

template<int N, typename T>
template<typename T2>
ShGeneric<N, T>& ShGeneric<N, T>::operator-=(const ShGeneric<N, T2>& right)
{
  *this = *this - right;
  return *this;
}

template<int N, typename T>
template<typename T2>
ShGeneric<N, T>& ShGeneric<N, T>::operator*=(const ShGeneric<N, T2>& right)
{
  *this = *this * right;
  return *this;
}

template<int N, typename T>
template<typename T2>
ShGeneric<N, T>& ShGeneric<N, T>::operator/=(const ShGeneric<N, T2>& right)
{
  *this = *this / right;
  return *this;
}

template<int N, typename T>
template<typename T2>
ShGeneric<N, T>& ShGeneric<N, T>::operator%=(const ShGeneric<N, T2>& right)
{
  *this = *this % right;
  return *this;
}

template<int N, typename T>
template<typename T2>
ShGeneric<N, T>& ShGeneric<N, T>::operator+=(const ShGeneric<1, T2>& right)
{
  *this = *this + right;
  return *this;
}

template<int N, typename T>
template<typename T2>
ShGeneric<N, T>& ShGeneric<N, T>::operator-=(const ShGeneric<1, T2>& right)
{
  *this = *this - right;
  return *this;
}

template<int N, typename T>
template<typename T2>
ShGeneric<N, T>& ShGeneric<N, T>::operator*=(const ShGeneric<1, T2>& right)
{
  *this = *this * right;
  return *this;
}

template<int N, typename T>
template<typename T2>
ShGeneric<N, T>& ShGeneric<N, T>::operator/=(const ShGeneric<1, T2>& right)
{
  *this = *this / right;
  return *this;
}

template<int N, typename T>
template<typename T2>
ShGeneric<N, T>& ShGeneric<N, T>::operator%=(const ShGeneric<1, T2>& right)
{
  *this = *this % right;
  return *this;
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator+=(host_type right)
{
  *this = *this + right;
  return *this;
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator-=(host_type right)
{
  *this = *this - right;
  return *this;
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator*=(host_type right)
{
  *this = *this * right;
  return *this;
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator/=(host_type right)
{
  *this = *this / right;
  return *this;
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator%=(host_type right)
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
void ShGeneric<N, T>::range(host_type low, host_type high) 
{
  rangeVariant(new VariantType(1, low), new VariantType(1, high));
}

template<int N, typename T>
typename ShGeneric<N, T>::VariantType ShGeneric<N, T>::lowBound() const
{
  return (*variant_cast<T, SH_HOST>(lowBoundVariant()));
}

template<int N, typename T>
typename ShGeneric<N, T>::host_type ShGeneric<N, T>::lowBound(int index) const
{
  return (*variant_cast<T, SH_HOST>(lowBoundVariant()))[index];
}

template<int N, typename T>
typename ShGeneric<N, T>::VariantType ShGeneric<N, T>::highBound() const
{
  return (*variant_cast<T, SH_HOST>(highBoundVariant()));
}

template<int N, typename T>
typename ShGeneric<N, T>::host_type ShGeneric<N, T>::highBound(int index) const
{
  return (*variant_cast<T, SH_HOST>(highBoundVariant()))[index];
}
  
template<int N, typename T> 
template<int N2>
ShGeneric<N2, T> ShGeneric<N, T>::swiz(int indices[]) const
{
  return ShGeneric<N2, T>(m_node, m_swizzle * ShSwizzle(N, N2, indices), m_neg);
}

template<int N, typename T>
void ShGeneric<N, T>::getValues(host_type dest[]) const
{
  VariantTypePtr c = variant_cast<T, SH_HOST>(getVariant()); 
  for(int i = 0; i < N; ++i) dest[i] = (*c)[i]; 
}

template<int N, typename T>
typename ShGeneric<N, T>::host_type ShGeneric<N, T>::getValue(int index) const
{
  VariantTypePtr c = variant_cast<T, SH_HOST>(getVariant(index)); 
  return (*c)[0];
}

template<int N, typename T>
void ShGeneric<N, T>::setValue(int index, const host_type &variantValue) 
{
  if (m_swizzle.identity() && !m_neg) {
    VariantTypePtr c(variant_cast<T, SH_HOST>(m_node->getVariant())); 
    (*c)[index] = variantValue;
  } else {
    VariantTypePtr variantPtr(new VariantType(1, variantValue));
    setVariant(variantPtr, false, ShSwizzle(N, index));
  }
}

template<int N, typename T>
void ShGeneric<N, T>::setValues(const host_type variantValues[]) 
{
  if (m_swizzle.identity() && !m_neg) {
    VariantTypePtr c(variant_cast<T, SH_HOST>(m_node->getVariant())); 
    for (int i=0; i < N; i++) {
      (*c)[i] = variantValues[i];
    }
  } else {
    host_type* data = const_cast<host_type*>(variantValues);
    VariantTypePtr variantPtr(new VariantType(N, data, false));
    setVariant(variantPtr);
  }
}

template<typename T>
ShGeneric<1, T>::ShGeneric(const ShVariableNodePtr& node)
  : ShVariable(node)
{
  SH_DEBUG_ASSERT(node); // DEBUG
}

template<typename T>
ShGeneric<1, T>::ShGeneric(const ShVariableNodePtr& node, ShSwizzle swizzle, bool neg)
  : ShVariable(node)
{
  m_swizzle = swizzle;
  m_neg = neg;
  SH_DEBUG_ASSERT(node); // DEBUG
}

template<typename T>
ShGeneric<1, T>::~ShGeneric()
{
}

template<typename T>
template<typename T2>
ShGeneric<1, T>::ShGeneric(const ShGeneric<1, T2>& other)
  : ShVariable(new ShVariableNode(SH_TEMP, 1, value_type, other.node()->specialType()))
{
  SH_DEBUG_ASSERT(other.node());
  SH_DEBUG_ASSERT(m_node);
  shASN(*this, other);
}

template<typename T>
ShGeneric<1, T>& ShGeneric<1, T>::operator=(const ShProgram& prg)
{
  this->ShVariable::operator=(prg);
  return *this;
}

template<typename T>
ShGeneric<1, T>& ShGeneric<1, T>::operator=(const ShGeneric<1, T>& other)
{
  shASN(*this, other);
  return *this;
}

template<typename T>
template<typename T2>
ShGeneric<1, T>& ShGeneric<1, T>::operator=(const ShGeneric<1, T2>& other)
{
  shASN(*this, other);
  return *this;
}


template<typename T>
ShGeneric<1, T>& ShGeneric<1, T>::operator=(host_type other)
{
  shASN(*this, ShAttrib<1, SH_CONST, T>(other));
  return *this;
}

template<typename T>
ShGeneric<1, T>& ShGeneric<1, T>::operator++()
{
  *this += 1;
  return *this;
}

template<typename T>
ShGeneric<1, T>& ShGeneric<1, T>::operator--()
{
  *this -= 1;
  return *this;
}

template<typename T>
template<typename T2>
ShGeneric<1, T>& ShGeneric<1, T>::operator+=(const ShGeneric<1, T2>& right)
{
  *this = *this + right;
  return *this;
}

template<typename T>
template<typename T2>
ShGeneric<1, T>& ShGeneric<1, T>::operator-=(const ShGeneric<1, T2>& right)
{
  *this = *this - right;
  return *this;
}

template<typename T>
template<typename T2>
ShGeneric<1, T>& ShGeneric<1, T>::operator*=(const ShGeneric<1, T2>& right)
{
  *this = *this * right;
  return *this;
}

template<typename T>
template<typename T2>
ShGeneric<1, T>& ShGeneric<1, T>::operator/=(const ShGeneric<1, T2>& right)
{
  *this = *this / right;
  return *this;
}

template<typename T>
template<typename T2>
ShGeneric<1, T>& ShGeneric<1, T>::operator%=(const ShGeneric<1, T2>& right)
{
  *this = *this % right;
  return *this;
}

template<typename T>
ShGeneric<1, T>& ShGeneric<1, T>::operator*=(host_type right)
{
  *this = *this * right;
  return *this;
}

template<typename T>
ShGeneric<1, T>& ShGeneric<1, T>::operator/=(host_type right)
{
  *this = *this / right;
  return *this;
}

template<typename T>
ShGeneric<1, T>& ShGeneric<1, T>::operator%=(host_type right)
{
  *this = *this % right;
  return *this;
}

template<typename T>
ShGeneric<1, T>& ShGeneric<1, T>::operator+=(host_type right)
{
  *this = *this + right;
  return *this;
}

template<typename T>
ShGeneric<1, T>& ShGeneric<1, T>::operator-=(host_type right)
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
void ShGeneric<1, T>::range(host_type low, host_type high) 
{
  rangeVariant(new VariantType(1, low), new VariantType(1, high));
}

template<typename T>
typename ShGeneric<1, T>::VariantType ShGeneric<1, T>::lowBound() const
{
  return (*variant_cast<T, SH_HOST>(lowBoundVariant()));
}

template<typename T>
typename ShGeneric<1, T>::host_type ShGeneric<1, T>::lowBound(int index) const
{
  return (*variant_cast<T, SH_HOST>(lowBoundVariant()))[index];
}

template<typename T>
typename ShGeneric<1, T>::VariantType ShGeneric<1, T>::highBound() const
{
  return (*variant_cast<T, SH_HOST>(highBoundVariant()));
}

template<typename T>
typename ShGeneric<1, T>::host_type ShGeneric<1, T>::highBound(int index) const
{
  return (*variant_cast<T, SH_HOST>(highBoundVariant()))[index];
}
  
template<typename T> 
template<int N2>
ShGeneric<N2, T> ShGeneric<1, T>::swiz(int indices[]) const
{
  return ShGeneric<N2, T>(m_node, m_swizzle * ShSwizzle(1, N2, indices), m_neg);
}

template<typename T>
void ShGeneric<1, T>::getValues(host_type dest[]) const
{
  VariantTypePtr c = variant_cast<T, SH_HOST>(getVariant()); 
  dest[0] = (*c)[0]; 
}

template<typename T>
typename ShGeneric<1, T>::host_type ShGeneric<1, T>::getValue(int index) const
{
  VariantTypePtr c = variant_cast<T, SH_HOST>(getVariant(index)); 
  return (*c)[0];
}

template<typename T>
void ShGeneric<1, T>::setValue(int index, const host_type &variantValue) 
{
  VariantTypePtr variant(new VariantType(1, variantValue));
  setVariant(variant, false, ShSwizzle(1, index));
}

template<typename T>
void ShGeneric<1, T>::setValues(const host_type variantValues[]) 
{
  setVariant(new VariantType(1, variantValues[0]));
}

}

#endif
