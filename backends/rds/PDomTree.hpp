// Partial Dominator Tree v 1.0
//
// Last Revised: Nov. 28 by Cynthia
//
//////////////////////////////////////////////////////////////////////////////

#ifndef PDOMTREE_HPP
#define PDOMTREE_HPP

#include <set>
#include <map>
#include <string>
#include <vector>
#include <algorithm>

#include "ShDllExport.hpp"
#include "ShRefCount.hpp"
#include "DAG.hpp"

/***
 * A parital dominator tree in a flowgraph
 * (dominator tree including multi-referenced nodes and their immediate dominators)
 */
// See "A Fast Algorithm for Finding Dominators in a Flowgraph", 
// ACM TOPLAS, Langauer & Tarjan, Vol 1 No 1, 1979 for more information

class SH_DLLEXPORT PDomTree {
  public:
    
	PDomTree(DAG::DAG* dag);
	
	/* pchildren(w): set of child vertices of w in partial dom tree */
    typedef std::vector<DAGNode::DAGNode*> PChildVector;
    typedef std::map<DAGNode::DAGNode*, PChildVector> PChildMap;
    PChildMap pchildren;

  typedef std::map<std::string,bool,std::less<std::string>,
                   std::allocator<std::pair<const std::string, bool> > > NodeList;
  typedef std::map<std::string,NodeList,std::less<std::string>,
                   std::allocator<std::pair<const std::string,NodeList> > > NodeGraph;
  
	void printDoms();

	void printGraph(DAGNode::DAGNode *node, int indent);

  std::ostream& PDomTree::graphvizDump(DAGNode::DAGNode *node, std::ostream& out);
  void PDomTree::graphvizDump(char* filename);

    int numbering(DAGNode::DAGNode* node) const {
      	return (int)(std::find( m_vertex.begin(), m_vertex.end(), node) - m_vertex.begin());
    }

	DAGNode::DAGNode* dominator(DAGNode::DAGNode* node) {
		return dom[node];
	}
  
  	// returns true if node is multi-referenced; false otherwise
  	bool mr(DAGNode::DAGNode* node) {
		return m_mr[node];
	}
	
	DAGNode::DAGNode* get_root() {
		return m_graph->m_root;
	}
  
private:
  void dfs(DAGNode::DAGNode* v);
  DAGNode::DAGNode* eval(DAGNode::DAGNode* v);
  void compress(DAGNode::DAGNode* v);
  void link(DAGNode::DAGNode* v, DAGNode::DAGNode* w);
  void build_pdt(DAGNode::DAGNode* v);

  NodeGraph *m_connect;
  
  	/* input */
    DAG::DAG *m_graph;

    /* parent(w): vertex that is parent of w in spanning tree */ 
	  typedef std::map<DAGNode::DAGNode*, DAGNode::DAGNode*> ParentMap;
    ParentMap m_parent;
	
	/* pred(w): set of vertices, st (v, w) is edge of graph */
    typedef std::set<DAGNode::DAGNode*> PredSet;
    typedef std::map<DAGNode::DAGNode*, PredSet> PredMap;
    PredMap m_pred;
	
	/* semi(w): before numbered, = 0
     			after numbered = number of w
				after semidominator = number of semidominator of w */
    typedef std::map<DAGNode::DAGNode*, int> SemiMap;
    SemiMap m_semi;
	
	/* vertex(i): vertex corresponding to number i */
	std::vector<DAGNode::DAGNode*> m_vertex;
	
	/* bucket(w): set of vertices whose semidominator is w */
    typedef std::set<DAGNode::DAGNode*> BucketSet;
    typedef std::map<DAGNode::DAGNode*, BucketSet> BucketMap;
    BucketMap m_bucket;
	
	/* pchild(w): set of vertices v st (w, v) is edge of graph */
	BucketMap m_pchild;
		
	/* dom(w):	after step 3, some dominator of w
				after step 4, immediate dominator of w */
    typedef std::map<DAGNode::DAGNode*, DAGNode::DAGNode*> DomMap;
    DomMap dom;

    typedef std::map<DAGNode::DAGNode*, DAGNode::DAGNode*> AncestorMap;
    AncestorMap m_ancestor;
    typedef std::map<DAGNode::DAGNode*, DAGNode::DAGNode*> LabelMap;
    LabelMap m_label;

    typedef std::set<DAGNode::DAGNode*> ChildrenSet;
    typedef std::map<DAGNode::DAGNode*, ChildrenSet> ChildrenMap;
    ChildrenMap m_children;
  
 	/* flag for multi-referenced nodes */
	typedef std::map<DAGNode::DAGNode*, bool> BoolMap;
	BoolMap m_mr; 
	
	/* flag for nodes in the partial dom tree */
	BoolMap m_pdt;
	BoolMap m_visited;
  
  	/* for numbering vertices in spanning tree */
    int m_n;
  };

#endif
