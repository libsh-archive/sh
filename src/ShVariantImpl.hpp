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
#include "ShTypeInfo.hpp"
#include "ShInterval.hpp"

namespace SH {

template<typename T>
ShDataVariant<T>::ShDataVariant(int N)
  : m_managed(true)
{
  alloc(N);
  std::fill(m_begin, m_end, ShConcreteTypeInfo<T>::ZERO);
}

template<typename T>
ShDataVariant<T>::ShDataVariant(int N, const T &value)
  : m_managed(true)
{
  alloc(N);
  std::fill(m_begin, m_end, value); 
}

template<typename T>
ShDataVariant<T>::ShDataVariant(std::string encodedValue) 
  : m_managed(true)
{
  std::istringstream in(encodedValue);

  int size;
  in >> size;
  alloc(size);

  for(iterator I = m_begin; I != m_end; ++I) {
    in.ignore(1, '$');
    in >> (*I); 
  }
}

template<typename T>
ShDataVariant<T>::ShDataVariant(void *data, int N, bool managed)
  : m_managed(managed)
{
  if(m_managed) {
    alloc(N);
    memcpy(m_begin, data, N * sizeof(T));
  } else {
    m_begin = reinterpret_cast<T*>(data);
    m_end = m_begin + N;
  }
}

template<typename T>
ShDataVariant<T>::ShDataVariant(const ShDataVariant<T> &other)
  : m_managed(true)
{
  int size = other.size();
  alloc(size);
  memcpy(m_begin, other.m_begin, size * sizeof(T));
}

template<typename T>
ShDataVariant<T>::ShDataVariant(const ShDataVariant<T> &other, 
    bool neg, const ShSwizzle &swizzle)
  : m_managed(true)
{
  alloc(swizzle.size());
  for(int i = 0; i < swizzle.size(); ++i) {
    m_begin[i] = (neg ? -other[swizzle[i]] : other[swizzle[i]]);
  }
}

template<typename T>
ShDataVariant<T>::~ShDataVariant() 
{
  if(m_managed) delete m_begin;
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
int ShDataVariant<T>::size() const
{
  return m_end - m_begin; 
}

template<typename T>
bool ShDataVariant<T>::managed() const
{
  return m_managed; 
}

template<typename T>
void ShDataVariant<T>::negate()
{
  transform(m_begin, m_end, m_begin, std::negate<T>());
}

template<typename T>
void ShDataVariant<T>::set(ShVariantCPtr other)
{
  SH_DEBUG_ASSERT(other->size() == size());
  ShCastManager* castmgr = ShCastManager::instance();
  CPtrType castOther = variant_cast<T>(castmgr->doCast(shTypeIndex<T>(), other));
//  CPtrType castOther = shref_dynamic_cast<const ShDataVariant<T> >(other);
  if(!castOther) { 
    // TODO throw some kind of exception
    SH_DEBUG_WARN("Cannot cast " << other->typeName() << " to " << typeName());
  } else {
    SH_DEBUG_ASSERT(other->size() == size());
    std::copy(castOther->begin(), castOther->end(), m_begin);
  }
}

template<typename T>
void ShDataVariant<T>::set(ShVariantCPtr other, int index)
{
  SH_DEBUG_ASSERT(other->size() == 1); 
  ShCastManager* castmgr = ShCastManager::instance();
  CPtrType castOther = variant_cast<T>(castmgr->doCast(shTypeIndex<T>(), other));
  //CPtrType castOther = shref_dynamic_cast<const ShDataVariant<T> >(other);
  if(!castOther) { 
    // TODO throw some kind of exception
    SH_DEBUG_WARN("Cannot cast " << other->typeName() << " to " << typeName());
  } else {
    SH_DEBUG_ASSERT(index >= 0 && index < size() && castOther->size() > 0); 
    m_begin[index] = (*castOther)[0];
  }
}

template<typename T>
void ShDataVariant<T>::set(ShVariantCPtr other, bool neg, const ShSwizzle &writemask) 
{
  SH_DEBUG_ASSERT(writemask.size() == other->size());
  ShCastManager* castmgr = ShCastManager::instance();
  CPtrType castOther = variant_cast<T>(castmgr->doCast(shTypeIndex<T>(), other));
  //CPtrType castOther = shref_dynamic_cast<const ShDataVariant<T> >(other);
  if(!castOther) { 
    // TODO throw some kind of exception
    SH_DEBUG_WARN("Cannot cast " << other->typeName() << " to " << typeName());
  } else {
    for(int i = 0; i < writemask.size(); ++i) {
      m_begin[writemask[i]] = neg ? -(*castOther)[i] : (*castOther)[i];
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
  return new ShDataVariant<T>(1, m_begin[index]);
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
  I = m_begin; 
  J = castOther->begin(); 
  for(;I != m_end; ++I, ++J) {
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
void* ShDataVariant<T>::array()
{
  return m_begin;
}

template<typename T>
const void* ShDataVariant<T>::array() const
{
  return m_begin;
}

template<typename T>
T& ShDataVariant<T>::operator[](int index) 
{
  return m_begin[index];
}

template<typename T>
const T& ShDataVariant<T>::operator[](int index) const
{
  return m_begin[index];
}

template<typename T>
typename ShDataVariant<T>::iterator ShDataVariant<T>::begin() {
  return m_begin;
}

template<typename T>
typename ShDataVariant<T>::iterator ShDataVariant<T>::end() {
  return m_end;
}

template<typename T>
typename ShDataVariant<T>::const_iterator ShDataVariant<T>::begin() const {
  return m_begin;
}

template<typename T>
typename ShDataVariant<T>::const_iterator ShDataVariant<T>::end() const {
  return m_end;
}

template<typename T>
std::string ShDataVariant<T>::encode() const {
  if(size() < 1) return "";

  std::ostringstream out;
  out << size(); 
  for(const_iterator I = m_begin; I != m_end; ++I) {
    out << "," << *I;
  }
  return out.str();
}

// @todo type do Interval types
template<typename T>
std::string ShDataVariant<T>::encodeArray() const {
  if(size() < 1) return "";

  std::ostringstream out;
  for(const_iterator I = m_begin; I != m_end; ++I) {
    if(I != m_begin) out << ", ";
    out << *I;
  }
  return out.str();
}


template<typename T>
void ShDataVariant<T>::alloc(int N) {
  m_begin = new T[N]; 
  m_end = m_begin + N;
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
