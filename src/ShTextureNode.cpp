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
#include <cassert>
#include "ShTextureNode.hpp"
#include "ShEnvironment.hpp"

namespace SH {

ShTextureNode::ShTextureNode(ShTextureDims dims)
  : ShVariableNode(SH_VAR_TEXTURE, 1),
    m_dims(dims)
{
}

ShTextureNode::~ShTextureNode()
{
  
}

ShTextureDims ShTextureNode::dims() const
{
  return m_dims;
}

ShDataTextureNode::ShDataTextureNode(ShTextureDims dims, int width, int height, 
    int depth, int elements)
  : ShTextureNode(dims),
    m_width(width), m_height(height), m_depth(depth),
    m_elements(elements), m_mem(0) 
{
}

ShDataTextureNode::~ShDataTextureNode()
{
}

int ShDataTextureNode::width() const
{
  return m_width;
}

int ShDataTextureNode::height() const
{
  return m_height;
}

int ShDataTextureNode::depth() const
{
  return m_depth;
}

int ShDataTextureNode::elements() const
{
  return m_elements;
}

bool ShDataTextureNode::compatibleWith(ShMemoryObjectPtr memObj) {
  return m_width == memObj->width() &&
         m_height == memObj->height() &&
         m_depth == memObj->depth() &&
         m_elements == memObj->elements();
}

void ShDataTextureNode::setMem(ShMemoryObjectPtr mem)
{
  if( !mem ) {
    m_mem = 0;
    return;
  }
  assert( compatibleWith(mem) );
  m_mem = mem;  
  for (int s = 0; s < shShaderKinds; s++) {
    if (ShEnvironment::boundShader[s]) ShEnvironment::boundShader[s]->updateUniform(this);
  }
}

ShMemoryObjectPtr ShDataTextureNode::mem() const
{
  return m_mem;
}

ShCubeTextureNode::ShCubeTextureNode()
  : ShTextureNode(SH_TEXTURE_CUBE)
{
}

ShCubeTextureNode::~ShCubeTextureNode()
{
}

ShDataTextureNodePtr& ShCubeTextureNode::face(ShCubeDirection dir)
{
  return m_faces[dir];
}

const ShDataTextureNodePtr ShCubeTextureNode::face(ShCubeDirection dir) const
{
  return m_faces[dir];
}

}

