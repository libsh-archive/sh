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
#ifndef SHBLOCK_HPP
#define SHBLOCK_HPP

#include <vector>
#include <iosfwd>
#include "ShDllExport.hpp"
#include "ShRefCount.hpp"
#include "ShStatement.hpp"

namespace SH {

/** A generic block or token.
 * These are either basic blocks or tokens.
 */
class
SH_DLLEXPORT ShBlock : public ShRefCountable {
public:
  virtual ~ShBlock();

  /// Output a textual representation of this control statement.
  virtual void print(std::ostream& out, int indent) const = 0;
};

typedef ShPointer<ShBlock> ShBlockPtr;
typedef ShPointer<const ShBlock> ShBlockCPtr;

/** A list of generic blocks.
 */
class
SH_DLLEXPORT ShBlockList : public ShRefCountable {
public:
  ShBlockList(bool isArgument = false);

  /// True iff this block list is an argument (e.g. to sh_IF)
  bool isArgument();

  /// Add a simple "three variable" statement.
  void addStatement(const ShStatement& statement);
  /// Add a generic block.
  void addBlock(const ShBlockPtr& statement);

  /// Return the front block from the list (does not remove it)
  ShBlockPtr ShBlockList::getFront() const;
  
  /// Remove the front block from the list and return it
  ShBlockPtr ShBlockList::removeFront();

  /// Return true iff this list does not contain any blocks.
  bool empty() const;

  /// Output a token list
  friend SH_DLLEXPORT std::ostream& operator<<(std::ostream& out, const ShBlockList& blockList);

  /// Output a token list at a given indentation
  std::ostream& print(std::ostream& out, int indentation) const;

private:
  bool m_isArgument;
  std::vector<ShBlockPtr> m_blocks;
};

typedef ShPointer<ShBlockList> ShBlockListPtr;
typedef ShPointer<const ShBlockList> ShBlockListCPtr;

}

#endif
