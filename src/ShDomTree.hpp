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

#ifndef SHDOMTREE_HPP
#define SHDOMTREE_HPP

#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include "ShDllExport.hpp"
#include "ShCtrlGraph.hpp"
#include "ShRefCount.hpp"

namespace SH {

  /** A dominator tree in a flowgraph.
   */
class
SH_DLLEXPORT ShDomTree {
  public:
    typedef std::set<ShCtrlGraphNodePtr> CfgNodeSet;
    typedef std::map<ShCtrlGraphNodePtr, CfgNodeSet> CfgNodeSetMap;

    ShDomTree(ShCtrlGraphPtr ctrlGraph);

    /// Postorder (bottom-up) traversal.
    template<typename T> void postorder(T& f);

    /// Preorder traversal.
    template<typename T> void preorder(T& f);

    /// Returns the successors of a node in the cfg 
    const CfgNodeSet& succ(ShCtrlGraphNodePtr node) const;

    /// Returns the predecessors of a node in the cfg 
    const CfgNodeSet& pred(ShCtrlGraphNodePtr node) const;

    /// Returns the immediate dominator of a given cfg node
    /// or 0 if passed the root. 
    ShCtrlGraphNodePtr idom(ShCtrlGraphNodePtr node) const; 

    /// Returns the set of nodes that a given cfg node immediately dominates
    const CfgNodeSet& idom_by(ShCtrlGraphNodePtr node) const;

    /// Returns the dominance frontier of node   
    const CfgNodeSet& df(ShCtrlGraphNodePtr node) const;

    /// Returns the dominance frontier of a set of nodes  
    /// Takes time proportional to total sum of |df(x)| for x in nodes  
    CfgNodeSet df(const CfgNodeSet &nodes) const;

    /// Returns the iterated dominance frontier 
    CfgNodeSet df_plus(const CfgNodeSet &nodes) const;

  
    void debugDump();

    int numbering(ShCtrlGraphNodePtr node) const {
      return (int)(std::find( m_vertex.begin(), m_vertex.end(), node) - m_vertex.begin());
    }
  
  private:
    void dfs(ShCtrlGraphNodePtr v);
    ShCtrlGraphNodePtr eval(ShCtrlGraphNodePtr v);
    void compress(ShCtrlGraphNodePtr v);
    void link(ShCtrlGraphNodePtr v, ShCtrlGraphNodePtr w);
  
    ShCtrlGraphPtr m_graph;

    // See Langauer & Tarjan, 1979 in TOPLAS for more info on this algorithm 
    typedef std::map<ShCtrlGraphNodePtr, ShCtrlGraphNodePtr> CfgNodeNodeMap;


    // m_parent(w) = parent of w in the dfs tree  
    CfgNodeNodeMap m_parent;

    // m_succ(v) = nodes w st (v,w) is an edge in the cfg 
    CfgNodeSetMap m_succ;

    // m_pred(w) = nodes v st (v,w) is an edge in the cfg 
    CfgNodeSetMap m_pred;

    // m_semi(w) = index of sdom(w)  
    typedef std::map<ShCtrlGraphNodePtr, int> SemiMap;
    SemiMap m_semi;

    // m_vertex(i) = cfg node with numbering i
    typedef std::vector<ShCtrlGraphNodePtr> CfgNodeVec;
    CfgNodeVec m_vertex;

    // m_bucket(w) = nodes v st semi(v) = w 
    CfgNodeSetMap m_bucket;

    // m_dom(w) = immediate dominator of w
    CfgNodeNodeMap m_dom;

    // @todo range - move all these derivative computations out of this class... 
    // They don't really belong in domtree

    // m_dom_by(w) = { v | m_dom(v) = w } 
    // This gets built on the first call to idom_by()
    mutable CfgNodeNodeMap m_dom_by;

    // m_df(w) = nodes v in the dominance frontier of w
    // This gets built on the first call to df()
    mutable CfgNodeSetMap m_df;

    CfgNodeNodeMap m_ancestor;

    CfgNodeNodeMap m_label;

    // m_children(w) = children of w in the dfs tree
    CfgNodeSetMap m_children;
  
    int m_n;

    template<typename T> void postorderNode(T& f, ShCtrlGraphNodePtr root, int level = 0);
    template<typename T> void preorderNode(T& f, ShCtrlGraphNodePtr root, int level = 0);
};

template<typename T>
void ShDomTree::postorder(T& f)
{
  postorderNode(f, m_graph->entry());
}

template<typename T>
void ShDomTree::postorderNode(T& f, ShCtrlGraphNodePtr root, int level)
{
  ChildrenSet& children = m_children[root];
  for (ChildrenSet::iterator I = children.begin(); I != children.end(); ++I) {
    postorderNode(f, *I, level + 1);
  }
  f(root, level);
}

template<typename T>
void ShDomTree::preorder(T& f)
{
  preorderNode(f, m_graph->entry());
}

template<typename T>
void ShDomTree::preorderNode(T& f, ShCtrlGraphNodePtr root, int level)
{
  f(root, level);
  ChildrenSet& children = m_children[root];
  for (ChildrenSet::iterator I = children.begin(); I != children.end(); ++I) {
    preorderNode(f, *I, level + 1);
  }
}

}

#endif
