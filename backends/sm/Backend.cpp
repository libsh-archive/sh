#include "Backend.hpp"
#include <iostream>
#include <sstream>
#include "ShDebug.hpp"
#include "sm.hpp"

namespace ShSm {

using namespace SH;

static SH::ShRefCount<Backend> instance = new Backend();
static std::string* opNames;

void initOps();

std::string SmRegister::print() const
{
  std::ostringstream stream;
  switch(type) {
  case SHSM_REG_INPUT:
    stream << "iR";
    break;
  case SHSM_REG_OUTPUT:
    stream << "oR";
    break;
  case SHSM_REG_TEMP:
    stream << "tR";
    break;
  case SHSM_REG_CONST:
    stream << "cR";
    break;
  }
  stream << "[" << index << "]" << std::ends;
  
  return stream.str();
}

BackendCode::BackendCode(const ShShader& shader)
  : m_shader(shader), m_maxLabel(0),
    m_maxCR(0), m_maxTR(0), m_maxIR(0), m_maxOR(0),
    m_cR(0), m_tR(0), m_iR(0), m_oR(0)
{
}

BackendCode::~BackendCode()
{
  delete [] m_cR;
  delete [] m_tR;
  delete [] m_iR;
  delete [] m_oR;
}

void BackendCode::upload()
{
  m_smShader = smDeclareShader(m_shader->kind());
  smShaderBegin(m_smShader);

  for (int i = 0; i < m_maxTR; i++) m_tR[i] = smReg();
  for (int i = 0; i < m_maxIR; i++) m_iR[i] = smInputReg(i);
  for (int i = 0; i < m_maxOR; i++) m_oR[i] = smOutputReg(i);
  // TODO: Initialize constants
  for (int i = 0; i < m_maxCR; i++) m_cR[i] = smConstantReg(i);

  for (SmInstList::const_iterator I = m_instructions.begin(); I != m_instructions.end();
       ++I) {
    // TODO: Negation!
    if (I->src1.null()) {
      smINSTR(I->op, getSmReg(I->dest.node()));
    } else if (I->src2.null()) {
      smINSTR(I->op, getSmReg(I->dest.node()), getSmReg(I->src1.node()));
    } else if (I->src3.null()) {
      smINSTR(I->op, getSmReg(I->dest.node()), getSmReg(I->src1.node()),
              getSmReg(I->src2.node()));
    } else {
      smINSTR(I->op, getSmReg(I->dest.node()), getSmReg(I->src1.node()),
              getSmReg(I->src2.node()), getSmReg(I->src3.node()));
    }
  }
  
  smShaderEnd();
}

void BackendCode::bind()
{
  // TODO: Set constants
  smBindShader(m_smShader);
}

std::string BackendCode::printVar(const ShVariable& var)
{
  const char* swizChars = "xyzw";

  std::string out;

  if (var.neg()) out += "-";
  
  out += getReg(var.node()).print();
  if (var.swizzle().size()) {  
    out += "[\"";
    for (int i = 0; i < std::min(var.swizzle().size(), 4); i++) {
      out += swizChars[var.swizzle()[i]];
    }
    out += "\"]";
  } else {
    if (var.size() < 4) {
      out += "[\"";
      for (int i = 0; i < var.size(); i++) out += swizChars[i];
      out += "\"]";
    }
  }
  return out;
}

std::ostream& BackendCode::print(std::ostream& out)
{
  out << "SMshader shader = smDeclareShader(" << m_shader->kind() << ");" << std::endl;
  out << "{" << std::endl;
  out << "smShaderBegin(shader);" << std::endl;
  out << std::endl;
  out << "// Register declarations" << std::endl;
  out << "SMreg tR[" << m_maxTR << "];" << std::endl;
  out << "for (int i = 0; i < " << m_maxTR << "; i++) tR[i] = smReg();" << std::endl;
  out << "SMreg iR[" << m_maxIR << "];" << std::endl;
  for (int i = 0; i < m_maxIR; i++) out << "iR[" << i << "] = smInputReg(" << i << ");" << std::endl;
  out << "SMreg oR[" << m_maxOR << "];" << std::endl;
  for (int i = 0; i < m_maxOR; i++) out << "oR[" << i << "] = smOutputReg(" << i << ");" << std::endl;
  out << "SMreg cR[" << m_maxCR << "];" << std::endl;
  for (int i = 0; i < m_maxCR; i++) out << "cR[" << i << "] = smConstantReg(" << i << ");" << std::endl;

  out << std::endl;
  out << "// Shader body" << std::endl;
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
  out << std::endl;
  out << "smShaderEnd();" << std::endl;
  out << "}" << std::endl;
  return out;
}

void BackendCode::addBasicBlock(const ShBasicBlockPtr& block)
{
  for (ShBasicBlock::ShStmtList::const_iterator I = block->begin();
       I != block->end(); ++I) {
    const ShStatement& stmt = *I;
    switch (stmt.op) {
      // TODO: Check number and dimensions of args! 
    case SH_OP_ASN:
      m_instructions.push_back(SmInstruction(OP_MOV, stmt.dest, stmt.src1));
      break;
    case SH_OP_ADD:
      m_instructions.push_back(SmInstruction(OP_ADD, stmt.dest, stmt.src1, stmt.src2));
      break;
    case SH_OP_MUL:
      // TODO: Scalar mult
      m_instructions.push_back(SmInstruction(OP_MUL, stmt.dest, stmt.src1, stmt.src2));
      break;
    case SH_OP_DIV:
      {
      int rsize = stmt.src2.swizzle().size();
      if (!rsize) rsize = stmt.src2.size();
      ShVariable rcp(new ShVariableNode(SH_VAR_TEMP, rsize));
      m_instructions.push_back(SmInstruction(OP_RCP, rcp, stmt.src2));
      // TODO: scalar
      m_instructions.push_back(SmInstruction(OP_MUL, stmt.dest, stmt.src1, rcp));
      break;
      }
    default:
      // TODO: other ops
      break;
    }
  }
}

void BackendCode::allocRegs()
{
  for (SmInstList::const_iterator I = m_instructions.begin();
       I != m_instructions.end(); ++I) {
    getReg(I->dest.node());
    getReg(I->src1.node());
    getReg(I->src2.node());
    getReg(I->src3.node());
  }
  m_cR = new SMreg[m_maxCR];
  m_tR = new SMreg[m_maxTR];
  m_iR = new SMreg[m_maxIR];
  m_oR = new SMreg[m_maxOR];
}

SmRegister BackendCode::getReg(const SH::ShVariableNodePtr& var)
{
  if (!var) return SmRegister(SHSM_REG_TEMP, -1);
  RegMap::const_iterator I = m_registers.find(var);
  if (I != m_registers.end()) return I->second;
  
  if (var->uniform()) {
    m_registers[var] = SmRegister(SHSM_REG_CONST, m_maxCR++);
    return m_registers[var];
  }

  // TODO: Mark uniforms here, perhaps, so they can be rebound.
  // OTOH we could just use the shader's uniform list.
  
  switch (var->kind()) {
  case SH_VAR_INPUT:
    m_registers[var] = SmRegister(SHSM_REG_INPUT, m_maxIR++);
    break;
  case SH_VAR_OUTPUT:
    m_registers[var] = SmRegister(SHSM_REG_OUTPUT, m_maxOR++);
    break;
  case SH_VAR_TEMP:
    m_registers[var] = SmRegister(SHSM_REG_TEMP, m_maxTR++);
    break;
  case SH_VAR_CONST:
    m_registers[var] = SmRegister(SHSM_REG_CONST, m_maxCR++);
    break;
  }
  return m_registers[var];
}

SMreg BackendCode::getSmReg(const SH::ShVariableNodePtr& var)
{
  SmRegister reg = getReg(var);
  if (reg.index < 0) return SMreg(); // TODO: Something better?
  switch (reg.type) {
  case SHSM_REG_INPUT:
    return m_iR[reg.index];
  case SHSM_REG_OUTPUT:
    return m_oR[reg.index];
  case SHSM_REG_TEMP:
    return m_tR[reg.index];
  case SHSM_REG_CONST:
    return m_cR[reg.index];
  }
  return SMreg(); // Control should never even get here.
}

Backend::Backend()
{
  SH_DEBUG_PRINT("SM Backend loaded.");
  initOps();
}

Backend::~Backend()
{
  delete opNames;
  SH_DEBUG_PRINT("SM Backend unloaded.");
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
  BackendCodePtr code = new BackendCode(shader);

  ShCtrlGraphPtr graph = shader->ctrlGraph;

  ShCtrlGraphNodePtr entry = graph->entry();

  entry->clearMarked();

  generateNode(code, entry);

  code->allocRegs();
  
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
