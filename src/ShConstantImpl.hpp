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
ShConstant<N, T>::ShConstant(T values[N])
  : ShVariableN<N, T>(new ShVariableNode(SH_VAR_CONST, N))
{
  for (int i = 0; i < N; i++)
    m_node->setValue(i, values[i]);
}
               
}

#endif
