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
#include <cmath>
#include <bitset>

#include "ShVariable.hpp"
#include "ShDebug.hpp"
#include "ShLinearAllocator.hpp"
#include "ShInternals.hpp"
#include "ShOptimizations.hpp"
#include "ShEnvironment.hpp"
#include "ShContext.hpp"
#include "ShTextureNode.hpp"
#include "ShSyntax.hpp"
#include "ArbReg.hpp"
#include "Arb.hpp"
#include "ShAttrib.hpp"
#include "ShError.hpp"

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

ArbBindingSpecs* arbBindingSpecs(bool output, const std::string& unit)
{
  if (unit == "vertex")
    return output ? arbVertexOutputBindingSpecs : arbVertexAttribBindingSpecs;
  if (unit == "fragment")
    return output ? arbFragmentOutputBindingSpecs : arbFragmentAttribBindingSpecs;
  return 0;
}

using namespace SH;

ArbCode::ArbCode(const ShProgramNodeCPtr& shader, const std::string& unit,
                 TextureStrategy* textures)
  : m_textures(textures), m_shader(0), m_originalShader(shader), m_unit(unit),
    m_numTemps(0), m_numInputs(0), m_numOutputs(0), m_numParams(0), m_numConsts(0),
    m_numTextures(0), m_programId(0), m_environment(0), m_max_label(0)
{
  if (unit == "fragment") m_environment |= SH_ARB_FP;
  if (unit == "vertex") m_environment |= SH_ARB_VP;

  const GLubyte* extensions = glGetString(GL_EXTENSIONS);

  std::string extstr(reinterpret_cast<const char*>(extensions));

  if (unit == "fragment") {
    if (extstr.find("NV_fragment_program_option") != std::string::npos) {
      m_environment |= SH_ARB_NVFP;
    }
    if (extstr.find("NV_fragment_program2") != std::string::npos) {
      m_environment |= SH_ARB_NVFP2;
    }
    if (extstr.find("ATI_draw_buffers") != std::string::npos) {
      m_environment |= SH_ARB_ATIDB;
    }
  }
  if (unit == "vertex") {
    if (extstr.find("NV_vertex_program2_option") != std::string::npos) {
      m_environment |= SH_ARB_NVVP2;
    }
    if (extstr.find("NV_vertex_program3") != std::string::npos) {
      m_environment |= SH_ARB_NVVP3;
    }
  }
}

ArbCode::~ArbCode()
{
}

void ArbCode::generate()
{
  // Transform code to be ARB_fragment_program compatible
  m_shader = m_originalShader->clone();
  ShContext::current()->enter(m_shader);
  ShTransformer transform(m_shader);

  transform.convertInputOutput(); 
  transform.splitTuples(4, m_splits);
  transform.convertTextureLookups();
  
  if(transform.changed()) {
    optimize(m_shader);
    m_shader->collectVariables();
  } else {
    m_shader = shref_const_cast<ShProgramNode>(m_originalShader);
    ShContext::current()->exit();
    ShContext::current()->enter(m_shader);
  }

  if (m_environment & SH_ARB_NVFP2) {
    // In NV_fragment_program2, we actually generate structured code.
    ShStructural str(m_shader->ctrlGraph);

    genStructNode(str.head());
    
  } else {
    m_shader->ctrlGraph->entry()->clearMarked();
    genNode(m_shader->ctrlGraph->entry());
    
    if (m_environment & SH_ARB_NVVP2) {
      m_instructions.push_back(ArbInst(SH_ARB_LABEL, getLabel(m_shader->ctrlGraph->exit())));
    }
  }
  m_shader->ctrlGraph->entry()->clearMarked();
  allocRegs();
  
  ShContext::current()->exit();
}

bool ArbCode::allocateRegister(const ShVariableNodePtr& var)
{
  if (!var) return true;
  if (var->kind() != SH_TEMP) return true;
  if (var->uniform()) return true;

  if (m_tempRegs.empty()) {
    shError(ShException("ARB Backend: Out of registers"));
    return false;
  }

  int idx = m_tempRegs.front();
  m_tempRegs.pop_front();
  if (idx + 1 > m_numTemps) m_numTemps = idx + 1;
  m_registers[var] = new ArbReg(SH_ARB_REG_TEMP, idx);
  m_reglist.push_back(m_registers[var]);
  
  return true;
}

void ArbCode::freeRegister(const ShVariableNodePtr& var)
{
  if (!var) return;
  if (var->kind() != SH_TEMP) return;
  if (var->uniform()) return;

  SH_DEBUG_ASSERT(m_registers.find(var) != m_registers.end());
  m_tempRegs.push_front(m_registers[var]->index);
}

void ArbCode::upload()
{
  if (!m_programId) {
    SH_GL_CHECK_ERROR(shGlGenProgramsARB(1, &m_programId));
  }

  SH_GL_CHECK_ERROR(shGlBindProgramARB(arbTarget(m_unit), m_programId));
  
  std::ostringstream out;
  print(out);
  std::string text = out.str();
  shGlProgramStringARB(arbTarget(m_unit), GL_PROGRAM_FORMAT_ASCII_ARB,
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
    SH_DEBUG_ERROR("Error uploading ARB program (" << m_unit << "): " << error);
    SH_DEBUG_ERROR("shGlProgramStringARB(" << arbTarget(m_unit)
                   << ", GL_PROGRAM_FORMAT_ASCII_ARB, " << (GLsizei)text.size() << 
                   ", <program text>);");
  }
}

void ArbCode::bind()
{
  if (!m_programId) {
    upload();
  }
  
  SH_GL_CHECK_ERROR(shGlBindProgramARB(arbTarget(m_unit), m_programId));
  

  ShContext::current()->set_binding(std::string("arb:") + m_unit,
                                    shref_const_cast<ShProgramNode>(m_originalShader));

  // Initialize constants
  for (RegMap::const_iterator I = m_registers.begin(); I != m_registers.end(); ++I) {
    ShVariableNodePtr node = I->first;
    ArbReg reg = *I->second;
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
    
  const ArbReg& reg = *I->second;
  
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
    SH_GL_CHECK_ERROR(shGlProgramLocalParameter4fvARB(arbTarget(m_unit), reg.bindingIndex, values));
    break;
  case SH_ARB_REG_PARAMENV:
    SH_GL_CHECK_ERROR(shGlProgramEnvParameter4fvARB(arbTarget(m_unit), reg.bindingIndex, values));
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
  const ArbReg& reg = *I->second;

  // Negation
  if (var.neg()) out << '-';

  // Register name
  out << reg;

  // Swizzling
  const char* swizChars = "xyzw";
  out << ".";
  if (dest) {
    bool masked[4] = {false, false, false, false};
    for (int i = 0; i < var.swizzle().size(); i++) {
      masked[var.swizzle()[i]] = true;
    }
    for (int i = 0; i < 4; i++) {
      if (masked[i]) out << swizChars[i];
    }
  } else if (var.swizzle().size() == 1) {
    out << swizChars[var.swizzle()[0]];
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
  if (instr.op != SH_ARB_TEX && instr.op != SH_ARB_TXP && instr.op != SH_ARB_TXB
      && instr.op != SH_ARB_TXD)
    return false;

  ShTextureNodePtr texture = shref_dynamic_cast<ShTextureNode>(instr.src[1].node());
  RegMap::const_iterator texRegIt = m_registers.find(instr.src[1].node());
  if (texRegIt == m_registers.end()) {
    SH_DEBUG_PRINT("Unallocated texture found.");
    SH_DEBUG_PRINT("Operation = " << arbOpInfo[instr.op].name);
    SH_DEBUG_PRINT("Destination* = " << instr.dest.node().object());
    if (instr.dest.node()) {
      SH_DEBUG_PRINT("Destination = " << instr.dest.name());
    }
    SH_DEBUG_PRINT("Texture pointer = " << texture.object());
    if (texture) {
      SH_DEBUG_PRINT("Texture = " << texture->name());
    }
    out << "  INVALID TEX INSTRUCTION;";
    return true;
  }
  //SH_DEBUG_ASSERT(texRegIt != m_registers.end());

  const ArbReg& texReg = *texRegIt->second;
  
  out << "  ";
  out << arbOpInfo[instr.op].name << " ";
  printVar(out, true, instr.dest, false) << ", ";
  printVar(out, false, instr.src[0], true, instr.dest.swizzle()) << ", ";
  if (instr.op == SH_ARB_TXD) {
    printVar(out, false, instr.src[2], true, instr.dest.swizzle()) << ", ";
    printVar(out, false, instr.src[3], true, instr.dest.swizzle()) << ", ";
  }
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
  out << ";";
  return true;
}

std::ostream& ArbCode::print(std::ostream& out)
{
  LineNumberer endl;
  const char* swizChars = "xyzw";

  // Print version header
  if (m_unit == "vertex") {
    out << "!!ARBvp1.0" << endl;
    if (m_environment & SH_ARB_NVVP3) out << "OPTION NV_vertex_program3;" << endl;
    else if (m_environment & SH_ARB_NVVP2) out << "OPTION NV_vertex_program2;" << endl;
  }
  if (m_unit == "fragment") {
    out << "!!ARBfp1.0" << endl;

    if (m_environment & SH_ARB_NVFP2) out << "OPTION NV_fragment_program2;" << endl;
    else if (m_environment & SH_ARB_NVFP) out << "OPTION NV_fragment_program;" << endl;

    if (m_environment & SH_ARB_ATIDB) out << "OPTION ATI_draw_buffers;" << endl;
  }
  
  // Print register declarations
  
  for (RegList::const_iterator I = m_reglist.begin();
       I != m_reglist.end(); ++I) {
    if ((*I)->type == SH_ARB_REG_TEMP) continue;
    if ((*I)->type == SH_ARB_REG_TEXTURE) continue;
    out << "  ";
    (*I)->printDecl(out);
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
    if (I->op == SH_ARB_LABEL) {
      out << "label" << I->label << ": ";
    } else if (I->op == SH_ARB_ELSE) {
      out << "  ELSE;";
    } else if (I->op == SH_ARB_ENDIF) {
      out << "  ENDIF;";
    } else if (I->op == SH_ARB_BRA) {
      if (I->src[0].node()) {
        out << "  MOVC ";
        printVar(out, true, I->src[0], false);
        out << ", ";
        printVar(out, false, I->src[0], false, I->src[0].swizzle());
        out << ";" << endl;
      }
      out << "  BRA label" << I->label;
      if (I->src[0].node()) {
        out << "  (GT";
        out << ".";
        for (int i = 0; i < I->src[0].swizzle().size(); i++) {
          out << swizChars[I->src[0].swizzle()[i]];
        }
        out << ")";
      }
      out << ";";
    } else if (I->op == SH_ARB_REP) {
      out << "  REP ";
      printVar(out, false, I->src[0], false, I->src[0].swizzle());
      out << ";";
    } else if (I->op == SH_ARB_BRK) {
      if (I->src[0].node()) {
        out << "  MOVC ";
        printVar(out, true, I->src[0], false);
        out << ", ";
        printVar(out, false, I->src[0], false, I->src[0].swizzle());
        out << ";" << endl;
      }
      out << "  BRK ";
      if (I->src[0].node()) {
        out << " (";
        if (I->invert) {
          out << "LE";
        } else {
          out << "GT";
        }
        out << ".";
        for (int i = 0; i < I->src[0].swizzle().size(); i++) {
          out << swizChars[I->src[0].swizzle()[i]];
        }
        out << ")";
      }
      out << ";";
    } else if (I->op == SH_ARB_ENDREP) {
      out << "  ENDREP;";
    } else if (I->op == SH_ARB_IF) {
      if (I->src[0].node()) {
        out << "  MOVC ";
        printVar(out, true, I->src[0], false);
        out << ", ";
        printVar(out, false, I->src[0], false, I->src[0].swizzle());
        out << ";" << endl;
      }
      out << "  IF ";
      if (I->src[0].node()) {
        out << "GT";
        out << ".";
        for (int i = 0; i < I->src[0].swizzle().size(); i++) {
          out << swizChars[I->src[0].swizzle()[i]];
        }
      } else {
        out << "TR";
      }
      out << ";";
    } else if (!printSamplingInstruction(out, *I)) {
      out << "  ";
      out << arbOpInfo[I->op].name;
      if (I->update_cc) out << "C";
      out << " ";
      printVar(out, true, I->dest, arbOpInfo[I->op].collectingOp);
      if (I->ccode != ArbInst::NOCC) {
        out << " (";
        out << arbCCnames[I->ccode];
        out << ".";
        for (int i = 0; i < 4; i++) {
          out << swizChars[(i < I->ccswiz.size() ? I->ccswiz[i]
                            : (I->ccswiz.size() == 1 ? I->ccswiz[0] : i))];
        }
        out << ") ";
      }
      for (int i = 0; i < arbOpInfo[I->op].arity; i++) {
        out << ", ";
        printVar(out, false, I->src[i], arbOpInfo[I->op].collectingOp, I->dest.swizzle());
      }
      out << ';';
    }
    out << " # ";
    if (I->dest.node() && I->dest.has_name()) {
      out << "d=" << I->dest.name() << " ";
    }
    for (int i = 0; i < ArbInst::max_num_sources; i++) {
      if (I->src[i].node()  && I->src[i].has_name()) {
        out << "s[" << i << "]=" << I->src[i].name() << " ";
      }
    }
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
    m_registers[*I]->printDecl(out);
    out << std::endl;
  }

  out << "Outputs:" << std::endl;
  for (I = m_shader->outputs.begin(); I != m_shader->outputs.end(); ++I) {
    out << "  ";
    m_registers[*I]->printDecl(out);
    out << std::endl;
  }
  return out;
}

int ArbCode::getLabel(ShCtrlGraphNodePtr node)
{
  if (m_label_map.find(node) == m_label_map.end()) {
    m_label_map[node] = m_max_label++;
  }
  return m_label_map[node];
}

void ArbCode::genNode(ShCtrlGraphNodePtr node)
{
  if (!node || node->marked()) return;
  node->mark();

  if (node == m_shader->ctrlGraph->exit()) return;
  
  if (m_environment & SH_ARB_NVVP2) {
    m_instructions.push_back(ArbInst(SH_ARB_LABEL, getLabel(node)));
  }
  
  if (node->block) for (ShBasicBlock::ShStmtList::const_iterator I = node->block->begin();
       I != node->block->end(); ++I) {
    const ShStatement& stmt = *I;
    emit(stmt);
  }

  if (m_environment & SH_ARB_NVVP2) {
    for(std::vector<SH::ShCtrlGraphBranch>::iterator I = node->successors.begin();
	I != node->successors.end(); I++) {
      m_instructions.push_back(ArbInst(SH_ARB_BRA, getLabel(I->node), I->cond));
    }
    m_instructions.push_back(ArbInst(SH_ARB_BRA, getLabel(node->follower)));
    for(std::vector<SH::ShCtrlGraphBranch>::iterator I = node->successors.begin();
	I != node->successors.end(); I++) {
      genNode(I->node);
    }
  }

  genNode(node->follower);
}

void ArbCode::genStructNode(const ShStructuralNodePtr& node)
{
  if (!node) return;

  if (node->type == ShStructuralNode::UNREDUCED) {
    ShBasicBlockPtr block = node->cfg_node->block;
    if (block) for (ShBasicBlock::ShStmtList::const_iterator I = block->begin();
                    I != block->end(); ++I) {
      const ShStatement& stmt = *I;
      emit(stmt);
    }
  } else if (node->type == ShStructuralNode::BLOCK) {
    for (ShStructuralNode::StructNodeList::const_iterator I = node->structnodes.begin();
         I != node->structnodes.end(); ++I) {
      genStructNode(*I);
    }
  } else if (node->type == ShStructuralNode::IFELSE) {
    ShStructuralNodePtr header = node->structnodes.front();
    // TODO Check that header->successors is only two.
    ShVariable cond;
    ShStructuralNodePtr ifnode, elsenode;
    for (ShStructuralNode::SuccessorList::iterator I = header->succs.begin();
         I != header->succs.end(); ++I) {
      if (I->first.node()) {
        ifnode = I->second;
        cond = I->first;
      } else {
        elsenode = I->second;
      }
    }
    genStructNode(header);
    m_instructions.push_back(ArbInst(SH_ARB_IF, ShVariable(), cond)); {
      genStructNode(ifnode);
    } m_instructions.push_back(ArbInst(SH_ARB_ELSE, ShVariable())); {
      genStructNode(elsenode);
    } m_instructions.push_back(ArbInst(SH_ARB_ENDIF, ShVariable()));
  } else if (node->type == ShStructuralNode::WHILELOOP) {
    ShStructuralNodePtr header = node->structnodes.front();

    ShVariable cond = header->succs.front().first;
    
    ShStructuralNodePtr body = node->structnodes.back();

    ShVariable maxloop(new ShVariableNode(SH_CONST, 1));
    float maxloopval = 255.0;
    maxloop.setValues(&maxloopval);
    m_shader->constants.push_back(maxloop.node());
    m_instructions.push_back(ArbInst(SH_ARB_REP, ShVariable(), maxloop));
    genStructNode(header);
    ArbInst brk(SH_ARB_BRK, ShVariable(), cond);
    brk.invert = true;
    m_instructions.push_back(brk);
    genStructNode(body);
    
    m_instructions.push_back(ArbInst(SH_ARB_ENDREP, ShVariable()));
  } else if (node->type == ShStructuralNode::SELFLOOP) {
    ShStructuralNodePtr loopnode = node->structnodes.front();

    bool condexit = true; // true if the condition causes us to exit the
                          // loop, rather than continue it
    ShVariable cond;
    for (ShStructuralNode::SuccessorList::iterator I = loopnode->succs.begin();
         I != loopnode->succs.end(); ++I) {
      if (I->first.node()) {
        if (I->second == loopnode) condexit = false; else condexit = true;
        cond = I->first;
      }
    }
    
    ShVariable maxloop(new ShVariableNode(SH_CONST, 1));
    float maxloopval = 255.0;
    maxloop.setValues(&maxloopval);
    m_shader->constants.push_back(maxloop.node());
    m_instructions.push_back(ArbInst(SH_ARB_REP, ShVariable(), maxloop));
    genStructNode(loopnode);
    ArbInst brk(SH_ARB_BRK, ShVariable(), cond);
    if (!condexit) {
      brk.invert = true;
    } 
    m_instructions.push_back(brk);
    m_instructions.push_back(ArbInst(SH_ARB_ENDREP, ShVariable()));
  }
}

void ArbCode::allocRegs()
{
  ArbLimits limits(m_unit);
  
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
    
    m_registers[node] = new ArbReg(type, num++, node->name());
    m_registers[node]->binding = specs.binding;
    m_registers[node]->bindingIndex = bindings.back();
    m_reglist.push_back(m_registers[node]);
    
    bindings.back()++;
    if (bindings.back() == specs.maxBindings) break;
  }    
}

void ArbCode::allocInputs(const ArbLimits& limits)
{
  // First, try to assign some "special" output register bindings
  for (int i = 0; arbBindingSpecs(false, m_unit)[i].binding != SH_ARB_REG_NONE; i++) {
    bindSpecial(m_shader->inputs.begin(), m_shader->inputs.end(),
                arbBindingSpecs(false, m_unit)[i], m_inputBindings,
                SH_ARB_REG_ATTRIB, m_numInputs);
  }
  
  for (ShProgramNode::VarList::const_iterator I = m_shader->inputs.begin();
       I != m_shader->inputs.end(); ++I) {
    ShVariableNodePtr node = *I;
    if (m_registers.find(node) != m_registers.end()) continue;
    m_registers[node] = new ArbReg(SH_ARB_REG_ATTRIB, m_numInputs++, node->name());
    m_reglist.push_back(m_registers[node]);

    // Binding
    for (int i = 0; arbBindingSpecs(false, m_unit)[i].binding != SH_ARB_REG_NONE; i++) {
      const ArbBindingSpecs& specs = arbBindingSpecs(false, m_unit)[i];

      if (specs.allowGeneric && m_inputBindings[i] < specs.maxBindings) {
        m_registers[node]->binding = specs.binding;
        m_registers[node]->bindingIndex = m_inputBindings[i];
        m_inputBindings[i]++;
        break;
      }
    }
  }
}

void ArbCode::allocOutputs(const ArbLimits& limits)
{
  // First, try to assign some "special" output register bindings
  for (int i = 0; arbBindingSpecs(true, m_unit)[i].binding != SH_ARB_REG_NONE; i++) {
    bindSpecial(m_shader->outputs.begin(), m_shader->outputs.end(),
                arbBindingSpecs(true, m_unit)[i], m_outputBindings,
                SH_ARB_REG_OUTPUT, m_numOutputs);
  }
  
  for (ShProgramNode::VarList::const_iterator I = m_shader->outputs.begin();
       I != m_shader->outputs.end(); ++I) {
    ShVariableNodePtr node = *I;
    if (m_registers.find(node) != m_registers.end()) continue;
    m_registers[node] = new ArbReg(SH_ARB_REG_OUTPUT, m_numOutputs++, node->name());
    m_reglist.push_back(m_registers[node]);
    
    // Binding
    for (int i = 0; arbBindingSpecs(true, m_unit)[i].binding != SH_ARB_REG_NONE; i++) {
      const ArbBindingSpecs& specs = arbBindingSpecs(true, m_unit)[i];

      if (specs.allowGeneric && m_outputBindings[i] < specs.maxBindings) {
        m_registers[node]->binding = specs.binding;
        m_registers[node]->bindingIndex = m_outputBindings[i];
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
  m_registers[node] = new ArbReg(SH_ARB_REG_PARAM, m_numParams, node->name());
  m_registers[node]->binding = SH_ARB_REG_PARAMLOC;
  m_registers[node]->bindingIndex = m_numParams;
  m_reglist.push_back(m_registers[node]);
  m_numParams++;
}

void ArbCode::allocConsts(const ArbLimits& limits)
{
  for (ShProgramNode::VarList::const_iterator I = m_shader->constants.begin();
       I != m_shader->constants.end(); ++I) {
    ShVariableNodePtr node = *I;

    // TODO: improve efficiency
    RegMap::const_iterator J;
    for (J = m_registers.begin(); J != m_registers.end(); ++J) {
      if (J->second->type != SH_ARB_REG_CONST) continue;
      int f = 0;
      for (int i = 0; i < node->size(); i++) {
        if (J->second->values[i] == node->getValue(i)) f++;
      }
      if (f == node->size()) break;
    }
    if (J == m_registers.end()) {
      m_registers[node] = new ArbReg(SH_ARB_REG_CONST, m_numConsts, node->name());
      m_reglist.push_back(m_registers[node]);
      for (int i = 0; i < 4; i++) {
        m_registers[node]->values[i] = (float)(i < node->size() ? node->getValue(i) : 0.0);
      }
      m_numConsts++;
    } else {
      m_registers[node] = J->second;
    }
  }
}

bool mark(ShLinearAllocator& allocator, ShVariableNodePtr node, int i)
{
  if (!node) return false;
  if (node->kind() != SH_TEMP) return false;
  if (node->hasValues()) return false;
  allocator.mark(node, i);
  return true;
}

bool markable(ShVariableNodePtr node)
{
  if (!node) return false;
  if (node->kind() != SH_TEMP) return false;
  if (node->hasValues()) return false;
  return true;
}

struct ArbScope {
  ArbScope(int start)
    : start(start)
  {
  }
  
  typedef std::map< ShVariableNode*, std::bitset<4> > UsageMap;

  typedef std::set<ShVariableNode*> MarkList;
  
  MarkList need_mark; // Need to mark at end of loop
  int start; // location where loop started
  UsageMap usage_map;
  UsageMap write_map; // locations last written to
};

void ArbCode::allocTemps(const ArbLimits& limits)
{

  typedef std::list<ArbScope> ScopeStack;
  ScopeStack scopestack;
  
  ShLinearAllocator allocator(this);

//   {
//     ScopeStack scopestack;
//     // First do a backwards traversal to find loop nodes that need to be
//     // marked due to later uses of assignments
//     std::map<ShVariableNode*, int> last_use;
    
//     for (int i = (int)m_instructions.size() - 1; i >= 0; --i) {
//       ArbInst instr = m_instructions[i];
//       if (instr.op == SH_ARB_ENDREP) {
//         scopestack.push_back((int)i);
//       }
//       if (instr.op == SH_ARB_REP) {
//         const ArbScope& scope = scopestack.back();
//         for (ArbScope::MarkList::const_iterator I = scope.need_mark.begin();
//              I != scope.need_mark.end(); ++I) {
//           mark(allocator, *I, (int)i);
//         }
//         scopestack.pop_back();
//       }

//       if (markable(instr.dest.node())) {
//         if (last_use.find(instr.dest.node().object()) == last_use.end()) continue;
//         for (ScopeStack::iterator S = scopestack.begin(); S != scopestack.end(); ++S) {
//           ArbScope& scope = *S;
//           // Note scope.start == location of ENDREP
//           // TODO: Consider sub-components in last_use update and here.
//           if (last_use[instr.dest.node().object()] > scope.start) {
//             mark(allocator, instr.dest.node().object(), scope.start);
//             scope.need_mark.insert(instr.dest.node().object());
//           }
//         }
//       }
      
//       for (int j = 0; j < ArbInst::max_num_sources; j++) {
//         if (!markable(instr.src[j].node())) continue;
        
//         if (last_use.find(instr.src[j].node().object()) == last_use.end()) {
//           last_use[instr.src[j].node().object()] = i;
//         }
//       }
//     }
//   }

  {
    ScopeStack scopestack;
    // First do a backwards traversal to find loop nodes that need to be
    // marked due to later uses of assignments

    // push root stack

    scopestack.push_back(m_instructions.size() - 1);
    
    for (int i = (int)m_instructions.size() - 1; i >= 0; --i) {
      ArbInst instr = m_instructions[i];
      if (instr.op == SH_ARB_ENDREP) {
        scopestack.push_back((int)i);
      }
      if (instr.op == SH_ARB_REP) {
        const ArbScope& scope = scopestack.back();
        for (ArbScope::MarkList::const_iterator I = scope.need_mark.begin();
             I != scope.need_mark.end(); ++I) {
          mark(allocator, *I, (int)i);
        }
        scopestack.pop_back();
      }

      if (markable(instr.dest.node())) {
        std::bitset<4> writemask;
        for (int k = 0; k < instr.dest.size(); k++) {
          writemask[instr.dest.swizzle()[k]] = true;
        }
        std::bitset<4> used;
        for (ScopeStack::iterator S = scopestack.begin(); S != scopestack.end(); ++S) {
          ArbScope& scope = *S;

          if ((used & writemask).any()) {
            mark(allocator, instr.dest.node().object(), scope.start);
            scope.need_mark.insert(instr.dest.node().object());
          }
          
          used |= scope.usage_map[instr.dest.node().object()];
        }

        ArbScope& scope = scopestack.back();
        scope.usage_map[instr.dest.node().object()] &= ~writemask;
      }
      
      for (int j = 0; j < ArbInst::max_num_sources; j++) {
        if (!markable(instr.src[j].node())) continue;
        std::bitset<4> usemask;
        for (int k = 0; k < instr.src[j].size(); k++) {
          usemask[instr.src[j].swizzle()[k]] = true;
        }
        ArbScope& scope = scopestack.back();
        scope.usage_map[instr.src[j].node().object()] |= usemask;
      }
    }
  }
  
  for (std::size_t i = 0; i < m_instructions.size(); i++) {
    ArbInst instr = m_instructions[i];
    if (instr.op == SH_ARB_REP) {
      scopestack.push_back((int)i);
    }
    if (instr.op == SH_ARB_ENDREP) {
      const ArbScope& scope = scopestack.back();
      for (ArbScope::MarkList::const_iterator I = scope.need_mark.begin();
           I != scope.need_mark.end(); ++I) {
        mark(allocator, *I, (int)i);
      }
      scopestack.pop_back();
    }

    if (mark(allocator, instr.dest.node(), (int)i)) {
      for (ScopeStack::iterator S = scopestack.begin(); S != scopestack.end(); ++S) {
        ArbScope& scope = *S;
        std::bitset<4> writemask;
        for (int k = 0; k < instr.dest.size(); k++) {
          writemask[instr.dest.swizzle()[k]] = true;
        }
        // TODO: Only change the writemask for scopes that see this
        // write unconditionally
        // I.e. don't change it if the scope is outside an if
        // statement, or a post-BRK REP scope.
        scope.write_map[instr.dest.node().object()] |= writemask;

      }        
    }
    
    for (int j = 0; j < ArbInst::max_num_sources; j++) {
      if (mark(allocator, instr.src[j].node(), (int)i)) {
        for (ScopeStack::iterator S = scopestack.begin(); S != scopestack.end(); ++S) {
          ArbScope& scope = *S;
          // Mark uses that weren't recently written to.
          std::bitset<4> usemask;
          for (int k = 0; k < instr.src[j].size(); k++) {
            usemask[instr.src[j].swizzle()[k]] = true;
          }
          if ((usemask & ~scope.write_map[instr.src[j].node().object()]).any()) {
            mark(allocator, instr.src[j].node(), scope.start);
            scope.need_mark.insert(instr.src[j].node().object());
          }
        }
      }
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
    m_registers[node] = new ArbReg(SH_ARB_REG_TEXTURE, index, node->name());
    m_reglist.push_back(m_registers[node]);
    m_numTextures++;
  }
}

void ArbCode::bindTextures()
{
  for (ShProgramNode::TexList::const_iterator I = m_shader->textures.begin();
       I != m_shader->textures.end(); ++I) {
    m_textures->bindTexture(*I, GL_TEXTURE0 + m_registers[*I]->index);
  }
}

}
