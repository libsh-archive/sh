// Partial Dominator Tree v 1.0
//
// Last Revised: Nov. 28 by Cynthia
//
//////////////////////////////////////////////////////////////////////////////

#include "PDomTree.hpp"
#include <iostream>
#include "ShDebug.hpp"
#include "ShUtility.hpp"

using namespace SH;


  PDomTree::PDomTree(ShCtrlGraphPtr graph)
          :   m_graph(graph), 
              m_n(0)
    {
      m_vertex.push_back(0); // Hack to have indexing start at 1.

        // Step 1 -- dfs to number vertices
        dfs(m_graph->entry());

        // Steps 2 and 3
        for (int i = m_n; i >= 2; i--) {
            ShCtrlGraphNodePtr w = m_vertex[i];
            // Step 2
            PredSet& pred = m_pred[w];
            for (PredSet::iterator I = pred.begin(); I != pred.end(); ++I) {
          ShCtrlGraphNodePtr v = *I;
          ShCtrlGraphNodePtr u = eval(v);
          if (m_semi[u] < m_semi[v]) m_semi[w] = m_semi[u];
            }
            m_bucket[m_vertex[m_semi[w]]].insert(w);
          link(m_parent[w], w);

            // Step 3
          BucketSet& bucket = m_bucket[m_parent[w]];
            while (!bucket.empty()) {
              BucketSet::iterator I = bucket.begin();
              ShCtrlGraphNodePtr v = *I;
              bucket.erase(I);
              ShCtrlGraphNodePtr u = eval(v);
              dom[v] = (m_semi[u] < m_semi[v] ? u : m_parent[w]);
            }
        }

        // Step 4
        for (int i = 2; i <= m_n; i++) {
          ShCtrlGraphNodePtr w = m_vertex[i];
          if (dom[w] != m_vertex[m_semi[w]]) dom[w] = dom[dom[w]];
        }
        dom[m_graph->entry()] = 0;
        
    /* End of Lengauer/Tarjan dominator algorithm */
        
        /* Generate the partial dom tree */
        build_pdt(m_graph->entry());
    }
  
    struct DebugDumper {
          DebugDumper(const PDomTree& tree)
            : tree(tree)
          {
          }
          
          void operator()(ShCtrlGraphNodePtr node, int level) {
              //    node->print(std::cerr, 0);
            shPrintIndent(std::cerr, level);
              SH_DEBUG_PRINT("Node with numbering " << tree.numbering(node));
          }
          const PDomTree& tree;
    };

    void PDomTree::debugDump()
    {
#ifdef SH_DEBUG
      SH_DEBUG_PRINT("Debug Dump");
      DebugDumper d(*this);
      preorder(d);
      SH_DEBUG_PRINT("Debug Dump End");
#endif
    }

  /* Depth-first search (also marks multi-referenced nodes) */
    void PDomTree::dfs(ShCtrlGraphNodePtr v)
    {
      if (!v) return;
  
      // node shouldn't be marked as multi-referenced initially
      m_mr[v] = false;
      m_pdt[v] = false;
  
      m_n++;
      m_semi[v] = m_n;
      m_vertex.push_back(v);
      m_ancestor[v] = 0;
      m_label[v] = v;
  
      for (std::vector<ShCtrlGraphBranch>::const_iterator I = v->successors.begin();
           I != v->successors.end(); ++I) 
      {
        ShCtrlGraphNodePtr w = I->node;
        if (m_semi[w] == 0) {
          m_parent[w] = v;
          dfs(w);
        }
        else {
          // this node has been visited already -- mark it as multi-referenced
          m_mr[w] = true;
          m_pdt[w] = true;
        }
        m_pred[w].insert(v);
      }
    
      ShCtrlGraphNodePtr w = v->follower;
      if (w) {
        if (m_semi[w] == 0) {
          m_parent[w] = v;
          dfs(w);
        }
        else {
          // this node has been visited already -- mark it as multi-referenced
          m_mr[w] = true;
          m_pdt[w] = true;
        }
        m_pred[w].insert(v);
      }
    }

    ShCtrlGraphNodePtr PDomTree::eval(ShCtrlGraphNodePtr v)
    {
      if (!m_ancestor[v]) {
          return v;
      } 
      else {
          compress(v);
          return m_label[v];
      }
    }

    void PDomTree::compress(ShCtrlGraphNodePtr v)
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

    void PDomTree::link(ShCtrlGraphNodePtr v, ShCtrlGraphNodePtr w)
    {
      m_ancestor[w] = v;
      m_children[v].insert(w);
    }
    
    /* record the children of each node in the partial dom tree */ 
    /* children listed in same order as postorder traversal of original graph */
    void PDomTree::build_pdt(ShCtrlGraphNodePtr v)
    {
      // visit v's children first
      for (std::vector<ShCtrlGraphBranch>::const_iterator I = v->successors.begin();
           I != v->successors.end(); ++I) 
      {
        ShCtrlGraphNodePtr w = I->node;
        build_pdt(w);
      }
        
      ShCtrlGraphNodePtr w = v->follower;
      if (w) {
        build_pdt(w);
      }
        
      // visit the node itself
      if (m_pdt[v]) {
        m_pdt[dom[v]] = true;
        pchildren[dom[v]].push_back(v);
      }
    }
