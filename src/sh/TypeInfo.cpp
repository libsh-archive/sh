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
#include "TypeInfo.hpp"
#include "Context.hpp"
#include "Eval.hpp"
#include "CastManager.hpp"
#include "VariantCast.hpp"

namespace {
using namespace SH;

// sets up m_valueType entries for host & memory type for the given value type
template<typename T>
void setTypeInfo(TypeInfo::TypeInfoMap &valueTypes) {
  ValueType V = StorageTypeInfo<T>::value_type;
  valueTypes(V, HOST) = DataTypeInfo<T, HOST>::instance();
  valueTypes(V, MEM) = DataTypeInfo<T, MEM>::instance();
}

}

namespace SH {

TypeInfo::TypeInfoMap* TypeInfo::m_valueTypes;

void TypeInfo::init()
{
  if(m_valueTypes) return;
  m_valueTypes = new TypeInfoMap();

  setTypeInfo<Affine<double> >(*m_valueTypes);
  setTypeInfo<Affine<float> >(*m_valueTypes);

  setTypeInfo<Interval<double> >(*m_valueTypes);
  setTypeInfo<Interval<float> >(*m_valueTypes);

  setTypeInfo<double>(*m_valueTypes);
  setTypeInfo<float>(*m_valueTypes);
  setTypeInfo<Half>(*m_valueTypes);

  setTypeInfo<int>(*m_valueTypes);
  setTypeInfo<short>(*m_valueTypes);
  setTypeInfo<char>(*m_valueTypes);
  setTypeInfo<unsigned int>(*m_valueTypes);
  setTypeInfo<unsigned short>(*m_valueTypes);
  setTypeInfo<unsigned char>(*m_valueTypes);

  setTypeInfo<FracInt> (*m_valueTypes);
  setTypeInfo<FracShort> (*m_valueTypes);
  setTypeInfo<FracByte> (*m_valueTypes);
  setTypeInfo<FracUInt> (*m_valueTypes);
  setTypeInfo<FracUShort> (*m_valueTypes);
  setTypeInfo<FracUByte> (*m_valueTypes);

  addOps();

  /* DEBUG */ //SH_DEBUG_PRINT("Eval ops: \n" << Eval::instance()->availableOps());

  addCasts();
}

const TypeInfo* TypeInfo::get(ValueType valueType, DataType dataType)
{
  init();
  const TypeInfo* result = (*m_valueTypes)(valueType, dataType);
  if(!result) SH_DEBUG_PRINT("Null TypeInfo");
  return result;
}

const TypeInfo* typeInfo(ValueType valueType, DataType dataType)
{
  return TypeInfo::get(valueType, dataType); 
}

const VariantFactory* variantFactory(ValueType valueType, DataType dataType)
{
  return typeInfo(valueType, dataType)->variantFactory();  
}

const char* valueTypeName(ValueType valueType)
{
  const TypeInfo* info = typeInfo(valueType, HOST);
  if(!info) return StorageTypeInfo<InvalidStorageType>::name; 
  return info->name();
}

}
