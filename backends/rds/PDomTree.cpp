/***
 * Partial Dominator Tree 
 *
 * Last Revised: February 17 by Cynthia
 *
 ***/

#include "PDomTree.hpp"
#include <iostream>
#include <fstream>
#include <iostream.h>
#include "ShDebug.hpp"
#include "ShUtility.hpp"

using namespace SH;

PDomTree::PDomTree(DAG::DAG* dag)
		: m_graph(dag), 
          m_n(0)
{
	m_vertex.push_back(0); // Hack to have indexing start at 1.

	// Step 1 -- dfs to number vertices
	dfs(m_graph->m_root);

	// Steps 2 and 3
	for (int i = m_n; i >= 2; i--) {
		DAGNode::DAGNode* w = m_vertex[i];

		// Step 2
		PredSet& pred = m_pred[w];

		for (PredSet::iterator I = pred.begin(); I != pred.end(); ++I) {
			DAGNode::DAGNode* v = *I;
			DAGNode::DAGNode* u = eval(v);
			if (m_semi[u] < m_semi[w]) {
				m_semi[w] = m_semi[u];
			}
		}

		m_bucket[m_vertex[m_semi[w]]].insert(w);
		link(m_parent[w], w);

		// Step 3
		BucketSet& bucket = m_bucket[m_parent[w]];
		while (!bucket.empty()) {
			BucketSet::iterator I = bucket.begin();
			DAGNode::DAGNode* v = *I;
			bucket.erase(I);
			DAGNode::DAGNode* u = eval(v);
			dom[v] = (m_semi[u] < m_semi[v] ? u : m_parent[w]);
		}
	}

	// Step 4
	for (int i = 2; i <= m_n; i++) {
		DAGNode::DAGNode* w = m_vertex[i];
		if (dom[w] != m_vertex[m_semi[w]]) {
			dom[w] = dom[dom[w]];
		}
	}

	dom[m_graph->m_root] = 0;

	/* End of Lengauer/Tarjan dominator algorithm */

	/* Generate the partial dom tree */
	build_pdt(m_graph->m_root);
}

/* Depth-first search (also marks multi-referenced nodes) */
void PDomTree::dfs(DAGNode::DAGNode* v)
{
	if (!v) return;

	// node shouldn't be marked as multi-referenced initially
	//m_mr[v] = false;
	//m_pdt[v] = false;

	m_n++;
	m_semi[v] = m_n;
	m_vertex.push_back(v);
	m_ancestor[v] = 0;
	m_label[v] = v;

	for (DAGNode::DAGNodeVector::iterator I = v->successors.begin(); I != v->successors.end(); ++I) {
		DAGNode::DAGNode *w = *I;
		if (m_semi[w] == 0) {
			m_parent[w] = v;
			dfs(w);
		}
		else {
			// this node has been visited already -- mark it as multi-referenced
			m_mr[w] = true;
			m_pdt[w] = true;
		}
		m_pred[w].insert(v);
	}
}

DAGNode::DAGNode* PDomTree::eval(DAGNode::DAGNode* v)
{
	if (!m_ancestor[v]) {
		return v;
	} 
	else {
		compress(v);
		return m_label[v];
	}
}

void PDomTree::compress(DAGNode::DAGNode* v)
{
	// This procedure assumes ancestor[v] != 0
	if (m_ancestor[m_ancestor[v]]) {
		compress(m_ancestor[v]);
		if (m_semi[m_label[m_ancestor[v]]] < m_semi[m_label[v]]) {
			m_label[v] = m_label[m_ancestor[v]];
		}
		m_ancestor[v] = m_ancestor[m_ancestor[v]];
	}
}

void PDomTree::link(DAGNode::DAGNode* v, DAGNode::DAGNode* w)
{
	m_ancestor[w] = v;
	m_children[v].insert(w);
}

/* record the children of each node in the partial dom tree */ 
/* children listed in same order as postorder traversal of original graph */
void PDomTree::build_pdt(DAGNode::DAGNode* v)
{
	if (m_visited[v]) return;

	// visit v's children first
	for (DAGNode::DAGNodeVector::iterator I = v->successors.begin(); I != v->successors.end(); ++I) {
		build_pdt(*I);
	}

	// visit the node itself
	if (m_pdt[v]) {
		m_pdt[dom[v]] = true;
		if (!m_visited[v]) {
			pchildren[dom[v]].push_back(v);
		}
	}
	m_visited[v] = true;
}

void PDomTree::printDoms() {
	for (int i = 1; i <= dom.size(); i++) {
		DAGNode::DAGNode* v = m_vertex[i];

		cout << "Node " << i << " Dom " << numbering(dom[v]) << " Succ";

		// print this node's successors, in numerical form
		for (DAGNode::DAGNodeVector::iterator I = v->successors.begin(); I != v->successors.end(); ++I) {
			DAGNode::DAGNode *w = *I;
			if (w)
				cout << " " << numbering(w);
		}

		cout << " Pred";
		// print this node's predecessors, in numerical form
		for (DAGNode::DAGNodeVector::iterator I = v->predecessors.begin(); I != v->predecessors.end(); ++I) {
			DAGNode::DAGNode *w = *I;
			if (w)
				cout << " " << numbering(w);
		}

		// print this node's successors in the pdom tree
		cout << " PSucc";
		for (PChildVector::iterator I = pchildren[v].begin(); I != pchildren[v].end(); ++I) {
			DAGNode::DAGNode *w = *I;
			if (w)
				cout << " " << numbering(w);
		}

		cout << " MR " << m_mr[v] << "\n";
	}
}

void PDomTree::printGraph(DAGNode::DAGNode *node, int indent) {
	shPrintIndent(cout, indent);
	cout << numbering(node) << " " << node->m_label << "\n";

	for (DAGNode::DAGNodeVector::iterator I = node->successors.begin(); I != node->successors.end(); ++I) {
		if (!node->m_cut[*I])
			printGraph(*I, indent + 2);
		else {
			shPrintIndent(cout, indent + 2);
			cout << "cut--> " << numbering(*I) << " " << (*I)->m_label << "\n";
		}
	}
}

std::ostream& PDomTree::graphvizDump(DAGNode::DAGNode *node, std::ostream& out) {
  out << "  \"" << node->m_label << numbering(node) << "\";" << std::endl;
  for (DAGNode::DAGNodeVector::iterator I = node->successors.begin(); I != node->successors.end(); ++I) {
    graphvizDump( *I, out );
    out << "  \"" << node->m_label << numbering(node) << "\" -> \"" 
        << (*I)->m_label << numbering(*I) << "\";" << std::endl;
  }
  return out;
}

void PDomTree::graphvizDump(char* filename) {
  std::ofstream out(filename);
  out << "digraph g {" << std::endl;
  graphvizDump(get_root(),out);
  out << "}" << std::endl;
  out.close();
}
