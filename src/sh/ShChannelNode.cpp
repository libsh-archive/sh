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
#include "ShChannelNode.hpp"

namespace SH {

ShChannelNode::ShChannelNode(ShSemanticType specType, int elements, ShValueType valueType)
  : ShVariableNode(SH_STREAM, elements, valueType),
    m_memory(0), m_count(0)
{
  ShContext::current()->enter(0);
  
  m_stride = ShAttrib1i(1);
  m_offset = ShAttrib1i(0);
  
  ShContext::current()->exit();

  specialType(specType);
}

ShChannelNode::ShChannelNode(ShSemanticType specType, int elements, ShValueType valueType,
                             const ShMemoryPtr& memory, int count)
  : ShVariableNode(SH_STREAM, elements, valueType),
    m_memory(memory), m_count(count)
{
  ShContext::current()->enter(0);
  
  m_stride = ShAttrib1i(1);
  m_offset = ShAttrib1i(0);
  
  ShContext::current()->exit();

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

void ShChannelNode::count(int count)
{
  m_count = count;
}

void ShChannelNode::stride(int stride)
{
  m_stride = ShAttrib1i(stride);
}

void ShChannelNode::offset(int offset)
{
  m_offset = ShAttrib1i(offset);
}

int ShChannelNode::stride() const
{
  return m_stride.getValue(0);
}

int ShChannelNode::offset() const
{
  return m_offset.getValue(0);
}

const ShAttrib1i& ShChannelNode::stride_var() const
{
  return m_stride;
}

const ShAttrib1i& ShChannelNode::offset_var() const
{
  return m_offset;
}

}

