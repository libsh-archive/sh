#ifndef SHBASETEXTUREIMPL_HPP
#define SHBASETEXTUREIMPL_HPP

#include "ShBaseTexture.hpp"

namespace SH {

template<typename T, unsigned int Traits>
ShBaseTexture1D<T, Traits>::ShBaseTexture1D(int width)
  : m_node(new ShTextureNode(SH_TEXTURE_1D, T::typesize, Traits, width))
{
}

template<typename T, unsigned int Traits>
ShBaseTexture2D<T, Traits>::ShBaseTexture2D(int width, int height)
  : m_node(new ShTextureNode(SH_TEXTURE_2D, T::typesize, Traits, width, height))
{
}

template<typename T, unsigned int Traits>
ShBaseTextureRect<T, Traits>::ShBaseTextureRect(int width, int height)
  : m_node(new ShTextureNode(SH_TEXTURE_RECT, T::typesize, Traits, width, height))
{
}

template<typename T, unsigned int Traits>
ShBaseTexture3D<T, Traits>::ShBaseTexture3D(int width, int height, int depth)
  : m_node(new ShTextureNode(SH_TEXTURE_3D, T::typesize, Traits, width, height, depth))
{
}

template<typename T, unsigned int Traits>
ShBaseTextureCube<T, Traits>::ShBaseTextureCube(int width, int height)
  : m_node(new ShTextureNode(SH_TEXTURE_CUBE, T::typesize, Traits, width, height))
{
}

template<typename T, unsigned int Traits>
T ShBaseTexture1D<T, Traits>::operator()(const ShVariableN<1, float>& coords) const
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

template<typename T, unsigned int Traits>
T ShBaseTexture2D<T, Traits>::operator()(const ShVariableN<2, float>& coords) const
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

template<typename T, unsigned int Traits>
T ShBaseTextureRect<T, Traits>::operator()(const ShVariableN<2, float>& coords) const
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

template<typename T, unsigned int Traits>
T ShBaseTexture3D<T, Traits>::operator()(const ShVariableN<3, float>& coords) const
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

template<typename T, unsigned int Traits>
T ShBaseTextureCube<T, Traits>::operator()(const ShVariableN<3, float>& coords) const
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

template<typename T, unsigned int Traits>
T ShBaseTexture1D<T, Traits>::operator[](const ShVariableN<1, float>& coords) const
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

template<typename T, unsigned int Traits>
T ShBaseTexture2D<T, Traits>::operator[](const ShVariableN<2, float>& coords) const
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

template<typename T, unsigned int Traits>
T ShBaseTextureRect<T, Traits>::operator[](const ShVariableN<2, float>& coords) const
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

template<typename T, unsigned int Traits>
T ShBaseTexture3D<T, Traits>::operator[](const ShVariableN<3, float>& coords) const
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

template<typename T, unsigned int Traits>
T ShBaseTextureCube<T, Traits>::operator[](const ShVariableN<3, float>& coords) const
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

template<typename T, unsigned int Traits>
void ShBaseTexture1D<T, Traits>::memory(ShMemoryPtr memory)
{
  m_node->memory(memory);
}

template<typename T, unsigned int Traits>
void ShBaseTexture2D<T, Traits>::memory(ShMemoryPtr memory)
{
  m_node->memory(memory);
}

template<typename T, unsigned int Traits>
void ShBaseTextureRect<T, Traits>::memory(ShMemoryPtr memory)
{
  m_node->memory(memory);
}

template<typename T, unsigned int Traits>
void ShBaseTexture3D<T, Traits>::memory(ShMemoryPtr memory)
{
  m_node->memory(memory);
}

template<typename T, unsigned int Traits>
void ShBaseTextureCube<T, Traits>::memory(ShMemoryPtr memory,
                                          ShCubeDirection face)
{
  m_node->memory(memory, face);
}


template<typename T, unsigned int Traits>
ShMemoryPtr ShBaseTexture1D<T, Traits>::memory()
{
  return m_node->memory();
}

template<typename T, unsigned int Traits>
ShMemoryPtr ShBaseTexture2D<T, Traits>::memory()
{
  return m_node->memory();
}

template<typename T, unsigned int Traits>
ShMemoryPtr ShBaseTextureRect<T, Traits>::memory()
{
  return m_node->memory();
}

template<typename T, unsigned int Traits>
ShMemoryPtr ShBaseTexture3D<T, Traits>::memory()
{
  return m_node->memory();
}

template<typename T, unsigned int Traits>
ShMemoryPtr ShBaseTextureCube<T, Traits>::memory(ShCubeDirection face)
{
  return m_node->memory(face);
}

}

#endif
