// RDS v 1.0
//
//  Last Revised: Nov. 28 by Cynthia
//
//  A somewhat skeletal version of the RDS pseudocode
//////////////////////////////////////////////////////////////////////////////

#include "RDS.hpp"
#include <iostream>
#include <fstream>
#include <iostream.h>
#include "ShDebug.hpp"
#include "ShUtility.hpp"

using namespace SH;

RDS::RDS(ShProgramNodePtr progPtr)
	: m_graph(new DAG(progPtr->ctrlGraph->entry()->follower->block))	
{
	// get partial dom tree	
	m_pdt = new PDomTree(m_graph);
	set_limits();
}

void RDS::set_limits() {
	max_ops = 4;
}

void RDS::print_partitions(char *filename) {
  std::ofstream dump(filename);
  dump << "digraph g {" << std::endl;
    for(PassVector::iterator I = m_passes.begin(); I != m_passes.end(); ++I) {
      m_pdt->graphvizDump(*I, dump);
    }
  dump << "}" << std::endl;
  dump.close();
}

void RDS::print_partition() {
	int count = 0;
	for(PassVector::iterator I = m_passes.begin(); I != m_passes.end(); ++I) {
		cout << "Pass " << ++count << "\n";
		m_pdt->printGraph(*I, 2);
	}
}

void RDS::set_partition() {
	DAGNode::DAGNode *root = m_pdt->get_root();
	unvisitall(root);
	m_marked[root] = true;
	partition(root);
}

void RDS::rds_search() {
	DAGNode::DAGNode *p = m_pdt->get_root();
	DAGNode::DAGNode *d = m_graph->m_root;
		  
	// this sets m_mrlist
	unvisitall(d);
	unfixall(d);
	add_mr(d);
  
	for (MRList::iterator I = m_mrlist.begin(); I != m_mrlist.end(); ++I) {
		DAGNode::DAGNode *m = *I;
		
		// find cost of saving subregion m
		unmarkall(d);
		m_fixed[m] = RDS_MARKED;
		rds_subdivide(p);
		unvisitall(p);
		int cost_s = cost(p);

		// find cost of recomputing subregion m
		unmarkall(d);
		m_fixed[m] = RDS_UNMARKED;
		rds_subdivide(p);
		unvisitall(p);
		int cost_r = cost(p); 

		cout << "Cost Save = " << cost_s << " Cost Recompute = " << cost_r << "\n";
		if (cost_s < cost_r) {
			m_fixed[m] = RDS_MARKED;
		}
	}
}

// partitions m_graph by marking nodes to indicate pass boundaries
// returns a set of passes
void RDS::rds_subdivide(DAGNode::DAGNode* v) {
	//cout << "Subdivide(" << m_pdt->numbering(v) << ")\n";
    // stop if subregion v can be mapped in one pass
	if (valid(v)) return;
        
	// list of pdt children of v (postorder)
	PDomTree::PChildVector& kids = m_pdt->pchildren[v];
        
	for (PDomTree::PChildVector::iterator I = kids.begin(); I != kids.end(); ++I) {
		DAGNode::DAGNode *k = *I;
		rds_subdivide(k);
        
		if (m_pdt->mr(k)) { 
			if(m_rdsh) {
				if (recompute(k)) {
					// recompute k
					m_marked[k] = false;
				}
				else {
					// save k
					m_marked[k] = true;
				}
			}
			else {
				switch(m_fixed[k]) {
					case RDS_MARKED:
						m_marked[k] = true;;  
						break;
					case RDS_UNMARKED:
						m_marked[k] = false;;
						break;
					default:
						if (recompute(k)) {
							// recompute 
							m_marked[k] = false;
						}
						else {
							// save w
							m_marked[k] = true;
						}
						break;
				}
			}
        }
    }

	// apply greedy merging
    rds_merge(v);
}

void RDS::rds_merge(DAGNode::DAGNode* v)
{
	//cout << "Merge(" << m_pdt->numbering(v) << ")\n";
	
	// check for any fixed nodes (their marked property cannot be changed)
	DAGNode::DAGNodeVector kids;
	DAGNode::DAGNodeVector unmarked_kids;

	// ensure all subregions are valid before merging
	for (DAGNode::DAGNodeVector::iterator I = v->successors.begin(); I != v->successors.end(); ++I) {
		if (!valid(*I)) 
			rds_merge(*I);
	}

	if (!m_rdsh) {
		for (DAGNode::DAGNodeVector::iterator I = v->successors.begin(); I != v->successors.end(); ++I) {
			if (m_fixed[*I] == RDS_UNMARKED) {
				unmarked_kids.push_back(*I);

				// forcefully ensure that this kid can be merged with v (apply merge with reduced limits)
				max_ops--;

				if (!valid(*I))
					rds_merge(*I);
				
				max_ops++;
			}
			else if (m_fixed[*I] != RDS_MARKED)
				kids.push_back(*I);
		}
	}
	else {
		kids = v->successors;
	}

	// get the number of kids of v
	int k = kids.size();

	for (int d = k; d > 0; d--) {
		// get each subset of v's kids with d kids
		// try to merge each subset with v
		PassVector subsets;
		int *a;

		a = (int *) malloc(sizeof(int) * (k + 1));

		for (int j = 0; j <= d; j++) {
			a[j] = j;
		}

		ksub_mtc = false;
		a = next_ksubset(k, d, a);

		// join v with subset of successors
		DAGNode::DAGNode *w;
		
		if (v->m_type == DAG_OP)
			w = new DAGNode(v->m_op);
		else
			w = new DAGNode(v->m_var);

		if (m_rdsh) {
			for (int j = 0; j < d; j++) {
				w->successors.push_back(kids.at(a[j]));
			}
			
			if (valid(w)) {
				subsets.push_back(w);
			}
		}
		else {
			for (int j = 0; j < d; j++) {
				w->successors.push_back(kids.at(a[j]));
			}

			for (DAGNode::DAGNodeVector::iterator I = unmarked_kids.begin(); I != unmarked_kids.end(); ++I) {
				w->successors.push_back(*I);
			}

			if (valid(w)) {
				subsets.push_back(w);
			}
		}

		while(ksub_mtc && d!=0) {
			if (v->m_type == DAG_OP)
				w = new DAGNode(v->m_op);
			else
				w = new DAGNode(v->m_var);
			
      //FIXME this probably isn't what you wanted to comment out
			if (d != 0)
				//cout << "Next k-subset for " << d << "\n";
			
			a = next_ksubset(k, d, a);
			// join v with subset of successors
			if (m_rdsh) {
				for (int j = 0; j < d; j++) {
					w->successors.push_back(kids.at(a[j]));
				}
				
				if (valid(w)) {
					subsets.push_back(w);
				}
			}
			else {
				for (int j = 0; j < d; j++) {
					w->successors.push_back(kids.at(a[j]));
				}

				for (DAGNode::DAGNodeVector::iterator I = unmarked_kids.begin(); I != unmarked_kids.end(); ++I) {
					w->successors.push_back(*I);
				}

				if (valid(w)) {
					subsets.push_back(w);
				}
			}
		}

		// if a subset can be merged, select and stop
		if (subsets.size() > 0) {
			DAGNode::DAGNode *w = *(subsets.begin());
			// if more than one subset, use MERGE to pick one and stop
			if (subsets.size() > 1) {
				w = merge(subsets);
			}

			// mark all the kids of w
			for (DAGNode::DAGNodeVector::iterator I = kids.begin(); I != kids.end(); ++I) {
				m_marked[*I] = true;
			}

			// unmark all the kids in the subset
			for (DAGNode::DAGNodeVector::iterator I = w->successors.begin(); I != w->successors.end(); ++I) {
				m_marked[*I] = false;
			}

			// we're done
			return;
		}
	}

	// nothing could be merged; mark all the kids
	for (DAGNode::DAGNodeVector::iterator I = kids.begin(); I != kids.end(); ++I) {
		m_marked[*I] = true;
	}
}
  
// creates a post-order list of MR nodes, storing in m_mrlist
void RDS::add_mr(DAGNode::DAGNode* v) {
	if (!v) return;
 
	if (m_visited[v]) return; // already visited
		
	m_visited[v] = true;
    
	// visit v's children first
	for (DAGNode::DAGNodeVector::iterator I = v->successors.begin(); I != v->successors.end(); ++I) {
			add_mr(*I);
	}
    
	// add v to list if it's multi-referenced
	if (m_pdt->mr(v)) {
			m_mrlist.push_back(v);
	}
}

/***
 * source: Nijenhuis & Wilf, Combinatorial Algorithms, Academic Press, 1975.
 * changed for range 0...n-1 (instead of 1...n)
 *
 * in:	int n		number of elements in universe
 *		int k		number of elements in subset
 *		int *a		a(i) is ith element in subset
 * out: int *		a(i); the output subset
 */
int *RDS::next_ksubset(int n, int k, int *a) {
	if (!ksub_mtc) {
		ksub_m2 = -1;
		ksub_h = k;
	}
	else {
		if (ksub_m2 < n - ksub_h - 1) {
			ksub_h = 0;
		}
		ksub_h++;
		ksub_m2 = a[k - ksub_h];
	}

	for(int j = 1; j <= ksub_h; j++) {
		a[k + j - ksub_h - 1] = ksub_m2 + j;
	}

	ksub_mtc = (a[0] != (n - k));

	return a;
}
  

int RDS::countnodes(DAGNode::DAGNode *v) {
	if (t_visited[v]) return 0;
	t_visited[v] = true;
	int count = 1;
	
	for (DAGNode::DAGNodeVector::iterator I = v->successors.begin(); I != v->successors.end(); ++I) {
		if (!(m_marked[*I] || m_fixed[*I] == RDS_MARKED)) {
			count += countnodes(*I);
		}
	} 
	return count;
}

void RDS::unvisitall(DAGNode::DAGNode *v) {
	t_visited[v] = false;
	m_visited[v] = false;
	for (DAGNode::DAGNodeVector::iterator I = v->successors.begin(); I != v->successors.end(); ++I) {
		unvisitall(*I);
	}
}

void RDS::unfixall(DAGNode::DAGNode *v) {
	m_fixed[v] = RDS_UNFIXED;
	for (DAGNode::DAGNodeVector::iterator I = v->successors.begin(); I != v->successors.end(); ++I) {
		unfixall(*I);
	}
}

void RDS::unmarkall(DAGNode::DAGNode *v) {
	m_marked[v] = false;
	for (DAGNode::DAGNodeVector::iterator I = v->successors.begin(); I != v->successors.end(); ++I) {
		unmarkall(*I);
	}
}

// partitions graph; roots of partitions based on marked nodes
void RDS::partition(DAGNode::DAGNode *v)
{    
    if (!v) return;
	if (m_visited[v]) return;

	m_visited[v] = true;
    
    // partition each of v's children
	for (DAGNode::DAGNodeVector::iterator I = v->successors.begin(); I != v->successors.end();  ) {
		DAGNode::DAGNode *w = *I;
      
		if (!m_visited[w]) {
			partition(w);
		}
      
		// cut w off from v if it's marked
		if (m_marked[w]) {
			I = v->successors.erase(I); 
		}
		else {
			++I;
		}
    }
   
    // check if v is a partition root
	if (m_marked[v]) {
		m_passes.push_back(v);
    }   
}

bool RDS::valid(DAGNode::DAGNode* v) {
	unvisitall(v);
	return countnodes(v) <= max_ops;
}

bool RDS::recompute(DAGNode::DAGNode* v) {
	unvisitall(v);
	return countnodes(v) <= (max_ops/2); 
}

DAGNode::DAGNode* RDS::merge(PassVector passes) {
	PassVector::iterator I = passes.begin();
	int lowest = countnodes(*I);
	DAGNode::DAGNode *winner = *I;

	for(++I; I != passes.end(); ++I) {
		int count = countnodes(*I);
		if (lowest < 0 || count < lowest) {
			lowest = count;
			winner = *I;
		}
	}
	return winner;
}

int RDS::countmarked(DAGNode::DAGNode* v) {
	if (t_visited[v]) return 0;
	t_visited[v] = true;
	
	int count = 0;

	if (m_marked[v]) count++;
	
	for (DAGNode::DAGNodeVector::iterator I = v->successors.begin(); I != v->successors.end(); ++I) {
			count += countnodes(*I);
	} 
	return count;
}

int RDS::cost(DAGNode::DAGNode* v) {
	unvisitall(v);
	return countmarked(v);
}


void RDS::set_nodelist(DAGNode::DAGNode *v) {
	if (m_visited[v]) return;

	m_visited[v] = true;
	m_nodelist.push_back(v);

	for (DAGNode::DAGNodeVector::iterator I = v->successors.begin(); I != v->successors.end(); ++I) {
		set_nodelist(*I);
	}
}

// brute force. code will be cleaned up later.
void RDS::full_search() {
	DAGNode::DAGNode *root = m_graph->m_root;
	bool found = false;
	int low_cost = 0;

	// get a list of all the nodes in the graph
	for (DAGNode::DAGNodeVector::iterator I = root->successors.begin(); I != root->successors.end(); ++I) {
		set_nodelist(*I);
	}	

	unfixall(root);

	// case zero
	int n = m_nodelist.size();
	bool marked[n];
	unmarkall(root);

	if(valid(root)) {
		found = true;
		low_cost = cost(root);
	}

	// use next-ksubset to choose marked nodes
	for (int k = 1; k < n; k++) {
		int *a = (int *) malloc(sizeof(int) * (n+1));

		for (int j = 0; j <= k; j++) {
			a[j] = j;
		}

		ksub_mtc = false;
		a = next_ksubset(n, k, a);

		// mark according to subsets
		unmarkall(root);

		for (int j = 0; j < k; j++) {
			m_marked[m_nodelist.at(a[j])] = true;
		}

		m_marked[root] = true;

		bool isvalid = true;

		if (!valid(root)) 
			isvalid = false;

		for (int j = 0; j < k && isvalid; j++) {
			if (!valid(m_nodelist.at(a[j])))
				isvalid = false;
		}

		if (isvalid) {
			int this_cost = cost(root);

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
		while(ksub_mtc) {				
			a = next_ksubset(n, k, a);
		
			// mark according to subsets
			unmarkall(root);

			for (int j = 0; j < k; j++) {
				m_marked[m_nodelist.at(a[j])] = true;
			}

			m_marked[root] = true;

			bool isvalid = true;

			if (!valid(root)) 
				isvalid = false;

			for (int j = 0; j < k && isvalid; j++) {
					if (!valid(m_nodelist.at(a[j])))
						isvalid = false;
			}

			if (isvalid) {
				int this_cost = cost(root);

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
	unvisitall(root);
	unmarkall(root);

	for(int i = 0; i < n; i++) {
		if (marked[i])
			m_marked[m_nodelist.at(i)] = true;
	}

	set_partition();
}
