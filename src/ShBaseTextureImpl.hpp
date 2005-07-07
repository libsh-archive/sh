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
#ifndef SHBASETEXTUREIMPL_HPP
#define SHBASETEXTUREIMPL_HPP

#include "ShBaseTexture.hpp"
#include "ShContext.hpp"
#include "ShError.hpp"
#include "ShLibMisc.hpp"

namespace SH {

template<typename T>
ShBaseTexture1D<T>::ShBaseTexture1D(int width, const ShTextureTraits& traits)
  : ShBaseTexture(new ShTextureNode(SH_TEXTURE_1D, T::typesize, T::value_type, traits, width))
{
}

template<typename T>
ShBaseTexture2D<T>::ShBaseTexture2D(int width, int height, const ShTextureTraits& traits)
  : ShBaseTexture(new ShTextureNode(SH_TEXTURE_2D, T::typesize, T::value_type, traits, width, height))
{
}

template<typename T>
ShBaseTextureRect<T>::ShBaseTextureRect(int width, int height, const ShTextureTraits& traits)
  : ShBaseTexture(new ShTextureNode(SH_TEXTURE_RECT, T::typesize, T::value_type, traits, width, height))
{
}

template<typename T>
ShBaseTexture3D<T>::ShBaseTexture3D(int width, int height, int depth, const ShTextureTraits& traits)
  : ShBaseTexture(new ShTextureNode(SH_TEXTURE_3D, T::typesize, T::value_type, traits, width, height, depth))
{
}

template<typename T>
ShBaseTextureCube<T>::ShBaseTextureCube(int width, int height, const ShTextureTraits& traits)
  : ShBaseTexture(new ShTextureNode(SH_TEXTURE_CUBE, T::typesize, T::value_type, traits, width, height))
{
}

template<typename T>
ShBaseTexture1D<T>::ShBaseTexture1D(const ShTextureTraits& traits)
  : ShBaseTexture(new ShTextureNode(SH_TEXTURE_1D, T::typesize, T::value_type, traits, 0))
{
}

template<typename T>
ShBaseTexture2D<T>::ShBaseTexture2D(const ShTextureTraits& traits)
  : ShBaseTexture(new ShTextureNode(SH_TEXTURE_2D, T::typesize, T::value_type, traits, 0, 0))
{
}

template<typename T>
ShBaseTextureRect<T>::ShBaseTextureRect(const ShTextureTraits& traits)
  : ShBaseTexture(new ShTextureNode(SH_TEXTURE_RECT, T::typesize, T::value_type, traits, 0, 0))
{
}

template<typename T>
ShBaseTexture3D<T>::ShBaseTexture3D(const ShTextureTraits& traits)
  : ShBaseTexture(new ShTextureNode(SH_TEXTURE_3D, T::typesize, T::value_type, traits, 0, 0, 0))
{
}

template<typename T>
ShBaseTextureCube<T>::ShBaseTextureCube(const ShTextureTraits& traits)
  : ShBaseTexture(new ShTextureNode(SH_TEXTURE_CUBE, T::typesize, T::value_type, traits, 0, 0))
{
}

template<typename T>
template<typename T2>
T ShBaseTexture1D<T>::operator()(const ShGeneric<1, T2>& coords) const
{
  if (ShContext::current()->parsing()) {
    T t;
    ShVariable texVar(m_node);
    ShStatement stmt(t, texVar, SH_OP_TEX, coords);
    ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
    return t;
  } else {
    // TODO!
    T t;
    return t;
  }
}

template<typename T>
template<typename T2>
T ShBaseTexture2D<T>::operator()(const ShGeneric<2, T2>& coords) const
{
  if (ShContext::current()->parsing()) {
    T t;
    ShVariable texVar(m_node);
    ShStatement stmt(t, texVar, SH_OP_TEX, coords);
    ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
    return t;
  } else {
    // TODO!
    T t;
    return t;
  }
}

template<typename T>
template<typename T2, typename T3, typename T4>
T ShBaseTexture2D<T>::operator()(const ShGeneric<2, T2>& coords,
                                 const ShGeneric<2, T3>& dx,
                                 const ShGeneric<2, T4>& dy) const
{
  if (ShContext::current()->parsing()) {
    T t;
    ShVariable texVar(m_node);
    ShStatement stmt(t, SH_OP_TEXD, texVar, coords, join(dx, dy));
    ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
    return t;
  } else {
    shError(ShScopeException("Cannot do derivative texture lookup in immediate mode"));
    T t; return t;
  }
}

template<typename T>
template<typename T2>
T ShBaseTextureRect<T>::operator()(const ShGeneric<2, T2>& coords) const
{
  if (ShContext::current()->parsing()) {
    T t;
    ShVariable texVar(m_node);
    ShStatement stmt(t, texVar, SH_OP_TEX, coords);
    ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
    return t;
  } else {
    // TODO!
    T t;
    return t;
  }
}

template<typename T>
template<typename T2>
T ShBaseTexture3D<T>::operator()(const ShGeneric<3, T2>& coords) const
{
  if (ShContext::current()->parsing()) {
    T t;
    ShVariable texVar(m_node);
    ShStatement stmt(t, texVar, SH_OP_TEX, coords);
    ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
    return t;
  } else {
    // TODO!
    T t;
    return t;
  }
} 

template<typename T>
template<typename T2>
T ShBaseTextureCube<T>::operator()(const ShGeneric<3, T2>& coords) const
{
  if (ShContext::current()->parsing()) {
    T t;
    ShVariable texVar(m_node);
    ShStatement stmt(t, texVar, SH_OP_TEX, coords);
    ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
    return t;
  } else {
    // TODO!
    T t;
    return t;
  }
} 

template<typename T>
template<typename T2>
T ShBaseTexture1D<T>::operator[](const ShGeneric<1, T2>& coords) const
{
  if (ShContext::current()->parsing()) {
    T t;
    ShVariable texVar(m_node);
    ShStatement stmt(t, texVar, SH_OP_TEXI, coords);
    ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
    return t;
  } else {
    // TODO!
    T t;
    return t;
  }
}

template<typename T>
template<typename T2>
T ShBaseTexture2D<T>::operator[](const ShGeneric<2, T2>& coords) const
{
  if (ShContext::current()->parsing()) {
    T t;
    ShVariable texVar(m_node);
    ShStatement stmt(t, texVar, SH_OP_TEXI, coords);
    ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
    return t;
  } else {
    // TODO!
    T t;
    return t;
  }
}

template<typename T>
template<typename T2>
T ShBaseTextureRect<T>::operator[](const ShGeneric<2, T2>& coords) const
{
  if (ShContext::current()->parsing()) {
    T t;
    ShVariable texVar(m_node);
    ShStatement stmt(t, texVar, SH_OP_TEXI, coords);
    ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
    return t;
  } else {
    // TODO!
    T t;
    return t;
  }
}

template<typename T>
template<typename T2>
T ShBaseTexture3D<T>::operator[](const ShGeneric<3, T2>& coords) const
{
  if (ShContext::current()->parsing()) {
    T t;
    ShVariable texVar(m_node);
    ShStatement stmt(t, texVar, SH_OP_TEXI, coords);
    ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
    return t;
  } else {
    // TODO!
    T t;
    return t;
  }
}

// setMemory

template<typename T>
void ShBaseTexture1D<T>::memory(ShMemoryPtr memory, int mipmap_level)
{
  m_node->memory(memory, mipmap_level);
}

template<typename T>
void ShBaseTexture2D<T>::memory(ShMemoryPtr memory, int mipmap_level)
{
  m_node->memory(memory, mipmap_level);
}

template<typename T>
void ShBaseTextureRect<T>::memory(ShMemoryPtr memory, int mipmap_level)
{
  m_node->memory(memory, mipmap_level);
}

template<typename T>
void ShBaseTexture3D<T>::memory(ShMemoryPtr memory, int mipmap_level)
{
  m_node->memory(memory, mipmap_level);
}

template<typename T>
void ShBaseTextureCube<T>::memory(ShMemoryPtr memory, ShCubeDirection face, 
				  int mipmap_level)
{
  m_node->memory(memory, face, mipmap_level);
}

// get memory

template<typename T>
ShMemoryPtr ShBaseTexture1D<T>::memory(int mipmap_level)
{
  return m_node->memory(mipmap_level);
}

template<typename T>
ShMemoryPtr ShBaseTexture2D<T>::memory(int mipmap_level)
{
  return m_node->memory(mipmap_level);
}

template<typename T>
ShMemoryPtr ShBaseTextureRect<T>::memory(int mipmap_level)
{
  return m_node->memory(mipmap_level);
}

template<typename T>
ShMemoryPtr ShBaseTexture3D<T>::memory(int mipmap_level)
{
  return m_node->memory(mipmap_level);
}

template<typename T>
ShMemoryPtr ShBaseTextureCube<T>::memory(ShCubeDirection face, int mipmap_level)
{
  return m_node->memory(face, mipmap_level);
}

// get size

template<typename T>
ShAttrib1f ShBaseTexture1D<T>::size() const
{
  ShAttrib1f t(m_node->texSizeVar().node(), ShSwizzle(m_node->texSizeVar().size()), false);
  return t;
}

template<typename T>
ShAttrib2f ShBaseTexture2D<T>::size() const
{
  ShAttrib2f t(m_node->texSizeVar().node(), ShSwizzle(m_node->texSizeVar().size()), false);
  return t;
}

template<typename T>
ShAttrib2f ShBaseTextureRect<T>::size() const
{
  ShAttrib2f t(m_node->texSizeVar().node(), ShSwizzle(m_node->texSizeVar().size()), false);
  return t;
}

template<typename T>
ShAttrib3f ShBaseTexture3D<T>::size() const
{
  ShAttrib3f t(m_node->texSizeVar().node(), ShSwizzle(m_node->texSizeVar().size()), false);
  return t;
}

template<typename T>
ShAttrib2f ShBaseTextureCube<T>::size() const
{
  ShAttrib2f t(m_node->texSizeVar().node(), ShSwizzle(m_node->texSizeVar().size()), false);
  return t;
}

// set size

template<typename T>
void ShBaseTexture1D<T>::size(int width)
{
  m_node->setTexSize(width);
}

template<typename T>
void ShBaseTexture2D<T>::size(int width, int height)
{
  m_node->setTexSize(width, height);
}

template<typename T>
void ShBaseTextureRect<T>::size(int width, int height)
{
  m_node->setTexSize(width, height);
}

template<typename T>
void ShBaseTexture3D<T>::size(int width, int height, int depth)
{
  m_node->setTexSize(width, height, depth);
}

template<typename T>
void ShBaseTextureCube<T>::size(int width, int height)
{
  m_node->setTexSize(width, height);
}

}

#endif
