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
  {"NOP", 0, false}, // 1.1
  
  // Vector
  {"MOV", 1, false}, // 1.1
  {"NRM", 1, false}, // 2.0
  {"ABS", 1, false}, // 2.0
  {"EXP", 1, false}, // vs 1.1, ps 2.0
  {"FRC", 1, false}, // vs 1.1, ps 2.0
  {"LOG", 1, false}, // vs 1.1, ps 2.0
  {"RCP", 1, false}, // vs 1.1, ps 2.0
  {"RSQ", 1, false}, // vs 1.1, ps 2.0
  
  // Register ops
  /*SH_PS_DCL_USAGE, // vs 1.1, ps 3.0
  SH_PS_DEF, // 1.1
  SH_PS_DEFB, // vs 2.0, ps 3.0
  SH_PS_DEFI, // vs 2.0, ps 3.0
  SH_PS_DCLTEXTURETYPE, // ps 2.0, vs 3.0*/

  // Binary vector
  {"ADD", 2, false}, // 1.1
  {"DP3", 2, true}, // 1.1
  {"DP4", 2, true}, // 1.1
  {"MUL", 2, false}, // 1.1
  {"SUB", 2, false}, // 1.1
  {"CRS", 2, true}, // 2.0
  //SH_PS_TEXLDL, // 3.0
  
  // Vector/matrix
  /*SH_PS_M3X2, // vs 1.1, ps 2.0
  SH_PS_M3X3, // vs 1.1, ps 2.0
  SH_PS_M3X4, // vs 1.1, ps 2.0
  SH_PS_M4X3, // vs 1.1, ps 2.0
  SH_PS_M4X4, // vs 1.1, ps 2.0*/

  // Vector/scalar
  {"POW", 2, false}, // 2.0

  // Ternary
  {"MAD", 3, false}, // 1.1
  //SH_PS_SINCOS, // 2.0

  // 4-ary
  {"LRP", 3, false}, // ps 1.1, vs 2.0

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
  //SH_PS_VS, // vs 1.1

  // Vector
  {"LOGP", 1, false}, // vs 1.1
  {"EXPP", 1, false}, // vs 1.1
  {"LIT", 1, false}, // vs 1.1
  {"MOVA", 1, false}, // vs 2.0

  // Binary vector
  {"DST", 2, true}, // vs 1.1
  {"MAX", 2, false}, // vs 1.1
  {"MIN", 2, false}, // vs 1.1
  {"SGE", 2, false}, // vs 1.1
  {"SLT", 2, false}, // vs 1.1

  // Ternary vector
  //SH_PS_SGN, // vs 2.0


  // FRAGMENT ONLY
  //SH_PS_PS, // ps 1.1
  //SH_PS_PHASE, // ps 1.4 ONLY

  // Vector
  //SH_PS_DSX, // ps 2_x
  //SH_PS_DSY, // ps 2_x

  // Binary vector
  ("BEM", 2, true}, // ps 1.1

  // Ternary vector
  {"CMP", 3, false}, // ps 1.1
  {"CND", 3, false}, // ps 1.1
  {"DP2ADD", 3, false}, // ps 2.0

  // Declarations
  //SH_PS_DCL, // ps 2.0

  // Texturing
  /*SH_PS_TEX, // ps 1.1-1.3
  SH_PS_TEXBEM, // ps 1.1-1.3
  SH_PS_TEXBEML, // ps 1.1-1.3
  SH_PS_TEXCOORD, // ps 1.1-1.3
  SH_PS_TEXCRD, // ps 1.4
  SH_PS_TEXDEPTH, // ps 1.4
  SH_PS_TEXDP3, // ps 1.2-1.3
  SH_PS_TEXDP3TEX, // ps 1.2-1.3
  SH_PS_TEXKILL, // ps 1.1
  SH_PS_TEXLD, // ps 1.4
  SH_PS_TEXM3X2DEPTH, // ps 1.3 ONLY
  SH_PS_TEXM3X2PAD, // ps 1.1-1.3
  SH_PS_TEXM3X2TEX, // ps 1.1-1.3
  SH_PS_TEXM3X3, // ps 1.2-1.3
  SH_PS_TEXM3X3PAD, // ps 1.1-1.3
  SH_PS_TEXM3X3SPEC, // ps 1.1-1.3
  SH_PS_TEXM3X3TEX, // ps 1.1-1.3
  SH_PS_TEXM3X3VSPEC, // ps 1.1-1.3
  SH_PS_TEXREG2AR, // ps 1.1-1.3
  SH_PS_TEXREG2GB, // ps 1.1-1.3
  SH_PS_TEXREG2RGB, // ps 1.2-1.3
  SH_PS_TEXLD_2, // ps 2.0
  SH_PS_TEXLDB, // ps 2.0
  SH_PS_TEXLDP, // ps 2.0
  SH_PS_TEXLDD, // ps 2_x*/
  
  // Special "operations" for emit
  {"<fun>", 0, false}
};

}
