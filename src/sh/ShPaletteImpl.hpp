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
#ifndef SHPALETTEIMPL_HPP
#define SHPALETTEIMPL_HPP

namespace SH {

template<typename T>
ShPalette<T>::ShPalette(std::size_t size)
  : m_node(new ShPaletteNode(T::typesize, T::semantic_type, T::value_type, size)),
    m_data(new T[size])
{
  for (std::size_t i = 0; i < size; i++) {
    m_node->set_node(i, m_data[i].node());
  }
}

template<typename T>
ShPalette<T>::~ShPalette()
{
  delete [] m_data;
}

template<typename T>
const T& ShPalette<T>::operator[](std::size_t index) const
{
  return m_data[index];
}

template<typename T>
T& ShPalette<T>::operator[](std::size_t index)
{
  return m_data[index];
}

template<typename T>
template<typename T2>
T ShPalette<T>::operator[](const ShGeneric<1, T2>& index) const
{
  if (ShContext::current()->parsing()) {
    T t;
    ShVariable palVar(m_node);
    ShStatement stmt(t, palVar, SH_OP_PAL, index);
    ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
    return t;
  } else {
    return m_data[(std::size_t)index.getValue(0)];
  }
}

}

#endif
