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

// reset all resource consumption fields
void DAGNode::init_resources(){
	m_instrs = 0;		
	m_params = 0;		
	m_attribs = 0;		
	m_temps = 0;		
	m_halftemps = 0;	
	m_texs = 0;			
	m_consts = 0;		
	m_outputs = 0;
	m_channels = 0;
}

ShBasicBlock::ShStmtList DAGNode::dag_to_stmt(ShBasicBlock::ShStmtList stmts, bool cut)
{
	if (successors.size() == 0) {
		m_visited = true;

		if (m_stmt == NULL) {
			return stmts;
		}
	}

	for (DAGNodeVector::iterator I = successors.begin(); I != successors.end(); ++I) {
		if (!((*I)->m_visited)) {
			if ((cut && !m_cut[*I]) || (!cut && !((*I)->m_marked || (*I)->m_fixed == RDS_MARKED))) {
				stmts = (*I)->dag_to_stmt(stmts, cut);
			}
		}
		else if (cut) {
#ifdef RDS_DEBUG
	cout << "Cut at " << (*I)->m_label << endl;
	//(*I)->print_stmts();
	if ( (*I)->m_stmt != NULL)
	cout << "  STMT " << *(*I)->m_stmt << endl;
#endif
		}
	}

	if (!m_visited && predecessors.size() > 0) {
		stmts.push_back(*m_stmt);
		m_visited = true;
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
	ShBasicBlock::ShStmtList stmts = dag_to_stmt(stmts, true); 
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

void DAGNode::print_stmts() {
	for (DAGNodeVector::iterator I = successors.begin(); I != successors.end(); ++I)  {
		if (!(*I)->m_visited && !m_cut[*I])
			(*I)->print_stmts();
	}
	
	if (!m_visited && successors.size() > 0 && predecessors.size() > 0) {
		cout << *m_stmt << "\n";
		m_visited = true;
	}
}

// statement output of this node and its successors
void DAGNode::dump_stmts() {
	unvisitall();
	print_stmts();
}

void DAGNode::set_resources() {
	unvisitall();
	init_resources();
	ShBasicBlock::ShStmtList stmts;
	stmts = dag_to_stmt(stmts, false);
		 
    for (ShBasicBlock::ShStmtList::const_iterator I = stmts.begin(); I != stmts.end(); ++I) {      
      set_var(I->dest.node());
      set_var(I->src[0].node());
      set_var(I->src[1].node());
      set_var(I->src[2].node());
    }

	unvisitall();
	m_instrs = count_instrs();
}

void DAGNode::set_resources_stmt() {
	init_resources();
		 
    if (m_stmt != NULL) {
		set_var(m_stmt->dest.node());
		set_var(m_stmt->src[0].node());
		set_var(m_stmt->src[1].node());
		set_var(m_stmt->src[2].node());
		m_instrs++;
	}
}

void DAGNode::set_var(const ShVariableNodePtr& var) {
  if (!var) return;
  if (var->uniform()) {
    m_params++;
  } 
  else switch (var->kind()) {
	  case SH_INPUT:
	  	m_attribs++;
		break;
	  case SH_OUTPUT:
	  	m_outputs++;
		break;
	  case SH_INOUT:
		m_outputs++;
		m_attribs++;
		break;
	  case SH_TEMP:		
		// right now Sh allocates all temps twice; once for temps and
	    // once for half-temps (see ArbCode)
		m_temps++;
		m_halftemps++;
	  	break;
	  case SH_CONST:
		m_consts++;
		break;
	  case SH_TEXTURE:
		m_texs++;   
		break;
	  case SH_STREAM:
		m_channels++;
		break;
	  case SH_PALETTE:
		m_params += (shref_dynamic_cast<ShPaletteNode>(var))->palette_length();
		break;
	  default:
		SH_DEBUG_PRINT(0);
		break;
	}
}

int DAGNode::count_instrs() {
	if (m_visited || successors.size() < 1) return 0;
	
	m_visited = true;
	
	int count = 1;
	
	for (DAGNode::DAGNodeVector::iterator I = successors.begin(); I != successors.end(); ++I) {
		if (!(m_marked || m_fixed == RDS_MARKED)) {
			count += (*I)->count_instrs();
		}
	} 
	return count;
}

void DAGNode::print_resources() {
	cout << "Resources:\n";
	cout << "\tInstructions: " << m_instrs << "\n";
	cout << "\tHalf-float Temporaries " << m_halftemps << "\n";
	cout << "\tTemporaries " << m_temps << "\n";
	cout << "\tAttributes " << m_attribs << "\n";
	cout << "\tParameters " << m_params << "\n";
	cout << "\tTextures " << m_texs << "\n";
	cout << "\tConstants " << m_consts << "\n";
	cout << "\tChannels " << m_channels << "\n";
	cout << "\tOutputs " << m_outputs << "\n";
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
	//OpMap::iterator op_it;
	//OpVector op_v;
	//bool created = false;  // for common subexpressions
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
	
	//ops[src_size][stmt.op].push_back(n);

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

	return;
}

void DAG::print(int indent) {
	m_root->print(indent);
}