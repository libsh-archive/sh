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

#ifndef SHDOMTREE_HPP
#define SHDOMTREE_HPP

#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include "DllExport.hpp"
#include "CtrlGraph.hpp"
#include "RefCount.hpp"

namespace SH {

  /** A dominator tree in a flowgraph.
   */
  class
  SH_DLLEXPORT DomTree {
  public:
    DomTree(CtrlGraphPtr ctrlGraph);

    /// Postorder (bottom-up) traversal.
    template<typename T> void postorder(T& f);

    /// Preorder traversal.
    template<typename T> void preorder(T& f);
  
    void debugDump();

    int numbering(CtrlGraphNodePtr node) const {
      return (int)(std::find( m_vertex.begin(), m_vertex.end(), node) - m_vertex.begin());
    }
  
  private:
    void dfs(CtrlGraphNodePtr v);
    CtrlGraphNodePtr eval(CtrlGraphNodePtr v);
    void compress(CtrlGraphNodePtr v);
    void link(CtrlGraphNodePtr v, CtrlGraphNodePtr w);
  
    CtrlGraphPtr m_graph;

    // See [TODO Ref Langauer & Tarjan pg 127] for more information on these

    typedef std::map<CtrlGraphNodePtr, CtrlGraphNodePtr> ParentMap;
    ParentMap m_parent;
    typedef std::set<CtrlGraphNodePtr> PredSet;
    typedef std::map<CtrlGraphNodePtr, PredSet> PredMap;
    PredMap m_pred;
    typedef std::map<CtrlGraphNodePtr, int> SemiMap;
    SemiMap m_semi;
    std::vector<CtrlGraphNodePtr> m_vertex;
    typedef std::set<CtrlGraphNodePtr> BucketSet;
    typedef std::map<CtrlGraphNodePtr, BucketSet> BucketMap;
    BucketMap m_bucket;
    typedef std::map<CtrlGraphNodePtr, CtrlGraphNodePtr> DomMap;
    DomMap m_dom;

    typedef std::map<CtrlGraphNodePtr, CtrlGraphNodePtr> AncestorMap;
    AncestorMap m_ancestor;
    typedef std::map<CtrlGraphNodePtr, CtrlGraphNodePtr> LabelMap;
    LabelMap m_label;

    typedef std::set<CtrlGraphNodePtr> ChildrenSet;
    typedef std::map<CtrlGraphNodePtr, ChildrenSet> ChildrenMap;
    ChildrenMap m_children;
  
    int m_n;

    template<typename T> void postorderNode(T& f, CtrlGraphNodePtr root, int level = 0);
    template<typename T> void preorderNode(T& f, CtrlGraphNodePtr root, int level = 0);
  };

  template<typename T>
  void DomTree::postorder(T& f)
  {
    postorderNode(f, m_graph->entry());
  }

  template<typename T>
  void DomTree::postorderNode(T& f, CtrlGraphNodePtr root, int level)
  {
    ChildrenSet& children = m_children[root];
    for (ChildrenSet::iterator I = children.begin(); I != children.end(); ++I) {
      postorderNode(f, *I, level + 1);
    }
    f(root, level);
  }

  template<typename T>
  void DomTree::preorder(T& f)
  {
    preorderNode(f, m_graph->entry());
  }

  template<typename T>
  void DomTree::preorderNode(T& f, CtrlGraphNodePtr root, int level)
  {
    f(root, level);
    ChildrenSet& children = m_children[root];
    for (ChildrenSet::iterator I = children.begin(); I != children.end(); ++I) {
      preorderNode(f, *I, level + 1);
    }
  }

}

#endif
