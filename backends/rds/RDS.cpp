// RDS v 1.0
//
//  Last Revised: Nov. 28 by Cynthia
//
//  A somewhat skeletal version of the RDS pseudocode
//////////////////////////////////////////////////////////////////////////////

#include "RDS.hpp"
#include <iostream>
#include "ShDebug.hpp"
#include "ShUtility.hpp"

using namespace SH;

  RDS::RDS(ShProgramNodePtr progPtr)
      :   m_graph(progPtr->ctrlGraph)
  {
    m_pdt = new PDomTree(m_graph);

#ifdef SH_RDSH
    subdivide(m_pdt->get_root());
#else
    // rds search algorithm
    ShCtrlGraphNodePtr d = m_graph->entry();
      
    // this sets m_mrlist
    add_mr(d);
      
    for (MRList::iterator I = m_mrlist.begin(); I != m_mrlist.end(); ++I) {
      ShCtrlGraphNodePtr m = *I;
      
      // find cost of saving subregion m      
      d->clearMarked();
      m_fixed[m] = RDS_MARKED;
      rds_subdivide(m_pdt->get_root());
      int cost_s = h_cost(d); // this line will likely be changed
      
      // find cost of recomputing subregion m
      d->clearMarked();
      m_fixed[m] = RDS_UNMARKED;
      rds_subdivide(m_pdt->get_root());
      int cost_r = h_cost(d); // this will also likely be changed 
      
      if (cost_s < cost_r) {
        m_fixed[m] = RDS_MARKED;
      }
    }     
      
#endif
  }
  
  //removed const because of complaints
  struct DebugDumper {
    DebugDumper(RDS& tree)
               : tree(*(tree.getPDT()))
    {
    }
            
    void operator()(ShCtrlGraphNodePtr node, int level) {
      //    node->print(std::cerr, 0);
      shPrintIndent(std::cerr, level);
      SH_DEBUG_PRINT("Node with numbering " << tree.numbering(node));
    }

    PDomTree& tree;
  };

  void RDS::debugDump()
    {
#ifdef SH_DEBUG
      SH_DEBUG_PRINT("Debug Dump");
      DebugDumper d( *this);
      preorder(d);
      SH_DEBUG_PRINT("Debug Dump End");
#endif
    }

  // partitions m_graph by marking nodes to indicate pass boundaries 
    void RDS::rds_subdivide(ShCtrlGraphNodePtr v)
    {
      if (!v) return;
    
    // stop if subregion v can be mapped in one pass
      if (h_valid(v)) return;
        
        // list of pdt children of v (postorder)
        // changed to vector, PChildSet was not created, and only ::iterator is used
        PDomTree::PChildVector& kids = m_pdt->pchildren[v];
        
        for (PDomTree::PChildVector::iterator I = kids.begin(); I != kids.end(); ++I) {
      ShCtrlGraphNodePtr w = *I;
            rds_subdivide(w);
            
      // NOTE: not sure if we should use their mark() and clearMarked() in ShCtrlGraph, or define our own
        
      if (m_pdt->mr(w)) { 
        #ifdef SH_RDSH
          if (h_recompute(w)) {
            // recompute w
            w->clearMarked();
          }
          else {
            // save w
            w->mark();
          }
        #else
          switch(m_fixed[w]) {
            case RDS_MARKED:
              w->mark();  
              break;
            case RDS_UNMARKED:
              w->clearMarked();
              break;
            default:
              if (h_recompute(w)) {
                // recompute w
                w->clearMarked();
              }
              else {
                // save w
                w->mark();
              }
              break;
          }
        #endif
        }
    }
    
    rds_merge(v);
    }

    void RDS::rds_merge(ShCtrlGraphNodePtr v)
    {
    // get the number of kids of v
    int k = v->successors.size();
    
        if (v->follower) {
          k++;
        }
    
    for (int d = k; d >= 0; d--) {
      // Note: Cynthia can write this subsets-of-size-d code later  
    }
    }
  
  // creates a post-order list of MR nodes, storing in m_mrlist
  void RDS::add_mr(ShCtrlGraphNodePtr v) {
    if (!v) return;
    
#ifdef SH_RDSH
    // do nothing
#else
    if (m_visit[v]) return; // already visited
    m_visit[v] = true;
    
    // visit v's children first
    for (std::vector<ShCtrlGraphBranch>::const_iterator I = v->successors.begin();
          I != v->successors.end(); ++I) 
    {
      ShCtrlGraphNodePtr w = I->node;
      add_mr(w);
    }
        
    ShCtrlGraphNodePtr w = v->follower;

    // I'm not sure what you meant to do here
//  if (w) {
//    ShCtrlGraphNodePtr w = I->node;
//    add_mr(w);
//  }
    
    // add v to list if it's multi-referenced
    if (m_pdt->mr(v)) {
      m_mrlist.push_back(v);
    }
#endif
  }
