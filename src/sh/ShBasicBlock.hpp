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
#ifndef SHBASICBLOCK_HPP
#define SHBASICBLOCK_HPP

#include <list>
#include "ShBlock.hpp"
#include "ShStatement.hpp"

namespace SH {

/** A basic block.
 * A basic block is a block composed of simpler statements, but with
 * no loops or conditionals.   It may only contain arithmetic 
 * operations, texture lookups, and assignments (although some of 
 * these assignments might be conditional).
 */
class 
SH_DLLEXPORT
ShBasicBlock : public ShBlock {
public:
  typedef std::list<ShStatement> ShStmtList;
  typedef ShStmtList::iterator iterator;
  typedef ShStmtList::const_iterator const_iterator;
  typedef ShStmtList::reverse_iterator reverse_iterator;
  typedef ShStmtList::const_reverse_iterator const_reverse_iterator;

  ~ShBasicBlock();

  void print(std::ostream& out, int indent) const;
  void graphvizDump(std::ostream& out) const;

  /**@name Add statement at start.
   * Adds the given statement after the statements in this block */
  void addStatement(const ShStatement& stmt);

  /**@name Add statement at end.
   * Adds the given statement before the statements in this block */
  void prependStatement(const ShStatement& stmt);

  /* List ops 
   * @{ */
  bool empty() { return m_statements.empty(); }

  const_iterator begin() const { return m_statements.begin(); }
  const_iterator end() const { return m_statements.end(); }
  iterator begin() { return m_statements.begin(); }
  iterator end() { return m_statements.end(); } 

  const_reverse_iterator rbegin() const { return m_statements.rbegin(); }
  const_reverse_iterator rend() const { return m_statements.rend(); }
  reverse_iterator rbegin() { return m_statements.rbegin(); }
  reverse_iterator rend() { return m_statements.rend(); } 

  iterator erase(iterator I) { return m_statements.erase(I); }

  // Place all the elements of l before the iterator I and removes them
  // from l
  void splice(iterator I, ShStmtList &l) { m_statements.splice(I, l); }

  // Places all the elements starting from lI in l before the iterator I and
  // removes them from l
  void splice(iterator I, ShStmtList &l, iterator lI) {
    m_statements.splice(I, l, lI, l.end());
  }
  
  ShStmtList m_statements;
//private:
};

typedef ShPointer<ShBasicBlock> ShBasicBlockPtr;
typedef ShPointer<const ShBasicBlock> ShBasicBlockCPtr;

}

#endif
