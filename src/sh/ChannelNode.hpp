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
#ifndef SHCHANNELNODE_HPP
#define SHCHANNELNODE_HPP

#include "DllExport.hpp"
#include "VariableNode.hpp"
#include "Memory.hpp"

namespace SH {

class
SH_DLLEXPORT ChannelNode : public VariableNode {
public:
  ChannelNode(SemanticType specType, int elements, ValueType valueType);
  ChannelNode(SemanticType specType, int elements, ValueType valueType,
                const MemoryPtr& memory, int count);
  virtual ~ChannelNode();

  void memory(const MemoryPtr& memory, int count);
  Pointer<const Memory> memory() const;
  MemoryPtr memory();
  
  int count() const { return m_count; }
  void count(int count) { m_count = count; }
  
  int stride() const { return m_stride; }
  void stride(int stride) { m_stride = stride; }
  
  int offset() const { return m_offset; }
  void offset(int offset) { m_offset = offset; }
  
private:
  MemoryPtr m_memory;
  int m_count;
  int m_stride;
  int m_offset;

  // NOT IMPLEMENTED
  ChannelNode(const ChannelNode& other);
  ChannelNode& operator=(const ChannelNode& other);
};

typedef Pointer<ChannelNode> ChannelNodePtr;
typedef Pointer<const ChannelNode> ChannelNodeCPtr;

}
#endif
