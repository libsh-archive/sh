#include "Partition.hpp"

Partition::Partition(DAG::DAG *g) :
	m_graph(g)
{
	m_passes.clear();
	m_shared_vars.clear();
}

void Partition::set_partition(DAGNode::DAGNode *root) {
	root->unvisitall();
	root->mark();
	partition(root);
}

// partitions graph; roots of partitions based on marked nodes
void Partition::partition(DAGNode::DAGNode *v)
{    
	if (!v) return;
	if (v->visited()) return;

#ifdef RDS_TRACE
      SH_DEBUG_PRINT(__FUNCTION__ << " node " << v->m_label );
#endif

	v->visit();
    
    // partition each of v's children
	for (DAGNode::DAGNodeVector::iterator I = v->successors.begin(); I != v->successors.end(); ++I) {
		DAGNode::DAGNode *w = *I;
      
		if (!(w->visited())) {
			partition(w);
		}
      
		// cut w off from v if it's marked
		if (w->marked()) {
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
  if (v->marked()) {
    m_passes.push_back(v);
  }
}

void Partition::print_stmt() {
	SH_DEBUG_PRINT(__FUNCTION__);
	int count = 0;
	for(Partition::PassVector::iterator I = m_passes.begin(); I != m_passes.end(); ++I) {
		std::cout << "Pass " << ++count << "\n";
		(*I)->set_resources();
		(*I)->print_resources();
		(*I)->dump_stmts();
	}
}
