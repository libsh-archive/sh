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
#ifndef SHCTRLGRAPH_HPP
#define SHCTRLGRAPH_HPP

#include <vector>
#include <list>
#include <iosfwd>
#include "ShRefCount.hpp"
#include "ShBasicBlock.hpp"
#include "ShVariable.hpp"
#include "ShBlock.hpp"

namespace SH {

class ShCtrlGraphNode;

struct ShCtrlGraphBranch {
  ShCtrlGraphBranch(const ShRefCount<ShCtrlGraphNode>& node,
                    ShVariable cond);
  
  ShRefCount<ShCtrlGraphNode> node; ///< The successor node
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
class ShCtrlGraphNode : public ShRefCountable {
public:
  ShCtrlGraphNode();
  ~ShCtrlGraphNode();

  ShBasicBlockPtr block;
  std::vector<ShCtrlGraphBranch> successors; ///< Conditional successors
  ShRefCount<ShCtrlGraphNode> follower; ///< Unconditional successor

  typedef std::list<ShCtrlGraphNode*> ShPredList;
  ShPredList predecessors;

  /// Output a graph node _and its successors_ at the given
  /// indentation level.
  std::ostream& print(std::ostream& out, int indent) const;

  /// Output a graph node and its successors in graphviz format.
  /// See http://www.research.att.com/sw/tools/graphviz/ for more details.
  std::ostream& graphvizDump(std::ostream& out) const;

  /// Append an unconditional successor, if node is not null.
  void append(const ShRefCount<ShCtrlGraphNode>& node);

  /// Append an conditional successor, if node is not null.
  void append(const ShRefCount<ShCtrlGraphNode>& node,
              ShVariable cond);

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
};

typedef ShRefCount<ShCtrlGraphNode> ShCtrlGraphNodePtr;

/** A control-flow graph.
 * This is the parsed form of a shader body.
 */
class ShCtrlGraph : public ShRefCountable {
public:
  ShCtrlGraph(ShCtrlGraphNodePtr head, ShCtrlGraphNodePtr tail);
  ShCtrlGraph(ShBlockListPtr blocks);
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

  template<typename F>
  void dfs(F& functor);
  
  template<typename F>
  void dfs(F& functor) const;
  
  /// Determine the predecessors in this graph
  void computePredecessors();
  
private:
  ShCtrlGraphNodePtr m_entry;
  ShCtrlGraphNodePtr m_exit;

  //  void parse(ShCtrlGraphNodePtr& tail, ShBlockListPtr blocks);
};

typedef ShRefCount<ShCtrlGraph> ShCtrlGraphPtr;

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
