#ifndef DAG_HPP
#define DAG_HPP

#include <map>
#include <vector>
#include <string>
#include <set>

#include "ShUtility.hpp"
#include "ShDllExport.hpp"
#include "ShRefCount.hpp"
#include "ShBasicBlock.hpp"
#include "ShStatement.hpp"
#include "ShVariable.hpp"
#include "ShOperation.hpp"
#include "ShVariableNode.hpp"

#define DAG_OP 0
#define DAG_LEAF 1

/***
 * Creates a directed acyclic graph for a basic block
 */

class SH_DLLEXPORT DAGNode {
  public:
	DAGNode() {}	
	DAGNode(SH::ShVariable var);
	DAGNode(SH::ShOperation op);

	SH::ShVariable m_var;
	SH::ShOperation m_op;

	std::string m_label;
	
	int m_type;

	typedef std::set<SH::ShVariableNodePtr> IdSet;
	IdSet id_list;

	typedef std::map<DAGNode *, bool> CutMap;
	CutMap m_cut;

	typedef std::vector<DAGNode *> DAGNodeVector;
	DAGNodeVector predecessors;
	DAGNodeVector successors;

	void add_kid(DAGNode *kid);
	void print(int indent);
  private:
	
};

class SH_DLLEXPORT DAG {
  public:
	DAG(SH::ShBasicBlockPtr block);  

	// change any types here
	typedef SH::ShBasicBlock::ShStmtList StmtList;
	typedef SH::ShStatement Stmt;

	StmtList m_statements;  

	DAGNode* m_root;

	void print(int indent);
  private:
	typedef std::map<SH::ShVariableNodePtr, DAGNode*> NodeMap;
	NodeMap node;

	typedef std::vector<DAGNode*> OpVector;
	typedef std::map<SH::ShOperation, OpVector> OpMap;
	OpMap ops[4];

	void DAG::add_statement(SH::ShStatement stmt);
};

#endif
