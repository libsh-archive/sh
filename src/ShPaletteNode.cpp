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
#include "ShPaletteNode.hpp"

namespace SH {

ShPaletteNode::ShPaletteNode(int elements, ShSemanticType semantic, std::size_t length)
  : ShVariableNode(SH_PALETTE, elements, semantic),
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
