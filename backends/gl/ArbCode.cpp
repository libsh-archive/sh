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
#include "ArbCode.hpp"
#include <iostream>
#include <sstream>

#include "ShVariable.hpp"
#include "ShDebug.hpp"
#include "ShLinearAllocator.hpp"
#include "ShInternals.hpp"
#include "ShOptimizer.hpp"
#include "ShEnvironment.hpp"
#include "ShTextureNode.hpp"
#include "ShSyntax.hpp"
#include "ArbReg.hpp"
#include "Arb.hpp"

namespace shgl {

using namespace SH;

#define shGlProgramStringARB glProgramStringARB
#define shGlActiveTextureARB glActiveTextureARB
#define shGlProgramLocalParameter4fvARB glProgramLocalParameter4fvARB
#define shGlProgramEnvParameter4fvARB glProgramEnvParameter4fvARB
#define shGlGetProgramivARB glGetProgramivARB
#define shGlGenProgramsARB glGenProgramsARB
#define shGlDeleteProgramsARB glDeleteProgramsARB
#define shGlBindProgramARB glBindProgramARB

struct ArbBindingSpecs {
  ArbRegBinding binding;
  int maxBindings;
  ShSemanticType semanticType;
  bool allowGeneric;
};

ArbBindingSpecs arbVertexAttribBindingSpecs[] = {
  {SH_ARB_REG_VERTEXPOS, 1, SH_POSITION, false},
  {SH_ARB_REG_VERTEXNRM, 1, SH_NORMAL, false},
  {SH_ARB_REG_VERTEXCOL, 1, SH_COLOR, false},
  {SH_ARB_REG_VERTEXTEX, 8, SH_TEXCOORD, true},
  {SH_ARB_REG_VERTEXFOG, 1, SH_ATTRIB, true},
  {SH_ARB_REG_NONE, 0, SH_ATTRIB, true}
};

ArbBindingSpecs arbFragmentAttribBindingSpecs[] = {
  {SH_ARB_REG_FRAGMENTPOS, 1, SH_POSITION, false},
  {SH_ARB_REG_FRAGMENTCOL, 1, SH_COLOR, false},
  {SH_ARB_REG_FRAGMENTTEX, 8, SH_TEXCOORD, true},
  {SH_ARB_REG_FRAGMENTFOG, 1, SH_ATTRIB, true},
  {SH_ARB_REG_NONE, 0, SH_ATTRIB, true}
};

ArbBindingSpecs arbVertexOutputBindingSpecs[] = {
  {SH_ARB_REG_RESULTPOS, 1, SH_POSITION, false},
  {SH_ARB_REG_RESULTCOL, 1, SH_COLOR, false},
  {SH_ARB_REG_RESULTTEX, 8, SH_TEXCOORD, true},
  {SH_ARB_REG_RESULTFOG, 1, SH_ATTRIB, true},
  {SH_ARB_REG_RESULTPTS, 1, SH_ATTRIB, true},
  {SH_ARB_REG_NONE, 0, SH_ATTRIB}
};

ArbBindingSpecs arbFragmentOutputBindingSpecs[] = {
  {SH_ARB_REG_RESULTCOL, 1, SH_COLOR, true},
  {SH_ARB_REG_RESULTDPT, 1, SH_ATTRIB, false},
  {SH_ARB_REG_NONE, 0, SH_ATTRIB}
};

ArbBindingSpecs* arbBindingSpecs(bool output, const std::string& target)
{
  if (target == "gpu:vertex")
    return output ? arbVertexOutputBindingSpecs : arbVertexAttribBindingSpecs;
  if (target == "gpu:fragment")
    return output ? arbFragmentOutputBindingSpecs : arbFragmentAttribBindingSpecs;
  return 0;
}

using namespace SH;

ArbCode::ArbCode(const ShProgram& shader, const std::string& target,
                 TextureStrategy* textures)
  : m_textures(textures), m_shader(shader), m_originalShader(m_shader), m_target(target),
    m_numTemps(0), m_numInputs(0), m_numOutputs(0), m_numParams(0), m_numConsts(0),
    m_numTextures(0), m_programId(0)
{
}

ArbCode::~ArbCode()
{
}

void ArbCode::generate()
{
  // Transform code to be ARB_fragment_program compatible
  m_shader = cloneProgram(m_originalShader);
  ShEnvironment::insideShader = true;
  ShEnvironment::shader = m_shader;
  ShTransformer transform(m_shader);

  transform.convertInputOutput(); 
  transform.splitTuples(4, m_splits);
  transform.convertTextureLookups();
  
  if(transform.changed()) {
    ShOptimizer optimizer(m_shader->ctrlGraph);
    optimizer.optimize(ShEnvironment::optimizationLevel);
    m_shader->collectVariables();
  } else {
    m_shader = m_originalShader;
    ShEnvironment::shader = m_shader;
  }

  m_shader->ctrlGraph->entry()->clearMarked();
  genNode(m_shader->ctrlGraph->entry());
  m_shader->ctrlGraph->entry()->clearMarked();
  allocRegs();

  ShEnvironment::insideShader = false;
  ShEnvironment::shader = 0;
}

bool ArbCode::allocateRegister(const ShVariableNodePtr& var)
{
  if (!var) return true;
  if (var->kind() != SH_TEMP) return true;
  if (var->uniform()) return true;

  if (m_tempRegs.empty()) {
    SH_DEBUG_ERROR("Out of temporaries!");
    return false;
  }

  int idx = m_tempRegs.front();
  m_tempRegs.pop_front();
  if (idx + 1 > m_numTemps) m_numTemps = idx + 1;
  m_registers[var] = ArbReg(SH_ARB_REG_TEMP, idx);
  
  return true;
}

void ArbCode::freeRegister(const ShVariableNodePtr& var)
{
  if (!var) return;
  if (var->kind() != SH_TEMP) return;
  if (var->uniform()) return;

  SH_DEBUG_ASSERT(m_registers.find(var) != m_registers.end());
  m_tempRegs.push_front(m_registers[var].index);
}

void ArbCode::upload()
{
  if (!m_programId) {
    SH_GL_CHECK_ERROR(shGlGenProgramsARB(1, &m_programId));
  }

  SH_GL_CHECK_ERROR(shGlBindProgramARB(arbTarget(m_target), m_programId));
  
  std::ostringstream out;
  print(out);
  std::string text = out.str();
  shGlProgramStringARB(arbTarget(m_target), GL_PROGRAM_FORMAT_ASCII_ARB,
                       (GLsizei)text.size(), text.c_str());
  int error = glGetError();
  if (error == GL_INVALID_OPERATION) {
    int pos = -1;
    SH_GL_CHECK_ERROR(glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &pos));
    if (pos >= 0){
      const unsigned char* message = glGetString(GL_PROGRAM_ERROR_STRING_ARB);
      SH_DEBUG_WARN("Error at character " << pos);
      SH_DEBUG_WARN("Message: " << message);
      while (pos >= 0 && text[pos] != '\n') pos--;
      if (pos > 0) pos++;
      SH_DEBUG_WARN("Code: " << text.substr(pos, text.find('\n', pos)));
    }
  }
  if (error != GL_NO_ERROR) {
    SH_DEBUG_ERROR("Error uploading ARB program (" << m_target << "): " << error);
    SH_DEBUG_ERROR("shGlProgramStringARB(" << arbTarget(m_target)
                   << ", GL_PROGRAM_FORMAT_ASCII_ARB, " << (GLsizei)text.size() << 
                   ", <program text>);");
  }
}

void ArbCode::bind()
{
  if (!m_programId) {
    upload();
  }
  
  SH_GL_CHECK_ERROR(shGlBindProgramARB(arbTarget(m_target), m_programId));
  
  SH::ShEnvironment::boundShaders()[m_target] = m_originalShader; 

  // Initialize constants
  for (RegMap::const_iterator I = m_registers.begin(); I != m_registers.end(); ++I) {
    ShVariableNodePtr node = I->first;
    ArbReg reg = I->second;
    if (node->hasValues() && reg.type == SH_ARB_REG_PARAM) {
      updateUniform(node);
    }
  }
  // Make sure all textures are loaded.

  bindTextures();
}

void ArbCode::updateUniform(const ShVariableNodePtr& uniform)
{
  int i;

  if (!uniform) return;

  RegMap::const_iterator I = m_registers.find(uniform);
  if (I == m_registers.end()) { // perhaps uniform was split
    if( m_splits.count(uniform) > 0 ) {
      ShTransformer::VarNodeVec &splitVec = m_splits[uniform];
      int offset = 0;
      for(ShTransformer::VarNodeVec::iterator it = splitVec.begin();
          it != splitVec.end(); offset += (*it)->size(), ++it) {
        for(i = 0; i < (*it)->size(); ++i) {
          (*it)->setValue(i, uniform->getValue(i + offset));
        }
        updateUniform(*it);
      }
    } 
    return;
  }

  ShTextureNodePtr tex = shref_dynamic_cast<ShTextureNode>(uniform);
  if (tex) {
    return;
  }
    
  const ArbReg& reg = I->second;
  
  float values[4];
  for (i = 0; i < uniform->size(); i++) {
    values[i] = (float)uniform->getValue(i);
  }
  for (; i < 4; i++) {
    values[i] = 0.0;
  }
  
  if (reg.type != SH_ARB_REG_PARAM) return;
  switch(reg.binding) {
  case SH_ARB_REG_PARAMLOC:
    SH_GL_CHECK_ERROR(shGlProgramLocalParameter4fvARB(arbTarget(m_target), reg.bindingIndex, values));
    break;
  case SH_ARB_REG_PARAMENV:
    SH_GL_CHECK_ERROR(shGlProgramEnvParameter4fvARB(arbTarget(m_target), reg.bindingIndex, values));
    break;
  default:
    return;
  }
}

std::ostream& ArbCode::printVar(std::ostream& out, bool dest, const ShVariable& var,
                                bool collectingOp, const ShSwizzle& destSwiz = ShSwizzle(4)) const
{
  RegMap::const_iterator I = m_registers.find(var.node());
  if (I == m_registers.end()) {
    out << "<no reg for " << var.name() << ">";
    return out;
  }
  const ArbReg& reg = I->second;

  // Negation
  if (var.neg()) out << '-';

  // Register name
  out << reg;

  // Swizzling
  const char* swizChars = "xyzw";
  out << ".";
  if (dest || var.swizzle().size() == 1) {
    for (int i = 0; i < std::min(var.swizzle().size(), 4); i++) {
       out << swizChars[var.swizzle()[i]];
    }
  } else if (collectingOp) {
    for (int i = 0; i < 4; i++) {
       out << swizChars[i < var.swizzle().size() ? var.swizzle()[i] : i];
    }
  } else {
    for (int i = 0; i < 4; i++) {
      int j;
      for (j = 0; j < destSwiz.size(); j++) {
        if (destSwiz[j] == i) break;
      }
      if (j == destSwiz.size()) j = i;
      out << swizChars[j < var.size() ? var.swizzle()[j] : j];
    }
  }

  return out;
}

struct LineNumberer {
  LineNumberer() { line = 0; }
  int line;
};

std::ostream& operator<<(std::ostream& out, LineNumberer& l)
{
  out << " # " << ++l.line << std::endl;
  return out;
}

bool ArbCode::printSamplingInstruction(std::ostream& out, const ArbInst& instr) const
{
  if (instr.op != SH_ARB_TEX && instr.op != SH_ARB_TXP && instr.op != SH_ARB_TXB)
    return false;

  ShTextureNodePtr texture = shref_dynamic_cast<ShTextureNode>(instr.src[1].node());
  RegMap::const_iterator texRegIt = m_registers.find(instr.src[1].node());
  SH_DEBUG_ASSERT(texRegIt != m_registers.end());

  const ArbReg& texReg = texRegIt->second;
  
  out << "  ";
  out << arbOpInfo[instr.op].name << " ";
  printVar(out, true, instr.dest, false) << ", ";
  printVar(out, false, instr.src[0], true, instr.dest.swizzle()) << ", ";
  out << "texture[" << texReg.index << "], ";
  switch (texture->dims()) {
  case SH_TEXTURE_1D:
    out << "1D";
    break;
  case SH_TEXTURE_2D:
    out << "2D";
    break;
  case SH_TEXTURE_3D:
    out << "3D";
    break;
  case SH_TEXTURE_CUBE:
    out << "CUBE";
    break;
  case SH_TEXTURE_RECT:
    out << "RECT";
    break;
  }
  return true;
}

std::ostream& ArbCode::print(std::ostream& out)
{
  LineNumberer endl;

  // Print version header
  if (m_target == "gpu:vertex") out << "!!ARBvp1.0" << endl;
  if (m_target == "gpu:fragment") out << "!!ARBfp1.0" << endl;

  // Print register declarations
  
  for (ShProgramNode::VarList::const_iterator I = m_shader->inputs.begin();
       I != m_shader->inputs.end(); ++I) {
    out << "  ";
    m_registers[*I].printDecl(out);
    out << endl;
  }

  for (ShProgramNode::VarList::const_iterator I = m_shader->outputs.begin();
       I != m_shader->outputs.end(); ++I) {
    out << "  ";
    m_registers[*I].printDecl(out);
    out << endl;
  }

  for (ShProgramNode::VarList::const_iterator I = m_shader->uniforms.begin();
       I != m_shader->uniforms.end(); ++I) {
    out << "  ";
    m_registers[*I].printDecl(out);
    out << endl;
  }

  for (ShProgramNode::VarList::const_iterator I = m_shader->constants.begin();
       I != m_shader->constants.end(); ++I) {
    out << "  ";
    m_registers[*I].printDecl(out);
    out << endl;
  }

  if (m_numTemps) {
    out << "  TEMP ";
    for (int i = 0; i < m_numTemps; i++) {
      if (i > 0) out << ", ";
      out << ArbReg(SH_ARB_REG_TEMP, i);
    }
    out << ";" << endl;
  }

  out << endl;
  
  // Print instructions
  for (ArbInstList::const_iterator I = m_instructions.begin();
       I != m_instructions.end(); ++I) {
    if (!printSamplingInstruction(out, *I)) {
      out << "  ";
      out << arbOpInfo[I->op].name << " ";
      printVar(out, true, I->dest, arbOpInfo[I->op].collectingOp);
      for (int i = 0; i < arbOpInfo[I->op].arity; i++) {
        out << ", ";
        printVar(out, false, I->src[i], arbOpInfo[I->op].collectingOp, I->dest.swizzle());
      }
    }
    out << ';';
    out << endl;
  }

  out << "END" << endl;
  return out;
}

std::ostream& ArbCode::printInputOutputFormat(std::ostream& out) {
  ShProgramNode::VarList::const_iterator I;
  out << "Inputs:" << std::endl;
  for (I = m_shader->inputs.begin(); I != m_shader->inputs.end(); ++I) {
    out << "  ";
    m_registers[*I].printDecl(out);
    out << std::endl;
  }

  out << "Outputs:" << std::endl;
  for (I = m_shader->outputs.begin(); I != m_shader->outputs.end(); ++I) {
    out << "  ";
    m_registers[*I].printDecl(out);
    out << std::endl;
  }
  return out;
}

void ArbCode::genNode(ShCtrlGraphNodePtr node)
{
  if (!node || node->marked()) return;
  node->mark();

  if (node->block) for (ShBasicBlock::ShStmtList::const_iterator I = node->block->begin();
       I != node->block->end(); ++I) {
    const ShStatement& stmt = *I;
    switch (stmt.op) {
    case SH_OP_ASN:
      m_instructions.push_back(ArbInst(SH_ARB_MOV, stmt.dest, stmt.src[0]));
      break;
    case SH_OP_NEG:
      m_instructions.push_back(ArbInst(SH_ARB_MOV, stmt.dest, -stmt.src[0]));
      break;
    case SH_OP_ADD:
      m_instructions.push_back(ArbInst(SH_ARB_ADD, stmt.dest, stmt.src[0], stmt.src[1]));
      break;
    case SH_OP_MUL:
      genScalarVectorInst(stmt.dest, stmt.src[0], stmt.src[1], SH_ARB_MUL);
      break;
    case SH_OP_DIV:
      genDiv(stmt.dest, stmt.src[0], stmt.src[1]);
      break;
    case SH_OP_ABS:
      m_instructions.push_back(ArbInst(SH_ARB_ABS, stmt.dest, stmt.src[0]));
      break;
    case SH_OP_ACOS:
      {
        genTrigInst(stmt.dest, stmt.src[0], SH_OP_ACOS);
      }
      break;
    case SH_OP_ASIN:
      {
        genTrigInst(stmt.dest, stmt.src[0], SH_OP_ASIN);
      }
      break;
    case SH_OP_CEIL:
      {
        m_instructions.push_back(ArbInst(SH_ARB_FLR, stmt.dest, -stmt.src[0])); 
        m_instructions.push_back(ArbInst(SH_ARB_MOV, stmt.dest, -stmt.dest));
      }
      break;
    case SH_OP_COS:
      if( m_target == "gpu:vertex" ) 
      {
        genTrigInst(stmt.dest, stmt.src[0], SH_OP_COS);
      }
      else 
      {
        for (int i = 0; i < stmt.src[0].size(); i++)
        {
          m_instructions.push_back(
              ArbInst(SH_ARB_COS, stmt.dest(i), stmt.src[0](i)));
        }
      }
      break;
    case SH_OP_DOT: 
      genDot(stmt.dest, stmt.src[0], stmt.src[1]);
      break;
    case SH_OP_FLR:
      m_instructions.push_back(ArbInst(SH_ARB_FLR, stmt.dest, stmt.src[0]));
      break;
    case SH_OP_FMOD:
      {
        // TODO - is this really optimal?
        ShVariable t1(new ShVariableNode(SH_TEMP, stmt.src[0].size()));
        ShVariable t2(new ShVariableNode(SH_TEMP, stmt.src[0].size()));
        
        // result = x - sign(x/y)*floor(abs(x/y))*y
        genDiv(t1, stmt.src[0], stmt.src[1]);
        m_instructions.push_back(ArbInst(SH_ARB_ABS, t2, t1));

        genDiv(t1, t1, t2);
        m_instructions.push_back(ArbInst(SH_ARB_FLR, t2, t2)); 
        m_instructions.push_back(ArbInst(SH_ARB_MUL, t1, t1, t2)); 
        m_instructions.push_back(ArbInst(SH_ARB_MUL, t1, t1, stmt.src[1])); 
        m_instructions.push_back(ArbInst(SH_ARB_SUB, stmt.dest, stmt.src[0], t1)); 
      }
      break;
    case SH_OP_FRAC:
      m_instructions.push_back(ArbInst(SH_ARB_FRC, stmt.dest, stmt.src[0]));
      break;
    case SH_OP_LRP:
      if(m_target == "gpu:vertex") {
        ShVariable t(new ShVariableNode(SH_TEMP, stmt.src[1].size()));
        // lerp(f,a,b)=f*a + (1-f)*b = f*(a-b) + b 
        m_instructions.push_back(ArbInst(SH_ARB_ADD, t, stmt.src[1], -stmt.src[2]));

        if (stmt.src[0].size() == 1 && stmt.src[1].size() != 1) { 
          int* swizzle = new int[stmt.src[1].size()];
          for (int i = 0; i < stmt.src[1].size(); i++) swizzle[i] = 0;

          m_instructions.push_back(ArbInst(SH_ARB_MAD, stmt.dest, 
                stmt.src[0](stmt.src[1].size(), swizzle), t, stmt.src[2]));
          delete [] swizzle;
        } else {
          m_instructions.push_back(ArbInst(SH_ARB_MAD, stmt.dest, 
                stmt.src[0], t, stmt.src[2]));
        }
      } else {
        if(stmt.src[0].size() == 1 && stmt.src[1].size() != 1) {
          int* swizzle = new int[stmt.src[1].size()];
          for (int i = 0; i < stmt.src[1].size(); i++) swizzle[i] = 0;

          m_instructions.push_back(ArbInst(SH_ARB_LRP, stmt.dest,
                stmt.src[0](stmt.src[1].size(), swizzle), stmt.src[1], stmt.src[2]));
          delete [] swizzle;
        } else {
          m_instructions.push_back(ArbInst(SH_ARB_LRP, stmt.dest,
                stmt.src[0], stmt.src[1], stmt.src[2]));
        }
      }
      break;
    case SH_OP_MAD:
      if (stmt.src[0].size() != 1 || stmt.src[1].size() != 1) {
        if (stmt.src[0].size() == 1) {
          int* swizzle = new int[stmt.src[1].size()];
          for (int i = 0; i < stmt.src[1].size(); i++) swizzle[i] = 0; 
          m_instructions.push_back(ArbInst(SH_ARB_MAD, stmt.dest, 
                stmt.src[0](stmt.src[1].size(), swizzle), stmt.src[1], stmt.src[2]));
          delete [] swizzle;
          break;
        } else if (stmt.src[1].size() == 1) {
          int* swizzle = new int[stmt.src[0].size()];
          for (int i = 0; i < stmt.src[0].size(); i++) swizzle[i] = 0;
          m_instructions.push_back(ArbInst(SH_ARB_MAD, stmt.dest, stmt.src[0],
                stmt.src[1](stmt.src[0].size(), swizzle), stmt.src[2]));
          delete [] swizzle;
          break;
        }
      } 
      m_instructions.push_back(ArbInst(SH_ARB_MAD, stmt.dest, stmt.src[0], 
            stmt.src[1], stmt.src[2])); 
      break;

    case SH_OP_MAX:
      m_instructions.push_back(ArbInst(SH_ARB_MAX, stmt.dest, stmt.src[0], stmt.src[1]));
      break;
    case SH_OP_MIN:
      m_instructions.push_back(ArbInst(SH_ARB_MIN, stmt.dest, stmt.src[0], stmt.src[1]));
      break;
    case SH_OP_NORM:
      { 
        ShVariable mul(new ShVariableNode(SH_TEMP, 1));
        genDot(mul, stmt.src[0], stmt.src[0]);
        m_instructions.push_back(ArbInst(SH_ARB_RSQ, mul, mul));
        m_instructions.push_back(ArbInst(SH_ARB_MUL, stmt.dest, mul, stmt.src[0]));
      }
      break;
    case SH_OP_SIN:
      if( m_target == "gpu:vertex" ) 
      {
        genTrigInst(stmt.dest, stmt.src[0], SH_OP_SIN);
      }
      else 
      {
        for (int i = 0; i < stmt.src[0].size(); i++)
        {
          m_instructions.push_back(
              ArbInst(SH_ARB_SIN, stmt.dest(i), stmt.src[0](i)));
        }
      }
      break;
    case SH_OP_SEQ:
      {
        int tempsize = stmt.src[0].size();
        if( tempsize < stmt.src[1].size() ) tempsize = stmt.src[1].size();
        ShVariable seq(new ShVariableNode(SH_TEMP, tempsize)); 
        ShVariable seq2(new ShVariableNode(SH_TEMP, tempsize)); 
        genScalarVectorInst(seq, stmt.src[0], stmt.src[1], SH_ARB_SGE);
        genScalarVectorInst(seq2, stmt.src[1], stmt.src[0], SH_ARB_SGE);
        m_instructions.push_back(ArbInst(SH_ARB_MUL, stmt.dest, seq, seq2 ));
      }
      break;
    case SH_OP_SLT:
      genScalarVectorInst(stmt.dest, stmt.src[0], stmt.src[1], SH_ARB_SLT);
      break;
    case SH_OP_SGT:
      genScalarVectorInst(stmt.dest, stmt.src[1], stmt.src[0], SH_ARB_SLT);
      break;
    case SH_OP_SLE:
      genScalarVectorInst(stmt.dest, stmt.src[1], stmt.src[0], SH_ARB_SGE);
      break;
    case SH_OP_SGE:
      genScalarVectorInst(stmt.dest, stmt.src[0], stmt.src[1], SH_ARB_SGE);
      break;
    case SH_OP_SQRT:
      {
        ShVariable rcp(new ShVariableNode(SH_TEMP, stmt.src[0].size()));
        m_instructions.push_back(ArbInst(SH_ARB_RSQ, rcp, stmt.src[0]));
        m_instructions.push_back(ArbInst(SH_ARB_RCP, stmt.dest, rcp));
      break;
      }
    case SH_OP_POW:
      // TODO: scalarize?
      m_instructions.push_back(ArbInst(SH_ARB_POW, stmt.dest, stmt.src[0], stmt.src[1]));
      break;
    case SH_OP_TEX:
    case SH_OP_TEXI:
      m_instructions.push_back(ArbInst(SH_ARB_TEX, stmt.dest, stmt.src[1], stmt.src[0]));
      break;
    case SH_OP_XPD:
      m_instructions.push_back(ArbInst(SH_ARB_XPD, stmt.dest, stmt.src[0], stmt.src[1]));
      break;
    case SH_OP_COND:
      if (stmt.src[0].size() == 1 && stmt.src[1].size() != 1) {
        int* swizzle = new int[stmt.src[1].size()];
        for (int i = 0; i < stmt.src[1].size(); i++) swizzle[i] = 0;
        m_instructions.push_back(ArbInst(SH_ARB_CMP, stmt.dest, -stmt.src[0](stmt.src[1].size(), swizzle),
                                         stmt.src[1], stmt.src[2]));
        delete [] swizzle;
      } else {
        m_instructions.push_back(ArbInst(SH_ARB_CMP, stmt.dest, -stmt.src[0], stmt.src[1], stmt.src[2]));
      }
      break;
    case SH_OP_KIL:
      m_instructions.push_back(ArbInst(SH_ARB_KIL, -stmt.src[0]));
      break;
    default:
      SH_DEBUG_WARN(opInfo[stmt.op].name << " not handled by ARB backend");
    }
  }

  genNode(node->follower);
}

void ArbCode::genDiv(const ShVariable &dest, const ShVariable &op1, const ShVariable &op2) {
  if (op2.size() == 1 && op1.size() != 1) {
    ShVariable rcp(new ShVariableNode(SH_TEMP, op2.size())); 
    m_instructions.push_back(ArbInst(SH_ARB_RCP, rcp, op2)); 

    int* swizzle = new int[op1.size()];
    for (int i = 0; i < op1.size(); i++) swizzle[i] = 0;
    m_instructions.push_back(ArbInst(SH_ARB_MUL, dest, op1,
          rcp(op1.size(), swizzle)));
    delete [] swizzle;
  } else {
    ShVariable rcp(new ShVariableNode(SH_TEMP, 1));

    // TODO arg...component-wise div is ugly, ARB RCP only works on scalars
    for(int i = 0; i < op2.size(); ++i) {
      m_instructions.push_back(ArbInst(SH_ARB_RCP, rcp, op2(i)));
      m_instructions.push_back(ArbInst(SH_ARB_MUL, dest(i), op1(i), rcp));
    }
  }
}

void ArbCode::genScalarVectorInst( const SH::ShVariable &dest, const SH::ShVariable &op1, 
    const SH::ShVariable &op2, int opcode ) {
  if (op1.size() != 1 || op2.size() != 1) {
    if (op1.size() == 1) {
      int* swizzle = new int[op2.size()];
      for (int i = 0; i < op2.size(); i++) swizzle[i] = 0; 
      m_instructions.push_back(ArbInst((ArbOp)opcode, dest, 
            op1(op2.size(), swizzle), op2));
      delete [] swizzle;
      return;
    } else if (op2.size() == 1) {
      int* swizzle = new int[op1.size()];
      for (int i = 0; i < op1.size(); i++) swizzle[i] = 0;
      m_instructions.push_back(ArbInst((ArbOp)opcode, dest, op1,
                                             op2(op1.size(), swizzle)));
      delete [] swizzle;
      return;
    }
  } 
  m_instructions.push_back(ArbInst((ArbOp)opcode, dest, op1, op2)); 
}

void ArbCode::genTrigInst( const SH::ShVariable& dest, 
    const SH::ShVariable& src, int opcode  )
{
  if (opcode == SH_OP_ASIN || opcode == SH_OP_ACOS) 
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
    ShVariable rs(new ShVariableNode(SH_TEMP, 4));
    ShVariable offset(new ShVariableNode(SH_TEMP, 4));
    m_instructions.push_back(ArbInst(SH_ARB_MOV, rs, src));
    m_instructions.push_back(ArbInst(SH_ARB_ABS, r0, rs));
    m_instructions.push_back(
        ArbInst(SH_ARB_MAD, offset, -r0, r0, c4(0,0,0,0)));
    for (int i = 0; i < src.size(); i++)
    {
      m_instructions.push_back(ArbInst(SH_ARB_MOV, r2, c0(0,1,0,1)));
      m_instructions.push_back(ArbInst(SH_ARB_SLT, r2(1), rs(i), c0(0)));
      m_instructions.push_back(ArbInst(SH_ARB_SGE, r2(0), rs(i), c0(0)));
      m_instructions.push_back(
          ArbInst(SH_ARB_MAD, r1(0,1), c0(2,3), r0(i,i), c1(0,1)));
      m_instructions.push_back(
          ArbInst(SH_ARB_MAD, r1(0,1), r1(0,1), r0(i,i), c1(2,3)));
      m_instructions.push_back(
          ArbInst(SH_ARB_MAD, r1(0,1), r1(0,1), r0(i,i), c2(0,1)));
      m_instructions.push_back(
          ArbInst(SH_ARB_MAD, r1(0,1), r1(0,1), r0(i,i), c2(2,3)));
      m_instructions.push_back(
          ArbInst(SH_ARB_MAD, r1(0,1), r1(0,1), r0(i,i), c3(0,1)));
      m_instructions.push_back(
          ArbInst(SH_ARB_MAD, r1(0,1), r1(0,1), r0(i,i), c3(2,3)));
      m_instructions.push_back(ArbInst(SH_ARB_RSQ,offset(i), offset(i)));
      m_instructions.push_back(ArbInst(SH_ARB_RCP,offset(i), offset(i)));
      m_instructions.push_back(
          ArbInst(SH_ARB_MAD, r1(0,1), c4(1,0), offset(i,i), r1(0,1)));
      m_instructions.push_back(
          ArbInst(SH_ARB_DP3, dest(i), r1(0,1,2), r2(0,1,2)));
    }
    if (opcode == SH_OP_ACOS)
    {
      m_instructions.push_back(
          ArbInst(SH_ARB_ADD, dest, -dest, c0(1,1,1,1)));
    }
  }
  else if (opcode == SH_OP_SIN || opcode == SH_OP_COS)
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
    m_instructions.push_back(ArbInst(SH_ARB_MOV, rs, src));
    if (opcode == SH_OP_SIN)
    {
      m_instructions.push_back(
          ArbInst(SH_ARB_MAD, rs, c1(3,3,3,3), rs, -c1(0,0,0,0)));
    }
    else 
    {
      m_instructions.push_back(ArbInst(SH_ARB_MUL, rs, c1(3,3,3,3), rs));
    }
    m_instructions.push_back(ArbInst(SH_ARB_FRC, rs, rs));
    for (int i = 0; i < src.size(); i++)
    {
      m_instructions.push_back(ArbInst(SH_ARB_SLT, r2(0), rs(i), c1(0)));
      m_instructions.push_back(
          ArbInst(SH_ARB_SGE, r2(1,2), rs(i,i), c1(1,2)));
      m_instructions.push_back(
          ArbInst(SH_ARB_DP3, r2(1), r2(0,1,2), c4(2,3,2)));
      m_instructions.push_back(
          ArbInst(SH_ARB_ADD, r0(0,1,2), -rs(i,i,i), c0(0,1,2)));
      m_instructions.push_back(ArbInst(SH_ARB_MUL, r0, r0, r0));
      m_instructions.push_back(
          ArbInst(SH_ARB_MAD, r1, c2(0,1,0,1), r0, c2(2,3,2,3)));
      m_instructions.push_back(
          ArbInst(SH_ARB_MAD, r1, r1, r0, c3(0,1,0,1)));
      m_instructions.push_back(
          ArbInst(SH_ARB_MAD, r1, r1, r0, c3(2,3,2,3)));
      m_instructions.push_back(
          ArbInst(SH_ARB_MAD, r1, r1, r0, c4(0,1,0,1)));
      m_instructions.push_back(
          ArbInst(SH_ARB_MAD, r1, r1, r0, c4(2,3,2,3)));
      m_instructions.push_back(
          ArbInst(SH_ARB_DP3, r0(0), r1(0,1,2), -r2(0,1,2)));
      m_instructions.push_back(ArbInst(SH_ARB_MOV, dest(i), r0(0)));
    }
  }
}

void ArbCode::genDot( const ShVariable& dest, 
    const ShVariable& src0, const ShVariable &src1) { 
  ShVariable left = src0;
  ShVariable right = src1;

  // expand src0/src1 if they are scalar
  if( src0.size() < src1.size() ) {
    int *swizzle = new int[ src1.size() ];
    for( int i = 0; i < src1.size(); ++i ) swizzle[i] = 0; 
    left = src0( src1.size(), swizzle ); 
    delete swizzle;
  } else if( src1.size() < src0.size() ) {
    int *swizzle = new int[ src0.size() ];
    for( int i = 0; i < src0.size(); ++i ) swizzle[i] = 0; 
    right = src1( src0.size(), swizzle ); 
    delete swizzle;
  }

  if (left.size() == 3) {
    m_instructions.push_back(ArbInst(SH_ARB_DP3, dest, left, right));
  } else if (left.size() == 4) {
    m_instructions.push_back(ArbInst(SH_ARB_DP4, dest, left, right));
  } else if (left.size() == 1) {
    m_instructions.push_back(ArbInst(SH_ARB_MUL, dest, left, right));
  } else {
    ShVariable mul(new ShVariableNode(SH_TEMP, left.size()));
    m_instructions.push_back(ArbInst(SH_ARB_MUL, mul, left, right));
    m_instructions.push_back(ArbInst(SH_ARB_ADD, dest, mul(0), mul(1)));
    for (int i = 2; i < left.size(); i++) {
      m_instructions.push_back(ArbInst(SH_ARB_ADD, dest, dest, mul(i)));
    }
  }
}


void ArbCode::allocRegs()
{
  ArbLimits limits(m_target);
  
  allocInputs(limits);
  
  allocOutputs(limits);
  
  for (ShProgramNode::VarList::const_iterator I = m_shader->uniforms.begin();
       I != m_shader->uniforms.end(); ++I) {
    allocParam(limits, *I);
  }

  allocConsts(limits);
  
  allocTemps(limits);

  allocTextures(limits);
}

void ArbCode::bindSpecial(const ShProgramNode::VarList::const_iterator& begin,
                          const ShProgramNode::VarList::const_iterator& end,
                          const ArbBindingSpecs& specs, 
                          std::vector<int>& bindings,
                          ArbRegType type, int& num)
{
  bindings.push_back(0);
  
  if (specs.semanticType == SH_ATTRIB) return;
  
  for (ShProgramNode::VarList::const_iterator I = begin; I != end; ++I) {
    ShVariableNodePtr node = *I;
    
    if (m_registers.find(node) != m_registers.end()) continue;
    if (node->specialType() != specs.semanticType) continue;
    
    m_registers[node] = ArbReg(type, num++, node->name());
    m_registers[node].binding = specs.binding;
    m_registers[node].bindingIndex = bindings.back();
    
    bindings.back()++;
    if (bindings.back() == specs.maxBindings) break;
  }    
}

void ArbCode::allocInputs(const ArbLimits& limits)
{
  // First, try to assign some "special" output register bindings
  for (int i = 0; arbBindingSpecs(false, m_target)[i].binding != SH_ARB_REG_NONE; i++) {
    bindSpecial(m_shader->inputs.begin(), m_shader->inputs.end(),
                arbBindingSpecs(false, m_target)[i], m_inputBindings,
                SH_ARB_REG_ATTRIB, m_numInputs);
  }
  
  for (ShProgramNode::VarList::const_iterator I = m_shader->inputs.begin();
       I != m_shader->inputs.end(); ++I) {
    ShVariableNodePtr node = *I;
    if (m_registers.find(node) != m_registers.end()) continue;
    m_registers[node] = ArbReg(SH_ARB_REG_ATTRIB, m_numInputs++, node->name());

    // Binding
    for (int i = 0; arbBindingSpecs(false, m_target)[i].binding != SH_ARB_REG_NONE; i++) {
      const ArbBindingSpecs& specs = arbBindingSpecs(false, m_target)[i];

      if (specs.allowGeneric && m_inputBindings[i] < specs.maxBindings) {
        m_registers[node].binding = specs.binding;
        m_registers[node].bindingIndex = m_inputBindings[i];
        m_inputBindings[i]++;
        break;
      }
    }
  }
}

void ArbCode::allocOutputs(const ArbLimits& limits)
{
  // First, try to assign some "special" output register bindings
  for (int i = 0; arbBindingSpecs(true, m_target)[i].binding != SH_ARB_REG_NONE; i++) {
    bindSpecial(m_shader->outputs.begin(), m_shader->outputs.end(),
                arbBindingSpecs(true, m_target)[i], m_outputBindings,
                SH_ARB_REG_OUTPUT, m_numOutputs);
  }
  
  for (ShProgramNode::VarList::const_iterator I = m_shader->outputs.begin();
       I != m_shader->outputs.end(); ++I) {
    ShVariableNodePtr node = *I;
    if (m_registers.find(node) != m_registers.end()) continue;
    m_registers[node] = ArbReg(SH_ARB_REG_OUTPUT, m_numOutputs++, node->name());

    // Binding
    for (int i = 0; arbBindingSpecs(true, m_target)[i].binding != SH_ARB_REG_NONE; i++) {
      const ArbBindingSpecs& specs = arbBindingSpecs(true, m_target)[i];

      if (specs.allowGeneric && m_outputBindings[i] < specs.maxBindings) {
        m_registers[node].binding = specs.binding;
        m_registers[node].bindingIndex = m_outputBindings[i];
        m_outputBindings[i]++;
        break;
      }
    }
  }
}

void ArbCode::allocParam(const ArbLimits& limits, const ShVariableNodePtr& node)
{
  // TODO: Check if we reached maximum
  if (m_registers.find(node) != m_registers.end()) return;
  m_registers[node] = ArbReg(SH_ARB_REG_PARAM, m_numParams, node->name());
  m_registers[node].binding = SH_ARB_REG_PARAMLOC;
  m_registers[node].bindingIndex = m_numParams;
  m_numParams++;
}

void ArbCode::allocConsts(const ArbLimits& limits)
{
  for (ShProgramNode::VarList::const_iterator I = m_shader->constants.begin();
       I != m_shader->constants.end(); ++I) {
    ShVariableNodePtr node = *I;
    m_registers[node] = ArbReg(SH_ARB_REG_CONST, m_numConsts, node->name());
    for (int i = 0; i < 4; i++) {
      m_registers[node].values[i] = (float)(i < node->size() ? node->getValue(i) : 0.0);
    }
    m_numConsts++;
  }
}

void mark(ShLinearAllocator& allocator, ShVariableNodePtr node, int i)
{
  if (!node) return;
  if (node->kind() != SH_TEMP) return;
  if (node->hasValues()) return;
  allocator.mark(node, i);
}

void ArbCode::allocTemps(const ArbLimits& limits)
{
  ShLinearAllocator allocator(this);

  for (std::size_t i = 0; i < m_instructions.size(); i++) {
    ArbInst instr = m_instructions[i];
    mark(allocator, instr.dest.node(), (int)i);
    for (int j = 0; j < 3; j++) {
      mark(allocator, instr.src[j].node(), (int)i);
    }
  }
  
  m_tempRegs.clear();
  m_numTemps = 0;
  for (int i = 0; i < limits.temps(); i++) {
    m_tempRegs.push_back(i);
  }
  
  allocator.allocate();
  
  m_tempRegs.clear();
}

void ArbCode::allocTextures(const ArbLimits& limits)
{
  for (ShProgramNode::TexList::const_iterator I = m_shader->textures.begin();
       I != m_shader->textures.end(); ++I) {
    ShTextureNodePtr node = *I;
    int index;
    index = m_numTextures;
    m_registers[node] = ArbReg(SH_ARB_REG_TEXTURE, index, node->name());
    m_numTextures++;
  }
}

void ArbCode::bindTextures()
{
  for (ShProgramNode::TexList::const_iterator I = m_shader->textures.begin();
       I != m_shader->textures.end(); ++I) {
    m_textures->bindTexture(*I, GL_TEXTURE0 + m_registers[*I].index);
  }
}


// #ifdef WIN32
//   DWORD err;
//   if (!(glProgramStringARB =
//         (PFNGLPROGRAMSTRINGARBPROC)wglGetProcAddress("glProgramStringARB")))
//     err = GetLastError();
//   if (!(glBindProgramARB = 
//         (PFNGLBINDPROGRAMARBPROC)wglGetProcAddress("glBindProgramARB")))
//     err = GetLastError();
//   if (!(glGenProgramsARB = 
//         (PFNGLGENPROGRAMSARBPROC)wglGetProcAddress("glGenProgramsARB")))
//     err = GetLastError();
//   if (!(glActiveTextureARB = 
//         (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB")))
//     err = GetLastError();
//   if (!(glTexImage3D = 
//         (PFNGLTEXIMAGE3DPROC)wglGetProcAddress("glTexImage3D")))
//     err = GetLastError();
//   if (!(glProgramEnvParameter4fvARB = 
//         (PFNGLPROGRAMENVPARAMETER4FVARBPROC)wglGetProcAddress("glProgramEnvParameter4fvARB")))
//     err = GetLastError();
//   if (!(glProgramLocalParameter4fvARB = 
//         (PFNGLPROGRAMLOCALPARAMETER4FVARBPROC)wglGetProcAddress("glProgramLocalParameter4fvARB")))
//     err = GetLastError();
//   if (!(glGetProgramivARB = 
//         (PFNGLGETPROGRAMIVARBPROC)wglGetProcAddress("glGetProgramivARB")))
//     err = GetLastError();
// #endif /* WIN32 */

}
