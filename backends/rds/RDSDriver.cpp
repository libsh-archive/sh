#include <iostream.h>
#include "sh.hpp"
#include "RDS.hpp"
#include "DAG.hpp"

using namespace SH;

ShProgram test_program() {
	ShProgram program;

	// stolen from particle example (in main.cpp)
	program = SH_BEGIN_PROGRAM("gpu:stream") {
		ShInOutPoint3f pos;
		ShInOutVector3f vel;
		ShInputVector3f acc;
		ShInputAttrib1f delta;

		// clamp acceleration to zero if particles at or below ground plane 
		acc = cond(abs(pos(1)) < 0.05, ShVector3f(0.0, 0.0, 0.0), acc);

		// integrate acceleration to get velocity
		vel += acc*delta;

		// clamp velocity to zero if small to force particles to settle
		//vel = cond((vel|vel) < 0.5, ShVector3f(0.0, 0.0, 0.0), vel);

		// integrate velocity to update position
		pos = vel*delta;

		acc = vel*delta;

		vel = acc;
		
		// parameters controlling the amount of momentum
		// tranfer on collision
		/*ShAttrib1f mu(0.3);
		ShAttrib1f eps(0.6);

		// check if below ground level
		ShAttrib1f under = pos(1) < 0.0;

		// clamp to ground level if necessary
		pos = cond(under, pos*ShAttrib3f(1.0, 0.0, 1.0), pos);

		// modify velocity in case of collision
		ShVector3f veln = vel*ShAttrib3f(0.0, 1.0, 0.0);
		ShVector3f velt = vel - veln;
		vel = cond(under, (1.0 - mu)*velt - eps*veln, vel);
		pos(1) = cond(min(under, (vel|vel) < 0.1), ShPoint1f(0.0f), pos(1));

		// clamp to the edge of the plane, just to make it look nice
		pos(0) = min(max(pos(0), -5), 5);
		pos(2) = min(max(pos(2), -5), 5); */
	
	} SH_END;

	return program;
}


int main() {
	RDS rds;
	ShProgram program;

	cout << "RDS Test\n";

	// get an Sh Program (this parses it and stuff)
	program = test_program();

	// extract the cfg and print it
	ShCtrlGraphPtr graph = program.node()->ctrlGraph;
	graph->print(cout, 1);

	//DAG *dag = new DAG(graph->entry()->follower->block);
	//dag->print(0);

	// create an rds object
	 rds = RDS(program.node());
	 rds.get_pdt()->printDoms();
	 //cout << "RDSh...\n";
	 //rds.rdsh();
	//cout << "\nRDS...\n";
	rds.rds();
	rds.print_partition();

	return 0;
}

