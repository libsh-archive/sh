#include <iostream>
#include "DAG.hpp"

using namespace SH;
using namespace std;

// creates a leaf node
DAGNode::DAGNode(ShVariable *var) 
	:	m_var(var),
		m_label(var->node()->name())
{
	m_visited = false;
}

// creates an operation node
DAGNode::DAGNode(ShStatement *stmt) 
	:	m_stmt(stmt),
		m_label(static_cast<std::string>(opInfo[stmt->op].name))	
{
	m_visited = false;
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

ShBasicBlock::ShStmtList DAGNode::dag_to_stmt(ShBasicBlock::ShStmtList stmts)
{
	if (successors.size() == 0) {
		m_visited = true;
		return stmts;
	}

	for (DAGNodeVector::iterator I = successors.begin(); I != successors.end(); ++I) {
		if (!((*I)->m_visited) && !m_cut[*I])
			stmts = (*I)->dag_to_stmt(stmts);
	}

	if (!m_visited && predecessors.size() > 0) {
		stmts.push_back(*m_stmt);
		m_visited = true;
	}
	return stmts;
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


// returns node associated with this variable
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
