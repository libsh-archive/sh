// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////
#include "PSInst.hpp"

namespace shdx {

PSOpInfo psOpInfo[] = {
  // VERTEX AND FRAGMENT
  
  // Other
  {"nop", 0, false}, // 1.1
  
  // Vector
  {"mov", 1, false}, // 1.1
  {"nrm", 1, false}, // 2.0
  {"abs", 1, false}, // 2.0
  {"exp", 1, false}, // vs 1.1, ps 2.0
  {"frc", 1, false}, // vs 1.1, ps 2.0
  {"log", 1, false}, // vs 1.1, ps 2.0
  {"rcp", 1, false}, // vs 1.1, ps 2.0
  {"rsq", 1, false}, // vs 1.1, ps 2.0
  
  // Binary vector
  {"add", 2, false}, // 1.1
  {"dp3", 2, true}, // 1.1
  {"dp4", 2, true}, // 1.1
  {"mul", 2, false}, // 1.1
  {"sub", 2, false}, // 1.1
  {"crs", 2, true}, // 2.0
  //SH_PS_TEXLDL, // 3.0
  
  // Vector/matrix
  /*SH_PS_M3X2, // vs 1.1, ps 2.0
  SH_PS_M3X3, // vs 1.1, ps 2.0
  SH_PS_M3X4, // vs 1.1, ps 2.0
  SH_PS_M4X3, // vs 1.1, ps 2.0
  SH_PS_M4X4, // vs 1.1, ps 2.0*/

  // Vector/scalar
  {"pow", 2, false}, // 2.0

  // Ternary
  {"mad", 3, false}, // 1.1
  //SH_PS_SINCOS, // 2.0

  // 4-ary
  {"lrp", 3, false}, // ps 1.1, vs 2.0

  // Branching & flow control
  /*SH_PS_BREAK, // 2_x
  SH_PS_BREAK_COMP, // 2_x
  SH_PS_BREAK_PRED, // 2_x
  SH_PS_CALL, // vs 2.0, ps 2_x
  SH_PS_CALLNZ, // vs 2.0, ps 2_x
  SH_PS_CALLNZ_PRED, // 2_x
  SH_PS_IF_COMP, // 2_x
  SH_PS_IF_PRED, // 2_x
  SH_PS_SETP, // 2_x
  SH_PS_ELSE, // vs 2.0, ps 2_x
  SH_PS_ENDIF, // vs 2.0, ps 2_x
  SH_PS_ENDREP, // vs 2.0, ps 2_x
  SH_PS_IF, // vs 2.0, ps 2_x
  SH_PS_LABEL, // vs 2.0, ps 2_x
  SH_PS_REP, // vs 2.0, ps 2_x
  SH_PS_RET, // vs 2.0, ps 2_x
  SH_PS_LOOP, // vs 2.0, ps 3.0
  SH_PS_ENDLOOP, // vs 2.0, ps 3.0*/


  // VERTEX ONLY

  // Vector
  {"logp", 1, false}, // vs 1.1
  {"expp", 1, false}, // vs 1.1
  {"lit", 1, false}, // vs 1.1
  {"mova", 1, false}, // vs 2.0

  // Binary vector
  {"dst", 2, true}, // vs 1.1
  {"max", 2, false}, // vs 1.1
  {"min", 2, false}, // vs 1.1
  {"sge", 2, false}, // vs 1.1
  {"slt", 2, false}, // vs 1.1

  // Ternary vector
  //SH_PS_SGN, // vs 2.0


  // FRAGMENT ONLY

  // Vector
  {"dsx", 1, false}, // ps 2_x
  {"dsy", 1, false}, // ps 2_x

  // Binary vector
  {"bem", 2, true}, // ps 1.1

  // Ternary vector
  {"cmp", 3, false}, // ps 1.1
  {"cnd", 3, false}, // ps 1.1
  {"dp2add", 3, false}, // ps 2.0

  // Texturing
  /*SH_PS_TEXKILL, // ps 1.1
  SH_PS_TEXLD, // ps 2.0
  SH_PS_TEXLDB, // ps 2.0
  SH_PS_TEXLDP, // ps 2.0
  SH_PS_TEXLDD, // ps 2_x*/
  
  // Special "operations" for emit
  {"<fun>", 0, false}
};

}
