// Partial Dominator Tree v 1.0
//
// Last Revised: Nov. 28 by Cynthia
//
//////////////////////////////////////////////////////////////////////////////

#ifndef PDOMTREE_HPP
#define PDOMTREE_HPP

#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include "sh.hpp"			 // necessary?
#include "ShDllExport.hpp"
#include "ShCtrlGraph.hpp"
#include "ShRefCount.hpp"

namespace SH {

  /* A parital dominator tree in a flowgraph
   * -- dominator tree including multi-referenced nodes and their immediate dominators
   */
  class
  SH_DLLEXPORT PDomTree {
  public:
    PDomTree(ShCtrlGraphPtr ctrlGraph);
	
	/* pchildren(w): set of child vertices of w in partial dom tree */
    typedef std::vector<ShCtrlGraphNodePtr> PChildVector;
    typedef std::map<ShCtrlGraphNodePtr, PChildVector> PChildMap;
    PChildMap pchildren;

    /// Postorder (bottom-up) traversal
    template<typename T> void postorder(T& f);

    /// Preorder traversal
    template<typename T> void preorder(T& f);
  
    void debugDump();

    int numbering(ShCtrlGraphNodePtr node) const {
      	return (int)(std::find( m_vertex.begin(), m_vertex.end(), node) - m_vertex.begin());
    }
  
  	// returns true if node is multi-referenced; false otherwise
  	bool mr(ShCtrlGraphNodePtr node) {
		return m_mr[node];
	}
	
	ShCtrlGraphNodePtr get_root() {
		return m_graph->entry();
	}
  
  private:
    void dfs(ShCtrlGraphNodePtr v);
    ShCtrlGraphNodePtr eval(ShCtrlGraphNodePtr v);
    void compress(ShCtrlGraphNodePtr v);
    void link(ShCtrlGraphNodePtr v, ShCtrlGraphNodePtr w);
	void build_pdt(ShCtrlGraphNodePtr v);
  
  	/* input */
    ShCtrlGraphPtr m_graph;

    // See "A Fast Algorithm for Finding Dominators in a Flowgraph", ACM TOPLAS, Langauer & Tarjan, Vol 1 No 1, 1979 for more information

    /* parent(w): vertex that is parent of w in spanning tree */ 
	typedef std::map<ShCtrlGraphNodePtr, ShCtrlGraphNodePtr> ParentMap;
    ParentMap m_parent;
	
	/* pred(w): set of vertices, st (v, w) is edge of graph */
    typedef std::set<ShCtrlGraphNodePtr> PredSet;
    typedef std::map<ShCtrlGraphNodePtr, PredSet> PredMap;
    PredMap m_pred;
	
	/* semi(w): before numbered, = 0
     			after numbered = number of w
				after semidominator = number of semidominator of w */
    typedef std::map<ShCtrlGraphNodePtr, int> SemiMap;
    SemiMap m_semi;
	
	/* vertex(i): vertex corresponding to number i */
	std::vector<ShCtrlGraphNodePtr> m_vertex;
	
	/* bucket(w): set of vertices whose semidominator is w */
    typedef std::set<ShCtrlGraphNodePtr> BucketSet;
    typedef std::map<ShCtrlGraphNodePtr, BucketSet> BucketMap;
    BucketMap m_bucket;
	
	/* pchild(w): set of vertices v st (w, v) is edge of graph */
	BucketMap m_pchild;
		
	/* dom(w):	after step 3, some dominator of w
				after step 4, immediate dominator of w */
    typedef std::map<ShCtrlGraphNodePtr, ShCtrlGraphNodePtr> DomMap;
    DomMap m_dom;

    typedef std::map<ShCtrlGraphNodePtr, ShCtrlGraphNodePtr> AncestorMap;
    AncestorMap m_ancestor;
    typedef std::map<ShCtrlGraphNodePtr, ShCtrlGraphNodePtr> LabelMap;
    LabelMap m_label;

    typedef std::set<ShCtrlGraphNodePtr> ChildrenSet;
    typedef std::map<ShCtrlGraphNodePtr, ChildrenSet> ChildrenMap;
    ChildrenMap m_children;
  
 	/* flag for multi-referenced nodes */
	typedef std::map<ShCtrlGraphNodePtr, bool> MRMap;
	MRMap m_mr; 
	
	/* flag for nodes in the partial dom tree */
	typedef std::map<ShCtrlGraphNodePtr, bool> MRMap;
	MRMap m_pdt;
  
  	/* for numbering vertices in spanning tree */
    int m_n;

    template<typename T> void postorderNode(T& f, ShCtrlGraphNodePtr root, int level = 0);
    template<typename T> void preorderNode(T& f, ShCtrlGraphNodePtr root, int level = 0);
  };


  typedef ShPointer<PDomTree> PDomTreePtr;
  
  template<typename T>
  void PDomTree::postorder(T& f)
  {
    postorderNode(f, m_graph->entry());
  }

  template<typename T>
  void PDomTree::postorderNode(T& f, ShCtrlGraphNodePtr root, int level)
  {
    ChildrenSet& children = m_children[root];
    for (ChildrenSet::iterator I = children.begin(); I != children.end(); ++I) {
      postorderNode(f, *I, level + 1);
    }
    f(root, level);
  }

  template<typename T>
  void PDomTree::preorder(T& f)
  {
    preorderNode(f, m_graph->entry());
  }

  template<typename T>
  void PDomTree::preorderNode(T& f, ShCtrlGraphNodePtr root, int level)
  {
    f(root, level);
    ChildrenSet& children = m_children[root];
    for (ChildrenSet::iterator I = children.begin(); I != children.end(); ++I) {
      preorderNode(f, *I, level + 1);
    }
  }

}

#endif
