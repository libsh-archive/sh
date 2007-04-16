// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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

#include "DomTree.hpp"
#include <iostream>
#include "Debug.hpp"
#include "Utility.hpp"

namespace {
using namespace SH;

/** Functor to compute dominance frontier */ 
struct DfComputer 
{
  DfComputer(const DomTree &domTree, DomTree::CfgNodeSetMap &df) 
    : domTree(domTree), df(df) {}

  void operator()(const CtrlGraphNode* &node, int level)
  {
    DomTree::CfgNodeSet &dfnode = df[node];
    dfnode.clear();

    DomTree::CfgNodeSet::const_iterator I;
    const DomTree::CfgNodeSet& succ = domTree.succ(node);

    // compute local component
    for(I = succ.begin(); I != succ.end(); ++I) {
      if(domTree.idom(*I) != node) {
        dfnode.insert(*I);
      }
    }

    // add on up component
    const DomTree::CfgNodeSet& idom_by = domTree.idom_by(node);
    for(I = idom_by.begin(); I != idom_by.end(); ++I) {
      DomTree::CfgNodeSet &dfI = df[*I]; 
      DomTree::CfgNodeSet::const_iterator J = dfI.begin(); 
      for(; J != dfI.end(); ++J) {
        if(domTree.idom(*J) != node) {
          dfnode.insert(*J);
        }
      }
    }
  }

  const DomTree &domTree;
  DomTree::CfgNodeSetMap &df;
};
}

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
      CtrlGraphNode* w = m_vertex[i];
      // Step 2
      CfgNodeSet& pred = m_pred[w];
      for (CfgNodeSet::iterator I = pred.begin(); I != pred.end(); ++I) {
	CtrlGraphNode* v = *I;
	CtrlGraphNode* u = eval(v);
	if (m_semi[u] < m_semi[v]) m_semi[w] = m_semi[u];
      }
      m_bucket[m_vertex[m_semi[w]]].insert(w);
      link(m_parent[w], w);
      
      // Step 3
      CfgNodeSet& bucket = m_bucket[m_parent[w]];
      while (!bucket.empty()) {
	CfgNodeSet::iterator I = bucket.begin();
	CtrlGraphNode* v = *I;
	bucket.erase(I);
	CtrlGraphNode* u = eval(v);
	m_dom[v] = (m_semi[u] < m_semi[v] ? u : m_parent[w]);
      }
    }
    
    // Step 4
    for (int i = 2; i <= m_n; i++) {
      CtrlGraphNode* w = m_vertex[i];
      if (m_dom[w] != m_vertex[m_semi[w]]) m_dom[w] = m_dom[m_dom[w]];
    }
    m_dom[m_graph->entry()] = 0;
  }
  
  struct DebugDumper {
    DebugDumper(const DomTree& tree)
      : tree(tree)
    {
    }
    void operator()(CtrlGraphNode* node, int level) {
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

  void DomTree::dfs(CtrlGraphNode* v)
  {
    if (!v) return;
    m_n++;
    m_semi[v] = m_n;
    m_vertex.push_back(v);
    m_ancestor[v] = 0;
    m_label[v] = v;
  
    for (std::vector<CtrlGraphBranch>::const_iterator I = v->successors.begin();
	 I != v->successors.end(); ++I) {
      CtrlGraphNode* w = I->node;
      if (m_semi[w] == 0) {
	m_parent[w] = v;
	dfs(w);
      }
      m_pred[w].insert(v);
      m_succ[v].insert(w);
    }
    CtrlGraphNode* w = v->follower;
    if (w) {
      if (m_semi[w] == 0) {
	m_parent[w] = v;
	dfs(w);
      }
      m_pred[w].insert(v);
      m_succ[v].insert(w);
    }
  }

  CtrlGraphNode* DomTree::eval(CtrlGraphNode* v)
  {
    if (!m_ancestor[v]) {
      return v;
    } else {
      compress(v);
      return m_label[v];
    }
  }

  void DomTree::compress(CtrlGraphNode* v)
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

  void DomTree::link(CtrlGraphNode* v, CtrlGraphNode* w)
  {
    m_ancestor[w] = v;
    m_children[v].insert(w);
  }

  const DomTree::CfgNodeSet& DomTree::succ(CtrlGraphNode* node) const
  {
    return m_succ(node); 
  }

  const DomTree::CfgNodeSet& DomTree::pred(CtrlGraphNode* node) const
  {
    return m_pred(node); 
  }

  CtrlGraphNode* DomTree::idom(CtrlGraphNode* node) const
  {
    return m_dom(node); 
  }

  const DomTree::CfgNodeSet& DomTree::idom_by(CtrlGraphNode* node) const
  {
    if(m_dom_by.empty()) {
      for(CfgNodeVec::const_iterator I = m_vertex.begin(); I != m_vertex.end(); ++I) {
        m_dom_by[m_dom[*I]].insert(*I);
      }
    }
    return m_dom_by[node];
  }

  const DomTree::CfgNodeSet& DomTree::df(CtrlGraphNode* node) const
  {
    if(m_df.empty()) {
      DfComputer dfcomp(*this, m_df);
      postorder(dfcomp);
    }
    return m_df[node];
  }

  DomTree::CfgNodeSet DomTree::df(const CfgNodeSet &nodes) const
  {
    CfgNodeSet::const_iterator N = nodes.begin();
    CfgNodeSet result;
    for(; N != nodes.end(); ++N) {
      const CfgNodeSet& dfN = df(*N);
      result.insert(dfN.begin(), dfN.end());
    }
    return result;
  }

  // @todo range runtime is not optimal 
  DomTree::CfgNodeSet DomTree::df_plus(const CfgNodeSet &nodes) const
  {
    CfgNodeSet::const_iterator N = nodes.begin();
    CfgNodeSet result = df(nodes);
    int size = result.size();
    for(;;) {
      result = df(result);
      int newsize = result.size();
      if(size == newsize) break; 
      size = newsize;
    } 
    return result;
  }
}
