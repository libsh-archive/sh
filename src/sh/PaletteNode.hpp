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
#ifndef SHPALETTENODE_HPP
#define SHPALETTENODE_HPP

#include <cstddef>
#include "VariableNode.hpp"

namespace SH {

/** @internal Palette Node Representation
 *
 * Represents a palette (i.e. a uniform array of variables)
 * internally.
 *
 * To use palettes, refer to the Palette class instead.
 *
 * @see Palette
 */
class DLLEXPORT PaletteNode : public VariableNode {
public:
  PaletteNode(int elements, SemanticType semantic, ValueType valueType, std::size_t length);

  /// Set the VariableNode corresponding to the given index. Only Palette should call this.
  void set_node(std::size_t index, const VariableNodePtr& node);

  /// Return the number of variables represented by this palette.
  std::size_t palette_length() const;

  /// Return one of the variables represented by this palette.
  VariableNodeCPtr get_node(std::size_t index) const;

  /// Return one of the variables represented by this palette.
  VariableNodePtr get_node(std::size_t index);

private:
  std::size_t m_length;
  VariableNodePtr* m_nodes;
};

typedef Pointer<PaletteNode> PaletteNodePtr;
typedef Pointer<const PaletteNode> PaletteNodeCPtr;

}

#endif
