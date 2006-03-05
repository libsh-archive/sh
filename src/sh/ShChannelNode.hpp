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

#include "ShDllExport.hpp"
#include "ShVariableNode.hpp"
#include "ShMemory.hpp"

namespace SH {

struct ShChannelNodeImpl;
template<int N, ShBindingType Binding, typename T, ShSemanticType Semantic, bool Swizzled>
class ShAttrib;
typedef ShAttrib<1, SH_TEMP, int, SH_ATTRIB, false> ShAttrib1i;


class
SH_DLLEXPORT ShChannelNode : public ShVariableNode {
public:
  ShChannelNode(ShSemanticType specType, int elements, ShValueType valueType);
  ShChannelNode(ShSemanticType specType, int elements, ShValueType valueType,
                const ShMemoryPtr& memory, int count);
  virtual ~ShChannelNode();

  void memory(const ShMemoryPtr& memory, int count);
  ShPointer<const ShMemory> memory() const;
  ShMemoryPtr memory();
  
  int count() const;
  void count(int count);
  
  int stride() const;
  void stride(int stride);
  const ShAttrib1i& stride_var() const;
  
  int offset() const;
  void offset(int offset);
  const ShAttrib1i& offset_var() const;
  
private:
  ShMemoryPtr m_memory;
  int m_count;

  std::auto_ptr<ShChannelNodeImpl> m_impl;

  // NOT IMPLEMENTED
  ShChannelNode(const ShChannelNode& other);
  ShChannelNode& operator=(const ShChannelNode& other);
};

typedef ShPointer<ShChannelNode> ShChannelNodePtr;
typedef ShPointer<const ShChannelNode> ShChannelNodeCPtr;

}
#endif
