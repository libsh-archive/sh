#include "RDS.hpp"
#include <iostream>
#include <fstream>
#include "ShDebug.hpp"
#include "ShUtility.hpp"
#include "NextKSubset.hpp"

#define RDS_DEBUG 
//#define RDS_TRACE
using namespace SH;

RDS::RDS(ShProgramNodePtr progPtr)
	: m_graph(new DAG(progPtr->ctrlGraph->entry()->follower->block))	
{
	m_callback = new CallBack();
	m_pdt = new PDomTree(m_graph);
	m_part = new Partition(m_graph);
}

void RDS::print_partitions(char *filename) {
  std::ofstream dump(filename);
  dump << "digraph g {" << std::endl;
    for(Partition::PassVector::iterator I = passes().begin(); I != passes().end(); ++I) {
#ifdef RDS_DEBUG
      SH_DEBUG_PRINT( "Pass starting at " << (*I)->m_label );
#endif
//      m_pdt->graphvizDump(*I, dump);
    }
  dump << "}" << std::endl;
  dump.close();
}

void RDS::print_partition() {
	int count = 0;
	for(Partition::PassVector::iterator I = passes().begin(); I != passes().end(); ++I) {
		std::cout << "Pass " << ++count << "\n";
		m_pdt->printGraph(*I, 2);
	}
}

void RDS::force_fake_limits() {
  m_callback->set_fake_limits();
}

void RDS::force_graph_limits() {
  m_callback->set_fake_limits();
}

void RDS::rds_search() {
#ifdef RDS_DEBUG
  SH_DEBUG_PRINT(__FUNCTION__);
#endif

	DAGNode::DAGNode *p = m_pdt->get_root();
	DAGNode::DAGNode *d = m_graph->m_root;

	// this sets m_mrlist
	d->unvisitall();
	d->unfixall();
	add_mr(d);
  
	for (MRList::iterator I = m_mrlist.begin(); I != m_mrlist.end(); ++I) {
		DAGNode::DAGNode *m = *I;
		
		// find cost of saving subregion m
		d->unmarkall();
		d->unvisitall();
		m->fix(RDS_MARKED);
		m_callback->reset();
		rds_subdivide(p);
		d->unvisitall();
		d->mark();
		float cost_s = m_callback->cost(d);

		// find cost of recomputing subregion m
		d->unmarkall();
		d->unvisitall();
		m->fix(RDS_UNMARKED);
		m_callback->reset();
		rds_subdivide(p);
		d->unvisitall();
		d->mark();
		float cost_r = m_callback->cost(d); 
#ifdef RDS_DEBUG
    SH_DEBUG_PRINT( "Cost Save = " << cost_s << " Cost Recompute = " << cost_r );
#endif
		d->unvisitall();
		if (cost_s < cost_r || !m_callback->valid_partition(p)) {
			m->fix(RDS_MARKED);
		}
		d->unvisitall();
	}

	d->unmarkall();
	d->unvisitall();
	m_callback->reset();
	rds_subdivide(p);
#ifdef RDS_DEBUG
	d->unvisitall();
	d->mark();
	SH_DEBUG_PRINT("Cost Final = " << m_callback->cost(d));
#endif
	d->unvisitall();
}

// partitions m_graph by marking nodes to indicate pass boundaries
// returns a set of passes
void RDS::rds_subdivide(DAGNode::DAGNode* v) {

#ifdef RDS_TRACE
	SH_DEBUG_PRINT( "Subdivide(" << m_pdt->numbering(v) << ")");
#endif

    // stop if subregion v can be mapped in one pass
	if (m_callback->valid(v)) return;
        
	// list of pdt children of v (postorder)
	PDomTree::PChildVector& kids = m_pdt->pchildren[v];
        
	for (PDomTree::PChildVector::iterator I = kids.begin(); I != kids.end(); ++I) {
		DAGNode::DAGNode *k = *I;
		rds_subdivide(k);
        
		if (m_pdt->mr(k)) { 
			if(m_rdsh) {
				if (m_callback->recompute(k)) {
					// recompute k
					k->unmark();
				}
				else {
					// save k
					k->mark();
				}
			}
			else {
				switch(k->fixed()) {
					case RDS_MARKED:
						k->mark();  
						break;
					case RDS_UNMARKED:
						k->unmark();
						break;
					default:
						if (m_callback->recompute(k)) {
							// recompute 
							k->unmark();
						}
						else {
							// save w
							k->mark();
						}
						break;
				}
			}
        }
    }

  // apply greedy merging
  rds_merge(v);
}

// merge kids at indices given in array a with node v
// all unmarked_kids are automatically merged
DAGNode::DAGNode *RDS::make_merge(DAGNode::DAGNode* v, int *a, int d, DAGNode::DAGNodeVector kids, 
								DAGNode::DAGNodeVector unmarked_kids)
{
	DAGNode::DAGNode *w;

	// create temporary parent node
	if (v->predecessors.size() == 0) {
		w = new DAGNode();
	}
	else {
		w = new DAGNode(v->m_stmt);
	}

	// attach children in subset
	if (m_rdsh) {
		for (int j = 0; j < d; j++)
			w->successors.push_back(kids.at(a[j]));
	}
	else {
		for (int j = 0; j < d; j++) {
			w->add_kid(kids.at(a[j]));
		}

		for (DAGNode::DAGNodeVector::iterator I = unmarked_kids.begin(); I != unmarked_kids.end(); ++I) {
			w->add_kid(*I);
		}	
	}
	return w;
}

void RDS::rds_merge(DAGNode::DAGNode* v) {
#ifdef RDS_TRACE
  SH_DEBUG_PRINT("Merge(" << m_pdt->numbering(v) << ")");
#endif

	// cannot merge if no kids
	if (v->successors.size() == 0) return;

	// vectors used to check for any fixed nodes (their marked property cannot be changed)
	DAGNode::DAGNodeVector kids;
	DAGNode::DAGNodeVector unmarked_kids;

	DAGNode::DAGNode *z;

	// figure out what this particular node consumes
	if (v->predecessors.size() > 0) {
		z = new DAGNode(v->m_stmt);
		z->set_resources_stmt();
	}
	else {
		z = new DAGNode();
	}

	// fixed-as-marked nodes not considered kids for subsets
	if (!m_rdsh) {
		for (DAGNode::DAGNodeVector::iterator I = v->successors.begin(); I != v->successors.end(); ++I) {
			if ((*I)->fixed() == RDS_UNMARKED) {
				unmarked_kids.push_back(*I);

				// forcefully ensure that this kid can be merged with v (apply merge with reduced limits)
				m_callback->increase_used(z);

				if (!m_callback->valid(*I))
					rds_merge(*I);
				
				m_callback->decrease_used(z);
			}
			else {
				if (!m_callback->valid(*I)) 
					rds_merge(*I);

				if ((*I)->fixed() != RDS_MARKED) {
					kids.push_back(*I);
				}
			}
		}
	}
	else {
		// ensure all subregions are valid before merging
		for (DAGNode::DAGNodeVector::iterator I = v->successors.begin(); I != v->successors.end(); ++I) {
			if (!m_callback->valid(*I)) 
				rds_merge(*I);
		}

		kids = v->successors;
	}

	// get the number of kids of v
	int k = kids.size();

	for (int d = k; d > 0; d--) {
		// get each subset of v's kids with d kids
		// try to merge each subset with v
		Partition::PassVector subsets;
		NextKSubset ksub;
		int *a;

		ksub.init(k, d);
		a = ksub.next();

		// join v with subset of successors
		DAGNode::DAGNode *w = make_merge(v, a, d, kids, unmarked_kids);

		if (m_callback->valid(w)) {
			//w->print_resources();
			//w->dump_stmts();
			subsets.push_back(w);
		}

		// do all the other subsets
		while(ksub.more() && d!=0) {			
			a = ksub.next();

			w = make_merge(v, a, d, kids, unmarked_kids);

			if (m_callback->valid(w)) {
				//w->print_resources();
				//w->dump_stmts();
				subsets.push_back(w);
			}
		}

		// if a subset can be merged, select and stop
		if (subsets.size() > 0) {
			DAGNode::DAGNode *w = *(subsets.begin());
			// if more than one subset, use MERGE to pick one and stop
			if (subsets.size() > 1) {
				w = m_callback->merge(subsets);
			}

			// mark all the kids of w
			for (DAGNode::DAGNodeVector::iterator I = kids.begin(); I != kids.end(); ++I) {
				(*I)->mark();
			}

			// unmark all the kids in the subset
			for (DAGNode::DAGNodeVector::iterator I = w->successors.begin(); I != w->successors.end(); ++I) {
				(*I)->unmark();
			}

			// we're done
			return;
		}
	}

	// nothing could be merged; mark all the kids
	for (DAGNode::DAGNodeVector::iterator I = kids.begin(); I != kids.end(); ++I) {
		(*I)->mark();
	}
}
  
// creates a post-order list of MR nodes, storing in m_mrlist
void RDS::add_mr(DAGNode::DAGNode* v) {
	if (!v) return;
 
	if (v->visited()) return; // already visited
		
	v->visit();
    
	// visit v's children first
	for (DAGNode::DAGNodeVector::iterator I = v->successors.begin(); I != v->successors.end(); ++I) {
			add_mr(*I);
	}
    
	// add v to list if it's multi-referenced
	if (m_pdt->mr(v)) {
			m_mrlist.push_back(v);
	}
}
