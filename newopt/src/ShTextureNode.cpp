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
#include "ShDebug.hpp"
#include "ShAttrib.hpp"

namespace SH {

ShTextureNode::ShTextureNode(ShTextureDims dims, int size,
                             const ShTextureTraits& traits,
                             int width, int height, int depth)
  : ShVariableNode(SH_TEXTURE, size),
    m_dims(dims),
    m_memory(new ShMemoryPtr[dims == SH_TEXTURE_CUBE ? 6 : 1]),
    m_traits(traits),
    m_width(width), m_height(height), m_depth(depth)
{
  if (m_dims == SH_TEXTURE_1D) {
    ShAttrib1f v = ShAttrib1f(width);
    m_texSizeVar = ShVariable(v.node());
  } else if (m_dims == SH_TEXTURE_3D) {
    ShAttrib3f v = ShAttrib3f(width, height, depth);
    m_texSizeVar = ShVariable(v.node());
  } else {
    ShAttrib2f v = ShAttrib2f(width, height);
    m_texSizeVar = ShVariable(v.node());
  }
}

ShTextureNode::~ShTextureNode()
{
  delete [] m_memory;
}

ShTextureDims ShTextureNode::dims() const
{
  return m_dims;
}

ShMemoryPtr ShTextureNode::memory(int n)
{
  SH_DEBUG_ASSERT((n == 0)
                  || (m_dims == SH_TEXTURE_CUBE && n >= 0 && n < 6));
  return m_memory[n];
}

ShPointer<const ShMemory> ShTextureNode::memory(int n) const
{
  SH_DEBUG_ASSERT((n == 0)
                  || (m_dims == SH_TEXTURE_CUBE && n >= 0 && n < 6));
  return m_memory[n];
}

void ShTextureNode::memory(ShMemoryPtr memory, int n)
{
  SH_DEBUG_ASSERT((n == 0)
                  || (m_dims == SH_TEXTURE_CUBE && n >= 0 && n < 6));
  m_memory[n] = memory;
}

ShMemoryPtr ShTextureNode::memory(ShCubeDirection dir)
{
  return memory(static_cast<int>(dir));
}

ShPointer<const ShMemory> ShTextureNode::memory(ShCubeDirection dir) const
{
  return memory(static_cast<int>(dir));
}

void ShTextureNode::memory(ShMemoryPtr mem, ShCubeDirection dir)
{
  memory(mem, static_cast<int>(dir));
}

const ShTextureTraits& ShTextureNode::traits() const
{
  return m_traits;
}

ShTextureTraits& ShTextureNode::traits()
{
  return m_traits;
}

void ShTextureNode::setTexSize(int w)
{
  m_width = w;
  ShAttrib1f s(m_texSizeVar.node(), ShSwizzle(m_texSizeVar().size()), false);
  s = static_cast<float>(w);
}

void ShTextureNode::setTexSize(int w, int h)
{
  m_width = w;
  m_height = h;
  ShAttrib2f s(m_texSizeVar.node(), ShSwizzle(m_texSizeVar().size()), false);
  s = ShAttrib2f(w, h);
}

void ShTextureNode::setTexSize(int w, int h, int d)
{
  m_width = w;
  m_height = h;
  m_depth = d;
  ShAttrib3f s(m_texSizeVar.node(), ShSwizzle(m_texSizeVar().size()), false);
  s = ShAttrib3f(w, h, d);
}

int ShTextureNode::width() const
{
  return m_width;
}

int ShTextureNode::height() const
{
  return m_height;
}

int ShTextureNode::depth() const
{
  return m_depth;
}

const ShVariable& ShTextureNode::texSizeVar() const
{
  return m_texSizeVar;
}

}

