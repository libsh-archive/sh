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

#define RDS_UNFIXED 0
#define RDS_MARKED 1
#define RDS_UNMARKED 2

/***
 * Creates a directed acyclic graph for a basic block
 */

class SH_DLLEXPORT DAGNode {
  public:
	DAGNode() { m_var = NULL; m_stmt = NULL; }
	DAGNode(SH::ShVariable *var);
	DAGNode(SH::ShStatement *stmt);

	SH::ShVariable *m_var;
	SH::ShStatement *m_stmt;

	std::string m_label;

	bool m_visited;
	bool m_marked;
	
	typedef std::map<DAGNode *, bool> CutMap;
	CutMap m_cut;

	typedef std::set<SH::ShVariable*> IdSet;
	IdSet id_list;

	typedef std::vector<DAGNode*> DAGNodeVector;
	DAGNodeVector predecessors;
	DAGNodeVector successors;

	// returns number of instructions/resources consumed pass starting at this node
	int instrs() const { return m_instrs; }
	int params() const { return m_params; }
	int attribs() const { return m_attribs; }
	int temps() const { return m_temps; }
	int halftemps() const { return m_halftemps; }
	int texs() const { return m_texs; }
	int consts() const { return m_consts; }
	int outputs() const { return m_outputs; }

	void mark() { m_marked = true; }
	void unmark() { m_marked = false; }
	bool marked() { return m_marked; }

	void fix(int fix_type) { m_fixed = fix_type; }
	int fixed() { return m_fixed; }

	void add_kid(DAGNode *kid);
	void print(int indent);
	void dump_stmts();
	void unvisitall();
  	void cuts();
	void set_resources();
	void set_resources_stmt();
	void print_resources();
	SH::ShBasicBlock::ShStmtList get_statements(); 
	int count_instrs();
	void init_resources();
  private:
  	void set_var(const SH::ShVariableNodePtr& var); 
	void print_stmts();

	SH::ShBasicBlock::ShStmtList dag_to_stmt(SH::ShBasicBlock::ShStmtList stmts, bool cut);
	
	int m_instrs;		// number of instructions
	int m_params;		// number of uniforms (including palettes)
	int m_attribs;		// number of inputs
	int m_temps;		// number of temporaries
	int m_halftemps;	// number of half-float temporaries
	int m_texs;			// number of textures
	int m_consts;		// number of constants
	int m_outputs;		// number of outputs
	int m_channels;		// number of channels

	//bool m_marked;

	int m_fixed;
};

class SH_DLLEXPORT DAG {
  public:
	DAG(SH::ShBasicBlockPtr block);  

	// change any types here
	 //void genNode(SH::ShCtrlGraphNodePtr node);
	typedef SH::ShBasicBlock::ShStmtList StmtList;
	typedef SH::ShStatement Stmt;

	StmtList m_statements;  

	DAGNode* m_root;

	void print(int indent);
	SH::ShVariable *find_var(SH::ShVariable *var);
  private:
	typedef std::vector<SH::ShVariable*> VarVector;
	typedef std::map<SH::ShVariableNodePtr, VarVector> VarVectorMap;
	typedef std::map<SH::ShVariable*, DAGNode*> NodeMap;
	VarVectorMap m_nodevector;
	NodeMap node;

	void DAG::add_statement(SH::ShStatement *stmt);
};

#endif
