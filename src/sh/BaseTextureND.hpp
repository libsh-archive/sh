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
#ifndef SHBASETEXTUREND_HPP
#define SHBASETEXTUREND_HPP

#include <string>
#include "DllExport.hpp"
#include "Memory.hpp"
#include "Variable.hpp"
#ifndef SH_DO_NOT_INCLUDE_GENERIC_IMPL
#define SH_DO_NOT_INCLUDE_GENERIC_IMPL
#include "Attrib.hpp"
#undef  SH_DO_NOT_INCLUDE_GENERIC_IMPL
#else
#include "Attrib.hpp"
#endif
#include "TexData.hpp"

namespace SH {

/** Base type for 1D Textures.
 */
template<typename T>
class BaseTexture1D : public BaseTexture {
public:
  BaseTexture1D(const TextureTraits& traits);
  BaseTexture1D(int width, const TextureTraits& traits);
  BaseTexture1D(const BaseTexture& base);

  template<typename T2>
  TexData<T, 1, T2> operator()(const Generic<1, T2>& coords) const;

  template<typename T2>
  TexData<T, 1, T2> operator[](const Generic<1, T2>& coords) const;

  MemoryPtr memory(int mipmap_level=0);
  MemoryCPtr memory(int mipmap_level=0) const;
  void memory(const MemoryPtr& memory, int mipmap_level=0);
  void size(int width);

  Attrib1f size() const;
  int width() const { return m_node->width(); }

  typedef T return_type;
};

/** Base type for 2D Textures.
 */
template<typename T>
class BaseTexture2D : public BaseTexture  {
public:
  BaseTexture2D(const TextureTraits& traits);
  BaseTexture2D(int width, int height, const TextureTraits& traits);

  template<typename T2>
  TexData<T, 2, T2> operator()(const Generic<2, T2>& coords) const;

  /// Texture lookup with derivatives
  template<typename T2>
  TexData<T, 2, T2> operator()(const Generic<2, T2>& coords,
                                 const Generic<2, T2>& dx,
                                 const Generic<2, T2>& dy) const;
  
  template<typename T2>
  TexData<T, 2, T2> operator[](const Generic<2, T2>& coords) const;

  MemoryPtr memory(int mipmap_level=0);
  MemoryCPtr memory(int mipmap_level=0) const;
  void memory(const MemoryPtr& memory, int mipmap_level=0);
  void size(int width, int height);

  Attrib2f size() const;

  int width() const { return m_node->width(); }
  int height() const { return m_node->height(); }

  typedef T return_type;
};

/** Base type for Rectangular Textures.
 */
template<typename T>
class BaseTextureRect : public BaseTexture  {
public:
  BaseTextureRect(const TextureTraits& traits);
  BaseTextureRect(int width, int height, const TextureTraits& traits);

  template<typename T2>
  TexData<T, 2, T2> operator()(const Generic<2, T2>& coords) const;

  template<typename T2>
  TexData<T, 2, T2> operator[](const Generic<2, T2>& coords) const;

  MemoryPtr memory(int mipmap_level=0);
  MemoryCPtr memory(int mipmap_level=0) const;
  void memory(const MemoryPtr& memory, int mipmap_level=0);
  void size(int width, int height);

  Attrib2f size() const;

  int width() const { return m_node->width(); }
  int height() const { return m_node->height(); }

  typedef T return_type;
};

/** Base type for 3D Textures.
 */
template<typename T>
class BaseTexture3D : public BaseTexture  {
public:
  BaseTexture3D(const TextureTraits& traits);
  BaseTexture3D(int width, int height, int depth, const TextureTraits& traits);

  template<typename T2>
  TexData<T, 3, T2> operator()(const Generic<3, T2>& coords) const;

  template<typename T2>
  TexData<T, 3, T2> operator[](const Generic<3, T2>& coords) const;

  MemoryPtr memory(int mipmap_level=0);
  MemoryCPtr memory(int mipmap_level=0) const;
  void memory(const MemoryPtr& memory, int mipmap_level=0);
  void size(int width, int height, int depth);

  Attrib3f size() const;
  int width() const { return m_node->width(); }
  int height() const { return m_node->height(); }
  int depth() const { return m_node->depth(); }

  typedef T return_type;
};

/** Base type for Cube Textures.
 */
template<typename T>
class BaseTextureCube : public BaseTexture {
public:
  BaseTextureCube(const TextureTraits& traits);
  BaseTextureCube(int width, int height, const TextureTraits& traits);

  template<typename T2>
  TexData<T, 3, T2> operator()(const Generic<3, T2>& coords) const;

  MemoryPtr memory(CubeDirection face, int mipmap_level=0);
  MemoryCPtr memory(CubeDirection face, int mipmap_level=0) const;
  void memory(const MemoryPtr& memory, CubeDirection face, int mipmap_level=0);
  void size(int width, int height);

  Attrib2f size() const;

  int width() const { return m_node->width(); }
  int height() const { return m_node->height(); }

  typedef T return_type;
};

}

#include "BaseTextureNDImpl.hpp"

#endif
