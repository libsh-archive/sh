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
#ifndef SHTYPEINFOIMPL_HPP
#define SHTYPEINFOIMPL_HPP

#include "ShTypeInfo.hpp"
#include "ShVariantFactory.hpp"
#include "ShInterval.hpp"

namespace SH {

template<ShValueType V, ShDataType DT>
const typename ShDataTypeInfo<V, DT>::type
ShDataTypeInfo<V, DT>::Zero = ShDataTypeConstant<V, DT>::Zero; 

template<ShValueType V, ShDataType DT>
const typename ShDataTypeInfo<V, DT>::type
ShDataTypeInfo<V, DT>::One = ShDataTypeConstant<V, DT>::One; 

template<ShValueType V, ShDataType DT>
const char* ShDataTypeInfo<V, DT>::name() const 
{
  return valueTypeName[V];
}

template<ShValueType V, ShDataType DT>
int ShDataTypeInfo<V, DT>::datasize() const 
{
  return sizeof(typename ShDataTypeCppType<V, DT>::type); 
}

template<ShValueType V, ShDataType DT>
const ShVariantFactory* ShDataTypeInfo<V, DT>::variantFactory() const
{
  return ShDataVariantFactory<V, DT>::instance();
}

template<ShValueType V, ShDataType DT>
const ShDataTypeInfo<V, DT>* ShDataTypeInfo<V, DT>::instance() 
{
  if(!m_instance) m_instance = new ShDataTypeInfo<V, DT>();
  return m_instance;
}

template<ShValueType V, ShDataType DT>
ShDataTypeInfo<V, DT>* ShDataTypeInfo<V, DT>::m_instance = 0;

// Type precedence information 
// 
// currently available types:
// float, double, int, ShInterval<double, ShInterval<float>
template<ShValueType V> 
struct ShCommonType<V, V> { static const ShValueType valueType = V; };

// implementation of the automatic promotion tree lookup
// (may need to break this out for non-compliant compilers)
#define SH_MATCH(V) ((V == V1) || (V == V2))
template<ShValueType V1, ShValueType V2>
struct ShCommonType {
  static const ShValueType valueType = 
          ((SH_MATCH(SH_INTERVAL_DOUBLE) 
            || (SH_MATCH(SH_INTERVAL_FLOAT) && SH_MATCH(SH_DOUBLE))) ? 
              SH_INTERVAL_DOUBLE :
          (SH_MATCH(SH_INTERVAL_FLOAT) ? 
              SH_INTERVAL_FLOAT :
          (SH_MATCH(SH_DOUBLE) ? 
              SH_DOUBLE :
          (SH_MATCH(SH_FLOAT) || SH_MATCH(SH_FRAC_INT) || SH_MATCH(SH_FRAC_SHORT) 
            || SH_MATCH(SH_FRAC_BYTE) || SH_MATCH(SH_FRAC_UINT) 
            || SH_MATCH(SH_FRAC_USHORT) || SH_MATCH(SH_FRAC_UBYTE)
            || (SH_MATCH(SH_UINT) && SH_MATCH(SH_INT)) ? 
              SH_FLOAT :
              SH_INT))));
};
#undef SH_MATCH


}

#endif
