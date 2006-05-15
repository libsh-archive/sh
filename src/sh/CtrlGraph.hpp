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
#ifndef SHCTRLGRAPH_HPP
#define SHCTRLGRAPH_HPP

#include <set>
#include <vector>
#include <list>
#include <iosfwd>
#include "DllExport.hpp"
#include "RefCount.hpp"
#include "BasicBlock.hpp"
#include "Variable.hpp"
#include "Block.hpp"

namespace SH {

class CtrlGraph;
class CtrlGraphNode;

struct
SH_DLLEXPORT CtrlGraphBranch {
  CtrlGraphBranch(CtrlGraphNode* node, Variable cond)
    : node(node), cond(cond)
  {}
  
  CtrlGraphNode *node; ///< The successor node
  Variable cond;       ///< If this is 1, take this branch.
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
 * 
 * Since this graph will often contain cycles (loops, etc), simple reference
 * counting is insufficient. Thus we implement an ownership mechanism whereby
 * each control graph node has a parent ("owner") who manages the life span
 * of the node.
 */
class
SH_DLLEXPORT CtrlGraphNode : public InfoHolder {
private:
  // NOTE: Vectors may not work here since we may erase while iterating
  typedef std::list<CtrlGraphBranch> SuccessorList;
  // May have to handle redundant edges temporarily (they will be removed by optimizer)
  typedef std::list<CtrlGraphNode*> PredecessorList;
  typedef std::set<VariableNodePtr> DeclSet;

public:
  /// Onwer is expected to live for at least the life of this node
  CtrlGraphNode(CtrlGraph *owner);

  /// If the node is part of the graph still, predecessors will be set
  /// to point to the current follower.
  ~CtrlGraphNode();

  BasicBlockPtr block;

  /// Output a graph node _and its successors_ at the given
  /// indentation level.
  std::ostream& print(std::ostream& out, int indent) const;

  /// Output a graph node and its successors in graphviz format.
  /// See http://www.research.att.com/sw/tools/graphviz/ for more details.
  std::ostream& graphviz_dump(std::ostream& out) const;

  /** Splits this control graph node into two nodes A, B, at the given statement.
   * A is this, and keeps all predecessor information, 
   * B is a new node that takes over all successor/follower
   * B is NOT appended by default as a child of A 
   *
   * A contains a block with all the statements up to and including the given iterator.
   * B contains a block with all statements after the given iterator.
   *
   * This is useful for splicing in a control graph between A and B to replace 
   * some statement.
   *
   * Returns B. 
   */
  //Pointer<CtrlGraphNode> 
  //split(BasicBlock::StmtList::iterator stmt);
  
  typedef DeclSet::const_iterator DeclIt;

  /** Adds a temporary declaration to this cfg node */
  void addDecl(const VariableNodePtr& node);

  /** Returns whether this node contains a declaration for the given node */
  bool hasDecl(const VariableNodePtr& node) const;

  /** Inserts the given declarations into this */ 
  void insert_decls(DeclIt f, DeclIt l);

  /** Iterators into the decl set */ 
  DeclIt decl_begin() const;
  DeclIt decl_end() const;

  /// If recursive is true, updates owner on all successors recursively
  void change_owner(CtrlGraph *new_owner, bool recursive = true);


  /// Successor types and functions  
  typedef SuccessorList::iterator SuccessorIt;
  typedef SuccessorList::const_iterator SuccessorConstIt;

  SuccessorIt successors_begin() { return m_successors.begin(); }
  SuccessorIt successors_end() { return m_successors.end();   }
  SuccessorConstIt successors_begin() const { return m_successors.begin(); }
  SuccessorConstIt successors_end() const { return m_successors.end();   }
  SuccessorList::size_type successors_size() const { return m_successors.size(); }
  bool successors_empty() const { return m_successors.empty(); }

  /// Erase the given successor, returning a new iterator to the following one
  SuccessorIt successors_erase(SuccessorIt i);

  /// Replace the given conditional successor with a different node
  /// Similar to the follower replacement, it will drop the reference to the
  /// current successor if we are the only predecessor.
  void replace_successor(SuccessorIt current, CtrlGraphNode* new_node);

  /// Append an conditional successor, if node is not null.
  /// Takes ownership of the new subtree.
  void append(CtrlGraphNode* node, Variable cond);

  /// Append an unconditional successor, if node is not null.
  /// NOTE: Current follower must be null! Otherwise, use follower(...) below.
  /// Takes ownership of the new subtree.
  void append(CtrlGraphNode* node);  


  /// Predecessor types and functions  
  typedef PredecessorList::const_iterator PredecessorIt;

  PredecessorIt predecessors_begin() const { return m_predecessors.begin(); }
  PredecessorIt predecessors_end() const   { return m_predecessors.end();   }
  PredecessorList::size_type predecessors_size() const { return m_predecessors.size(); }
  bool predecessors_empty() const { return m_predecessors.empty(); }

  /// Follower functions
  CtrlGraphNode * follower() const { return m_follower; }

  /// Update the follower
  /// Note that this will drop the reference to the previous follower if we were the
  /// only predecessor!
  void follower(CtrlGraphNode *new_follower);


  typedef std::map<const CtrlGraphNode*, CtrlGraphNode*> CloneMap;

  /// Implements a DEEP COPY of this node and successors, maintaining relative
  /// relationships and ownership.
  CtrlGraphNode * clone() const;

  /// Alternate version of clone that uses and fills the given clone map
  CtrlGraphNode * clone(CloneMap *clone_map) const;

  /// Whether this node has been "marked". Useful for mark and sweep
  /// type algorithms.
  bool marked() const;
  
  // Technically these should not be const. But they're useful in
  // functions which are const, so we just make the "marked" flag
  // mutable.
  void mark() const;         ///< Set the marked flag
  void clear_marked() const; ///< Clears the marked flag of this and
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

  /// Internal helper function to drop references to successors
  /// If we are the only predecessor,
  /// it will also drop the reference to that node totally. Thus after
  /// this function the pointer may no longer be valid.
  /// The user function is responsible for actually removing the pointer
  /// to that node if necessary (since they know where it came from).
  void remove_successor_reference(CtrlGraphNode* node);

  /// Each node is owned by a single control graph at all times
  CtrlGraph *m_owner;

  SuccessorList m_successors;      ///< Conditional successors
  CtrlGraphNode* m_follower;     ///< Unconditional successor  
  PredecessorList m_predecessors;

  mutable bool m_marked;

  DeclSet m_decls; ///< Temporary declarations in this node
};



/** A control-flow graph.
 * This is the parsed form of a shader body.
 */
class
SH_DLLEXPORT CtrlGraph : public RefCountable {
public:
  CtrlGraph(CtrlGraphNode* head, CtrlGraphNode* tail);
  CtrlGraph(const BlockListPtr& blocks);
  ~CtrlGraph();

  std::ostream& print(std::ostream& out, int indent) const;
  
  std::ostream& graphviz_dump(std::ostream& out) const;

  /// Use these with care... respect the ownership system!
  CtrlGraphNode* entry() const { return m_entry; }
  CtrlGraphNode* exit() const  { return m_exit;  }

  /// Adds an empty node before entry, gives old entry a block and returns it.
  // New entry is marked (so it does not prevent clearMarking on future DFSes)
  CtrlGraphNode* prepend_entry();

  /// Adds an empty node after exit, gives old exit a block and returns it.
  CtrlGraphNode* append_exit();

  /// Prepends another ctrl graph to this' entry (updating entry)
  /// The passed cfg will be destructed at the end of this function, as ownership
  /// is taken over by this cfg.
  void prepend(const std::auto_ptr<CtrlGraph> cfg);

  /// appends another ctrl graph to this' exit (updating exit)
  /// The passed cfg will be destructed at the end of this function, as ownership
  /// is taken over by this cfg.
  void append(const std::auto_ptr<CtrlGraph> cfg);


  template<typename F>
  void dfs(F& functor);
  
  template<typename F>
  void dfs(F& functor) const;

  // Make a copy of this control graph.
  CtrlGraph * clone() const;
   
  /// Release a node owned by this graph
  /// Be really sure that you know what you're doing here, and that the
  /// node is unreferenced (as it might be deleted if allow_delete is true!)  
  void release_owned_node(CtrlGraphNode* node, bool allow_delete = true);

private:
  CtrlGraphNode* m_entry;
  CtrlGraphNode* m_exit;

  typedef std::set<CtrlGraphNode*> NodeSet;
  NodeSet m_owned_nodes;

  /// Owner reference management functions
  void add_owned_node(CtrlGraphNode *node) { m_owned_nodes.insert(node); }
  void release_all_owned_nodes();

  // Allow nodes to call node management functions
  friend class CtrlGraphNode;  
};

typedef Pointer<CtrlGraph> CtrlGraphPtr;
typedef Pointer<const CtrlGraph> CtrlGraphCPtr;

template<typename F>
void CtrlGraphNode::real_dfs(F& functor)
{
  if (this == 0) return;
  if (m_marked) return;
  mark();
  functor(this);
  for (SuccessorList::iterator I = m_successors.begin(); I != m_successors.end(); ++I) {
    I->node->real_dfs(functor);
  }
  if (m_follower) m_follower->real_dfs(functor);
}

template<typename F>
void CtrlGraphNode::real_dfs(F& functor) const
{
  if (this == 0) return;
  if (m_marked) return;
  mark();
  functor(this);
  for (SuccessorList::const_iterator I = m_successors.begin();
       I != m_successors.end(); ++I) {
    I->node->real_dfs(functor);
  }
  if (m_follower) m_follower->real_dfs(functor);
}

template<typename F>
void CtrlGraphNode::dfs(F& functor)
{
  clear_marked();
  real_dfs(functor);
  clear_marked();
}

template<typename F>
void CtrlGraphNode::dfs(F& functor) const
{
  clear_marked();
  real_dfs(functor);
  clear_marked();
}

template<typename F>
void CtrlGraph::dfs(F& functor)
{
  m_entry->dfs(functor);
}

template<typename F>
void CtrlGraph::dfs(F& functor) const
{
  m_entry->dfs(functor);
}

}

#endif
