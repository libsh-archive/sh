#include "ArbInst.hpp"

namespace shgl {

ArbOpInfo arbOpInfo[] = {
  // VERTEX AND FRAGMENT
  // Vector
  {"ABS", true, true, 1, true, false},
  {"FLR", true, true, 1, true, false},
  {"FRC", true, true, 1, true, false},
  {"LIT", true, true, 1, true, false},
  {"MOV", true, true, 1, true, false},

  // Scalar
  {"EX2", true, true, 1, false, false},
  {"LG2", true, true, 1, false, false},
  {"RCP", true, true, 1, false, false},
  {"RSQ", true, true, 1, false, false},

  // Binary scalar
  {"POW", true, true, 2, false, false},

  // Binary vector
  {"ADD", true, true, 2, true, false},
  {"DP3", true, true, 2, true, true},
  {"DP4", true, true, 2, true, true},
  {"DPH", true, true, 2, true, true},
  {"DST", true, true, 2, true, true},
  {"MAX", true, true, 2, true, false},
  {"MIN", true, true, 2, true, false},
  {"MUL", true, true, 2, true, false},
  {"SGE", true, true, 2, true, false},
  {"SLT", true, true, 2, true, false},
  {"SUB", true, true, 2, true, false},
  {"XPD", true, true, 2, true, true},

  // Trinary
  {"MAD", true, true, 3, true, false},

  // Swizzling
  {"SWZ", true, true, 2, true, true}, // should this really be a collectingOp?

  // VERTEX ONLY
  // Scalar
  {"EXP", true, false, 1, false, false},
  {"LOG", true, false, 1, false, false},
  
  // FRAGMENT ONLY
  // Scalar
  {"COS", false, true, 1, false, false},
  {"SIN", false, true, 1, false, false},
  {"SCS", false, true, 1, false, false},

  // Trinary
  {"CMP", false, true, 3, true, false},
  {"LRP", false, true, 3, true, false},

  // Sampling
  {"TEX", false, true, 3, true, false},
  {"TXP", false, true, 3, true, false},
  {"TXB", false, true, 3, true, false},

  // KIL
  {"KIL", false, true, 0, false, false}
};

}
