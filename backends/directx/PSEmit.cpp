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
#include "PSCode.hpp"
#include <algorithm>
#include <cmath>
#include "ShDebug.hpp"
#include "ShError.hpp"

#ifdef WIN32
namespace {
double log2(double x) { return log(x)/log(2.0); }
}
#endif

namespace shdx {

using namespace SH;

// Transformations
namespace {
const unsigned int scalarize    = 0x01; // Split into scalar instructions
const unsigned int swap_sources = 0x02; // Swap first and second sources
const unsigned int negate_first = 0x04; // Negate first source
const unsigned int delay_mask   = 0x08; // Do writemasking in separate step
};

struct PSMapping {
  ShOperation sh_op;
  unsigned int filters;
  
  unsigned int transforms;
  PSOp arb_op;

  typedef void (PSCode::*PSFunction)(const ShStatement&);
  PSFunction function;
};

PSMapping PSCode::table[] = {
  {SH_OP_ASN, SH_PS_ANY, 0, SH_PS_MOV, 0},

  // Arithmetic
  {SH_OP_ADD,  SH_PS_ANY,   0,            SH_PS_ADD, 0},
  {SH_OP_NEG,  SH_PS_ANY,   negate_first, SH_PS_MOV, 0},
  {SH_OP_MUL,  SH_PS_ANY,   0,            SH_PS_MUL, 0},

  {SH_OP_DIV,  SH_PS_ANY,   scalarize,    SH_PS_FUN, &PSCode::emit_div},
  {SH_OP_POW,  SH_PS_ANY,   scalarize,    SH_PS_POW, 0},
  {SH_OP_RCP,  SH_PS_ANY,   scalarize,    SH_PS_RCP, 0},
  {SH_OP_RSQ,  SH_PS_ANY,   scalarize,    SH_PS_RSQ, 0},
  {SH_OP_SQRT, SH_PS_ANY,   scalarize,    SH_PS_FUN, &PSCode::emit_sqrt},

  {SH_OP_LRP, SH_PS_PS_3_0 | SH_PS_VS_3_0,  0, SH_PS_LRP, 0},
  {SH_OP_LRP, SH_PS_ANY,  0, SH_PS_FUN, &PSCode::emit_lerp},
  {SH_OP_MAD, SH_PS_ANY, 0, SH_PS_MAD, 0},

  // Sum/product of components
  {SH_OP_CMUL, SH_PS_ANY, 0, SH_PS_FUN, &PSCode::emit_cmul},
  {SH_OP_CSUM, SH_PS_ANY, 0, SH_PS_FUN, &PSCode::emit_csum},
  
  // Dot product
  {SH_OP_DOT, SH_PS_VEC1,                0,         SH_PS_MUL, 0},
  {SH_OP_DOT, SH_PS_VEC3,                0,         SH_PS_DP3, 0},
  {SH_OP_DOT, SH_PS_VEC4,                0,         SH_PS_DP4, 0},
  {SH_OP_DOT, SH_PS_VEC2,                0,         SH_PS_FUN, &PSCode::emit_dot2},

  // Boolean
  {SH_OP_SLT, SH_PS_ANY,   0,            SH_PS_SLT, 0},
  {SH_OP_SGE, SH_PS_ANY,   0,            SH_PS_SGE, 0},

  /*{SH_OP_SLE, SH_PS_NVVP2, 0,            SH_PS_SLE, 0},
  {SH_OP_SLE, SH_PS_NVFP,  0,            SH_PS_SLE, 0},
  {SH_OP_SLE, SH_PS_ANY,   swap_sources, SH_PS_SGE, 0},*/

  /*{SH_OP_SGT, SH_PS_NVVP2, 0,            SH_PS_SGT, 0},
  {SH_OP_SGT, SH_PS_NVFP,  0,            SH_PS_SGT, 0},
  {SH_OP_SGT, SH_PS_ANY,   swap_sources, SH_PS_SLT, 0},*/

  /*{SH_OP_SEQ, SH_PS_NVVP2, 0,            SH_PS_SEQ, 0},
  {SH_OP_SEQ, SH_PS_NVFP,  0,            SH_PS_SEQ, 0},
  {SH_OP_SEQ, SH_PS_ANY,   0,            SH_PS_FUN, &PSCode::emit_eq},*/

  /*{SH_OP_SNE, SH_PS_NVVP2, 0,            SH_PS_SNE, 0},
  {SH_OP_SNE, SH_PS_NVFP,  0,            SH_PS_SNE, 0},
  {SH_OP_SNE, SH_PS_ANY,   0,            SH_PS_FUN, &PSCode::emit_eq},*/

  // Clamping
  {SH_OP_ABS,  SH_PS_ANY, 0, SH_PS_ABS, 0},
  {SH_OP_CEIL, SH_PS_ANY, 0, SH_PS_FUN, &PSCode::emit_ceil},
  {SH_OP_FLR,  SH_PS_ANY, 0, SH_PS_FUN, &PSCode::emit_floor},
  {SH_OP_FRAC, SH_PS_ANY, 0, SH_PS_FRC, 0},
  {SH_OP_MOD,  SH_PS_ANY, 0, SH_PS_FUN, &PSCode::emit_mod},
  {SH_OP_MAX,  SH_PS_ANY, 0, SH_PS_MAX, 0},
  {SH_OP_MIN,  SH_PS_ANY, 0, SH_PS_MIN, 0},
  /* TODO: This has different semantics from OpenGL's SSG isntruction
  //{SH_OP_SGN,  SH_PS_VS_3_0, 0, SH_PS_SGN, 0},
  */
  {SH_OP_SGN,  SH_PS_ANY, 0, SH_PS_FUN, &PSCode::emit_sgn},
  
  // Trig
  {SH_OP_ACOS,  SH_PS_ANY, 0,         SH_PS_FUN, &PSCode::emit_invtrig},
  {SH_OP_ASIN,  SH_PS_ANY, 0,         SH_PS_FUN, &PSCode::emit_invtrig},
  /* TODO
  {SH_OP_ATAN,  SH_PS_ANY, 0,         SH_PS_FUN, &PSCode::emit_invtrig},
  {SH_OP_ATAN2, SH_PS_ANY, 0,         SH_PS_FUN, &PSCode::emit_invtrig},
  */
  //{SH_OP_COS,   SH_PS_PS_1_1,  scalarize, SH_PS_COS, 0},
  {SH_OP_COS,   SH_PS_ANY,  0,         SH_PS_FUN, &PSCode::emit_trig},
  //{SH_OP_SIN,   SH_PS_PS_1_1,  scalarize, SH_PS_SIN, 0},
  {SH_OP_SIN,   SH_PS_ANY,  0,         SH_PS_FUN, &PSCode::emit_trig},
  {SH_OP_TAN,   SH_PS_ANY, 0,         SH_PS_FUN, &PSCode::emit_tan},

  // Derivatives
  {SH_OP_DX, SH_PS_PS_3_0, 0, SH_PS_DSX, 0},
  {SH_OP_DY, SH_PS_PS_3_0, 0, SH_PS_DSY, 0},

  // Expontential
  {SH_OP_EXP2,  SH_PS_ANY, scalarize, SH_PS_EXP, 0},
  {SH_OP_LOG2,  SH_PS_ANY, scalarize, SH_PS_LOG, 0},
  {SH_OP_EXP,   SH_PS_ANY, scalarize, SH_PS_FUN, &PSCode::emit_exp},
  {SH_OP_LOG,   SH_PS_ANY, 0,         SH_PS_FUN, &PSCode::emit_log},
  {SH_OP_EXP10, SH_PS_ANY, scalarize, SH_PS_FUN, &PSCode::emit_exp},
  {SH_OP_LOG10, SH_PS_ANY, 0,         SH_PS_FUN, &PSCode::emit_log},

  // Geometric
  {SH_OP_NORM, SH_PS_VS_3_0 | SH_PS_VEC3, 0, SH_PS_FUN, &PSCode::emit_no_output_norm},
  {SH_OP_NORM, SH_PS_PS_3_0 | SH_PS_VEC3, 0, SH_PS_FUN, &PSCode::emit_no_output_norm},
  {SH_OP_NORM, SH_PS_ANY,                 0, SH_PS_FUN, &PSCode::emit_norm},
  {SH_OP_XPD,  SH_PS_VS_3_0 | SH_PS_VEC3,   0, SH_PS_CRS, 0},
  {SH_OP_XPD,  SH_PS_PS_3_0 | SH_PS_VEC3,   0, SH_PS_CRS, 0},

  // Texture
  /*{SH_OP_TEX,  SH_PS_NVVP2, 0, SH_PS_FUN, &PSCode::emit_tex},
  {SH_OP_TEX,  SH_PS_PS_1_1,    0, SH_PS_FUN, &PSCode::emit_tex},
  {SH_OP_TEXI, SH_PS_NVVP2, 0, SH_PS_FUN, &PSCode::emit_tex},
  {SH_OP_TEXI, SH_PS_PS_1_1,    0, SH_PS_FUN, &PSCode::emit_tex},*/

  // Misc.
  /*{SH_OP_COND, SH_PS_NVFP, 0, SH_PS_FUN, &PSCode::emit_nvcond},
  {SH_OP_COND, SH_PS_NVVP2, 0, SH_PS_FUN, &PSCode::emit_nvcond},
  {SH_OP_COND, SH_PS_ANY, negate_first, SH_PS_CMP, 0},
  {SH_OP_KIL,  SH_PS_PS_1_1,  0, SH_PS_FUN, &PSCode::emit_kil},*/

  {SH_OP_ASN, SH_PS_END, 0, SH_PS_FUN, 0}
};

void PSCode::emit(const ShStatement& stmt)
{
  int maxlen = 0; // Maximum tuple length over all sources
  for (int i = 0; i < opInfo[stmt.op].arity; i++) {
    if (stmt.src[i].size() > maxlen) maxlen = stmt.src[i].size();
  }

  unsigned int match = m_environment;
  switch(maxlen) {
  case 1: match |= SH_PS_VEC1; break;
  case 2: match |= SH_PS_VEC2; break;
  case 3: match |= SH_PS_VEC3; break;
  case 4: match |= SH_PS_VEC4; break;
  }
  
  PSMapping* mapping;

  for (mapping = table; mapping->filters != SH_PS_END; mapping++) {
    if (mapping->sh_op != stmt.op) continue;
    if ((mapping->filters & match) != mapping->filters) continue;
    break;
  }
  if (mapping->filters == SH_PS_END) {
	  FILE *fp = fopen("c:\\mike.txt", "wt");
	  fprintf(fp, "Unknown operation: %s\n", opInfo[stmt.op].name);
	  fclose(fp);
    shError(ShException(std::string("PS Code: Unknown operation ") + opInfo[stmt.op].name));
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
      ShVariable tmp(new ShVariableNode(SH_TEMP, 4));
      I->dest = tmp;

      ShStatement mask(realdest, SH_OP_ASN, tmp);

      std::list<ShStatement>::iterator next = I; ++next;
      I = stmts.insert(next, mask);
    }
  }

  for (std::list<ShStatement>::const_iterator I = stmts.begin(); I != stmts.end(); ++I) {
    if (mapping->arb_op == SH_PS_FUN) {
      (this->*(mapping->function))(*I);
    } else {
      // HACK for delay_mask to work.
      PSOp op = (I->op == SH_OP_ASN) ? SH_PS_MOV : mapping->arb_op;
      switch (opInfo[I->op].arity) {
      case 0:
        m_instructions.push_back(PSInst(op, I->dest));
        break;
      case 1:
        m_instructions.push_back(PSInst(op, I->dest,
                                         I->src[0]));
        break;
      case 2:
        m_instructions.push_back(PSInst(op, I->dest,
                                         I->src[0], I->src[1]));
        break;
      case 3:
        m_instructions.push_back(PSInst(op, I->dest,
                                         I->src[0], I->src[1], I->src[2]));
        break;
      }
    }
  }
}

void PSCode::emit_div(const ShStatement& stmt)
{
  ShVariable rcp(new ShVariableNode(SH_TEMP, 1));
  m_instructions.push_back(PSInst(SH_PS_RCP, rcp, stmt.src[1]));
  m_instructions.push_back(PSInst(SH_PS_MUL, stmt.dest, stmt.src[0], rcp));
}

void PSCode::emit_sqrt(const ShStatement& stmt)
{
  ShVariable rsq(new ShVariableNode(SH_TEMP, 1));
  m_instructions.push_back(PSInst(SH_PS_RSQ, rsq, stmt.src[0]));
  m_instructions.push_back(PSInst(SH_PS_RCP, stmt.dest, rsq));
}

void PSCode::emit_lerp(const ShStatement& stmt)
{
  // lerp(f,a,b)= f*a + (1-f)*b = f*(a-b) + b
  
  ShVariable t(new ShVariableNode(SH_TEMP, stmt.src[1].size()));
  m_instructions.push_back(PSInst(SH_PS_ADD, t, stmt.src[1], -stmt.src[2]));
  m_instructions.push_back(PSInst(SH_PS_MAD, stmt.dest, stmt.src[0], t, stmt.src[2]));
}

void PSCode::emit_dot2(const ShStatement& stmt)
{
  ShVariable mul(new ShVariableNode(SH_TEMP, 2));
  m_instructions.push_back(PSInst(SH_PS_MUL, mul, stmt.src[0], stmt.src[1]));
  m_instructions.push_back(PSInst(SH_PS_ADD, stmt.dest, mul(0), mul(1)));
}

void PSCode::emit_eq(const ShStatement& stmt)
{
/*  ShVariable t1(new ShVariableNode(SH_TEMP, stmt.dest.size()));
  ShVariable t2(new ShVariableNode(SH_TEMP, stmt.dest.size()));

  PSOp op;
  if (stmt.op == SH_OP_SEQ) {
    op = SH_ARB_SGE;
  } else if (stmt.op == SH_OP_SNE) {
    op = SH_ARB_SLT;
  } else {
    SH_DEBUG_ASSERT(false);
  }
  
  m_instructions.push_back(PSInst(SH_ARB_SGE, t1, stmt.src[0], stmt.src[1]));
  m_instructions.push_back(PSInst(SH_ARB_SGE, t2, stmt.src[1], stmt.src[0]));
  m_instructions.push_back(PSInst(SH_ARB_MUL, stmt.dest, t1, t2));*/
}

// Calculate the floor by subtracting the fractional component from the source
void PSCode::emit_floor(const ShStatement& stmt)
{
  ShVariable t1(new ShVariableNode(SH_TEMP, stmt.src[0].size()));

  m_instructions.push_back(PSInst(SH_PS_FRC, t1, stmt.src[0]));
  m_instructions.push_back(PSInst(SH_PS_SUB, stmt.dest, stmt.src[0], t1));
}

// Simulate ceil by subtracting off the fractional component from the 
// negative source and flipping the sign
void PSCode::emit_ceil(const ShStatement& stmt)
{
  ShVariable t1(new ShVariableNode(SH_TEMP, stmt.src[0].size()));

  m_instructions.push_back(PSInst(SH_PS_FRC, t1, stmt.src[0]));
  m_instructions.push_back(PSInst(SH_PS_SUB, t1, -stmt.src[0], t1));
  m_instructions.push_back(PSInst(SH_PS_MOV, stmt.dest, -t1));
}

void PSCode::emit_mod(const ShStatement& stmt)
{
  // TODO - is this really optimal?
  ShVariable t1(new ShVariableNode(SH_TEMP, stmt.src[0].size()));
  ShVariable t2(new ShVariableNode(SH_TEMP, stmt.src[0].size()));
  ShVariable t3(new ShVariableNode(SH_TEMP, stmt.src[0].size()));
  
  // result = x - sign(x/y)*floor(abs(x/y))*y
  emit(ShStatement(t1, stmt.src[0], SH_OP_DIV, stmt.src[1]));
  m_instructions.push_back(PSInst(SH_PS_ABS, t2, t1));
  emit(ShStatement(t1, SH_OP_SGN, t1));

  // Simulation for floor: put fractional component of t2 in t3,
  // subtract t3 from t2 and store in t2
  m_instructions.push_back(PSInst(SH_PS_FRC, t3, t2));
  m_instructions.push_back(PSInst(SH_PS_SUB, t2, t2, t3));

  m_instructions.push_back(PSInst(SH_PS_MUL, t1, t1, t2)); 
  m_instructions.push_back(PSInst(SH_PS_MUL, t1, t1, stmt.src[1])); 
  m_instructions.push_back(PSInst(SH_PS_SUB, stmt.dest, stmt.src[0], t1)); 
}

void PSCode::emit_trig(const ShStatement& stmt)
{
  ShVariableNode::ValueType c0_values[] = 
    { 0.0, 0.5, 1.0, 0.0 };
  ShVariable c0(new ShVariableNode(SH_CONST, 4));
  c0.setValues(c0_values);
  ShVariableNode::ValueType c1_values[] = 
    { 0.25, -9.0, 0.75, 1.0/(2.0*M_PI) };
  ShVariable c1(new ShVariableNode(SH_CONST, 4));
  c1.setValues(c1_values);
  ShVariableNode::ValueType c2_values[] = 
    { 24.9808039603, -24.9808039603, -60.1458091736, 60.1458091736 };
  ShVariable c2(new ShVariableNode(SH_CONST, 4));
  c2.setValues(c2_values);
  ShVariableNode::ValueType c3_values[] = 
    { 85.4537887573, -85.4537887573, -64.9393539429, 64.9393539429 };
  ShVariable c3(new ShVariableNode(SH_CONST, 4));
  c3.setValues(c3_values);
  ShVariableNode::ValueType c4_values[] = 
    { 19.7392082214, -19.7392082214, -1.0, 1.0 };
  ShVariable c4(new ShVariableNode(SH_CONST, 4));
  c4.setValues(c4_values);
  m_shader->constants.push_back(c0.node());
  m_shader->constants.push_back(c1.node());
  m_shader->constants.push_back(c2.node());
  m_shader->constants.push_back(c3.node());
  m_shader->constants.push_back(c4.node());
  
  ShVariable r0(new ShVariableNode(SH_TEMP, 4));
  ShVariable r1(new ShVariableNode(SH_TEMP, 4));
  ShVariable r2(new ShVariableNode(SH_TEMP, 4));
  ShVariable rs(new ShVariableNode(SH_TEMP, 4));
  
  if (stmt.op == SH_OP_SIN) {
    m_instructions.push_back(PSInst(SH_PS_MAD, rs, c1(3,3,3,3), stmt.src[0], -c1(0,0,0,0)));
  } else if (stmt.op == SH_OP_COS) {
    m_instructions.push_back(PSInst(SH_PS_MUL, rs, c1(3,3,3,3), stmt.src[0]));
  } else {
    SH_DEBUG_ASSERT(false);
  }
  m_instructions.push_back(PSInst(SH_PS_FRC, rs, rs));
  for (int i = 0; i < stmt.src[0].size(); i++) {
    m_instructions.push_back(PSInst(SH_PS_SLT, r2(0), rs(i), c1(0)));
    m_instructions.push_back(PSInst(SH_PS_SGE, r2(1,2), rs(i,i), c1(1,2)));
    m_instructions.push_back(PSInst(SH_PS_DP3, r2(1), r2(0,1,2), c4(2,3,2)));
    m_instructions.push_back(PSInst(SH_PS_ADD, r0(0,1,2), -rs(i,i,i), c0(0,1,2)));
    m_instructions.push_back(PSInst(SH_PS_MUL, r0, r0, r0));
    m_instructions.push_back(PSInst(SH_PS_MAD, r1, c2(0,1,0,1), r0, c2(2,3,2,3)));
    m_instructions.push_back(PSInst(SH_PS_MAD, r1, r1, r0, c3(0,1,0,1)));
    m_instructions.push_back(PSInst(SH_PS_MAD, r1, r1, r0, c3(2,3,2,3)));
    m_instructions.push_back(PSInst(SH_PS_MAD, r1, r1, r0, c4(0,1,0,1)));
    m_instructions.push_back(PSInst(SH_PS_MAD, r1, r1, r0, c4(2,3,2,3)));
    m_instructions.push_back(PSInst(SH_PS_DP3, r0(0), r1(0,1,2), -r2(0,1,2)));
    m_instructions.push_back(PSInst(SH_PS_MOV, stmt.dest(i), r0(0)));
  }
}

void PSCode::emit_invtrig(const ShStatement& stmt)
{
  ShVariableNode::ValueType c0_values[] = 
    { 0.0, 1.570796327, -0.5860008052, 0.5860008052 };
  ShVariable c0(new ShVariableNode(SH_CONST, 4));
  c0.setValues(c0_values);
  ShVariableNode::ValueType c1_values[] = 
    { 1.571945105, -1.571945105, -1.669668977, 1.669668977 };
  ShVariable c1(new ShVariableNode(SH_CONST, 4));
  c1.setValues(c1_values);
  ShVariableNode::ValueType c2_values[] = 
    { 0.8999841642, -0.8999841642, -0.6575341673, 0.6575341673 };
  ShVariable c2(new ShVariableNode(SH_CONST, 4));
  c2.setValues(c2_values);
  ShVariableNode::ValueType c3_values[] = 
    { 1.012386649, -1.012386649, 0.9998421793, -0.9998421793 };
  ShVariable c3(new ShVariableNode(SH_CONST, 4));
  c3.setValues(c3_values);
  ShVariableNode::ValueType c4_values[] = 
    { 1.0, -1.0, 1.0, -1.0 };
  ShVariable c4(new ShVariableNode(SH_CONST, 4));
  c4.setValues(c4_values);
  m_shader->constants.push_back(c0.node());
  m_shader->constants.push_back(c1.node());
  m_shader->constants.push_back(c2.node());
  m_shader->constants.push_back(c3.node());
  m_shader->constants.push_back(c4.node());
  
  ShVariable r0(new ShVariableNode(SH_TEMP, 4));
  ShVariable r1(new ShVariableNode(SH_TEMP, 4));
  ShVariable r2(new ShVariableNode(SH_TEMP, 4));
  ShVariable offset(new ShVariableNode(SH_TEMP, 4));
  ShVariable output(new ShVariableNode(SH_TEMP, stmt.dest.size()));
  m_instructions.push_back(PSInst(SH_PS_ABS, r0, stmt.src[0]));
  m_instructions.push_back(PSInst(SH_PS_MAD, offset, -r0, r0, c4(0,0,0,0)));
  
  m_instructions.push_back(PSInst(SH_PS_MOV, r2, c0(0,1,0,1)));
  for (int i = 0; i < stmt.src[0].size(); i++) {
    m_instructions.push_back(PSInst(SH_PS_SLT, r2(1), stmt.src[0](i), c0(0)));
    m_instructions.push_back(PSInst(SH_PS_SGE, r2(0), stmt.src[0](i), c0(0)));
    m_instructions.push_back(PSInst(SH_PS_MAD, r1(0,1), c0(2,3), r0(i,i), c1(0,1)));
    m_instructions.push_back(PSInst(SH_PS_MAD, r1(0,1), r1(0,1), r0(i,i), c1(2,3)));
    m_instructions.push_back(PSInst(SH_PS_MAD, r1(0,1), r1(0,1), r0(i,i), c2(0,1)));
    m_instructions.push_back(PSInst(SH_PS_MAD, r1(0,1), r1(0,1), r0(i,i), c2(2,3)));
    m_instructions.push_back(PSInst(SH_PS_MAD, r1(0,1), r1(0,1), r0(i,i), c3(0,1)));
    m_instructions.push_back(PSInst(SH_PS_MAD, r1(0,1), r1(0,1), r0(i,i), c3(2,3)));
    m_instructions.push_back(PSInst(SH_PS_RSQ,offset(i), offset(i)));
    m_instructions.push_back(PSInst(SH_PS_RCP,offset(i), offset(i)));
    m_instructions.push_back(PSInst(SH_PS_MAD, r1(0,1), c4(1,0), offset(i,i), r1(0,1)));
    if (stmt.op == SH_OP_ACOS) {
      m_instructions.push_back(PSInst(SH_PS_DP3, output(i), r1(0,1,2), r2(0,1,2)));
    } else {
      m_instructions.push_back(PSInst(SH_PS_DP3, stmt.dest(i), r1(0,1,2), r2(0,1,2)));
    }
  }
  if (stmt.op == SH_OP_ACOS) {
    m_instructions.push_back(PSInst(SH_PS_ADD, stmt.dest, -output, c0(1,1,1,1)));
  }
}

void PSCode::emit_tan(const ShStatement& stmt)
{
  ShVariable tmp1(new ShVariableNode(SH_TEMP, stmt.src[0].size()));
  ShVariable tmp2(new ShVariableNode(SH_TEMP, stmt.src[0].size()));

  emit(ShStatement(tmp1, SH_OP_COS, stmt.src[0]));
  emit(ShStatement(tmp1, SH_OP_RCP, tmp1));
  emit(ShStatement(tmp2, SH_OP_SIN, stmt.src[0]));
  
  m_instructions.push_back(PSInst(SH_PS_MUL, stmt.dest, tmp1, tmp2));
}

void PSCode::emit_exp(const ShStatement& stmt)
{
  float basef = (stmt.op == SH_OP_EXP ? M_E : 10.0f);

  ShVariable base(new ShVariableNode(SH_CONST, 1));
  base.setValues(&basef);
  m_shader->constants.push_back(base.node());

  m_instructions.push_back(PSInst(SH_PS_POW, stmt.dest, base, stmt.src[0]));
}

void PSCode::emit_log(const ShStatement& stmt)
{
  float scalef = 1.0/log2((stmt.op == SH_OP_LOG ? M_E : 10.0f));

  ShVariable scale(new ShVariableNode(SH_CONST, 1));
  scale.setValues(&scalef);
  m_shader->constants.push_back(scale.node());

  ShVariable tmp(new ShVariableNode(SH_TEMP, stmt.dest.size()));
  
  emit(ShStatement(tmp, SH_OP_LOG2, stmt.src[0]));
  m_instructions.push_back(PSInst(SH_PS_MUL, stmt.dest, tmp, scale));
}

void PSCode::emit_no_output_norm(const ShStatement& stmt)
{
  // We need to do this for 3.0 shaders since we can't normalize to an output register
  if (stmt.dest.node()->kind() == SH_OUTPUT || stmt.dest.node()->kind() == SH_INOUT)
  {
    ShVariable tmp(new ShVariableNode(SH_TEMP, stmt.dest.node()->size()));
	m_instructions.push_back(PSInst(SH_PS_NRM, tmp, stmt.src[0]));
	m_instructions.push_back(PSInst(SH_PS_MOV, stmt.dest, tmp));
  }
  else
	m_instructions.push_back(PSInst(SH_PS_NRM, stmt.dest, stmt.src[0]));
}
void PSCode::emit_norm(const ShStatement& stmt)
{
  ShVariable tmp(new ShVariableNode(SH_TEMP, 1));
  emit(ShStatement(tmp, stmt.src[0], SH_OP_DOT, stmt.src[0]));
  m_instructions.push_back(PSInst(SH_PS_RSQ, tmp, tmp));
  m_instructions.push_back(PSInst(SH_PS_MUL, stmt.dest, tmp, stmt.src[0]));
}

void PSCode::emit_sgn(const ShStatement& stmt)
{
  ShVariable tmp(new ShVariableNode(SH_TEMP, stmt.src[0].size()));
  m_instructions.push_back(PSInst(SH_PS_ABS, tmp, stmt.src[0]));
  emit(ShStatement(stmt.dest, stmt.src[0], SH_OP_DIV, tmp));
}

void PSCode::emit_tex(const ShStatement& stmt)
{
/*  bool delay = false;
  ShVariable tmpdest;
  ShVariable tmpsrc;
  
  if (!stmt.dest.swizzle().identity()) {
    tmpdest = ShVariable(new ShVariableNode(SH_TEMP, 4));
    tmpsrc = tmpdest;
    delay = true;
  }

  ShTextureNodePtr tnode = shref_dynamic_cast<ShTextureNode>(stmt.src[0].node());

  SH_DEBUG_ASSERT(tnode);

  if (tnode->size() == 2) {
    // Special case for LUMINANCE_ALPHA
    if (!delay) {
      tmpdest = ShVariable(new ShVariableNode(SH_TEMP, 4));
      tmpsrc = tmpdest;
    }
    tmpsrc = tmpsrc(0,3);
    delay = true;
  }

  m_instructions.push_back(PSInst(SH_ARB_TEX,
                                   (delay ? tmpdest : stmt.dest), stmt.src[1], stmt.src[0]));
  if (delay) emit(ShStatement(stmt.dest, SH_OP_ASN, tmpsrc));*/
}

void PSCode::emit_nvcond(const ShStatement& stmt)
{

/*  ShVariable dummy(new ShVariableNode(SH_TEMP, stmt.src[0].size()));
  PSInst updatecc(SH_ARB_MOV, dummy, stmt.src[0]);
  updatecc.update_cc = true;
  m_instructions.push_back(updatecc);

  / *
  ShSwizzle ccswiz = stmt.src[0].swizzle();
  if (ccswiz.size() == 1) {
    int indices[4];
    for (int i = 0; i < stmt.dest.size(); i++) {
      indices[i] = 0;
    }
    ccswiz *= ShSwizzle(1, stmt.dest.size(), indices);
  }
  * /  
  if (stmt.dest != stmt.src[1]) {
    PSInst movt(SH_ARB_MOV, stmt.dest, stmt.src[1]);
    movt.ccode = PSInst::GT;
    movt.ccswiz = stmt.src[0].swizzle();
    m_instructions.push_back(movt);
  }
  if (stmt.dest != stmt.src[2]) {
    PSInst movf(SH_ARB_MOV, stmt.dest, stmt.src[2]);
    movf.ccode = PSInst::LE;
    movf.ccswiz = stmt.src[0].swizzle();
    m_instructions.push_back(movf);
  }*/
}

void PSCode::emit_csum(const ShStatement& stmt)
{
  ShVariableNode::ValueType* c1_values = new ShVariableNode::ValueType[stmt.src[0].size()];
  for (int i = 0; i < stmt.src[0].size(); i++) c1_values[i] = 1.0;
  ShVariable c1(new ShVariableNode(SH_CONST, stmt.src[0].size()));
  c1.setValues(c1_values);
  m_shader->constants.push_back(c1.node());
  
  emit(ShStatement(stmt.dest, stmt.src[0], SH_OP_DOT, c1));
}

void PSCode::emit_cmul(const ShStatement& stmt)
{
  ShVariable prod(new ShVariableNode(SH_TEMP, 1));

  // TODO: Could use vector mul here.
  
  m_instructions.push_back(PSInst(SH_PS_MOV, prod, stmt.src[0](0)));
  for (int i = 1; i < stmt.src[0].size(); i++) {
    m_instructions.push_back(PSInst(SH_PS_MUL, prod, stmt.src[0](i)));
  }
  m_instructions.push_back(PSInst(SH_PS_MOV, stmt.dest, prod));
}

/*void PSCode::emit_kil(const ShStatement& stmt)
{
  m_instructions.push_back(PSInst(SH_ARB_KIL, -stmt.src[0]));
}*/

}
