// RDS v 1.0
//
//  Last Revised: Nov. 28 by Cynthia
//
//  A somewhat skeletal version of the RDS pseudocode
//////////////////////////////////////////////////////////////////////////////

#include "RDS.hpp"
#include <iostream>
#include <fstream>
#include "ShDebug.hpp"
#include "ShUtility.hpp"


#define RDS_DEBUG 
//#define RDS_TRACE
using namespace SH;

RDS::RDS(ShProgramNodePtr progPtr)
	: m_graph(new DAG(progPtr->ctrlGraph->entry()->follower->block))	
{
	// set cost params
	c_pass = 15.7;
	c_tex = 1.36;
	c_instr = 1.0;
	
	// get partial dom tree	
	m_pdt = new PDomTree(m_graph);
#ifdef RDS_FAKE_LIMITS
	m_limits = new rds::FakeLimits();
#else
  m_limits = new rds::ArbLimits("vertex");
#endif
#ifdef RDS_DEBUG
  m_limits->dump_limits();
#endif
}

void RDS::print_partitions(char *filename) {
  std::ofstream dump(filename);
  dump << "digraph g {" << std::endl;
    for(PassVector::iterator I = m_passes.begin(); I != m_passes.end(); ++I) {
#ifdef RDS_DEBUG
      SH_DEBUG_PRINT( "Pass starting at " << (*I)->m_label );
#endif
//      m_pdt->graphvizDump(*I, dump);
    }
  dump << "}" << std::endl;
  dump.close();
}

void RDS::force_fake_limits() {
  m_limits = new rds::FakeLimits();
}

void RDS::force_graph_limits() {
  m_limits = new rds::FakeLimits();
}

void RDS::print_partition() {
	int count = 0;
	for(PassVector::iterator I = m_passes.begin(); I != m_passes.end(); ++I) {
		std::cout << "Pass " << ++count << "\n";
		(*I)->set_resources();
		(*I)->print_resources();
		m_pdt->printGraph(*I, 2);
	}
}

void RDS::print_partition_stmt() {
	SH_DEBUG_PRINT(__FUNCTION__);
	int count = 0;
	for(PassVector::iterator I = m_passes.begin(); I != m_passes.end(); ++I) {
		std::cout << "Pass " << ++count << "\n";
		(*I)->dump_stmts();
	}
}

void RDS::set_partition() {
	DAGNode::DAGNode *root = m_pdt->get_root();
	unvisitall(root);
	root->m_marked = true;
	partition(root);
}

void RDS::init_used() {
	m_ops_used = 0;
	m_halftemps_used = 0;
	m_temps_used = 0;
	m_params_used = 0;
	m_attribs_used = 0;
	m_texs_used = 0;
}

void RDS::rds_search() {
#ifdef RDS_DEBUG
  SH_DEBUG_PRINT(__FUNCTION__);
#endif

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
		unvisitall(d);
		m->fix(RDS_MARKED);
		init_used();
		rds_subdivide(p);
		unvisitall(d);
		d->m_marked = true;
		float cost_s = cost(d);

		// find cost of recomputing subregion m
		unmarkall(d);
		unvisitall(d);
		m->fix(RDS_UNMARKED);
		init_used();
		rds_subdivide(p);
		unvisitall(d);
		d->m_marked = true;
		float cost_r = cost(d); 
#ifdef RDS_DEBUG
    SH_DEBUG_PRINT( "Cost Save = " << cost_s << " Cost Recompute = " << cost_r );
#endif
		unvisitall(d);
		if (cost_s < cost_r || !all_passes_valid(p)) {
			m->fix(RDS_MARKED);
		}
		unvisitall(d);
	}

	unmarkall(d);
	unvisitall(d);
	init_used();
	rds_subdivide(p);
#ifdef RDS_DEBUG
	unvisitall(d);
	d->m_marked = true;
	SH_DEBUG_PRINT("Cost Final = " << cost(d));
#endif
	unvisitall(d);
}

bool RDS::all_passes_valid(DAGNode::DAGNode* v) {
	if (v->m_visited) return true;
	v->m_visited = true;

	if ((v->m_marked || v->fixed() == RDS_MARKED) && !valid(v)) return false;
	
	for (DAGNode::DAGNodeVector::iterator I = v->successors.begin(); I != v->successors.end(); ++I) {
		if (!all_passes_valid(*I))
			return false;
	}

	return true;
}

// partitions m_graph by marking nodes to indicate pass boundaries
// returns a set of passes
void RDS::rds_subdivide(DAGNode::DAGNode* v) {

#ifdef RDS_TRACE
	SH_DEBUG_PRINT( "Subdivide(" << m_pdt->numbering(v) << ")");
#endif

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
					k->m_marked = false;
				}
				else {
					// save k
					k->m_marked = true;
				}
			}
			else {
				switch(k->fixed()) {
					case RDS_MARKED:
						k->m_marked = true;;  
						break;
					case RDS_UNMARKED:
						k->m_marked = false;;
						break;
					default:
						if (recompute(k)) {
							// recompute 
							k->m_marked = false;
						}
						else {
							// save w
							k->m_marked = true;
						}
						break;
				}
			}
        }
    }

  // apply greedy merging
  rds_merge(v);
}

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

void RDS::set_cost_vars(DAGNode::DAGNode *v) {
	if (v->m_visited) return;
	v->m_visited = true;

	// add to cumulative values for this pass
	if (v->m_marked) {
		v->set_resources();
		m_num_passes++;
		m_num_texs += v->texs();
		m_num_instrs += v->instrs();
	}
	
	for (DAGNode::DAGNodeVector::iterator I = v->successors.begin(); I != v->successors.end(); ++I) {
		set_cost_vars(*I);
	}
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
				m_ops_used += z->instrs();
				m_ops_used += z->attribs();
				m_halftemps_used += z->halftemps();
				m_temps_used += z->temps();
				m_params_used += z->params();
				m_attribs_used += z->attribs();
				m_texs_used += z->texs();

				if (!valid(*I))
					rds_merge(*I);
				
				m_ops_used -= z->instrs();
				m_ops_used -= z->attribs();
				m_halftemps_used -= z->halftemps();
				m_temps_used -= z->temps();
				m_params_used -= z->params();
				m_attribs_used -= z->attribs();
				m_texs_used -= z->texs();
			}
			else {
				if (!valid(*I)) 
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
			(*I)->m_marked = true;
			if (!valid(*I)) 
				rds_merge(*I);
			(*I)->m_marked = false;
		}

		kids = v->successors;
	}

	// get the number of kids of v
	int k = kids.size();

	for (int d = k; d > 0; d--) {
		// get each subset of v's kids with d kids
		// try to merge each subset with v
		PassVector subsets;
		int *a;

		// ksubset init stuff
		a = (int *) malloc(sizeof(int) * (k + 1));

		for (int j = 0; j <= d; j++) {
			a[j] = j;
		}

		ksub_mtc = false;
		a = next_ksubset(k, d, a);

		// join v with subset of successors
		DAGNode::DAGNode *w = make_merge(v, a, d, kids, unmarked_kids);

		if (valid(w)) {
			//w->print_resources();
			//w->dump_stmts();
			subsets.push_back(w);
		}

		// do all the other subsets
		while(ksub_mtc && d!=0) {			
			a = next_ksubset(k, d, a);

			w = make_merge(v, a, d, kids, unmarked_kids);

			if (valid(w)) {
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
				w = merge(subsets);
			}

			// mark all the kids of w
			for (DAGNode::DAGNodeVector::iterator I = kids.begin(); I != kids.end(); ++I) {
				(*I)->m_marked = true;
			}

			// unmark all the kids in the subset
			for (DAGNode::DAGNodeVector::iterator I = w->successors.begin(); I != w->successors.end(); ++I) {
				(*I)->m_marked = false;
			}

			// we're done
			return;
		}
	}

	// nothing could be merged; mark all the kids
	for (DAGNode::DAGNodeVector::iterator I = kids.begin(); I != kids.end(); ++I) {
		(*I)->m_marked = true;
	}
}
  
// creates a post-order list of MR nodes, storing in m_mrlist
void RDS::add_mr(DAGNode::DAGNode* v) {
	if (!v) return;
 
	if (v->m_visited) return; // already visited
		
	v->m_visited = true;
    
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
  
void RDS::unvisitall(DAGNode::DAGNode *v) {
	v->m_visited = false;
	for (DAGNode::DAGNodeVector::iterator I = v->successors.begin(); I != v->successors.end(); ++I) {
		unvisitall(*I);
	}
}

void RDS::unfixall(DAGNode::DAGNode *v) {
	v->fix(RDS_UNFIXED);
	for (DAGNode::DAGNodeVector::iterator I = v->successors.begin(); I != v->successors.end(); ++I) {
		unfixall(*I);
	}
}

void RDS::unmarkall(DAGNode::DAGNode *v) {
	v->m_marked = false;
	for (DAGNode::DAGNodeVector::iterator I = v->successors.begin(); I != v->successors.end(); ++I) {
		unmarkall(*I);
	}
}

// partitions graph; roots of partitions based on marked nodes
void RDS::partition(DAGNode::DAGNode *v)
{    
	if (!v) return;
	if (v->m_visited) return;

#ifdef RDS_TRACE
      SH_DEBUG_PRINT(__FUNCTION__ << " node " << v->m_label );
#endif

	v->m_visited = true;
    
    // partition each of v's children
	for (DAGNode::DAGNodeVector::iterator I = v->successors.begin(); I != v->successors.end(); ++I) {
		DAGNode::DAGNode *w = *I;
      
		if (!(w->m_visited)) {
			partition(w);
		}
      
		// cut w off from v if it's marked
		if (w->m_marked) {
			if (w->m_stmt != NULL) {
				SH::ShVariable *var = m_graph->find_var( &(w->m_stmt->dest) );
				m_shared_vars.push_back(var);
			}

			v->m_cut[w] = true; 
			
#ifdef RDS_TRACE
	  SH_DEBUG_PRINT("Cut: " << w->m_label );
	  if (w->m_stmt != NULL)
		SH_DEBUG_PRINT("Shared variable: " << m_graph->find_var( &(w->m_stmt->dest) )->name());
#endif
		}
    }
   
    // check if v is a partition root
  if (v->m_marked) {
    m_passes.push_back(v);
  }
}

// true if pass starting at v can execute in one pass
bool RDS::valid(DAGNode::DAGNode* v) {
  v->set_resources();
  //SH_DEBUG_PRINT(__FUNCTION__);
  //v->print_resources();

  /*SH_DEBUG_PRINT("halftemps_used " << m_halftemps_used);		
  SH_DEBUG_PRINT("temps_used " << m_temps_used);
  SH_DEBUG_PRINT("attribs_used " << m_attribs_used);
  SH_DEBUG_PRINT("params_used " << m_params_used);
  SH_DEBUG_PRINT("texs_used " << m_texs_used);
  SH_DEBUG_PRINT("instrs_used " << m_ops_used);*/

  if (m_rdsh) {
    return  v->halftemps() <= m_limits->halftemps() &&
			v->temps() <= m_limits->temps() &&
			v->attribs() <= m_limits->attribs() &&
			v->params() <= m_limits->params() &&
			v->texs() <= m_limits->texs() &&			
			v->instrs() <= m_limits->instrs();
  }

 return		v->halftemps() <= (m_limits->halftemps() - m_halftemps_used) &&
			v->temps() <= (m_limits->temps() - m_temps_used) &&
			v->attribs() <= (m_limits->attribs() - m_attribs_used) &&
			v->params() <= (m_limits->params()  - m_params_used) &&
			v->texs() <= (m_limits->texs() - m_texs_used) && 
			v->instrs() <= (m_limits->instrs() - m_ops_used);
}

// true if pass starting at v consumes less than 1/2 the max resources
bool RDS::recompute(DAGNode::DAGNode* v) {
	v->set_resources();

	return  v->halftemps() <= m_limits->halftemps()/2 &&
			v->temps() <= m_limits->temps()/2 &&
			v->attribs() <= m_limits->attribs()/2 &&
			v->params() <= m_limits->params()/2 &&
			v->texs() <= m_limits->texs()/2 &&
			v->instrs() <= (m_limits->instrs() / 2); 
}

// chooses pass that uses fewest instructions
DAGNode::DAGNode* RDS::merge(PassVector passes) {
	PassVector::iterator I = passes.begin();
	(*I)->set_resources();
	int lowest = (*I)->instrs();
	DAGNode::DAGNode *winner = *I;

	for(++I; I != passes.end(); ++I) {
		(*I)->set_resources();
		int count = (*I)->instrs();
		if (lowest < 0 || count < lowest) {
			lowest = count;
			winner = *I;
		}
	}
	return winner;
}

float RDS::cost(DAGNode::DAGNode* v) {
	m_num_passes = 0;
	m_num_texs = 0;
	m_num_instrs = 0;
	set_cost_vars(v);
	return c_pass*v->num_passes() + c_tex*v->texs() + c_instr*v->instrs();
}

void RDS::set_nodelist(DAGNode::DAGNode *v) {
	if (v->m_visited) return;

	v->m_visited = true;
	m_nodelist.push_back(v);

	for (DAGNode::DAGNodeVector::iterator I = v->successors.begin(); I != v->successors.end(); ++I) {
		set_nodelist(*I);
	}
}

// brute force. code will be cleaned up later.
void RDS::full_search() {
	DAGNode::DAGNode *root = m_graph->m_root;
	bool found = false;
	float low_cost = 0.0;

	// get a list of all the nodes in the graph
	for (DAGNode::DAGNodeVector::iterator I = root->successors.begin(); I != root->successors.end(); ++I) {
		set_nodelist(*I);
	}	

	unfixall(root);

	// case zero
	int n = m_nodelist.size();
	bool marked[n];
	
	for (int j = 0; j < n; j++) {
		marked[j] = false;
	}

	unmarkall(root);
	root->m_marked = true;

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
			m_nodelist.at(a[j])->m_marked = true;
		}

		root->m_marked = true;

		bool isvalid = true;

		if (!valid(root)) 
			isvalid = false;

		for (int j = 0; j < k && isvalid; j++) {
			if (!valid(m_nodelist.at(a[j])))
				isvalid = false;
		}

		if (isvalid) {
			float this_cost = cost(root);

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
				m_nodelist.at(a[j])->m_marked = true;
			}

			root->m_marked = true;

			bool isvalid = true;

			if (!valid(root)) 
				isvalid = false;

			for (int j = 0; j < k && isvalid; j++) {
					if (!valid(m_nodelist.at(a[j])))
						isvalid = false;
			}

			if (isvalid) {
				float this_cost = cost(root);
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
			m_nodelist.at(i)->m_marked = true;
	}
	root->m_marked = true;
	set_partition();
}
