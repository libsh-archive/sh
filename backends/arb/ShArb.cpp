#include "ShArb.hpp"
#include <iostream>
#include "ShVariable.hpp"
#include "ShDebug.hpp"

namespace ShArb {

using namespace SH;

enum ArbOp {

  // VERTEX AND FRAGMENT
  
  // Vector
  ARB_ABS,
  ARB_FLR,
  ARB_FRC,
  ARB_LIT,
  ARB_MOV,

  // Scalar
  ARB_EX2,
  ARB_LG2,
  ARB_RCP,
  ARB_RSQ,

  // Binary scalar
  ARB_POW,

  // Binary vector
  ARB_ADD,
  ARB_DP3,
  ARB_DP4,
  ARB_DPH,
  ARB_DST,
  ARB_MAX,
  ARB_MIN,
  ARB_MUL,
  ARB_SGE,
  ARB_SLT,
  ARB_SUB,
  ARB_XPD,

  // Trinary
  ARB_MAD,

  // Swizzling
  ARB_SWZ,

  // VERTEX ONLY
  // Scalar
  ARB_EXP,
  ARB_LOG,
  
  // FRAGMENT ONLY
  // Scalar
  ARB_COS,
  ARB_SIN,
  ARB_SCS,

  // Trinary
  ARB_CMP,
  ARB_LRP,

  // Sampling
  ARB_TEX,
  ARB_TXP,
  ARB_TXB,

  // KIL
  ARB_KIL
};

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

using namespace SH;

static SH::ShRefCount<ArbBackend> instance = new ArbBackend();

ArbCode::ArbCode(ArbBackendPtr backend, const ShShader& shader)
  : m_backend(backend), m_shader(shader)
{
  shader->ctrlGraph->entry()->clearMarked();
  genNode(shader->ctrlGraph->entry());
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
  // TODO
}

void ArbCode::bind()
{
  // TODO
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
      m_instructions.push_back(ArbInst(ARB_MOV, stmt.dest, stmt.src1));
      break;
    case SH_OP_NEG:
      m_instructions.push_back(ArbInst(ARB_MOV, stmt.dest, -stmt.src1));
      break;
    case SH_OP_ADD:
      m_instructions.push_back(ArbInst(ARB_ADD, stmt.dest, stmt.src1, stmt.src2));
      break;
    case SH_OP_MUL:
      m_instructions.push_back(ArbInst(ARB_MUL, stmt.dest, stmt.src1, stmt.src2));
      break;
    case SH_OP_DIV:
      {
      ShVariable rcp(new ShVariableNode(SH_VAR_TEMP, stmt.src2.size()));
      m_instructions.push_back(ArbInst(ARB_RCP, rcp, stmt.src2));
      m_instructions.push_back(ArbInst(ARB_MUL, stmt.dest, stmt.src1, rcp));
      }
      break;
    default:
      SH_DEBUG_WARN(opInfo[stmt.op].name << " not handled by ARB backend");
    }
  }

  genNode(node->follower);
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
