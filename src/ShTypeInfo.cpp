// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
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

ShTypeInfo::TypeInfoMap ShTypeInfo::m_valueTypes;

void ShTypeInfo::init()
{
  setTypeInfo<double>(m_valueTypes);
  setTypeInfo<float>(m_valueTypes);
  setTypeInfo<ShHalf>(m_valueTypes);

  setTypeInfo<int>(m_valueTypes);
  setTypeInfo<short>(m_valueTypes);
  setTypeInfo<char>(m_valueTypes);
  setTypeInfo<unsigned int>(m_valueTypes);
  setTypeInfo<unsigned short>(m_valueTypes);
  setTypeInfo<unsigned char>(m_valueTypes);

  setTypeInfo<ShFracInt> (m_valueTypes);
  setTypeInfo<ShFracShort> (m_valueTypes);
  setTypeInfo<ShFracByte> (m_valueTypes);
  setTypeInfo<ShFracUInt> (m_valueTypes);
  setTypeInfo<ShFracUShort> (m_valueTypes);
  setTypeInfo<ShFracUByte> (m_valueTypes);

  addOps();

  /* DEBUG */ //SH_DEBUG_PRINT("ShEval ops: \n" << ShEval::instance()->availableOps());

  addCasts();
}

const ShTypeInfo* ShTypeInfo::get(ShValueType valueType, ShDataType dataType)
{
  const ShTypeInfo* result = m_valueTypes(valueType, dataType);
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
