#include "FullSearch.hpp"
#include "NextKSubset.hpp"

FullSearch::FullSearch(SH::ShProgramNodePtr progPtr)
	: m_graph(new DAG(progPtr->ctrlGraph->entry()->follower->block))	
{
	m_callback = new CallBack();
	m_part = new Partition(m_graph);
	full_search();
}

// brute force. code will be cleaned up later.
void FullSearch::full_search() {
	DAGNode::DAGNode *root = m_graph->m_root;
	bool found = false;
	float low_cost = 0.0;

	// get a list of all the nodes in the graph
	for (DAGNode::DAGNodeVector::iterator I = root->successors.begin(); I != root->successors.end(); ++I) {
		set_nodelist(*I);
	}	

	root->unfixall();

	// case zero
	int n = m_nodelist.size();
	bool marked[n];
	
	for (int j = 0; j < n; j++) {
		marked[j] = false;
	}

	root->unmarkall();
	root->mark();

	if(m_callback->valid(root)) {
		found = true;
		low_cost = m_callback->cost(root);
	}

	// use next-ksubset to choose marked nodes
	for (int k = 1; k < n; k++) {	
		int *a;

		ksub_init(n, k);
		a = ksub_next();

		// mark according to subsets
		root->unmarkall();

		for (int j = 0; j < k; j++) {
			m_nodelist.at(a[j])->mark();
		}

		root->mark();

		bool isvalid = true;

		if (!m_callback->valid(root)) 
			isvalid = false;

		for (int j = 0; j < k && isvalid; j++) {
			if (!m_callback->valid(m_nodelist.at(a[j])))
				isvalid = false;
		}

		if (isvalid) {
			float this_cost = m_callback->cost(root);

			// see if present optimal partition
			if (!found || this_cost < low_cost) {
				for (int j = 0; j < n; j++) {
					marked[j] = false;
				}

				for (int j = 0; j < k; j++) {
					marked[a[j]] = true;
				}

				low_cost = this_cost;
				found = true;
			}
		}

		// check all other subsets
		while(ksub_more()) {				
			a = ksub_next();
		
			// mark according to subsets
			root->unmarkall();

			for (int j = 0; j < k; j++) {
				m_nodelist.at(a[j])->mark();
			}

			root->mark();

			bool isvalid = true;

			if (!m_callback->valid(root)) 
				isvalid = false;

			for (int j = 0; j < k && isvalid; j++) {
					if (!m_callback->valid(m_nodelist.at(a[j])))
						isvalid = false;
			}

			if (isvalid) {
				float this_cost = m_callback->cost(root);
				// see if present optimal partition
				if (!found || this_cost < low_cost) {
					for (int j = 0; j < n; j++) {
						marked[j] = false;
					}

					for (int j = 0; j < k; j++) {
						marked[a[j]] = true;
					}

					low_cost = this_cost;
					found = true;
				}
			}
		}
	}

	// mark the correct set of passes
	root->unvisitall();
	root->unmarkall();

	for(int i = 0; i < n; i++) {
		if (marked[i])
			m_nodelist.at(i)->mark();
	}
	root->mark();
	m_part->set_partition(root);
}

void FullSearch::set_nodelist(DAGNode::DAGNode *v) {
	if (v->visited()) return;

	v->visit();
	m_nodelist.push_back(v);

	for (DAGNode::DAGNodeVector::iterator I = v->successors.begin(); I != v->successors.end(); ++I) {
		set_nodelist(*I);
	}
}

/*****************
 * NextKSubset stuff here for now because of strange linking issues
 */

 /***
 * source: Nijenhuis & Wilf, Combinatorial Algorithms, Academic Press, 1975.
 * changed for range 0...n-1 (instead of 1...n)
 *		n: number of elements in universe
 *		k: number of elements in subset
 *		*a:	a(i) is ith element in subset
 */
int *FullSearch::ksub_next() {
	if (!ksub_mtc) {
		ksub_m2 = -1;
		ksub_h = m_k;
	}
	else {
		if (ksub_m2 < m_n - ksub_h - 1) {
			ksub_h = 0;
		}
		ksub_h++;
		ksub_m2 = m_a[m_k - ksub_h];
	}

	for(int j = 1; j <= ksub_h; j++) {
		m_a[m_k + j - ksub_h - 1] = ksub_m2 + j;
	}

	ksub_mtc = (m_a[0] != (m_n - m_k));

	return m_a;
}


void FullSearch::ksub_init(int n, int k) {
	// ksubset init stuff
	m_a = (int *) malloc(sizeof(int) * (n + 1));

	for (int j = 0; j <= k; j++) {
		m_a[j] = j;
	}

	ksub_mtc = false;
	m_n = n;
	m_k = k;
}