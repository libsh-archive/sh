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
#ifndef PSINST_HPP
#define PSINST_HPP

#include "ShVariable.hpp"

namespace shdx {

/** All the possible operations in the PS spec.
 */
enum PSOp {
  // VERTEX AND FRAGMENT

  // Other
  SH_PS_NOP, // 1.1
  
  // Vector
  SH_PS_MOV, // 1.1
  SH_PS_NRM, // 2.0
  SH_PS_ABS, // 2.0
  SH_PS_EXP, // vs 1.1, ps 2.0
  SH_PS_FRC, // vs 1.1, ps 2.0
  SH_PS_LOG, // vs 1.1, ps 2.0
  SH_PS_RCP, // vs 1.1, ps 2.0
  SH_PS_RSQ, // vs 1.1, ps 2.0
  
  // Register ops
  /*SH_PS_DCL_USAGE, // vs 1.1, ps 3.0
  SH_PS_DEF, // 1.1
  SH_PS_DEFB, // vs 2.0, ps 3.0
  SH_PS_DEFI, // vs 2.0, ps 3.0
  SH_PS_DCLTEXTURETYPE, // ps 2.0, vs 3.0*/

  // Binary vector
  SH_PS_ADD, // 1.1
  SH_PS_DP3, // 1.1
  SH_PS_DP4, // 1.1
  SH_PS_MUL, // 1.1
  SH_PS_SUB, // 1.1
  SH_PS_CRS, // 2.0
  //SH_PS_TEXLDL, // 3.0
  
  // Vector/matrix
  /*SH_PS_M3X2, // vs 1.1, ps 2.0
  SH_PS_M3X3, // vs 1.1, ps 2.0
  SH_PS_M3X4, // vs 1.1, ps 2.0
  SH_PS_M4X3, // vs 1.1, ps 2.0
  SH_PS_M4X4, // vs 1.1, ps 2.0*/

  // Vector/scalar
  SH_PS_POW, // 2.0

  // Ternary
  SH_PS_MAD, // 1.1
  //SH_PS_SINCOS, // 2.0

  // 4-ary
  SH_PS_LRP, // ps 1.1, vs 2.0

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
  SH_PS_LOGP, // vs 1.1
  SH_PS_EXPP, // vs 1.1
  SH_PS_LIT, // vs 1.1
  SH_PS_MOVA, // vs 2.0

  // Binary vector
  SH_PS_DST, // vs 1.1
  SH_PS_MAX, // vs 1.1
  SH_PS_MIN, // vs 1.1
  SH_PS_SGE, // vs 1.1
  SH_PS_SLT, // vs 1.1

  // Ternary vector
  //SH_PS_SGN, // vs 2.0


  // FRAGMENT ONLY
  //SH_PS_PS, // ps 1.1
  //SH_PS_PHASE, // ps 1.4 ONLY

  // Vector
  SH_PS_DSX, // ps 2_x
  SH_PS_DSY, // ps 2_x

  // Binary vector
  SH_PS_BEM, // ps 1.1

  // Ternary vector
  SH_PS_CMP, // ps 1.1
  SH_PS_CND, // ps 1.1
  SH_PS_DP2ADD, // ps 2.0

  // Declarations
  //SH_PS_DCL, // ps 2.0

  // Texturing
  SH_PS_TEX, // ps 1.1-1.3
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
  SH_PS_TEXLDD, // ps 2_x
  
  // Special "operations" for emit
  SH_PS_FUN
};

/** Information about the operations from PSOp.
 */
struct PSOpInfo {
  char* name;
  int arity;
  bool collectingOp;
};

extern PSOpInfo psOpInfo[];

/** A PS/VS instruction.
 */
struct PSInst {
  PSInst(PSOp op, int label)
    : op(op), label(label), invert(false)
      //update_cc(false), ccode(NOCC)
  {
  }

  PSInst(PSOp op, int label, const SH::ShVariable& condition)
    : op(op), label(label), invert(false)
      //update_cc(false), ccode(NOCC)
  {
    src[0] = condition;
  }
  
  PSInst(PSOp op, const SH::ShVariable& dest)
    : op(op), dest(dest), invert(false)
      //update_cc(false), ccode(NOCC)
  {
  }

  PSInst(PSOp op, const SH::ShVariable& dest, const SH::ShVariable& src0)
    : op(op), dest(dest), invert(false)
      //update_cc(false), ccode(NOCC)
  {
    src[0] = src0;
  }

  PSInst(PSOp op, const SH::ShVariable& dest, const SH::ShVariable& src0,
          const SH::ShVariable& src1)
    : op(op), dest(dest), invert(false)
      //update_cc(false), ccode(NOCC)
  {
    src[0] = src0;
    src[1] = src1;
  }

  PSInst(PSOp op, const SH::ShVariable& dest, const SH::ShVariable& src0,
          const SH::ShVariable& src1, const SH::ShVariable& src2)
    : op(op), dest(dest), invert(false)
      //update_cc(false), ccode(NOCC)
  {
    src[0] = src0;
    src[1] = src1;
    src[2] = src2;
  }
  
  PSInst(PSOp op, const SH::ShVariable& dest, const SH::ShVariable& src0,
          const SH::ShVariable& src1, const SH::ShVariable& src2,
		  const SH::ShVariable& src3)
    : op(op), dest(dest), invert(false)
      //update_cc(false), ccode(NOCC)
  {
    src[0] = src0;
    src[1] = src1;
    src[2] = src2;
	src[3] = src3;
  }

  PSOp op;
  SH::ShVariable dest;
  SH::ShVariable src[4];

  int label; // For branching instructions and labels
  bool invert; // Invert the sense of a break condition.

  // NV specific stuff for condition codes
  /*bool update_cc; ///< Prepend "C" to update the condition code
  
  enum CCode {
    NOCC,
    EQ,
    GE,
    GT,
    LE,
    LT,
    NE,
    TR,
    FL
  } ccode; ///< Do a conditional instruction*/

  SH::ShSwizzle ccswiz;
  
};

extern char* psCCnames[];

}

#endif
