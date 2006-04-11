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
#include "DllExport.hpp"
#include "RefCount.hpp"
#include "Info.hpp"
#include "CtrlGraph.hpp"
#include "Variable.hpp"
#include "Statement.hpp"

namespace SH {

class
DLLEXPORT StructuralNode : public virtual RefCountable /*, public virtual Info */ {
public:
  friend class Structural;
  
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

  StructuralNode(const CtrlGraphNodePtr& node);
  StructuralNode(NodeType type);

  // Graphviz-format dump of this node and its children
  std::ostream& dump(std::ostream& out, int nodes = -1) const;

  // Structural information
  NodeType type;
  StructuralNode* container;
  typedef std::list< Pointer< StructuralNode> > StructNodeList;
  StructNodeList structnodes; ///< Nodes in this region
  bool contains(const CtrlGraphNodePtr& node) const; ///< Contains the cfg_node in this region
  
  // Graph structure
  CtrlGraphNodePtr cfg_node;
  typedef std::pair<Variable, Pointer<StructuralNode> > SuccessorEdge;
  typedef std::list<SuccessorEdge> SuccessorList;
  SuccessorList succs;
  typedef std::pair<Variable, StructuralNode*> PredecessorEdge;
  typedef std::list<PredecessorEdge> PredecessorList;
  PredecessorList preds;

  // Functions to access underlying CFG structure
  
  /** Flags whether entry is marked by a STARTSEC and
   * exit marked by an ENDSEC that has not already been
   * handled in a SECTION reduced block 
   * Remember to set these when building reduced nodes */
  Statement *secStart, *secEnd; 
 
 
  /** Describes a cfg edge.
   * Used in searching for cfg edges that match criteria in the structural
   * graph.
   */
  struct DLLEXPORT CfgMatch {
    CtrlGraphNodePtr from, to;
    CtrlGraphNode::SuccessorList::iterator S; //< set to succ.end() if edge is from->follower 

    CfgMatch();
    CfgMatch(const CtrlGraphNodePtr& from); //< for a follower
    CfgMatch(const CtrlGraphNodePtr& from, 
        CtrlGraphNode::SuccessorList::iterator S); //< for a successor 
        
    bool isFollower(); 
  };
  typedef std::list<CfgMatch> CfgMatchList; 

  // Retrieves successors from CFG that match the given SuccessorEdge
  void getSuccs(CfgMatchList &result, const SuccessorEdge &edge);
  void getPreds(CfgMatchList &result, const PredecessorEdge &edge);
  
  // Retrieves all successors from CFG in this node that leave the given node 
  // node = 0 indicates the typical case of node = this;
  void getExits(CfgMatchList &result, Pointer<StructuralNode> node = 0);

  // Retrieves all successor edges from CFG in this node that enter the given node 
  // node = 0 indicates the typical case of node = this
  void getEntries(CfgMatchList &result, Pointer<StructuralNode> node = 0);


  // Spanning tree
  StructuralNode* parent;
  typedef std::list< Pointer<StructuralNode> > ChildList;
  ChildList children;

};

typedef Pointer<StructuralNode> StructuralNodePtr;
typedef Pointer<const StructuralNode> StructuralNodeCPtr;

class
DLLEXPORT Structural {
public:
  Structural(const CtrlGraphPtr& graph);

  // Graphviz-format dump of the structural tree.
  std::ostream& dump(std::ostream& out) const;

  const StructuralNodePtr& head();
  
private:
  CtrlGraphPtr m_graph;
  StructuralNodePtr m_head;

  typedef std::list<StructuralNode*> PostorderList;
  PostorderList m_postorder;

  StructuralNodePtr build_tree(const CtrlGraphNodePtr& node, std::map<CtrlGraphNodePtr, StructuralNodePtr>& nodemap);
  void build_postorder(const StructuralNodePtr& node);
};

}

#endif
