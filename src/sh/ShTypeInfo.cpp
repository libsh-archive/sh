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
#include "ShTypeInfo.hpp"
#include "ShContext.hpp"
#include "ShEval.hpp"
#include "ShCastManager.hpp"
#include "ShVariantCast.hpp"

namespace {
using namespace SH;

// sets up m_valueType entries for host & memory type for the given value type
template<typename T>
void setTypeInfo(ShTypeInfo::TypeInfoMap &valueTypes) {
  ShValueType V = ShStorageTypeInfo<T>::value_type;
  valueTypes(V, SH_HOST) = ShDataTypeInfo<T, SH_HOST>::instance();
  valueTypes(V, SH_MEM) = ShDataTypeInfo<T, SH_MEM>::instance();
}

}

namespace SH {

ShTypeInfo::TypeInfoMap* ShTypeInfo::m_valueTypes;

void ShTypeInfo::init()
{
  if(m_valueTypes) return;
  m_valueTypes = new TypeInfoMap();

  setTypeInfo<double>(*m_valueTypes);
  setTypeInfo<float>(*m_valueTypes);
  setTypeInfo<ShHalf>(*m_valueTypes);

  setTypeInfo<int>(*m_valueTypes);
  setTypeInfo<short>(*m_valueTypes);
  setTypeInfo<char>(*m_valueTypes);
  setTypeInfo<unsigned int>(*m_valueTypes);
  setTypeInfo<unsigned short>(*m_valueTypes);
  setTypeInfo<unsigned char>(*m_valueTypes);

  setTypeInfo<ShFracInt> (*m_valueTypes);
  setTypeInfo<ShFracShort> (*m_valueTypes);
  setTypeInfo<ShFracByte> (*m_valueTypes);
  setTypeInfo<ShFracUInt> (*m_valueTypes);
  setTypeInfo<ShFracUShort> (*m_valueTypes);
  setTypeInfo<ShFracUByte> (*m_valueTypes);

  addOps();

  /* DEBUG */ //SH_DEBUG_PRINT("ShEval ops: \n" << ShEval::instance()->availableOps());

  addCasts();
}

const ShTypeInfo* ShTypeInfo::get(ShValueType valueType, ShDataType dataType)
{
  init();
  const ShTypeInfo* result = (*m_valueTypes)(valueType, dataType);
  if(!result) SH_DEBUG_PRINT("Null ShTypeInfo");
  return result;
}

const ShTypeInfo* shTypeInfo(ShValueType valueType, ShDataType dataType)
{
  return ShTypeInfo::get(valueType, dataType); 
}

const ShVariantFactory* shVariantFactory(ShValueType valueType, ShDataType dataType)
{
  return shTypeInfo(valueType, dataType)->variantFactory();  
}

const char* shValueTypeName(ShValueType valueType)
{
  const ShTypeInfo* typeInfo = shTypeInfo(valueType, SH_HOST);
  if(!typeInfo) return ShStorageTypeInfo<ShInvalidStorageType>::name; 
  return typeInfo->name();
}

}
