// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#include "ShChannelNode.hpp"

namespace SH {

  ShChannelNode::ShChannelNode(ShSemanticType specType, int elements, ShValueType valueType/*, int stride, int offset*/)
  : ShVariableNode(SH_STREAM, elements, valueType),
  m_memory(0), m_count(0)//, m_stride(stride), m_offset(offset)
{
  specialType(specType);
}

ShChannelNode::ShChannelNode(ShSemanticType specType, int elements, ShValueType valueType,
                             const ShMemoryPtr& memory, int count/*, int stride, int offset*/)
  : ShVariableNode(SH_STREAM, elements, valueType),
  m_memory(memory), m_count(count)//, m_stride(stride), m_offset(offset)
{
  specialType(specType);
}

ShChannelNode::~ShChannelNode()
{
}

void ShChannelNode::memory(const ShMemoryPtr& memory, int count)
{
  m_memory = memory;
  m_count = count;
}

ShPointer<const ShMemory> ShChannelNode::memory() const
{
  return m_memory;
}

ShMemoryPtr ShChannelNode::memory() 
{
  return m_memory;
}

int ShChannelNode::count() const
{
  return m_count;
}

int &ShChannelNode::stride()
{
  return m_stride;
}

int &ShChannelNode::offset()
{
  return m_offset;
}

}
