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

/***
 * Creates a directed acyclic graph for a basic block
 */

class SH_DLLEXPORT DAGNode {
  public:
	DAGNode() {}	
	DAGNode(SH::ShVariable var);
	DAGNode(SH::ShStatement *stmt);

	SH::ShVariable m_var;
	SH::ShStatement *m_stmt;

	std::string m_label;

	bool m_visited;
	bool m_marked;

	typedef std::map<DAGNode *, bool> CutMap;
	CutMap m_cut;

	typedef std::set<SH::ShVariableNodePtr> IdSet;
	IdSet id_list;

	typedef std::vector<DAGNode *> DAGNodeVector;
	DAGNodeVector predecessors;
	DAGNodeVector successors;

	void add_kid(DAGNode *kid);
	void print(int indent);
	void dump_stmts();
	void unvisitall();
	SH::ShBasicBlock::ShStmtList get_statements(); 
  private:
	void print_stmts();
	SH::ShBasicBlock::ShStmtList dag_to_stmt(SH::ShBasicBlock::ShStmtList stmts);
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

	void DAG::add_statement(SH::ShStatement *stmt);
};

#endif
