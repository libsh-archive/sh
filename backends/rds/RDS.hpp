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

class SH_DLLEXPORT RDS {
  public:
  
	// this sets the control flow graph and runs pdomtree
	// may accept a SH::ShCtrlGraphNodePtr instead, depending on how callbacks are implemented
	RDS(SH::ShProgramNodePtr progPtr);

	// empty constructor, for testing purposes
	RDS() {};

	void debugDump();

	// perform rds and return resulting set of programs
	void get_partitions();
  
	// return partial dom tree for this program
	PDomTree::PDomTree *get_pdt() {
		return m_pdt;
	}

  private:
	// limits
	int max_ops;

	typedef std::set<DAGNode::DAGNode*> ChildrenSet;
    typedef std::vector<DAGNode::DAGNode*> PassVector;
    
	DAG::DAG *m_graph;
	PDomTree::PDomTree *m_pdt;
    
	// for next_ksubset
	int  ksub_h, ksub_m2;
	bool ksub_mtc;

	// fixes nodes as marked or unmarked (default: RDS_UNFIXED)
    typedef std::map<DAGNode::DAGNode*, int> FixedMap;
    FixedMap m_fixed;
  
	// list of multi-referenced nodes of DAG in postorder
	typedef std::vector<DAGNode::DAGNode*> MRList;
	MRList m_mrlist;
  
	// true if node already visited
	typedef std::map<DAGNode::DAGNode*, bool> VisitMap;
    VisitMap m_visit;
  
	// for setting hardware limits
	void set_limits();

  	// from rds paper
	void rds_subdivide(DAGNode::DAGNode* v);
	void rds_merge(DAGNode::DAGNode* v);

	// callbacks
	bool valid(DAGNode::DAGNode* v);
	bool recompute(DAGNode::DAGNode* v);
	int merge(PassVector passes);
	int cost(DAGNode::DAGNode* v);

	void add_mr(DAGNode::DAGNode* v);
	int *next_ksubset(int n, int k, int *a);
  };
  #endif