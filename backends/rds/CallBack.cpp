#include "CallBack.hpp"

CallBack::CallBack() {
	// set cost params
	c_pass = 15.7;
	c_tex = 1.36;
	c_instr = 1.0;

	#ifdef RDS_FAKE_LIMITS
		m_limits = new rds::FakeLimits();
	#else
		m_limits = new rds::ArbLimits("vertex");
	#endif
	
	#ifdef RDS_DEBUG
		m_limits->dump_limits();
	#endif

	reset();	
}

void CallBack::reset() {
	init_used();
}

// true if pass starting at v can execute in one pass
bool CallBack::valid(DAGNode::DAGNode* v) {
  v->set_resources();

  return	v->halftemps() <= (m_limits->halftemps() - m_halftemps_used) &&
			v->temps() <= (m_limits->temps() - m_temps_used) &&
			v->attribs() <= (m_limits->attribs() - m_attribs_used) &&
			v->params() <= (m_limits->params()  - m_params_used) &&
			v->texs() <= (m_limits->texs() - m_texs_used) && 
			v->instrs() <= (m_limits->instrs() - m_ops_used);
}

// true if pass starting at v consumes less than 1/2 the max resources
bool CallBack::recompute(DAGNode::DAGNode* v) {
	v->set_resources();

	return  v->halftemps() <= m_limits->halftemps()/2 &&
			v->temps() <= m_limits->temps()/2 &&
			v->attribs() <= m_limits->attribs()/2 &&
			v->params() <= m_limits->params()/2 &&
			v->texs() <= m_limits->texs()/2 &&
			v->instrs() <= m_limits->instrs()/2; 
}

// chooses pass that uses fewest instructions
DAGNode::DAGNode* CallBack::merge(PassVector passes) {
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

float CallBack::cost(DAGNode::DAGNode* v) {
	m_num_passes = 0;
	m_num_texs = 0;
	m_num_instrs = 0;
	set_cost_vars(v);
	return c_pass*v->num_passes() + c_tex*v->texs() + c_instr*v->instrs();
}

void CallBack::init_used() {
	m_ops_used = 0;
	m_halftemps_used = 0;
	m_temps_used = 0;
	m_params_used = 0;
	m_attribs_used = 0;
	m_texs_used = 0;
}

void CallBack::increase_used(DAGNode::DAGNode *v) {
	m_ops_used += v->instrs();
	m_ops_used += v->attribs();
	m_halftemps_used += v->halftemps();
	m_temps_used += v->temps();
	m_params_used += v->params();
	m_attribs_used += v->attribs();
	m_texs_used += v->texs();
}

void CallBack::decrease_used(DAGNode::DAGNode *v) {
	m_ops_used -= v->instrs();
	m_ops_used -= v->attribs();
	m_halftemps_used -= v->halftemps();
	m_temps_used -= v->temps();
	m_params_used -= v->params();
	m_attribs_used -= v->attribs();
	m_texs_used -= v->texs();
}

void CallBack::set_cost_vars(DAGNode::DAGNode *v) {
	if (v->visited()) return;
	v->visit();

	// add to cumulative values for this pass
	if (v->marked()) {
		v->set_resources();
		m_num_passes++;
		m_num_texs += v->texs();
		m_num_instrs += v->instrs();
	}
	
	for (DAGNode::DAGNodeVector::iterator I = v->successors.begin(); I != v->successors.end(); ++I) {
		set_cost_vars(*I);
	}
}

bool CallBack::valid_partition(DAGNode::DAGNode *v) {
	v->unvisitall();
	bool result = all_passes_valid(v);
	v->unvisitall();
	return result;
}

bool CallBack::all_passes_valid(DAGNode::DAGNode* v) {
	if (v->visited()) return true;
	v->visit();

	if ((v->marked() || v->fixed() == RDS_MARKED) && !valid(v)) return false;
	
	for (DAGNode::DAGNodeVector::iterator I = v->successors.begin(); I != v->successors.end(); ++I) {
		if (!all_passes_valid(*I))
			return false;
	}

	return true;
}

void CallBack::set_fake_limits() {
	m_limits = new rds::FakeLimits();
}