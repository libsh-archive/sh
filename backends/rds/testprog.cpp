#include <iostream>
#include "sh.hpp"

using namespace SH;
using namespace std;

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

		// integrate velocity to update position
		pos = vel*delta;
		acc = vel*delta;		

		// check if below ground level
		ShAttrib1f under = pos(1) < 0.0;

		// clamp to ground level if necessary
		pos = cond(under, pos*ShAttrib3f(1.0, 0.0, 1.0), pos);
	
	} SH_END;

	return program;
}