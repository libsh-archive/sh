#include <iostream>
#include "DAG.hpp"
#include "ShPaletteNode.hpp"

using namespace SH;
using namespace std;

#define RDS_DEBUG

// creates a leaf node
DAGNode::DAGNode(ShVariable *var) 
	:	m_var(var),
		m_label(var->node()->name())
{
  	m_stmt = NULL;
	m_visited = false;
	init_resources();
}

// creates an operation node
DAGNode::DAGNode(ShStatement *stmt) 
	:	m_stmt(stmt),
		m_label(static_cast<std::string>(opInfo[stmt->op].name))	
{
  	m_var = NULL;
	m_visited = false;
	init_resources();
}

// adds a node to successors of this one
void DAGNode::add_kid(DAGNode *kid)
{
	successors.push_back(kid);
	m_cut[kid] = false;
	kid->predecessors.push_back(this);
}

// sets m_visited for this node and all children to false
void DAGNode::unvisitall() 
{
	m_visited = false;
	for (DAGNodeVector::iterator I = successors.begin(); I != successors.end(); ++I) {
		(*I)->unvisitall();
	}
}

// sets m_marked for this node and all children to false
void DAGNode::unmarkall() 
{
	m_marked = false;
	for (DAGNodeVector::iterator I = successors.begin(); I != successors.end(); ++I) {
		(*I)->unmarkall();
	}
}

// sets m_fixed for this node and all children to RDS_UNFIXED
void DAGNode::unfixall() 
{
	m_fixed = RDS_UNFIXED;
	for (DAGNodeVector::iterator I = successors.begin(); I != successors.end(); ++I) {
		(*I)->unfixall();
	}
}

int DAGNode::countmarked() {
	if (m_visited) return 0;
	m_visited = true;
	
	int count = 0;

	if (m_marked) count++;
	
	for (DAGNode::DAGNodeVector::iterator I = successors.begin(); I != successors.end(); ++I) {
		count += (*I)->countmarked();
	} 
	return count;
}

int DAGNode::num_passes() {
	unvisitall();
	int count = 0;
	if (!m_marked) count++;
	return countmarked() + count;
}

// reset all resource consumption fields
void DAGNode::init_resources(){
	m_num_instrs = 0;		
	m_params.clear();		
	m_attribs.clear();	
	m_temps.clear();		
	m_halftemps.clear();
	m_texs.clear();			
	m_consts.clear();		
	m_outputs.clear();
	m_channels.clear();
	m_extra_instrs = 0;
	m_extra_temps = 0;
	m_extra_consts = 0;
}

// converts dag nodes to statements, adding to statement list
// cut = true if final partition (m_cut set); cut = false if intermediate partition (cuts at marked nodes)
ShBasicBlock::ShStmtList DAGNode::dag_to_stmt(ShBasicBlock::ShStmtList stmts)
{
	if (m_visited) 
		return stmts;
	
	m_visited = true;

	// add statements for successors that are not cut off or marked
	for (DAGNodeVector::iterator I = successors.begin(); I != successors.end(); ++I) {
		DAGNode *v = *I;

		if (!v->m_marked) {
			stmts = v->dag_to_stmt(stmts);
		}
	}

	if (m_stmt != NULL) {
		stmts.push_back(*m_stmt);
	}

	return stmts;
}

void DAGNode::cuts()
{
  for (CutMap::iterator I = m_cut.begin(); I != m_cut.end(); ++I) {
#ifdef RDS_DEBUG
    cout << "Cut at " << (I)->first->m_label << " "<< (I)->second << endl;
#endif
  }
}

// return statement list for dag starting at this node
ShBasicBlock::ShStmtList DAGNode::get_statements() {
	unvisitall();
	ShBasicBlock::ShStmtList stmts;
	stmts = dag_to_stmt(stmts); 
	return stmts;
}

// text output of this node and its successors
void DAGNode::print(int indent) {
	shPrintIndent(cout, indent);	
	cout << "-->";
	cout << m_label << " {";
	for (IdSet::iterator I = id_list.begin(); I != id_list.end(); ++I) {
		cout << " " << (*I)->name() << (*I)->swizzle();
	}
	cout << " }\n";
	for (DAGNodeVector::iterator I = successors.begin(); I != successors.end(); ++I) {
			(*I)->print(indent + 2);
	}
}

// statement output of this node and its successors
void DAGNode::dump_stmts() {
	ShBasicBlock::ShStmtList stmts = get_statements(); 
	for (ShBasicBlock::ShStmtList::const_iterator I = stmts.begin(); I != stmts.end(); ++I) {      
		cout << *I << "\n";
	}
}

// set # of instructions, textures, params, attribs, etc for pass starting at this node
void DAGNode::set_resources() {
	init_resources();
	unvisitall();
	ShBasicBlock::ShStmtList stmts = get_statements();
		 
    for (ShBasicBlock::ShStmtList::const_iterator I = stmts.begin(); I != stmts.end(); ++I) {      
      set_var(I->dest.node());
      set_var(I->src[0].node());
      set_var(I->src[1].node());
      set_var(I->src[2].node());
	  add_extras(*I);
    }

	m_num_instrs = stmts.size();
	unvisitall();
}

void DAGNode::set_resources_stmt() {
	init_resources();
		 
    if (m_stmt != NULL) {
		set_var(m_stmt->dest.node());
		set_var(m_stmt->src[0].node());
		set_var(m_stmt->src[1].node());
		set_var(m_stmt->src[2].node());
		m_num_instrs++;
	}
}

void DAGNode::add_var_to_list(DAGNode::VarList *vlist, const ShVariableNodePtr& var) {
	if (vlist->find(var) == vlist->end())
		vlist->insert(var);
}

void DAGNode::set_var(const ShVariableNodePtr& var) {
  if (!var) return;
  if (var->uniform()) {
	add_var_to_list(&m_params, var);  
  } 
  else 
	  switch (var->kind()) {
	  case SH_INPUT:
		add_var_to_list(&m_attribs, var);
		break;
	  case SH_OUTPUT:
		add_var_to_list(&m_outputs, var);
		break;
	  case SH_INOUT:
		add_var_to_list(&m_outputs, var);
		add_var_to_list(&m_attribs, var);
		break;
	  case SH_TEMP:		
		// right now Sh allocates all temps twice; once for temps and
	    // once for half-temps (see ArbCode)
		add_var_to_list(&m_temps, var);
		add_var_to_list(&m_halftemps, var);
	  	break;
	  case SH_CONST:
		add_var_to_list(&m_consts, var);
		break;
	  case SH_TEXTURE:
		add_var_to_list(&m_texs, var);   
		break;
	  case SH_STREAM:
		add_var_to_list(&m_channels, var);
		break;
	  case SH_PALETTE:
		for (unsigned int j = 0; j < (shref_dynamic_cast<ShPaletteNode>(var))->palette_length(); j++) {
		  add_var_to_list(&m_params, (shref_dynamic_cast<ShPaletteNode>(var))->get_node(j));
		}
		break;
	  default:
		SH_DEBUG_PRINT(0);
		break;
	} 
}

void DAGNode::prune_vars() {
	if (m_visited) return;

	m_visited = true;

	for (DAGNodeVector::iterator I = successors.begin(); I != successors.end(); ++I) {
		if (!(*I)->m_visited)
			(*I)->prune_vars();
		
		if ((*I)->m_stmt == NULL && (*I)->successors.size() == 0) {
			I = successors.erase(I);
			--I;
		}
	}
}

void DAGNode::print_resources() {
	cout << "Resources:\n";
	cout << "\tInstructions: " << instrs() << "\n";
	cout << "\tHalf-float Temporaries " << halftemps() << "\n";
	cout << "\tTemporaries " << temps() << "\n";
	cout << "\tAttributes " << attribs() << "\n";
	cout << "\tParameters " << params() << "\n";
	cout << "\tTextures " << texs() << "\n";
	cout << "\tConstants " << consts() << "\n";
	cout << "\tOutputs " << outputs() << "\n";
} 

// returns variable associated with this variable
// creates a new one if it doesn't exist
ShVariable *DAG::find_var(ShVariable *var) {
	VarVectorMap::iterator node_it;

	node_it = m_nodevector.find(var->node());
				
	if (node_it == m_nodevector.end()) {
		VarVector v;
		v.push_back(var);
		m_nodevector[var->node()] = v;
	}
	else {
		for (VarVector::iterator I = (*node_it).second.begin(); I != (*node_it).second.end(); ++I) {
			if ((*I)->swizzle() == var->swizzle() && (*I)->neg() == var->neg()) {
				return *I;
			}
		}
		
		(*node_it).second.push_back(var);
	}

	return var;
}

// adds an Sh statement to the graph
void DAG::add_statement(Stmt *stmt) {
	NodeMap::iterator node_it;
	int src_size = opInfo[stmt->op].arity;
	DAGNode *src[src_size], *n;
	
	// Source: The Dragon Book, section 9.8 (page 549)

	// Step 1
	// go through each of the src fields, creating new nodes if they don't exist
	for (int i = 0; i < src_size; i++) {
		node_it = node.find( find_var( &(stmt->src[i]) ));
				
		if (node_it == node.end()) {
			src[i] = new DAGNode( &(stmt->src[i]) );
			node[&(stmt->src[i])] = src[i];
		}
		else {
			src[i] = (*node_it).second;
		}
	}

	// create a new node for the statement (if full Step 2, check created here first)
	n = new DAGNode(stmt);

	// attach all kids
	for (int i = 0; i < src_size; i++) {
		n->add_kid(src[i]);
	}

	// Step 3
	// delete dest from list of ids for node(dest)
	ShVariable *var = find_var(&(stmt->dest));
	NodeMap::iterator dest_it = node.find(var);
	DAGNode *dest;

	if (dest_it != node.end()) {
		dest = (*dest_it).second;
		dest->id_list.erase((*dest_it).first);
	}

	// append dest to list of ids for n
	n->id_list.insert(var);

	node[var] = n;
}

DAG::DAG(ShBasicBlockPtr block)
        :   m_statements(block->m_statements)
{ 	
	// iterate through all statements
	for (StmtList::iterator I = m_statements.begin(); I != m_statements.end(); ++I) {
		add_statement(&(*I));
	}

	m_root = new DAGNode();
	m_root->m_label = "root";

	// pseudo-root points to all active nodes
	// (active meaning a variable is currently assigned to it) 
	for (NodeMap::iterator I = node.begin(); I != node.end(); ++I) {
		DAGNode *n = (*I).second;

		// if node has no parents, make root point to it
		if (n->predecessors.size() == 0) {
			m_root->successors.push_back(n);
			m_root->m_cut[n] = false;
			n->predecessors.push_back(m_root);
		}
	}

	// prune the variables (we don't care about predecessors anymore)
	m_root->unvisitall();
	m_root->prune_vars();
	m_root->unvisitall();

	return;
}

void DAG::print(int indent) {
	m_root->print(indent);
}

void DAGNode::add_extras(const ShStatement& stmt) {
	switch(stmt.op) {
		case SH_OP_DIV:
		case SH_OP_SQRT:
		case SH_OP_LRP:
		case SH_OP_DOT:
			m_extra_instrs += 1;
			m_extra_temps += 2;
			break;
		case SH_OP_SEQ:
		case SH_OP_SNE:
			m_extra_instrs += 3;
			m_extra_temps += 2;
			break;
		case SH_OP_CEIL:
			m_extra_instrs += 2;
			break;
		case SH_OP_MOD:
			m_extra_instrs += 8;
			m_extra_temps += 5;
			break;
		case SH_OP_COS:
		case SH_OP_SIN:
			m_extra_instrs += 2 + 12 * stmt.src[0].size();
			m_extra_temps += 4;
			m_extra_consts += 5;
			break;
		case SH_OP_TAN:
			m_extra_instrs += 5 + 24 * stmt.src[0].size();
			m_extra_temps += 10;
			m_extra_consts += 10;
			break;
		case SH_OP_ACOS:
		case SH_OP_ASIN:
		case SH_OP_ATAN:
		case SH_OP_ATAN2:
			m_extra_instrs += 4 + 12 * stmt.src[0].size();
			m_extra_temps += 5;
			m_extra_consts += 5;
			break;
		case SH_OP_EXP:
		case SH_OP_EXP10:
			m_extra_instrs += 1;
			m_extra_consts += 1;
			break;
		case SH_OP_LOG:
		case SH_OP_LOG2:
			m_extra_instrs += 2;
			m_extra_temps += 1;
			m_extra_consts += 1;
			break;
		case SH_OP_NORM:
			m_extra_instrs += 3;
			m_extra_temps += 2;
			break;
		case SH_OP_SGN:
			m_extra_instrs += 3;
			m_extra_temps += 2;
			break;
		case SH_OP_TEX:
		case SH_OP_COND:
			m_extra_instrs += 2;
			m_extra_temps += 1;
			break;
		case SH_OP_CSUM:
		case SH_OP_CMUL:
			m_extra_instrs += 2 + stmt.src[0].size();
			m_extra_temps += 1;
			break;
		case SH_OP_KIL:
			m_extra_instrs += 1;
			break;
		case SH_OP_PAL:
			m_extra_instrs += 2;
			break;
		default:
			break;
	}
}
