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
    IF,
    IFELSE,
    SELFLOOP,
    WHILELOOP,
    PROPINT
  };

  ShStructuralNode(const ShCtrlGraphNodePtr& node);
  ShStructuralNode(NodeType type);

  // Graphviz-format dump of this node and its children
  std::ostream& dump(std::ostream& out, int nodes = -1) const;

  // Structural information
  NodeType type;
  ShStructuralNode* container;
  typedef std::list<ShStructuralNode*> StructNodeList;
  StructNodeList structnodes; ///< Nodes in this region
  bool contains(const ShCtrlGraphNodePtr& node) const; ///< Contains the cfg_node in this region
  
  // Graph structure
  ShCtrlGraphNodePtr cfg_node;
  typedef std::pair<ShVariable, ShStructuralNode* > SuccessorEdge;
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
  struct SH_DLLEXPORT CfgMatch {
    ShCtrlGraphNodePtr from, to;
    ShCtrlGraphNode::SuccessorList::iterator S; //< set to succ.end() if edge is from->follower 

    CfgMatch();
    CfgMatch(const ShCtrlGraphNodePtr& from); //< for a follower
    CfgMatch(const ShCtrlGraphNodePtr& from, 
        ShCtrlGraphNode::SuccessorList::iterator S); //< for a successor 
        
    bool isFollower(); 
  };
  typedef std::list<CfgMatch> CfgMatchList; 

  // Retrieves successors from CFG that match the given SuccessorEdge
  void getSuccs(CfgMatchList &result, const SuccessorEdge &edge);
  void getPreds(CfgMatchList &result, const PredecessorEdge &edge);
  
  // Retrieves all successors from CFG in this node that leave the given node 
  // node = 0 indicates the typical case of node = this;
  void getExits(CfgMatchList &result, ShStructuralNode* node = 0);

  // Retrieves all successor edges from CFG in this node that enter the given node 
  // node = 0 indicates the typical case of node = this
  void getEntries(CfgMatchList &result, ShStructuralNode* node = 0);


  // Spanning tree
  ShStructuralNode* parent;
  typedef std::list< ShStructuralNode* > ChildList;
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

  ShStructuralNode* head();
  
private:
  ShCtrlGraphPtr m_graph;
  ShStructuralNode* m_head;

  typedef std::list<ShStructuralNode*> PostorderList;
  PostorderList m_postorder;

  ShStructuralNode* build_tree(const ShCtrlGraphNodePtr& node, std::map<ShCtrlGraphNodePtr, ShStructuralNode*>& nodemap);
  void build_postorder(ShStructuralNode* node);


  // Keep ShStructuralNodes alive
  typedef std::list<ShStructuralNodePtr> StructuralNodePtrList;
  StructuralNodePtrList m_live_structural_nodes;
  
  // Structural node factory
  template <typename T>
  ShStructuralNode * create_structural_node(T p)
  {
    ShStructuralNode *i = new ShStructuralNode(p);
    m_live_structural_nodes.push_back(i);
    return i;
  }
};

}

#endif
