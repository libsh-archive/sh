#ifndef PARTITION_HPP
#define PARTITION_HPP

#include <vector>

#include "ShDllExport.hpp"
#include "ShCtrlGraph.hpp"
#include "ShProgramNode.hpp"
#include "ShBasicBlock.hpp"
#include "DAG.hpp"

class SH_DLLEXPORT Partition {
  public:
	Partition(DAG::DAG *g);

  	typedef std::vector<DAGNode::DAGNode*> PassVector;
	typedef std::vector<SH::ShVariable*> VarVector;

	// sets list of passes after rds or full search marks the boundaries
	void set_partition(DAGNode::DAGNode *root); 

	void print_stmt();

	PassVector passes() { return m_passes; }
	VarVector shared_vars() { return m_shared_vars; }
  private:	
	PassVector m_passes;
	VarVector m_shared_vars;
	DAG::DAG *m_graph;

	void partition(DAGNode::DAGNode *v);
};

#endif
