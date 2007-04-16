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
Palette<T>::Palette(std::size_t size)
  : m_node(new PaletteNode(T::typesize, T::semantic_type, T::value_type, size)),
    m_data(size == 0 ? 0 : new T[size])
{
  for (std::size_t i = 0; i < size; i++) {
    m_node->set_node(i, m_data[i].node());
  }
}

template<typename T>
Palette<T>::~Palette()
{
  if(m_data) delete[] m_data; 
}

template<typename T>
const T& Palette<T>::operator[](std::size_t index) const
{
  return m_data[index];
}

template<typename T>
T& Palette<T>::operator[](std::size_t index)
{
  return m_data[index];
}

template<typename T>
template<typename T2>
T Palette<T>::operator[](const Generic<1, T2>& index) const
{
  if (Context::current()->parsing()) {
    T t;
    Variable palVar(m_node);
    Statement stmt(t, palVar, OP_PAL, index);
    Context::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
    return t;
  } else {
    return m_data[(std::size_t)index.getValue(0)];
  }
}

}

#endif
