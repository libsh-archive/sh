// RDS v 1.0
//
//  Last Revised: Nov. 28 by Cynthia
//
//  A somewhat skeletal version of the RDS pseudocode
//////////////////////////////////////////////////////////////////////////////

#include "RDS.hpp"
#include <iostream>
#include <iostream.h>
#include "ShDebug.hpp"
#include "ShUtility.hpp"

#define SH_RDSH

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

void RDS::print_partitions() {
	int count = 0;
	for(PassVector::iterator I = m_passes.begin(); I != m_passes.end(); ++I) {
		cout << "Pass " << ++count << "\n";
		m_pdt->printGraph(*I, 2);
	}
}

// the rds algorithm, as seen in stanford paper
void RDS::get_partitions() {
#ifdef SH_RDSH
	DAGNode::DAGNode *root = m_pdt->get_root();
    rds_subdivide(root);
#else
    // rds search algorithm
    ShCtrlGraphNodePtr d = m_graph->entry();
      
    // this sets m_mrlist
    add_mr(d);
      
    for (MRList::iterator I = m_mrlist.begin(); I != m_mrlist.end(); ++I) {
		ShCtrlGraphNodePtr m = *I;
      
		// find cost of saving subregion m      
		d->clearMarked();
		m_fixed[m] = RDS_MARKED;
		rds_subdivide(m_pdt->get_root());
		int cost_s = cost(d); // this line will likely be changed
		  
		// find cost of recomputing subregion m
		d->clearMarked();
		m_fixed[m] = RDS_UNMARKED;
		rds_subdivide(m_pdt->get_root());
		int cost_r = cost(d); // this will also likely be changed 
      
		if (cost_s < cost_r) {
			m_fixed[m] = RDS_MARKED;
		}
    }
#endif

	unvisitall(root);
	m_marked[root] = true;
	partition(root);
}

// partitions m_graph by marking nodes to indicate pass boundaries
// returns a set of passes
void RDS::rds_subdivide(DAGNode::DAGNode* v) {
	cout << "Subdivide(" << m_pdt->numbering(v) << ")\n";
    // stop if subregion v can be mapped in one pass
	if (valid(v)) return;
        
	// list of pdt children of v (postorder)
	PDomTree::PChildVector& kids = m_pdt->pchildren[v];
        
	for (PDomTree::PChildVector::iterator I = kids.begin(); I != kids.end(); ++I) {
		DAGNode::DAGNode *k = *I;
		rds_subdivide(k);
        
		if (m_pdt->mr(k)) { 
        #ifdef SH_RDSH
			if (recompute(k)) {
				// recompute k
				m_marked[k] = false;
			}
			else {
				// save k
				m_marked[k] = true;
			}
        #else
          switch(m_fixed[w]) {
            case RDS_MARKED:
              w->mark();  
              break;
            case RDS_UNMARKED:
              w->clearMarked();
              break;
            default:
              if (recompute(w)) {
                // recompute w
                w->clearMarked();
              }
              else {
                // save w
                w->mark();
              }
              break;
          }
        #endif
        }
    }

	// apply greedy merging
    rds_merge(v);
}

void RDS::rds_merge(DAGNode::DAGNode* v)
{
	cout << "Merge(" << m_pdt->numbering(v) << ")\n";
	
	// get the number of kids of v
	int k = v->successors.size();
       
	for (int d = k; d >= 0; d--) {
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
		DAGNode::DAGNode *w = new DAGNode(v->m_label);
		for (int j = 0; j < d; j++) {
			w->successors.push_back(v->successors.at(a[j]));
		}

		if (valid(w)) {
			subsets.push_back(w);
		}

		while(ksub_mtc) {
			a = next_ksubset(k, d, a);
			// join v with subset of successors
			DAGNode::DAGNode *w = new DAGNode(v->m_label);
			for (int j = 0; j < d; j++) {
				w->successors.push_back(v->successors.at(a[j]));
			}

			if (valid(w)) {
				subsets.push_back(w);
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
			for (DAGNode::DAGNodeVector::iterator I = v->successors.begin(); I != v->successors.end(); ++I) {
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
}
  
// creates a post-order list of MR nodes, storing in m_mrlist
void RDS::add_mr(DAGNode::DAGNode* v) {
	if (!v) return;
    
	#ifdef SH_RDSH
		// do nothing
	#else
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
	#endif
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
		if (!m_marked[*I])
			count += countnodes(*I);
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

// partitions graph; roots of partitions based on marked nodes
void RDS::partition(DAGNode::DAGNode *v)
{    
    if (!v) return;
	if (m_visited[v]) return;

	cout << "Parition(" << m_pdt->numbering(v) << ")\n";
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

int RDS::cost(DAGNode::DAGNode* v) {
	return 0;
}
