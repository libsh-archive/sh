// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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

namespace SH {

#ifdef SH_USE_MEMORY_POOL
template<typename T, ShDataType DT>
ShPool* ShDataVariant<T, DT>::m_pool = 0;
#endif

template<typename T, ShDataType DT>
ShDataVariant<T, DT>::ShDataVariant()
  : m_managed(false)
{
  alloc(0);
}

template<typename T, ShDataType DT>
ShDataVariant<T, DT>::ShDataVariant(int N)
  : m_managed(true)
{
  alloc(N);
  std::fill(m_begin, m_end, ShDataTypeConstant<T, DT>::Zero);
}

template<typename T, ShDataType DT>
ShDataVariant<T, DT>::ShDataVariant(int N, const DataType &value)
  : m_managed(true)
{
  alloc(N);
  std::fill(m_begin, m_end, value); 
}

template<typename T, ShDataType DT>
ShDataVariant<T, DT>::ShDataVariant(std::string encodedValue) 
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

template<typename T, ShDataType DT>
ShDataVariant<T, DT>::ShDataVariant(void *data, int N, bool managed)
  : m_managed(managed)
{
  if(m_managed) {
    alloc(N);
    DataType* cast_data = reinterpret_cast<DataType*>(data); 
    std::copy(cast_data, cast_data + N, m_begin);
  } else {
    m_begin = reinterpret_cast<DataType*>(data);
    m_end = m_begin + N;
  }
}

template<typename T, ShDataType DT>
ShDataVariant<T, DT>::ShDataVariant(const ShDataVariant<T, DT> &other)
  : m_managed(true)
{
  int size = other.size();
  alloc(size);
  std::copy(other.begin(), other.end(), m_begin);
}

template<typename T, ShDataType DT>
ShDataVariant<T, DT>::ShDataVariant(const ShDataVariant<T, DT> &other, 
    bool neg, const ShSwizzle &swizzle, int count)
  : m_managed(true)
{
  const int swizzle_size = swizzle.size();
  const int other_size = other.size() / count;
  
  alloc(swizzle_size * count);
  for (int j = 0; j < count; j++) {
    for (int i = 0; i < swizzle_size; ++i) {
      m_begin[j*swizzle_size + i] = other[j*other_size + swizzle[i]];
    }
  }
  if(neg) negate();
}

template<typename T, ShDataType DT>
ShDataVariant<T, DT>::~ShDataVariant() 
{
  if(m_managed) delete[] m_begin;
}

template<typename T, ShDataType DT>
ShDataVariant<T, DT>& ShDataVariant<T, DT>::operator=(const ShDataVariant<T, DT>& other) 
{
  if(managed) delete[] m_begin;
  alloc(other.size());
  std::copy(other.begin(), other.end(), m_begin);
}

template<typename T, ShDataType DT>
ShValueType ShDataVariant<T, DT>::valueType() const {
  return value_type; 
}

template<typename T, ShDataType DT>
ShDataType ShDataVariant<T, DT>::dataType() const {
  return DT; 
}

template<typename T, ShDataType DT>
bool ShDataVariant<T, DT>::typeMatches(
    ShValueType valueType, ShDataType dataType) const {
  return (valueType == value_type) && (dataType == DT);
}


template<typename T, ShDataType DT>
const char* ShDataVariant<T, DT>::typeName() const {
  return ShStorageTypeInfo<T>::name;
}

template<typename T, ShDataType DT>
int ShDataVariant<T, DT>::size() const
{
  return m_end - m_begin; 
}

template<typename T, ShDataType DT>
int ShDataVariant<T, DT>::datasize() const
{
  return sizeof(DataType); 
}

template<typename T, ShDataType DT>
bool ShDataVariant<T, DT>::managed() const
{
  return m_managed; 
}

template<typename T, ShDataType DT>
void ShDataVariant<T, DT>::negate()
{
  transform(m_begin, m_end, m_begin, std::negate<DataType>());
}

template<typename T, ShDataType DT>
void ShDataVariant<T, DT>::set(const ShVariant* other)
{
  SH_DEBUG_ASSERT(other->size() == size());

  CPtrType cast_other = variant_cast<T, DT>(other);
  if(cast_other) {
    std::copy(cast_other->begin(), cast_other->end(), begin());
  } else {
    ShCastManager::instance()->doCast(this, other);
  }
}

template<typename T, ShDataType DT>
void ShDataVariant<T, DT>::set(ShVariantCPtr other)
{
  set(other.object());
}

template<typename T, ShDataType DT>
void ShDataVariant<T, DT>::set(const ShVariant* other, int index)
{
  SH_DEBUG_ASSERT(other->size() == 1); 
  CPtrType cast_other = variant_cast<T, DT>(other);
  if(cast_other) {
    m_begin[index] = (*cast_other)[0];
  } else {
    // make a new DataVariant that uses the index element as it's array 
    ShDataVariant *temp = new ShDataVariant(m_begin + index, 1, false);
    ShCastManager::instance()->doCast(temp, other);
    delete temp; // okay - it doesn't delete its array
  }
}

template<typename T, ShDataType DT>
void ShDataVariant<T, DT>::set(ShVariantCPtr other, int index)
{
  set(other.object(), index);
}

template<typename T, ShDataType DT>
void ShDataVariant<T, DT>::set(const ShVariant* other, bool neg, const ShSwizzle &writemask) 
{
  int wmsize = writemask.size();
  SH_DEBUG_ASSERT(wmsize == other->size());

  if(writemask.identity() && (wmsize == size())) {
    set(other);
    if(neg) negate();
    return;
  }


  CPtrType cast_other = variant_cast<T, DT>(other);
  if(cast_other) {
    for(int i = 0; i < wmsize; ++i) {
      m_begin[writemask[i]] = (*cast_other)[i];
    }
    if(neg) negate();
  } else {
  // otherwise we need a temp buffer variant...doh
    ShDataVariant *temp = new ShDataVariant(wmsize);
    ShCastManager::instance()->doCast(temp, other);
    for(int i = 0; i < wmsize; ++i) {
      m_begin[writemask[i]] = neg ? -(*temp)[i] : (*temp)[i];
    }
    delete temp;
  }
}

template<typename T, ShDataType DT>
void ShDataVariant<T, DT>::set(ShVariantCPtr other, bool neg, const ShSwizzle &writemask) 
{
  set(other.object(), neg, writemask);
}

template<typename T, ShDataType DT>
ShVariantPtr ShDataVariant<T, DT>::get() const
{
  return new ShDataVariant<T, DT>(*this);
}

template<typename T, ShDataType DT>
ShVariantPtr ShDataVariant<T, DT>::get(int index) const
{
  return new ShDataVariant<T, DT>(1, m_begin[index]);
}

template<typename T, ShDataType DT>
ShVariantPtr ShDataVariant<T, DT>::get(bool neg, const ShSwizzle &swizzle, int count) const 
{
  return new ShDataVariant<T, DT>(*this, neg, swizzle, count);
}


template<typename T, ShDataType DT>
bool ShDataVariant<T, DT>::equals(const ShVariant* other) const 
{
  if(!other || (size() != other->size()) 
      || !other->typeMatches(valueType(), dataType())) return false;

  const ShDataVariant* castOther = variant_cast<T, DT>(other);
  const_iterator I, J;
  I = m_begin; 
  J = castOther->begin(); 
  for(;I != m_end; ++I, ++J) {
    if(!shDataTypeEqual((*I), (*J))) return false;
  }
  return true;
}

template<typename T, ShDataType DT>
bool ShDataVariant<T, DT>::equals(ShVariantCPtr other) const 
{
  return equals(other.object());
}

template<typename T, ShDataType DT>
bool ShDataVariant<T, DT>::isTrue() const 
{
  for(const_iterator I = begin(); I != end(); ++I) {
    if(!shDataTypeIsPositive((*I))) return false;
  }
  return true;
}

template<typename T, ShDataType DT>
void* ShDataVariant<T, DT>::array()
{
  return m_begin;
}

template<typename T, ShDataType DT>
const void* ShDataVariant<T, DT>::array() const
{
  return m_begin;
}

template<typename T, ShDataType DT>
typename ShDataVariant<T, DT>::DataType& ShDataVariant<T, DT>::operator[](int index) 
{
  return m_begin[index];
}

template<typename T, ShDataType DT>
const typename ShDataVariant<T, DT>::DataType& ShDataVariant<T, DT>::operator[](int index) const
{
  return m_begin[index];
}

template<typename T, ShDataType DT>
typename ShDataVariant<T, DT>::iterator ShDataVariant<T, DT>::begin() {
  return m_begin;
}

template<typename T, ShDataType DT>
typename ShDataVariant<T, DT>::iterator ShDataVariant<T, DT>::end() {
  return m_end;
}

template<typename T, ShDataType DT>
typename ShDataVariant<T, DT>::const_iterator ShDataVariant<T, DT>::begin() const {
  return m_begin;
}

template<typename T, ShDataType DT>
typename ShDataVariant<T, DT>::const_iterator ShDataVariant<T, DT>::end() const {
  return m_end;
}

template<typename T, ShDataType DT>
std::string ShDataVariant<T, DT>::encode() const {
  if(size() < 1) return "";

  std::ostringstream out;
  out << size();
  for(const_iterator I = m_begin; I != m_end; ++I) {
    out << "," << *I;
  }
  return out.str();
}
template<typename T, ShDataType DT>
std::string ShDataVariant<T, DT>::encode(int index, int repeats) const 
{
  std::ostringstream out;
  out << repeats;
  for(int i = 0; i < repeats; ++i) {
    out << ", " << m_begin[index];
  }
  return out.str();
}

template<typename T, ShDataType DT>
std::string ShDataVariant<T, DT>::encode(bool neg, const ShSwizzle &swizzle) const 
{
  std::ostringstream out;
  out << swizzle.size(); 
  for(int i = 0; i < swizzle.size(); ++i) {
    out << ", " << m_begin[swizzle[i]];
  }
  return out.str();
}

template<typename T, ShDataType DT>
std::string ShDataVariant<T, DT>::encodeArray() const {
  if(size() < 1) return "";

  std::ostringstream out;
  for(const_iterator I = m_begin; I != m_end; ++I) {
    if(I != m_begin) out << ", ";
    out << *I;
  }
  return out.str();
}

template<typename T, ShDataType DT>
void ShDataVariant<T, DT>::alloc(int N) {
  if(N == 0) {
    m_managed = false;
    m_begin = m_end = 0;
    return;
  }
  // SH_DEBUG_PRINT("alloc " << valueTypeName[V] << " " << dataTypeName[DT]);
  m_begin = new DataType[N];
  m_end = m_begin + N;
}

#ifdef SH_USE_MEMORY_POOL
template<typename T, ShDataType DT>
void* ShDataVariant<T, DT>::operator new(std::size_t size)
{
  if (size != sizeof(ShDataVariant)) return ::operator new(size);
  if (!m_pool) {
    m_pool = new ShPool(sizeof(ShDataVariant), 32768);
  }
  return m_pool->alloc();
}

template<typename T, ShDataType DT>
void ShDataVariant<T, DT>::operator delete(void* ptr, std::size_t size)
{
  if(size != sizeof(ShDataVariant)) {
    SH_DEBUG_PRINT("delete size does not match " << size << " " << sizeof(ShDataVariant));
  }
  if(!m_pool) shError( ShException( "Deleting from memory pool without an m_pool." ) );
  m_pool->free(ptr);
}
#endif

template<typename T, ShDataType DT>
ShPointer<ShDataVariant<T, DT> > variant_cast(ShVariantPtr c)
{
  return shref_dynamic_cast<ShDataVariant<T, DT> >(c);
}

template<typename T, ShDataType DT>
ShPointer<const ShDataVariant<T, DT> > variant_cast(ShVariantCPtr c)
{
  return shref_dynamic_cast<const ShDataVariant<T, DT> >(c);
}

template<typename T, ShDataType DT>
ShDataVariant<T, DT>* variant_cast(ShVariant* c)
{
  return dynamic_cast<ShDataVariant<T, DT>*>(c);
}

template<typename T, ShDataType DT>
const ShDataVariant<T, DT>* variant_cast(const ShVariant* c)
{
  return dynamic_cast<const ShDataVariant<T, DT>*>(c);
}

template<typename T, ShDataType DT>
ShPointer<ShDataVariant<T, DT> > variant_convert(ShVariantCPtr c)
{
  ShDataVariant<T, DT>* result = new ShDataVariant<T, DT>(c->size());
  result->set(c);
  return result;
}

}
#endif
