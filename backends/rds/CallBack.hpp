#ifndef CALLBACK_HPP
#define CALLBACK_HPP

#include <vector>
#include <string>

#include "ShDllExport.hpp"
#include "DAG.hpp"
#include "Limits.hpp"

/***
 * Hardware limit checking functions for RDS
 * VALID, RECOMPUTE, MERGE, and COST
 */
class SH_DLLEXPORT CallBack {
  public:
	CallBack();

	typedef std::vector<DAGNode::DAGNode*> PassVector;	

	// checking of subregions
  	bool valid(DAGNode::DAGNode* v);
	bool recompute(DAGNode::DAGNode* v);
	DAGNode::DAGNode *merge(PassVector passes);

	// checking of entire partitions
	float cost(DAGNode::DAGNode* v);
	bool valid_partition(DAGNode::DAGNode *v);

	void increase_used(DAGNode::DAGNode *v);
	void decrease_used(DAGNode::DAGNode *v);
	void set_fake_limits();
	void reset();
  private:
	DAG::DAG *m_graph;
	rds::Limits *m_limits;

	// cost parameters
	float c_pass;
	float c_tex;
	float c_instr;

	// totals for whole program
  	int m_num_instrs;
	int m_num_texs;
	int m_num_passes;

	// used up resources
	int m_ops_used;
	int m_halftemps_used;
	int m_temps_used;
	int m_params_used;
	int m_attribs_used;
	int m_texs_used;

	// helper functions
	void init_used();
	void set_cost_vars(DAGNode::DAGNode *v);
	bool all_passes_valid(DAGNode::DAGNode* v);
};

#endif
