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
#include "ShArb.hpp"
#include <iostream>
#include <sstream>
#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glx.h>
#undef GL_GLEXT_PROTOTYPES
#include "ShVariable.hpp"
#include "ShDebug.hpp"
#include "ShLinearAllocator.hpp"
#include "ShEnvironment.hpp"
#include "ShTextureNode.hpp"

namespace ShArb {

using namespace SH;
/*
PFNGLPROGRAMSTRINGARBPROC shGlProgramStringARB;
PFNGLACTIVETEXTUREPROC shGlActiveTexture;
PFNGLPROGRAMLOCALPARAMETER4FVARBPROC shGlProgramLocalParameter4fvARB;
PFNGLPROGRAMENVPARAMETER4FVARBPROC shGlProgramEnvParameter4fvARB;
PFNGLGETPROGRAMIVARBPROC shGlGetProgramivARB;
*/

#define shGlProgramStringARB glProgramStringARB
#define shGlActiveTextureARB glActiveTextureARB
#define shGlProgramLocalParameter4fvARB glProgramLocalParameter4fvARB
#define shGlProgramEnvParameter4fvARB glProgramEnvParameter4fvARB
#define shGlGetProgramivARB glGetProgramivARB


unsigned int shArbTargets[] = {
  GL_VERTEX_PROGRAM_ARB,
  GL_FRAGMENT_PROGRAM_ARB
};

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
  SH_ARB_KIL
};

/** Information about the operations from ArbOp.
 */
static struct {
  char* name;
  bool vp, fp;
  int arity;
  bool vector;
} arbOpInfo[] = {
  // VERTEX AND FRAGMENT
  // Vector
  {"ABS", true, true, 1, true},
  {"FLR", true, true, 1, true},
  {"FRC", true, true, 1, true},
  {"LIT", true, true, 1, true},
  {"MOV", true, true, 1, true},

  // Scalar
  {"EX2", true, true, 1, false},
  {"LG2", true, true, 1, false},
  {"RCP", true, true, 1, false},
  {"RSQ", true, true, 1, false},

  // Binary scalar
  {"POW", true, true, 2, false},

  // Binary vector
  {"ADD", true, true, 2, true},
  {"DP3", true, true, 2, true},
  {"DP4", true, true, 2, true},
  {"DPH", true, true, 2, true},
  {"DST", true, true, 2, true},
  {"MAX", true, true, 2, true},
  {"MIN", true, true, 2, true},
  {"MUL", true, true, 2, true},
  {"SGE", true, true, 2, true},
  {"SLT", true, true, 2, true},
  {"SUB", true, true, 2, true},
  {"XPD", true, true, 2, true},

  // Trinary
  {"MAD", true, true, 3, true},

  // Swizzling
  {"SWZ", true, true, 2, true},

  // VERTEX ONLY
  // Scalar
  {"EXP", true, false, 2, false},
  {"LOG", true, false, 2, false},
  
  // FRAGMENT ONLY
  // Scalar
  {"COS", false, true, 1, false},
  {"SIN", false, true, 1, false},
  {"SCS", false, true, 1, false},

  // Trinary
  {"CMP", false, true, 3, true},
  {"LRP", false, true, 3, true},

  // Sampling
  {"TEX", false, true, 3, true},
  {"TXP", false, true, 3, true},
  {"TXB", false, true, 3, true},

  // KIL
  {"KIL", false, true, 0, false}
};

/** An ARB instruction.
 */
struct ArbInst {
  ArbInst(ArbOp op, const ShVariable& dest)
    : op(op), dest(dest)
  {
  }

  ArbInst(ArbOp op, const ShVariable& dest, const ShVariable& src0)
    : op(op), dest(dest)
  {
    src[0] = src0;
  }

  ArbInst(ArbOp op, const ShVariable& dest, const ShVariable& src0,
          const ShVariable& src1)
    : op(op), dest(dest)
  {
    src[0] = src0;
    src[1] = src1;
  }
  ArbInst(ArbOp op, const ShVariable& dest, const ShVariable& src0,
          const ShVariable& src1, const ShVariable& src2)
    : op(op), dest(dest)
  {
    src[0] = src0;
    src[1] = src1;
    src[2] = src2;
  }
  
  ArbOp op;
  ShVariable dest;
  ShVariable src[3];
};

/** Information about ShArbRegType members.
 */
struct {
  char* name;
  char* estName;
} shArbRegTypeInfo[] = {
  {"ATTRIB", "i"},
  {"PARAM", "u"},
  {"TEMP", "t"},
  {"ADDRESS", "a"},
  {"OUTPUT", "o"},
  {"PARAM", "c"},
  {"<texture>", "<texture>"}
};

/** Possible bindings for a register (see ARB spec).
 */
enum ShArbRegBinding {
  // VERTEX and FRAGMENT
  // Parameter
  SH_ARB_REG_PARAMLOC,
  SH_ARB_REG_PARAMENV,
  // Output
  SH_ARB_REG_RESULTCOL,

  // VERTEX
  // Input
  SH_ARB_REG_VERTEXPOS,
  SH_ARB_REG_VERTEXWGT,
  SH_ARB_REG_VERTEXNRM,
  SH_ARB_REG_VERTEXCOL,
  SH_ARB_REG_VERTEXFOG,
  SH_ARB_REG_VERTEXTEX,
  SH_ARB_REG_VERTEXMAT,
  SH_ARB_REG_VERTEXATR,
  // Output
  SH_ARB_REG_RESULTPOS,
  SH_ARB_REG_RESULTFOG,
  SH_ARB_REG_RESULTPTS, ///< Result point size
  SH_ARB_REG_RESULTTEX,

  // FRAGMENT
  // Input
  SH_ARB_REG_FRAGMENTCOL,
  SH_ARB_REG_FRAGMENTTEX,
  SH_ARB_REG_FRAGMENTFOG,
  SH_ARB_REG_FRAGMENTPOS,
  // Output
  SH_ARB_REG_RESULTDPT,

  SH_ARB_REG_NONE
};

/** Information about the ShArbRegBinding members.
 */
struct {
  ShArbRegType type;
  char* name;
  bool indexed;
} shArbRegBindingInfo[] = {
  {SH_ARB_REG_PARAM, "program.local", true},
  {SH_ARB_REG_PARAM, "program.env", true},
  {SH_ARB_REG_OUTPUT, "result.color", false}, // TODO: Special case?
  
  {SH_ARB_REG_ATTRIB, "vertex.position", false},
  {SH_ARB_REG_ATTRIB, "vertex.weight", true},
  {SH_ARB_REG_ATTRIB, "vertex.normal", false},
  {SH_ARB_REG_ATTRIB, "vertex.color", false}, // TODO: Special case?
  {SH_ARB_REG_ATTRIB, "vertex.fogcoord", false},
  {SH_ARB_REG_ATTRIB, "vertex.texcoord", true},
  {SH_ARB_REG_ATTRIB, "vertex.matrixindex", true},
  {SH_ARB_REG_ATTRIB, "vertex.attrib", true},
  {SH_ARB_REG_OUTPUT, "result.position", false},
  {SH_ARB_REG_OUTPUT, "result.fogcoord", false},
  {SH_ARB_REG_OUTPUT, "result.pointsize", false},
  {SH_ARB_REG_OUTPUT, "result.texcoord", true},

  {SH_ARB_REG_ATTRIB, "fragment.color", false}, // TODO: Special case?
  {SH_ARB_REG_ATTRIB, "fragment.texcoord", true},
  {SH_ARB_REG_ATTRIB, "fragment.fogcoord", false},
  {SH_ARB_REG_ATTRIB, "fragment.position", false},
  {SH_ARB_REG_OUTPUT, "result.depth", false},

  {SH_ARB_REG_ATTRIB, "<nil>", false},
};

struct ArbBindingSpecs {
  ShArbRegBinding binding;
  int maxBindings;
  ShVariableSpecialType specialType;
  bool allowGeneric;
};

ArbBindingSpecs shArbVertexAttribBindingSpecs[] = {
  {SH_ARB_REG_VERTEXPOS, 1, SH_VAR_POSITION, false},
  {SH_ARB_REG_VERTEXNRM, 1, SH_VAR_NORMAL, false},
  {SH_ARB_REG_VERTEXCOL, 1, SH_VAR_COLOR, false},
  {SH_ARB_REG_VERTEXTEX, 8, SH_VAR_TEXCOORD, true},
  {SH_ARB_REG_VERTEXFOG, 1, SH_VAR_ATTRIB, true},
  {SH_ARB_REG_NONE, 0, SH_VAR_ATTRIB, true}
};

ArbBindingSpecs shArbFragmentAttribBindingSpecs[] = {
  {SH_ARB_REG_FRAGMENTPOS, 1, SH_VAR_POSITION, false},
  {SH_ARB_REG_FRAGMENTCOL, 1, SH_VAR_COLOR, false},
  {SH_ARB_REG_FRAGMENTTEX, 8, SH_VAR_TEXCOORD, true},
  {SH_ARB_REG_FRAGMENTFOG, 1, SH_VAR_ATTRIB, true},
  {SH_ARB_REG_NONE, 0, SH_VAR_ATTRIB, true}
};

ArbBindingSpecs* shArbAttribBindingSpecs[] = {
  shArbVertexAttribBindingSpecs,
  shArbFragmentAttribBindingSpecs
};

ArbBindingSpecs shArbVertexOutputBindingSpecs[] = {
  {SH_ARB_REG_RESULTPOS, 1, SH_VAR_POSITION, false},
  {SH_ARB_REG_RESULTCOL, 1, SH_VAR_COLOR, false},
  {SH_ARB_REG_RESULTTEX, 8, SH_VAR_TEXCOORD, true},
  {SH_ARB_REG_RESULTFOG, 1, SH_VAR_ATTRIB, true},
  {SH_ARB_REG_RESULTPTS, 1, SH_VAR_ATTRIB, true},
  {SH_ARB_REG_NONE, 0, SH_VAR_ATTRIB}
};

ArbBindingSpecs shArbFragmentOutputBindingSpecs[] = {
  {SH_ARB_REG_RESULTCOL, 1, SH_VAR_COLOR, false},
  {SH_ARB_REG_RESULTDPT, 1, SH_VAR_ATTRIB, false},
  {SH_ARB_REG_NONE, 0, SH_VAR_ATTRIB}
};

ArbBindingSpecs* shArbOutputBindingSpecs[] = {
  shArbVertexOutputBindingSpecs,
  shArbFragmentOutputBindingSpecs
};

/** An ARB register.
 */
struct ArbReg {
  ArbReg()
    : type(SH_ARB_REG_TEMP), index(-1), binding(SH_ARB_REG_NONE), bindingIndex(-1)
  {
  }
  
  ArbReg(ShArbRegType type, int index)
    : type(type), index(index), binding(SH_ARB_REG_NONE), bindingIndex(-1)
  {
  }

  ShArbRegType type;
  int index;

  union {
    struct {
      ShArbRegBinding binding;
      int bindingIndex;
    };
    float values[4];
  };

  friend std::ostream& operator<<(std::ostream& out, const ArbReg& reg);

  /// Print a declaration for this register
  std::ostream& printDecl(std::ostream& out) const
  {
    out << shArbRegTypeInfo[type].name << " " << *this;
    if (type == SH_ARB_REG_CONST) {
      out << " = " << "{";
      for (int i = 0; i < 4; i++) {
        if (i) out << ", ";
        out << values[i];
      }
      out << "}";
    } else if (binding != SH_ARB_REG_NONE) {
      out << " = " << shArbRegBindingInfo[binding].name;
      if (shArbRegBindingInfo[binding].indexed) {
        out << "[" << bindingIndex << "]";
      }
    }
    out << ";";
    return out;
  }
};

/** Output a use of an arb register.
 */
std::ostream& operator<<(std::ostream& out, const ArbReg& reg)
{
  out << shArbRegTypeInfo[reg.type].estName << reg.index;
  return out;
}

using namespace SH;

static SH::ShRefCount<ArbBackend> instance = new ArbBackend();

ArbCode::ArbCode(ArbBackendPtr backend, const ShProgram& shader)
  : m_backend(backend), m_shader(shader),
    m_numTemps(0), m_numInputs(0), m_numOutputs(0), m_numParams(0), m_numConsts(0),
    m_numTextures(0)
{
}

ArbCode::~ArbCode()
{
}

void ArbCode::generate()
{
  m_shader->ctrlGraph->entry()->clearMarked();
  genNode(m_shader->ctrlGraph->entry());
  m_shader->ctrlGraph->entry()->clearMarked();
  allocRegs();
}

bool ArbCode::allocateRegister(const ShVariableNodePtr& var)
{
  if (!var) return true;
  if (var->kind() != SH_VAR_TEMP) return true;
  if (var->uniform()) return true;

  if (m_tempRegs.empty()) {
    SH_DEBUG_WARN("Out of temporaries!");
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
  if (var->kind() != SH_VAR_TEMP) return;
  if (var->uniform()) return;

  SH_DEBUG_ASSERT(m_registers.find(var) != m_registers.end());
  m_tempRegs.push_front(m_registers[var].index);
}

void ArbCode::upload()
{
  // TODO: anything?
}

void ArbCode::bind()
{
  std::ostringstream out;
  print(out);
  std::string text = out.str();
  shGlProgramStringARB(shArbTargets[m_shader->kind()], GL_PROGRAM_FORMAT_ASCII_ARB, text.size(), text.c_str());
  int error = glGetError();
  if (error == GL_INVALID_OPERATION) {
    int pos = -1;
    glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &pos);
    const unsigned char* message = glGetString(GL_PROGRAM_ERROR_STRING_ARB);
    SH_DEBUG_WARN("Error at character " << pos);
    SH_DEBUG_WARN("Message: " << message);
    SH_DEBUG_WARN("Code (30 chars): " << text.substr(pos, 30));
  }
  if (error != GL_NO_ERROR) {
    // TODO: Better error handling.
    SH_DEBUG_ERROR("Error binding ARB program: " << error);
  }
  SH::ShEnvironment::boundShader[m_shader->kind()] = m_shader;

  // Initialize constants
  for (RegMap::const_iterator I = m_registers.begin(); I != m_registers.end(); ++I) {
    ShVariableNodePtr node = I->first;
    ArbReg reg = I->second;
    if (node->hasValues() && reg.type == SH_ARB_REG_PARAM) {
      updateUniform(node);
    }
  }
  for (ShProgramNode::VarList::const_iterator I = m_shader->textures.begin(); I != m_shader->textures.end();
       ++I) {
    ShTextureNodePtr texture = *I;
    if (!texture) {
      SH_DEBUG_WARN((*I)->name() << " is not a valid texture!");
      continue;
    }

    RegMap::const_iterator texRegIterator = m_registers.find(texture);

    SH_DEBUG_ASSERT(texRegIterator != m_registers.end());
    
    const ArbReg& texReg = texRegIterator->second;

    SH_DEBUG_PRINT("Setting active texture to " << texReg.index << ", my shader is " << m_shader.object());
    
    shGlActiveTextureARB(GL_TEXTURE0 + texReg.index);
    // TODO: Only do this once.
    unsigned int type;
    switch (texture->dims()) {
    case SH_TEXTURE_1D:
      type = GL_TEXTURE_1D;
      break;
    case SH_TEXTURE_2D:
      type = GL_TEXTURE_2D;
      break;
    case SH_TEXTURE_3D:
      type = GL_TEXTURE_3D;
      break;
      //    case SH_TEXTURE_CUBE:
      //      type = GL_TEXTURE_CUBE;
      //      break;
    }
    
    SH_DEBUG_PRINT("Binding texture " << texReg.index << " with texture unit " << texReg.bindingIndex);

    glBindTexture(type, texReg.bindingIndex);

    SH_DEBUG_PRINT("Sending texture image");
    // TODO: Other types of textures.
    // TODO: Element Format
    // TODO: sampling/filtering
    // TODO: wrap/
    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP);
    unsigned int format;

    switch (texture->elements()) {
    case 1:
      format = GL_LUMINANCE;
      break;
    case 2:
      format = GL_LUMINANCE_ALPHA;
      break;
    case 3:
      format = GL_RGB;
      break;
    case 4:
      format = GL_RGBA;
      break;
    default:
      format = 0;
      break;
    }

    switch(type) {
    case GL_TEXTURE_1D:
      glTexImage1D(type, 0, texture->elements(), texture->width(), 0, format, GL_FLOAT, texture->data());
      break;
    case GL_TEXTURE_2D:
      glTexImage2D(type, 0, texture->elements(), texture->width(), texture->height(), 0, format, GL_FLOAT,
                   texture->data());
      break;
    case GL_TEXTURE_3D:
      glTexImage3D(type, 0, texture->elements(), texture->width(), texture->height(), texture->depth(),
                   0, format, GL_FLOAT, texture->data());
      break;
    default:
      SH_DEBUG_WARN("Texture type not handled by ARB backend");
      break;
    }
    int error = glGetError();
    if (error != GL_NO_ERROR) {
      SH_DEBUG_ERROR("Error loading texture: " << error);
      switch(error) {
      case GL_INVALID_ENUM:
        SH_DEBUG_ERROR("INVALID_ENUM");
        break;
      case GL_INVALID_VALUE:
        SH_DEBUG_ERROR("INVALID_VALUE");
        break;
      case GL_INVALID_OPERATION:
        SH_DEBUG_ERROR("INVALID_OPERATION");
        break;
      }
    }
    
    //    glEnable(type);
  }
}

void ArbCode::updateUniform(const ShVariableNodePtr& uniform)
{
  // TODO: textures?
  if (!uniform) return;
  RegMap::const_iterator I = m_registers.find(uniform);
  if (I == m_registers.end()) return;
  
  const ArbReg& reg = I->second;
  
  float values[4];
  int i;
  for (i = 0; i < uniform->size(); i++) {
    values[i] = uniform->getValue(i);
  }
  for (; i < 4; i++) {
    values[i] = 0.0;
  }
  
  if (reg.type != SH_ARB_REG_PARAM) return;
  switch(reg.binding) {
  case SH_ARB_REG_PARAMLOC:
    shGlProgramLocalParameter4fvARB(shArbTargets[m_shader->kind()], reg.bindingIndex, values);
    break;
  case SH_ARB_REG_PARAMENV:
    shGlProgramEnvParameter4fvARB(shArbTargets[m_shader->kind()], reg.bindingIndex, values);
    break;
  default:
    return;
  }
}

std::ostream& ArbCode::printVar(std::ostream& out, bool dest, const ShVariable& var) const
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
  int i;
  for (i = 0; i < std::min(var.swizzle().size(), 4); i++) {
    out << swizChars[var.swizzle()[i]];
  }
  for (; !dest && i != 1 && i < 4; i++) {
    out << swizChars[i];
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

  ShTextureNodePtr texture = instr.src[1].node();
  RegMap::const_iterator texRegIt = m_registers.find(instr.src[1].node());
  SH_DEBUG_ASSERT(texRegIt != m_registers.end());

  const ArbReg& texReg = texRegIt->second;
  
  out << "  ";
  out << arbOpInfo[instr.op].name << " ";
  printVar(out, true, instr.dest) << ", ";
  printVar(out, false, instr.src[0]) << ", ";
  out << "texture[" << texReg.index << "], ";
  switch (texture->dims()) {
  case SH_TEXTURE_1D:
    out << "1D";
    break;
  case SH_TEXTURE_2D:
    out << "2D"; // TODO: RECT
    break;
  case SH_TEXTURE_3D:
    out << "3D";
    break;
  case SH_TEXTURE_CUBE:
    out << "CUBE";
    break;
  }
  out << ";" << std::endl;
  return true;
}

std::ostream& ArbCode::print(std::ostream& out)
{
  LineNumberer endl;

  // Print version header
  switch(m_shader->kind()) {
  case 0:
    out << "!!ARBvp1.0" << endl;
    break;
  case 1:
    out << "!!ARBfp1.0" << endl;
    break;
  }

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
      printVar(out, true, I->dest);
      for (int i = 0; i < arbOpInfo[I->op].arity; i++) {
        out << ", ";
        printVar(out, false, I->src[i]);
      }
      out << ';';
      out << endl;
    }
  }

  out << "END" << endl;
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
      m_instructions.push_back(ArbInst(SH_ARB_MOV, stmt.dest, stmt.src1));
      break;
    case SH_OP_NEG:
      m_instructions.push_back(ArbInst(SH_ARB_MOV, stmt.dest, -stmt.src1));
      break;
    case SH_OP_ADD:
      m_instructions.push_back(ArbInst(SH_ARB_ADD, stmt.dest, stmt.src1, stmt.src2));
      break;
    case SH_OP_MUL:
      {
      if (stmt.src1.size() != 1 || stmt.src2.size() != 1) {
        if (stmt.src1.size() == 1) {
          int* swizzle = new int[stmt.src2.size()];
          for (int i = 0; i < stmt.src2.size(); i++) swizzle[i] = 0;
          m_instructions.push_back(ArbInst(SH_ARB_MUL, stmt.dest,
                                                 stmt.src1(stmt.src2.size(), swizzle), stmt.src2));
          delete [] swizzle;
          break;
        } else if (stmt.src2.size() == 1) {
          int* swizzle = new int[stmt.src1.size()];
          for (int i = 0; i < stmt.src1.size(); i++) swizzle[i] = 0;
          m_instructions.push_back(ArbInst(SH_ARB_MUL, stmt.dest, stmt.src1,
                                                 stmt.src2(stmt.src1.size(), swizzle)));
          delete [] swizzle;
          break;
        }
      }

      m_instructions.push_back(ArbInst(SH_ARB_MUL, stmt.dest, stmt.src1, stmt.src2));
      break;
      }
    case SH_OP_DIV:
      {
        ShVariable rcp(new ShVariableNode(SH_VAR_TEMP, stmt.src2.size()));
        m_instructions.push_back(ArbInst(SH_ARB_RCP, rcp, stmt.src2));

        if (rcp.size() == 1 && stmt.src1.size() != 1) {
          int* swizzle = new int[stmt.src1.size()];
          for (int i = 0; i < stmt.src1.size(); i++) swizzle[i] = 0;
          m_instructions.push_back(ArbInst(SH_ARB_MUL, stmt.dest, stmt.src1,
                                                 rcp(stmt.src1.size(), swizzle)));
          delete [] swizzle;
        } else {
          m_instructions.push_back(ArbInst(SH_ARB_MUL, stmt.dest, stmt.src1, rcp));
        }
        break;
      }
      /*
      {
      ShVariable rcp(new ShVariableNode(SH_VAR_TEMP, stmt.src2.size()));
      m_instructions.push_back(ArbInst(SH_ARB_RCP, rcp, stmt.src2));
      m_instructions.push_back(ArbInst(SH_ARB_MUL, stmt.dest, stmt.src1, rcp));
      }
      */
      break;
    case SH_OP_DOT:
      {
        if (stmt.src1.size() == 3) {
          m_instructions.push_back(ArbInst(SH_ARB_DP3, stmt.dest, stmt.src1, stmt.src2));
        } else if (stmt.src1.size() == 4) {
          m_instructions.push_back(ArbInst(SH_ARB_DP4, stmt.dest, stmt.src1, stmt.src2));
        } else if (stmt.src1.size() == 1) {
          m_instructions.push_back(ArbInst(SH_ARB_MUL, stmt.dest, stmt.src1, stmt.src2));
        } else {
          ShVariable mul(new ShVariableNode(SH_VAR_TEMP, stmt.src1.size()));
          m_instructions.push_back(ArbInst(SH_ARB_MUL, mul, stmt.src1, stmt.src2));
          m_instructions.push_back(ArbInst(SH_ARB_ADD, stmt.dest, mul(0), mul(1)));
          for (int i = 2; i < stmt.src1.size(); i++) {
            m_instructions.push_back(ArbInst(SH_ARB_ADD, stmt.dest, stmt.dest, mul(i)));
          }
        }
      }
      break;
    case SH_OP_MAX:
      m_instructions.push_back(ArbInst(SH_ARB_MAX, stmt.dest, stmt.src1, stmt.src2));
      break;
    case SH_OP_MIN:
      m_instructions.push_back(ArbInst(SH_ARB_MIN, stmt.dest, stmt.src1, stmt.src2));
      break;
    case SH_OP_NORM:
      { // TODO: other than 3 components
        ShVariable mul(new ShVariableNode(SH_VAR_TEMP, 1));
        m_instructions.push_back(ArbInst(SH_ARB_DP3, mul, stmt.src1, stmt.src1));
        m_instructions.push_back(ArbInst(SH_ARB_RSQ, mul, mul));
        m_instructions.push_back(ArbInst(SH_ARB_MUL, stmt.dest, mul, stmt.src1));
      }
      break;
    case SH_OP_SLT:
      m_instructions.push_back(ArbInst(SH_ARB_SLT, stmt.dest, stmt.src1, stmt.src2));
      break;
    case SH_OP_SGT:
      m_instructions.push_back(ArbInst(SH_ARB_SGE, stmt.dest, stmt.src1, stmt.src2));
      break;
    case SH_OP_SQRT:
      {
        ShVariable rcp(new ShVariableNode(SH_VAR_TEMP, stmt.src1.size()));
        m_instructions.push_back(ArbInst(SH_ARB_RSQ, rcp, stmt.src1));
        m_instructions.push_back(ArbInst(SH_ARB_RCP, stmt.dest, rcp));
      break;
      }
    case SH_OP_POW:
      // TODO: scalarize?
      m_instructions.push_back(ArbInst(SH_ARB_POW, stmt.dest, stmt.src1, stmt.src2));
      break;
    case SH_OP_TEX:
      m_instructions.push_back(ArbInst(SH_ARB_TEX, stmt.dest, stmt.src2, stmt.src1));
      break;
    case SH_OP_COND:
      m_instructions.push_back(ArbInst(SH_ARB_CMP, stmt.dest, -stmt.src1, stmt.src2, stmt.src3));
      break;
    case SH_OP_KIL:
      m_instructions.push_back(ArbInst(SH_ARB_KIL, -stmt.dest));
      break;
    default:
      SH_DEBUG_WARN(opInfo[stmt.op].name << " not handled by ARB backend");
    }
  }

  genNode(node->follower);
}

void ArbCode::allocRegs()
{
  allocInputs();
  
  allocOutputs();
  
  for (ShProgramNode::VarList::const_iterator I = m_shader->uniforms.begin();
       I != m_shader->uniforms.end(); ++I) {
    allocParam(*I);
  }

  allocConsts();
  
  allocTemps();

  allocTextures();
}

void ArbCode::bindSpecial(const ShProgramNode::VarList::const_iterator& begin,
                          const ShProgramNode::VarList::const_iterator& end,
                          const ArbBindingSpecs& specs, 
                          std::vector<int>& bindings,
                          ShArbRegType type, int& num)
{
  bindings.push_back(0);
  
  if (specs.specialType == SH_VAR_ATTRIB) return;
  
  for (ShProgramNode::VarList::const_iterator I = begin; I != end; ++I) {
    ShVariableNodePtr node = *I;
    
    if (m_registers.find(node) != m_registers.end()) continue;
    if (node->specialType() != specs.specialType) continue;
    
    m_registers[node] = ArbReg(type, num++);
    m_registers[node].binding = specs.binding;
    m_registers[node].bindingIndex = bindings.back();
    
    bindings.back()++;
    if (bindings.back() == specs.maxBindings) break;
  }    
}

void ArbCode::allocInputs()
{
  // First, try to assign some "special" output register bindings
  for (int i = 0; shArbAttribBindingSpecs[m_shader->kind()][i].binding != SH_ARB_REG_NONE; i++) {
    bindSpecial(m_shader->inputs.begin(), m_shader->inputs.end(),
                shArbAttribBindingSpecs[m_shader->kind()][i], m_inputBindings,
                SH_ARB_REG_ATTRIB, m_numInputs);
  }
  
  for (ShProgramNode::VarList::const_iterator I = m_shader->inputs.begin();
       I != m_shader->inputs.end(); ++I) {
    ShVariableNodePtr node = *I;
    if (m_registers.find(node) != m_registers.end()) continue;
    m_registers[node] = ArbReg(SH_ARB_REG_ATTRIB, m_numInputs++);

    // Binding
    for (int i = 0; shArbAttribBindingSpecs[m_shader->kind()][i].binding != SH_ARB_REG_NONE; i++) {
      const ArbBindingSpecs& specs = shArbAttribBindingSpecs[m_shader->kind()][i];

      if (specs.allowGeneric && m_inputBindings[i] < specs.maxBindings) {
        m_registers[node].binding = specs.binding;
        m_registers[node].bindingIndex = m_inputBindings[i];
        m_inputBindings[i]++;
        break;
      }
    }
  }
}

void ArbCode::allocOutputs()
{
  // First, try to assign some "special" output register bindings
  for (int i = 0; shArbOutputBindingSpecs[m_shader->kind()][i].binding != SH_ARB_REG_NONE; i++) {
    bindSpecial(m_shader->outputs.begin(), m_shader->outputs.end(),
                shArbOutputBindingSpecs[m_shader->kind()][i], m_outputBindings,
                SH_ARB_REG_OUTPUT, m_numOutputs);
  }
  
  for (ShProgramNode::VarList::const_iterator I = m_shader->outputs.begin();
       I != m_shader->outputs.end(); ++I) {
    ShVariableNodePtr node = *I;
    if (m_registers.find(node) != m_registers.end()) continue;
    m_registers[node] = ArbReg(SH_ARB_REG_OUTPUT, m_numOutputs++);

    // Binding
    for (int i = 0; shArbOutputBindingSpecs[m_shader->kind()][i].binding != SH_ARB_REG_NONE; i++) {
      const ArbBindingSpecs& specs = shArbOutputBindingSpecs[m_shader->kind()][i];

      if (specs.allowGeneric && m_outputBindings[i] < specs.maxBindings) {
        m_registers[node].binding = specs.binding;
        m_registers[node].bindingIndex = m_outputBindings[i];
        m_outputBindings[i]++;
        break;
      }
    }
  }
}

void ArbCode::allocParam(ShVariableNodePtr node)
{
  // TODO: Check if we reached maximum
  if (m_registers.find(node) != m_registers.end()) return;
  m_registers[node] = ArbReg(SH_ARB_REG_PARAM, m_numParams);
  m_registers[node].binding = SH_ARB_REG_PARAMLOC;
  m_registers[node].bindingIndex = m_numParams;
  m_numParams++;
}

void ArbCode::allocConsts()
{
  for (ShProgramNode::VarList::const_iterator I = m_shader->constants.begin();
       I != m_shader->constants.end(); ++I) {
    ShVariableNodePtr node = *I;
    m_registers[node] = ArbReg(SH_ARB_REG_CONST, m_numConsts);
    for (int i = 0; i < 4; i++) {
      m_registers[node].values[i] = (i < node->size() ? node->getValue(i) : 0.0);
    }
    m_numConsts++;
  }
}

void ArbCode::allocTextures()
{
  
  for (ShProgramNode::VarList::const_iterator I = m_shader->textures.begin();
       I != m_shader->textures.end(); ++I) {
    ShTextureNodePtr node = *I;
    int binding, index;
    index = m_numTextures;
    glGenTextures(1, (GLuint*)&binding);
    m_registers[node] = ArbReg(SH_ARB_REG_TEXTURE, index);
    m_registers[node].bindingIndex = binding;
    m_numTextures++;
  }
}

void mark(ShLinearAllocator& allocator, ShVariableNodePtr node, int i)
{
  if (!node) return;
  if (node->kind() != SH_VAR_TEMP) return;
  if (node->hasValues()) return;
  allocator.mark(node, i);
}

void ArbCode::allocTemps()
{
  ShLinearAllocator allocator(this);

  for (std::size_t i = 0; i < m_instructions.size(); i++) {
    ArbInst instr = m_instructions[i];
    mark(allocator, instr.dest.node(), i);
    for (int j = 0; j < 3; j++) {
      mark(allocator, instr.src[j].node(), i);
    }
  }
  
  m_tempRegs.clear();
  m_numTemps = 0;
  for (int i = 0; i < m_backend->temps(m_shader->kind()); i++) {
    m_tempRegs.push_back(i);
  }
  
  allocator.allocate();
  
  m_tempRegs.clear();
}

ArbBackend::ArbBackend()
{
  /*
#define SH_ARB_GL_EXT_LOOKUP(type, name) shGl ## name = (PFNGL ## type ## PROC)glXGetProcAddressARB((const GLubyte*) "gl" # name ); SH_DEBUG_PRINT("Looked up " << # name << " as " << &shGl ## name)
  SH_ARB_GL_EXT_LOOKUP(PROGRAMSTRINGARB, ProgramStringARB);
  SH_ARB_GL_EXT_LOOKUP(ACTIVETEXTURE, ActiveTextureARB);
  SH_ARB_GL_EXT_LOOKUP(PROGRAMLOCALPARAMETER4FVARB, ProgramLocalParameter4fvARB);
  SH_ARB_GL_EXT_LOOKUP(PROGRAMENVPARAMETER4FVARB, ProgramEnvParameter4fvARB);
  SH_ARB_GL_EXT_LOOKUP(GETPROGRAMIVARB, GetProgramivARB);
#undef SH_ARB_GL_EXT_LOOKUP  
  */
  // TODO Max TEX instructions, texture indirections
  for (int i = 0; i < 2; i++) {
    unsigned int target = shArbTargets[i];
    m_instrs[i] = (!i ? 128 : 48);
    shGlGetProgramivARB(target, GL_MAX_PROGRAM_INSTRUCTIONS_ARB, &m_instrs[i]);
    SH_DEBUG_PRINT("instrs[" << i << "] = " << m_instrs[i]);
    m_temps[i] = (!i ? 12 : 16);
    shGlGetProgramivARB(target, GL_MAX_PROGRAM_TEMPORARIES_ARB, &m_temps[i]);
    SH_DEBUG_PRINT("temps[" << i << "] = " << m_temps[i]);
    m_attribs[i] = (!i ? 16 : 10);
    shGlGetProgramivARB(target, GL_MAX_PROGRAM_ATTRIBS_ARB, &m_attribs[i]);
    SH_DEBUG_PRINT("attribs[" << i << "] = " << m_attribs[i]);
    m_params[i] = (!i ? 96 : 24);
    shGlGetProgramivARB(target, GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB, &m_params[i]);
    SH_DEBUG_PRINT("params[" << i << "] = " << m_params[i]);
    m_texs[i] = (!i ? 0 : 24);
    if (i) shGlGetProgramivARB(target, GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB, &m_texs[i]);
    SH_DEBUG_PRINT("texs[" << i << "] = " << m_texs[i]);
  }
}

ArbBackend::~ArbBackend()
{
}

std::string ArbBackend::name() const
{
  return "arb";
}

ShBackendCodePtr ArbBackend::generateCode(const ShProgram& shader)
{
  SH_DEBUG_ASSERT(shader.object());
  ArbCodePtr code = new ArbCode(this, shader);
  code->generate();
  return code;
}

}
