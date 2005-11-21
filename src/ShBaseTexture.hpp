// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#ifndef SHBASETEXTURE_HPP
#define SHBASETEXTURE_HPP

#include <string>
#include "ShDllExport.hpp"
#include "ShTextureNode.hpp"
#include "ShMemory.hpp"
#include "ShVariable.hpp"
#ifndef SH_DO_NOT_INCLUDE_GENERIC_IMPL
#define SH_DO_NOT_INCLUDE_GENERIC_IMPL
#include "ShAttrib.hpp"
#undef  SH_DO_NOT_INCLUDE_GENERIC_IMPL
#else
#include "ShAttrib.hpp"
#endif
#include "ShMetaForwarder.hpp"
#include "ShTexData.hpp"

namespace SH {

class
SH_DLLEXPORT ShBaseTexture : public ShMetaForwarder {
public:
  ShBaseTexture(const ShTextureNodePtr& node);

  void build_mipmaps() { m_node->build_mipmaps(); }

  /// Obtain access to the node that this texture refers to
  const ShTextureNodePtr & node() const { return m_node; }

protected:
  ShTextureNodePtr m_node;
};

/** Base type for 1D Textures.
 */
template<typename T>
class ShBaseTexture1D : public ShBaseTexture {
public:
  ShBaseTexture1D(const ShTextureTraits& traits);
  ShBaseTexture1D(int width, const ShTextureTraits& traits);

  template<typename T2>
  ShTexData<T, 1, T2> operator()(const ShGeneric<1, T2>& coords) const;

  template<typename T2>
  ShTexData<T, 1, T2> operator[](const ShGeneric<1, T2>& coords) const;

  ShMemoryPtr memory(int mipmap_level=0);
  void memory(const ShMemoryPtr& memory, int mipmap_level=0);
  void size(int width);

  ShAttrib1f size() const;
  int width() const { return m_node->width(); }

  typedef T return_type;
};

/** Base type for 2D Textures.
 */
template<typename T>
class ShBaseTexture2D : public ShBaseTexture  {
public:
  ShBaseTexture2D(const ShTextureTraits& traits);
  ShBaseTexture2D(int width, int height, const ShTextureTraits& traits);

  template<typename T2>
  ShTexData<T, 2, T2> operator()(const ShGeneric<2, T2>& coords) const;

  /// Texture lookup with derivatives
  template<typename T2>
  ShTexData<T, 2, T2> operator()(const ShGeneric<2, T2>& coords,
                                 const ShGeneric<2, T2>& dx,
                                 const ShGeneric<2, T2>& dy) const;
  
  template<typename T2>
  ShTexData<T, 2, T2> operator[](const ShGeneric<2, T2>& coords) const;

  ShMemoryPtr memory(int mipmap_level=0);
  void memory(const ShMemoryPtr& memory, int mipmap_level=0);
  void size(int width, int height);

  ShAttrib2f size() const;

  int width() const { return m_node->width(); }
  int height() const { return m_node->height(); }

  typedef T return_type;
};

/** Base type for Rectangular Textures.
 */
template<typename T>
class ShBaseTextureRect : public ShBaseTexture  {
public:
  ShBaseTextureRect(const ShTextureTraits& traits);
  ShBaseTextureRect(int width, int height, const ShTextureTraits& traits);

  template<typename T2>
  ShTexData<T, 2, T2> operator()(const ShGeneric<2, T2>& coords) const;

  template<typename T2>
  ShTexData<T, 2, T2> operator[](const ShGeneric<2, T2>& coords) const;

  ShMemoryPtr memory(int mipmap_level=0);
  void memory(const ShMemoryPtr& memory, int mipmap_level=0);
  void size(int width, int height);

  ShAttrib2f size() const;

  int width() const { return m_node->width(); }
  int height() const { return m_node->height(); }

  typedef T return_type;
};

/** Base type for 3D Textures.
 */
template<typename T>
class ShBaseTexture3D : public ShBaseTexture  {
public:
  ShBaseTexture3D(const ShTextureTraits& traits);
  ShBaseTexture3D(int width, int height, int depth, const ShTextureTraits& traits);

  template<typename T2>
  ShTexData<T, 3, T2> operator()(const ShGeneric<3, T2>& coords) const;

  template<typename T2>
  ShTexData<T, 3, T2> operator[](const ShGeneric<3, T2>& coords) const;

  ShMemoryPtr memory(int mipmap_level=0);
  void memory(const ShMemoryPtr& memory, int mipmap_level=0);
  void size(int width, int height, int depth);

  ShAttrib3f size() const;
  int width() const { return m_node->width(); }
  int height() const { return m_node->height(); }
  int depth() const { return m_node->depth(); }

  typedef T return_type;
};

/** Base type for Cube Textures.
 */
template<typename T>
class ShBaseTextureCube : public ShBaseTexture {
public:
  ShBaseTextureCube(const ShTextureTraits& traits);
  ShBaseTextureCube(int width, int height, const ShTextureTraits& traits);

  template<typename T2>
  ShTexData<T, 3, T2> operator()(const ShGeneric<3, T2>& coords) const;

  ShMemoryPtr memory(ShCubeDirection face, int mipmap_level=0);
  void memory(const ShMemoryPtr& memory, ShCubeDirection face, int mipmap_level=0);
  void size(int width, int height);

  ShAttrib2f size() const;

  int width() const { return m_node->width(); }
  int height() const { return m_node->height(); }

  typedef T return_type;
};

}

#include "ShBaseTextureImpl.hpp"

#endif
