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
#include "ShInterval.hpp"
#include "ShEval.hpp"
#include "ShCastManager.hpp"
#include "ShVariantCast.hpp"

namespace {
using namespace SH;

// sets up m_valueType entries for host & memory type for the given value type
template<ShValueType V>
void setTypeInfo(const ShTypeInfo* valueTypes[SH_VALUETYPE_END][SH_DATATYPE_END]) {
  valueTypes[V][SH_HOST] = ShDataTypeInfo<V, SH_HOST>::instance();
  valueTypes[V][SH_MEM] = ShDataTypeInfo<V, SH_MEM>::instance();
}

}

namespace SH {

const ShTypeInfo* ShTypeInfo::m_valueTypes[SH_VALUETYPE_END][SH_DATATYPE_END];

void ShTypeInfo::init()
{
  setTypeInfo<SH_INTERVAL_DOUBLE>(m_valueTypes);
  setTypeInfo<SH_INTERVAL_FLOAT>(m_valueTypes);

  setTypeInfo<SH_DOUBLE>(m_valueTypes);
  setTypeInfo<SH_FLOAT>(m_valueTypes);
  setTypeInfo<SH_HALF>(m_valueTypes);

  setTypeInfo<SH_INT>(m_valueTypes);
  setTypeInfo<SH_SHORT>(m_valueTypes);
  setTypeInfo<SH_BYTE>(m_valueTypes);
  setTypeInfo<SH_UINT>(m_valueTypes);
  setTypeInfo<SH_USHORT>(m_valueTypes);
  setTypeInfo<SH_UBYTE>(m_valueTypes);

  setTypeInfo<SH_FRAC_INT>(m_valueTypes);
  setTypeInfo<SH_FRAC_SHORT>(m_valueTypes);
  setTypeInfo<SH_FRAC_BYTE>(m_valueTypes);
  setTypeInfo<SH_FRAC_UINT>(m_valueTypes);
  setTypeInfo<SH_FRAC_USHORT>(m_valueTypes);
  setTypeInfo<SH_FRAC_UBYTE>(m_valueTypes);

  addOps();

  /* DEBUG */ //SH_DEBUG_PRINT("ShEval ops: \n" << ShEval::instance()->availableOps());

  addCasts();
}

const ShTypeInfo* ShTypeInfo::get(ShValueType valueType, ShDataType dataType)
{
  return m_valueTypes[valueType][dataType];
}

int shNumTypes()
{
  return (int)(SH_VALUETYPE_END); 
}

const ShTypeInfo* shTypeInfo(ShValueType valueType, ShDataType dataType)
{
  return ShTypeInfo::get(valueType, dataType); 
}

const ShVariantFactory* shVariantFactory(ShValueType valueType, ShDataType dataType)
{
  return shTypeInfo(valueType, dataType)->variantFactory();  
}

}
