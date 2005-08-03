// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#include "ArbCode.hpp"
#include <algorithm>
#include <cmath>
#include "ShDebug.hpp"
#include "ShError.hpp"
#include "ShAttrib.hpp"
#include "ShTypeInfo.hpp"

#ifdef WIN32
namespace {
double log2(double x) { return log(x)/log(2.0); }
}
#endif

namespace shgl {

using namespace SH;

// Transformations
namespace {
const unsigned int scalarize    = 0x01; // Split into scalar instructions
const unsigned int swap_sources = 0x02; // Swap first and second sources
const unsigned int negate_first = 0x04; // Negate first source
const unsigned int delay_mask   = 0x08; // Do writemasking in separate step
};

struct ArbMapping {
  ShOperation sh_op;
  unsigned int filters;
  
  unsigned int transforms;
  ArbOp arb_op;

  typedef void (ArbCode::*ArbFunction)(const ShStatement&);
  ArbFunction function;
};

ArbMapping ArbCode::table[] = {
  {SH_OP_ASN, SH_ARB_ANY, 0, SH_ARB_MOV, 0},

  // Arithmetic
  {SH_OP_ADD,  SH_ARB_ANY,   0,            SH_ARB_ADD, 0},
  {SH_OP_NEG,  SH_ARB_ANY,   negate_first, SH_ARB_MOV, 0},
  {SH_OP_MUL,  SH_ARB_ANY,   0,            SH_ARB_MUL, 0},

  // Removed this temporarily because of a bug in the NV drivers
  //{SH_OP_DIV,  SH_ARB_NVFP2, scalarize,    SH_ARB_DIV, 0},
  
  {SH_OP_DIV,  SH_ARB_ANY,   0,    SH_ARB_FUN, &ArbCode::emit_div},
  {SH_OP_POW,  SH_ARB_ANY,   scalarize,    SH_ARB_POW, 0},
  {SH_OP_RCP,  SH_ARB_ANY,   scalarize,    SH_ARB_RCP, 0},
  {SH_OP_RSQ,  SH_ARB_ANY,   scalarize,    SH_ARB_RSQ, 0},
  {SH_OP_CBRT, SH_ARB_ANY,   scalarize,    SH_ARB_FUN, &ArbCode::emit_cbrt},
  {SH_OP_SQRT, SH_ARB_ANY,   scalarize,    SH_ARB_FUN, &ArbCode::emit_sqrt},

  {SH_OP_LRP, SH_ARB_FP,  0, SH_ARB_LRP, 0},
  {SH_OP_LRP, SH_ARB_VP,  0, SH_ARB_FUN, &ArbCode::emit_lerp},
  {SH_OP_MAD, SH_ARB_ANY, 0, SH_ARB_MAD, 0},

  // Sum/product of components
  {SH_OP_CMUL, SH_ARB_ANY, 0, SH_ARB_FUN, &ArbCode::emit_cmul},
  {SH_OP_CSUM, SH_ARB_ANY, 0, SH_ARB_FUN, &ArbCode::emit_csum},
  
  // Dot product
  {SH_OP_DOT, SH_ARB_VEC1,                0,         SH_ARB_MUL, 0},
  {SH_OP_DOT, SH_ARB_VEC3,                0,         SH_ARB_DP3, 0},
  {SH_OP_DOT, SH_ARB_VEC4,                0,         SH_ARB_DP4, 0},
  {SH_OP_DOT, SH_ARB_VEC2 | SH_ARB_NVFP2, 0,         SH_ARB_DP2, 0},
  {SH_OP_DOT, SH_ARB_VEC2,                0,         SH_ARB_FUN, &ArbCode::emit_dot2},

  // Boolean
  {SH_OP_SLT, SH_ARB_ANY,   0,            SH_ARB_SLT, 0},
  {SH_OP_SGE, SH_ARB_ANY,   0,            SH_ARB_SGE, 0},

  {SH_OP_SLE, SH_ARB_NVVP2, 0,            SH_ARB_SLE, 0},
  {SH_OP_SLE, SH_ARB_NVFP,  0,            SH_ARB_SLE, 0},
  {SH_OP_SLE, SH_ARB_ANY,   swap_sources, SH_ARB_SGE, 0},

  {SH_OP_SGT, SH_ARB_NVVP2, 0,            SH_ARB_SGT, 0},
  {SH_OP_SGT, SH_ARB_NVFP,  0,            SH_ARB_SGT, 0},
  {SH_OP_SGT, SH_ARB_ANY,   swap_sources, SH_ARB_SLT, 0},

  {SH_OP_SEQ, SH_ARB_NVVP2, 0,            SH_ARB_SEQ, 0},
  {SH_OP_SEQ, SH_ARB_NVFP,  0,            SH_ARB_SEQ, 0},
  {SH_OP_SEQ, SH_ARB_ANY,   0,            SH_ARB_FUN, &ArbCode::emit_eq},

  {SH_OP_SNE, SH_ARB_NVVP2, 0,            SH_ARB_SNE, 0},
  {SH_OP_SNE, SH_ARB_NVFP,  0,            SH_ARB_SNE, 0},
  {SH_OP_SNE, SH_ARB_ANY,   0,            SH_ARB_FUN, &ArbCode::emit_eq},

  // Clamping
  {SH_OP_ABS,  SH_ARB_ANY, 0, SH_ARB_ABS, 0},
  {SH_OP_CEIL, SH_ARB_ANY, 0, SH_ARB_FUN, &ArbCode::emit_ceil},
  {SH_OP_FLR,  SH_ARB_ANY, 0, SH_ARB_FLR, 0},
  {SH_OP_FRAC, SH_ARB_ANY, 0, SH_ARB_FRC, 0},
  {SH_OP_MOD,  SH_ARB_ANY, 0, SH_ARB_FUN, &ArbCode::emit_mod},
  {SH_OP_MAX,  SH_ARB_ANY, 0, SH_ARB_MAX, 0},
  {SH_OP_MIN,  SH_ARB_ANY, 0, SH_ARB_MIN, 0},
  {SH_OP_RND,  SH_ARB_ANY, 0, SH_ARB_FUN, &ArbCode::emit_rnd},
  {SH_OP_SGN,  SH_ARB_NVVP2, 0, SH_ARB_SSG, 0},
  {SH_OP_SGN,  SH_ARB_ANY, 0, SH_ARB_FUN, &ArbCode::emit_sgn},
  
  // Trig
  {SH_OP_ACOS,  SH_ARB_ANY, 0,         SH_ARB_FUN, &ArbCode::emit_invtrig},
  {SH_OP_ASIN,  SH_ARB_ANY, 0,         SH_ARB_FUN, &ArbCode::emit_invtrig},
  //{SH_OP_ATAN,  SH_ARB_ANY, 0,         SH_ARB_FUN, &ArbCode::emit_invtrig},
  {SH_OP_COS,   SH_ARB_FP,  scalarize, SH_ARB_COS, 0},
  {SH_OP_COS,   SH_ARB_VP,  0,         SH_ARB_FUN, &ArbCode::emit_trig},
  {SH_OP_COSH,  SH_ARB_ANY, 0,         SH_ARB_FUN, &ArbCode::emit_hyperbolic},
  {SH_OP_SIN,   SH_ARB_FP,  scalarize, SH_ARB_SIN, 0},
  {SH_OP_SIN,   SH_ARB_VP,  0,         SH_ARB_FUN, &ArbCode::emit_trig},
  {SH_OP_SINH,  SH_ARB_ANY, 0,         SH_ARB_FUN, &ArbCode::emit_hyperbolic},
  {SH_OP_TAN,   SH_ARB_ANY, 0,         SH_ARB_FUN, &ArbCode::emit_tan},
  {SH_OP_TANH,  SH_ARB_ANY, 0,         SH_ARB_FUN, &ArbCode::emit_hyperbolic},

  // Derivatives
  {SH_OP_DX, SH_ARB_NVFP, 0, SH_ARB_DDX, 0},
  {SH_OP_DY, SH_ARB_NVFP, 0, SH_ARB_DDY, 0},

  // Expontential
  {SH_OP_EXP2,  SH_ARB_ANY, scalarize, SH_ARB_EX2, 0},
  {SH_OP_LOG2,  SH_ARB_ANY, scalarize, SH_ARB_LG2, 0},
  {SH_OP_EXP,   SH_ARB_ANY, scalarize, SH_ARB_FUN, &ArbCode::emit_exp},
  {SH_OP_LOG,   SH_ARB_ANY, 0,         SH_ARB_FUN, &ArbCode::emit_log},
  {SH_OP_EXP10, SH_ARB_ANY, scalarize, SH_ARB_FUN, &ArbCode::emit_exp},
  {SH_OP_LOG10, SH_ARB_ANY, 0,         SH_ARB_FUN, &ArbCode::emit_log},

  // Geometric
  {SH_OP_NORM, SH_ARB_NVFP2 | SH_ARB_VEC3, 0, SH_ARB_NRM, 0},
  {SH_OP_NORM, SH_ARB_ANY,                 0, SH_ARB_FUN, &ArbCode::emit_norm},
  {SH_OP_XPD,  SH_ARB_ANY | SH_ARB_VEC3,   0, SH_ARB_XPD, 0},

  // Texture
  {SH_OP_TEX,  SH_ARB_NVVP3, 0, SH_ARB_FUN, &ArbCode::emit_tex},
  {SH_OP_TEX,  SH_ARB_FP,    0, SH_ARB_FUN, &ArbCode::emit_tex},
  {SH_OP_TEXI, SH_ARB_NVVP3, 0, SH_ARB_FUN, &ArbCode::emit_tex},
  {SH_OP_TEXI, SH_ARB_FP,    0, SH_ARB_FUN, &ArbCode::emit_tex},

  {SH_OP_TEXD,  SH_ARB_NVFP,  0, SH_ARB_FUN, &ArbCode::emit_tex},
  
  // Misc.
  {SH_OP_LIT,  SH_ARB_ANY,   0,            SH_ARB_FUN, &ArbCode::emit_lit},
  {SH_OP_COND, SH_ARB_NVFP,  0,            SH_ARB_FUN, &ArbCode::emit_nvcond},
  {SH_OP_COND, SH_ARB_NVVP2, 0,            SH_ARB_FUN, &ArbCode::emit_nvcond},
  {SH_OP_COND, SH_ARB_ANY,   negate_first, SH_ARB_CMP, 0},
  {SH_OP_KIL,  SH_ARB_FP,    0,            SH_ARB_FUN, &ArbCode::emit_kil},
  /* TODO
  {SH_OP_NOISE, SH_ARB_ANY,  0,            SH_ARB_FUN, &ArbCode::emit_noise},
  */

  {SH_OP_PAL,  SH_ARB_VP, 0, SH_ARB_FUN, &ArbCode::emit_pal},

//  {SH_OP_RET,  SH_ARB_NVFP2, 0, SH_ARB_FUN, &ArbCode::emit_ret},
  {SH_OP_RET,  SH_ARB_NVFP2, 0, SH_ARB_RET, 0},

  {SH_OP_COMMENT, SH_ARB_ANY, 0, SH_ARB_FUN, &ArbCode::emit_comment},

  {SH_OP_ASN, SH_ARB_END, 0, SH_ARB_FUN, 0}
};

void ArbCode::emit(const ShStatement& stmt)
{
  int maxlen = 0; // Maximum tuple length over all sources
  for (int i = 0; i < opInfo[stmt.op].arity; i++) {
    if (stmt.src[i].size() > maxlen) maxlen = stmt.src[i].size();
  }

  unsigned int match = m_environment;
  switch(maxlen) {
  case 1: match |= SH_ARB_VEC1; break;
  case 2: match |= SH_ARB_VEC2; break;
  case 3: match |= SH_ARB_VEC3; break;
  case 4: match |= SH_ARB_VEC4; break;
  }
  
  ArbMapping* mapping;

  for (mapping = table; mapping->filters != SH_ARB_END; mapping++) {
    if (mapping->sh_op != stmt.op) continue;
    if ((mapping->filters & match) != mapping->filters) continue;
    break;
  }
  if (mapping->filters == SH_ARB_END) {
    shError(ShException(std::string("ARB Code: Unknown operation ") + opInfo[stmt.op].name));
    return;
  }

  ShStatement actual = stmt;
  
  if (mapping->transforms & swap_sources) {
    ShVariable tmp(actual.src[0]);
    actual.src[0] = actual.src[1];
    actual.src[1] = tmp;
  }
  if (mapping->transforms & negate_first) {
    actual.src[0] = -actual.src[0];
  }
  
  std::list<ShStatement> stmts;
  if ((mapping->transforms & scalarize) && maxlen > 1) {
    for (int i = 0; i < maxlen; i++) {
      ShStatement scalar = actual;
      scalar.dest = scalar.dest(i);
      for (int j = 0; j < opInfo[actual.op].arity; j++) {
        scalar.src[j] = scalar.src[j](std::min(i, scalar.src[j].size() - 1));
      }
      stmts.push_back(scalar);
    }
  } else {
    stmts.push_back(actual);
  }

  if (mapping->transforms & delay_mask) {
    for (std::list<ShStatement>::iterator I = stmts.begin(); I != stmts.end(); ++I) {
      if (I->dest.swizzle().identity()) continue;

      ShVariable realdest(I->dest);
      ShVariable tmp(I->dest.node()->clone(SH_TEMP, SH_ATTRIB, 4));
      I->dest = tmp;

      ShStatement mask(realdest, SH_OP_ASN, tmp);

      std::list<ShStatement>::iterator next = I; ++next;
      I = stmts.insert(next, mask);
    }
  }

  for (std::list<ShStatement>::const_iterator I = stmts.begin(); I != stmts.end(); ++I) {
    if (mapping->arb_op == SH_ARB_FUN) {
      (this->*(mapping->function))(*I);
    } else {
      // HACK for delay_mask to work.
      ArbOp op = (I->op == SH_OP_ASN) ? SH_ARB_MOV : mapping->arb_op;
      switch (opInfo[I->op].arity) {
      case 0:
        m_instructions.push_back(ArbInst(op, I->dest));
        break;
      case 1:
        m_instructions.push_back(ArbInst(op, I->dest,
                                         I->src[0]));
        break;
      case 2:
        m_instructions.push_back(ArbInst(op, I->dest,
                                         I->src[0], I->src[1]));
        break;
      case 3:
        m_instructions.push_back(ArbInst(op, I->dest,
                                         I->src[0], I->src[1], I->src[2]));
        break;
      }
    }
  }
}

void ArbCode::real_div(const ShVariable& dest, const ShVariable& src0,
		       const ShVariable& src1)
{
  // @todo type should handle other types (half-floats, fixed point)

  ShVariable rcp(new ShVariableNode(SH_TEMP, src1.size(), SH_FLOAT));

  for (int i = 0; i < src1.size(); i++) {
    m_instructions.push_back(ArbInst(SH_ARB_RCP, rcp(i), src1(i)));
  }
  m_instructions.push_back(ArbInst(SH_ARB_MUL, dest, src0, rcp));
}

void ArbCode::emit_div(const ShStatement& stmt)
{
  real_div(stmt.dest, stmt.src[0], stmt.src[1]);
}

void ArbCode::emit_sqrt(const ShStatement& stmt)
{
  ShVariable rsq(new ShVariableNode(SH_TEMP, 1, SH_FLOAT));
  m_instructions.push_back(ArbInst(SH_ARB_RSQ, rsq, stmt.src[0]));
  m_instructions.push_back(ArbInst(SH_ARB_RCP, stmt.dest, rsq));
}

void ArbCode::emit_lerp(const ShStatement& stmt)
{
  // lerp(f,a,b)= f*a + (1-f)*b = f*(a-b) + b
  
  ShVariable t(new ShVariableNode(SH_TEMP, stmt.src[1].size(), SH_FLOAT));
  m_instructions.push_back(ArbInst(SH_ARB_ADD, t, stmt.src[1], -stmt.src[2]));
  m_instructions.push_back(ArbInst(SH_ARB_MAD, stmt.dest, stmt.src[0], t, stmt.src[2]));
}

void ArbCode::emit_dot2(const ShStatement& stmt)
{
  ShVariable mul(new ShVariableNode(SH_TEMP, 2, SH_FLOAT));
  m_instructions.push_back(ArbInst(SH_ARB_MUL, mul, stmt.src[0], stmt.src[1]));
  m_instructions.push_back(ArbInst(SH_ARB_ADD, stmt.dest, mul(0), mul(1)));
}

void ArbCode::emit_eq(const ShStatement& stmt)
{
  ShVariable t1(new ShVariableNode(SH_TEMP, stmt.dest.size(), SH_FLOAT));
  ShVariable t2(new ShVariableNode(SH_TEMP, stmt.dest.size(), SH_FLOAT));

  ArbOp op, combine;
  if (stmt.op == SH_OP_SEQ) {
    op = SH_ARB_SGE;
    combine = SH_ARB_MUL;
  } else if (stmt.op == SH_OP_SNE) {
    op = SH_ARB_SLT;
    combine = SH_ARB_MAX;
  } else {
    SH_DEBUG_ASSERT(false);
  }
  
  m_instructions.push_back(ArbInst(op, t1, stmt.src[0], stmt.src[1]));
  m_instructions.push_back(ArbInst(op, t2, stmt.src[1], stmt.src[0]));
  m_instructions.push_back(ArbInst(combine, stmt.dest, t1, t2));
}

void ArbCode::emit_ceil(const ShStatement& stmt)
{
  ShVariable t(new ShVariableNode(SH_TEMP, stmt.dest.size(), SH_FLOAT));

  m_instructions.push_back(ArbInst(SH_ARB_FLR, t, -stmt.src[0])); 
  m_instructions.push_back(ArbInst(SH_ARB_MOV, stmt.dest, -t));
}

void ArbCode::emit_mod(const ShStatement& stmt)
{
  // TODO - is this really optimal?
  ShVariable t1(new ShVariableNode(SH_TEMP, stmt.dest.size(), SH_FLOAT));
  ShVariable t2(new ShVariableNode(SH_TEMP, stmt.dest.size(), SH_FLOAT));
  
  // result = x - y*floor(x/y)
  emit(ShStatement(t1, stmt.src[0], SH_OP_DIV, stmt.src[1]));
  m_instructions.push_back(ArbInst(SH_ARB_FLR, t2, t1));
  m_instructions.push_back(ArbInst(SH_ARB_MUL, t1, stmt.src[1], t2)); 
  m_instructions.push_back(ArbInst(SH_ARB_SUB, stmt.dest, stmt.src[0], t1)); 
}

void ArbCode::emit_trig(const ShStatement& stmt)
{
  // Use float constants.  Conversions will take place as necessary
  ShConstAttrib4f c0(0.0, 0.5, 1.0, 0.0);
  ShConstAttrib4f c1(0.25, -9.0, 0.75, 1.0/(2.0*M_PI));
  ShConstAttrib4f c2(24.9808039603, -24.9808039603, -60.1458091736, 60.1458091736);
  ShConstAttrib4f c3(85.4537887573, -85.4537887573, -64.9393539429, 64.9393539429);
  ShConstAttrib4f c4(19.7392082214, -19.7392082214, -1.0, 1.0);

  m_shader->constants.push_back(c0.node());
  m_shader->constants.push_back(c1.node());
  m_shader->constants.push_back(c2.node());
  m_shader->constants.push_back(c3.node());
  m_shader->constants.push_back(c4.node());
  
  ShVariable r0(new ShVariableNode(SH_TEMP, 4, SH_FLOAT));
  ShVariable r1(new ShVariableNode(SH_TEMP, 4, SH_FLOAT));
  ShVariable r2(new ShVariableNode(SH_TEMP, 4, SH_FLOAT));
  ShVariable rs(new ShVariableNode(SH_TEMP, 4, SH_FLOAT));
  
  if (stmt.op == SH_OP_SIN) {
    m_instructions.push_back(ArbInst(SH_ARB_MAD, rs, c1(3,3,3,3), stmt.src[0], -c1(0,0,0,0)));
  } else if (stmt.op == SH_OP_COS) {
    m_instructions.push_back(ArbInst(SH_ARB_MUL, rs, c1(3,3,3,3), stmt.src[0]));
  } else {
    SH_DEBUG_ASSERT(false);
  }
  m_instructions.push_back(ArbInst(SH_ARB_FRC, rs, rs));
  for (int i = 0; i < stmt.src[0].size(); i++) {
    m_instructions.push_back(ArbInst(SH_ARB_SLT, r2(0), rs(i), c1(0)));
    m_instructions.push_back(ArbInst(SH_ARB_SGE, r2(1,2), rs(i,i), c1(1,2)));
    m_instructions.push_back(ArbInst(SH_ARB_DP3, r2(1), r2(0,1,2), c4(2,3,2)));
    m_instructions.push_back(ArbInst(SH_ARB_ADD, r0(0,1,2), -rs(i,i,i), c0(0,1,2)));
    m_instructions.push_back(ArbInst(SH_ARB_MUL, r0, r0, r0));
    m_instructions.push_back(ArbInst(SH_ARB_MAD, r1, c2(0,1,0,1), r0, c2(2,3,2,3)));
    m_instructions.push_back(ArbInst(SH_ARB_MAD, r1, r1, r0, c3(0,1,0,1)));
    m_instructions.push_back(ArbInst(SH_ARB_MAD, r1, r1, r0, c3(2,3,2,3)));
    m_instructions.push_back(ArbInst(SH_ARB_MAD, r1, r1, r0, c4(0,1,0,1)));
    m_instructions.push_back(ArbInst(SH_ARB_MAD, r1, r1, r0, c4(2,3,2,3)));
    m_instructions.push_back(ArbInst(SH_ARB_DP3, r0(0), r1(0,1,2), -r2(0,1,2)));
    m_instructions.push_back(ArbInst(SH_ARB_MOV, stmt.dest(i), r0(0)));
  }
}

void ArbCode::emit_invtrig(const ShStatement& stmt)
{
  // Use float constants.  Conversions will take place to other types as necessary 
  ShConstAttrib4f c0(0.0, 1.570796327, -0.5860008052, 0.5860008052);
  ShConstAttrib4f c1(1.571945105, -1.571945105, -1.669668977, 1.669668977);
  ShConstAttrib4f c2(0.8999841642, -0.8999841642, -0.6575341673, 0.6575341673);
  ShConstAttrib4f c3(1.012386649, -1.012386649, 0.9998421793, -0.9998421793);
  ShConstAttrib4f c4(1.0, -1.0, 1.0, -1.0);

  m_shader->constants.push_back(c0.node());
  m_shader->constants.push_back(c1.node());
  m_shader->constants.push_back(c2.node());
  m_shader->constants.push_back(c3.node());
  m_shader->constants.push_back(c4.node());
  
  ShVariable r0(new ShVariableNode(SH_TEMP, 4, SH_FLOAT));
  ShVariable r1(new ShVariableNode(SH_TEMP, 4, SH_FLOAT));
  ShVariable r2(new ShVariableNode(SH_TEMP, 4, SH_FLOAT));
  ShVariable offset(new ShVariableNode(SH_TEMP, 4, SH_FLOAT));
  ShVariable output(new ShVariableNode(SH_TEMP, stmt.dest.size(), SH_FLOAT));
  m_instructions.push_back(ArbInst(SH_ARB_ABS, r0, stmt.src[0]));
  m_instructions.push_back(ArbInst(SH_ARB_MAD, offset, -r0, r0, c4(0,0,0,0)));
  
  m_instructions.push_back(ArbInst(SH_ARB_MOV, r2, c0(0,1,0,1)));
  for (int i = 0; i < stmt.src[0].size(); i++) {
    m_instructions.push_back(ArbInst(SH_ARB_SLT, r2(1), stmt.src[0](i), c0(0)));
    m_instructions.push_back(ArbInst(SH_ARB_SGE, r2(0), stmt.src[0](i), c0(0)));
    m_instructions.push_back(ArbInst(SH_ARB_MAD, r1(0,1), c0(2,3), r0(i,i), c1(0,1)));
    m_instructions.push_back(ArbInst(SH_ARB_MAD, r1(0,1), r1(0,1), r0(i,i), c1(2,3)));
    m_instructions.push_back(ArbInst(SH_ARB_MAD, r1(0,1), r1(0,1), r0(i,i), c2(0,1)));
    m_instructions.push_back(ArbInst(SH_ARB_MAD, r1(0,1), r1(0,1), r0(i,i), c2(2,3)));
    m_instructions.push_back(ArbInst(SH_ARB_MAD, r1(0,1), r1(0,1), r0(i,i), c3(0,1)));
    m_instructions.push_back(ArbInst(SH_ARB_MAD, r1(0,1), r1(0,1), r0(i,i), c3(2,3)));
    m_instructions.push_back(ArbInst(SH_ARB_RSQ,offset(i), offset(i)));
    m_instructions.push_back(ArbInst(SH_ARB_RCP,offset(i), offset(i)));
    m_instructions.push_back(ArbInst(SH_ARB_MAD, r1(0,1), c4(1,0), offset(i,i), r1(0,1)));
    if (stmt.op == SH_OP_ACOS) {
      m_instructions.push_back(ArbInst(SH_ARB_DP3, output(i), r1(0,1,2), r2(0,1,2)));
    } else {
      m_instructions.push_back(ArbInst(SH_ARB_DP3, stmt.dest(i), r1(0,1,2), r2(0,1,2)));
    }
  }
  if (stmt.op == SH_OP_ACOS) {
    m_instructions.push_back(ArbInst(SH_ARB_ADD, stmt.dest, -output, c0(1,1,1,1)));
  }
}

void ArbCode::emit_tan(const ShStatement& stmt)
{
  ShVariable tmp1(new ShVariableNode(SH_TEMP, stmt.src[0].size(), SH_FLOAT));
  ShVariable tmp2(new ShVariableNode(SH_TEMP, stmt.src[0].size(), SH_FLOAT));

  emit(ShStatement(tmp1, SH_OP_COS, stmt.src[0]));
  emit(ShStatement(tmp1, SH_OP_RCP, tmp1));
  emit(ShStatement(tmp2, SH_OP_SIN, stmt.src[0]));
  
  m_instructions.push_back(ArbInst(SH_ARB_MUL, stmt.dest, tmp1, tmp2));
}

void ArbCode::emit_exp(const ShStatement& stmt)
{
  float basef = (stmt.op == SH_OP_EXP ? M_E : 10.0f);

  ShConstAttrib1f base(basef);
  m_shader->constants.push_back(base.node());

  m_instructions.push_back(ArbInst(SH_ARB_POW, stmt.dest, base, stmt.src[0]));
}

void ArbCode::emit_hyperbolic(const ShStatement& stmt)
{
  ShConstAttrib1f two(2.0f);
  m_shader->constants.push_back(two.node());

  ShVariable e_plusX(new ShVariableNode(SH_TEMP, stmt.src[0].size(), SH_FLOAT)); // e^x
  ShVariable e_minusX(new ShVariableNode(SH_TEMP, stmt.src[0].size(), SH_FLOAT)); // e^-x
  emit(ShStatement(e_plusX, SH_OP_EXP, stmt.src[0]));
  emit(ShStatement(e_minusX, SH_OP_NEG, stmt.src[0]));
  emit(ShStatement(e_minusX, SH_OP_EXP, e_minusX));

  ShVariable temp_cosh(new ShVariableNode(SH_TEMP, stmt.src[0].size(), SH_FLOAT)); // e^x + e^-x
  ShVariable temp_sinh(new ShVariableNode(SH_TEMP, stmt.src[0].size(), SH_FLOAT)); // e^x - e^-x

  switch (stmt.op) {
  case SH_OP_COSH:
    // cosh x = [e^x + e^-x] / 2
    m_instructions.push_back(ArbInst(SH_ARB_ADD, temp_cosh, e_plusX, e_minusX)); 
    real_div(stmt.dest, temp_cosh, two);
    break;
  case SH_OP_SINH:
    // cosh x = [e^x - e^-x] / 2
    m_instructions.push_back(ArbInst(SH_ARB_SUB, temp_sinh, e_plusX, e_minusX)); 
    real_div(stmt.dest, temp_sinh, two);
    break;
  case SH_OP_TANH:
    // tanh x = sinh x / cosh x = [e^x - e^-x] / [e^x + e^-x]
    m_instructions.push_back(ArbInst(SH_ARB_ADD, temp_cosh, e_plusX, e_minusX)); 
    m_instructions.push_back(ArbInst(SH_ARB_SUB, temp_sinh, e_plusX, e_minusX)); 
    real_div(stmt.dest, temp_sinh, temp_cosh);
    break;
  default:
    SH_DEBUG_ASSERT(0);
  }
}

void ArbCode::emit_log(const ShStatement& stmt)
{
  float scalef = 1.0/log2((stmt.op == SH_OP_LOG ? M_E : 10.0f));

  ShConstAttrib1f scale(scalef);
  m_shader->constants.push_back(scale.node());

  ShVariable tmp(new ShVariableNode(SH_TEMP, stmt.dest.size(), SH_FLOAT));
  
  emit(ShStatement(tmp, SH_OP_LOG2, stmt.src[0]));
  m_instructions.push_back(ArbInst(SH_ARB_MUL, stmt.dest, tmp, scale));
}

void ArbCode::emit_norm(const ShStatement& stmt)
{
  ShVariable tmp(new ShVariableNode(SH_TEMP, 1, SH_FLOAT));
  emit(ShStatement(tmp, stmt.src[0], SH_OP_DOT, stmt.src[0]));
  m_instructions.push_back(ArbInst(SH_ARB_RSQ, tmp, tmp));
  m_instructions.push_back(ArbInst(SH_ARB_MUL, stmt.dest, tmp, stmt.src[0]));
}

void ArbCode::emit_sgn(const ShStatement& stmt)
{
  ShVariable tmp1(new ShVariableNode(SH_TEMP, stmt.src[0].size(), SH_FLOAT));
  ShVariable tmp2(new ShVariableNode(SH_TEMP, stmt.src[0].size(), SH_FLOAT));
  ShConstAttrib1f zero(0.0);
  ShConstAttrib1f minus_one(-1.0);

  emit(ShStatement(tmp1, stmt.src[0], SH_OP_SGE, zero));
  emit(ShStatement(tmp2, stmt.src[0], SH_OP_SGT, zero));
  emit(ShStatement(tmp1, tmp1, SH_OP_ADD, tmp2));
  emit(ShStatement(tmp1, tmp1, SH_OP_ADD, minus_one));

  /*
  emit(ShStatement(tmp, stmt.src[0], SH_OP_SGE, zero));
  emit(ShStatement(stmt.dest, SH_OP_MAD, tmp, ShConstAttrib1f(2.0), ShConstAttrib1f(-1.0)));
  */
}

void ArbCode::emit_tex(const ShStatement& stmt)
{
  bool delay = false;
  ShVariable tmpdest;
  ShVariable tmpsrc;
  
  ShTextureNodePtr tnode = shref_dynamic_cast<ShTextureNode>(stmt.src[0].node());

  if (!stmt.dest.swizzle().identity()) {
    tmpdest = ShVariable(new ShVariableNode(SH_TEMP, tnode->size(), SH_FLOAT));
    tmpsrc = tmpdest;
    delay = true;
  }

  SH_DEBUG_ASSERT(tnode);

  if (tnode->size() == 2) {
    // Special case for LUMINANCE_ALPHA
    tmpdest = ShVariable(new ShVariableNode(SH_TEMP, 4, SH_FLOAT));
    tmpsrc = tmpdest(0,3);
    delay = true;
  }

  if (stmt.op == SH_OP_TEXD) {
    SH_DEBUG_ASSERT(tnode->dims() == SH_TEXTURE_2D);
    m_instructions.push_back(ArbInst(SH_ARB_TXD,
                                     (delay ? tmpdest : stmt.dest), stmt.src[1], stmt.src[0],
                                     stmt.src[2](0,1), stmt.src[2](2,3)));
  } else {
    m_instructions.push_back(ArbInst(SH_ARB_TEX,
                                     (delay ? tmpdest : stmt.dest), stmt.src[1], stmt.src[0]));
  }

  if (delay) emit(ShStatement(stmt.dest, SH_OP_ASN, tmpsrc));
}

void ArbCode::emit_nvcond(const ShStatement& stmt)
{

  ShVariable dummy(new ShVariableNode(SH_TEMP, stmt.src[0].size(), SH_FLOAT));
  ArbInst updatecc(SH_ARB_MOV, dummy, stmt.src[0]);
  updatecc.update_cc = true;
  m_instructions.push_back(updatecc);

  /*
  ShSwizzle ccswiz = stmt.src[0].swizzle();
  if (ccswiz.size() == 1) {
    int indices[4];
    for (int i = 0; i < stmt.dest.size(); i++) {
      indices[i] = 0;
    }
    ccswiz *= ShSwizzle(1, stmt.dest.size(), indices);
  }
  */  
  if (stmt.dest != stmt.src[1]) {
    ArbInst movt(SH_ARB_MOV, stmt.dest, stmt.src[1]);
    movt.ccode = ArbInst::GT;
    movt.ccswiz = stmt.src[0].swizzle();
    m_instructions.push_back(movt);
  }
  if (stmt.dest != stmt.src[2]) {
    ArbInst movf(SH_ARB_MOV, stmt.dest, stmt.src[2]);
    movf.ccode = ArbInst::LE;
    movf.ccswiz = stmt.src[0].swizzle();
    m_instructions.push_back(movf);
  }
}

void ArbCode::emit_csum(const ShStatement& stmt)
{

  // @todo type make this function handle more than floats
  ShVariantPtr c1_values = new ShDataVariant<float, SH_HOST>(stmt.src[0].size(), 1.0f); 
  ShVariable c1(new ShVariableNode(SH_CONST, stmt.src[0].size(), SH_FLOAT));
  c1.setVariant(c1_values);
  m_shader->constants.push_back(c1.node());
  
  emit(ShStatement(stmt.dest, stmt.src[0], SH_OP_DOT, c1));
}

void ArbCode::emit_cmul(const ShStatement& stmt)
{
  // @todo use clone
  ShVariable prod(new ShVariableNode(SH_TEMP, 1, stmt.dest.valueType()));

  // TODO: Could use vector mul here.
  
  m_instructions.push_back(ArbInst(SH_ARB_MOV, prod, stmt.src[0](0)));
  for (int i = 1; i < stmt.src[0].size(); i++) {
    m_instructions.push_back(ArbInst(SH_ARB_MUL, prod, prod, stmt.src[0](i)));
  }
  m_instructions.push_back(ArbInst(SH_ARB_MOV, stmt.dest, prod));
}

void ArbCode::emit_kil(const ShStatement& stmt)
{
  m_instructions.push_back(ArbInst(SH_ARB_KIL, -stmt.src[0]));
}

void ArbCode::emit_ret(const ShStatement& stmt)
{
  ShVariable dummy(new ShVariableNode(SH_TEMP, stmt.src[0].size(), SH_FLOAT));
  ArbInst updatecc(SH_ARB_MOV, dummy, stmt.src[0]);
  updatecc.update_cc = true;
  m_instructions.push_back(updatecc);
  m_instructions.push_back(ArbInst(SH_ARB_RET));
}

void ArbCode::emit_pal(const ShStatement& stmt)
{
  m_instructions.push_back(ArbInst(SH_ARB_ARL, m_address_register, stmt.src[1]));
  m_instructions.push_back(ArbInst(SH_ARB_ARRAYMOV, stmt.dest, stmt.src[0], m_address_register));
}

void ArbCode::emit_lit(const ShStatement& stmt)
{
  ShVariable tmp(new ShVariableNode(SH_TEMP, stmt.dest.size(), SH_FLOAT));
  ArbInst inst(SH_ARB_LIT, tmp, stmt.src[0]);
  m_instructions.push_back(inst);

  emit(ShStatement(stmt.dest, SH_OP_ASN, tmp));
}

void ArbCode::emit_rnd(const ShStatement& stmt)
{
  ShVariable t(new ShVariableNode(SH_TEMP, stmt.dest.size(), SH_FLOAT));
  
  ShVariantPtr c1_values = new ShDataVariant<float, SH_HOST>(stmt.src[0].size(), 0.5f); 
  ShVariable c1(new ShVariableNode(SH_CONST, stmt.src[0].size(), SH_FLOAT));
  c1.setVariant(c1_values);
  m_shader->constants.push_back(c1.node());

  m_instructions.push_back(ArbInst(SH_ARB_ADD, t, stmt.src[0], c1)); 
  m_instructions.push_back(ArbInst(SH_ARB_FLR, stmt.dest, t));
}

void ArbCode::emit_cbrt(const ShStatement& stmt)
{
  ShVariable t(new ShVariableNode(SH_TEMP, 1, SH_FLOAT));
  ShConstAttrib1f c(3.0f);
  m_shader->constants.push_back(c.node());

  m_instructions.push_back(ArbInst(SH_ARB_RCP, t, c)); 
  m_instructions.push_back(ArbInst(SH_ARB_POW, stmt.dest, stmt.src[0], t));
}

void ArbCode::emit_comment(const ShStatement& stmt)
{
  m_instructions.push_back(ArbInst(SH_ARB_COMMENT, stmt.get_info<ShInfoComment>()->comment));
}

}
