// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
//////////////////////////////////////////////////////////////////////////////
#ifndef ARBINST_HPP
#define ARBINST_HPP

#include "Variable.hpp"

namespace shgl {

/** All the possible operations in the ARB spec.
 */
enum ArbOp {
  // VERTEX AND FRAGMENT
  
  // Vector
  ARB_ABS,
  ARB_FLR,
  ARB_FRC,
  ARB_LIT,
  ARB_MOV,

  // Scalar
  ARB_EX2,
  ARB_LG2,
  ARB_RCP,
  ARB_RSQ,

  // Binary scalar
  ARB_POW,

  // Binary vector
  ARB_ADD,
  ARB_DP3,
  ARB_DP4,
  ARB_DPH,
  ARB_DST,
  ARB_MAX,
  ARB_MIN,
  ARB_MUL,
  ARB_SGE,
  ARB_SLT,
  ARB_SUB,
  ARB_XPD,

  // Trinary
  ARB_MAD,

  // Swizzling
  ARB_SWZ,

  // VERTEX ONLY
  // Scalar
  ARB_EXP,
  ARB_LOG,
  
  // Weird,
  ARB_ARL,
  ARB_ARRAYMOV, // Kind of hacky but works.
  
  // FRAGMENT ONLY
  // Scalar
  ARB_COS,
  ARB_SIN,
  ARB_SCS,

  // Trinary
  ARB_CMP,
  ARB_LRP,

  // Sampling
  ARB_TEX,
  ARB_TXP,
  ARB_TXB,

  // KIL
  ARB_KIL,

  // NV_vertex_program2/NV_fragment_program
  ARB_SEQ,
  ARB_SGT,
  ARB_SLE,
  ARB_SNE,
  ARB_SFL,
  ARB_STR,

  // NV_fragment_program
  ARB_DDX,
  ARB_DDY,
  ARB_RFL,
  ARB_TXD,

  // NV_vertex_program2
  ARB_SSG,
  ARB_BRA,
  ARB_LABEL, // Special label "instruction"
  
  // NV_fragment_program2
  ARB_DIV,
  ARB_DP2,
  ARB_NRM,
  ARB_IF,
  ARB_ELSE,
  ARB_ENDIF,
  ARB_REP,
  ARB_ENDREP,
  ARB_BRK,
  ARB_RET,
  
  // Special "operations" for emit
  ARB_FUN,

  // Just comments
  ARB_COMMENT,
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
  
  explicit ArbInst(ArbOp op)
    : op(op), invert(false), update_cc(false), ccode(NOCC)
  {
  }

  ArbInst(ArbOp op, int label)
    : op(op), label(label), invert(false),
      update_cc(false), ccode(NOCC)
  {
  }

  ArbInst(ArbOp op, const std::string& comment)
    : op(op), invert(false), update_cc(false), ccode(NOCC),
    comment(comment)
  {
  }

  ArbInst(ArbOp op, int label, const SH::Variable& condition)
    : op(op), label(label), invert(false),
      update_cc(false), ccode(NOCC)
  {
    src[0] = condition;
  }
  
  ArbInst(ArbOp op, const SH::Variable& dest)
    : op(op), dest(dest), invert(false),
      update_cc(false), ccode(NOCC)
  {
  }

  ArbInst(ArbOp op, const SH::Variable& dest, const SH::Variable& src0)
    : op(op), dest(dest), invert(false),
      update_cc(false), ccode(NOCC)
  {
    src[0] = src0;
  }

  ArbInst(ArbOp op, const SH::Variable& dest, const SH::Variable& src0,
          const SH::Variable& src1)
    : op(op), dest(dest), invert(false),
      update_cc(false), ccode(NOCC)
  {
    src[0] = src0;
    src[1] = src1;
  }
  ArbInst(ArbOp op, const SH::Variable& dest, const SH::Variable& src0,
          const SH::Variable& src1, const SH::Variable& src2)
    : op(op), dest(dest), invert(false),
      update_cc(false), ccode(NOCC)
  {
    src[0] = src0;
    src[1] = src1;
    src[2] = src2;
  }
  ArbInst(ArbOp op, const SH::Variable& dest, const SH::Variable& src0,
          const SH::Variable& src1, const SH::Variable& src2,
          const SH::Variable& src3)
    : op(op), dest(dest), invert(false),
      update_cc(false), ccode(NOCC)
  {
    src[0] = src0;
    src[1] = src1;
    src[2] = src2;
    src[3] = src3;
  }
  
  ArbOp op;
  SH::Variable dest;

  SH::Variable src[max_num_sources];

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

  SH::Swizzle ccswiz;
  
  // Only used for COMMENTs
  std::string comment;
};

extern char* arbCCnames[];

}

#endif
