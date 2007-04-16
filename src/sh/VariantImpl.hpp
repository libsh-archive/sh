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
#ifndef VARIANT_IMPL_HPP
#define VARIANT_IMPL_HPP

#include <algorithm>
#include <sstream>
#include "Debug.hpp"
#include "Error.hpp"
#include "CastManager.hpp"
#include "Variant.hpp"
#include "TypeInfo.hpp"

namespace SH {

#ifdef USE_MEMORY_POOL
template<typename T, DataType DT>
Pool* DataVariant<T, DT>::m_pool = 0;
#endif

template<typename T, DataType DT>
DataVariant<T, DT>::DataVariant()
  : m_managed(false)
{
  alloc(0);
}

template<typename T, DataType DT>
DataVariant<T, DT>::DataVariant(int N)
  : m_managed(true)
{
  alloc(N);
  std::fill(m_begin, m_end, DataTypeConstant<T, DT>::Zero);
}

template<typename T, DataType DT>
DataVariant<T, DT>::DataVariant(int N, const CppDataType &value)
  : m_managed(true)
{
  alloc(N);
  std::fill(m_begin, m_end, value); 
}

template<typename T, DataType DT>
DataVariant<T, DT>::DataVariant(std::string encodedValue) 
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

template<typename T, DataType DT>
DataVariant<T, DT>::DataVariant(int N, void *data, bool managed)
  : m_managed(managed)
{
  if(m_managed) {
    alloc(N);
    CppDataType* cast_data = reinterpret_cast<CppDataType*>(data); 
    std::copy(cast_data, cast_data + N, m_begin);
  } else {
    m_begin = reinterpret_cast<CppDataType*>(data);
    m_end = m_begin + N;
  }
}

template<typename T, DataType DT>
DataVariant<T, DT>::DataVariant(const DataVariant<T, DT> &other)
  : m_managed(true)
{
  int size = other.size();
  alloc(size);
  std::copy(other.begin(), other.end(), m_begin);
}

template<typename T, DataType DT>
DataVariant<T, DT>::DataVariant(const DataVariant<T, DT> &other, 
    bool neg, const Swizzle &swizzle, int count)
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

template<typename T, DataType DT>
DataVariant<T, DT>::~DataVariant() 
{
  if(m_managed) delete[] m_begin;
}

template<typename T, DataType DT>
DataVariant<T, DT>& DataVariant<T, DT>::operator=(const DataVariant<T, DT>& other) 
{
  if(managed) delete[] m_begin;
  alloc(other.size());
  std::copy(other.begin(), other.end(), m_begin);
}

template<typename T, DataType DT>
ValueType DataVariant<T, DT>::valueType() const {
  return value_type; 
}

template<typename T, DataType DT>
DataType DataVariant<T, DT>::dataType() const {
  return DT; 
}

template<typename T, DataType DT>
bool DataVariant<T, DT>::typeMatches(
    ValueType valueType, DataType dataType) const {
  return (valueType == value_type) && (dataType == DT);
}


template<typename T, DataType DT>
const char* DataVariant<T, DT>::typeName() const {
  return StorageTypeInfo<T>::name;
}

template<typename T, DataType DT>
int DataVariant<T, DT>::size() const
{
  return m_end - m_begin; 
}

template<typename T, DataType DT>
int DataVariant<T, DT>::datasize() const
{
  return sizeof(CppDataType); 
}

template<typename T, DataType DT>
bool DataVariant<T, DT>::managed() const
{
  return m_managed; 
}

template<typename T, DataType DT>
void DataVariant<T, DT>::negate()
{
  transform(m_begin, m_end, m_begin, std::negate<CppDataType>());
}

template<typename T, DataType DT>
void DataVariant<T, DT>::set(const Variant* other)
{
  SH_DEBUG_ASSERT(other->size() == size());

  CPtrType cast_other = variant_cast<T, DT>(other);
  if(cast_other) {
    std::copy(cast_other->begin(), cast_other->end(), begin());
  } else {
    CastManager::instance()->doCast(this, other);
  }
}

template<typename T, DataType DT>
void DataVariant<T, DT>::set(const VariantCPtr& other)
{
  set(other.object());
}

template<typename T, DataType DT>
void DataVariant<T, DT>::set(const Variant* other, int index)
{
  SH_DEBUG_ASSERT(other->size() == 1); 
  CPtrType cast_other = variant_cast<T, DT>(other);
  if(cast_other) {
    m_begin[index] = (*cast_other)[0];
  } else {
    // make a new DataVariant that uses the index element as it's array 
    DataVariant *temp = new DataVariant(1, m_begin + index, false);
    CastManager::instance()->doCast(temp, other);
    delete temp; // okay - it doesn't delete its array
  }
}

template<typename T, DataType DT>
void DataVariant<T, DT>::set(const VariantCPtr& other, int index)
{
  set(other.object(), index);
}

template<typename T, DataType DT>
void DataVariant<T, DT>::set(const Variant* other, bool neg, const Swizzle &writemask) 
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
    DataVariant *temp = new DataVariant(wmsize);
    CastManager::instance()->doCast(temp, other);
    for(int i = 0; i < wmsize; ++i) {
      m_begin[writemask[i]] = neg ? -(*temp)[i] : (*temp)[i];
    }
    delete temp;
  }
}

template<typename T, DataType DT>
void DataVariant<T, DT>::set(const VariantCPtr& other, bool neg, const Swizzle &writemask) 
{
  set(other.object(), neg, writemask);
}

template<typename T, DataType DT>
VariantPtr DataVariant<T, DT>::get() const
{
  return new DataVariant<T, DT>(*this);
}

template<typename T, DataType DT>
VariantPtr DataVariant<T, DT>::get(int index) const
{
  return new DataVariant<T, DT>(1, m_begin[index]);
}

template<typename T, DataType DT>
VariantPtr DataVariant<T, DT>::get(bool neg, const Swizzle &swizzle, int count) const 
{
  return new DataVariant<T, DT>(*this, neg, swizzle, count);
}


template<typename T, DataType DT>
bool DataVariant<T, DT>::equals(const Variant* other) const 
{
  if(!other || (size() != other->size()) 
      || !other->typeMatches(valueType(), dataType())) return false;

  const DataVariant* castOther = variant_cast<T, DT>(other);
  const_iterator I, J;
  I = m_begin; 
  J = castOther->begin(); 
  for(;I != m_end; ++I, ++J) {
    if(!dataTypeEqual((*I), (*J))) return false;
  }
  return true;
}

template<typename T, DataType DT>
bool DataVariant<T, DT>::equals(const VariantCPtr& other) const 
{
  return equals(other.object());
}

template<typename T, DataType DT>
bool DataVariant<T, DT>::isTrue() const 
{
  for(const_iterator I = begin(); I != end(); ++I) {
    if(!dataTypeIsPositive((*I))) return false;
  }
  return true;
}

template<typename T, DataType DT>
void* DataVariant<T, DT>::array()
{
  return m_begin;
}

template<typename T, DataType DT>
const void* DataVariant<T, DT>::array() const
{
  return m_begin;
}

template<typename T, DataType DT>
typename DataVariant<T, DT>::CppDataType& DataVariant<T, DT>::operator[](int index) 
{
  return m_begin[index];
}

template<typename T, DataType DT>
const typename DataVariant<T, DT>::CppDataType& DataVariant<T, DT>::operator[](int index) const
{
  return m_begin[index];
}

template<typename T, DataType DT>
typename DataVariant<T, DT>::iterator DataVariant<T, DT>::begin() {
  return m_begin;
}

template<typename T, DataType DT>
typename DataVariant<T, DT>::iterator DataVariant<T, DT>::end() {
  return m_end;
}

template<typename T, DataType DT>
typename DataVariant<T, DT>::const_iterator DataVariant<T, DT>::begin() const {
  return m_begin;
}

template<typename T, DataType DT>
typename DataVariant<T, DT>::const_iterator DataVariant<T, DT>::end() const {
  return m_end;
}

template<typename T, DataType DT>
std::string DataVariant<T, DT>::encode() const {
  if(size() < 1) return "";

  std::ostringstream out;
  out << size();
  for(const_iterator I = m_begin; I != m_end; ++I) {
    out << "; " << *I;
  }
  return out.str();
}
template<typename T, DataType DT>
std::string DataVariant<T, DT>::encode(int index, int repeats) const 
{
  std::ostringstream out;
  out << repeats;
  for(int i = 0; i < repeats; ++i) {
    out << "; " << m_begin[index];
  }
  return out.str();
}

template<typename T, DataType DT>
std::string DataVariant<T, DT>::encode(bool neg, const Swizzle &swizzle) const 
{
  std::ostringstream out;
  out << swizzle.size(); 
  for(int i = 0; i < swizzle.size(); ++i) {
    out << "; " << m_begin[swizzle[i]];
  }
  return out.str();
}

template<typename T, DataType DT>
std::string DataVariant<T, DT>::encodeArray() const {
  if(size() < 1) return "";

  std::ostringstream out;
  for(const_iterator I = m_begin; I != m_end; ++I) {
    if(I != m_begin) out << ", ";
    out << *I;
  }
  return out.str();
}

template<typename T, DataType DT>
void DataVariant<T, DT>::alloc(int N) {
  if(N == 0) {
    m_managed = false;
    m_begin = m_end = 0;
    return;
  }
  // SH_DEBUG_PRINT("alloc " << valueTypeName[V] << " " << dataTypeName[DT]);
  m_begin = new CppDataType[N];
  m_end = m_begin + N;
}

#ifdef USE_MEMORY_POOL
template<typename T, DataType DT>
void* DataVariant<T, DT>::operator new(std::size_t size)
{
  if (size != sizeof(DataVariant)) return ::operator new(size);
  if (!m_pool) {
    m_pool = new Pool(sizeof(DataVariant), 32768);
  }
  return m_pool->alloc();
}

template<typename T, DataType DT>
void DataVariant<T, DT>::operator delete(void* ptr, std::size_t size)
{
  if(size != sizeof(DataVariant)) {
    SH_DEBUG_PRINT("delete size does not match " << size << " " << sizeof(DataVariant));
  }
  if(!m_pool) error( Exception( "Deleting from memory pool without an m_pool." ) );
  m_pool->free(ptr);
}
#endif

template<typename T, DataType DT>
inline
Pointer<DataVariant<T, DT> > variant_cast(const VariantPtr& c)
{
  return shref_dynamic_cast<DataVariant<T, DT> >(c);
}

template<typename T, DataType DT>
inline
Pointer<const DataVariant<T, DT> > variant_cast(const VariantCPtr& c)
{
  return shref_dynamic_cast<const DataVariant<T, DT> >(c);
}

template<typename T, DataType DT>
inline
DataVariant<T, DT>* variant_cast(Variant* c)
{
  return dynamic_cast<DataVariant<T, DT>*>(c);
}

template<typename T, DataType DT>
inline
const DataVariant<T, DT>* variant_cast(const Variant* c)
{
  return dynamic_cast<const DataVariant<T, DT>*>(c);
}

template<typename T, DataType DT>
Pointer<DataVariant<T, DT> > variant_convert(const VariantCPtr& c)
{
  DataVariant<T, DT>* result = new DataVariant<T, DT>(c->size());
  result->set(c);
  return result;
}

}
#endif
