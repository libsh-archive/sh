#ifndef DAG_HPP
#define DAG_HPP

#include <map>
#include <vector>
#include <string>
#include <set>
//#include <algorithm>
//#include "sh.hpp"      // necessary?
#include "ShUtility.hpp"
#include "ShDllExport.hpp"
#include "ShRefCount.hpp"
#include "ShBasicBlock.hpp"
#include "ShStatement.hpp"

/***
 * Creates a directed acyclic graph for a basic block
 */

class SH_DLLEXPORT DAGNode {
  public:
	DAGNode() {}
	DAGNode(const std::string& label);
	DAGNode(const std::string& label, DAGNode *kid);
	DAGNode(const std::string& label, DAGNode* kid0, DAGNode* kid1);
	DAGNode(const std::string& label, DAGNode* kid0, DAGNode* kid1, DAGNode* kid2);

	std::string m_label; 

	typedef std::set<std::string> IdSet;
	IdSet id_list;

	typedef std::vector<DAGNode *> DAGNodeVector;
	DAGNodeVector predecessors;
	DAGNodeVector successors;

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
	typedef std::map<const std::string, DAGNode*> NodeMap;
	NodeMap node;

	typedef std::vector<DAGNode*> OpVector;
	typedef std::map<const std::string, OpVector> OpMap;
	OpMap ops[4];

	void DAG::add_statement(Stmt stmt, int src_size);
};

#endif
