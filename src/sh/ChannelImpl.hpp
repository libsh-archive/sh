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
#ifndef SHCHANNELIMPL_HPP
#define SHCHANNELIMPL_HPP

namespace SH {

template <typename T>
Channel<T>::Channel(int width)
  : BaseTexture1D<T>(width, ArrayTraits())
{
  int size = width * T::typesize * sizeof(typename T::mem_type);
  HostMemoryPtr mem = new HostMemory(size, T::value_type);
  BaseTexture1D<T>::memory(mem);
}

template <typename T>
Channel<T>::Channel(const MemoryPtr& mem, int width)
  : BaseTexture1D<T>(width, ArrayTraits())
{
  BaseTexture1D<T>::memory(mem, 0);
}

template <typename T>
int Channel<T>::offset() const
{
  int result;
  BaseTexture1D<T>::m_node->get_offset(&result, 1);
  return result;
}

template <typename T>
int Channel<T>::stride() const
{
  int result;
  BaseTexture1D<T>::m_node->get_stride(&result, 1);
  return result;
}

template <typename T>
int Channel<T>::count() const
{
  int result;
  BaseTexture1D<T>::m_node->get_count(&result, 1);
  return result;
}

template <typename T>
void Channel<T>::offset(int o)
{
  BaseTexture1D<T>::m_node->set_offset(&o, 1);
}

template <typename T>
void Channel<T>::stride(int s)
{
  BaseTexture1D<T>::m_node->set_stride(&s, 1);
}

template <typename T>
void Channel<T>::count(int c)
{
  BaseTexture1D<T>::m_node->set_count(&c, 1);
}

template <typename T>
MemoryCPtr Channel<T>::memory() const
{
  return BaseTexture1D<T>::m_node->memory(0);
}

template <typename T>
MemoryPtr Channel<T>::memory()
{
  return BaseTexture1D<T>::m_node->memory(0);
}

template <typename T>
Channel<T>& Channel<T>::operator=(const Program& program)
{
  Stream stream(*this);
  stream = program;
  return *this;
}

}

#endif
