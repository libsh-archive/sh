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
#ifndef SHVARIANTCASTIMPL_HPP
#define SHVARIANTCASTIMPL_HPP

#include "Internals.hpp"
#include "Variant.hpp"
#include "VariantCast.hpp"

namespace SH {

template<typename Dest, DataType DestDT, 
  typename Src, DataType SrcDT> 
DataVariantCast<Dest, DestDT, Src, SrcDT>* 
DataVariantCast<Dest, DestDT, Src, SrcDT>::m_instance = 0;

template<typename Dest, DataType DestDT, 
  typename Src, DataType SrcDT> 
void DataVariantCast<Dest, DestDT, Src, SrcDT>::doCast(
    Variant* dest, const Variant *src) const
{

  SrcVariant* sv = variant_cast<Src, SrcDT>(src);
  DestVariant* dv = variant_cast<Dest, DestDT>(dest); 

  typename SrcVariant::const_iterator S = sv->begin();
  typename DestVariant::iterator D = dv->begin();
  for(;S != sv->end(); ++S, ++D) doCast(*D, *S);
}

template<typename Dest, DataType DestDT, 
  typename Src, DataType SrcDT>
void DataVariantCast<Dest, DestDT, Src, SrcDT>::getCastTypes(
    ValueType &dest, DataType &destDT, 
    ValueType &src, DataType &srcDT) const
{
  dest = DestValueType;
  destDT = DestDT;
  src = SrcValueType;
  srcDT = SrcDT;
}

template<typename Dest, DataType DestDT, 
  typename Src, DataType SrcDT>
void DataVariantCast<Dest, DestDT, Src, SrcDT>::getDestTypes(
    ValueType &valueType, DataType &dataType) const
{
  valueType = DestValueType; 
  dataType = DestDT;
}

template<typename Dest, DataType DestDT, 
  typename Src, DataType SrcDT>
void DataVariantCast<Dest, DestDT, Src, SrcDT>::doCast(D &dest, const S &src) const
{
  dataTypeCast<Dest, DestDT, Src, SrcDT>(dest, src);
}

template<typename Dest, DataType DestDT, 
  typename Src, DataType SrcDT>
const DataVariantCast<Dest, DestDT, Src, SrcDT>*
DataVariantCast<Dest, DestDT, Src, SrcDT>::instance()
{
  if(!m_instance) m_instance = new DataVariantCast();
  return m_instance;
}

}

#endif
