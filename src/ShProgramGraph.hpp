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
#ifndef SHPRGMGRAPH_HPP
#define SHPRGMGRAPH_HPP

#include <vector>
#include <list>
#include <iosfwd>
#include "ShProgram.hpp"

namespace SH {

class ShPrgmGraphNode : public ShRefCountable {
public:
  ShPrgmGraphNode();
  ShPrgmGraphNode(ShProgram prg);

  ShProgram program;
  ShRefCount<ShPrgmGraphNode> next, prev;

  /// Output a graph node _and its successors_ at the given
  /// indentation level.
  std::ostream& print(std::ostream& out, int indent) const;

  /// Output a graph node and its successors in graphviz format.
  /// See http://www.research.att.com/sw/tools/graphviz/ for more details.
  std::ostream& graphvizDump(std::ostream& out) const;

  /// Append a successor, if node is not null.
  void append(const ShRefCount<ShPrgmGraphNode>& node);    
  void append(ShProgram prgm);
  
  /// Attach a program
  void attach(ShProgram prgm);

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

private:
  mutable bool m_marked;
};

typedef ShRefCount<ShPrgmGraphNode> ShPrgmGraphNodePtr;

/** A control-flow graph.
 * This is the parsed form of a shader body.
 */
class ShPrgmGraph : public ShRefCountable {
public:
  ShPrgmGraph();
  ~ShPrgmGraph();


  // only a linear structure for now
  void append(ShPrgmGraphNodePtr);
  void append(ShProgram);
  
  // hack for now, consider only a linear structure
  ShProgram& getProgram(int i);
  int numPrograms();

  // get the size
  int getSize();

  // the root of th eprogram
  ShPrgmGraphNodePtr root;

  std::ostream& print(std::ostream& out, int indent) const;
  std::ostream& graphvizDump(std::ostream& out) const;

  template<typename F>
  void dfs(F& functor);
  
};

typedef ShRefCount<ShPrgmGraph> ShComplexProgram;

template<typename F>
void ShPrgmGraphNode::dfs(F& functor)
{
  if (this == 0) return;
  if (m_marked) return;
  mark();
  functor(this);
  for (std::vector<ShCtrlGraphBranch>::iterator I = successors.begin(); I != successors.end(); ++I) {
    I->node->dfs(functor);
  }
  if (follower) follower->dfs(functor);
}

template<typename F>
void ShPrgmGraph::dfs(F& functor)
{
    //m_entry->clearMarked();
    //m_entry->dfs(functor);
    //m_entry->clearMarked();
}

}

#endif
