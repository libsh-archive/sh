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
#ifndef SHVARIANTFACTORYIMPL_HPP
#define SHVARIANTFACTORYIMPL_HPP

#include "ShVariantFactory.hpp"
#include "ShTypeInfo.hpp"

namespace SH {

template<typename T>
ShVariantPtr ShDataVariantFactory<T>::generate(int N) const
{
  return new ShDataVariant<H>(N);
}

template<typename T>
ShVariantPtr ShDataVariantFactory<T>::generate(std::string s) const
{
  return new ShDataVariant<H>(s);
}

template<typename T>
ShVariantPtr ShDataVariantFactory<T>::generate(void *data, int N, bool managed) const
{
  return new ShDataVariant<H>(data, N, managed);
}

template<typename T>
ShVariantPtr ShDataVariantFactory<T>::generateLowBound(int N, ShSemanticType type) const
{
  return new ShDataVariant<H>(N, ShConcreteTypeInfo<T>::defaultLo(type));
}

template<typename T>
ShVariantPtr ShDataVariantFactory<T>::generateHighBound(int N, ShSemanticType type) const
{
  return new ShDataVariant<H>(N, ShConcreteTypeInfo<T>::defaultHi(type));
}

template<typename T>
ShVariantPtr ShDataVariantFactory<T>::generateZero(int N) const
{
  return new ShDataVariant<H>(N, ShConcreteTypeInfo<T>::ZERO);
}

template<typename T>
ShVariantPtr ShDataVariantFactory<T>::generateOne(int N) const
{
  return new ShDataVariant<H>(N, ShConcreteTypeInfo<T>::ONE);
}

template<typename T>
ShVariantPtr ShDataVariantFactory<T>::generateTrue(int N) const
{
  return new ShDataVariant<H>(N, ShConcreteTypeInfo<T>::TrueVal);
}

template<typename T>
ShVariantPtr ShDataVariantFactory<T>::generateFalse(int N) const
{
  return new ShDataVariant<H>(N, ShConcreteTypeInfo<T>::FalseVal);
}

template<typename T>
ShVariantPtr ShDataVariantFactory<T>::generateMemory(int N) const
{
  return new ShDataVariant<M>(N);
}

template<typename T>
ShVariantPtr ShDataVariantFactory<T>::generateMemory(void *data, int N, bool managed) const
{
  return new ShDataVariant<M>(data, N, managed);
}

template<typename T>
ShDataVariantFactory<T>* ShDataVariantFactory<T>::m_instance = 0;

template<typename T>
const ShDataVariantFactory<T>*
ShDataVariantFactory<T>::instance() 
{
  if(!m_instance) m_instance = new ShDataVariantFactory<T>();
  return m_instance;
}

template<typename T>
ShDataVariantFactory<T>::ShDataVariantFactory()
{}

}

#endif
