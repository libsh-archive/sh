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
#include "ArbCode.hpp"
#include <algorithm>
#include <cmath>
#include "Debug.hpp"
#include "Error.hpp"
#include "Attrib.hpp"
#include "TypeInfo.hpp"

#if defined (_WIN32) || defined (__CYGWIN__)
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

class ArbMapping {
public:
  Operation sh_op;
  unsigned int filters;
  
  unsigned int transforms;
  ArbOp arb_op;

  typedef void (ArbCode::*ArbFunction)(const Statement&);
  ArbFunction function;
};

ArbMapping ArbCode::table[] = {
  {OP_ASN, ARB_ANY, 0, ARB_MOV, 0},

  // Arithmetic
  {OP_ADD,  ARB_ANY,   0,            ARB_ADD, 0},
  {OP_NEG,  ARB_ANY,   negate_first, ARB_MOV, 0},
  {OP_MUL,  ARB_ANY,   0,            ARB_MUL, 0},

  // Removed this temporarily because of a bug in the NV drivers
  //{OP_DIV,  ARB_NVFP2, scalarize,    ARB_DIV, 0},
  
  {OP_DIV,  ARB_ANY,   0,    ARB_FUN, &ArbCode::emit_div},
  {OP_POW,  ARB_ANY,   scalarize,    ARB_POW, 0},
  {OP_RCP,  ARB_ANY,   scalarize,    ARB_RCP, 0},
  {OP_RSQ,  ARB_ANY,   scalarize,    ARB_RSQ, 0},
  {OP_CBRT, ARB_ANY,   scalarize,    ARB_FUN, &ArbCode::emit_cbrt},
  {OP_SQRT, ARB_ANY,   scalarize,    ARB_FUN, &ArbCode::emit_sqrt},

  {OP_LRP, ARB_FP,  0, ARB_LRP, 0},
  {OP_LRP, ARB_VP,  0, ARB_FUN, &ArbCode::emit_lerp},
  {OP_MAD, ARB_ANY, 0, ARB_MAD, 0},

  // Sum/product of components
  {OP_CMUL, ARB_ANY, 0, ARB_FUN, &ArbCode::emit_cmul},
  {OP_CSUM, ARB_ANY, 0, ARB_FUN, &ArbCode::emit_csum},
  
  // Dot product
  {OP_DOT, ARB_VEC1,                0,         ARB_MUL, 0},
  {OP_DOT, ARB_VEC3,                0,         ARB_DP3, 0},
  {OP_DOT, ARB_VEC4,                0,         ARB_DP4, 0},
  {OP_DOT, ARB_VEC2 | ARB_NVFP2, 0,         ARB_DP2, 0},
  {OP_DOT, ARB_VEC2,                0,         ARB_FUN, &ArbCode::emit_dot2},

  // Boolean
  {OP_SLT, ARB_ANY,   0,            ARB_SLT, 0},
  {OP_SGE, ARB_ANY,   0,            ARB_SGE, 0},

  {OP_SLE, ARB_NVVP2, 0,            ARB_SLE, 0},
  {OP_SLE, ARB_NVFP,  0,            ARB_SLE, 0},
  {OP_SLE, ARB_ANY,   swap_sources, ARB_SGE, 0},

  {OP_SGT, ARB_NVVP2, 0,            ARB_SGT, 0},
  {OP_SGT, ARB_NVFP,  0,            ARB_SGT, 0},
  {OP_SGT, ARB_ANY,   swap_sources, ARB_SLT, 0},

  {OP_SEQ, ARB_NVVP2, 0,            ARB_SEQ, 0},
  {OP_SEQ, ARB_NVFP,  0,            ARB_SEQ, 0},
  {OP_SEQ, ARB_ANY,   0,            ARB_FUN, &ArbCode::emit_eq},

  {OP_SNE, ARB_NVVP2, 0,            ARB_SNE, 0},
  {OP_SNE, ARB_NVFP,  0,            ARB_SNE, 0},
  {OP_SNE, ARB_ANY,   0,            ARB_FUN, &ArbCode::emit_eq},

  // Clamping
  {OP_ABS,  ARB_ANY, 0, ARB_ABS, 0},
  {OP_CEIL, ARB_ANY, 0, ARB_FUN, &ArbCode::emit_ceil},
  {OP_FLR,  ARB_ANY, 0, ARB_FLR, 0},
  {OP_FRAC, ARB_ANY, 0, ARB_FRC, 0},
  {OP_MOD,  ARB_ANY, 0, ARB_FUN, &ArbCode::emit_mod},
  {OP_MAX,  ARB_ANY, 0, ARB_MAX, 0},
  {OP_MIN,  ARB_ANY, 0, ARB_MIN, 0},
  {OP_RND,  ARB_ANY, 0, ARB_FUN, &ArbCode::emit_rnd},
  {OP_SGN,  ARB_NVVP2, 0, ARB_SSG, 0},
  {OP_SGN,  ARB_ANY, 0, ARB_FUN, &ArbCode::emit_sgn},
  
  // Trig
  {OP_ACOS,  ARB_ANY, 0,         ARB_FUN, &ArbCode::emit_invtrig},
  {OP_ASIN,  ARB_ANY, 0,         ARB_FUN, &ArbCode::emit_invtrig},
  {OP_ATAN,  ARB_ANY, 0,         ARB_FUN, &ArbCode::emit_atan},
  {OP_COS,   ARB_FP,  scalarize, ARB_COS, 0},
  {OP_COS,   ARB_VP,  0,         ARB_FUN, &ArbCode::emit_trig},
  {OP_COSH,  ARB_ANY, 0,         ARB_FUN, &ArbCode::emit_hyperbolic},
  {OP_SIN,   ARB_FP,  scalarize, ARB_SIN, 0},
  {OP_SIN,   ARB_VP,  0,         ARB_FUN, &ArbCode::emit_trig},
  {OP_SINH,  ARB_ANY, 0,         ARB_FUN, &ArbCode::emit_hyperbolic},
  {OP_TAN,   ARB_ANY, 0,         ARB_FUN, &ArbCode::emit_tan},
  {OP_TANH,  ARB_ANY, 0,         ARB_FUN, &ArbCode::emit_hyperbolic},

  // Derivatives
  {OP_DX, ARB_NVFP, 0, ARB_DDX, 0},
  {OP_DY, ARB_NVFP, 0, ARB_DDY, 0},

  // Expontential
  {OP_EXP2,  ARB_ANY, scalarize, ARB_EX2, 0},
  {OP_LOG2,  ARB_ANY, scalarize, ARB_LG2, 0},
  {OP_EXP,   ARB_ANY, scalarize, ARB_FUN, &ArbCode::emit_exp},
  {OP_LOG,   ARB_ANY, 0,         ARB_FUN, &ArbCode::emit_log},
  {OP_EXP10, ARB_ANY, scalarize, ARB_FUN, &ArbCode::emit_exp},
  {OP_LOG10, ARB_ANY, 0,         ARB_FUN, &ArbCode::emit_log},

  // Geometric
  {OP_NORM, ARB_NVFP2 | ARB_VEC3, 0, ARB_NRM, 0},
  {OP_NORM, ARB_ANY,                 0, ARB_FUN, &ArbCode::emit_norm},
  {OP_XPD,  ARB_ANY | ARB_VEC3,   0, ARB_XPD, 0},

  // Texture
  {OP_TEX,  ARB_NVVP3, 0, ARB_FUN, &ArbCode::emit_tex},
  {OP_TEX,  ARB_FP,    0, ARB_FUN, &ArbCode::emit_tex},
  {OP_TEXI, ARB_NVVP3, 0, ARB_FUN, &ArbCode::emit_tex},
  {OP_TEXI, ARB_FP,    0, ARB_FUN, &ArbCode::emit_tex},

  {OP_TEXD,  ARB_NVFP,  0, ARB_FUN, &ArbCode::emit_tex},
  
  // Misc.
  {OP_LIT,  ARB_ANY,   0,            ARB_FUN, &ArbCode::emit_lit},
  {OP_COND, ARB_NVFP,  0,            ARB_FUN, &ArbCode::emit_nvcond},
  {OP_COND, ARB_NVVP2, 0,            ARB_FUN, &ArbCode::emit_nvcond},
  {OP_COND, ARB_ANY,   negate_first, ARB_CMP, 0},
  {OP_KIL,  ARB_FP,    0,            ARB_FUN, &ArbCode::emit_kil},
  /* TODO
  {OP_NOISE, ARB_ANY,  0,            ARB_FUN, &ArbCode::emit_noise},
  */

  {OP_PAL,  ARB_VP, 0, ARB_FUN, &ArbCode::emit_pal},

//  {OP_RET,  ARB_NVFP2, 0, ARB_FUN, &ArbCode::emit_ret},
  {OP_RET,  ARB_NVFP2, 0, ARB_RET, 0},

  {OP_COMMENT, ARB_ANY, 0, ARB_FUN, &ArbCode::emit_comment},

  {OP_ASN, ARB_END, 0, ARB_FUN, 0}
};

void ArbCode::emit(const Statement& stmt)
{
  int maxlen = 0; // Maximum tuple length over all sources
  for (int i = 0; i < opInfo[stmt.op].arity; i++) {
    if (stmt.src[i].size() > maxlen) maxlen = stmt.src[i].size();
  }

  unsigned int match = m_environment;
  switch(maxlen) {
  case 1: match |= ARB_VEC1; break;
  case 2: match |= ARB_VEC2; break;
  case 3: match |= ARB_VEC3; break;
  case 4: match |= ARB_VEC4; break;
  }
  
  ArbMapping* mapping;

  for (mapping = table; mapping->filters != ARB_END; mapping++) {
    if (mapping->sh_op != stmt.op) continue;
    if ((mapping->filters & match) != mapping->filters) continue;
    break;
  }
  if (mapping->filters == ARB_END) {
    error(Exception(std::string("ARB Code: Unknown operation ") + opInfo[stmt.op].name));
    return;
  }

  Statement actual = stmt;
  
  if (mapping->transforms & swap_sources) {
    Variable tmp(actual.src[0]);
    actual.src[0] = actual.src[1];
    actual.src[1] = tmp;
  }
  if (mapping->transforms & negate_first) {
    actual.src[0] = -actual.src[0];
  }
  
  std::list<Statement> stmts;
  if ((mapping->transforms & scalarize) && maxlen > 1) {
    for (int i = 0; i < maxlen; i++) {
      Statement scalar = actual;
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
    for (std::list<Statement>::iterator I = stmts.begin(); I != stmts.end(); ++I) {
      if (I->dest.swizzle().identity()) continue;

      Variable realdest(I->dest);
      Variable tmp(I->dest.node()->clone(SH_TEMP, SH_ATTRIB, 4));
      I->dest = tmp;

      Statement mask(realdest, OP_ASN, tmp);

      std::list<Statement>::iterator next = I; ++next;
      I = stmts.insert(next, mask);
    }
  }

  for (std::list<Statement>::const_iterator I = stmts.begin(); I != stmts.end(); ++I) {
    if (mapping->arb_op == ARB_FUN) {
      (this->*(mapping->function))(*I);
    } else {
      // HACK for delay_mask to work.
      ArbOp op = (I->op == OP_ASN) ? ARB_MOV : mapping->arb_op;
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

void ArbCode::emit_div(const Statement& stmt)
{
  // @todo type should handle other types (half-floats, fixed point)

  Variable rcp(new VariableNode(SH_TEMP, stmt.src[1].size(), SH_FLOAT));

  for (int i = 0; i < stmt.src[1].size(); i++) {
    m_instructions.push_back(ArbInst(ARB_RCP, rcp(i), stmt.src[1](i)));
  }
  m_instructions.push_back(ArbInst(ARB_MUL, stmt.dest, stmt.src[0], rcp));
}

void ArbCode::emit_sqrt(const Statement& stmt)
{
  Variable rsq(new VariableNode(SH_TEMP, 1, SH_FLOAT));
  m_instructions.push_back(ArbInst(ARB_RSQ, rsq, stmt.src[0]));
  m_instructions.push_back(ArbInst(ARB_RCP, stmt.dest, rsq));
}

void ArbCode::emit_lerp(const Statement& stmt)
{
  // lerp(f,a,b)= f*a + (1-f)*b = f*(a-b) + b
  
  Variable t(new VariableNode(SH_TEMP, stmt.src[1].size(), SH_FLOAT));
  m_instructions.push_back(ArbInst(ARB_ADD, t, stmt.src[1], -stmt.src[2]));
  m_instructions.push_back(ArbInst(ARB_MAD, stmt.dest, stmt.src[0], t, stmt.src[2]));
}

void ArbCode::emit_dot2(const Statement& stmt)
{
  Variable mul(new VariableNode(SH_TEMP, 2, SH_FLOAT));
  m_instructions.push_back(ArbInst(ARB_MUL, mul, stmt.src[0], stmt.src[1]));
  m_instructions.push_back(ArbInst(ARB_ADD, stmt.dest, mul(0), mul(1)));
}

void ArbCode::emit_eq(const Statement& stmt)
{
  Variable t1(new VariableNode(SH_TEMP, stmt.dest.size(), SH_FLOAT));
  Variable t2(new VariableNode(SH_TEMP, stmt.dest.size(), SH_FLOAT));

  ArbOp op, combine;
  if (stmt.op == OP_SEQ) {
    op = ARB_SGE;
    combine = ARB_MUL;
  } else if (stmt.op == OP_SNE) {
    op = ARB_SLT;
    combine = ARB_MAX;
  } else {
    DEBUG_ASSERT(false);
    return;
  }
  
  m_instructions.push_back(ArbInst(op, t1, stmt.src[0], stmt.src[1]));
  m_instructions.push_back(ArbInst(op, t2, stmt.src[1], stmt.src[0]));
  m_instructions.push_back(ArbInst(combine, stmt.dest, t1, t2));
}

void ArbCode::emit_ceil(const Statement& stmt)
{
  Variable t(new VariableNode(SH_TEMP, stmt.dest.size(), SH_FLOAT));

  m_instructions.push_back(ArbInst(ARB_FLR, t, -stmt.src[0])); 
  m_instructions.push_back(ArbInst(ARB_MOV, stmt.dest, -t));
}

void ArbCode::emit_mod(const Statement& stmt)
{
  // TODO - is this really optimal?
  Variable t1(new VariableNode(SH_TEMP, stmt.dest.size(), SH_FLOAT));
  Variable t2(new VariableNode(SH_TEMP, stmt.dest.size(), SH_FLOAT));
  
  // result = x - y*floor(x/y)
  emit(Statement(t1, stmt.src[0], OP_DIV, stmt.src[1]));
  m_instructions.push_back(ArbInst(ARB_FLR, t2, t1));
  m_instructions.push_back(ArbInst(ARB_MUL, t1, stmt.src[1], t2)); 
  m_instructions.push_back(ArbInst(ARB_SUB, stmt.dest, stmt.src[0], t1)); 
}

void ArbCode::emit_trig(const Statement& stmt)
{
  // Use float constants.  Conversions will take place as necessary
  ConstAttrib4f c0(0.00f,  0.50f, 1.00f, 0.0f);
  ConstAttrib4f c1(0.25f, -9.00f, 0.75f, 1.0f / (2.0 * M_PI));
  ConstAttrib4f c2(24.9808039603f, -24.9808039603f, -60.1458091736f, 60.1458091736f);
  ConstAttrib4f c3(85.4537887573f, -85.4537887573f, -64.9393539429f, 64.9393539429f);
  ConstAttrib4f c4(19.7392082214f, -19.7392082214f, -1.0f, 1.0f);

  m_shader->constants.push_back(c0.node());
  m_shader->constants.push_back(c1.node());
  m_shader->constants.push_back(c2.node());
  m_shader->constants.push_back(c3.node());
  m_shader->constants.push_back(c4.node());
  
  Variable r0(new VariableNode(SH_TEMP, 4, SH_FLOAT));
  Variable r1(new VariableNode(SH_TEMP, 4, SH_FLOAT));
  Variable r2(new VariableNode(SH_TEMP, 4, SH_FLOAT));
  Variable rs(new VariableNode(SH_TEMP, 4, SH_FLOAT));
  
  if (stmt.op == OP_SIN) {
    m_instructions.push_back(ArbInst(ARB_MAD, rs, c1(3,3,3,3), stmt.src[0], -c1(0,0,0,0)));
  } else if (stmt.op == OP_COS) {
    m_instructions.push_back(ArbInst(ARB_MUL, rs, c1(3,3,3,3), stmt.src[0]));
  } else {
    DEBUG_ASSERT(false);
  }
  m_instructions.push_back(ArbInst(ARB_FRC, rs, rs));
  for (int i = 0; i < stmt.src[0].size(); i++) {
    m_instructions.push_back(ArbInst(ARB_SLT, r2(0), rs(i), c1(0)));
    m_instructions.push_back(ArbInst(ARB_SGE, r2(1,2), rs(i,i), c1(1,2)));
    m_instructions.push_back(ArbInst(ARB_DP3, r2(1), r2(0,1,2), c4(2,3,2)));
    m_instructions.push_back(ArbInst(ARB_ADD, r0(0,1,2), -rs(i,i,i), c0(0,1,2)));
    m_instructions.push_back(ArbInst(ARB_MUL, r0, r0, r0));
    m_instructions.push_back(ArbInst(ARB_MAD, r1, c2(0,1,0,1), r0, c2(2,3,2,3)));
    m_instructions.push_back(ArbInst(ARB_MAD, r1, r1, r0, c3(0,1,0,1)));
    m_instructions.push_back(ArbInst(ARB_MAD, r1, r1, r0, c3(2,3,2,3)));
    m_instructions.push_back(ArbInst(ARB_MAD, r1, r1, r0, c4(0,1,0,1)));
    m_instructions.push_back(ArbInst(ARB_MAD, r1, r1, r0, c4(2,3,2,3)));
    m_instructions.push_back(ArbInst(ARB_DP3, r0(0), r1(0,1,2), -r2(0,1,2)));
    m_instructions.push_back(ArbInst(ARB_MOV, stmt.dest(i), r0(0)));
  }
}

void ArbCode::emit_invtrig(const Statement& stmt)
{
  // Use float constants.  Conversions will take place to other types as necessary 
  ConstAttrib4f c0(0.0f, 1.570796327f, -0.5860008052f, 0.5860008052f);
  ConstAttrib4f c1(1.5719451050f, -1.5719451050f, -1.6696689770f,  1.6696689770f);
  ConstAttrib4f c2(0.8999841642f, -0.8999841642f, -0.6575341673f,  0.6575341673f);
  ConstAttrib4f c3(1.0123866490f, -1.0123866490f,  0.9998421793f, -0.9998421793f);
  ConstAttrib4f c4(1.0f, -1.0f, 1.0f, -1.0f);

  m_shader->constants.push_back(c0.node());
  m_shader->constants.push_back(c1.node());
  m_shader->constants.push_back(c2.node());
  m_shader->constants.push_back(c3.node());
  m_shader->constants.push_back(c4.node());
  
  Variable r0(new VariableNode(SH_TEMP, 4, SH_FLOAT));
  Variable r1(new VariableNode(SH_TEMP, 4, SH_FLOAT));
  Variable r2(new VariableNode(SH_TEMP, 4, SH_FLOAT));
  Variable offset(new VariableNode(SH_TEMP, 4, SH_FLOAT));
  Variable output(new VariableNode(SH_TEMP, stmt.dest.size(), SH_FLOAT));
  m_instructions.push_back(ArbInst(ARB_ABS, r0, stmt.src[0]));
  m_instructions.push_back(ArbInst(ARB_MAD, offset, -r0, r0, c4(0,0,0,0)));
  
  m_instructions.push_back(ArbInst(ARB_MOV, r2, c0(0,1,0,1)));
  for (int i = 0; i < stmt.src[0].size(); i++) {
    m_instructions.push_back(ArbInst(ARB_SLT, r2(1), stmt.src[0](i), c0(0)));
    m_instructions.push_back(ArbInst(ARB_SGE, r2(0), stmt.src[0](i), c0(0)));
    m_instructions.push_back(ArbInst(ARB_MAD, r1(0,1), c0(2,3), r0(i,i), c1(0,1)));
    m_instructions.push_back(ArbInst(ARB_MAD, r1(0,1), r1(0,1), r0(i,i), c1(2,3)));
    m_instructions.push_back(ArbInst(ARB_MAD, r1(0,1), r1(0,1), r0(i,i), c2(0,1)));
    m_instructions.push_back(ArbInst(ARB_MAD, r1(0,1), r1(0,1), r0(i,i), c2(2,3)));
    m_instructions.push_back(ArbInst(ARB_MAD, r1(0,1), r1(0,1), r0(i,i), c3(0,1)));
    m_instructions.push_back(ArbInst(ARB_MAD, r1(0,1), r1(0,1), r0(i,i), c3(2,3)));
    m_instructions.push_back(ArbInst(ARB_RSQ,offset(i), offset(i)));
    m_instructions.push_back(ArbInst(ARB_RCP,offset(i), offset(i)));
    m_instructions.push_back(ArbInst(ARB_MAD, r1(0,1), c4(1,0), offset(i,i), r1(0,1)));
    if (stmt.op == OP_ACOS) {
      m_instructions.push_back(ArbInst(ARB_DP3, output(i), r1(0,1,2), r2(0,1,2)));
    } else {
      m_instructions.push_back(ArbInst(ARB_DP3, stmt.dest(i), r1(0,1,2), r2(0,1,2)));
    }
  }
  if (stmt.op == OP_ACOS) {
    m_instructions.push_back(ArbInst(ARB_ADD, stmt.dest, -output, c0(1,1,1,1)));
  }
}

void ArbCode::emit_atan(const Statement& stmt)
{
  Variable tmp(new VariableNode(SH_TEMP, stmt.src[0].size(), SH_FLOAT));
  
  // arctan(x) = arcsin( x / sqrt(x^2 + 1) )
  emit(Statement(tmp, stmt.src[0], OP_MUL, stmt.src[0]));
  emit(Statement(tmp, tmp, OP_ADD, ConstAttrib1f(1)));
  emit(Statement(tmp, OP_SQRT, tmp));
  emit(Statement(tmp, stmt.src[0], OP_DIV, tmp));
  emit(Statement(stmt.dest, OP_ASIN, tmp));
}

void ArbCode::emit_tan(const Statement& stmt)
{
  Variable tmp1(new VariableNode(SH_TEMP, stmt.src[0].size(), SH_FLOAT));
  Variable tmp2(new VariableNode(SH_TEMP, stmt.src[0].size(), SH_FLOAT));

  emit(Statement(tmp1, OP_COS, stmt.src[0]));
  emit(Statement(tmp1, OP_RCP, tmp1));
  emit(Statement(tmp2, OP_SIN, stmt.src[0]));
  
  m_instructions.push_back(ArbInst(ARB_MUL, stmt.dest, tmp1, tmp2));
}

void ArbCode::emit_exp(const Statement& stmt)
{
  float basef = (stmt.op == OP_EXP ? M_E : 10.0f);

  ConstAttrib1f base(basef);
  m_shader->constants.push_back(base.node());

  m_instructions.push_back(ArbInst(ARB_POW, stmt.dest, base, stmt.src[0]));
}

void ArbCode::emit_hyperbolic(const Statement& stmt)
{
  ConstAttrib1f two(2.0f);
  m_shader->constants.push_back(two.node());

  Variable e_plusX(new VariableNode(SH_TEMP, stmt.src[0].size(), SH_FLOAT)); // e^x
  Variable e_minusX(new VariableNode(SH_TEMP, stmt.src[0].size(), SH_FLOAT)); // e^-x
  emit(Statement(e_plusX, OP_EXP, stmt.src[0]));
  emit(Statement(e_minusX, OP_NEG, stmt.src[0]));
  emit(Statement(e_minusX, OP_EXP, e_minusX));

  Variable temp_cosh(new VariableNode(SH_TEMP, stmt.src[0].size(), SH_FLOAT)); // e^x + e^-x
  Variable temp_sinh(new VariableNode(SH_TEMP, stmt.src[0].size(), SH_FLOAT)); // e^x - e^-x

  switch (stmt.op) {
  case OP_COSH:
    // cosh x = [e^x + e^-x] / 2
    m_instructions.push_back(ArbInst(ARB_ADD, temp_cosh, e_plusX, e_minusX));
    emit(Statement(stmt.dest, temp_cosh, OP_DIV, two));
    break;
  case OP_SINH:
    // cosh x = [e^x - e^-x] / 2
    m_instructions.push_back(ArbInst(ARB_SUB, temp_sinh, e_plusX, e_minusX)); 
    emit(Statement(stmt.dest, temp_sinh, OP_DIV, two));
    break;
  case OP_TANH:
    // tanh x = sinh x / cosh x = [e^x - e^-x] / [e^x + e^-x]
    m_instructions.push_back(ArbInst(ARB_ADD, temp_cosh, e_plusX, e_minusX)); 
    m_instructions.push_back(ArbInst(ARB_SUB, temp_sinh, e_plusX, e_minusX)); 
    emit(Statement(stmt.dest, temp_sinh, OP_DIV, temp_cosh));
    break;
  default:
    DEBUG_ASSERT(0);
  }
}

void ArbCode::emit_log(const Statement& stmt)
{
  float scalef = 1.0/log2((stmt.op == OP_LOG ? M_E : 10.0f));

  ConstAttrib1f scale(scalef);
  m_shader->constants.push_back(scale.node());

  Variable tmp(new VariableNode(SH_TEMP, stmt.dest.size(), SH_FLOAT));
  
  emit(Statement(tmp, OP_LOG2, stmt.src[0]));
  m_instructions.push_back(ArbInst(ARB_MUL, stmt.dest, tmp, scale));
}

void ArbCode::emit_norm(const Statement& stmt)
{
  Variable tmp(new VariableNode(SH_TEMP, 1, SH_FLOAT));
  emit(Statement(tmp, stmt.src[0], OP_DOT, stmt.src[0]));
  m_instructions.push_back(ArbInst(ARB_RSQ, tmp, tmp));
  m_instructions.push_back(ArbInst(ARB_MUL, stmt.dest, tmp, stmt.src[0]));
}

void ArbCode::emit_sgn(const Statement& stmt)
{
  Variable tmp1(new VariableNode(SH_TEMP, stmt.src[0].size(), SH_FLOAT));
  Variable tmp2(new VariableNode(SH_TEMP, stmt.src[0].size(), SH_FLOAT));
  ConstAttrib1f zero(0.0);

  emit(Statement(tmp1, stmt.src[0], OP_SGT, zero));
  emit(Statement(tmp2, stmt.src[0], OP_SLT, zero));
  m_instructions.push_back(ArbInst(ARB_SUB, stmt.dest, tmp1, tmp2)); 
}

void ArbCode::emit_tex(const Statement& stmt)
{
  bool delay = false;
  Variable tmpdest;
  Variable tmpsrc;
  
  TextureNodePtr tnode = shref_dynamic_cast<TextureNode>(stmt.src[0].node());

  if (!stmt.dest.swizzle().identity()) {
    tmpdest = Variable(new VariableNode(SH_TEMP, tnode->size(), SH_FLOAT));
    tmpsrc = tmpdest;
    delay = true;
  }

  DEBUG_ASSERT(tnode);

  if (tnode->size() == 2) {
    // Special case for LUMINANCE_ALPHA
    tmpdest = Variable(new VariableNode(SH_TEMP, 4, SH_FLOAT));
    tmpsrc = tmpdest(0,3);
    delay = true;
  }

  if (stmt.op == OP_TEXD) {
    DEBUG_ASSERT(tnode->dims() == SH_TEXTURE_2D);
    m_instructions.push_back(ArbInst(ARB_TXD,
                                     (delay ? tmpdest : stmt.dest), stmt.src[1], stmt.src[0],
                                     stmt.src[2](0,1), stmt.src[2](2,3)));
  } else {
    m_instructions.push_back(ArbInst(ARB_TEX,
                                     (delay ? tmpdest : stmt.dest), stmt.src[1], stmt.src[0]));
  }

  if (delay) emit(Statement(stmt.dest, OP_ASN, tmpsrc));
}

void ArbCode::emit_nvcond(const Statement& stmt)
{
  Variable dummy(new VariableNode(SH_TEMP, stmt.src[0].size(), SH_FLOAT));
  ArbInst updatecc(ARB_MOV, dummy, stmt.src[0]);
  updatecc.update_cc = true;
  m_instructions.push_back(updatecc);

  if (stmt.dest != stmt.src[1]) {
    ArbInst movt(ARB_MOV, stmt.dest, stmt.src[1]);
    movt.ccode = ArbInst::GT;
    movt.ccswiz = stmt.src[0].swizzle();
    m_instructions.push_back(movt);
  }
  if (stmt.dest != stmt.src[2]) {
    ArbInst movf(ARB_MOV, stmt.dest, stmt.src[2]);
    movf.ccode = ArbInst::LE;
    movf.ccswiz = stmt.src[0].swizzle();
    m_instructions.push_back(movf);
  }
}

void ArbCode::emit_csum(const Statement& stmt)
{

  // @todo type make this function handle more than floats
  VariantPtr c1_values = new DataVariant<float, HOST>(stmt.src[0].size(), 1.0f); 
  Variable c1(new VariableNode(SH_CONST, stmt.src[0].size(), SH_FLOAT));
  c1.setVariant(c1_values);
  m_shader->constants.push_back(c1.node());
  
  emit(Statement(stmt.dest, stmt.src[0], OP_DOT, c1));
}

void ArbCode::emit_cmul(const Statement& stmt)
{
  // @todo use clone
  Variable prod(new VariableNode(SH_TEMP, 1, stmt.dest.valueType()));

  // TODO: Could use vector mul here.
  
  m_instructions.push_back(ArbInst(ARB_MOV, prod, stmt.src[0](0)));
  for (int i = 1; i < stmt.src[0].size(); i++) {
    m_instructions.push_back(ArbInst(ARB_MUL, prod, prod, stmt.src[0](i)));
  }
  m_instructions.push_back(ArbInst(ARB_MOV, stmt.dest, prod));
}

void ArbCode::emit_kil(const Statement& stmt)
{
  m_instructions.push_back(ArbInst(ARB_KIL, -stmt.src[0]));
}

void ArbCode::emit_ret(const Statement& stmt)
{
  Variable dummy(new VariableNode(SH_TEMP, stmt.src[0].size(), SH_FLOAT));
  ArbInst updatecc(ARB_MOV, dummy, stmt.src[0]);
  updatecc.update_cc = true;
  m_instructions.push_back(updatecc);
  m_instructions.push_back(ArbInst(ARB_RET));
}

void ArbCode::emit_pal(const Statement& stmt)
{
  m_instructions.push_back(ArbInst(ARB_ARL, m_address_register, stmt.src[1]));
  m_instructions.push_back(ArbInst(ARB_ARRAYMOV, stmt.dest, stmt.src[0], m_address_register));
}

void ArbCode::emit_lit(const Statement& stmt)
{
  Variable tmp(new VariableNode(SH_TEMP, stmt.dest.size(), SH_FLOAT));
  ArbInst inst(ARB_LIT, tmp, stmt.src[0]);
  m_instructions.push_back(inst);

  emit(Statement(stmt.dest, OP_ASN, tmp));
}

void ArbCode::emit_rnd(const Statement& stmt)
{
  Variable t(new VariableNode(SH_TEMP, stmt.dest.size(), SH_FLOAT));
  
  VariantPtr c1_values = new DataVariant<float, HOST>(stmt.src[0].size(), 0.5f); 
  Variable c1(new VariableNode(SH_CONST, stmt.src[0].size(), SH_FLOAT));
  c1.setVariant(c1_values);
  m_shader->constants.push_back(c1.node());

  m_instructions.push_back(ArbInst(ARB_ADD, t, stmt.src[0], c1)); 
  m_instructions.push_back(ArbInst(ARB_FLR, stmt.dest, t));
}

void ArbCode::emit_cbrt(const Statement& stmt)
{
  Variable t(new VariableNode(SH_TEMP, 1, SH_FLOAT));
  ConstAttrib1f c(3.0f);
  m_shader->constants.push_back(c.node());

  m_instructions.push_back(ArbInst(ARB_RCP, t, c)); 
  m_instructions.push_back(ArbInst(ARB_POW, stmt.dest, stmt.src[0], t));
}

void ArbCode::emit_comment(const Statement& stmt)
{
  m_instructions.push_back(ArbInst(ARB_COMMENT, stmt.get_info<InfoComment>()->comment));
}

}
