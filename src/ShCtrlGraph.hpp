#ifndef SHCTRLGRAPH_HPP
#define SHCTRLGRAPH_HPP

#include <vector>
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
  ShVariable cond; ///< For conditional branches. Can be null.
};

class ShCtrlGraphNode : public ShRefCountable {
public:
  ShCtrlGraphNode();

  ShBasicBlockPtr block;
  std::vector<ShCtrlGraphBranch> successors; ///< Conditional successors
  ShRefCount<ShCtrlGraphNode> follower; ///< Unconditional successor,
                                        ///  if any

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
                            ///  all successors' flags
  
private:
  mutable bool m_marked;
};

typedef ShRefCount<ShCtrlGraphNode> ShCtrlGraphNodePtr;

/** A control-flow graph.
 * This is the parsed form of a shader body.
 */
class ShCtrlGraph : public ShRefCountable {
public:
  ShCtrlGraph(ShBlockListPtr blocks);
  ~ShCtrlGraph();

  std::ostream& print(std::ostream& out, int indent) const;
  
  std::ostream& graphvizDump(std::ostream& out) const;

  ShCtrlGraphNodePtr entry() const;

private:
  ShCtrlGraphNodePtr m_entry;
  ShCtrlGraphNodePtr m_exit;

  //  void parse(ShCtrlGraphNodePtr& tail, ShBlockListPtr blocks);
};

typedef ShRefCount<ShCtrlGraph> ShCtrlGraphPtr;

}

#endif
