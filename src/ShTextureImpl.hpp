// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////
#ifndef SHTEXTUREIMPL_HPP
#define SHTEXTUREIMPL_HPP

#include "ShTexture.hpp"

namespace SH {

template<typename T>
ShTexture<T>::~ShTexture()
{
}

template<typename T>
ShTexture<T>::ShTexture(ShTextureNodePtr node)
  : m_node(node)
{
}

template<typename T>
ShTextureNodePtr ShTexture<T>::node() const
{
  return m_node;
}

template<typename T>
void ShTexture<T>::loadImage(const ShImage& image, int slice)
{
  ShDataTextureNodePtr node = m_node;
  assert(node);
  
  assert(node->width() == image.width()
         && node->height() == image.height());

  // give node a data memory object
  ShDataMemoryObjectPtr nodeData = node->mem();
  if( !nodeData ) {
    node->setMem( new ShDataMemoryObject(node->width(), node->height(), 
          node->depth(), node->elements()));
    nodeData = node->mem();
    assert( nodeData );
  }

  if (node->elements() > image.depth()) {
    float* data = new float[image.width() * image.height() * node->elements()];
    for (int y = 0; y < image.height(); y++) {
      for (int x = 0; x < image.width(); x++) {
        int d;
        for (d = 0; d < image.depth(); d++) {
          data[(y * image.width() + x) * node->elements() + d] = image(x, y, d);
        }
        for (; d < node->elements(); d++) {
          data[(y * image.width() + x) * node->elements() + d] = 0.0;
        }
      }
    }
    nodeData->setData(data, slice);
    delete [] data;
  } else {
    assert(node->elements() == image.depth());
    nodeData->setData(image.data(), slice);
  }
}

template<typename T>
void ShTexture<T>::attach(ShMemoryObjectPtr memObj) {
  ShDataTextureNodePtr node = m_node;
  assert( node );
  node->setMem( memObj );
}

template<typename T>
ShTexture1D<T>::ShTexture1D(int length)
  : ShTexture<T>(new ShDataTextureNode(SH_TEXTURE_1D, length, 1, 1, T().size()))
{
}

template<typename T>
T ShTexture1D<T>::operator()(const ShVariableN<1, double>& coords) const
{
  T t;
  ShVariable texVar(m_node);
  ShStatement stmt(t, texVar, SH_OP_TEX, coords);
  
  ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
  return t;
}

template<typename T>
void ShTexture1D<T>::load(const ShImage& image)
{
  loadImage(image);
}

template<typename T>
ShTexture2D<T>::ShTexture2D(int width, int height)
  : ShTexture<T>(new ShDataTextureNode(SH_TEXTURE_2D, width, height, 1, T().size()))
{
}

template<typename T>
T ShTexture2D<T>::operator()(const ShVariableN<2, double>& coords) const
{
  T t;
  ShVariable texVar(m_node);
  ShStatement stmt(t, texVar, SH_OP_TEX, coords);
  
  ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
  return t;
}

template<typename T>
void ShTexture2D<T>::load(const ShImage& image)
{
  loadImage(image);
}

template<typename T>
ShTexture3D<T>::ShTexture3D(int width, int height, int depth)
  : ShTexture<T>(new ShDataTextureNode(SH_TEXTURE_3D, width, height, depth, T().size()))
{
}

template<typename T>
T ShTexture3D<T>::operator()(const ShVariableN<3, double>& coords) const
{
  T t;
  ShVariable texVar(m_node);
  ShStatement stmt(t, texVar, SH_OP_TEX, coords);
  
  ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
  return t;
}

template<typename T>
void ShTexture3D<T>::load(const ShImage& image, int slice)
{
  loadImage(image, slice);
}


template<typename T>
ShTextureCube<T>::ShTextureCube()
  : m_node(new ShCubeTextureNode())
{
}

template<typename T>
void ShTextureCube<T>::set(ShCubeDirection dir, const ShTexture2D<T>& texture)
{
  m_node->face(dir) = texture.node();
}

template<typename T>
T ShTextureCube<T>::operator()(const ShVariableN<3, double>& dir) const
{
  T t;
  ShVariable texVar(m_node);
  ShStatement stmt(t, texVar, SH_OP_TEX, dir);
  
  ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
  return t;
}

}

#endif
