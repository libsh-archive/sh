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
#ifndef SHSTRUCTURAL_HPP
#define SHSTRUCTURAL_HPP

#include <list>
#include <utility>
#include <iosfwd>
#include "ShDllExport.hpp"
#include "ShRefCount.hpp"
#include "ShInfo.hpp"
#include "ShCtrlGraph.hpp"
#include "ShVariable.hpp"
#include "ShStatement.hpp"

namespace SH {

class
SH_DLLEXPORT ShStructuralNode : public virtual ShRefCountable /*, public virtual ShInfo */ {
public:
  friend class ShStructural;
  
  enum NodeType {
    UNREDUCED,
    BLOCK,
    SECTION,
    IF, ///< never used - we only make ifelses
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
  bool contains(ShCtrlGraphNodePtr node) const; ///< Contains the cfg_node in this region
  
  // Graph structure
  ShCtrlGraphNodePtr cfg_node;
  typedef std::pair<ShVariable, ShPointer<ShStructuralNode> > SuccessorEdge;
  typedef std::list<SuccessorEdge> SuccessorList;
  SuccessorList succs;
  typedef std::pair<ShVariable, ShStructuralNode*> PredecessorEdge;
  typedef std::list<PredecessorEdge> PredecessorList;
  PredecessorList preds;

  // Functions to access underlying CFG structure
  
  /** Flags whether entry is marked by a STARTSEC and
   * exit marked by an ENDSEC that has not already been
   * handled in a SECTION reduced block 
   * Remember to set these when building reduced nodes */
  ShStatement *secStart, *secEnd; 
 
 
  /** Describes a cfg edge.
   * Used in searching for cfg edges that match criteria in the structural
   * graph.
   */
  struct CfgMatch {
    ShCtrlGraphNodePtr from, to;
    ShCtrlGraphNode::SuccessorList::iterator S; //< set to succ.end() if edge is from->follower 

    CfgMatch();
    CfgMatch(ShCtrlGraphNodePtr from); //< for a follower
    CfgMatch(ShCtrlGraphNodePtr from, 
        ShCtrlGraphNode::SuccessorList::iterator S); //< for a successor 
        
    bool isFollower(); 
  };
  typedef std::list<CfgMatch> CfgMatchList; 

  // Retrieves successors from CFG that match the given SuccessorEdge
  void getSuccs(CfgMatchList &result, const SuccessorEdge &edge);
  void getPreds(CfgMatchList &result, const PredecessorEdge &edge);
  
  // Retrieves all successors from CFG in this node that leave the given node 
  // node = 0 indicates the typical case of node = this;
  void getExits(CfgMatchList &result, ShPointer<ShStructuralNode> node = 0);

  // Retrieves all successor edges from CFG in this node that enter the given node 
  // node = 0 indicates the typical case of node = this
  void getEntries(CfgMatchList &result, ShPointer<ShStructuralNode> node = 0);


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
