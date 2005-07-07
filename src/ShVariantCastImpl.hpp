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
#ifndef SHVARIANTCASTIMPL_HPP
#define SHVARIANTCASTIMPL_HPP

#include "ShInternals.hpp"
#include "ShVariant.hpp"
#include "ShVariantCast.hpp"

namespace SH {

template<typename Dest, ShDataType DestDT, 
  typename Src, ShDataType SrcDT> 
ShDataVariantCast<Dest, DestDT, Src, SrcDT>* 
ShDataVariantCast<Dest, DestDT, Src, SrcDT>::m_instance = 0;

template<typename Dest, ShDataType DestDT, 
  typename Src, ShDataType SrcDT> 
void ShDataVariantCast<Dest, DestDT, Src, SrcDT>::doCast(
    ShVariant* dest, const ShVariant *src) const
{

  SrcVariant* sv = variant_cast<Src, SrcDT>(src);
  DestVariant* dv = variant_cast<Dest, DestDT>(dest); 

  typename SrcVariant::const_iterator S = sv->begin();
  typename DestVariant::iterator D = dv->begin();
  for(;S != sv->end(); ++S, ++D) doCast(*D, *S);
}

template<typename Dest, ShDataType DestDT, 
  typename Src, ShDataType SrcDT>
void ShDataVariantCast<Dest, DestDT, Src, SrcDT>::getCastTypes(
    ShValueType &dest, ShDataType &destDT, 
    ShValueType &src, ShDataType &srcDT) const
{
  dest = DestValueType;
  destDT = DestDT;
  src = SrcValueType;
  srcDT = SrcDT;
}

template<typename Dest, ShDataType DestDT, 
  typename Src, ShDataType SrcDT>
void ShDataVariantCast<Dest, DestDT, Src, SrcDT>::getDestTypes(
    ShValueType &valueType, ShDataType &dataType) const
{
  valueType = DestValueType; 
  dataType = DestDT;
}

template<typename Dest, ShDataType DestDT, 
  typename Src, ShDataType SrcDT>
void ShDataVariantCast<Dest, DestDT, Src, SrcDT>::doCast(D &dest, const S &src) const
{
  shDataTypeCast<Dest, DestDT, Src, SrcDT>(dest, src);
}

template<typename Dest, ShDataType DestDT, 
  typename Src, ShDataType SrcDT>
const ShDataVariantCast<Dest, DestDT, Src, SrcDT>*
ShDataVariantCast<Dest, DestDT, Src, SrcDT>::instance()
{
  if(!m_instance) m_instance = new ShDataVariantCast();
  return m_instance;
}

}

#endif
