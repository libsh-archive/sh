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

namespace SH {

ShTextureNode::ShTextureNode(ShTextureDims dims, int size,
                             unsigned int traits,
                             int width, int height, int depth)
  : ShVariableNode(SH_TEXTURE, size),
    m_dims(dims),
    m_memory(new ShMemoryPtr[dims == SH_TEXTURE_CUBE ? 6 : 1]),
    m_traits(traits),
    m_width(width), m_height(height), m_depth(depth)
{
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

ShRefCount<const ShMemory> ShTextureNode::memory(int n) const
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

ShRefCount<const ShMemory> ShTextureNode::memory(ShCubeDirection dir) const
{
  return memory(static_cast<int>(dir));
}

void ShTextureNode::memory(ShMemoryPtr mem, ShCubeDirection dir)
{
  memory(mem, static_cast<int>(dir));
}

unsigned int ShTextureNode::traits() const
{
  return m_traits;
}

void ShTextureNode::traits(unsigned int traits)
{
  m_traits = traits;
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

}

