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
#include "ShError.hpp"
#include "ShCastManager.hpp"
#include "ShVariant.hpp"
#include "ShTypeInfo.hpp"
#include "ShInterval.hpp"

namespace SH {

#ifdef SH_USE_MEMORY_POOL
template<ShValueType V, ShDataType DT>
ShPool* ShDataVariant<V, DT>::m_pool = 0;
#endif

template<ShValueType V, ShDataType DT>
ShDataVariant<V, DT>::ShDataVariant(int N)
  : m_managed(true)
{
  alloc(N);
  std::fill(m_begin, m_end, ShDataTypeConstant<V, DT>::Zero);
}

template<ShValueType V, ShDataType DT>
ShDataVariant<V, DT>::ShDataVariant(int N, const DataType &value)
  : m_managed(true)
{
  alloc(N);
  std::fill(m_begin, m_end, value); 
}

template<ShValueType V, ShDataType DT>
ShDataVariant<V, DT>::ShDataVariant(std::string encodedValue) 
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

template<ShValueType V, ShDataType DT>
ShDataVariant<V, DT>::ShDataVariant(void *data, int N, bool managed)
  : m_managed(managed)
{
  if(m_managed) {
    alloc(N);
    memcpy(m_begin, data, N * datasize());
  } else {
    m_begin = reinterpret_cast<DataType*>(data);
    m_end = m_begin + N;
  }
}

template<ShValueType V, ShDataType DT>
ShDataVariant<V, DT>::ShDataVariant(const ShDataVariant<V, DT> &other)
  : m_managed(true)
{
  int size = other.size();
  alloc(size);
  memcpy(m_begin, other.m_begin, size * datasize());
}

template<ShValueType V, ShDataType DT>
ShDataVariant<V, DT>::ShDataVariant(const ShDataVariant<V, DT> &other, 
    bool neg, const ShSwizzle &swizzle)
  : m_managed(true)
{
  alloc(swizzle.size());
  for(int i = 0; i < swizzle.size(); ++i) {
    m_begin[i] = other[swizzle[i]];
  }
  if(neg) negate();
}

template<ShValueType V, ShDataType DT>
ShDataVariant<V, DT>::~ShDataVariant() 
{
  if(m_managed) delete m_begin;
}

template<ShValueType V, ShDataType DT>
ShValueType ShDataVariant<V, DT>::valueType() const {
  return V; 
}

template<ShValueType V, ShDataType DT>
ShDataType ShDataVariant<V, DT>::dataType() const {
  return DT; 
}

template<ShValueType V, ShDataType DT>
bool ShDataVariant<V, DT>::typeMatches(
    ShValueType valueType, ShDataType dataType) const {
  return (valueType == V) && (dataType == DT);
}


template<ShValueType V, ShDataType DT>
std::string ShDataVariant<V, DT>::typeName() const {
  return valueTypeName[V]; 
}

template<ShValueType V, ShDataType DT>
int ShDataVariant<V, DT>::size() const
{
  return m_end - m_begin; 
}

template<ShValueType V, ShDataType DT>
int ShDataVariant<V, DT>::datasize() const
{
  return sizeof(DataType); 
}

template<ShValueType V, ShDataType DT>
bool ShDataVariant<V, DT>::managed() const
{
  return m_managed; 
}

template<ShValueType V, ShDataType DT>
void ShDataVariant<V, DT>::negate()
{
  transform(m_begin, m_end, m_begin, std::negate<DataType>());
}

template<ShValueType V, ShDataType DT>
void ShDataVariant<V, DT>::set(const ShVariant* other)
{
  SH_DEBUG_ASSERT(other->size() == size());
  ShCastManager::instance()->doCast(this, other);
}

template<ShValueType V, ShDataType DT>
void ShDataVariant<V, DT>::set(ShVariantCPtr other)
{
  set(other.object());
}

template<ShValueType V, ShDataType DT>
void ShDataVariant<V, DT>::set(const ShVariant* other, int index)
{
  SH_DEBUG_ASSERT(other->size() == 1); 
  // make a new DataVariant that uses the index element as it's array 
  ShDataVariant *temp = new ShDataVariant(m_begin + index, 1, false);
  ShCastManager::instance()->doCast(temp, other);
  delete temp; // okay - it doesn't delete its array
}

template<ShValueType V, ShDataType DT>
void ShDataVariant<V, DT>::set(ShVariantCPtr other, int index)
{
  set(other.object(), index);
}

template<ShValueType V, ShDataType DT>
void ShDataVariant<V, DT>::set(const ShVariant* other, bool neg, const ShSwizzle &writemask) 
{
  int wmsize = writemask.size();
  SH_DEBUG_ASSERT(wmsize == other->size());
  if(writemask.identity() && (wmsize == size())) {
    set(other);
    if(neg) negate();
    return;
  }

  // otherwise we need a temp buffer variant...doh
  ShDataVariant *temp = new ShDataVariant(wmsize);
  ShCastManager::instance()->doCast(temp, other);
  for(int i = 0; i < wmsize; ++i) {
    m_begin[writemask[i]] = neg ? -(*temp)[i] : (*temp)[i];
  }
  delete temp;
}

template<ShValueType V, ShDataType DT>
void ShDataVariant<V, DT>::set(ShVariantCPtr other, bool neg, const ShSwizzle &writemask) 
{
  set(other.object(), neg, writemask);
}

template<ShValueType V, ShDataType DT>
ShVariantPtr ShDataVariant<V, DT>::get() const
{
  return new ShDataVariant<V, DT>(*this);
}

template<ShValueType V, ShDataType DT>
ShVariantPtr ShDataVariant<V, DT>::get(int index) const
{
  return new ShDataVariant<V, DT>(1, m_begin[index]);
}

template<ShValueType V, ShDataType DT>
ShVariantPtr ShDataVariant<V, DT>::get(bool neg, const ShSwizzle &swizzle) const 
{
  return new ShDataVariant<V, DT>(*this, neg, swizzle);
}


template<ShValueType V, ShDataType DT>
bool ShDataVariant<V, DT>::equals(const ShVariant* other) const 
{
  if(!other || (size() != other->size()) 
      || !other->typeMatches(valueType(), dataType())) return false;

  const ShDataVariant* castOther = variant_cast<V, DT>(other);
  const_iterator I, J;
  I = m_begin; 
  J = castOther->begin(); 
  for(;I != m_end; ++I, ++J) {
    if(!shDataTypeEqual((*I), (*J))) return false;
  }
  return true;
}

template<ShValueType V, ShDataType DT>
bool ShDataVariant<V, DT>::equals(ShVariantCPtr other) const 
{
  return equals(other.object());
}

template<ShValueType V, ShDataType DT>
bool ShDataVariant<V, DT>::isTrue() const 
{
  for(const_iterator I = begin(); I != end(); ++I) {
    if(!shDataTypeIsPositive((*I))) return false;
  }
  return true;
}

template<ShValueType V, ShDataType DT>
void* ShDataVariant<V, DT>::array()
{
  return m_begin;
}

template<ShValueType V, ShDataType DT>
const void* ShDataVariant<V, DT>::array() const
{
  return m_begin;
}

template<ShValueType V, ShDataType DT>
typename ShDataVariant<V, DT>::DataType& ShDataVariant<V, DT>::operator[](int index) 
{
  return m_begin[index];
}

template<ShValueType V, ShDataType DT>
const typename ShDataVariant<V, DT>::DataType& ShDataVariant<V, DT>::operator[](int index) const
{
  return m_begin[index];
}

template<ShValueType V, ShDataType DT>
typename ShDataVariant<V, DT>::iterator ShDataVariant<V, DT>::begin() {
  return m_begin;
}

template<ShValueType V, ShDataType DT>
typename ShDataVariant<V, DT>::iterator ShDataVariant<V, DT>::end() {
  return m_end;
}

template<ShValueType V, ShDataType DT>
typename ShDataVariant<V, DT>::const_iterator ShDataVariant<V, DT>::begin() const {
  return m_begin;
}

template<ShValueType V, ShDataType DT>
typename ShDataVariant<V, DT>::const_iterator ShDataVariant<V, DT>::end() const {
  return m_end;
}

template<ShValueType V, ShDataType DT>
std::string ShDataVariant<V, DT>::encode() const {
  if(size() < 1) return "";

  std::ostringstream out;
  out << size(); 
  for(const_iterator I = m_begin; I != m_end; ++I) {
    out << "," << *I;
  }
  return out.str();
}
template<ShValueType V, ShDataType DT>
std::string ShDataVariant<V, DT>::encode(int index, int repeats) const 
{
  std::ostringstream out;
  out << repeats;
  for(int i = 0; i < repeats; ++i) {
    out << ", " << m_begin[index];
  }
  return out.str();
}

template<ShValueType V, ShDataType DT>
std::string ShDataVariant<V, DT>::encode(bool neg, const ShSwizzle &swizzle) const 
{
  std::ostringstream out;
  out << swizzle.size(); 
  for(int i = 0; i < swizzle.size(); ++i) {
    out << ", " << m_begin[swizzle[i]];
  }
  return out.str();
}

// @todo type do Interval types
template<ShValueType V, ShDataType DT>
std::string ShDataVariant<V, DT>::encodeArray() const {
  if(size() < 1) return "";

  std::ostringstream out;
  for(const_iterator I = m_begin; I != m_end; ++I) {
    if(I != m_begin) out << ", ";
    out << *I;
  }
  return out.str();
}

template<ShValueType V, ShDataType DT>
void ShDataVariant<V, DT>::alloc(int N) {
  // SH_DEBUG_PRINT("alloc " << valueTypeName[V] << " " << dataTypeName[DT]);
  m_begin = new DataType[N];
  m_end = m_begin + N;
}

#ifdef SH_USE_MEMORY_POOL
template<ShValueType V, ShDataType DT>
void* ShDataVariant<V, DT>::operator new(std::size_t size)
{
  if (size != sizeof(ShDataVariant)) return ::operator new(size);
  if (!m_pool) {
    std::cout << "made new pool " << valueTypeName[V] << " " << dataTypeName[DT] << " " << size << " " << sizeof(ShDataVariant) << std::endl;
    m_pool = new ShPool(sizeof(ShDataVariant), 32768);
  }
  return m_pool->alloc();
}

template<ShValueType V, ShDataType DT>
void ShDataVariant<V, DT>::operator delete(void* ptr, std::size_t size)
{
  if(size != sizeof(ShDataVariant)) {
    SH_DEBUG_PRINT("delete size does not match " << size << " " << sizeof(ShDataVariant));
  }
  if(!m_pool) shError( ShException( "Deleting from memory pool without an m_pool." ) );
  m_pool->free(ptr);
}
#endif

template<ShValueType V, ShDataType DT>
ShPointer<ShDataVariant<V, DT> > variant_cast(ShVariantPtr c)
{
  return shref_dynamic_cast<ShDataVariant<V, DT> >(c);
}

template<ShValueType V, ShDataType DT>
ShPointer<const ShDataVariant<V, DT> > variant_cast(ShVariantCPtr c)
{
  return shref_dynamic_cast<const ShDataVariant<V, DT> >(c);
}

template<ShValueType V, ShDataType DT>
ShDataVariant<V, DT>* variant_cast(ShVariant* c)
{
  return dynamic_cast<ShDataVariant<V, DT>*>(c);
}

template<ShValueType V, ShDataType DT>
const ShDataVariant<V, DT>* variant_cast(const ShVariant* c)
{
  return dynamic_cast<const ShDataVariant<V, DT>*>(c);
}

template<ShValueType V, ShDataType DT>
ShPointer<ShDataVariant<V, DT> > variant_convert(ShVariantCPtr c)
{
  ShDataVariant<V, DT>* result = new ShDataVariant<V, DT>(c->size());
  ShCastManager::instance()->doCast(result, c.object());
  return result;
}

}
#endif
