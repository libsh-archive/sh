#include "ShArb.hpp"
#include <iostream>
#include <sstream>
#include <GL/gl.h>
#include "ShVariable.hpp"
#include "ShDebug.hpp"

namespace ShArb {

using namespace SH;

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

/** Possible register types in the ARB spec.
 */
enum ShArbRegType {
  SH_ARB_REG_ATTRIB,
  SH_ARB_REG_PARAM,
  SH_ARB_REG_TEMP,
  SH_ARB_REG_ADDRESS,
  SH_ARB_REG_OUTPUT
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
  {"OUTPUT", "o"}
};

/** Possible bindings for a register (see ARB spec).
 */
enum ShArbRegBinding {
  // VERTEX and FRAGMENT
  // Parameter
  SH_ARB_REG_PARAMPRG,
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
  {SH_ARB_REG_PARAM, "program.env", true},
  {SH_ARB_REG_OUTPUT, "result.color", false}, // TODO: Special case?
  
  {SH_ARB_REG_ATTRIB, "vertex.position", false},
  {SH_ARB_REG_ATTRIB, "vertex.weight", true},
  {SH_ARB_REG_ATTRIB, "vertex.normal", false},
  {SH_ARB_REG_ATTRIB, "vertex.color", false}, // TODO: Special case?
  {SH_ARB_REG_ATTRIB, "vertex.fogcoord", false},
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

/** An ARB register.
 */
struct ArbReg {
  ArbReg(ShArbRegType type, int index)
    : type(type), index(index), binding(SH_ARB_REG_NONE)
  {
  }

  ShArbRegType type;
  int index;
  ShArbRegBinding binding;
  int bindingIndex;

  friend std::ostream& operator<<(std::ostream& out, const ArbReg& reg);

  /// Print a declaration for this register
  std::ostream& printDecl(std::ostream& out) {
    out << shArbRegTypeInfo[type].name << " " << *this;
    if (binding != SH_ARB_REG_NONE) {
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

ArbCode::ArbCode(ArbBackendPtr backend, const ShShader& shader)
  : m_backend(backend), m_shader(shader)
{
  shader->ctrlGraph->entry()->clearMarked();
  genNode(shader->ctrlGraph->entry());
  shader->ctrlGraph->entry()->clearMarked();
  allocRegs();
}

ArbCode::~ArbCode()
{
}

bool ArbCode::allocateRegister(const ShVariableNodePtr& var)
{
  // TODO
  return true;
}

void ArbCode::freeRegister(const ShVariableNodePtr& var)
{
  // TODO
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
  unsigned int target = (m_shader->kind() == 0 ? GL_VERTEX_PROGRAM_ARB : GL_FRAGMENT_PROGRAM_ARB);
  glProgramStringARB(target, GL_PROGRAM_FORMAT_ASCII_ARB, text.size(), text.c_str());
  // TODO: Check for errors
}

void ArbCode::updateUniform(const ShVariableNodePtr& uniform)
{
  // TODO
}

std::ostream& ArbCode::print(std::ostream& out)
{
  using std::endl;
  switch(m_shader->kind()) {
  case 0:
    out << "!!ARBvp1.0" << endl;
    break;
  case 1:
    out << "!!ARBfp1.0" << endl;
    break;
  }

  for (ArbInstList::const_iterator I = m_instructions.begin();
       I != m_instructions.end(); ++I) {
    out << "  ";
    out << arbOpInfo[I->op].name << " " << I->dest.name() << I->dest.swizzle();
    for (int i = 0; i < arbOpInfo[I->op].arity; i++) {
      out << ", " << I->src[i].name() << I->src[i].swizzle();
    }
    out << ';';
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
      m_instructions.push_back(ArbInst(SH_ARB_MOV, stmt.dest, stmt.src1));
      break;
    case SH_OP_NEG:
      m_instructions.push_back(ArbInst(SH_ARB_MOV, stmt.dest, -stmt.src1));
      break;
    case SH_OP_ADD:
      m_instructions.push_back(ArbInst(SH_ARB_ADD, stmt.dest, stmt.src1, stmt.src2));
      break;
    case SH_OP_MUL:
      m_instructions.push_back(ArbInst(SH_ARB_MUL, stmt.dest, stmt.src1, stmt.src2));
      break;
    case SH_OP_DIV:
      {
      ShVariable rcp(new ShVariableNode(SH_VAR_TEMP, stmt.src2.size()));
      m_instructions.push_back(ArbInst(SH_ARB_RCP, rcp, stmt.src2));
      m_instructions.push_back(ArbInst(SH_ARB_MUL, stmt.dest, stmt.src1, rcp));
      }
      break;
    default:
      SH_DEBUG_WARN(opInfo[stmt.op].name << " not handled by ARB backend");
    }
  }

  genNode(node->follower);
}

void ArbCode::allocRegs()
{
  for (ShShaderNode::VarList::const_iterator I = m_shader->inputs.begin();
       I != m_shader->inputs.end(); ++I) {
    // Allocate an input register
  }

}

ArbBackend::ArbBackend()
{
  // TODO
}

ArbBackend::~ArbBackend()
{
}

std::string ArbBackend::name() const
{
  return "arb";
}

ShBackendCodePtr ArbBackend::generateCode(const ShShader& shader)
{
  return new ArbCode(this, shader);
}

}
