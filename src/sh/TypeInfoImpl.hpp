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
#ifndef SHTYPEINFOIMPL_HPP
#define SHTYPEINFOIMPL_HPP

#include "TypeInfo.hpp"
#include "VariantFactory.hpp"
#include "Interval.hpp"

namespace SH {

template<typename T, DataType DT>
const typename DataTypeInfo<T, DT>::type
DataTypeInfo<T, DT>::Zero = DataTypeConstant<T, DT>::Zero; 

template<typename T, DataType DT>
const typename DataTypeInfo<T, DT>::type
DataTypeInfo<T, DT>::One = DataTypeConstant<T, DT>::One; 

template<typename T, DataType DT>
const char* DataTypeInfo<T, DT>::name() const 
{
  return StorageTypeInfo<T>::name;
}

template<typename T, DataType DT>
int DataTypeInfo<T, DT>::datasize() const 
{
  return sizeof(typename DataTypeCppType<T, DT>::type); 
}

template<typename T, DataType DT>
const VariantFactory* DataTypeInfo<T, DT>::variantFactory() const
{
  return DataVariantFactory<T, DT>::instance();
}

template<typename T, DataType DT>
const DataTypeInfo<T, DT>* DataTypeInfo<T, DT>::instance() 
{
  if(!m_instance) m_instance = new DataTypeInfo<T, DT>();
  return m_instance;
}

template<typename T, DataType DT>
DataTypeInfo<T, DT>* DataTypeInfo<T, DT>::m_instance = 0;


}

#endif
