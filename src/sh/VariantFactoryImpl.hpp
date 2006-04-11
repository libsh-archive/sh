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
#ifndef SHVARIANTFACTORYIMPL_HPP 
#define SHVARIANTFACTORYIMPL_HPP 

#include "Variant.hpp"
#include "VariantFactory.hpp"

namespace SH {

template<typename T, DataType DT>
Variant* DataVariantFactory<T, DT>::generate(int N) const
{
  return new DataVariant<T, DT>(N);
}

template<typename T, DataType DT>
Variant* DataVariantFactory<T, DT>::generate(std::string s) const
{
  return new DataVariant<T, DT>(s);
}

template<typename T, DataType DT>
Variant* DataVariantFactory<T, DT>::generate(int N, void *data, bool managed) const
{
  return new DataVariant<T, DT>(N, data, managed);
}

template<typename T, DataType DT>
Variant* DataVariantFactory<T, DT>::generateZero(int N) const
{
  return new DataVariant<T, DT>(N);
}

template<typename T, DataType DT>
Variant* DataVariantFactory<T, DT>::generateOne(int N) const
{
  return new DataVariant<T, DT>(N, DataTypeConstant<T, DT>::One);
}

template<typename T, DataType DT>
DataVariantFactory<T, DT>* DataVariantFactory<T, DT>::m_instance = 0;

template<typename T, DataType DT>
const DataVariantFactory<T, DT>*
DataVariantFactory<T, DT>::instance() 
{
  if(!m_instance) m_instance = new DataVariantFactory<T, DT>();
  return m_instance;
}

template<typename T, DataType DT>
DataVariantFactory<T, DT>::DataVariantFactory()
{}

}

#endif
