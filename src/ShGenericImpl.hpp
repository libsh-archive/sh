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
#include "ShDebug.hpp"
#include "ShProgram.hpp"

namespace SH {

template<int N, ShValueType V>
inline
ShGeneric<N, V>::ShGeneric(const ShVariableNodePtr& node)
  : ShVariable(node)
{
  SH_DEBUG_ASSERT(node); // DEBUG
}

template<int N, ShValueType V>
inline
ShGeneric<N, V>::ShGeneric(const ShVariableNodePtr& node, ShSwizzle swizzle, bool neg)
  : ShVariable(node)
{
  m_swizzle = swizzle;
  m_neg = neg;
  SH_DEBUG_ASSERT(node); // DEBUG
}

template<int N, ShValueType V>
inline
ShGeneric<N, V>::~ShGeneric()
{
}

//template<int N, ShValueType V>
//ShGeneric<N, V>::ShGeneric(const ShGeneric<N, V>& other)
//  : ShVariable(new ShVariableNode(SH_TEMP, N, V, 
//        other.node()->specialType()))
//{
//  SH_DEBUG_ASSERT(other.node());
//  SH_DEBUG_ASSERT(m_node);
//  shASN(*this, other);
//}

template<int N, ShValueType V>
template<ShValueType V2>
inline
ShGeneric<N, V>::ShGeneric(const ShGeneric<N, V2>& other)
  : ShVariable(new ShVariableNode(SH_TEMP, N, V, 
        other.node()->specialType()))
{
  SH_DEBUG_ASSERT(other.node());
  SH_DEBUG_ASSERT(m_node);
  shASN(*this, other);
}

template<int N, ShValueType V>
inline
ShGeneric<N, V>& ShGeneric<N, V>::operator=(const ShProgram& prg)
{
  this->ShVariable::operator=(prg);
  return *this;
}

template<int N, ShValueType V>
inline
ShGeneric<N, V>& ShGeneric<N, V>::operator=(const ShGeneric<N, V>& other)
{
  shASN(*this, other);
  return *this;
}

template<int N, ShValueType V>
template<ShValueType V2>
ShGeneric<N, V>& ShGeneric<N, V>::operator=(const ShGeneric<N, V2>& other)
{
  shASN(*this, other);
  return *this;
}

template<int N, ShValueType V>
template<ShValueType V2>
inline
ShGeneric<N, V>& ShGeneric<N, V>::operator+=(const ShGeneric<N, V2>& right)
{
  *this = *this + right;
  return *this;
}

template<int N, ShValueType V>
template<ShValueType V2>
inline
ShGeneric<N, V>& ShGeneric<N, V>::operator-=(const ShGeneric<N, V2>& right)
{
  *this = *this - right;
  return *this;
}

template<int N, ShValueType V>
template<ShValueType V2>
inline
ShGeneric<N, V>& ShGeneric<N, V>::operator*=(const ShGeneric<N, V2>& right)
{
  *this = *this * right;
  return *this;
}

template<int N, ShValueType V>
template<ShValueType V2>
inline
ShGeneric<N, V>& ShGeneric<N, V>::operator/=(const ShGeneric<N, V2>& right)
{
  *this = *this / right;
  return *this;
}

template<int N, ShValueType V>
template<ShValueType V2>
inline
ShGeneric<N, V>& ShGeneric<N, V>::operator%=(const ShGeneric<N, V2>& right)
{
  *this = *this % right;
  return *this;
}

template<int N, ShValueType V>
template<ShValueType V2>
inline
ShGeneric<N, V>& ShGeneric<N, V>::operator+=(const ShGeneric<1, V2>& right)
{
  *this = *this + right;
  return *this;
}

template<int N, ShValueType V>
template<ShValueType V2>
inline
ShGeneric<N, V>& ShGeneric<N, V>::operator-=(const ShGeneric<1, V2>& right)
{
  *this = *this - right;
  return *this;
}

template<int N, ShValueType V>
template<ShValueType V2>
inline
ShGeneric<N, V>& ShGeneric<N, V>::operator*=(const ShGeneric<1, V2>& right)
{
  *this = *this * right;
  return *this;
}

template<int N, ShValueType V>
template<ShValueType V2>
inline
ShGeneric<N, V>& ShGeneric<N, V>::operator/=(const ShGeneric<1, V2>& right)
{
  *this = *this / right;
  return *this;
}

template<int N, ShValueType V>
template<ShValueType V2>
inline
ShGeneric<N, V>& ShGeneric<N, V>::operator%=(const ShGeneric<1, V2>& right)
{
  *this = *this % right;
  return *this;
}

template<int N, ShValueType V>
inline
ShGeneric<N, V>& ShGeneric<N, V>::operator+=(H right)
{
  *this = *this + right;
  return *this;
}

template<int N, ShValueType V>
inline
ShGeneric<N, V>& ShGeneric<N, V>::operator-=(H right)
{
  *this = *this - right;
  return *this;
}

template<int N, ShValueType V>
inline
ShGeneric<N, V>& ShGeneric<N, V>::operator*=(H right)
{
  *this = *this * right;
  return *this;
}

template<int N, ShValueType V>
inline
ShGeneric<N, V>& ShGeneric<N, V>::operator/=(H right)
{
  *this = *this / right;
  return *this;
}

template<int N, ShValueType V>
inline
ShGeneric<N, V>& ShGeneric<N, V>::operator%=(H right)
{
  *this = *this % right;
  return *this;
}

template<int N, ShValueType V>
inline
ShGeneric<N, V> ShGeneric<N, V>::operator-() const
{
  return ShGeneric<N, V>(m_node, m_swizzle, !m_neg);
}


template<int N, ShValueType V>
inline
ShGeneric<N, V> ShGeneric<N, V>::operator()() const
{
  return ShGeneric<N, V>(m_node, m_swizzle, m_neg);
}

template<int N, ShValueType V>
inline
ShGeneric<1, V> ShGeneric<N, V>::operator()(int i1) const
{
  return ShGeneric<1, V>(m_node, m_swizzle * ShSwizzle(size(), i1), m_neg);
}

template<int N, ShValueType V>
inline
ShGeneric<1, V> ShGeneric<N, V>::operator[](int i1) const
{
  return ShGeneric<1, V>(m_node, m_swizzle * ShSwizzle(size(), i1), m_neg);
}

template<int N, ShValueType V>
inline
ShGeneric<2, V> ShGeneric<N, V>::operator()(int i1, int i2) const
{
  return ShGeneric<2, V>(m_node, m_swizzle * ShSwizzle(size(), i1, i2), m_neg);
}

template<int N, ShValueType V>
inline
ShGeneric<3, V> ShGeneric<N, V>::operator()(int i1, int i2, int i3) const
{
  return ShGeneric<3, V>(m_node, m_swizzle * ShSwizzle(size(), i1, i2, i3), m_neg);
}

template<int N, ShValueType V>
inline
ShGeneric<4, V> ShGeneric<N, V>::operator()(int i1, int i2, int i3, int i4) const
{
  return ShGeneric<4, V>(m_node, m_swizzle * ShSwizzle(size(), i1, i2, i3, i4), m_neg);
}

template<int N, ShValueType V>
void ShGeneric<N, V>::range(H low, H high) 
{
  rangeVariant(new VariantType(1, low), new VariantType(1, high));
}

template<int N, ShValueType V>
typename ShGeneric<N, V>::VariantType ShGeneric<N, V>::lowBound() const
{
  return (*variant_cast<V, SH_HOST>(lowBoundVariant()));
}

template<int N, ShValueType V>
typename ShGeneric<N, V>::HostType ShGeneric<N, V>::lowBound(int index) const
{
  return (*variant_cast<V, SH_HOST>(lowBoundVariant()))[index];
}

template<int N, ShValueType V>
typename ShGeneric<N, V>::VariantType ShGeneric<N, V>::highBound() const
{
  return (*variant_cast<V, SH_HOST>(highBoundVariant()));
}

template<int N, ShValueType V>
typename ShGeneric<N, V>::H ShGeneric<N, V>::highBound(int index) const
{
  return (*variant_cast<V, SH_HOST>(highBoundVariant()))[index];
}
  
template<int N, ShValueType V> 
template<int N2>
ShGeneric<N2, V> ShGeneric<N, V>::swiz(int indices[]) const
{
  return ShGeneric<N2, V>(m_node, m_swizzle * ShSwizzle(N, N2, indices), m_neg);
}

template<int N, ShValueType V>
void ShGeneric<N, V>::getValues(H dest[]) const
{
  VariantTypePtr c = variant_cast<V, SH_HOST>(getVariant()); 
  for(int i = 0; i < N; ++i) dest[i] = (*c)[i]; 
}

template<int N, ShValueType V>
typename ShGeneric<N, V>::H ShGeneric<N, V>::getValue(int index) const
{
  VariantTypePtr c = variant_cast<V, SH_HOST>(getVariant(index)); 
  return (*c)[0];
}

template<int N, ShValueType V>
void ShGeneric<N, V>::setValue(int index, const H &variantValue) 
{
  VariantTypePtr variant(new VariantType(1, variantValue));
  setVariant(variant, false, ShSwizzle(N, index));
}

template<int N, ShValueType V>
void ShGeneric<N, V>::setValues(const H variantValues[]) 
{
  VariantTypePtr variantPtr(new VariantType(N));
  for(int i = 0; i < N; ++i) {
    (*variantPtr)[i] = variantValues[i]; 
  }
  setVariant(variantPtr);
}

template<ShValueType V>
inline
ShGeneric<1, V>::ShGeneric(const ShVariableNodePtr& node)
  : ShVariable(node)
{
  SH_DEBUG_ASSERT(node); // DEBUG
}

template<ShValueType V>
inline
ShGeneric<1, V>::ShGeneric(const ShVariableNodePtr& node, ShSwizzle swizzle, bool neg)
  : ShVariable(node)
{
  m_swizzle = swizzle;
  m_neg = neg;
  SH_DEBUG_ASSERT(node); // DEBUG
}

template<ShValueType V>
inline
ShGeneric<1, V>::~ShGeneric()
{
}

//template<ShValueType V>
//ShGeneric<1, V>::ShGeneric(const ShGeneric<1, V>& other)
//  : ShVariable(new ShVariableNode(SH_TEMP, 1, V, 
//        other.node()->specialType()))
//{
//  SH_DEBUG_ASSERT(other.node());
//  SH_DEBUG_ASSERT(m_node);
//  SH_DEBUG_PRINT(m_node->size() << " " << other.node()->size());
//  shASN(*this, other);
//}

template<ShValueType V>
template<ShValueType V2>
inline
ShGeneric<1, V>::ShGeneric(const ShGeneric<1, V2>& other)
  : ShVariable(new ShVariableNode(SH_TEMP, 1, V, other.node()->specialType()))
{
  SH_DEBUG_ASSERT(other.node());
  SH_DEBUG_ASSERT(m_node);
  SH_DEBUG_PRINT(m_node->size() << " " << other.node()->size());
  shASN(*this, other);
}

template<ShValueType V>
inline
ShGeneric<1, V>& ShGeneric<1, V>::operator=(const ShProgram& prg)
{
  this->ShVariable::operator=(prg);
  return *this;
}

template<ShValueType V>
inline
ShGeneric<1, V>& ShGeneric<1, V>::operator=(const ShGeneric<1, V>& other)
{
  shASN(*this, other);
  return *this;
}

template<ShValueType V>
template<ShValueType V2>
inline
ShGeneric<1, V>& ShGeneric<1, V>::operator=(const ShGeneric<1, V2>& other)
{
  shASN(*this, other);
  return *this;
}


template<ShValueType V>
inline
ShGeneric<1, V>& ShGeneric<1, V>::operator=(H other)
{
  shASN(*this, ShAttrib<1, SH_CONST, V>(other));
  return *this;
}

template<ShValueType V>
template<ShValueType V2>
inline
ShGeneric<1, V>& ShGeneric<1, V>::operator+=(const ShGeneric<1, V2>& right)
{
  *this = *this + right;
  return *this;
}

template<ShValueType V>
template<ShValueType V2>
inline
ShGeneric<1, V>& ShGeneric<1, V>::operator-=(const ShGeneric<1, V2>& right)
{
  *this = *this - right;
  return *this;
}

template<ShValueType V>
template<ShValueType V2>
inline
ShGeneric<1, V>& ShGeneric<1, V>::operator*=(const ShGeneric<1, V2>& right)
{
  *this = *this * right;
  return *this;
}

template<ShValueType V>
template<ShValueType V2>
inline
ShGeneric<1, V>& ShGeneric<1, V>::operator/=(const ShGeneric<1, V2>& right)
{
  *this = *this / right;
  return *this;
}

template<ShValueType V>
template<ShValueType V2>
inline
ShGeneric<1, V>& ShGeneric<1, V>::operator%=(const ShGeneric<1, V2>& right)
{
  *this = *this % right;
  return *this;
}

template<ShValueType V>
inline
ShGeneric<1, V>& ShGeneric<1, V>::operator*=(H right)
{
  *this = *this * right;
  return *this;
}

template<ShValueType V>
inline
ShGeneric<1, V>& ShGeneric<1, V>::operator/=(H right)
{
  *this = *this / right;
  return *this;
}

template<ShValueType V>
inline
ShGeneric<1, V>& ShGeneric<1, V>::operator%=(H right)
{
  *this = *this % right;
  return *this;
}

template<ShValueType V>
inline
ShGeneric<1, V>& ShGeneric<1, V>::operator+=(H right)
{
  *this = *this + right;
  return *this;
}

template<ShValueType V>
inline
ShGeneric<1, V>& ShGeneric<1, V>::operator-=(H right)
{
  *this = *this - right;
  return *this;
}

template<ShValueType V>
inline
ShGeneric<1, V> ShGeneric<1, V>::operator-() const
{
  return ShGeneric<1, V>(m_node, m_swizzle, !m_neg);
}


template<ShValueType V>
inline
ShGeneric<1, V> ShGeneric<1, V>::operator()() const
{
  return ShGeneric<1, V>(m_node, m_swizzle, m_neg);
}

template<ShValueType V>
inline
ShGeneric<1, V> ShGeneric<1, V>::operator()(int i1) const
{
  return ShGeneric<1, V>(m_node, m_swizzle * ShSwizzle(size(), i1), m_neg);
}

template<ShValueType V>
inline
ShGeneric<1, V> ShGeneric<1, V>::operator[](int i1) const
{
  return ShGeneric<1, V>(m_node, m_swizzle * ShSwizzle(size(), i1), m_neg);
}

template<ShValueType V>
inline
ShGeneric<2, V> ShGeneric<1, V>::operator()(int i1, int i2) const
{
  return ShGeneric<2, V>(m_node, m_swizzle * ShSwizzle(size(), i1, i2), m_neg);
}

template<ShValueType V>
inline
ShGeneric<3, V> ShGeneric<1, V>::operator()(int i1, int i2, int i3) const
{
  return ShGeneric<3, V>(m_node, m_swizzle * ShSwizzle(size(), i1, i2, i3), m_neg);
}

template<ShValueType V>
inline
ShGeneric<4, V> ShGeneric<1, V>::operator()(int i1, int i2, int i3, int i4) const
{
  return ShGeneric<4, V>(m_node, m_swizzle * ShSwizzle(size(), i1, i2, i3, i4), m_neg);
}

template<ShValueType V>
void ShGeneric<1, V>::range(H low, H high) 
{
  rangeVariant(new VariantType(1, low), new VariantType(1, high));
}

template<ShValueType V>
typename ShGeneric<1, V>::VariantType ShGeneric<1, V>::lowBound() const
{
  return (*variant_cast<V, SH_HOST>(lowBoundVariant()));
}

template<ShValueType V>
typename ShGeneric<1, V>::H ShGeneric<1, V>::lowBound(int index) const
{
  return (*variant_cast<V, SH_HOST>(lowBoundVariant()))[index];
}

template<ShValueType V>
typename ShGeneric<1, V>::VariantType ShGeneric<1, V>::highBound() const
{
  return (*variant_cast<V, SH_HOST>(highBoundVariant()));
}

template<ShValueType V>
typename ShGeneric<1, V>::H ShGeneric<1, V>::highBound(int index) const
{
  return (*variant_cast<V, SH_HOST>(highBoundVariant()))[index];
}
  
template<ShValueType V> 
template<int N2>
ShGeneric<N2, V> ShGeneric<1, V>::swiz(int indices[]) const
{
  return ShGeneric<N2, V>(m_node, m_swizzle * ShSwizzle(1, N2, indices), m_neg);
}

template<ShValueType V>
void ShGeneric<1, V>::getValues(H dest[]) const
{
  VariantTypePtr c = variant_cast<V, SH_HOST>(getVariant()); 
  dest[0] = (*c)[0]; 
}

template<ShValueType V>
typename ShGeneric<1, V>::H ShGeneric<1, V>::getValue(int index) const
{
  VariantTypePtr c = variant_cast<V, SH_HOST>(getVariant(index)); 
  return (*c)[0];
}

template<ShValueType V>
void ShGeneric<1, V>::setValue(int index, const H &variantValue) 
{
  VariantTypePtr variant(new VariantType(1, variantValue));
  setVariant(variant, false, ShSwizzle(1, index));
}

template<ShValueType V>
void ShGeneric<1, V>::setValues(const H variantValues[]) 
{
  VariantTypePtr variant(new VariantType(1));
  for(int i = 0; i < N; ++i) {
    (*variant)[i] = variantValues[i]; 
  }
  setVariant(new VariantType(1, variantValues[0]));
}

}

#endif
