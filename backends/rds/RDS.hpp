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
#include "Limits.hpp"

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

	void rds() {
		m_rdsh = false;
		rds_search();
		set_partition();
	};

	void rdsh() {
		m_rdsh = true;
		rds_subdivide(m_pdt->get_root());
		set_partition();
	};

	// brute force
	void full_search();
  
	// return partial dom tree for this program
	PDomTree::PDomTree *get_pdt() {
		return m_pdt;
	}

	void print_partitions(char* filename);
	void print_partition();
	void print_partition_stmt();
private:
  typedef std::set<DAGNode::DAGNode*> ChildrenSet;
  typedef std::vector<DAGNode::DAGNode*> PassVector;	
  typedef std::vector<DAGNode::DAGNode*> NodeVector;

	// limits
  rds::Limits *m_limits;
  int m_ops_used;

	// true if rdsh; false if rds
	bool m_rdsh;

	DAG::DAG *m_graph;
	PDomTree::PDomTree *m_pdt;

	PassVector m_passes;
    
	// for next_ksubset
	int  ksub_h, ksub_m2;
	bool ksub_mtc;

	// fixes nodes as marked or unmarked (default: RDS_UNFIXED)
    typedef std::map<DAGNode::DAGNode*, int> FixedMap;
    FixedMap m_fixed;
  
	// list of multi-referenced nodes of DAG in postorder
	typedef std::vector<DAGNode::DAGNode*> MRList;
	MRList m_mrlist;
    
	// get list of passes after rds or rdsh
	void set_partition();

	// for creating list of passes
	void partition(DAGNode::DAGNode *v);

	// from rds paper
	void rds_subdivide(DAGNode::DAGNode* v);
	void rds_merge(DAGNode::DAGNode* v);
	void rds_search();

	// callbacks
	bool valid(DAGNode::DAGNode* v);
	bool recompute(DAGNode::DAGNode* v);
	DAGNode::DAGNode *merge(PassVector passes);
	int cost(DAGNode::DAGNode* v);

	DAGNode::DAGNode *make_merge(DAGNode::DAGNode* v, int *a, int d, 
			DAGNode::DAGNodeVector kids, DAGNode::DAGNodeVector unmarked_kids);
	void add_mr(DAGNode::DAGNode* v);
	int *next_ksubset(int n, int k, int *a);
	void unfixall(DAGNode::DAGNode *v);
	void unmarkall(DAGNode::DAGNode *v);
	
	int countmarked(DAGNode::DAGNode *v);
	int countnodes(DAGNode::DAGNode *v);
	void unvisitall(DAGNode::DAGNode *v);

	// for brute force
	void set_nodelist(DAGNode::DAGNode *v);
	NodeVector m_nodelist;

};

#endif
