#ifndef SHSTRUCTURAL_HPP
#define SHSTRUCTURAL_HPP

#include <list>
#include <utility>
#include <iosfwd>
#include "ShRefCount.hpp"
#include "ShCtrlGraph.hpp"
#include "ShVariable.hpp"

namespace SH {

class
SH_DLLEXPORT ShStructuralNode : public ShRefCountable {
public:
  friend class ShStructural;
  
  enum NodeType {
    UNREDUCED,
    BLOCK,
    IF,
    IFELSE,
    SELFLOOP,
    WHILELOOP
  };

  ShStructuralNode(const ShCtrlGraphNodePtr& node);
  ShStructuralNode(NodeType type);

  // Graphviz-format dump of this node and its children
  std::ostream& dump(std::ostream& out, int nodes = -1) const;

  // Structural information
  NodeType type;
  ShStructuralNode* container;
  typedef std::list< ShPointer< ShStructuralNode> > StructNodeList;
  StructNodeList structnodes; ///< Nodes in this region
  
  // Graph structure
  ShCtrlGraphNodePtr cfg_node;
  typedef std::pair<ShVariable, ShPointer<ShStructuralNode> > SuccessorEdge;
  typedef std::list<SuccessorEdge> SuccessorList;
  SuccessorList succs;
  typedef std::pair<ShVariable, ShStructuralNode*> PredecessorEdge;
  typedef std::list<PredecessorEdge> PredecessorList;
  PredecessorList preds;

  // Spanning tree
  ShStructuralNode* parent;
  typedef std::list< ShPointer<ShStructuralNode> > ChildList;
  ChildList children;

};

typedef ShPointer<ShStructuralNode> ShStructuralNodePtr;
typedef ShPointer<const ShStructuralNode> ShStructuralNodeCPtr;

class
SH_DLLEXPORT ShStructural {
public:
  ShStructural(const ShCtrlGraphPtr& graph);

  // Graphviz-format dump of the structural tree.
  std::ostream& dump(std::ostream& out) const;

  const ShStructuralNodePtr& head();
  
private:
  ShCtrlGraphPtr m_graph;
  ShStructuralNodePtr m_head;

  typedef std::list<ShStructuralNode*> PostorderList;
  PostorderList m_postorder;

  ShStructuralNodePtr build_tree(const ShCtrlGraphNodePtr& node);
  void build_postorder(const ShStructuralNodePtr& node);
};

}

#endif
