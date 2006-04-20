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
#include "PaletteNode.hpp"

namespace SH {

PaletteNode::PaletteNode(int elements, SemanticType semantic, ValueType valueType, std::size_t length)
  : VariableNode(SH_PALETTE, elements, valueType, semantic),
    m_length(length),
    m_nodes(new VariableNodePtr[length])
{
}

void PaletteNode::set_node(std::size_t i, const VariableNodePtr& node)
{
  m_nodes[i] = node;
}

std::size_t PaletteNode::palette_length() const
{
  return m_length;
}

VariableNodeCPtr PaletteNode::get_node(std::size_t index) const
{
  return m_nodes[index];
}

VariableNodePtr PaletteNode::get_node(std::size_t index)
{
  return m_nodes[index];
}

}
