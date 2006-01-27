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
#ifndef SHCTRLGRAPH_HPP
#define SHCTRLGRAPH_HPP

#include <set>
#include <vector>
#include <list>
#include <iosfwd>
#include "ShDllExport.hpp"
#include "ShRefCount.hpp"
#include "ShBasicBlock.hpp"
#include "ShVariable.hpp"
#include "ShBlock.hpp"

namespace SH {

class ShCtrlGraphNode;

struct
SH_DLLEXPORT ShCtrlGraphBranch {
  ShCtrlGraphBranch(const ShPointer<ShCtrlGraphNode>& node,
                    ShVariable cond);
  
  ShPointer<ShCtrlGraphNode> node; ///< The successor node
  ShVariable cond; ///< If this is 1, take this branch.
};

/** A node in the control graph.
 * This contains of (optionally) some code, in the form of a basic
 * block, 0 or more conditional successors (nodes which will be
 * branched to if a particular variable is greater than 0) and one
 * unconditional successor, which is another node that will be
 * branched to if none of the conditional successors' conditions are true.
 *
 * Only the exit node of a control graph will have an unconditional
 * successor of 0.
 */
class
SH_DLLEXPORT ShCtrlGraphNode : public ShRefCountable, public ShInfoHolder {
public:
  ShCtrlGraphNode();
  ~ShCtrlGraphNode();

  ShBasicBlockPtr block;
  typedef std::vector<ShCtrlGraphBranch> SuccessorList;
  SuccessorList successors; ///< Conditional successors
  ShPointer<ShCtrlGraphNode> follower; ///< Unconditional successor

  typedef std::list<ShCtrlGraphNode*> ShPredList;
  ShPredList predecessors;

  /// Output a graph node _and its successors_ at the given
  /// indentation level.
  std::ostream& print(std::ostream& out, int indent) const;

  /// Output a graph node and its successors in graphviz format.
  /// See http://www.research.att.com/sw/tools/graphviz/ for more details.
  std::ostream& graphvizDump(std::ostream& out) const;

  /// Append an unconditional successor, if node is not null.
  void append(const ShPointer<ShCtrlGraphNode>& node);

  /// Append an conditional successor, if node is not null.
  void append(const ShPointer<ShCtrlGraphNode>& node,
              ShVariable cond);

  /** Splits this control graph node into two nodes A, B, at the given statement.
   * A is this, and keeps all predecessor information, 
   * B is a new node that takes over all successor/follower
   * B is NOT appended by default as a child of A 
   *
   * A contains a block with all the statements up to and including the given iterator.
   * B contains a block with all statements after the given iterator.
   *
   * This is useful for splicing in a control graph between A and B to replace 
   * some statement (or TODO a sequence of statements)
   *
   * Returns B. 
   */
  ShPointer<ShCtrlGraphNode> 
  split(ShBasicBlock::ShStmtList::iterator stmt);

  typedef std::set<ShVariableNodePtr> DeclSet;
  typedef DeclSet::const_iterator DeclIt;

  /** Adds a temporary declaration to this cfg node */
  void addDecl(const ShVariableNodePtr& node);

  /** Returns whether this node contains a declaration for the given node */
  bool hasDecl(const ShVariableNodePtr& node) const;

  /** Inserts the given declarations into this */ 
  void insert_decls(DeclIt f, DeclIt l);

  /** Iterators into the decl set */ 
  DeclIt decl_begin() const;
  DeclIt decl_end() const;


  /// Whether this node has been "marked". Useful for mark and sweep
  /// type algorithms.
  bool marked() const;
  
  // Technically these should not be const. But they're useful in
  // functions which are const, so we just make the "marked" flag
  // mutable.
  void mark() const; ///< Set the marked flag
  void clearMarked() const; ///< Clears the marked flag of this and
                            ///  all successors'/followers flags
  
  template<typename F>
  void dfs(F& functor);

  template<typename F>
  void dfs(F& functor) const;

private:
  template<typename F>
  void real_dfs(F& functor);

  template<typename F>
  void real_dfs(F& functor) const;

  mutable bool m_marked;

  DeclSet m_decls; ///< temporary declarations in this node
};

typedef ShPointer<ShCtrlGraphNode> ShCtrlGraphNodePtr;
typedef ShPointer<const ShCtrlGraphNode> ShCtrlGraphNodeCPtr;

/** A control-flow graph.
 * This is the parsed form of a shader body.
 */
class
SH_DLLEXPORT ShCtrlGraph : public ShRefCountable {
public:
  ShCtrlGraph(const ShCtrlGraphNodePtr& head, const ShCtrlGraphNodePtr& tail);
  ShCtrlGraph(const ShBlockListPtr& blocks);
  ~ShCtrlGraph();

  std::ostream& print(std::ostream& out, int indent) const;
  
  std::ostream& graphvizDump(std::ostream& out) const;

  ShCtrlGraphNodePtr entry() const;
  ShCtrlGraphNodePtr exit() const;

  /// Adds an empty node before entry, gives old entry a block and returns it.
  // New entry is marked (so it does not prevent clearMarking on future DFSes)
  ShCtrlGraphNodePtr prependEntry();

  /// Adds an empty node after exit, gives old exit a block and returns it. 
  ShCtrlGraphNodePtr appendExit();

  /// prepends another ctrl graph to this' entry (updating entry) 
  void prepend(ShPointer<ShCtrlGraph> cfg); 

  /// appends another ctrl graph to this' exit (updating exit)
  void append(ShPointer<ShCtrlGraph> cfg); 


  template<typename F>
  void dfs(F& functor);
  
  template<typename F>
  void dfs(F& functor) const;
  
  /// Determine the predecessors in this graph
  void computePredecessors();

  // Make a copy of this control graph placing the result into head and tail.
  void copy(ShCtrlGraphNodePtr& head, ShCtrlGraphNodePtr& tail) const;
  
private:
  ShCtrlGraphNodePtr m_entry;
  ShCtrlGraphNodePtr m_exit;

  //  void parse(ShCtrlGraphNodePtr& tail, ShBlockListPtr blocks);
};

typedef ShPointer<ShCtrlGraph> ShCtrlGraphPtr;
typedef ShPointer<const ShCtrlGraph> ShCtrlGraphCPtr;

template<typename F>
void ShCtrlGraphNode::real_dfs(F& functor)
{
  if (this == 0) return;
  if (m_marked) return;
  mark();
  functor(this);
  for (std::vector<ShCtrlGraphBranch>::iterator I = successors.begin(); I != successors.end(); ++I) {
    I->node->real_dfs(functor);
  }
  if (follower) follower->real_dfs(functor);
}

template<typename F>
void ShCtrlGraphNode::real_dfs(F& functor) const
{
  if (this == 0) return;
  if (m_marked) return;
  mark();
  functor(this);
  for (std::vector<ShCtrlGraphBranch>::const_iterator I = successors.begin();
       I != successors.end(); ++I) {
    I->node->real_dfs(functor);
  }
  if (follower) follower->real_dfs(functor);
}

template<typename F>
void ShCtrlGraphNode::dfs(F& functor)
{
  clearMarked();
  real_dfs(functor);
  clearMarked();
}

template<typename F>
void ShCtrlGraphNode::dfs(F& functor) const
{
  clearMarked();
  real_dfs(functor);
  clearMarked();
}

template<typename F>
void ShCtrlGraph::dfs(F& functor)
{
  m_entry->dfs(functor);
}

template<typename F>
void ShCtrlGraph::dfs(F& functor) const
{
  m_entry->dfs(functor);
}

}

#endif