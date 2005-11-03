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
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <bitset>

#include "ShVariable.hpp"
#include "ShDebug.hpp"
#include "ShLinearAllocator.hpp"
#include "ShInternals.hpp"
#include "ShOptimizations.hpp"
#include "ShContext.hpp"
#include "ShTypeInfo.hpp"
#include "ShVariant.hpp"
#include "ShTextureNode.hpp"
#include "ShSyntax.hpp"
#include "ArbReg.hpp"
#include "Arb.hpp"
#include "ShAttrib.hpp"
#include "ShCastManager.hpp"
#include "ShError.hpp"

namespace shgl {

using namespace SH;
using namespace std;

#define shGlProgramStringARB glProgramStringARB
#define shGlActiveTextureARB glActiveTextureARB
#define shGlProgramLocalParameter4fvARB glProgramLocalParameter4fvARB
#define shGlProgramEnvParameter4fvARB glProgramEnvParameter4fvARB
#define shGlGetProgramivARB glGetProgramivARB
#define shGlGenProgramsARB glGenProgramsARB
#define shGlDeleteProgramsARB glDeleteProgramsARB
#define shGlBindProgramARB glBindProgramARB

// #define ARBCODE_DEBUG

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
  {SH_ARB_REG_RESULTCOL, 1, SH_COLOR, true}, // modified by constructor if ATI_draw_buffers is present
  {SH_ARB_REG_RESULTDPT, 1, SH_POSITION, false},
  {SH_ARB_REG_NONE, 0, SH_ATTRIB}
};

ArbBindingSpecs* arbBindingSpecs(bool output, const string& unit)
{
  if (unit == "vertex")
    return output ? arbVertexOutputBindingSpecs : arbVertexAttribBindingSpecs;
  if (unit == "fragment")
    return output ? arbFragmentOutputBindingSpecs : arbFragmentAttribBindingSpecs;
  return 0;
}

ArbCode::ArbCode(const ShProgramNodeCPtr& shader, const string& unit,
                 TextureStrategy* texture)
  : m_texture(texture), m_shader(0), m_originalShader(0), m_unit(unit),
    m_numTemps(0), m_numHalfTemps(0), m_numInputs(0), m_numOutputs(0), m_numParams(0), m_numParamBindings(0),
    m_numConsts(0),
    m_numTextures(0), m_programId(0), m_environment(0), m_max_label(0),
    m_address_register(new ShVariableNode(SH_TEMP, 1, SH_FLOAT))
{
  m_originalShader =  const_cast<ShProgramNode*>(shader.object());

  if (unit == "fragment") {
    m_environment |= SH_ARB_FP;
  } else if (unit == "vertex") {
    m_environment |= SH_ARB_VP;
  } else {
    stringstream s;
    s << "Cannot generate ARB code for the '" << unit
      << "' unit.  Valid units are 'vertex' and 'fragment'." << endl;
    shError(ArbException(s.str()));
  }

  const GLubyte* extensions = glGetString(GL_EXTENSIONS);
  if (extensions) {
    string extstr(reinterpret_cast<const char*>(extensions));

    if (unit == "fragment") {
      if (extstr.find("NV_fragment_program_option") != string::npos) {
        m_environment |= SH_ARB_NVFP;
      }
      if (extstr.find("NV_fragment_program2") != string::npos) {
        m_environment |= SH_ARB_NVFP2;
      }
      if ((extstr.find("ATI_draw_buffers") != string::npos) ||
          (extstr.find("ARB_draw_buffers") != string::npos)) {
        m_environment |= SH_ARB_ATIDB;
	arbFragmentOutputBindingSpecs[0].binding = SH_ARB_REG_RESULTCOL_ATI;

        int max_draw_buffers;
        if (extstr.find("ATI_draw_buffers") != string::npos) {
          SH_GL_CHECK_ERROR(glGetIntegerv(GL_MAX_DRAW_BUFFERS_ATI, &max_draw_buffers));
        } else {
          SH_GL_CHECK_ERROR(glGetIntegerv(GL_MAX_DRAW_BUFFERS_ARB, &max_draw_buffers));
        }
        arbFragmentOutputBindingSpecs[0].maxBindings = max_draw_buffers;
      }
    }
    if (unit == "vertex") {
      if (extstr.find("NV_vertex_program2_option") != string::npos) {
        m_environment |= SH_ARB_NVVP2;
      }
      if (extstr.find("NV_vertex_program3") != string::npos) {
        m_environment |= SH_ARB_NVVP3;
      }
    }
  }

  // initialize m_convertMap
  m_convertMap[SH_DOUBLE] = SH_FLOAT; 

  bool halfSupport = m_environment & SH_ARB_NVFP; 
  if (!halfSupport) m_convertMap[SH_HALF] = SH_FLOAT;

  m_convertMap[SH_INT] = SH_FLOAT;
  m_convertMap[SH_SHORT] = halfSupport ? SH_HALF: SH_FLOAT;
  m_convertMap[SH_BYTE] = halfSupport ? SH_HALF: SH_FLOAT;
  m_convertMap[SH_UINT] = SH_FLOAT;
  m_convertMap[SH_USHORT] = halfSupport ? SH_HALF: SH_FLOAT;
  m_convertMap[SH_UBYTE] = halfSupport ? SH_HALF: SH_FLOAT;

  m_convertMap[SH_FINT] = SH_FLOAT;
  m_convertMap[SH_FSHORT] = SH_FLOAT;
  m_convertMap[SH_FBYTE] = halfSupport ? SH_HALF : SH_FLOAT;
  m_convertMap[SH_FUINT] = SH_FLOAT;
  m_convertMap[SH_FUSHORT] = SH_FLOAT;
  m_convertMap[SH_FUBYTE] = halfSupport ? SH_HALF : SH_FLOAT;
}

ArbCode::~ArbCode()
{
 if (m_shader != m_originalShader)
   {
   delete m_shader;
   }
}

void dump(ShProgramNodePtr foo, string desc) {
#ifdef ARBCODE_DEBUG
  optimize(foo);
  foo->dump(desc + "_" + foo->name() + "_" + foo->target());
#endif
}

void ArbCode::generate()
{
  // Keep a mapping back to the original variables
  m_originalVarsMap.clear();

  // Transform code to be ARB_fragment_program compatible
  // TODO: clone should update m_originalVarsMap to map our cloned variables
  // back to the originals in m_originalShader.
  ShProgramNodePtr temp_shader = m_originalShader->clone();
  m_shader = temp_shader.object();
  m_shader->acquireRef();
  temp_shader = NULL;

  ShContext::current()->enter(m_shader);
  ShTransformer transform(m_shader);

  dump(m_shader, "arbcode_start");
  // Record changes that the transformer makes to the input/output variables
  // so that we have a mapping from the changed ones back to the originals.
  transform.convertInputOutput(&m_originalVarsMap);
  transform.convertTextureLookups();
  transform.convertToFloat(m_convertMap);
  transform.splitTuples(4, m_splits);
  transform.stripDummyOps();
  transform.expand_atan2();
  transform.expand_inverse_hyperbolic();
  dump(m_shader, "arbcode_done");
 
  if (transform.changed()) {
    optimize(m_shader);
  } else {
    m_shader->releaseRef();
    m_shader = m_originalShader;
    m_originalVarsMap.clear();       // We're now pointing directly to the originals
    ShContext::current()->exit();
    ShContext::current()->enter(m_shader);
  }

  try {
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
  } catch (...) {
    m_shader->ctrlGraph->entry()->clearMarked();
    ShContext::current()->exit();
    throw;
  }
  ShContext::current()->exit();
}

bool ArbCode::allocateRegister(const ShVariableNodePtr& var)
{
  if (!var) return true;
  if (var->kind() != SH_TEMP) return true;
  if (var->uniform()) return true;

  if (m_tempRegs.empty()) {
    // This gets caught around allocTemps.
    throw 1; // yes, it's hacky. Instead we should throw a different
             // type, or store the limit information.
  }

  int idx = m_tempRegs.front();
  m_tempRegs.pop_front();
  if (var->valueType() == SH_HALF) {
    if (idx + 1 > m_numHalfTemps) m_numHalfTemps = idx + 1;
    m_registers[var] = new ArbReg(SH_ARB_REG_HALF_TEMP, idx);
  } else {
    if (idx + 1 > m_numTemps) m_numTemps = idx + 1;
    m_registers[var] = new ArbReg(SH_ARB_REG_TEMP, idx);
  }
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
  
  ostringstream out;
  print(out);
  string text = out.str();
#ifdef ARBCODE_DEBUG
  ofstream fout((m_originalShader->name() + "_arb.asm").c_str());
  fout << text;
  fout.close();
#endif
  shGlProgramStringARB(arbTarget(m_unit), GL_PROGRAM_FORMAT_ASCII_ARB,
                       (GLsizei)text.size(), text.c_str());
  int error = glGetError();
  ostringstream error_os;
  if (error == GL_NO_ERROR) return;
  
  error_os << "Failed to upload ARB program." << endl;
  if (error == GL_INVALID_OPERATION) {
    error_os << "Program error:" << endl;
    GLint pos = -1;
    SH_GL_CHECK_ERROR(glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &pos));
    if (pos >= 0){
      const unsigned char* message = glGetString(GL_PROGRAM_ERROR_STRING_ARB);
      error_os << "Error at character " << pos << endl;
      error_os << "Driver Message: " << message << endl;
      while (pos >= 0 && text[pos] != '\n') pos--;
      if (pos > 0) pos++;
      error_os << "Code: " << text.substr(pos, text.find('\n', pos)) << endl;
    }
  } else {
    error_os << "Unknown error." << endl;
  }
  shError(ArbException(error_os.str()));
}

void ArbCode::bind()
{
  if (!m_programId) {
    upload();
  }
  
  SH_GL_CHECK_ERROR(shGlBindProgramARB(arbTarget(m_unit), m_programId));
  
  ShContext::current()->set_binding(string("arb:") + m_unit, ShProgram(m_originalShader));

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
  
  if (m_unit == "vertex") {
    SH_GL_CHECK_ERROR(glEnable(GL_VERTEX_PROGRAM_ARB));
  } else if (m_unit == "fragment") {
    SH_GL_CHECK_ERROR(glEnable(GL_FRAGMENT_PROGRAM_ARB));
  }
}

void ArbCode::unbind()
{
  ShContext::current()->unset_binding(string("arb:") + m_unit);

  if (m_unit == "vertex") {
    SH_GL_CHECK_ERROR(glDisable(GL_VERTEX_PROGRAM_ARB));
  } else if (m_unit == "fragment") {
    SH_GL_CHECK_ERROR(glDisable(GL_FRAGMENT_PROGRAM_ARB));
  }
}

void ArbCode::update()
{
  bindTextures();
}

void ArbCode::updateUniform(const ShVariableNodePtr& uniform)
{
  if (!uniform) return;

  // This is too slow - if we really need it here later we have to find a
  // better way of doing it than looking up in a map with a string key!
  //if (!uniform->meta("opengl:readonly").empty()) {
  //  return;
  //}

  // TODO The "find" and "count" map accesses here are still bottlenecks.
  // There may be a good way to replace these with something faster.
  const ShVariant *uniformVariant = uniform->getVariant();
  RegMap::const_iterator I = m_registers.find(uniform);
  if (I == m_registers.end()) { // perhaps uniform was split
    if (m_splits.count(uniform) > 0) {
      ShTransformer::VarNodeVec &splitVec = m_splits[uniform];

      int offset = 0;
      int copySwiz[4];
      for (ShTransformer::VarNodeVec::iterator it = splitVec.begin();
           it != splitVec.end(); offset += (*it)->size(), ++it) {
        // TODO switch to properly swizzled version
        for (int i = 0; i < (*it)->size(); ++i) copySwiz[i] = i + offset;
        (*it)->setVariant(uniformVariant->get(false, ShSwizzle(uniform->size(), (*it)->size(), copySwiz))); 
        updateUniform(*it);
      }
    } 
    return;
  }

  if (uniform->kind() == SH_TEXTURE) {
    return;
  }
    
  const ArbReg& reg = *I->second;
  
  const int uniform_size = uniform->size();
  SH_DEBUG_ASSERT(uniform_size <= 4);

  float values[4];
  int i;
  if (uniform->valueType() == SH_FLOAT) {
    // Copy to a float array
    const float *variant = static_cast<const float *>(uniformVariant->array());
    for (i = 0; i < uniform_size; ++i)
      values[i] = variant[i];
  }
  else {
    // Componentwise cast to float and copy
    typedef ShDataVariant<float, SH_HOST> FloatVariant;
    FloatVariant floatVariant(uniform->size());
    floatVariant.set(uniformVariant);
    for (i = 0; i < uniform_size; ++i)
      values[i] = floatVariant[i];
  }

  // Zero out any extra components
  for (; i < 4; i++) {
    values[i] = 0.0f;
  }
  
  if (reg.type != SH_ARB_REG_PARAM) return;
  switch(reg.binding.type) {
  case SH_ARB_REG_PROGRAMLOC:
    SH_GL_CHECK_ERROR(shGlProgramLocalParameter4fvARB(arbTarget(m_unit), reg.binding.index, values));
    break;
  case SH_ARB_REG_PROGRAMENV:
    SH_GL_CHECK_ERROR(shGlProgramEnvParameter4fvARB(arbTarget(m_unit), reg.binding.index, values));
    break;
  case SH_ARB_REG_STATE:
    SH_DEBUG_WARN("Updating uniforms bound to OpenGL state is not currently supported.");
  default:
    return;
  }
}

ostream& ArbCode::printVar(ostream& out, bool dest, const ShVariable& var,
                           bool collectingOp, const ShSwizzle& destSwiz = ShSwizzle(4),
                           bool do_swiz = true) const
{
  RegMap::const_iterator I = m_registers.find(var.node());
  if (I == m_registers.end()) {
    if ((1 == var.size()) && (SH_CONST == var.node()->kind())) {
      // Immediate value -- no need for a register
      out << var.getVariant()->encodeArray();
      return out; // no swizzling
    } else {
      cerr << "No register allocated for variable '" << var.name() << "' (size=" 
		<< var.size() << "; kind=" << var.node()->kind() << ")" << endl;
      out << "<no reg for " << var.name() << ">";
      return out;
    }
  } else {
    // The variable has a register assigned to it
    if (var.neg()) out << '-';
    const ArbReg& reg = *I->second;
    out << reg;
  }

  if (do_swiz) {
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
  }
  
  return out;
}

struct LineNumberer {
  LineNumberer() { line = 0; }
  int line;
};

ostream& operator<<(ostream& out, LineNumberer& l)
{
  out << " # " << ++l.line << endl;
  return out;
}

bool ArbCode::printSamplingInstruction(ostream& out, const ArbInst& instr) const
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

ostream& ArbCode::print(ostream& out)
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
    if ((*I)->type == SH_ARB_REG_HALF_TEMP) continue;
    if ((*I)->type == SH_ARB_REG_TEXTURE) continue;
    out << "  ";
    (*I)->printDecl(out);
    out << endl;
  }
  bool halfSupport = m_environment & SH_ARB_NVFP;
  if (m_numTemps > 0) { 
    if (halfSupport) {
      out << "  LONG TEMP ";
    } else {
      out << "  TEMP ";
    }
    for (int i = 0; i < m_numTemps; i++) {
      if (i > 0) out << ", ";
      out << ArbReg(SH_ARB_REG_TEMP, i);
    }
    out << ";" << endl;
  }

  if (m_numHalfTemps > 0) { 
    SH_DEBUG_ASSERT(halfSupport); // assume half support...
    out << "  SHORT TEMP ";
    for (int i = 0; i < m_numHalfTemps; i++) {
      if (i > 0) out << ", ";
      out << ArbReg(SH_ARB_REG_HALF_TEMP, i);
    }
    out << ";" << endl;
  }

  out << endl;
  
  // Print instructions
  for (ArbInstList::const_iterator I = m_instructions.begin();
       I != m_instructions.end(); ++I) {
    if (I->op == SH_ARB_LABEL) {
      out << "label" << I->label << ": ";
    } else if (I->op == SH_ARB_COMMENT) {
      out << "### " << I->comment;
    } else if (I->op == SH_ARB_ELSE) {
      out << "  ELSE;";
    } else if (I->op == SH_ARB_ENDIF) {
      out << "  ENDIF;";
    } else if (I->op == SH_ARB_BRA) {
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
    } else if (I->op == SH_ARB_RET) {
      out << "  RET ";
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
      out << "  IF ";
      if (I->src[0].node()) {
        if (I->invert) {
          out << "LE";
        } else {
          out << "GT";
        }
        out << ".";
        for (int i = 0; i < I->src[0].swizzle().size(); i++) {
          out << swizChars[I->src[0].swizzle()[i]];
        }
      } else {
        out << "TR";
      }
      out << ";";
    } else if (I->op == SH_ARB_ARRAYMOV) {
      out << "  MOV ";
      printVar(out, true, I->dest, false);
      out << ", ";
      printVar(out, false, I->src[0], false, ShSwizzle(4), false);
      out << "[";
      printVar(out, false, I->src[1], false);
      out << "]";
      out << ";";
    } else if (!printSamplingInstruction(out, *I)) {
      out << "  ";
      out << arbOpInfo[I->op].name;
      if (I->update_cc) out << "C";
      out << " ";
      if (I->dest.node()) {
        printVar(out, true, I->dest, arbOpInfo[I->op].collectingOp);
      }
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

ostream& ArbCode::describe_interface(ostream& out)
{
  out << "Inputs:" << endl;
  for (ShProgramNode::VarList::const_iterator I = m_shader->begin_inputs();
       I != m_shader->end_inputs(); ++I) {
    out << "  ";
    m_registers[*I]->printDecl(out);
    out << endl;
  }

  out << "Outputs:" << endl;
  for (ShProgramNode::VarList::const_iterator I = m_shader->begin_outputs();
       I != m_shader->end_outputs(); ++I) {
    out << "  ";
    m_registers[*I]->printDecl(out);
    out << endl;
  }
  return out;
}

ostream& ArbCode::describe_bindings(ostream& out)
{
  out << "Inputs:" << endl;
  for (ShProgramNode::VarList::const_iterator I = m_shader->begin_inputs();
       I != m_shader->end_inputs(); ++I) {
    out << "  " << (*I)->name() << " => " << m_registers[*I]->binding_name() << endl;
  }

  out << "Outputs:" << endl;
  for (ShProgramNode::VarList::const_iterator I = m_shader->begin_outputs();
       I != m_shader->end_outputs(); ++I) {
    out << "  " << (*I)->name() << " => " << m_registers[*I]->binding_name() << endl;
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
    for (vector<ShCtrlGraphBranch>::iterator I = node->successors.begin();
	I != node->successors.end(); I++) {
      ShVariable dummy(new ShVariableNode(SH_TEMP, I->cond.size(), SH_FLOAT));
      ArbInst updatecc(SH_ARB_MOV, dummy, I->cond);
      updatecc.update_cc = true;
      m_instructions.push_back(updatecc);
      m_instructions.push_back(ArbInst(SH_ARB_BRA, getLabel(I->node), I->cond));
    }
    if (!node->successors.empty() || node->follower->marked()) { // else it's next anyway, no need for bra
      m_instructions.push_back(ArbInst(SH_ARB_BRA, getLabel(node->follower)));
    }
    for (vector<ShCtrlGraphBranch>::iterator I = node->successors.begin();
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
  } 
  else if (node->type == ShStructuralNode::BLOCK) {
    for (ShStructuralNode::StructNodeList::const_iterator I = node->structnodes.begin();
         I != node->structnodes.end(); ++I) {
      genStructNode(*I);
    }
  } 
  else if (node->type == ShStructuralNode::IFELSE) {
    ShStructuralNodePtr header = node->structnodes.front();
    
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
    push_if(cond, false); {
      genStructNode(ifnode);
    } m_instructions.push_back(ArbInst(SH_ARB_ELSE, ShVariable())); {
      genStructNode(elsenode);
    } m_instructions.push_back(ArbInst(SH_ARB_ENDIF, ShVariable()));
  } 
  else if (node->type == ShStructuralNode::WHILELOOP) {
    ShStructuralNodePtr header = node->structnodes.front();

    ShVariable cond = header->succs.front().first;
    
    ShStructuralNodePtr body = node->structnodes.back();

    float maxloopval = 255.0f;
    ShConstAttrib1f maxloop(maxloopval);

    m_shader->constants.push_back(maxloop.node());
    m_instructions.push_back(ArbInst(SH_ARB_REP, ShVariable(), maxloop));
    genStructNode(header);
    push_break(cond, true);
    genStructNode(body);
    m_instructions.push_back(ArbInst(SH_ARB_ENDREP, ShVariable()));
  } 
  else if (node->type == ShStructuralNode::SELFLOOP) {
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
    
    float maxloopval = 255.0f;
    ShConstAttrib1f maxloop(maxloopval);

    m_shader->constants.push_back(maxloop.node());
    m_instructions.push_back(ArbInst(SH_ARB_REP, ShVariable(), maxloop));
    genStructNode(loopnode);
    push_break(cond, !condexit);
    m_instructions.push_back(ArbInst(SH_ARB_ENDREP, ShVariable()));
  } 
  else if (node->type == ShStructuralNode::IF) {
    ShStructuralNodePtr header = node->structnodes.front();
    
    ShStructuralNode::SuccessorList::iterator B = header->succs.begin();
    ShVariable cond = B->first;
    ShStructuralNodePtr ifnode = B->second;
    genStructNode(header);
    push_if(cond, false); {
      genStructNode(ifnode);
    } m_instructions.push_back(ArbInst(SH_ARB_ENDIF, ShVariable()));
  }
  else if (ShStructuralNode::PROPINT == node->type) {
    float maxloopval = 255.0f;
    ShConstAttrib1f maxloop(maxloopval);

    m_shader->constants.push_back(maxloop.node());
    m_instructions.push_back(ArbInst(SH_ARB_REP, ShVariable(), maxloop));

    int continue_levels=0;
    set<ShStructuralNode*> seen; // keep track of the nodes we have seen already
    bool found_loop_condition=false;
    ShStructuralNodePtr n = node->structnodes.front(); 
    while (seen.find(n.object()) == seen.end()) {
      seen.insert(n.object());
      genStructNode(n);
      if (n->succs.size() > 1) {
        // Figure out which branch is the conditional one
        pair<ShVariable, ShStructuralNodePtr> branch_pair = n->succs.front(); // conditional successor
        pair<ShVariable, ShStructuralNodePtr> default_pair = n->succs.back(); // non-conditional successor
        if (!branch_pair.first.node()) {
          branch_pair = n->succs.back();
          default_pair = n->succs.front();
        }
        ShVariable branch_cond = branch_pair.first;
        const ShStructuralNodePtr branch_node = branch_pair.second;

        // Check if the non-conditional node leaves the PROPINT node
        if (default_pair.second == node->succs.front().second) {
          // We are on the condition of a "while (branch_cond)" loop
          // so we can negate the condition and break
          push_break(branch_cond, true);
          found_loop_condition = true;
          n = branch_pair.second;
        } else {
          // We are on a node that looks like
          //   if (branch_node) break/continue;
          if (branch_node == node->succs.front().second) {
            // 'break' case
            bool is_last_break_statement = (seen.find(default_pair.second.object()) != seen.end());
            if (!found_loop_condition && is_last_break_statement) {
              // Must be the loop condition of a do..until loop
              
              // Put the loop condition outside of the continue block
              for (int i=0; i < continue_levels; i++) {
                m_instructions.push_back(ArbInst(SH_ARB_ENDIF, ShVariable()));
              }
              continue_levels = 0;

              push_break(branch_cond, false);
            } else {
              // Usual 'break' case
              push_break(branch_cond, false);
            }
          } else {
            // 'continue' case
            // Wrap the rest of the body in an if statement
            continue_levels++;
            push_if(branch_cond, true);
          }

          n = default_pair.second;
        }
      } else {
        n = n->succs.front().second; // only one successor
      }
    }
    
    // Close the continue block
    for (int i=0; i < continue_levels; i++) {
      m_instructions.push_back(ArbInst(SH_ARB_ENDIF, ShVariable()));
    }

    m_instructions.push_back(ArbInst(SH_ARB_ENDREP, ShVariable()));
  } 
  else {
    SH_DEBUG_WARN("Unknown ShStructuralNode type encountered.  Generated code may be incomplete.");
  }
}

void ArbCode::push_break(ShVariable& cond, bool negate)
{
  ShVariable dummy(new ShVariableNode(SH_TEMP, cond.size(), SH_FLOAT));
  ArbInst updatecc(SH_ARB_MOV, dummy, cond);
  updatecc.update_cc = true;
  m_instructions.push_back(updatecc);

  ArbInst brk(SH_ARB_BRK, ShVariable(), cond);
  brk.invert = negate;
  m_instructions.push_back(brk);
}

void ArbCode::push_if(ShVariable& cond, bool negate)
{
  ShVariable dummy(new ShVariableNode(SH_TEMP, cond.size(), SH_FLOAT));
  ArbInst updatecc(SH_ARB_MOV, dummy, cond);
  updatecc.update_cc = true;
  m_instructions.push_back(updatecc);
  ArbInst if_inst(SH_ARB_IF, ShVariable(), cond);
  if_inst.invert = negate;
  m_instructions.push_back(if_inst);
}

void ArbCode::allocRegs()
{
  ArbLimits limits(m_unit);
  
  allocInputs(limits);
  
  allocOutputs(limits);

  for (ShProgramNode::PaletteList::const_iterator I = m_shader->begin_palettes();
       I != m_shader->end_palettes(); ++I) {
    allocPalette(limits, *I);
  }
  
  for (ShProgramNode::VarList::const_iterator I = m_shader->begin_parameters();
       I != m_shader->end_parameters(); ++I) {
    allocParam(limits, *I);
  }

  allocConsts(limits);

  try {
    allocTemps(limits, false);
    bool halfSupport = m_environment & SH_ARB_NVFP; 
    if (halfSupport) {
      allocTemps(limits, true);
    }
  } catch (int) {
    ostringstream os;
    os << "Out of temporary registers (" << limits.temps()
       << " were available)";
    throw ArbException(os.str());
  } catch (...) {
    throw;
  }

  // Allocate array register
  if (m_shader->begin_palettes() != m_shader->end_palettes()) {
    m_registers[m_address_register.node()] = new ArbReg(SH_ARB_REG_ADDRESS, 0);
    m_reglist.push_back(m_registers[m_address_register.node()]);
  }

  allocTextures(limits);
}

void ArbCode::bindSpecial(const ShProgramNode::VarList::const_iterator& begin,
                          const ShProgramNode::VarList::const_iterator& end,
                          const ArbBindingSpecs& specs, 
                          vector<int>& bindings,
                          ArbRegType type, int& num)
{
  bindings.push_back(0);
  
  if (specs.semanticType == SH_ATTRIB) return;
  
  for (ShProgramNode::VarList::const_iterator I = begin; I != end; ++I) {
    ShVariableNodePtr node = *I;
    
    if (m_registers.find(node) != m_registers.end()) continue;
    if (node->specialType() != specs.semanticType) continue;
    
    m_registers[node] = new ArbReg(type, num++, node->name());
    m_registers[node]->binding.type = specs.binding;
    m_registers[node]->binding.index = bindings.back();
    m_reglist.push_back(m_registers[node]);
    
    bindings.back()++;
    if (bindings.back() == specs.maxBindings) break;
  }    
}

void ArbCode::allocSemanticInputs(const ArbLimits& limits)
{
  // First, try to assign some "special" input register bindings
  for (int i = 0; arbBindingSpecs(false, m_unit)[i].binding != SH_ARB_REG_NONE; i++) {
    bindSpecial(m_shader->inputs.begin(), m_shader->inputs.end(),
                arbBindingSpecs(false, m_unit)[i], m_inputBindings,
                SH_ARB_REG_ATTRIB, m_numInputs);
  }
  
  int inputnb=0;
  for (ShProgramNode::VarList::const_iterator I = m_shader->inputs.begin();
       I != m_shader->inputs.end(); ++I, ++inputnb) {
    ShVariableNodePtr node = *I;
    if (m_registers.find(node) != m_registers.end()) continue;
    
    // Binding
    bool bound = false;
    for (int i = 0; arbBindingSpecs(false, m_unit)[i].binding != SH_ARB_REG_NONE; i++) {
      const ArbBindingSpecs& specs = arbBindingSpecs(false, m_unit)[i];

      if (specs.allowGeneric && m_inputBindings[i] < specs.maxBindings) {
        m_registers[node] = new ArbReg(SH_ARB_REG_ATTRIB, m_numInputs++, node->name());
        m_reglist.push_back(m_registers[node]);
        m_registers[node]->binding.type = specs.binding;
        m_registers[node]->binding.index = m_inputBindings[i];
        m_inputBindings[i]++;
        bound = true;
        break;
      }
    }

    if (!bound) {
      std::cerr << "Could not bind input " << inputnb << " to a built-in variable." << std::endl;
    }
  }
}

void ArbCode::allocGenericInputs(const ArbLimits& limits)
{
  // First look for a position variable and map it to generic attribute 0
  for (ShProgramNode::VarList::const_iterator I = m_shader->inputs.begin();
       I != m_shader->inputs.end(); ++I) {
    ShVariableNodePtr node = *I;
    if (m_registers.find(node) == m_registers.end() &&
        node->specialType() == SH_POSITION) {
      m_registers[node] = new ArbReg(SH_ARB_REG_ATTRIB, m_numInputs++, node->name());
      m_registers[node]->binding.type = SH_ARB_REG_VERTEXATR;
      m_registers[node]->binding.index = 0;    // map to vertex.attrib[0]
      m_reglist.push_back(m_registers[node]);

      // Set the associated metadata so the caller knows which attrib we used
      // Note that we want to change the metadata in the ORIGINAL variables so that
      // it is accessible to the user.
      m_originalVarsMap.get_original_variable(node)->meta("opengl:attribindex", "0");
      break;                                   // we only want to bind at most one
    }
  }

  // Ok, now second pass, everything else we just do in order, using indices 1 and up
  int AttribIndex = 1;
  for (ShProgramNode::VarList::const_iterator I = m_shader->inputs.begin();
       I != m_shader->inputs.end(); ++I) {
    ShVariableNodePtr node = *I;
    if (m_registers.find(node) != m_registers.end()) continue;

    m_registers[node] = new ArbReg(SH_ARB_REG_ATTRIB, m_numInputs++, node->name());
    m_registers[node]->binding.type = SH_ARB_REG_VERTEXATR;
    m_registers[node]->binding.index = AttribIndex++;
    m_reglist.push_back(m_registers[node]);

    // See comment in the above block.
    ostringstream os;
    os << m_registers[node]->binding.index;
    m_originalVarsMap.get_original_variable(node)->meta("opengl:attribindex", os.str());
  }
}

void ArbCode::allocInputs(const ArbLimits& limits)
{
  // Vertex shaders with opengl:matching set to "generic" need to be bound differently
  if (m_unit == "vertex" && (m_shader->meta("opengl:matching") == "generic"))
    allocGenericInputs(limits);
  else
    allocSemanticInputs(limits);
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
        m_registers[node]->binding.type = specs.binding;
        m_registers[node]->binding.index = m_outputBindings[i];
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

  if (!node->meta("opengl:state").empty())
    {
    m_registers[node]->binding.type = SH_ARB_REG_STATE;
    m_registers[node]->binding.name = node->meta("opengl:state");
    } 
  else
    {
    m_registers[node]->binding.type = SH_ARB_REG_PROGRAMLOC;
    m_registers[node]->binding.index = m_numParamBindings++;
    }

  m_reglist.push_back(m_registers[node]);
  m_numParams++;
}

void ArbCode::allocPalette(const ArbLimits& limits, const ShPaletteNodePtr& palette)
{
  if (m_registers.find(palette) != m_registers.end()) return;

  m_registers[palette] = new ArbReg(SH_ARB_REG_PARAM, m_numParams, palette->name());
  m_registers[palette]->binding.type = SH_ARB_REG_PROGRAMLOC;
  m_registers[palette]->binding.index = m_numParamBindings;
  m_registers[palette]->binding.count = palette->palette_length();
  m_reglist.push_back(m_registers[palette]);
  m_numParams++;
  
  for (size_t i = 0; i < palette->palette_length(); i++) {
    ShVariableNodePtr node = palette->get_node(i);
    SH_DEBUG_ASSERT(m_registers.find(node) == m_registers.end());
    m_registers[node] = new ArbReg(SH_ARB_REG_PARAM, m_numParams + i, node->name());
    m_registers[node]->binding.type = SH_ARB_REG_PROGRAMLOC;
    m_registers[node]->binding.index = m_numParamBindings + i;
    m_reglist.push_back(m_registers[node]);
  }

  m_numParams += palette->palette_length();
  m_numParamBindings += palette->palette_length();
}

void ArbCode::allocConsts(const ArbLimits& limits)
{
  for (ShProgramNode::VarList::const_iterator I = m_shader->constants.begin();
       I != m_shader->constants.end(); ++I) {
    ShVariableNodePtr node = *I;

    // @todo type don't really need to copy here, or shold use the
    // ArbReg's array...but whatever
    ShPointer<ShDataVariant<float, SH_HOST> > variant =
        new ShDataVariant<float, SH_HOST>(node->size());
    variant->set(node->getVariant());

    // TODO: improve efficiency
    RegMap::const_iterator J;
    for (J = m_registers.begin(); J != m_registers.end(); ++J) {
      if (J->second->type != SH_ARB_REG_CONST) continue;
      int f = 0;
      // TODO handle other stuff
      for (int i = 0; i < node->size(); i++) {
        if (J->second->binding.values[i] == (*variant)[i]) f++;
      }
      if (f == node->size()) break;
    }
    if (J == m_registers.end()) {
      m_registers[node] = new ArbReg(SH_ARB_REG_CONST, m_numConsts, node->name());
      m_reglist.push_back(m_registers[node]);
      for (int i = 0; i < 4; i++) {
        m_registers[node]->binding.values[i] = (float)(i < node->size() ? (*variant)[i] : 0.0);
      }
      m_numConsts++;
    } else {
      m_registers[node] = J->second;
    }
  }
}

bool mark(ShLinearAllocator& allocator, ShVariableNodePtr node, int i, bool half)
{
  if (!node) return false;
  if (node->kind() != SH_TEMP) return false;
  if (half && (node->valueType() != SH_HALF)) return false; 
  if (node->hasValues()) return false;
  allocator.mark(node, i);
  return true;
}

bool markable(ShVariableNodePtr node, bool half)
{
  if (!node) return false;
  if (node->kind() != SH_TEMP) return false;
  if (half && (node->valueType() != SH_HALF)) return false; 
  if (node->hasValues()) return false;
  return true;
}

struct ArbScope {
  ArbScope(int start)
    : start(start)
  {
  }
  
  typedef map< ShVariableNode*, bitset<4> > UsageMap;

  typedef set<ShVariableNode*> MarkList;
  
  MarkList need_mark; // Need to mark at end of loop
  int start; // location where loop started
  UsageMap usage_map;
  UsageMap write_map; // locations last written to
};

// @todo type
// cheap hack with half to allocate temps twice, once for non-half temporaries
// once for half temporaries
void ArbCode::allocTemps(const ArbLimits& limits, bool half)
{

  typedef list<ArbScope> ScopeStack;
  ScopeStack scopestack;
  
  ShLinearAllocator allocator(this);

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
          mark(allocator, *I, (int)i, half);
        }
        scopestack.pop_back();
      }

      if (markable(instr.dest.node(), half)) {
        bitset<4> writemask;
        for (int k = 0; k < instr.dest.size(); k++) {
          writemask[instr.dest.swizzle()[k]] = true;
        }
        bitset<4> used;
        for (ScopeStack::iterator S = scopestack.begin(); S != scopestack.end(); ++S) {
          ArbScope& scope = *S;

          if ((used & writemask).any()) {
            mark(allocator, instr.dest.node().object(), scope.start, half);
            scope.need_mark.insert(instr.dest.node().object());
          }
          
          used |= scope.usage_map[instr.dest.node().object()];
        }

        ArbScope& scope = scopestack.back();
        scope.usage_map[instr.dest.node().object()] &= ~writemask;
      }
      
      for (int j = 0; j < ArbInst::max_num_sources; j++) {
        if (!markable(instr.src[j].node(), half)) continue;
        bitset<4> usemask;
        for (int k = 0; k < instr.src[j].size(); k++) {
          usemask[instr.src[j].swizzle()[k]] = true;
        }
        ArbScope& scope = scopestack.back();
        scope.usage_map[instr.src[j].node().object()] |= usemask;
      }
    }
  }
  
  for (size_t i = 0; i < m_instructions.size(); i++) {
    ArbInst instr = m_instructions[i];
    if (instr.op == SH_ARB_REP) {
      scopestack.push_back((int)i);
    }
    if (instr.op == SH_ARB_ENDREP) {
      const ArbScope& scope = scopestack.back();
      for (ArbScope::MarkList::const_iterator I = scope.need_mark.begin();
           I != scope.need_mark.end(); ++I) {
        mark(allocator, *I, (int)i, half);
      }
      scopestack.pop_back();
    }

    for (int j = 0; j < ArbInst::max_num_sources; j++) {
      if (mark(allocator, instr.src[j].node(), (int)i, half)) {
        for (ScopeStack::iterator S = scopestack.begin(); S != scopestack.end(); ++S) {
          ArbScope& scope = *S;
          // Mark uses that weren't recently written to.
          bitset<4> usemask;
          for (int k = 0; k < instr.src[j].size(); k++) {
            usemask[instr.src[j].swizzle()[k]] = true;
          }
          if ((usemask & ~scope.write_map[instr.src[j].node().object()]).any()) {
            mark(allocator, instr.src[j].node(), scope.start, half);
            scope.need_mark.insert(instr.src[j].node().object());
          }
        }
      }
    }

    if (mark(allocator, instr.dest.node(), (int)i, half)) {
      for (ScopeStack::iterator S = scopestack.begin(); S != scopestack.end(); ++S) {
        ArbScope& scope = *S;
        bitset<4> writemask;
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
  }
  
  m_tempRegs.clear();
  int limit;
  if (half) {
    m_numHalfTemps = 0;
    limit = limits.halftemps(); 
  } else {
    m_numTemps = 0;
    limit = limits.temps();
  }

  for (int i = 0; i < limit; i++) {
    m_tempRegs.push_back(i);
  }
  
  allocator.allocate();
  
  m_tempRegs.clear();
}

void ArbCode::allocTextures(const ArbLimits& limits)
{
  list<GLuint> reserved;

  // reserved any texunits specified at the program level
  if (!m_shader->meta("opengl:reservetex").empty()) {
    GLuint index;
    istringstream is(m_shader->meta("opengl:reservetex"));

    while(1) {
      is >> index;
      if (!is) break;
      reserved.push_back(index);
    }
  }
  
  // reserve and allocate any preset texunits
  for (ShProgramNode::TexList::const_iterator I = m_shader->textures.begin();
       I != m_shader->textures.end(); ++I) {
    ShTextureNodePtr node = *I;
    
    if (!node->meta("opengl:texunit").empty() ||
	!node->meta("opengl:preset").empty()) {
      GLuint index;
      istringstream is;

      if (!node->meta("opengl:texunit").empty()) {
	is.str(node->meta("opengl:texunit"));
      } else {
	is.str(node->meta("opengl:preset"));
      }

      is >> index; // TODO: Check for errors
      
      if (find(reserved.begin(), reserved.end(), index) == reserved.end()) {
	m_registers[node] = new ArbReg(SH_ARB_REG_TEXTURE, index, node->name());
	m_registers[node]->preset = true;
	m_reglist.push_back(m_registers[node]);
	reserved.push_back(index);
      } else {
	// TODO: flag some sort of error for multiple tex unit use
      }
    }
  }
  
  // allocate remaining textures units with respect to the reserved list
  for (ShProgramNode::TexList::const_iterator I = m_shader->textures.begin();
       I != m_shader->textures.end(); ++I) {
    ShTextureNodePtr node = *I;
    
    if (node->meta("opengl:texunit").empty() &&
	node->meta("opengl:preset").empty()) {
      GLuint index;
      index = m_numTextures;
      
      // TODO: should there be an upperlimit of the texunit, maybe query
      // OpenGL for the maximum number of texunits
      while(1) {
	if (find(reserved.begin(), reserved.end(), index) == reserved.end()) break;
	else index++;
      }
      
      m_registers[node] = new ArbReg(SH_ARB_REG_TEXTURE, index, node->name());
      m_reglist.push_back(m_registers[node]);
      m_numTextures = index + 1;
    }
  }
}

void ArbCode::bindTextures()
{
  for (ShProgramNode::TexList::const_iterator I = m_shader->textures.begin();
       I != m_shader->textures.end(); ++I) {
    if (!m_registers[*I]->preset) {
      m_texture->bindTexture(*I, GL_TEXTURE0 + m_registers[*I]->index);
    }
  }
}

}
