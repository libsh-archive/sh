// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
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

#include "Generic.hpp"
#include "Attrib.hpp"
#include "Instructions.hpp"
#include "Debug.hpp"
#include "Program.hpp"

#ifdef __APPLE__
#include "Lib.hpp"
#endif

namespace SH {

template<int N, typename T>
Generic<N, T>::Generic(const VariableNodePtr& node)
  : Variable(node)
{
  DEBUG_ASSERT(node);
}

template<int N, typename T>
Generic<N, T>::Generic(const VariableNodePtr& node, Swizzle swizzle, bool neg)
  : Variable(node)
{
  m_swizzle = swizzle;
  m_neg = neg;
  DEBUG_ASSERT(node);
}

template<int N, typename T>
Generic<N, T>::~Generic()
{
}

template<int N, typename T>
template<typename T2>
Generic<N, T>::Generic(const Generic<N, T2>& other)
  : Variable(new VariableNode(TEMP, N, value_type, 
        other.node()->specialType()))
{
  DEBUG_ASSERT(other.node());
  DEBUG_ASSERT(m_node);
  shASN(*this, other);
}

template<int N, typename T>
Generic<N, T>& Generic<N, T>::operator=(const Program& prg)
{
  this->Variable::operator=(prg);
  return *this;
}

template<int N, typename T>
Generic<N, T>& Generic<N, T>::operator=(const Generic<N, T>& other)
{
  shASN(*this, other);
  return *this;
}

template<int N, typename T>
template<typename T2>
Generic<N, T>& Generic<N, T>::operator=(const Generic<N, T2>& other)
{
  shASN(*this, other);
  return *this;
}

template<int N, typename T>
Generic<N, T>& Generic<N, T>::operator++()
{
  *this += 1;
  return *this;
}

template<int N, typename T>
Generic<N, T>& Generic<N, T>::operator--()
{
  *this -= 1;
  return *this;
}

template<int N, typename T>
template<typename T2>
Generic<N, T>& Generic<N, T>::operator+=(const Generic<N, T2>& right)
{
  *this = *this + right;
  return *this;
}

template<int N, typename T>
template<typename T2>
Generic<N, T>& Generic<N, T>::operator-=(const Generic<N, T2>& right)
{
  *this = *this - right;
  return *this;
}

template<int N, typename T>
template<typename T2>
Generic<N, T>& Generic<N, T>::operator*=(const Generic<N, T2>& right)
{
  *this = *this * right;
  return *this;
}

template<int N, typename T>
template<typename T2>
Generic<N, T>& Generic<N, T>::operator/=(const Generic<N, T2>& right)
{
  *this = *this / right;
  return *this;
}

template<int N, typename T>
template<typename T2>
Generic<N, T>& Generic<N, T>::operator%=(const Generic<N, T2>& right)
{
  *this = *this % right;
  return *this;
}

template<int N, typename T>
template<typename T2>
Generic<N, T>& Generic<N, T>::operator+=(const Generic<1, T2>& right)
{
  *this = *this + right;
  return *this;
}

template<int N, typename T>
template<typename T2>
Generic<N, T>& Generic<N, T>::operator-=(const Generic<1, T2>& right)
{
  *this = *this - right;
  return *this;
}

template<int N, typename T>
template<typename T2>
Generic<N, T>& Generic<N, T>::operator*=(const Generic<1, T2>& right)
{
  *this = *this * right;
  return *this;
}

template<int N, typename T>
template<typename T2>
Generic<N, T>& Generic<N, T>::operator/=(const Generic<1, T2>& right)
{
  *this = *this / right;
  return *this;
}

template<int N, typename T>
template<typename T2>
Generic<N, T>& Generic<N, T>::operator%=(const Generic<1, T2>& right)
{
  *this = *this % right;
  return *this;
}

template<int N, typename T>
Generic<N, T>& Generic<N, T>::operator+=(host_type right)
{
  *this = *this + right;
  return *this;
}

template<int N, typename T>
Generic<N, T>& Generic<N, T>::operator-=(host_type right)
{
  *this = *this - right;
  return *this;
}

template<int N, typename T>
Generic<N, T>& Generic<N, T>::operator*=(host_type right)
{
  *this = *this * right;
  return *this;
}

template<int N, typename T>
Generic<N, T>& Generic<N, T>::operator/=(host_type right)
{
  *this = *this / right;
  return *this;
}

template<int N, typename T>
Generic<N, T>& Generic<N, T>::operator%=(host_type right)
{
  *this = *this % right;
  return *this;
}

template<int N, typename T>
Generic<N, T> Generic<N, T>::operator-() const
{
  return Generic<N, T>(m_node, m_swizzle, !m_neg);
}


template<int N, typename T>
Generic<N, T> Generic<N, T>::operator()() const
{
  return Generic<N, T>(m_node, m_swizzle, m_neg);
}

template<int N, typename T>
Generic<1, T> Generic<N, T>::operator()(int i1) const
{
  return Generic<1, T>(m_node, m_swizzle * Swizzle(size(), i1), m_neg);
}

template<int N, typename T>
Generic<1, T> Generic<N, T>::operator[](int i1) const
{
  return Generic<1, T>(m_node, m_swizzle * Swizzle(size(), i1), m_neg);
}

template<int N, typename T>
Generic<2, T> Generic<N, T>::operator()(int i1, int i2) const
{
  return Generic<2, T>(m_node, m_swizzle * Swizzle(size(), i1, i2), m_neg);
}

template<int N, typename T>
Generic<3, T> Generic<N, T>::operator()(int i1, int i2, int i3) const
{
  return Generic<3, T>(m_node, m_swizzle * Swizzle(size(), i1, i2, i3), m_neg);
}

template<int N, typename T>
Generic<4, T> Generic<N, T>::operator()(int i1, int i2, int i3, int i4) const
{
  return Generic<4, T>(m_node, m_swizzle * Swizzle(size(), i1, i2, i3, i4), m_neg);
}

template<int N, typename T>
void Generic<N, T>::range(host_type low, host_type high) 
{
  rangeVariant(new VariantType(1, low), new VariantType(1, high));
}

template<int N, typename T>
typename Generic<N, T>::VariantType Generic<N, T>::lowBound() const
{
  return (*variant_cast<T, HOST>(lowBoundVariant()));
}

template<int N, typename T>
typename Generic<N, T>::host_type Generic<N, T>::lowBound(int index) const
{
  return (*variant_cast<T, HOST>(lowBoundVariant()))[index];
}

template<int N, typename T>
typename Generic<N, T>::VariantType Generic<N, T>::highBound() const
{
  return (*variant_cast<T, HOST>(highBoundVariant()));
}

template<int N, typename T>
typename Generic<N, T>::host_type Generic<N, T>::highBound(int index) const
{
  return (*variant_cast<T, HOST>(highBoundVariant()))[index];
}
  
template<int N, typename T> 
template<int N2>
Generic<N2, T> Generic<N, T>::swiz(int indices[]) const
{
  return Generic<N2, T>(m_node, m_swizzle * Swizzle(N, N2, indices), m_neg);
}

template<int N, typename T>
void Generic<N, T>::getValues(host_type dest[]) const
{
  VariantTypePtr c = variant_cast<T, HOST>(getVariant()); 
  for(int i = 0; i < N; ++i) dest[i] = (*c)[i]; 
}

template<int N, typename T>
typename Generic<N, T>::host_type Generic<N, T>::getValue(int index) const
{
  VariantTypePtr c = variant_cast<T, HOST>(getVariant(index)); 
  return (*c)[0];
}

template<int N, typename T>
void Generic<N, T>::setValue(int index, const host_type &variantValue) 
{
  if (m_swizzle.identity() && !m_neg) {
    VariantTypePtr c(variant_cast<T, HOST>(m_node->getVariant())); 
    (*c)[index] = variantValue;
  } else {
    VariantTypePtr variantPtr(new VariantType(1, variantValue));
    setVariant(variantPtr, false, Swizzle(N, index));
  }
}

template<int N, typename T>
void Generic<N, T>::setValues(const host_type variantValues[]) 
{
  if (m_swizzle.identity() && !m_neg) {
    VariantTypePtr c(variant_cast<T, HOST>(m_node->getVariant())); 
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
Generic<1, T>::Generic(const VariableNodePtr& node)
  : Variable(node)
{
  DEBUG_ASSERT(node);
}

template<typename T>
Generic<1, T>::Generic(const VariableNodePtr& node, Swizzle swizzle, bool neg)
  : Variable(node)
{
  m_swizzle = swizzle;
  m_neg = neg;
  DEBUG_ASSERT(node);
}

template<typename T>
Generic<1, T>::~Generic()
{
}

template<typename T>
template<typename T2>
Generic<1, T>::Generic(const Generic<1, T2>& other)
  : Variable(new VariableNode(TEMP, 1, value_type, other.node()->specialType()))
{
  DEBUG_ASSERT(other.node());
  DEBUG_ASSERT(m_node);
  shASN(*this, other);
}

template<typename T>
Generic<1, T>& Generic<1, T>::operator=(const Program& prg)
{
  this->Variable::operator=(prg);
  return *this;
}

template<typename T>
Generic<1, T>& Generic<1, T>::operator=(const Generic<1, T>& other)
{
  shASN(*this, other);
  return *this;
}

template<typename T>
template<typename T2>
Generic<1, T>& Generic<1, T>::operator=(const Generic<1, T2>& other)
{
  shASN(*this, other);
  return *this;
}


template<typename T>
Generic<1, T>& Generic<1, T>::operator=(host_type other)
{
  shASN(*this, Attrib<1, CONST, T>(other));
  return *this;
}

template<typename T>
Generic<1, T>& Generic<1, T>::operator++()
{
  *this += 1;
  return *this;
}

template<typename T>
Generic<1, T>& Generic<1, T>::operator--()
{
  *this -= 1;
  return *this;
}

template<typename T>
template<typename T2>
Generic<1, T>& Generic<1, T>::operator+=(const Generic<1, T2>& right)
{
  *this = *this + right;
  return *this;
}

template<typename T>
template<typename T2>
Generic<1, T>& Generic<1, T>::operator-=(const Generic<1, T2>& right)
{
  *this = *this - right;
  return *this;
}

template<typename T>
template<typename T2>
Generic<1, T>& Generic<1, T>::operator*=(const Generic<1, T2>& right)
{
  *this = *this * right;
  return *this;
}

template<typename T>
template<typename T2>
Generic<1, T>& Generic<1, T>::operator/=(const Generic<1, T2>& right)
{
  *this = *this / right;
  return *this;
}

template<typename T>
template<typename T2>
Generic<1, T>& Generic<1, T>::operator%=(const Generic<1, T2>& right)
{
  *this = *this % right;
  return *this;
}

template<typename T>
Generic<1, T>& Generic<1, T>::operator*=(host_type right)
{
  *this = *this * right;
  return *this;
}

template<typename T>
Generic<1, T>& Generic<1, T>::operator/=(host_type right)
{
  *this = *this / right;
  return *this;
}

template<typename T>
Generic<1, T>& Generic<1, T>::operator%=(host_type right)
{
  *this = *this % right;
  return *this;
}

template<typename T>
Generic<1, T>& Generic<1, T>::operator+=(host_type right)
{
  *this = *this + right;
  return *this;
}

template<typename T>
Generic<1, T>& Generic<1, T>::operator-=(host_type right)
{
  *this = *this - right;
  return *this;
}

template<typename T>
Generic<1, T> Generic<1, T>::operator-() const
{
  return Generic<1, T>(m_node, m_swizzle, !m_neg);
}


template<typename T>
Generic<1, T> Generic<1, T>::operator()() const
{
  return Generic<1, T>(m_node, m_swizzle, m_neg);
}

template<typename T>
Generic<1, T> Generic<1, T>::operator()(int i1) const
{
  return Generic<1, T>(m_node, m_swizzle * Swizzle(size(), i1), m_neg);
}

template<typename T>
Generic<1, T> Generic<1, T>::operator[](int i1) const
{
  return Generic<1, T>(m_node, m_swizzle * Swizzle(size(), i1), m_neg);
}

template<typename T>
Generic<2, T> Generic<1, T>::operator()(int i1, int i2) const
{
  return Generic<2, T>(m_node, m_swizzle * Swizzle(size(), i1, i2), m_neg);
}

template<typename T>
Generic<3, T> Generic<1, T>::operator()(int i1, int i2, int i3) const
{
  return Generic<3, T>(m_node, m_swizzle * Swizzle(size(), i1, i2, i3), m_neg);
}

template<typename T>
Generic<4, T> Generic<1, T>::operator()(int i1, int i2, int i3, int i4) const
{
  return Generic<4, T>(m_node, m_swizzle * Swizzle(size(), i1, i2, i3, i4), m_neg);
}

template<typename T>
void Generic<1, T>::range(host_type low, host_type high) 
{
  rangeVariant(new VariantType(1, low), new VariantType(1, high));
}

template<typename T>
typename Generic<1, T>::VariantType Generic<1, T>::lowBound() const
{
  return (*variant_cast<T, HOST>(lowBoundVariant()));
}

template<typename T>
typename Generic<1, T>::host_type Generic<1, T>::lowBound(int index) const
{
  return (*variant_cast<T, HOST>(lowBoundVariant()))[index];
}

template<typename T>
typename Generic<1, T>::VariantType Generic<1, T>::highBound() const
{
  return (*variant_cast<T, HOST>(highBoundVariant()));
}

template<typename T>
typename Generic<1, T>::host_type Generic<1, T>::highBound(int index) const
{
  return (*variant_cast<T, HOST>(highBoundVariant()))[index];
}
  
template<typename T> 
template<int N2>
Generic<N2, T> Generic<1, T>::swiz(int indices[]) const
{
  return Generic<N2, T>(m_node, m_swizzle * Swizzle(1, N2, indices), m_neg);
}

template<typename T>
void Generic<1, T>::getValues(host_type dest[]) const
{
  VariantTypePtr c = variant_cast<T, HOST>(getVariant()); 
  dest[0] = (*c)[0]; 
}

template<typename T>
typename Generic<1, T>::host_type Generic<1, T>::getValue(int index) const
{
  VariantTypePtr c = variant_cast<T, HOST>(getVariant(index)); 
  return (*c)[0];
}

template<typename T>
void Generic<1, T>::setValue(int index, const host_type &variantValue) 
{
  VariantTypePtr variant(new VariantType(1, variantValue));
  setVariant(variant, false, Swizzle(1, index));
}

template<typename T>
void Generic<1, T>::setValues(const host_type variantValues[]) 
{
  setVariant(new VariantType(1, variantValues[0]));
}

}

#endif
