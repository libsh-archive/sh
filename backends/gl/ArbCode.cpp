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

#include "ShVariable.hpp"
#include "ShDebug.hpp"
#include "ShLinearAllocator.hpp"
#include "ShInternals.hpp"
#include "ShOptimizer.hpp"
#include "ShEnvironment.hpp"
#include "ShContext.hpp"
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
    m_numTextures(0), m_programId(0)
{
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
    ShOptimizer optimizer(m_shader->ctrlGraph);
    optimizer.optimize(ShContext::current()->optimization());
    m_shader->collectVariables();
  } else {
    m_shader = shref_const_cast<ShProgramNode>(m_originalShader);
    ShContext::current()->exit();
    ShContext::current()->enter(m_shader);
  }

  m_shader->ctrlGraph->entry()->clearMarked();
  genNode(m_shader->ctrlGraph->entry());
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
    SH_DEBUG_ERROR("Out of temporaries!");
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
  if (instr.op != SH_ARB_TEX && instr.op != SH_ARB_TXP && instr.op != SH_ARB_TXB)
    return false;

  ShTextureNodePtr texture = shref_dynamic_cast<ShTextureNode>(instr.src[1].node());
  RegMap::const_iterator texRegIt = m_registers.find(instr.src[1].node());
  SH_DEBUG_ASSERT(texRegIt != m_registers.end());

  const ArbReg& texReg = *texRegIt->second;
  
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
  if (m_unit == "vertex") out << "!!ARBvp1.0" << endl;
  if (m_unit == "fragment") out << "!!ARBfp1.0" << endl;

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

void ArbCode::genNode(ShCtrlGraphNodePtr node)
{
  if (!node || node->marked()) return;
  node->mark();

  if (node->block) for (ShBasicBlock::ShStmtList::const_iterator I = node->block->begin();
       I != node->block->end(); ++I) {
    const ShStatement& stmt = *I;
    emit(stmt);
  }

  genNode(node->follower);
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
