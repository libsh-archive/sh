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
#ifndef SHPALETTENODE_HPP
#define SHPALETTENODE_HPP

#include <cstddef>
#include "ShVariableNode.hpp"

namespace SH {

/** @internal Palette Node Representation
 *
 * Represents a palette (i.e. a uniform array of variables)
 * internally.
 *
 * To use palettes, refer to the ShPalette class instead.
 *
 * @see ShPalette
 */
class SH_DLLEXPORT ShPaletteNode : public ShVariableNode {
public:
  ShPaletteNode(int elements, ShSemanticType semantic, ShValueType valueType, std::size_t length);

  /// Set the VariableNode corresponding to the given index. Only ShPalette should call this.
  void set_node(std::size_t index, const ShVariableNodePtr& node);

  /// Return the number of variables represented by this palette.
  std::size_t palette_length() const;

  /// Return one of the variables represented by this palette.
  ShVariableNodeCPtr get_node(std::size_t index) const;

  /// Return one of the variables represented by this palette.
  ShVariableNodePtr get_node(std::size_t index);

private:
  std::size_t m_length;
  ShVariableNodePtr* m_nodes;
};

typedef ShPointer<ShPaletteNode> ShPaletteNodePtr;
typedef ShPointer<const ShPaletteNode> ShPaletteNodeCPtr;

}

#endif
