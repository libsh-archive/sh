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

#include "ShTypeInfo.hpp"
#include "ShVariantFactory.hpp"

namespace SH {

template<typename T, ShDataType DT>
const typename ShDataTypeInfo<T, DT>::type
ShDataTypeInfo<T, DT>::Zero = ShDataTypeConstant<T, DT>::Zero; 

template<typename T, ShDataType DT>
const typename ShDataTypeInfo<T, DT>::type
ShDataTypeInfo<T, DT>::One = ShDataTypeConstant<T, DT>::One; 

template<typename T, ShDataType DT>
const char* ShDataTypeInfo<T, DT>::name() const 
{
  return ShStorageTypeInfo<T>::name;
}

template<typename T, ShDataType DT>
int ShDataTypeInfo<T, DT>::datasize() const 
{
  return sizeof(typename ShDataTypeCppType<T, DT>::type); 
}

template<typename T, ShDataType DT>
const ShVariantFactory* ShDataTypeInfo<T, DT>::variantFactory() const
{
  return ShDataVariantFactory<T, DT>::instance();
}

template<typename T, ShDataType DT>
const ShDataTypeInfo<T, DT>* ShDataTypeInfo<T, DT>::instance() 
{
  if(!m_instance) m_instance = new ShDataTypeInfo<T, DT>();
  return m_instance;
}

template<typename T, ShDataType DT>
ShDataTypeInfo<T, DT>* ShDataTypeInfo<T, DT>::m_instance = 0;


}

#endif
