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
#ifndef SHVARIANT_IMPL_HPP
#define SHVARIANT_IMPL_HPP

#include <algorithm>
#include <sstream>
#include "ShDebug.hpp"
#include "ShCastManager.hpp"
#include "ShVariant.hpp"
#include "ShContext.hpp"
#include "ShTypeInfo.hpp"
#include "ShInterval.hpp"

namespace SH {

template<typename T>
ShDataVariant<T>::ShDataVariant(int N)
  : m_data(N, 0)
{
}

template<typename T>
ShDataVariant<T>::ShDataVariant(int N, const T &value)
  : m_data(N, value)
{
}

template<typename T>
ShDataVariant<T>::ShDataVariant(const ShDataVariant<T> &other)
  : m_data(other.m_data)
{
}

template<typename T>
ShDataVariant<T>::ShDataVariant(const ShDataVariant<T> &other, 
    bool neg, const ShSwizzle &swizzle)
  : m_data(swizzle.size())
{
  for(int i = 0; i < swizzle.size(); ++i) {
    m_data[i] = (neg ? -other[swizzle[i]] : other[swizzle[i]]);
  }
}

template<typename T>
int ShDataVariant<T>::typeIndex() const {
  return shTypeIndex<T>(); 
}


template<typename T>
std::string ShDataVariant<T>::typeName() const {
  return ShConcreteTypeInfo<T>::m_name; 
}

template<typename T>
ShDataVariant<T>::ShDataVariant(std::string s)
{
   decode(s);
}

template<typename T>
int ShDataVariant<T>::size() const
{
  return m_data.size();
}

template<typename T>
void ShDataVariant<T>::negate()
{
  transform(m_data.begin(), m_data.end(), m_data.begin(), std::negate<T>());
}

template<typename T>
void ShDataVariant<T>::set(ShVariantCPtr other)
{
  CPtrType castOther = shref_dynamic_cast<const ShDataVariant<T> >(
      ShCastManager::instance()->doCast(shTypeIndex<T>(), other));
//  CPtrType castOther = shref_dynamic_cast<const ShDataVariant<T> >(other);
  if(!castOther) { 
    // TODO throw some kind of exception
    SH_DEBUG_WARN("Cannot cast " << other->typeName() << " to " << typeName());
  } else {
    SH_DEBUG_ASSERT(other->size() == size());
    std::copy(castOther->data().begin(), castOther->data().end(), m_data.begin());
  }
}

template<typename T>
void ShDataVariant<T>::set(ShVariantCPtr other, int index)
{
  CPtrType castOther = shref_dynamic_cast<const ShDataVariant<T> >(
      ShCastManager::instance()->doCast(shTypeIndex<T>(), other));
  //CPtrType castOther = shref_dynamic_cast<const ShDataVariant<T> >(other);
  if(!castOther) { 
    // TODO throw some kind of exception
    SH_DEBUG_WARN("Cannot cast " << other->typeName() << " to " << typeName());
  } else {
    SH_DEBUG_ASSERT(index >= 0 && index < size() && castOther->size() > 0); 
    m_data[index] = (*castOther)[0];
  }
}

template<typename T>
void ShDataVariant<T>::set(ShVariantCPtr other, bool neg, const ShSwizzle &writemask) 
{
  CPtrType castOther = shref_dynamic_cast<const ShDataVariant<T> >(
      ShCastManager::instance()->doCast(shTypeIndex<T>(), other));
  //CPtrType castOther = shref_dynamic_cast<const ShDataVariant<T> >(other);
  if(!castOther) { 
    // TODO throw some kind of exception
    SH_DEBUG_WARN("Cannot cast " << other->typeName() << " to " << typeName());
  } else {
    for(int i = 0; i < writemask.size(); ++i) {
      m_data[writemask[i]] = neg ? -(*castOther)[i] : (*castOther)[i];
    }
  }
}

template<typename T>
ShVariantPtr ShDataVariant<T>::get() const
{
  return new ShDataVariant<T>(*this);
}

template<typename T>
ShVariantPtr ShDataVariant<T>::get(int index) const
{
  return new ShDataVariant<T>(1, m_data[index]);
}

template<typename T>
ShVariantPtr ShDataVariant<T>::get(bool neg, const ShSwizzle &swizzle) const 
{
  return new ShDataVariant<T>(*this, neg, swizzle);
}



template<typename T>
bool ShDataVariant<T>::equals(ShVariantCPtr other) const 
{
  if(!other || size() != other->size() || typeIndex() != other->typeIndex()) return false;
  CPtrType castOther = shref_dynamic_cast<const ShDataVariant<T> >(other);
  const_iterator I, J;
  I = begin();
  J = castOther->begin(); 
  for(;I != end(); ++I, ++J) {
    if(!ShConcreteTypeInfo<T>::valuesEqual((*I), (*J))) return false;
  }
  return true;
}

template<typename T>
bool ShDataVariant<T>::isTrue() const 
{
  for(const_iterator I = begin(); I != end(); ++I) {
    if(!ShConcreteTypeInfo<T>::valuesEqual((*I), ShConcreteTypeInfo<T>::TrueVal)) return false;
  }
  return true;
}

template<typename T>
T& ShDataVariant<T>::operator[](int index) 
{
  return m_data[index];
}

template<typename T>
const T& ShDataVariant<T>::operator[](int index) const
{
  return m_data[index];
}

template<typename T>
std::vector<T>& ShDataVariant<T>::data() {
  return m_data;
}

template<typename T>
const std::vector<T>& ShDataVariant<T>::data() const {
  return m_data;
}

template<typename T>
typename std::vector<T>::iterator ShDataVariant<T>::begin() {
  return m_data.begin();
}

template<typename T>
typename std::vector<T>::iterator ShDataVariant<T>::end() {
  return m_data.end();
}

template<typename T>
typename std::vector<T>::const_iterator ShDataVariant<T>::begin() const {
  return m_data.begin();
}

template<typename T>
typename std::vector<T>::const_iterator ShDataVariant<T>::end() const {
  return m_data.end();
}

template<typename T>
std::string ShDataVariant<T>::encode() const {
  if(size() < 1) return "";

  std::ostringstream out;
  out << m_data.size();
  for(int i = 0; i < (int)m_data.size(); ++i) {
    out << "," << m_data[i];
  }
  return out.str();
}

template<typename T>
int ShDataVariant<T>::decode(std::string value) {
  m_data.clear();

  std::istringstream in(value);

  int size;
  in >> size;
  m_data.reserve(size);

  T component; 
  for(int i = 0; i < size; ++i) {
    in.ignore(1, '$');
    in >> component;
    m_data.push_back(component);
  }
  return size;
}

template<typename T>
ShPointer<ShDataVariant<T> > variant_cast(ShVariantPtr c)
{
  return shref_dynamic_cast<ShDataVariant<T> >(c);
}

template<typename T>
ShPointer<const ShDataVariant<T> > variant_cast(ShVariantCPtr c)
{
  return shref_dynamic_cast<const ShDataVariant<T> >(c);
}

}
#endif
