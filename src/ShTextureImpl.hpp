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
#include "ShDebug.hpp"

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
ShTexture2D<T>::ShTexture2D(int width, int height)
  : ShTexture<T>(new ShTextureNode(SH_TEXTURE_2D, width, height, T().size()))
{
}

template<typename T>
T ShTexture2D<T>::operator()(const ShVariableN<2, double>& coords)
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
  SH_DEBUG_PRINT("Loading image");
  
  assert(m_node->width() == image.width()
         && m_node->height() == image.height());
  if (m_node->elements() > image.depth()) {
    float* data = new float[image.width() * image.height() * m_node->elements()];
    for (int y = 0; y < image.height(); y++) {
      for (int x = 0; x < image.width(); x++) {
        int d;
        for (d = 0; d < image.depth(); d++) {
          data[(y * image.width() + x) * m_node->elements() + d] = image(x, y, d);
        }
        for (; d < m_node->elements(); d++) {
          data[(y * image.width() + x) * m_node->elements() + d] = 0.0;
        }
      }
    }
    m_node->setData(data);
    delete [] data;
  } else {
    SH_DEBUG_PRINT("Setting image data directly");
    
    assert(m_node->elements() == image.depth());
    m_node->setData(image.data());
  }
}

}

#endif
