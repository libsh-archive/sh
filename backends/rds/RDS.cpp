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

using namespace SH;

RDS::RDS(ShProgramNodePtr progPtr)
	: m_graph(new DAG(progPtr->ctrlGraph->entry()->follower->block))	
{
	// get partial dom tree	
	m_pdt = new PDomTree(m_graph);
}

void RDS::set_limits() {
	max_ops = 4;
}


// the rds algorithm, as seen in stanford paper
void RDS::get_partitions() {
//#ifdef SH_RDSH
    rds_subdivide(m_pdt->get_root());
/*#else
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
#endif */
}

void RDS::debugDump()
{
#ifdef SH_DEBUG
      SH_DEBUG_PRINT("Debug Dump");
      //DebugDumper d( *this);
      //preorder(d);
      SH_DEBUG_PRINT("Debug Dump End");
#endif
}

// partitions m_graph by marking nodes to indicate pass boundaries
// returns a set of passes
void RDS::rds_subdivide(DAGNode::DAGNode* v) {
	if (!v) return;
    
	// stop if subregion v can be mapped in one pass
	if (valid(v)) return;
        
	// list of pdt children of v (postorder)
	PDomTree::PChildVector& kids = m_pdt->pchildren[v];
        
	for (PDomTree::PChildVector::iterator I = kids.begin(); I != kids.end(); ++I) {
		DAGNode::DAGNode *w = *I;
		rds_subdivide(w);
            
		// NOTE: not sure if we should use their mark() and clearMarked() in ShCtrlGraph, or define our own
        
		if (m_pdt->mr(w)) { 
        //#ifdef SH_RDSH
			if (recompute(w)) {
				// recompute w
//				w->clearMarked();
			}
			else {
				// save w
//				w->mark();
			}
        /*#else
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
        #endif*/
        }
    }

	// apply greedy merging
    rds_merge(v);
}

void RDS::rds_merge(DAGNode::DAGNode* v)
{
	// get the number of kids of v
	int k = v->successors.size();
       
	for (int d = k; d >= 0; d--) {
		// get each subset of v's kids with d kids

		// try to merge all subsets with v

		// if only one can be merged, select and stop

		// otherwise, use MERGE to pick one
	}
}
  
// creates a post-order list of MR nodes, storing in m_mrlist
void RDS::add_mr(DAGNode::DAGNode* v) {
	if (!v) return;
    
	#ifdef SH_RDSH
		// do nothing
	#else
		if (m_visit[v]) return; // already visited
		
		m_visit[v] = true;
    
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
  

bool RDS::valid(DAGNode::DAGNode* v) {
	return true;
}

bool RDS::recompute(DAGNode::DAGNode* v) {
	return true;
}

int RDS::merge(PassVector passes) {
	return 0;
}

int cost(DAGNode::DAGNode* v) {
	return 0;
}
