#include <iostream>
#include "DAG.hpp"

using namespace SH;
using namespace std;

// creates a leaf node
DAGNode::DAGNode(ShVariable var) 
	:	m_var(var),
		m_label(var.node()->name())
{
	m_type = DAG_LEAF;
}

// creates an operation node
DAGNode::DAGNode(ShOperation op) 
	:	m_op(op),
		m_label(static_cast<std::string>(opInfo[op].name))	
{
	m_type = DAG_OP;
}

// adds a node to successors of this one
void DAGNode::add_kid(DAGNode *kid)
{
	successors.push_back(kid);
	m_cut[kid] = false;
	kid->predecessors.push_back(this);
}

// text output of this node and its successors
void DAGNode::print(int indent) {
	shPrintIndent(cout, indent);	
	cout << "-->";
	cout << m_label << " {";
	for (IdSet::iterator I = id_list.begin(); I != id_list.end(); ++I) {
		cout << " " << (*I)->name();
	}
	cout << " }\n";
	for (DAGNodeVector::iterator I = successors.begin(); I != successors.end(); ++I) {
			(*I)->print(indent + 2);
	}
}

// adds an Sh statement to the graph
void DAG::add_statement(Stmt stmt) {
	NodeMap::iterator node_it;
	OpMap::iterator op_it;
	OpVector op_v;
	bool created = false;
	int src_size = opInfo[stmt.op].arity;
	DAGNode *src[src_size], *n;
	
	// Source: The Dragon Book, section 9.8 (page 549)

	// Step 1
	// go through each of the src fields, creating new nodes if they don't exist
	for (int i = 0; i < src_size; i++) {
		node_it = node.find(stmt.src[i].node());
				
		if (node_it == node.end()) {
			src[i] = new DAGNode(stmt.src[i]);
			node[stmt.src[i].node()] = src[i];
		}
		else {
			src[i] = (*node_it).second;
		}
	}

	// Step 2
	if (stmt.op == SH_OP_ASN) {
		// in assignment, just set n to node(src[0])
		n = src[0];
		created = true;
	}
	else {
		// see if a node for this operator exists
		op_it = ops[src_size].find(stmt.op);

		if (op_it != ops[src_size].end()) {
			// see if matching set of kids in vector for this operation
			op_v = (*op_it).second;

			for(OpVector::iterator V = op_v.begin(); !created && V != op_v.end(); ++V) {
				DAGNode *v = *V;
				for (int i = 0; i < src_size; i++) {
					if (v->successors.at(i) != src[i]) {
						break;
					}
					else if (i == src_size - 1) {
						created = true;
						n = v;
					}
				}
			}
		}
	}

	// create a new node for the operation if not found
	if (!created) {
		n = new DAGNode(stmt.op);

		// attach all kids
		for (int i = 0; i < src_size; i++) {
			n->add_kid(src[i]);
		}
		ops[src_size][stmt.op].push_back(n);
	}

	// Step 3
	// delete dest from list of ids for node(dest)
	NodeMap::iterator dest_it = node.find(stmt.dest.node());
	DAGNode *dest;

	if (dest_it != node.end()) {
		dest = (*dest_it).second;
		dest->id_list.erase((*dest_it).first);
	}

	// append dest to list of ids for n
	n->id_list.insert(stmt.dest.node());

	node[stmt.dest.node()] = n;
}

DAG::DAG(ShBasicBlockPtr block)
        :   m_statements(block->m_statements)
{ 	
	// iterate through all statements
	for (StmtList::iterator I = m_statements.begin(); I != m_statements.end(); ++I) {
		add_statement(*I);
	}

	m_root = new DAGNode();
	m_root->m_label = "root";

	ShOperation op = SH_OP_ASN;

	// this might be a rough implementation for now -- an assignment node is created for each active node
	// and the root just points to these assignment nodes
	for (NodeMap::iterator I = node.begin(); I != node.end(); ++I) {
		// if the node isn't a leaf, add an assignment node
		if ((*I).second->successors.size() > 0) {
			DAGNode *parent = new DAGNode(op);
			parent->add_kid((*I).second);
			parent->id_list.insert((*I).first);
			//(*I).second->id_list.erase((*I).first);
			node[(*I).first] = parent;
			m_root->successors.push_back(parent);
			m_root->m_cut[parent] = false;
			parent->predecessors.push_back(m_root);
		}
	}

	return;
}

void DAG::print(int indent) {
	m_root->print(indent);
}
