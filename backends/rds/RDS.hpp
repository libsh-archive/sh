// RDS v 1.0
//
// Last Revised: Nov. 28 by Cynthia
//
//////////////////////////////////////////////////////////////////////////////

#ifndef RDS_HPP
#define RDS_HPP

#include <set>
#include <map>
#include <vector>
#include <algorithm>
//#include "sh.hpp"      // necessary?
#include "ShDllExport.hpp"
#include "ShCtrlGraph.hpp"
#include "ShProgramNode.hpp"
#include "PDomTree.hpp"

#define RDS_UNFIXED 0
#define RDS_MARKED 1
#define RDS_UNMARKED 2

  /* A parital dominator tree in a flowgraph
   * -- dominator tree including multi-referenced nodes and their immediate dominators
   */
  class
  SH_DLLEXPORT RDS {
  public:
  // this may accept a SH::ShCtrlGraphNodePtr instead, depending on how things are implemented later (like heuristics)
  RDS(SH::ShProgramNodePtr progPtr);

  // Postorder (bottom-up) traversal
  template<typename T> void postorder(T& f);

  // Preorder traversal
  template<typename T> void preorder(T& f);

  void debugDump();
  PDomTreePtr getPDT() {
    return m_pdt;
  }

  private:
  void rds_subdivide(SH::ShCtrlGraphNodePtr v);
  void rds_merge(SH::ShCtrlGraphNodePtr v);
  void add_mr(SH::ShCtrlGraphNodePtr v);

  typedef std::set<SH::ShCtrlGraphNodePtr> ChildrenSet;

  template<typename T>
  void RDS::postorderNode(T& f, SH::ShCtrlGraphNodePtr root, int level);

  template<typename T>
  void RDS::preorderNode(T& f, SH::ShCtrlGraphNodePtr root, int level);
  
  // heuristics -- stubs, for now
  // these will definitely have to be adjusted for testing (even before implemented)
  // and should probably end up in their own class
  bool h_valid(SH::ShCtrlGraphNodePtr v) {
    return true;
  }
  
  bool h_recompute(SH::ShCtrlGraphNodePtr v) {
    return true;
  }
  
  int h_merge() {
    return 0;
  }
  
  int h_cost(SH::ShCtrlGraphNodePtr v) {
    return 0;
  }
  // end heuristics
  
  // DAG
  SH::ShCtrlGraphPtr m_graph;

  // PDT
  PDomTreePtr m_pdt;
    
  

#ifdef SH_RDSH
#else
  // fixes nodes as marked or unmarked (default: RDS_UNFIXED)
    typedef std::map<SH::ShCtrlGraphNodePtr, int> FixedMap;
    FixedMap m_fixed;
  
  // list of multi-referenced nodes of DAG in postorder
  typedef std::vector<SH::ShCtrlGraphNodePtr> MRList;
  MRList m_mrlist;
  
  // true if node already visited
    typedef std::map<SH::ShCtrlGraphNodePtr, bool> VisitMap;
    VisitMap m_visit;
#endif
  };

  // from SH::ShDomTree -- I've left them here, for now
  template<typename T>
  void RDS::postorder(T& f)
  {
    postorderNode(f, m_graph->entry(),0);
  }

  template<typename T>
  void RDS::postorderNode(T& f, SH::ShCtrlGraphNodePtr root, int level)
  {
    PDomTree::PChildVector children = m_pdt->pchildren[root];
    for (PDomTree::PChildVector::iterator I = children.begin(); I != children.end(); ++I) {
      postorderNode(f, *I, level + 1);
    }
    f(root, level);
  }

  template<typename T>
  void RDS::preorder(T& f)
  {
    preorderNode(f, m_graph->entry(),0);
  }

  template<typename T>
  void RDS::preorderNode(T& f, SH::ShCtrlGraphNodePtr root, int level)
  {
    f(root, level);
    PDomTree::PChildVector children = m_pdt->pchildren[root];
    for (PDomTree::PChildVector::iterator I = children.begin(); I != children.end(); ++I) {
      preorderNode(f, *I, level + 1);
    }
  }

#endif
