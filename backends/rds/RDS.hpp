#ifndef RDS_HPP
#define RDS_HPP

#include <set>
#include <map>
#include <vector>
#include <algorithm>

#include "ShDllExport.hpp"
#include "ShCtrlGraph.hpp"
#include "ShProgramNode.hpp"
#include "ShBasicBlock.hpp"
#include "PDomTree.hpp"
#include "CallBack.hpp"
#include "Partition.hpp"

class SH_DLLEXPORT RDS {
public:  
	// this sets the control flow graph and runs pdomtree
	RDS(SH::ShProgramNodePtr progPtr);
	RDS() {};

	void rds() {
		m_rdsh = false;
		rds_search();
		m_part->set_partition(m_pdt->get_root());
	};

	void rdsh() {
		m_rdsh = true;
		rds_subdivide(m_pdt->get_root());
		m_part->set_partition(m_pdt->get_root());
	};

	void force_fake_limits();
	void force_graph_limits();
	void force_real_limits();

	PDomTree::PDomTree *pdt() { return m_pdt; }

	Partition::PassVector passes() { return m_part->passes(); }
	Partition::VarVector shared_vars() { return m_part->shared_vars(); }

	void print_partitions(char* filename);
	void print_partition();
	void print_partition_stmt() { m_part->print_stmt(); }
private:
	typedef std::set<DAGNode::DAGNode*> ChildrenSet;

	// true if rdsh; false if rds
	bool m_rdsh;

	DAG::DAG *m_graph;
	PDomTree::PDomTree *m_pdt;
	CallBack::CallBack *m_callback;	
	Partition *m_part;

	// for next_ksubset
	int  ksub_h, ksub_m2;
	bool ksub_mtc;

	// fixes nodes as marked or unmarked (default: RDS_UNFIXED)
    typedef std::map<DAGNode::DAGNode*, int> FixedMap;
    FixedMap m_fixed;
  
	// list of multi-referenced nodes of DAG in postorder
	typedef std::vector<DAGNode::DAGNode*> MRList;
	MRList m_mrlist;

	// from rds paper
	void rds_subdivide(DAGNode::DAGNode* v);
	void rds_merge(DAGNode::DAGNode* v);
	void rds_search();

	DAGNode::DAGNode *make_merge(DAGNode::DAGNode* v, int *a, int d, 
	DAGNode::DAGNodeVector kids, DAGNode::DAGNodeVector unmarked_kids);
	void add_mr(DAGNode::DAGNode* v);
};

#endif
