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
#include "sh.hpp"			 // necessary?
#include "ShDllExport.hpp"
#include "ShCtrlGraph.hpp"
#include "ShRefCount.hpp"
#include "Sh
#include "RDS.hpp"

#define RDS_UNFIXED 0
#define RDS_MARKED 1
#define RDS_UNMARKED 2

namespace SH {

  /* A parital dominator tree in a flowgraph
   * -- dominator tree including multi-referenced nodes and their immediate dominators
   */
  class
  SH_DLLEXPORT RDS {
  public:
  	// this may accept a ShCtrlGraphNodePtr instead, depending on how things are implemented later (like heuristics)
    RDS(ShProgramNodePtr program);	
	
    // Postorder (bottom-up) traversal
    template<typename T> void postorder(T& f);

    // Preorder traversal
    template<typename T> void preorder(T& f);
  
    void debugDump();
	  
  private:
    void rds_subdivide(ShCtrlGraphNodePtr v);
	void rds_merge(ShCtrlGraphNodePtr v);
	void add_mr(ShCtrlGraphNodePtr v);
	
	// heuristics -- stubs, for now
	// these will definitely have to be adjusted for testing (even before implemented)
	// and should probably end up in their own class
	bool h_valid(ShCtrlGraphNodePtr v) {
		return true;
	}
	
	bool h_recompute(ShCtrlGraphNodePtr v) {
		return true;
	}
	
	int h_merge() {
		return 0;
	}
	
	int h_cost(ShCtrlGraphNodePtr v) {
		return 0;
	}
	// end heuristics
	
  	// DAG
    ShCtrlGraphPtr m_graph;
	
	// P
	PDomTreePtr m_pdt;

#ifdef SH_RDSH
#else
	// fixes nodes as marked or unmarked (default: RDS_UNFIXED)
    typedef std::map<ShCtrlGraphNodePtr, int> FixedMap;
    FixedMap m_fixed;
	
	// list of multi-referenced nodes of DAG in postorder
	typedef std::vector<ShCtrlGraphNodePtr> MRList;
	MRList m_mrlist;
	
	// true if node already visited
    typedef std::map<ShCtrlGraphNodePtr, bool> VisitMap;
    VisitMap m_visit;
#endif
  };

  // from ShDomTree -- I've left them here, for now
  template<typename T>
  void RDS::postorder(T& f)
  {
    postorderNode(f, m_graph->entry());
  }

  template<typename T>
  void RDS::postorderNode(T& f, ShCtrlGraphNodePtr root, int level)
  {
    ChildrenSet& children = m_children[root];
    for (ChildrenSet::iterator I = children.begin(); I != children.end(); ++I) {
      postorderNode(f, *I, level + 1);
    }
    f(root, level);
  }

  template<typename T>
  void RDS::preorder(T& f)
  {
    preorderNode(f, m_graph->entry());
  }

  template<typename T>
  void RDS::preorderNode(T& f, ShCtrlGraphNodePtr root, int level)
  {
    f(root, level);
    ChildrenSet& children = m_children[root];
    for (ChildrenSet::iterator I = children.begin(); I != children.end(); ++I) {
      preorderNode(f, *I, level + 1);
    }
  }

}

#endif
