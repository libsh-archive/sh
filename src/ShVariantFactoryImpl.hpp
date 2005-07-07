// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#ifndef SH_VARIANTFACTORYIMPL_HPP 
#define SH_VARIANTFACTORYIMPL_HPP 

#include "ShVariant.hpp"
#include "ShVariantFactory.hpp"

namespace SH {

template<typename T, ShDataType DT>
ShVariant* ShDataVariantFactory<T, DT>::generate(int N) const
{
  return new ShDataVariant<T, DT>(N);
}

template<typename T, ShDataType DT>
ShVariant* ShDataVariantFactory<T, DT>::generate(std::string s) const
{
  return new ShDataVariant<T, DT>(s);
}

template<typename T, ShDataType DT>
ShVariant* ShDataVariantFactory<T, DT>::generate(int N, void *data, bool managed) const
{
  return new ShDataVariant<T, DT>(N, data, managed);
}

template<typename T, ShDataType DT>
ShVariant* ShDataVariantFactory<T, DT>::generateZero(int N) const
{
  return new ShDataVariant<T, DT>(N);
}

template<typename T, ShDataType DT>
ShVariant* ShDataVariantFactory<T, DT>::generateOne(int N) const
{
  return new ShDataVariant<T, DT>(N, ShDataTypeConstant<T, DT>::One);
}

template<typename T, ShDataType DT>
ShDataVariantFactory<T, DT>* ShDataVariantFactory<T, DT>::m_instance = 0;

template<typename T, ShDataType DT>
const ShDataVariantFactory<T, DT>*
ShDataVariantFactory<T, DT>::instance() 
{
  if(!m_instance) m_instance = new ShDataVariantFactory<T, DT>();
  return m_instance;
}

template<typename T, ShDataType DT>
ShDataVariantFactory<T, DT>::ShDataVariantFactory()
{}

}

#endif
