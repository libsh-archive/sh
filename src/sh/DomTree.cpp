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

#include "DomTree.hpp"
#include <iostream>
#include "Debug.hpp"
#include "Utility.hpp"


namespace SH {

  DomTree::DomTree(CtrlGraphPtr graph)
    : m_graph(graph),
      m_n(0)
  {
    m_vertex.push_back(0); // Hack to have indexing start at 1.
    // Step 1
    dfs(m_graph->entry());
    
    // Steps 2 and 3
    for (int i = m_n; i >= 2; i--) {
      CtrlGraphNodePtr w = m_vertex[i];
      // Step 2
      PredSet& pred = m_pred[w];
      for (PredSet::iterator I = pred.begin(); I != pred.end(); ++I) {
	CtrlGraphNodePtr v = *I;
	CtrlGraphNodePtr u = eval(v);
	if (m_semi[u] < m_semi[v]) m_semi[w] = m_semi[u];
      }
      m_bucket[m_vertex[m_semi[w]]].insert(w);
      link(m_parent[w], w);
      
      // Step 3
      BucketSet& bucket = m_bucket[m_parent[w]];
      while (!bucket.empty()) {
	BucketSet::iterator I = bucket.begin();
	CtrlGraphNodePtr v = *I;
	bucket.erase(I);
	CtrlGraphNodePtr u = eval(v);
	m_dom[v] = (m_semi[u] < m_semi[v] ? u : m_parent[w]);
      }
    }
    
    // Step 4
    for (int i = 2; i <= m_n; i++) {
      CtrlGraphNodePtr w = m_vertex[i];
      if (m_dom[w] != m_vertex[m_semi[w]]) m_dom[w] = m_dom[m_dom[w]];
    }
    m_dom[m_graph->entry()] = 0;
  }
  
  struct DebugDumper {
    DebugDumper(const DomTree& tree)
      : tree(tree)
    {
    }
    void operator()(CtrlGraphNodePtr node, int level) {
      //    node->print(std::cerr, 0);
      printIndent(std::cerr, level);
      SH_DEBUG_PRINT("Node with numbering " << tree.numbering(node));
    }
    const DomTree& tree;
  };

  void DomTree::debugDump()
  {
#ifdef SH_DEBUG
    SH_DEBUG_PRINT("Debug Dump");
    DebugDumper d(*this);
    preorder(d);
    SH_DEBUG_PRINT("Debug Dump End");
#endif
  }

  void DomTree::dfs(CtrlGraphNodePtr v)
  {
    if (!v) return;
    m_n++;
    m_semi[v] = m_n;
    m_vertex.push_back(v);
    m_ancestor[v] = 0;
    m_label[v] = v;
  
    for (std::vector<CtrlGraphBranch>::const_iterator I = v->successors.begin();
	 I != v->successors.end(); ++I) {
      CtrlGraphNodePtr w = I->node;
      if (m_semi[w] == 0) {
	m_parent[w] = v;
	dfs(w);
      }
      m_pred[w].insert(v);
    }
    CtrlGraphNodePtr w = v->follower;
    if (w) {
      if (m_semi[w] == 0) {
	m_parent[w] = v;
	dfs(w);
      }
      m_pred[w].insert(v);
    }
  }

  CtrlGraphNodePtr DomTree::eval(CtrlGraphNodePtr v)
  {
    if (!m_ancestor[v]) {
      return v;
    } else {
      compress(v);
      return m_label[v];
    }
  }

  void DomTree::compress(CtrlGraphNodePtr v)
  {
    // This procedure assumes ancestor[v] != 0
    if (m_ancestor[m_ancestor[v]]) {
      compress(m_ancestor[v]);
      if (m_semi[m_label[m_ancestor[v]]] < m_semi[m_label[v]]) {
	m_label[v] = m_label[m_ancestor[v]];
      }
      m_ancestor[v] = m_ancestor[m_ancestor[v]];
    }
  }

  void DomTree::link(CtrlGraphNodePtr v, CtrlGraphNodePtr w)
  {
    m_ancestor[w] = v;
    m_children[v].insert(w);
  }

}
