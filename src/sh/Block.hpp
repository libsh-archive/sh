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
#ifndef SHBLOCK_HPP
#define SHBLOCK_HPP

#include <vector>
#include <iosfwd>
#include "DllExport.hpp"
#include "RefCount.hpp"
#include "Statement.hpp"

namespace SH {

/** A generic block or token.
 * These are either basic blocks or tokens.
 */
class
SH_DLLEXPORT Block : public RefCountable {
public:
  virtual ~Block();

  /// Output a textual representation of this control statement.
  virtual void print(std::ostream& out, int indent) const = 0;
};

typedef Pointer<Block> BlockPtr;
typedef Pointer<const Block> BlockCPtr;

/** A list of generic blocks.
 */
class
SH_DLLEXPORT BlockList : public RefCountable {
public:
  BlockList(bool isArgument = false);

  /// True iff this block list is an argument (e.g. to sh_IF)
  bool isArgument();

  /// Add a simple "three variable" statement.
  void addStatement(const Statement& statement);
  /// Add a generic block.
  void addBlock(const BlockPtr& statement);

  /// Return the front block from the list (does not remove it)
  BlockPtr getFront() const;
  
  /// Remove the front block from the list and return it
  BlockPtr removeFront();

  /// Return true iff this list does not contain any blocks.
  bool empty() const;

  /// Output a token list
  friend SH_DLLEXPORT std::ostream& operator<<(std::ostream& out, const BlockList& blockList);

  /// Output a token list at a given indentation
  std::ostream& print(std::ostream& out, int indentation) const;

private:
  bool m_isArgument;
  std::vector<BlockPtr> m_blocks;
};

typedef Pointer<BlockList> BlockListPtr;
typedef Pointer<const BlockList> BlockListCPtr;

}

#endif
