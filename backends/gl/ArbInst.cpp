#include "ArbInst.hpp"

namespace shgl {

ArbOpInfo arbOpInfo[] = {
  // VERTEX AND FRAGMENT
  // Vector
  {"ABS", 1, false},
  {"FLR", 1, false},
  {"FRC", 1, false},
  {"LIT", 1, false},
  {"MOV", 1, false},

  // Scalar
  {"EX2", 1, false},
  {"LG2", 1, false},
  {"RCP", 1, false},
  {"RSQ", 1, false},

  // Binary scalar
  {"POW", 2, false},

  // Binary vector
  {"ADD", 2, false},
  {"DP3", 2, true},
  {"DP4", 2, true},
  {"DPH", 2, true},
  {"DST", 2, true},
  {"MAX", 2, false},
  {"MIN", 2, false},
  {"MUL", 2, false},
  {"SGE", 2, false},
  {"SLT", 2, false},
  {"SUB", 2, false},
  {"XPD", 2, true}, // should this really be a collectingOp?

  // Trinary
  {"MAD", 3, false},

  // Swizzling
  {"SWZ", 2, true}, // should this really be a collectingOp?

  // VERTEX ONLY
  // Scalar
  {"EXP", 1, false},
  {"LOG", 1, false},
  
  // FRAGMENT ONLY
  // Scalar
  {"COS", 1, false},
  {"SIN", 1, false},
  {"SCS", 1, false},

  // Trinary
  {"CMP", 3, false},
  {"LRP", 3, false},

  // Sampling
  {"TEX", 3, false},
  {"TXP", 3, false},
  {"TXB", 3, false},

  // KIL
  {"KIL", 0, false},

  // NV_vertex_program/NV_fragment_program
  {"SEQ", 2, false},
  {"SGT", 2, false},
  {"SLE", 2, false},
  {"SNE", 2, false},
  {"SFL", 2, false},
  {"STR", 2, false},

  // NV_fragment_program
  {"DDX", 1, false},
  {"DDY", 1, false},
  {"RFL", 2, false},

  // NV_vertex_program2
  {"SSG", 1, false},
  {"BRA", 2, false},
  {"<label>", 1, false},
  
  // NV_fragment_program2
  {"DIV", 2, false},
  {"DP2", 2, true},
  {"NRM", 1, false},
  {"IF", 1, false},
  {"ELSE", 0, false},
  {"ENDIF", 0, false},
  
  {"<fun>", 0, false}
};

}
