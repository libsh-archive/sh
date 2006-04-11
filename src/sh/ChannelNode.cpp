// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
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
#include "ChannelNode.hpp"
#include "Attrib.hpp"

namespace SH {

ChannelNode::ChannelNode(SemanticType specType, int elements, ValueType valueType)
  : VariableNode(SH_STREAM, elements, valueType),
    m_memory(0), m_count(0), m_stride(1), m_offset(0)
{
  specialType(specType);
}

ChannelNode::ChannelNode(SemanticType specType, int elements, ValueType valueType,
                             const MemoryPtr& memory, int count)
  : VariableNode(SH_STREAM, elements, valueType),
    m_memory(memory), m_count(count), m_stride(1), m_offset(0)
{
  specialType(specType);
}

ChannelNode::~ChannelNode()
{
}

void ChannelNode::memory(const MemoryPtr& memory, int count)
{
  m_memory = memory;
  m_count = count;
}

Pointer<const Memory> ChannelNode::memory() const
{
  return m_memory;
}

MemoryPtr ChannelNode::memory() 
{
  return m_memory;
}

}

