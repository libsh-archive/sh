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

//template<int N, typename T>
//ShGeneric<N, T>::ShGeneric(const ShGeneric<N, T>& other)
//  : ShVariable(new ShVariableNode(SH_TEMP, N, shTypeIndex<T>(), 
//        other.node()->specialType()))
//{
//  SH_DEBUG_ASSERT(other.node());
//  SH_DEBUG_ASSERT(m_node);
//  shASN(*this, other);
//}

template<int N, typename T>
template<typename T2>
ShGeneric<N, T>::ShGeneric(const ShGeneric<N, T2>& other)
  : ShVariable(new ShVariableNode(SH_TEMP, N, shTypeIndex<T>(), 
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
  *this = *this / right;
  return *this;
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator+=(T right)
{
  *this = *this + right;
  return *this;
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator-=(T right)
{
  *this = *this - right;
  return *this;
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator*=(T right)
{
  *this = *this * right;
  return *this;
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator/=(T right)
{
  *this = *this / right;
  return *this;
}

template<int N, typename T>
ShGeneric<N, T>& ShGeneric<N, T>::operator%=(T right)
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
void ShGeneric<N, T>::range(T low, T high) 
{
  rangeVariant(new ShDataVariant<T>(1, low), new ShDataVariant<T>(1, high));
}

template<int N, typename T>
ShDataVariant<T> ShGeneric<N, T>::lowBound() const
{
  return (*shref_dynamic_cast<VariantType>(lowBoundVariant()));
}

template<int N, typename T>
T ShGeneric<N, T>::lowBound(int index) const
{
  return (*shref_dynamic_cast<VariantType>(lowBoundVariant()))[index];
}

template<int N, typename T>
ShDataVariant<T> ShGeneric<N, T>::highBound() const
{
  return (*shref_dynamic_cast<VariantType>(highBoundVariant()));
}

template<int N, typename T>
T ShGeneric<N, T>::highBound(int index) const
{
  return (*shref_dynamic_cast<VariantType>(highBoundVariant()))[index];
}
  
template<int N, typename T> 
template<int N2>
ShGeneric<N2, T> ShGeneric<N, T>::swiz(int indices[]) const
{
  return ShGeneric<N2, T>(m_node, m_swizzle * ShSwizzle(N, N2, indices), m_neg);
}

template<int N, typename T>
void ShGeneric<N, T>::getValues(T dest[]) const
{
  VariantTypePtr c = shref_dynamic_cast<VariantType>(getVariant()); 
  for(int i = 0; i < N; ++i) dest[i] = (*c)[i]; 
}

template<int N, typename T>
T ShGeneric<N, T>::getValue(int index) const
{
  VariantTypePtr c = shref_dynamic_cast<VariantType>(getVariant(index)); 
  return (*c)[0];
}

template<int N, typename T>
void ShGeneric<N, T>::setValue(int index, const T &variantValue) 
{
  VariantTypePtr variant(new VariantType(1, variantValue));
  setVariant(variant, false, ShSwizzle(N, index));
}

template<int N, typename T>
void ShGeneric<N, T>::setValues(const T variantValues[]) 
{
  VariantTypePtr variantPtr(new VariantType(N));
  for(int i = 0; i < N; ++i) {
    (*variantPtr)[i] = variantValues[i]; 
  }
  setVariant(variantPtr);
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

//template<typename T>
//ShGeneric<1, T>::ShGeneric(const ShGeneric<1, T>& other)
//  : ShVariable(new ShVariableNode(SH_TEMP, 1, shTypeIndex<T>(), 
//        other.node()->specialType()))
//{
//  SH_DEBUG_ASSERT(other.node());
//  SH_DEBUG_ASSERT(m_node);
//  SH_DEBUG_PRINT(m_node->size() << " " << other.node()->size());
//  shASN(*this, other);
//}

template<typename T>
template<typename T2>
ShGeneric<1, T>::ShGeneric(const ShGeneric<1, T2>& other)
  : ShVariable(new ShVariableNode(SH_TEMP, 1, shTypeIndex<T>(), 
        other.node()->specialType()))
{
  SH_DEBUG_ASSERT(other.node());
  SH_DEBUG_ASSERT(m_node);
  SH_DEBUG_PRINT(m_node->size() << " " << other.node()->size());
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
ShGeneric<1, T>& ShGeneric<1, T>::operator=(T other)
{
  shASN(*this, ShAttrib<1, SH_TEMP, T>(other));
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
ShGeneric<1, T>& ShGeneric<1, T>::operator*=(T right)
{
  *this = *this * right;
  return *this;
}

template<typename T>
ShGeneric<1, T>& ShGeneric<1, T>::operator/=(T right)
{
  *this = *this / right;
  return *this;
}

template<typename T>
ShGeneric<1, T>& ShGeneric<1, T>::operator%=(T right)
{
  *this = *this % right;
  return *this;
}

template<typename T>
ShGeneric<1, T>& ShGeneric<1, T>::operator+=(T right)
{
  *this = *this + right;
  return *this;
}

template<typename T>
ShGeneric<1, T>& ShGeneric<1, T>::operator-=(T right)
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
void ShGeneric<1, T>::range(T low, T high) 
{
  rangeVariant(new ShDataVariant<T>(1, low), new ShDataVariant<T>(1, high));
}

template<typename T>
ShDataVariant<T> ShGeneric<1, T>::lowBound() const
{
  return (*shref_dynamic_cast<VariantType>(lowBoundVariant()));
}

template<typename T>
T ShGeneric<1, T>::lowBound(int index) const
{
  return (*shref_dynamic_cast<VariantType>(lowBoundVariant()))[index];
}

template<typename T>
ShDataVariant<T> ShGeneric<1, T>::highBound() const
{
  return (*shref_dynamic_cast<VariantType>(highBoundVariant()));
}

template<typename T>
T ShGeneric<1, T>::highBound(int index) const
{
  return (*shref_dynamic_cast<VariantType>(highBoundVariant()))[index];
}
  
template<typename T> 
template<int N2>
ShGeneric<N2, T> ShGeneric<1, T>::swiz(int indices[]) const
{
  return ShGeneric<N2, T>(m_node, m_swizzle * ShSwizzle(1, N2, indices), m_neg);
}

template<typename T>
void ShGeneric<1, T>::getValues(T dest[]) const
{
  VariantTypePtr c = shref_dynamic_cast<VariantType>(getVariant()); 
  dest[0] = (*c)[0]; 
}

template<typename T>
T ShGeneric<1, T>::getValue(int index) const
{
  VariantTypePtr c = shref_dynamic_cast<VariantType>(getVariant(index)); 
  return (*c)[0];
}

template<typename T>
void ShGeneric<1, T>::setValue(int index, const T &variantValue) 
{
  VariantTypePtr variant(new VariantType(1, variantValue));
  setVariant(variant, false, ShSwizzle(1, index));
}

template<typename T>
void ShGeneric<1, T>::setValues(const T variantValues[]) 
{
  VariantTypePtr variant(new VariantType(1));
  for(int i = 0; i < N; ++i) {
    (*variant)[i] = variantValues[i]; 
  }
  setVariant(new VariantType(1, variantValues[0]));
}

}

#endif
