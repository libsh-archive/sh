#include "Backend.hpp"
#include <iostream>

namespace ShSm {

using namespace SH;

static SH::ShRefCount<Backend> instance = new Backend();
static std::string* opNames;

void initOps();

BackendCode::BackendCode()
  : m_maxLabel(0)
{
}

BackendCode::~BackendCode()
{
}

void BackendCode::upload()
{
}

void BackendCode::bind()
{
}

std::string printVar(const ShVariable& var)
{
  // TODO: Use registers
  const char* swizChars = "xyzw";

  std::string out;
  
  out += var.name();
  if (var.swizzle().size()) {  
    out += "[\"";
    for (int i = 0; i < std::min(var.swizzle().size(), 4); i++) {
      out += swizChars[i];
    }
    out += "\"]";
  }
  return out;
}

std::ostream& BackendCode::print(std::ostream& out)
{
  // TODO: smBegin etc.
  for (SmInstList::const_iterator I = m_instructions.begin(); I != m_instructions.end();
       ++I) {
    out << "sm" << opNames[I->op] << "(" << printVar(I->dest);
    if (!I->src1.null()) {
      out << ", " << printVar(I->src1);
    }
    if (!I->src2.null()) {
      out << ", " << printVar(I->src2);
    }
    if (!I->src3.null()) {
      out << ", " << printVar(I->src3);
    }
    out << ");" << std::endl;
  }
  return out;
}

void BackendCode::addBasicBlock(const ShBasicBlockPtr& block)
{
  for (ShBasicBlock::ShStmtList::const_iterator I = block->begin();
       I != block->end(); ++I) {
    const ShStatement& stmt = *I;
    switch (stmt.op) {
      // TODO: Check number and dimensions or args! 
    case SH_OP_ASN:
      m_instructions.push_back(SmInstruction(OP_MOV, stmt.dest, stmt.src1));
      break;
    case SH_OP_MUL:
      m_instructions.push_back(SmInstruction(OP_MUL, stmt.dest, stmt.src1, stmt.src2));
      break;
    default:
      // TODO: other ops
      break;
    }
  }
}

Backend::Backend()
{
  std::cerr << "SM Backend loaded" << std::endl;
  initOps();
}

Backend::~Backend()
{
  delete opNames;
  std::cerr << "SM Backend unloaded" << std::endl;
}

void Backend::generateNode(BackendCodePtr& code, const ShCtrlGraphNodePtr& node)
{
  if (node->marked()) return;
  node->mark();
  if (node->block) code->addBasicBlock(node->block);
  for (std::vector<ShCtrlGraphBranch>::const_iterator I = node->successors.begin();
       I != node->successors.end(); ++I) {
    // TODO: generate branch
  }
  if (node->follower) {
    // TODO: generate jump
    generateNode(code, node->follower);
  }
  for (std::vector<ShCtrlGraphBranch>::const_iterator I = node->successors.begin();
       I != node->successors.end(); ++I) {
    generateNode(code, I->node);
  }
}

ShBackendCodePtr Backend::generateCode(const ShShader& shader)
{
  BackendCodePtr code = new BackendCode();

  ShCtrlGraphPtr graph = shader->ctrlGraph;

  ShCtrlGraphNodePtr entry = graph->entry();

  entry->clearMarked();

  generateNode(code, entry);
  
  return code;
}

std::string Backend::name() const
{
  return "sm";
}

void initOps()
{
  opNames = new std::string[256];
  opNames[OP_ADD] = "ADD";
  opNames[OP_RCP] = "RCP";
  opNames[OP_MUL] = "MUL";
  opNames[OP_LIT] = "LIT";
  opNames[OP_DST] = "DST";
  opNames[OP_DP3] = "DP3";
  opNames[OP_DP4] = "DP4";
  opNames[OP_MOV] = "MOV";
  opNames[OP_RSQ] = "RSQ";
  opNames[OP_MIN] = "MIN";
  opNames[OP_MAX] = "MAX";
  opNames[OP_SLT] = "SLT";
  opNames[OP_SGE] = "SGE";
  opNames[OP_EXP] = "EXP";
  opNames[OP_LOG] = "LOG";
  opNames[OP_JR] = "JR";
  opNames[OP_BNE] = "BNE";
  opNames[OP_BE] = "BE";
  opNames[OP_TEX] = "TEX";
  opNames[OP_NSE1] = "NSE1";
  opNames[OP_NSE2] = "NSE2";
  opNames[OP_NSE3] = "NSE3";
  opNames[OP_DNS1] = "DNS1";
  opNames[OP_DNS2] = "DNS2";
  opNames[OP_DNS3] = "DNS3";
  opNames[OP_GNS1] = "GNS1";
  opNames[OP_GNS2] = "GNS2";
  opNames[OP_GNS3] = "GNS3";
  opNames[OP_TNS1] = "TNS1";
  opNames[OP_TNS2] = "TNS2";
  opNames[OP_TNS3] = "TNS3";
  opNames[OP_TEXJ] = "TEXJ";
  opNames[OP_SIN] = "SIN";
  opNames[OP_COS] = "COS";
  opNames[OP_FLR] = "FLR";
  opNames[OP_FRC] = "FRC";
  opNames[OP_SSG] = "SSG";
  opNames[OP_MAD] = "MAD";
  opNames[OP_NORM] = "NORM";
  opNames[OP_OUT] = "OUT";
  opNames[OP_TEXM] = "TEXM";
  opNames[OP_SHL] = "SHL";
  opNames[OP_SHR] = "SHR";
  opNames[OP_HSH] = "HSH";
  opNames[OP_MOVD] = "MOVD";
  opNames[OP_DBG] = "DBG";
  opNames[OP_DDX] = "DDX";
  opNames[OP_DDY] = "DDY";
  opNames[OP_TXD] = "TXD";
  opNames[OP_KLL] = "KLL";
  opNames[OP_HLT] = "HLT";
}

}
