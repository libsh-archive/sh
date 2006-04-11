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
#ifndef SHBASETEXTUREIMPL_HPP
#define SHBASETEXTUREIMPL_HPP

#include "BaseTexture.hpp"
#include "Context.hpp"
#include "Error.hpp"

namespace SH {

template<typename T>
BaseTexture1D<T>::BaseTexture1D(int width, const TextureTraits& traits)
  : BaseTexture(new TextureNode(SH_TEXTURE_1D, T::typesize, T::value_type, traits, width))
{
}

template<typename T>
BaseTexture2D<T>::BaseTexture2D(int width, int height, const TextureTraits& traits)
  : BaseTexture(new TextureNode(SH_TEXTURE_2D, T::typesize, T::value_type, traits, width, height))
{
}

template<typename T>
BaseTextureRect<T>::BaseTextureRect(int width, int height, const TextureTraits& traits)
  : BaseTexture(new TextureNode(SH_TEXTURE_RECT, T::typesize, T::value_type, traits, width, height))
{
}

template<typename T>
BaseTexture3D<T>::BaseTexture3D(int width, int height, int depth, const TextureTraits& traits)
  : BaseTexture(new TextureNode(SH_TEXTURE_3D, T::typesize, T::value_type, traits, width, height, depth))
{
}

template<typename T>
BaseTextureCube<T>::BaseTextureCube(int width, int height, const TextureTraits& traits)
  : BaseTexture(new TextureNode(SH_TEXTURE_CUBE, T::typesize, T::value_type, traits, width, height))
{
}

template<typename T>
BaseTexture1D<T>::BaseTexture1D(const TextureTraits& traits)
  : BaseTexture(new TextureNode(SH_TEXTURE_1D, T::typesize, T::value_type, traits, 0))
{
}

template<typename T>
BaseTexture2D<T>::BaseTexture2D(const TextureTraits& traits)
  : BaseTexture(new TextureNode(SH_TEXTURE_2D, T::typesize, T::value_type, traits, 0, 0))
{
}

template<typename T>
BaseTextureRect<T>::BaseTextureRect(const TextureTraits& traits)
  : BaseTexture(new TextureNode(SH_TEXTURE_RECT, T::typesize, T::value_type, traits, 0, 0))
{
}

template<typename T>
BaseTexture3D<T>::BaseTexture3D(const TextureTraits& traits)
  : BaseTexture(new TextureNode(SH_TEXTURE_3D, T::typesize, T::value_type, traits, 0, 0, 0))
{
}

template<typename T>
BaseTextureCube<T>::BaseTextureCube(const TextureTraits& traits)
  : BaseTexture(new TextureNode(SH_TEXTURE_CUBE, T::typesize, T::value_type, traits, 0, 0))
{
}

template<typename T>
template<typename T2>
TexData<T, 1, T2> BaseTexture1D<T>::operator()(const Generic<1, T2>& coords) const
{
  return TexData<T, 1, T2>(m_node, coords, false);
}

template<typename T>
template<typename T2> 
TexData<T, 2, T2> BaseTexture2D<T>::operator()(const Generic<2, T2>& coords) const
{
  return TexData<T, 2, T2>(m_node, coords, false);
}

template<typename T>
template<typename T2>
TexData<T, 2, T2> BaseTexture2D<T>::operator()(const Generic<2, T2>& coords,
                                                   const Generic<2, T2>& dx,
                                                   const Generic<2, T2>& dy) const
{
  return TexData<T, 2, T2>(m_node, coords, dx, dy);
}

template<typename T>
template<typename T2>
TexData<T, 2, T2> BaseTextureRect<T>::operator()(const Generic<2, T2>& coords) const
{
  return TexData<T, 2, T2>(m_node, coords, false);
}

template<typename T>
template<typename T2>
TexData<T, 3, T2> BaseTexture3D<T>::operator()(const Generic<3, T2>& coords) const
{
  return TexData<T, 3, T2>(m_node, coords, false);
} 

template<typename T>
template<typename T2>
TexData<T, 3, T2> BaseTextureCube<T>::operator()(const Generic<3, T2>& coords) const
{
  return TexData<T, 3, T2>(m_node, coords, false);
} 

template<typename T>
template<typename T2>
TexData<T, 1, T2> BaseTexture1D<T>::operator[](const Generic<1, T2>& coords) const
{
  return TexData<T, 1, T2>(m_node, coords, true);
}

template<typename T>
template<typename T2>
TexData<T, 2, T2> BaseTexture2D<T>::operator[](const Generic<2, T2>& coords) const
{
  return TexData<T, 2, T2>(m_node, coords, true);
}

template<typename T>
template<typename T2>
TexData<T, 2, T2> BaseTextureRect<T>::operator[](const Generic<2, T2>& coords) const
{
  return TexData<T, 2, T2>(m_node, coords, true);
}

template<typename T>
template<typename T2>
TexData<T, 3, T2> BaseTexture3D<T>::operator[](const Generic<3, T2>& coords) const
{
  return TexData<T, 3, T2>(m_node, coords, true);
}

// setMemory

template<typename T>
void BaseTexture1D<T>::memory(const MemoryPtr& memory, int mipmap_level)
{
  m_node->memory(memory, mipmap_level);
}

template<typename T>
void BaseTexture2D<T>::memory(const MemoryPtr& memory, int mipmap_level)
{
  m_node->memory(memory, mipmap_level);
}

template<typename T>
void BaseTextureRect<T>::memory(const MemoryPtr& memory, int mipmap_level)
{
  m_node->memory(memory, mipmap_level);
}

template<typename T>
void BaseTexture3D<T>::memory(const MemoryPtr& memory, int mipmap_level)
{
  m_node->memory(memory, mipmap_level);
}

template<typename T>
void BaseTextureCube<T>::memory(const MemoryPtr& memory, CubeDirection face, 
				  int mipmap_level)
{
  m_node->memory(memory, face, mipmap_level);
}

// get memory

template<typename T>
MemoryPtr BaseTexture1D<T>::memory(int mipmap_level)
{
  return m_node->memory(mipmap_level);
}

template<typename T>
MemoryPtr BaseTexture2D<T>::memory(int mipmap_level)
{
  return m_node->memory(mipmap_level);
}

template<typename T>
MemoryPtr BaseTextureRect<T>::memory(int mipmap_level)
{
  return m_node->memory(mipmap_level);
}

template<typename T>
MemoryPtr BaseTexture3D<T>::memory(int mipmap_level)
{
  return m_node->memory(mipmap_level);
}

template<typename T>
MemoryPtr BaseTextureCube<T>::memory(CubeDirection face, int mipmap_level)
{
  return m_node->memory(face, mipmap_level);
}

template<typename T>
MemoryCPtr BaseTexture1D<T>::memory(int mipmap_level) const
{
  return m_node->memory(mipmap_level);
}

template<typename T>
MemoryCPtr BaseTexture2D<T>::memory(int mipmap_level) const
{
  return m_node->memory(mipmap_level);
}

template<typename T>
MemoryCPtr BaseTextureRect<T>::memory(int mipmap_level) const
{
  return m_node->memory(mipmap_level);
}

template<typename T>
MemoryCPtr BaseTexture3D<T>::memory(int mipmap_level) const
{
  return m_node->memory(mipmap_level);
}

template<typename T>
MemoryCPtr BaseTextureCube<T>::memory(CubeDirection face, int mipmap_level) const
{
  return m_node->memory(face, mipmap_level);
}

// get size

template<typename T>
Attrib1f BaseTexture1D<T>::size() const
{
  Attrib1f t(m_node->texSizeVar().node(), Swizzle(m_node->texSizeVar().size()), false);
  return t;
}

template<typename T>
Attrib2f BaseTexture2D<T>::size() const
{
  Attrib2f t(m_node->texSizeVar().node(), Swizzle(m_node->texSizeVar().size()), false);
  return t;
}

template<typename T>
Attrib2f BaseTextureRect<T>::size() const
{
  Attrib2f t(m_node->texSizeVar().node(), Swizzle(m_node->texSizeVar().size()), false);
  return t;
}

template<typename T>
Attrib3f BaseTexture3D<T>::size() const
{
  Attrib3f t(m_node->texSizeVar().node(), Swizzle(m_node->texSizeVar().size()), false);
  return t;
}

template<typename T>
Attrib2f BaseTextureCube<T>::size() const
{
  Attrib2f t(m_node->texSizeVar().node(), Swizzle(m_node->texSizeVar().size()), false);
  return t;
}

// set size

template<typename T>
void BaseTexture1D<T>::size(int width)
{
  m_node->setTexSize(width);
}

template<typename T>
void BaseTexture2D<T>::size(int width, int height)
{
  m_node->setTexSize(width, height);
}

template<typename T>
void BaseTextureRect<T>::size(int width, int height)
{
  m_node->setTexSize(width, height);
}

template<typename T>
void BaseTexture3D<T>::size(int width, int height, int depth)
{
  m_node->setTexSize(width, height, depth);
}

template<typename T>
void BaseTextureCube<T>::size(int width, int height)
{
  m_node->setTexSize(width, height);
}

}

#endif
