#ifndef FULLSEARCH_HPP
#define FULLSEARCH_HPP

#include <map>
#include <vector>
#include <string>
#include <set>

#include "ShDllExport.hpp"
#include "ShCtrlGraph.hpp"
#include "ShProgramNode.hpp"
#include "ShBasicBlock.hpp"
#include "DAG.hpp"
#include "CallBack.hpp"
#include "Partition.hpp"

/***
 * A brute force approach to partitioning (for comparison purposes only)
 */
class SH_DLLEXPORT FullSearch {
  public:
	FullSearch(SH::ShProgramNodePtr progPtr);

  	Partition::PassVector passes() { return m_part->passes(); }
	Partition::VarVector shared_vars() { return m_part->shared_vars(); }

	void print_partition_stmt() { m_part->print_stmt(); }

	void ksub_init(int n, int k);
	int *ksub_next();
	bool ksub_more() { return ksub_mtc; }
  private:
	typedef std::vector<DAGNode::DAGNode*> NodeVector;

	DAG::DAG *m_graph;
	Partition *m_part;
	CallBack::CallBack *m_callback;
	NodeVector m_nodelist;	

	int m_n;	// number of elements 
	int m_k;	// size of subset
	int *m_a;	// array of elements
	int  ksub_h, ksub_m2;
	bool ksub_mtc;

	void full_search();
	void set_nodelist(DAGNode::DAGNode *v);
};

#endif
