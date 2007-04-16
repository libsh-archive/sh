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
#ifndef SHARRAYIMPL_HPP
#define SHARRAYIMPL_HPP

#include "Array.hpp"
#include "Stream.hpp"
#include "BaseTexture.hpp"
#include "BaseTextureND.hpp"
#include "LibStream.hpp"

namespace SH {

template <typename T>
Array1D<T>::Array1D(const BaseTexture& base)
  : BaseTexture1D<T>(base)
{
}

template <typename T>
Array1D<T>::Array1D(int width)
  : BaseTexture1D<T>(width, ArrayTraits())
{
  int size = width * T::typesize * sizeof(typename T::mem_type);
  HostMemoryPtr mem = new HostMemory(size, T::value_type);
  BaseTexture1D<T>::memory(mem);
}

template <typename T>
Array1D<T>::Array1D(const MemoryPtr& mem, int width)
  : BaseTexture1D<T>(width, ArrayTraits())
{
  BaseTexture1D<T>::memory(mem);
}

template <typename T>
Array1D<T>& Array1D<T>::operator=(const Program& program)
{
  Stream stream(*this);
  stream = program;
  return *this;
}

template <typename T>
template <typename T2>
Array1D<T>& Array1D<T>::operator=(const IndexedArray< Array1D<T>, Array1D<T2> >& array)
{
  gather(*this, array.array(), array.index());
  return *this;
}

template <typename T>
template <typename T2>
const IndexedArray< Array1D<T>, Array1D<T2> > 
Array1D<T>::operator[](const Array1D<T2>& index) const
{
  return IndexedArray< Array1D<T>, Array1D<T2> >(*this, index);
}

template <typename T>
template <typename T2>
IndexedArray< Array1D<T>, Array1D<T2> > 
Array1D<T>::operator[](const Array1D<T2>& index)
{
  return IndexedArray< Array1D<T>, Array1D<T2> >(*this, index);
}

template <typename T> inline
const typename T::mem_type* Array1D<T>::read_data() const
{
  return static_cast<typename T::mem_type*>(BaseTexture::read_data(0));
}

template <typename T> inline
typename T::mem_type* Array1D<T>::write_data()
{
  return static_cast<typename T::mem_type*>(BaseTexture::write_data(0));
}

template <typename T>
Array2D<T>::Array2D(int width, int height)
  : BaseTexture2D<T>(width, height, ArrayTraits())
{
  int size = width * height * T::typesize * sizeof(typename T::mem_type);
  HostMemoryPtr mem = new HostMemory(size, T::value_type);
  BaseTexture2D<T>::memory(mem);
}

template <typename T>
Array2D<T>::Array2D(const MemoryPtr& mem, int width, int height)
  : BaseTexture2D<T>(width, height, ArrayTraits())
{
  BaseTexture2D<T>::memory(mem);
}


template <typename T>
Array2D<T>& Array2D<T>::operator=(const Program& program)
{
  Stream stream(*this);
  stream = program;
  return *this;
}

template <typename T> inline
const typename T::mem_type* Array2D<T>::read_data() const
{
  return static_cast<typename T::mem_type*>(BaseTexture::read_data(0));
}

template <typename T> inline
typename T::mem_type* Array2D<T>::write_data()
{
  return static_cast<typename T::mem_type*>(BaseTexture::write_data(0));
}

template <typename T>
ArrayRect<T>::ArrayRect(int width, int height)
  : BaseTextureRect<T>(width, height, ArrayTraits())
{
  int size = width * height * T::typesize * sizeof(typename T::mem_type);
  HostMemoryPtr mem = new HostMemory(size, T::value_type);
  BaseTextureRect<T>::memory(mem);
}

template <typename T>
ArrayRect<T>::ArrayRect(const MemoryPtr& mem, int width, int height)
  : BaseTextureRect<T>(width, height, ArrayTraits())
{
  BaseTextureRect<T>::memory(mem);
}

template <typename T>
ArrayRect<T>& ArrayRect<T>::operator=(const Program& program)
{
  Stream stream(*this);
  stream = program;
  return *this;
}

template <typename T> inline
const typename T::mem_type* ArrayRect<T>::read_data() const
{
  return static_cast<typename T::mem_type*>(BaseTexture::read_data(0));
}

template <typename T> inline
typename T::mem_type* ArrayRect<T>::write_data()
{
  return static_cast<typename T::mem_type*>(BaseTexture::write_data(0));
}

template <typename T>
Array3D<T>::Array3D(int width, int height, int depth)
  : BaseTexture3D<T>(width, height, depth, ArrayTraits())
{
  int size = width * height * depth * T::typesize * sizeof(typename T::mem_type);
  HostMemoryPtr mem = new HostMemory(size, T::value_type);
  BaseTexture3D<T>::memory(mem);
}

template <typename T>
Array3D<T>::Array3D(const MemoryPtr& mem, int width, int height, int depth)
  : BaseTexture3D<T>(width, height, depth, ArrayTraits())
{
  BaseTexture3D<T>::memory(mem);
}

template <typename T>
Array3D<T>& Array3D<T>::operator=(const Program& program)
{
  Stream stream(*this);
  stream = program;
  return *this;
}

template <typename T> inline
const typename T::mem_type* Array3D<T>::read_data() const
{
  return static_cast<typename T::mem_type*>(BaseTexture::read_data(0));
}

template <typename T> inline
typename T::mem_type* Array3D<T>::write_data()
{
  return static_cast<typename T::mem_type*>(BaseTexture::write_data(0));
}

template <typename T1, typename T2>
T1& IndexedArray<T1, T2>::operator=(const T1& src)
{
  scatter(m_array, m_index, src);
  return m_array;
}

}

#endif
