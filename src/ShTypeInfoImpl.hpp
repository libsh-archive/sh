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
#include "ShEval.hpp"

namespace SH {

template<typename T>
ShPointer<ShDataCloakFactory<T> > ShConcreteTypeInfo<T>::m_cloakFactory;   

template<typename T>
ShPointer<ShEval > ShConcreteTypeInfo<T>::m_eval;   

template<typename T>
T ShConcreteTypeInfo<T>::defaultLo(ShSemanticType type)
{
  switch(type) {
    case SH_POINT:
    case SH_VECTOR:
    case SH_NORMAL:
    case SH_POSITION:
      return -1;
    default:
      return 0;
  }
}

template<typename T>
T ShConcreteTypeInfo<T>::defaultHi(ShSemanticType type)
{
  return 1;
}

template<typename T>
const char* ShConcreteTypeInfo<T>::name() const 
{
  return m_name;
}

template<typename T>
ShCloakFactoryCPtr ShConcreteTypeInfo<T>::cloakFactory() 
{
  if(!m_cloakFactory) {
    m_cloakFactory = new ShDataCloakFactory<T>();
  }
  return m_cloakFactory;
}

template<typename T>
ShEvalCPtr ShConcreteTypeInfo<T>::eval() 
{
  if(!m_eval) {
    m_eval = new ShDataEval<T>();
  }
  return m_eval;
}

}

#endif
