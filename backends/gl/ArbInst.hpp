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
#ifndef ARBINST_HPP
#define ARBINST_HPP

#include "ShVariable.hpp"

namespace shgl {

/** All the possible operations in the ARB spec.
 */
enum ArbOp {
  // VERTEX AND FRAGMENT
  
  // Vector
  SH_ARB_ABS,
  SH_ARB_FLR,
  SH_ARB_FRC,
  SH_ARB_LIT,
  SH_ARB_MOV,

  // Scalar
  SH_ARB_EX2,
  SH_ARB_LG2,
  SH_ARB_RCP,
  SH_ARB_RSQ,

  // Binary scalar
  SH_ARB_POW,

  // Binary vector
  SH_ARB_ADD,
  SH_ARB_DP3,
  SH_ARB_DP4,
  SH_ARB_DPH,
  SH_ARB_DST,
  SH_ARB_MAX,
  SH_ARB_MIN,
  SH_ARB_MUL,
  SH_ARB_SGE,
  SH_ARB_SLT,
  SH_ARB_SUB,
  SH_ARB_XPD,

  // Trinary
  SH_ARB_MAD,

  // Swizzling
  SH_ARB_SWZ,

  // VERTEX ONLY
  // Scalar
  SH_ARB_EXP,
  SH_ARB_LOG,
  
  // FRAGMENT ONLY
  // Scalar
  SH_ARB_COS,
  SH_ARB_SIN,
  SH_ARB_SCS,

  // Trinary
  SH_ARB_CMP,
  SH_ARB_LRP,

  // Sampling
  SH_ARB_TEX,
  SH_ARB_TXP,
  SH_ARB_TXB,

  // KIL
  SH_ARB_KIL,

  // NV_vertex_program2/NV_fragment_program
  SH_ARB_SEQ,
  SH_ARB_SGT,
  SH_ARB_SLE,
  SH_ARB_SNE,
  SH_ARB_SFL,
  SH_ARB_STR,

  // NV_fragment_program
  SH_ARB_DDX,
  SH_ARB_DDY,
  SH_ARB_RFL,
  SH_ARB_TXD,

  // NV_vertex_program2
  SH_ARB_SSG,
  SH_ARB_BRA,
  SH_ARB_LABEL, // Special label "instruction"
  
  // NV_fragment_program2
  SH_ARB_DIV,
  SH_ARB_DP2,
  SH_ARB_NRM,
  SH_ARB_IF,
  SH_ARB_ELSE,
  SH_ARB_ENDIF,
  SH_ARB_REP,
  SH_ARB_ENDREP,
  SH_ARB_BRK,
  
  // Special "operations" for emit
  SH_ARB_FUN
};

/** Information about the operations from ArbOp.
 */
struct ArbOpInfo {
  char* name;
  int arity;
  bool collectingOp;
};

extern ArbOpInfo arbOpInfo[];

/** An ARB instruction.
 */
struct ArbInst {
  static const int max_num_sources = 4;
  
  ArbInst(ArbOp op, int label)
    : op(op), label(label), invert(false),
      update_cc(false), ccode(NOCC)
  {
  }

  ArbInst(ArbOp op, int label, const SH::ShVariable& condition)
    : op(op), label(label), invert(false),
      update_cc(false), ccode(NOCC)
  {
    src[0] = condition;
  }
  
  ArbInst(ArbOp op, const SH::ShVariable& dest)
    : op(op), dest(dest), invert(false),
      update_cc(false), ccode(NOCC)
  {
  }

  ArbInst(ArbOp op, const SH::ShVariable& dest, const SH::ShVariable& src0)
    : op(op), dest(dest), invert(false),
      update_cc(false), ccode(NOCC)
  {
    src[0] = src0;
  }

  ArbInst(ArbOp op, const SH::ShVariable& dest, const SH::ShVariable& src0,
          const SH::ShVariable& src1)
    : op(op), dest(dest), invert(false),
      update_cc(false), ccode(NOCC)
  {
    src[0] = src0;
    src[1] = src1;
  }
  ArbInst(ArbOp op, const SH::ShVariable& dest, const SH::ShVariable& src0,
          const SH::ShVariable& src1, const SH::ShVariable& src2)
    : op(op), dest(dest), invert(false),
      update_cc(false), ccode(NOCC)
  {
    src[0] = src0;
    src[1] = src1;
    src[2] = src2;
  }
  ArbInst(ArbOp op, const SH::ShVariable& dest, const SH::ShVariable& src0,
          const SH::ShVariable& src1, const SH::ShVariable& src2,
          const SH::ShVariable& src3)
    : op(op), dest(dest), invert(false),
      update_cc(false), ccode(NOCC)
  {
    src[0] = src0;
    src[1] = src1;
    src[2] = src2;
    src[3] = src3;
  }
  
  ArbOp op;
  SH::ShVariable dest;

  SH::ShVariable src[max_num_sources];

  int label; // For branching instructions and labels
  bool invert; // Invert the sense of a break condition.

  // NV specific stuff for condition codes
  bool update_cc; ///< Prepend "C" to update the condition code
  
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
  } ccode; ///< Do a conditional instruction

  SH::ShSwizzle ccswiz;
  
};

extern char* arbCCnames[];

}

#endif
