#include "ShArb.hpp"
#include <iostream>
#include <sstream>
#include <GL/gl.h>
#include "ShVariable.hpp"
#include "ShDebug.hpp"
#include "ShLinearAllocator.hpp"

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
  ArbReg()
    : type(SH_ARB_REG_TEMP), index(-1), binding(SH_ARB_REG_NONE)
  {
  }
  
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
  std::ostream& printDecl(std::ostream& out) const
  {
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
  : m_backend(backend), m_shader(shader),
    m_numTemps(0), m_numInputs(0), m_numOutputs(0), m_numParams(0)
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
  unsigned int target = (m_shader->kind() == 0 ? GL_VERTEX_PROGRAM_ARB : GL_FRAGMENT_PROGRAM_ARB);
  glProgramStringARB(target, GL_PROGRAM_FORMAT_ASCII_ARB, text.size(), text.c_str());
  // TODO: Check for errors
}

void ArbCode::updateUniform(const ShVariableNodePtr& uniform)
{
  // TODO
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

std::ostream& ArbCode::print(std::ostream& out)
{
  SH_DEBUG_PRINT("printing the code");
  using std::endl;

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
  
  for (ShShaderNode::VarList::const_iterator I = m_shader->inputs.begin();
       I != m_shader->inputs.end(); ++I) {
    out << "  ";
    m_registers[*I].printDecl(out);
    out << endl;
  }

  for (ShShaderNode::VarList::const_iterator I = m_shader->outputs.begin();
       I != m_shader->outputs.end(); ++I) {
    out << "  ";
    m_registers[*I].printDecl(out);
    out << endl;
  }

  for (ShShaderNode::VarList::const_iterator I = m_shader->uniforms.begin();
       I != m_shader->uniforms.end(); ++I) {
    out << "  ";
    m_registers[*I].printDecl(out);
    out << endl;
  }

  for (int i = 0; i < m_numTemps; i++) {
    out << "  ";
    ArbReg(SH_ARB_REG_TEMP, i).printDecl(out);
    out << endl;
  }

  out << endl;
  
  // Print instructions
  for (ArbInstList::const_iterator I = m_instructions.begin();
       I != m_instructions.end(); ++I) {
    out << "  ";
    out << arbOpInfo[I->op].name << " ";
    printVar(out, true, I->dest);
    for (int i = 0; i < arbOpInfo[I->op].arity; i++) {
      out << ", ";
      printVar(out, false, I->src[i]);
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
    allocInput(*I);
  }
  for (ShShaderNode::VarList::const_iterator I = m_shader->outputs.begin();
       I != m_shader->outputs.end(); ++I) {
    allocOutput(*I);
  }
  for (ShShaderNode::VarList::const_iterator I = m_shader->uniforms.begin();
       I != m_shader->uniforms.end(); ++I) {
    allocParam(*I);
  }
  
  allocTemps();
}

void ArbCode::allocInput(ShVariableNodePtr node)
{
  if (m_registers.find(node) != m_registers.end()) return;
  m_registers[node] = ArbReg(SH_ARB_REG_ATTRIB, m_numInputs++);
}

void ArbCode::allocOutput(ShVariableNodePtr node)
{
  if (m_registers.find(node) != m_registers.end()) return;
  m_registers[node] = ArbReg(SH_ARB_REG_OUTPUT, m_numOutputs++);
}

void ArbCode::allocParam(ShVariableNodePtr node)
{
  if (m_registers.find(node) != m_registers.end()) return;
  m_registers[node] = ArbReg(SH_ARB_REG_PARAM, m_numParams++);
}

void mark(ShLinearAllocator& allocator, ShVariableNodePtr node, int i)
{
  if (!node) return;
  if (node->kind() == SH_VAR_TEMP) allocator.mark(node, i);
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
  
  allocator.debugDump();

  allocator.allocate();
  
  m_tempRegs.clear();
}

ArbBackend::ArbBackend()
{
  for (int i = 0; i < 2; i++) {
    unsigned long target = (i ? GL_FRAGMENT_PROGRAM_ARB : GL_VERTEX_PROGRAM_ARB);
    glGetProgramivARB(target, GL_MAX_PROGRAM_INSTRUCTIONS_ARB, &m_instrs[i]);
    m_instrs[i] = 128; // FIXME
    SH_DEBUG_PRINT("instrs[" << i << "] = " << m_instrs[i]);
    glGetProgramivARB(target, GL_MAX_PROGRAM_TEMPORARIES_ARB, &m_temps[i]);
    m_temps[i] = 12; // FIXME
    SH_DEBUG_PRINT("temps[" << i << "] = " << m_temps[i]);
    glGetProgramivARB(target, GL_MAX_PROGRAM_ATTRIBS_ARB, &m_attribs[i]);
    m_attribs[i] = 16; // FIXME
    SH_DEBUG_PRINT("attribs[" << i << "] = " << m_attribs[i]);
    glGetProgramivARB(target, GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB, &m_params[i]);
    m_params[i] = 96; // FIXME
    SH_DEBUG_PRINT("params[" << i << "] = " << m_params[i]);
  }
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
  ArbCodePtr code = new ArbCode(this, shader);
  code->generate();
  return code;
}

}
