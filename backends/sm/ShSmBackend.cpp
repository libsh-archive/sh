#include "ShSmBackend.hpp"
#include <iostream>
#include <sstream>
#include "ShEnvironment.hpp"
#include "ShDebug.hpp"
#include "ShLinearAllocator.hpp"
#include "sm.hpp"

namespace ShSm {

const int SmMaxTR = 256;

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
  case SHSM_REG_TEXTURE:
    stream << index;
    return stream.str();
  }
  stream << "[" << index << "]";
  
  return stream.str();
}

BackendCode::BackendCode(ShRefCount<Backend> backend, const ShShader& shader)
  : m_backend(backend), m_shader(shader),
    m_maxCR(0), m_maxTR(0), m_maxIR(0), m_maxOR(0), m_maxTex(0),
    m_cR(0), m_tR(0), m_iR(0), m_oR(0)
{
}

BackendCode::~BackendCode()
{
  delete [] m_cR;
  delete [] m_tR;
  delete [] m_iR;
  delete [] m_oR;

  for (TextureNodeMap::iterator I = m_textureMap.begin(); I != m_textureMap.end(); ++I) {
    smDeleteTexture(I->second);
  }
}

bool BackendCode::allocateRegister(const SH::ShVariableNodePtr& var)
{
  if (!var) return true;
  if (var->kind() != SH_VAR_TEMP) return true; // ignore anything but temporaries
  if (var->uniform()) return true;
  
  if (m_tempRegs.empty()) {
    SH_DEBUG_WARN("Oh no, out of registers!");
    return false;
  }

  int idx = m_tempRegs.front();
  m_tempRegs.pop_front();

  if (idx + 1 > m_maxTR) m_maxTR = idx + 1;
  m_registers[var] = SmRegister(SHSM_REG_TEMP, idx);
  return true;
}

void BackendCode::freeRegister(const SH::ShVariableNodePtr& var)
{
  if (!var) return;
  if (var->kind() != SH_VAR_TEMP) return; // ignore anything but temporaries
  if (var->uniform()) return;

  SH_DEBUG_ASSERT(m_registers.find(var) != m_registers.end());
  m_tempRegs.push_front(m_registers[var].index);
}

void BackendCode::upload()
{
  SH_DEBUG_PRINT("Uploading shader");
  m_smShader = smDeclareShader(m_shader->kind());
  smShaderBegin(m_smShader);

  for (int i = 0; i < m_maxTR; i++) m_tR[i] = smReg();
  for (int i = 0; i < m_maxIR; i++) m_iR[i] = smInputReg(i);
  for (int i = 0; i < m_maxOR; i++) m_oR[i] = smOutputReg(i);
  for (int i = 0; i < m_maxCR; i++) m_cR[i] = smConstantReg(i);

  int i = 0;
  for (SmInstList::const_iterator I = m_instructions.begin(); I != m_instructions.end();
       ++I) {
    if (I->op == OP_TEX) {
      SH_DEBUG_PRINT("Adding an OP_TEX");
      SH_DEBUG_ASSERT(getReg(I->src2.node()).type == SHSM_REG_TEXTURE);
      smInstr(OP_TEX, getSmReg(I->dest), getSmReg(I->src1), getReg(I->src2.node()).index);
    } else if (I->src1.null()) {
      smInstr(I->op, getSmReg(I->dest));
    } else if (I->src2.null()) {
      smInstr(I->op, getSmReg(I->dest), getSmReg(I->src1));
    } else if (I->src3.null()) {
      smInstr(I->op, getSmReg(I->dest), getSmReg(I->src1), getSmReg(I->src2));
    } else {
      smInstr(I->op, getSmReg(I->dest), getSmReg(I->src1), getSmReg(I->src2), getSmReg(I->src3));
    }
    i++;
  }
  SH_DEBUG_PRINT(i << " instructions uploaded.");
  smShaderEnd();
}

void BackendCode::bind()
{
  SH_DEBUG_PRINT("Binding shader");
  
  smBindShader(m_smShader);
  SH::ShEnvironment::boundShader[m_shader->kind()] = m_shader;

  // Initialize constants
  for (RegMap::const_iterator I = m_registers.begin(); I != m_registers.end(); ++I) {
    ShVariableNodePtr node = I->first;
    SmRegister reg = I->second;
    if (node->hasValues() && reg.type == SHSM_REG_CONST) {
      float values[4];
      int i;
      for (i = 0; i < node->size(); i++) {
        values[i] = node->getValue(i);
      }
      for (; i < 4; i++) {
        values[i] = 0.0;
      }
      smModLocalConstant(m_shader->kind(), reg.index, smTuple(values[0], values[1], values[2], values[3]));
    }
  }

  SH_DEBUG_PRINT("Uploading textures...");
  for (ShShaderNode::VarList::const_iterator I = m_shader->textures.begin(); I != m_shader->textures.end();
       ++I) {
    ShTextureNodePtr texture = *I;
    if (!texture) {
      SH_DEBUG_WARN((*I)->name() << " is not a valid texture!");
      continue;
    }
    if (m_textureMap.find(texture) == m_textureMap.end()) {
      // TODO: Other types of textures.
      
      m_textureMap[texture] = smNewTexture2DRect(texture->width(), texture->height(),
                                                 texture->elements(), SM_FLOAT);
      SMtexture smTex = m_textureMap[texture];
      smTexImage2D(smTex, 0, const_cast<float*>(texture->data()));

      // TODO: Capabilities
      smTexWrapS(smTex, SM_REPEAT);
      smTexWrapT(smTex, SM_REPEAT);
    } else {
      SH_DEBUG_PRINT("Texture already allocated");
    }
    SH_DEBUG_PRINT("Binding texture " << m_textureMap[texture] << " to texture unit " << getReg(texture).index);
    smBindTexture(m_shader->kind(), getReg(texture).index, m_textureMap[texture]);
  }
}

std::string BackendCode::printVar(const ShVariable& var)
{
  const char* swizChars = "xyzw";

  std::string out;

  if (var.neg()) out += "-";
  
  out += getReg(var.node()).print();

  if (var.node()->kind() == SH_VAR_TEXTURE) return out;
  
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

void BackendCode::updateUniform(const ShVariableNodePtr& uniform)
{
  if (!haveReg(uniform)) return;

  SmRegister reg = getReg(uniform);
  
  float values[4];
  int i;
  for (i = 0; i < uniform->size(); i++) {
    values[i] = uniform->getValue(i);
  }
  for (; i < 4; i++) {
    values[i] = 0.0;
  }
  smModLocalConstant(m_shader->kind(), reg.index, smTuple(values[0], values[1], values[2], values[3]));
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
#ifdef SH_DEBUG
  int i = 0;
#endif
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
    out << ");";
#ifdef SH_DEBUG
    out << " // PC = " << i++;
#endif
    out << std::endl;
  }
  out << std::endl;
  out << "smShaderEnd();" << std::endl;
  out << "}" << std::endl;

  out << std::endl;
  out << "// Initialize constant registers" << std::endl;
  
  // Set constants.
  // This should really only happen at bind time.
  // The question is: how useful is printing the code out in the
  // first place if the constant values aren't really accessible.
  // I guess printing out the code is more of a debugging tool than anything.
  for (RegMap::const_iterator I = m_registers.begin(); I != m_registers.end(); ++I) {
    ShVariableNodePtr node = I->first;
    SmRegister reg = I->second;
    if (node->hasValues() && reg.type == SHSM_REG_CONST) {
      out << "smModLocalConstant(" << m_shader->kind() << ", " << reg.index << ", smTuple(";
      for (int i = 0; i < node->size(); i++) {
        if (i) out << ", ";
        out << node->getValue(i);
      }
      out << "));" << std::endl;
    }
  }
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
      {
      if (stmt.src1.size() != 1 || stmt.src2.size() != 1) {
        if (stmt.src1.size() == 1) {
          int* swizzle = new int[stmt.src2.size()];
          for (int i = 0; i < stmt.src2.size(); i++) swizzle[i] = 0;
          m_instructions.push_back(SmInstruction(OP_MUL, stmt.dest,
                                                 stmt.src1(stmt.src2.size(), swizzle), stmt.src2));
          delete [] swizzle;
          break;
        } else if (stmt.src2.size() == 1) {
          int* swizzle = new int[stmt.src1.size()];
          for (int i = 0; i < stmt.src1.size(); i++) swizzle[i] = 0;
          m_instructions.push_back(SmInstruction(OP_MUL, stmt.dest, stmt.src1,
                                                 stmt.src2(stmt.src1.size(), swizzle)));
          delete [] swizzle;
          break;
        }
      }
      
      m_instructions.push_back(SmInstruction(OP_MUL, stmt.dest, stmt.src1, stmt.src2));
      break;
      }
    case SH_OP_DIV:
      {
        ShVariable rcp(new ShVariableNode(SH_VAR_TEMP, stmt.src2.size()));
        m_instructions.push_back(SmInstruction(OP_RCP, rcp, stmt.src2));

        if (rcp.size() == 1 && stmt.src1.size() != 1) {
          int* swizzle = new int[stmt.src1.size()];
          for (int i = 0; i < stmt.src1.size(); i++) swizzle[i] = 0;
          m_instructions.push_back(SmInstruction(OP_MUL, stmt.dest, stmt.src1,
                                                 rcp(stmt.src1.size(), swizzle)));
          delete [] swizzle;
        } else {
          m_instructions.push_back(SmInstruction(OP_MUL, stmt.dest, stmt.src1, rcp));
        }
        break;
      }
    case SH_OP_SLT:
      m_instructions.push_back(SmInstruction(OP_SLT, stmt.dest, stmt.src1, stmt.src2));
      break;
    case SH_OP_SLE:
      m_instructions.push_back(SmInstruction(OP_SLE, stmt.dest, stmt.src1, stmt.src2));
      break;
    case SH_OP_SGT:
      m_instructions.push_back(SmInstruction(OP_SGT, stmt.dest, stmt.src1, stmt.src2));
      break;
    case SH_OP_SGE:
      m_instructions.push_back(SmInstruction(OP_SGE, stmt.dest, stmt.src1, stmt.src2));
      break;
    case SH_OP_SEQ:
      m_instructions.push_back(SmInstruction(OP_SEQ, stmt.dest, stmt.src1, stmt.src2));
      break;
    case SH_OP_SNE:
      m_instructions.push_back(SmInstruction(OP_SNE, stmt.dest, stmt.src1, stmt.src2));
      break;
    case SH_OP_ABS:
      m_instructions.push_back(SmInstruction(OP_ABS, stmt.dest, stmt.src1));
      break;
    case SH_OP_COS:
      m_instructions.push_back(SmInstruction(OP_COS, stmt.dest, stmt.src1));
      break;
    case SH_OP_DOT:
      {
        if (stmt.src1.size() == 3) {
          m_instructions.push_back(SmInstruction(OP_DP3, stmt.dest, stmt.src1, stmt.src2));
        } else if (stmt.src1.size() == 4) {
          m_instructions.push_back(SmInstruction(OP_DP4, stmt.dest, stmt.src1, stmt.src2));
        } else if (stmt.src1.size() == 1) {
          m_instructions.push_back(SmInstruction(OP_MUL, stmt.dest, stmt.src1, stmt.src2));
        } else {
          ShVariable mul(new ShVariableNode(SH_VAR_TEMP, stmt.src1.size()));
          m_instructions.push_back(SmInstruction(OP_MUL, mul, stmt.src1, stmt.src2));
          m_instructions.push_back(SmInstruction(OP_ADD, stmt.dest, mul(0), mul(1)));
          for (int i = 2; i < stmt.src1.size(); i++) {
            m_instructions.push_back(SmInstruction(OP_ADD, stmt.dest, stmt.dest, mul(i)));
          }
        }
      break;
      }
    case SH_OP_FRAC:
      m_instructions.push_back(SmInstruction(OP_FRC, stmt.dest, stmt.src1));
      break;
    case SH_OP_POW:
      for (int i = 0; i < stmt.src1.size(); i++) {
        m_instructions.push_back(SmInstruction(OP_POW, stmt.dest(i), stmt.src1(i), stmt.src2(i)));
      }
      break;
    case SH_OP_SIN:
      m_instructions.push_back(SmInstruction(OP_SIN, stmt.dest, stmt.src1));
      break;
    case SH_OP_SQRT:
      {
        int rsize = stmt.src1.size();
        ShVariable rsq(new ShVariableNode(SH_VAR_TEMP, rsize));
        m_instructions.push_back(SmInstruction(OP_RSQ, rsq, stmt.src1));
        m_instructions.push_back(SmInstruction(OP_RCP, stmt.dest, rsq));
        break;
      }
    case SH_OP_NORM:
      m_instructions.push_back(SmInstruction(OP_NORM, stmt.dest, stmt.src1));
      break;
    case SH_OP_TEX:
      {
        ShTextureNodePtr texture = stmt.src1.node();
        if (!texture) break;
        // TODO: Check texture for not mipmapped

        /*
        // Scale the lookup as necessary
        ShVariableNodePtr scale = new ShVariableNode(SH_VAR_CONST, stmt.src2.size());
        
        scale->setValue(0, texture->width());
        if (stmt.src2.size() >= 2) scale->setValue(1, texture->height());

        ShVariable scaled(new ShVariableNode(SH_VAR_TEMP, stmt.src2.size()));
        m_instructions.push_back(SmInstruction(OP_MUL, scaled, stmt.src2, ShVariable(scale)));
        */
        m_instructions.push_back(SmInstruction(OP_TEX, stmt.dest, stmt.src2, stmt.src1));
        break;
      }
    case SH_OP_COND:
      m_instructions.push_back(SmInstruction(OP_CMP, stmt.dest, -stmt.src1, stmt.src2, stmt.src3));
      break;
    default:
      // TODO: other ops
      SH_DEBUG_WARN(opInfo[stmt.op].name << " not implement in SM backend");
      break;
    }
  }
}

void BackendCode::allocRegs()
{
  for (ShShaderNode::VarList::const_iterator I = m_shader->inputs.begin();
       I != m_shader->inputs.end(); ++I) {
    getReg(*I);
  }
  for (ShShaderNode::VarList::const_iterator I = m_shader->outputs.begin();
       I != m_shader->outputs.end(); ++I) {
    getReg(*I);
  }

  m_tempRegs.clear();
  m_maxTR = 0;
  for (int i = 0; i < SmMaxTR; i++) {
    m_tempRegs.push_back(i);
  }
  
  ShLinearAllocator allocator(ShBackendCodePtr(this));
  
  for (std::size_t i = 0; i < m_instructions.size(); i++) {
    SmInstruction instr = m_instructions[i];
    getReg(instr.dest.node());
    allocator.mark(instr.dest.node(), i);
    getReg(instr.src1.node());
    allocator.mark(instr.src1.node(), i);
    getReg(instr.src2.node());
    allocator.mark(instr.src2.node(), i);
    getReg(instr.src3.node());
    allocator.mark(instr.src3.node(), i);
  }
  allocator.allocate();

  m_tempRegs.clear();
  
  m_tR = new SMreg[m_maxTR];
  m_cR = new SMreg[m_maxCR];
  m_iR = new SMreg[m_maxIR];
  m_oR = new SMreg[m_maxOR];
}

bool BackendCode::haveReg(const SH::ShVariableNodePtr& var)
{
  return m_registers.find(var) != m_registers.end();
}

SmRegister BackendCode::getReg(const SH::ShVariableNodePtr& var)
{
  if (!var) return SmRegister(SHSM_REG_TEMP, -1);
  RegMap::const_iterator I = m_registers.find(var);
  if (I != m_registers.end()) return I->second;
  
  if (var->uniform() && var->kind() != SH_VAR_TEXTURE) {
    m_registers[var] = SmRegister(SHSM_REG_CONST, m_maxCR++);
    return m_registers[var];
  }

  if (var->kind() == SH_VAR_TEMP) return SmRegister(SHSM_REG_TEMP, -1);
  
  switch (var->kind()) {
  case SH_VAR_INPUT:
    m_registers[var] = SmRegister(SHSM_REG_INPUT, m_maxIR++);
    break;
  case SH_VAR_OUTPUT:
    m_registers[var] = SmRegister(SHSM_REG_OUTPUT, m_maxOR++);
    break;
  case SH_VAR_TEMP:
    break;
  case SH_VAR_CONST:
    m_registers[var] = SmRegister(SHSM_REG_CONST, m_maxCR++);
    break;
  case SH_VAR_TEXTURE:
    SH_DEBUG_PRINT("Allocating texture unit " << m_maxTex);
    m_registers[var] = SmRegister(SHSM_REG_TEXTURE, m_maxTex++);
    break;
  }
  return m_registers[var];
}

SMreg BackendCode::getSmReg(const SH::ShVariable& var)
{
  SmRegister reg = getReg(var.node());
  if (reg.index < 0) {
    SH_DEBUG_WARN("Could not obtain register!");
    return SMreg(); // TODO: Something better?
  }
  
  SMreg smReg;
  // Get the storage register
  switch (reg.type) {
  case SHSM_REG_INPUT:
    smReg = m_iR[reg.index];
    break;
  case SHSM_REG_OUTPUT:
    smReg = m_oR[reg.index];
    break;
  case SHSM_REG_TEMP:
    smReg = m_tR[reg.index];
    break;
  case SHSM_REG_CONST:
    smReg = m_cR[reg.index];
    break;
  case SHSM_REG_TEXTURE:
    return SMreg();
    break;
  default:
    SH_DEBUG_WARN("Unknown register type " << (int)reg.type);
    break;
  }
  
  // Swizzling
  const char* swizChars = "xyzw";
  std::string swizzle;
  if (var.swizzle().size()) {
    for (int i = 0; i < std::min(var.swizzle().size(), 4); i++) {
      swizzle += swizChars[var.swizzle()[i]];
    }
  } else {
    if (var.size() < 4) {
      for (int i = 0; i < var.size(); i++) swizzle += swizChars[i];
    }
  }
  if (!swizzle.empty()) smReg = smReg[swizzle.c_str()];

  // Negation
  if (var.neg()) smReg = -smReg;
  
  return smReg;
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
  BackendCodePtr code = new BackendCode(this, shader);

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
  opNames[OP_ABS] = "ABS";
  opNames[OP_ADD] = "ADD";
  opNames[OP_BRA] = "BRA";
  opNames[OP_CAL] = "CAL";
  opNames[OP_CMP] = "CMP";
  opNames[OP_COS] = "COS";
  opNames[OP_DDX] = "DDX";
  opNames[OP_DDY] = "DDY";
  opNames[OP_DP3] = "DP3";
  opNames[OP_DP4] = "DP4";
  opNames[OP_DPH] = "DPH";
  opNames[OP_DST] = "DST";
  opNames[OP_EX2] = "EX2";
  opNames[OP_EXP] = "EXP";
  opNames[OP_FLR] = "FLR";
  opNames[OP_FRC] = "FRC";
  opNames[OP_KIL] = "KIL";
  opNames[OP_LG2] = "LG2";
  opNames[OP_LIT] = "LIT";
  opNames[OP_LOG] = "LOG";
  opNames[OP_LRP] = "LRP";
  opNames[OP_MAD] = "MAD";
  opNames[OP_MAX] = "MAX";
  opNames[OP_MIN] = "MIN";
  opNames[OP_MOV] = "MOV";
  opNames[OP_MUL] = "MUL";
  opNames[OP_NORM] = "NORM"; 
  opNames[OP_OUT] = "OUT"; 
  opNames[OP_POW] = "POW";
  opNames[OP_RCC] = "RCC";
  opNames[OP_RCP] = "RCP";
  opNames[OP_RET] = "RET";
  opNames[OP_RFL] = "RFL";
  opNames[OP_RSQ] = "RSQ";
  opNames[OP_SEQ] = "SEQ";
  opNames[OP_SCS] = "SCS";
  opNames[OP_SFL] = "SFL";
  opNames[OP_SGE] = "SGE";
  opNames[OP_SGT] = "SGT";
  opNames[OP_SIN] = "SIN";
  opNames[OP_SLE] = "SLE";
  opNames[OP_SLT] = "SLT";
  opNames[OP_SNE] = "SNE";
  opNames[OP_SSG] = "SSG";
  opNames[OP_STR] = "STR";
  opNames[OP_SUB] = "SUB";
  opNames[OP_TEX] = "TEX";
  opNames[OP_TXB] = "TXB";
  opNames[OP_TXD] = "TXD";
  opNames[OP_TXP] = "TXP";
  opNames[OP_X2D] = "X2D";
  opNames[OP_XPD] = "XPD";
  opNames[OP_HLT] = "HLT";
}

}
