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
#ifndef SH_VARIANTFACTORYIMPL_HPP 
#define SH_VARIANTFACTORYIMPL_HPP 

#include "ShVariant.hpp"
#include "ShVariantFactory.hpp"

namespace SH {

template<ShValueType V, ShDataType DT>
ShVariant* ShDataVariantFactory<V, DT>::generate(int N) const
{
  return new ShDataVariant<V, DT>(N);
}

template<ShValueType V, ShDataType DT>
ShVariant* ShDataVariantFactory<V, DT>::generate(std::string s) const
{
  return new ShDataVariant<V, DT>(s);
}

template<ShValueType V, ShDataType DT>
ShVariant* ShDataVariantFactory<V, DT>::generate(void *data, int N, bool managed) const
{
  return new ShDataVariant<V, DT>(data, N, managed);
}

template<ShValueType V, ShDataType DT>
ShVariant* ShDataVariantFactory<V, DT>::generateZero(int N) const
{
  return new ShDataVariant<V, DT>(N);
}

template<ShValueType V, ShDataType DT>
ShVariant* ShDataVariantFactory<V, DT>::generateOne(int N) const
{
  return new ShDataVariant<V, DT>(N, ShDataTypeConstant<V, DT>::One);
}

template<ShValueType V, ShDataType DT>
ShDataVariantFactory<V, DT>* ShDataVariantFactory<V, DT>::m_instance = 0;

template<ShValueType V, ShDataType DT>
const ShDataVariantFactory<V, DT>*
ShDataVariantFactory<V, DT>::instance() 
{
  if(!m_instance) m_instance = new ShDataVariantFactory<V, DT>();
  return m_instance;
}

template<ShValueType V, ShDataType DT>
ShDataVariantFactory<V, DT>::ShDataVariantFactory()
{}

}

#endif
