#ifndef SHBASETEXTUREIMPL_HPP
#define SHBASETEXTUREIMPL_HPP

#include "ShBaseTexture.hpp"
#include "ShEnvironment.hpp"

namespace SH {

template<typename T>
ShBaseTexture1D<T>::ShBaseTexture1D(int width, const ShTextureTraits& traits)
  : m_node(new ShTextureNode(SH_TEXTURE_1D, T::typesize, traits, width))
{
}

template<typename T>
ShBaseTexture2D<T>::ShBaseTexture2D(int width, int height, const ShTextureTraits& traits)
  : m_node(new ShTextureNode(SH_TEXTURE_2D, T::typesize, traits, width, height))
{
}

template<typename T>
ShBaseTextureRect<T>::ShBaseTextureRect(int width, int height, const ShTextureTraits& traits)
  : m_node(new ShTextureNode(SH_TEXTURE_RECT, T::typesize, traits, width, height))
{
}

template<typename T>
ShBaseTexture3D<T>::ShBaseTexture3D(int width, int height, int depth, const ShTextureTraits& traits)
  : m_node(new ShTextureNode(SH_TEXTURE_3D, T::typesize, traits, width, height, depth))
{
}

template<typename T>
ShBaseTextureCube<T>::ShBaseTextureCube(int width, int height, const ShTextureTraits& traits)
  : m_node(new ShTextureNode(SH_TEXTURE_CUBE, T::typesize, traits, width, height))
{
}

template<typename T>
ShBaseTexture1D<T>::ShBaseTexture1D(const ShTextureTraits& traits)
  : m_node(new ShTextureNode(SH_TEXTURE_1D, T::typesize, traits, 0))
{
}

template<typename T>
ShBaseTexture2D<T>::ShBaseTexture2D(const ShTextureTraits& traits)
  : m_node(new ShTextureNode(SH_TEXTURE_2D, T::typesize, traits, 0, 0))
{
}

template<typename T>
ShBaseTextureRect<T>::ShBaseTextureRect(const ShTextureTraits& traits)
  : m_node(new ShTextureNode(SH_TEXTURE_RECT, T::typesize, traits, 0, 0))
{
}

template<typename T>
ShBaseTexture3D<T>::ShBaseTexture3D(const ShTextureTraits& traits)
  : m_node(new ShTextureNode(SH_TEXTURE_3D, T::typesize, traits, 0, 0, 0))
{
}

template<typename T>
ShBaseTextureCube<T>::ShBaseTextureCube(const ShTextureTraits& traits)
  : m_node(new ShTextureNode(SH_TEXTURE_CUBE, T::typesize, traits, 0, 0))
{
}

template<typename T>
T ShBaseTexture1D<T>::operator()(const ShGeneric<1, float>& coords) const
{
  if (ShEnvironment::insideShader) {
    T t;
    ShVariable texVar(m_node);
    ShStatement stmt(t, texVar, SH_OP_TEX, coords);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  } else {
    // TODO!
    T t;
    return t;
  }
}

template<typename T>
T ShBaseTexture2D<T>::operator()(const ShGeneric<2, float>& coords) const
{
  if (ShEnvironment::insideShader) {
    T t;
    ShVariable texVar(m_node);
    ShStatement stmt(t, texVar, SH_OP_TEX, coords);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  } else {
    // TODO!
    T t;
    return t;
  }
}

template<typename T>
T ShBaseTextureRect<T>::operator()(const ShGeneric<2, float>& coords) const
{
  if (ShEnvironment::insideShader) {
    T t;
    ShVariable texVar(m_node);
    ShStatement stmt(t, texVar, SH_OP_TEX, coords);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  } else {
    // TODO!
    T t;
    return t;
  }
}

template<typename T>
T ShBaseTexture3D<T>::operator()(const ShGeneric<3, float>& coords) const
{
  if (ShEnvironment::insideShader) {
    T t;
    ShVariable texVar(m_node);
    ShStatement stmt(t, texVar, SH_OP_TEX, coords);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  } else {
    // TODO!
    T t;
    return t;
  }
} 

template<typename T>
T ShBaseTextureCube<T>::operator()(const ShGeneric<3, float>& coords) const
{
  if (ShEnvironment::insideShader) {
    T t;
    ShVariable texVar(m_node);
    ShStatement stmt(t, texVar, SH_OP_TEX, coords);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  } else {
    // TODO!
    T t;
    return t;
  }
} 

template<typename T>
T ShBaseTexture1D<T>::operator[](const ShGeneric<1, float>& coords) const
{
  if (ShEnvironment::insideShader) {
    T t;
    ShVariable texVar(m_node);
    ShStatement stmt(t, texVar, SH_OP_TEXI, coords);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  } else {
    // TODO!
    T t;
    return t;
  }
}

template<typename T>
T ShBaseTexture2D<T>::operator[](const ShGeneric<2, float>& coords) const
{
  if (ShEnvironment::insideShader) {
    T t;
    ShVariable texVar(m_node);
    ShStatement stmt(t, texVar, SH_OP_TEXI, coords);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  } else {
    // TODO!
    T t;
    return t;
  }
}

template<typename T>
T ShBaseTextureRect<T>::operator[](const ShGeneric<2, float>& coords) const
{
  if (ShEnvironment::insideShader) {
    T t;
    ShVariable texVar(m_node);
    ShStatement stmt(t, texVar, SH_OP_TEXI, coords);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  } else {
    // TODO!
    T t;
    return t;
  }
}

template<typename T>
T ShBaseTexture3D<T>::operator[](const ShGeneric<3, float>& coords) const
{
  if (ShEnvironment::insideShader) {
    T t;
    ShVariable texVar(m_node);
    ShStatement stmt(t, texVar, SH_OP_TEXI, coords);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  } else {
    // TODO!
    T t;
    return t;
  }
}

// setMemory

template<typename T>
void ShBaseTexture1D<T>::memory(ShMemoryPtr memory)
{
  m_node->memory(memory);
}

template<typename T>
void ShBaseTexture2D<T>::memory(ShMemoryPtr memory)
{
  m_node->memory(memory);
}

template<typename T>
void ShBaseTextureRect<T>::memory(ShMemoryPtr memory)
{
  m_node->memory(memory);
}

template<typename T>
void ShBaseTexture3D<T>::memory(ShMemoryPtr memory)
{
  m_node->memory(memory);
}

template<typename T>
void ShBaseTextureCube<T>::memory(ShMemoryPtr memory,
                                  ShCubeDirection face)
{
  m_node->memory(memory, face);
}

// get memory

template<typename T>
ShMemoryPtr ShBaseTexture1D<T>::memory()
{
  return m_node->memory();
}

template<typename T>
ShMemoryPtr ShBaseTexture2D<T>::memory()
{
  return m_node->memory();
}

template<typename T>
ShMemoryPtr ShBaseTextureRect<T>::memory()
{
  return m_node->memory();
}

template<typename T>
ShMemoryPtr ShBaseTexture3D<T>::memory()
{
  return m_node->memory();
}

template<typename T>
ShMemoryPtr ShBaseTextureCube<T>::memory(ShCubeDirection face)
{
  return m_node->memory(face);
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
