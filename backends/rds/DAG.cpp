#include <iostream>
//#include <iostream.h>
#include "DAG.hpp"

using namespace SH;
using namespace std;

DAGNode::DAGNode(ShVariable var) 
	:	m_var(var),
		m_label(var.node()->name())
{
	//cout << "Creating new node with name " << m_label << "\n";
	m_type = DAG_LEAF;
}

DAGNode::DAGNode(ShOperation op) 
	:	m_op(op),
		m_label(static_cast<std::string>(opInfo[op].name))	
{
	//cout << "Creating new node with name " << m_label << "\n";
	m_type = DAG_OP;
}

DAGNode::DAGNode(ShOperation op, DAGNode* kid) 
	:	m_op(op),
		m_label(static_cast<std::string>(opInfo[op].name))
{
	//cout << "Creating new node with name " << m_label << " Kids: " << kid->m_label << "\n";
	m_type = DAG_OP;
	successors.push_back(kid);
	m_cut[kid] = false;
	kid->predecessors.push_back(this);
}

DAGNode::DAGNode(ShOperation op, DAGNode *kid0, DAGNode* kid1)
	:	m_op(op),
		m_label(static_cast<std::string>(opInfo[op].name))
{
	//cout << "Creating new node with name " << m_label << " Kids: " << kid0->m_label << " " << kid1->m_label <<"\n";
	m_type = DAG_OP;
	successors.push_back(kid0);
	successors.push_back(kid1);
	m_cut[kid0] = false;
	m_cut[kid1] = false;
	kid0->predecessors.push_back(this);
	kid1->predecessors.push_back(this);
}

DAGNode::DAGNode(ShOperation op, DAGNode* kid0, DAGNode* kid1, DAGNode* kid2) 
	:	m_op(op),
		m_label(static_cast<std::string>(opInfo[op].name))
{
	//cout << "Creating new node with name " << m_label << " Kids: " << kid0->m_label << " " << kid1->m_label << kid2->m_label << "\n";
	m_type = DAG_OP;
	successors.push_back(kid0);
	successors.push_back(kid1);
	successors.push_back(kid2);
	m_cut[kid0] = false;
	m_cut[kid1] = false;
	m_cut[kid2] = false;
	kid0->predecessors.push_back(this);
	kid1->predecessors.push_back(this);
	kid2->predecessors.push_back(this);
}

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

			switch (src_size) {
				case 1:
					for(OpVector::iterator V = op_v.begin(); !created && V != op_v.end(); ++V) {
						DAGNode *v = *V;
						if (v->successors.at(0) == src[0]) {
							created = true;
							n = v;
						}
					}
					break;
				case 2:
					for(OpVector::iterator V = op_v.begin(); !created && V != op_v.end(); ++V) {
						DAGNode *v = *V;
						//cout << "kids: " << v->kids[0] << " " << v->kids[1] << " src; " << src[0] << " " << src[1] << "\n";
						if (v->successors.at(0) == src[0] && 
							v->successors.at(1) == src[1]) {
							created = true;
							n = v;
						}
					}
					break;
				case 3:
					for(OpVector::iterator V = op_v.begin(); !created && V != op_v.end(); ++V) {
						DAGNode *v = *V;
						if (v->successors.at(0) == src[0] &&
							v->successors.at(1) == src[1] &&
							v->successors.at(2) == src[2]) {
							created = true;
							n = v;
						}
					}
					break;
			}
		}
	}

	// create a new node for the operation if not found
	if (!created) {
		switch (src_size) {
			case 0:
				n = new DAGNode(stmt.op);
				//cout << "making node for " << opInfo[stmt.op].name << "\n";
				break;
			case 1:
				n = new DAGNode(stmt.op, src[0]);					
				//cout << "making node for " << opInfo[stmt.op].name << " " << src[0]->m_label << "\n";
				break;
			case 2:
				n = new DAGNode(stmt.op, src[0], src[1]);
				//cout << "making node for " << opInfo[stmt.op].name << " " << src[0]->m_label << " " << src[1]->m_label << "\n";
				break;
			case 3:
				n = new DAGNode(stmt.op, src[0], src[1], src[2]);
				//cout << "making node for " << opInfo[stmt.op].name << " " << src[0]->m_label << " " << src[1]->m_label << " " << src[2]->m_label << "\n";
				break;
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
		//cout << "Delete" <<  (*dest_it).first << " from " << dest->m_label << "\n";
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
			DAGNode *parent = new DAGNode(op, (*I).second); 
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
