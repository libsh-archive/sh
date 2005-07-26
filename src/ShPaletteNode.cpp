// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#include "ShPaletteNode.hpp"

namespace SH {

ShPaletteNode::ShPaletteNode(int elements, ShSemanticType semantic, ShValueType valueType, std::size_t length)
  : ShVariableNode(SH_PALETTE, elements, valueType, semantic),
    m_length(length),
    m_nodes(new ShVariableNodePtr[length])
{
}

void ShPaletteNode::set_node(std::size_t i, const ShVariableNodePtr& node)
{
  m_nodes[i] = node;
}

std::size_t ShPaletteNode::palette_length() const
{
  return m_length;
}

ShVariableNodeCPtr ShPaletteNode::get_node(std::size_t index) const
{
  return m_nodes[index];
}

ShVariableNodePtr ShPaletteNode::get_node(std::size_t index)
{
  return m_nodes[index];
}

}
