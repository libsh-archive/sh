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
#ifndef SHCONSTANTIMPL_HPP
#define SHCONSTANTIMPL_HPP

#include "ShConstant.hpp"

namespace SH {

template<int N, typename T>
ShConstant<N, T>::ShConstant(T value)
  : ShVariableN<N, T>(new ShVariableNode(SH_VAR_CONST, N))
{
  m_node->setValue(0, value);
}

template<int N, typename T>
ShConstant<N, T>::ShConstant(T value0, T value1)
  : ShVariableN<N, T>(new ShVariableNode(SH_VAR_CONST, N))
{
  m_node->setValue(0, value0);
  m_node->setValue(1, value1);
}

template<int N, typename T>
ShConstant<N, T>::ShConstant(T value0, T value1, T value2)
  : ShVariableN<N, T>(new ShVariableNode(SH_VAR_CONST, N))
{
  m_node->setValue(0, value0);
  m_node->setValue(1, value1);
  m_node->setValue(2, value2);
}

template<int N, typename T>
ShConstant<N, T>::ShConstant(T value0, T value1, T value2, T value3)
  : ShVariableN<N, T>(new ShVariableNode(SH_VAR_CONST, N))
{
  m_node->setValue(0, value0);
  m_node->setValue(1, value1);
  m_node->setValue(2, value2);
  m_node->setValue(3, value3);
}

template<int N, typename T>
ShConstant<N, T>::ShConstant(T values[N])
  : ShVariableN<N, T>(new ShVariableNode(SH_VAR_CONST, N))
{
  for (int i = 0; i < N; i++)
    m_node->setValue(i, values[i]);
}
               
}

#endif
