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
//#include "sh.hpp"			 // necessary?
#include "ShDllExport.hpp"
#include "ShCtrlGraph.hpp"
#include "ShRefCount.hpp"

  /* A parital dominator tree in a flowgraph
   * -- dominator tree including multi-referenced nodes and their immediate dominators
   */
  class
  SH_DLLEXPORT PDomTree {
  public:
    PDomTree(SH::ShCtrlGraphPtr ctrlGraph);
	
	/* pchildren(w): set of child vertices of w in partial dom tree */
    typedef std::vector<SH::ShCtrlGraphNodePtr> PChildVector;
    typedef std::map<SH::ShCtrlGraphNodePtr, PChildVector> PChildMap;
    PChildMap pchildren;

    /// Postorder (bottom-up) traversal
    template<typename T> void postorder(T& f);

    /// Preorder traversal
    template<typename T> void preorder(T& f);
  
    void debugDump();

    int numbering(SH::ShCtrlGraphNodePtr node) const {
      	return (int)(std::find( m_vertex.begin(), m_vertex.end(), node) - m_vertex.begin());
    }
  
  	// returns true if node is multi-referenced; false otherwise
  	bool mr(SH::ShCtrlGraphNodePtr node) {
		return m_mr[node];
	}
	
	SH::ShCtrlGraphNodePtr get_root() {
		return m_graph->entry();
	}
  
  private:
    void dfs(SH::ShCtrlGraphNodePtr v);
    SH::ShCtrlGraphNodePtr eval(SH::ShCtrlGraphNodePtr v);
    void compress(SH::ShCtrlGraphNodePtr v);
    void link(SH::ShCtrlGraphNodePtr v, SH::ShCtrlGraphNodePtr w);
	  void build_pdt(SH::ShCtrlGraphNodePtr v);
  
  	/* input */
    SH::ShCtrlGraphPtr m_graph;

    // See "A Fast Algorithm for Finding Dominators in a Flowgraph", ACM TOPLAS, Langauer & Tarjan, Vol 1 No 1, 1979 for more information

    /* parent(w): vertex that is parent of w in spanning tree */ 
	  typedef std::map<SH::ShCtrlGraphNodePtr, SH::ShCtrlGraphNodePtr> ParentMap;
    ParentMap m_parent;
	
	/* pred(w): set of vertices, st (v, w) is edge of graph */
    typedef std::set<SH::ShCtrlGraphNodePtr> PredSet;
    typedef std::map<SH::ShCtrlGraphNodePtr, PredSet> PredMap;
    PredMap m_pred;
	
	/* semi(w): before numbered, = 0
     			after numbered = number of w
				after semidominator = number of semidominator of w */
    typedef std::map<SH::ShCtrlGraphNodePtr, int> SemiMap;
    SemiMap m_semi;
	
	/* vertex(i): vertex corresponding to number i */
	std::vector<SH::ShCtrlGraphNodePtr> m_vertex;
	
	/* bucket(w): set of vertices whose semidominator is w */
    typedef std::set<SH::ShCtrlGraphNodePtr> BucketSet;
    typedef std::map<SH::ShCtrlGraphNodePtr, BucketSet> BucketMap;
    BucketMap m_bucket;
	
	/* pchild(w): set of vertices v st (w, v) is edge of graph */
	BucketMap m_pchild;
		
	/* dom(w):	after step 3, some dominator of w
				after step 4, immediate dominator of w */
    typedef std::map<SH::ShCtrlGraphNodePtr, SH::ShCtrlGraphNodePtr> DomMap;
    DomMap dom;

    typedef std::map<SH::ShCtrlGraphNodePtr, SH::ShCtrlGraphNodePtr> AncestorMap;
    AncestorMap m_ancestor;
    typedef std::map<SH::ShCtrlGraphNodePtr, SH::ShCtrlGraphNodePtr> LabelMap;
    LabelMap m_label;

    typedef std::set<SH::ShCtrlGraphNodePtr> ChildrenSet;
    typedef std::map<SH::ShCtrlGraphNodePtr, ChildrenSet> ChildrenMap;
    ChildrenMap m_children;
  
 	/* flag for multi-referenced nodes */
	typedef std::map<SH::ShCtrlGraphNodePtr, bool> MRMap;
	MRMap m_mr; 
	
	/* flag for nodes in the partial dom tree */
	typedef std::map<SH::ShCtrlGraphNodePtr, bool> MRMap;
	MRMap m_pdt;
  
  	/* for numbering vertices in spanning tree */
    int m_n;

    template<typename T> void postorderNode(T& f, SH::ShCtrlGraphNodePtr root, int level = 0);
    template<typename T> void preorderNode(T& f, SH::ShCtrlGraphNodePtr root, int level = 0);
  };


//typedef SH::ShPointer<PDomTree> PDomTreePtr;
  typedef PDomTree* PDomTreePtr;
  
  template<typename T>
  void PDomTree::postorder(T& f)
  {
    postorderNode(f, m_graph->entry());
  }

  template<typename T>
  void PDomTree::postorderNode(T& f, SH::ShCtrlGraphNodePtr root, int level)
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
  void PDomTree::preorderNode(T& f, SH::ShCtrlGraphNodePtr root, int level)
  {
    f(root, level);
    ChildrenSet& children = m_children[root];
    for (ChildrenSet::iterator I = children.begin(); I != children.end(); ++I) {
      preorderNode(f, *I, level + 1);
    }
  }

#endif
