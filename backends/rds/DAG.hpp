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

	// returns number of instructions/resources consumed by pass starting at this node
	int instrs() const { return m_num_instrs; }
	int params() const { return m_params.size(); }
	int attribs() const { return m_attribs.size(); }
	int temps() const { return m_temps.size(); }
	int halftemps() const { return m_halftemps.size(); }
	int texs() const { return m_texs.size(); }
	int consts() const { return m_consts.size(); }
	int outputs() const { return m_outputs.size(); }

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
	int num_passes();

	SH::ShBasicBlock::ShStmtList get_statements(); 
	void prune_vars();

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
  private:
	typedef std::set<SH::ShVariableNodePtr> VarList;
	void init_resources();
  	void set_var(const SH::ShVariableNodePtr& var); 
	int countmarked(); 
	SH::ShBasicBlock::ShStmtList dag_to_stmt(SH::ShBasicBlock::ShStmtList stmts);
	void add_var_to_list(VarList *vlist, const SH::ShVariableNodePtr& var);

	
	int m_num_instrs;	// number of instructions
	VarList m_params;		// list of uniforms (including palettes)
	VarList m_attribs;		// list of inputs
	VarList m_temps;		// list of temporaries
	VarList m_halftemps;	// list of half-float temporaries
	VarList m_texs;			// list of textures
	VarList m_consts;		// list of constants
	VarList m_outputs;		// list of outputs
	VarList m_channels;		// list of channels

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
