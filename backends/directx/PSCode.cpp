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
#include "PSCode.hpp"
#include <iostream>
#include <sstream>
#include <cmath>
#include <bitset>

#include <d3d9.h>
#include <d3dx9.h>

#include "ShVariable.hpp"
#include "ShDebug.hpp"
#include "ShLinearAllocator.hpp"
#include "ShInternals.hpp"
#include "ShOptimizations.hpp"
#include "ShEnvironment.hpp"
#include "ShContext.hpp"
#include "ShTextureNode.hpp"
#include "ShSyntax.hpp"
#include "PSReg.hpp"
#include "PS.hpp"
#include "ShAttrib.hpp"
#include "ShError.hpp"

namespace shdx {

using namespace SH;

struct PSBindingSpecs {
  PSRegBinding binding;
  int maxBindings;
  ShSemanticType semanticType;
  bool allowGeneric;
};

PSBindingSpecs psVertexAttribBindingSpecs[] = {
  {SH_PS_REG_VERTEXPOS, 1, SH_POSITION, false},
  {SH_PS_REG_VERTEXNRM, 1, SH_NORMAL, false},
  {SH_PS_REG_VERTEXDIFFUSE, 1, SH_COLOR, false},
  {SH_PS_REG_VERTEXTEX, 8, SH_TEXCOORD, true},
  {SH_PS_REG_NONE, 0, SH_ATTRIB, true}
};

PSBindingSpecs psVertexOutputBindingSpecs[] = {
  {SH_PS_REG_RESULTPOS, 1, SH_POSITION, false},
  {SH_PS_REG_RESULTDIFFUSE, 1, SH_COLOR, false},
  {SH_PS_REG_RESULTSPECULAR, 1, SH_COLOR, false},
  {SH_PS_REG_RESULTTEX, 8, SH_TEXCOORD, true},
  {SH_PS_REG_RESULTFOG, 1, SH_ATTRIB, true},
  {SH_PS_REG_RESULTPTS, 1, SH_ATTRIB, true},
  {SH_PS_REG_NONE, 0, SH_ATTRIB}
};

PSBindingSpecs psFragmentAttribBindingSpecs[] = {
  {SH_PS_REG_FRAGMENTPOS, 1, SH_POSITION, false},
  {SH_PS_REG_FRAGMENTNRM, 1, SH_NORMAL, false},
  {SH_PS_REG_FRAGMENTDIFFUSE, 1, SH_COLOR, false},
  {SH_PS_REG_FRAGMENTTEX, 8, SH_TEXCOORD, true},
  {SH_PS_REG_NONE, 0, SH_ATTRIB, true}
};

PSBindingSpecs psFragmentOutputBindingSpecs[] = {
  {SH_PS_REG_RESULTCOL, 1, SH_COLOR, true},
  {SH_PS_REG_NONE, 0, SH_ATTRIB}
};

PSBindingSpecs* psBindingSpecs(bool output, const std::string& unit)
{
  if (unit == "vertex")
    return output ? psVertexOutputBindingSpecs : psVertexAttribBindingSpecs;
  if (unit == "fragment")
    return output ? psFragmentOutputBindingSpecs : psFragmentAttribBindingSpecs;
  return 0;
}

using namespace SH;

PSCode::PSCode(const ShProgramNodeCPtr& shader, const std::string& unit,
                 TextureStrategy* textures)
  : m_textures(textures), m_shader(0), m_originalShader(shader), m_unit(unit),
    m_numTemps(0), m_numInputs(0), m_numOutputs(0), m_numParams(0),
    m_numTextures(0), m_environment(0), m_max_label(0), m_pVS(NULL), m_pPS(NULL)
{
  if (unit == "fragment") m_environment |= SH_PS_PS_3_0;
  if (unit == "vertex") m_environment |= SH_PS_VS_3_0;
}

PSCode::~PSCode()
{
}

// Set the Direct3D device
void PSCode::setD3DDevice(LPDIRECT3DDEVICE9 pD3DDevice)
{
  m_pD3DDevice = pD3DDevice;
}

void PSCode::generate()
{
  // Transform code to be VS/PS compatible
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

  /*if (m_environment & SH_ARB_NVFP2) {
    // In NV_fragment_program2, we actually generate structured code.
    ShStructural str(m_shader->ctrlGraph);

    genStructNode(str.head());
    
  } else {*/
    m_shader->ctrlGraph->entry()->clearMarked();
    genNode(m_shader->ctrlGraph->entry());
    
    /*if (m_environment & SH_ARB_NVVP2) {
      m_instructions.push_back(PSInst(SH_ARB_LABEL, getLabel(m_shader->ctrlGraph->exit())));
    }*/
  //}
  m_shader->ctrlGraph->entry()->clearMarked();
  allocRegs();
  
  ShContext::current()->exit();
}

bool PSCode::allocateRegister(const ShVariableNodePtr& var)
{
  if (!var) return true;
  if (var->kind() != SH_TEMP) return true;
  if (var->uniform()) return true;

  if (m_tempRegs.empty()) {
    shError(ShException("DX Backend: Out of registers"));
    return false;
  }

  int idx = m_tempRegs.front();
  m_tempRegs.pop_front();
  if (idx + 1 > m_numTemps) m_numTemps = idx + 1;
  m_registers[var] = new PSReg(SH_PS_REG_TEMP, idx);
  m_reglist.push_back(m_registers[var]);
  
  return true;
}

void PSCode::freeRegister(const ShVariableNodePtr& var)
{
  if (!var) return;
  if (var->kind() != SH_TEMP) return;
  if (var->uniform()) return;

  SH_DEBUG_ASSERT(m_registers.find(var) != m_registers.end());
  m_tempRegs.push_front(m_registers[var]->index);
}

void PSCode::upload()
{
  LPD3DXBUFFER pErrorBuffer, pShaderBuffer;

  // Get the text for the shader
  std::ostringstream out;
  print(out);
  std::string text = out.str();

  if (m_environment & SH_PS_VS_3_0)
  {
	  FILE *fp = fopen("c:\\vertex_ps.txt", "wt");
	  fprintf(fp, "%s\n", text.c_str());
	  fclose(fp);
  }
  else
	{
	  FILE *fp = fopen("c:\\fragment_ps.txt", "wt");
	  fprintf(fp, "%s\n", text.c_str());
	  fclose(fp);
	}

  // Assemble the shader code
  if (FAILED(D3DXAssembleShader(text.c_str(), text.length(), NULL, NULL, 0, &pShaderBuffer, &pErrorBuffer)))
  {
	SH_DEBUG_WARN("Error compiling shader (" << m_unit << "): " << pErrorBuffer->GetBufferPointer());
	  if (m_environment & SH_PS_VS_3_0)
	  {
		  FILE *fp = fopen("c:\\vertex_ps_errors.txt", "wt");
		  fprintf(fp, "%s\n", pErrorBuffer->GetBufferPointer());
		  fclose(fp);
	  }
	  else
		{
		  FILE *fp = fopen("c:\\fragment_ps_errors.txt", "wt");
		  fprintf(fp, "%s\n", pErrorBuffer->GetBufferPointer());
		  fclose(fp);
		}

	return;
  }
  else // Didn't fail: overwrite the error files
  {
	  if (m_environment & SH_PS_VS_3_0)
	  {
		  FILE *fp = fopen("c:\\vertex_ps_errors.txt", "wt");
		  fprintf(fp, "No errors\n");
		  fclose(fp);
	  }
	  else
		{
		  FILE *fp = fopen("c:\\fragment_ps_errors.txt", "wt");
		  fprintf(fp, "No errors\n");
		  fclose(fp);
		}
  }
  // Vertex shader
  if (m_environment & SH_PS_VS_3_0)
  {
	// Create a vertex shader from the compiled code
	if (FAILED(m_pD3DDevice->CreateVertexShader((DWORD*)pShaderBuffer->GetBufferPointer(), &m_pVS)))
	{
	  FILE *fp = fopen("c:\\vertex_ps_errors.txt", "at");
	  fprintf(fp, "Couldn't bind pixel shader\n");
	  fclose(fp);
	  SH_DEBUG_ERROR("Error uploading VS program (" << m_unit << ")");
	  return;
	}

	// Set the shader as active
    m_pD3DDevice->SetVertexShader(m_pVS);
  }
  else if (m_environment & SH_PS_PS_3_0) // Fragment shader
  {
	// Create a fragment shader from the compiled code
	if (FAILED(m_pD3DDevice->CreatePixelShader((DWORD*)pShaderBuffer->GetBufferPointer(), &m_pPS)))
	{
	  FILE *fp = fopen("c:\\fragment_ps_errors.txt", "at");
	  fprintf(fp, "Couldn't bind pixel shader\n");
	  fclose(fp);
	  SH_DEBUG_ERROR("Error uploading PS program (" << m_unit << ")");
	  return;
	}

	// Set the shader as active
    m_pD3DDevice->SetPixelShader(m_pPS);
  }
}

void PSCode::bind()
{
  if (m_pPS == NULL && m_pVS == NULL) {
    upload();
  }
  
  ShContext::current()->set_binding(std::string("ps:") + m_unit,
                                    shref_const_cast<ShProgramNode>(m_originalShader));

  // Initialize constants
  for (RegMap::const_iterator I = m_registers.begin(); I != m_registers.end(); ++I) {
    ShVariableNodePtr node = I->first;
    PSReg reg = *I->second;
    if (node->hasValues() && reg.type == SH_PS_REG_PARAM) {
      updateUniform(node);
    }
  }

  // Make sure all textures are loaded.
  bindTextures();
}

void PSCode::updateUniform(const ShVariableNodePtr& uniform)
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
    
  const PSReg& reg = *I->second;
  
  float values[4];
  for (i = 0; i < uniform->size(); i++) {
    values[i] = (float)uniform->getValue(i);
  }
  for (; i < 4; i++) {
    values[i] = 0.0;
  }
  
  if (reg.type != SH_PS_REG_PARAM) return;
  switch(reg.binding) {
/*  case SH_ARB_REG_PARAMLOC:
    SH_GL_CHECK_ERROR(shGlProgramLocalParameter4fvARB(arbTarget(m_unit), reg.bindingIndex, values));
    break;
  case SH_ARB_REG_PARAMENV:
    SH_GL_CHECK_ERROR(shGlProgramEnvParameter4fvARB(arbTarget(m_unit), reg.bindingIndex, values));
    break;*/
  default:
    return;
  }
}

std::ostream& PSCode::printVar(std::ostream& out, bool dest, const ShVariable& var,
                                bool collectingOp, const ShSwizzle& destSwiz = ShSwizzle(4)) const
{
  RegMap::const_iterator I = m_registers.find(var.node());
  if (I == m_registers.end()) {
    out << "<no reg for " << var.name() << ">";
    return out;
  }
  const PSReg& reg = *I->second;

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
  out << " // " << ++l.line << std::endl;
  return out;
}

bool PSCode::printSamplingInstruction(std::ostream& out, const PSInst& instr) const
{
/*  if (instr.op != SH_ARB_TEX && instr.op != SH_ARB_TXP && instr.op != SH_ARB_TXB)
    return false;

  ShTextureNodePtr texture = shref_dynamic_cast<ShTextureNode>(instr.src[1].node());
  RegMap::const_iterator texRegIt = m_registers.find(instr.src[1].node());
  if (texRegIt == m_registers.end()) {
    SH_DEBUG_PRINT("Unallocated texture found.");
    SH_DEBUG_PRINT("Operation = " << psOpInfo[instr.op].name);
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

  const PSReg& texReg = *texRegIt->second;
  
  out << "  ";
  out << psOpInfo[instr.op].name << " ";
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
  out << ";";
  return true;*/

  // For now, assume our shaders are non-textured
  return false;
}

std::ostream& PSCode::print(std::ostream& out)
{
  LineNumberer endl;
  const char* swizChars = "xyzw";
  int major, minor;

  // Print version header
  if (psTarget(m_unit, m_pD3DDevice, major, minor) == SH_PSTARGET_VS)
  {
	out << "vs.3.0" << endl;
  }
  else
  {
	  out << "ps.3.0" << endl;
  }
  
  // Print register declarations
  
  for (RegList::const_iterator I = m_reglist.begin();
       I != m_reglist.end(); ++I) {
    if ((*I)->type == SH_PS_REG_TEMP) continue;
    if ((*I)->type == SH_PS_REG_TEXTURE) continue;
    out << "  ";
    (*I)->printDecl(out);
    out << endl;
  }

  out << endl;
  
  // Print instructions
  for (PSInstList::const_iterator I = m_instructions.begin();
       I != m_instructions.end(); ++I) {
    /*if (I->op == SH_ARB_LABEL) {
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
      out << ";";*/
    //} else
	if (!printSamplingInstruction(out, *I)) {
      out << "  ";
      out << psOpInfo[I->op].name;
      //if (I->update_cc) out << "C";
      out << " ";
      printVar(out, true, I->dest, psOpInfo[I->op].collectingOp);
      /*if (I->ccode != PSInst::NOCC) {
        out << " (";
        out << arbCCnames[I->ccode];
        out << ".";
        for (int i = 0; i < 4; i++) {
          out << swizChars[(i < I->ccswiz.size() ? I->ccswiz[i]
                            : (I->ccswiz.size() == 1 ? I->ccswiz[0] : i))];
        }
        out << ") ";
      }*/
      for (int i = 0; i < psOpInfo[I->op].arity; i++) {
        out << ", ";
        printVar(out, false, I->src[i], psOpInfo[I->op].collectingOp, I->dest.swizzle());
      }
      out << ';';
    }
    out << " // ";
    if (I->dest.node() && I->dest.has_name()) {
      out << "d=" << I->dest.name() << " ";
    }
    for (int i = 0; i < 3; i++) {
      if (I->src[i].node()  && I->src[i].has_name()) {
        out << "s[" << i << "]=" << I->src[i].name() << " ";
      }
    }
    out << endl;
  }

  return out;
}

std::ostream& PSCode::printInputOutputFormat(std::ostream& out) {
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

int PSCode::getLabel(ShCtrlGraphNodePtr node)
{
  if (m_label_map.find(node) == m_label_map.end()) {
    m_label_map[node] = m_max_label++;
  }
  return m_label_map[node];
}

void PSCode::genNode(ShCtrlGraphNodePtr node)
{
  if (!node || node->marked()) return;
  node->mark();

  if (node == m_shader->ctrlGraph->exit()) return;
  
  //if (m_environment & SH_ARB_NVVP2) {
  //  m_instructions.push_back(PSInst(SH_ARB_LABEL, getLabel(node)));
  //}
  
  if (node->block) for (ShBasicBlock::ShStmtList::const_iterator I = node->block->begin();
       I != node->block->end(); ++I) {
    const ShStatement& stmt = *I;
    emit(stmt);
  }

  /*if (m_environment & SH_ARB_NVVP2) {
    for(std::vector<SH::ShCtrlGraphBranch>::iterator I = node->successors.begin();
	I != node->successors.end(); I++) {
      m_instructions.push_back(PSInst(SH_ARB_BRA, getLabel(I->node), I->cond));
    }
    m_instructions.push_back(PSInst(SH_ARB_BRA, getLabel(node->follower)));
    for(std::vector<SH::ShCtrlGraphBranch>::iterator I = node->successors.begin();
	I != node->successors.end(); I++) {
      genNode(I->node);
    }
  }*/

  genNode(node->follower);
}

void PSCode::genStructNode(const ShStructuralNodePtr& node)
{
/*  if (!node) return;

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
    m_instructions.push_back(PSInst(SH_ARB_IF, ShVariable(), cond)); {
      genStructNode(ifnode);
    } m_instructions.push_back(PSInst(SH_ARB_ELSE, ShVariable())); {
      genStructNode(elsenode);
    } m_instructions.push_back(PSInst(SH_ARB_ENDIF, ShVariable()));
  } else if (node->type == ShStructuralNode::WHILELOOP) {
    ShStructuralNodePtr header = node->structnodes.front();

    ShVariable cond = header->succs.front().first;
    
    ShStructuralNodePtr body = node->structnodes.back();

    ShVariable maxloop(new ShVariableNode(SH_CONST, 1));
    float maxloopval = 255.0;
    maxloop.setValues(&maxloopval);
    m_shader->constants.push_back(maxloop.node());
    m_instructions.push_back(PSInst(SH_ARB_REP, ShVariable(), maxloop));
    genStructNode(header);
    PSInst brk(SH_ARB_BRK, ShVariable(), cond);
    brk.invert = true;
    m_instructions.push_back(brk);
    genStructNode(body);
    
    m_instructions.push_back(PSInst(SH_ARB_ENDREP, ShVariable()));
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
    m_instructions.push_back(PSInst(SH_ARB_REP, ShVariable(), maxloop));
    genStructNode(loopnode);
    PSInst brk(SH_ARB_BRK, ShVariable(), cond);
    if (!condexit) {
      brk.invert = true;
    } 
    m_instructions.push_back(brk);
    m_instructions.push_back(PSInst(SH_ARB_ENDREP, ShVariable()));
  }*/
}

void PSCode::allocRegs()
{
  PSLimits limits(m_unit, m_pD3DDevice);

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

void PSCode::bindSpecial(const ShProgramNode::VarList::const_iterator& begin,
                          const ShProgramNode::VarList::const_iterator& end,
                          const PSBindingSpecs& specs, 
                          std::vector<int>& bindings,
                          PSRegType type, int& num)
{
  bindings.push_back(0);
  
  if (specs.semanticType == SH_ATTRIB) return;
  
  for (ShProgramNode::VarList::const_iterator I = begin; I != end; ++I) {
    ShVariableNodePtr node = *I;
    
    if (m_registers.find(node) != m_registers.end()) continue;
    if (node->specialType() != specs.semanticType) continue;
    
    m_registers[node] = new PSReg(type, num++, node->name());
    m_registers[node]->binding = specs.binding;
    m_registers[node]->bindingIndex = bindings.back();
    m_reglist.push_back(m_registers[node]);
    
    bindings.back()++;
    if (bindings.back() == specs.maxBindings) break;
  }    
}

void PSCode::allocInputs(const PSLimits& limits)
{
  // First, try to assign some "special" output register bindings
  for (int i = 0; psBindingSpecs(false, m_unit)[i].binding != SH_PS_REG_NONE; i++) {
    bindSpecial(m_shader->inputs.begin(), m_shader->inputs.end(),
                psBindingSpecs(false, m_unit)[i], m_inputBindings,
                SH_PS_REG_INPUT, m_numInputs);
  }
  
  for (ShProgramNode::VarList::const_iterator I = m_shader->inputs.begin();
       I != m_shader->inputs.end(); ++I) {
    ShVariableNodePtr node = *I;
    if (m_registers.find(node) != m_registers.end()) continue;
    m_registers[node] = new PSReg(SH_PS_REG_INPUT, m_numInputs++, node->name());
    m_reglist.push_back(m_registers[node]);

    // Binding
    for (int i = 0; psBindingSpecs(false, m_unit)[i].binding != SH_PS_REG_NONE; i++) {
      const PSBindingSpecs& specs = psBindingSpecs(false, m_unit)[i];

      if (specs.allowGeneric && m_inputBindings[i] < specs.maxBindings) {
        m_registers[node]->binding = specs.binding;
        m_registers[node]->bindingIndex = m_inputBindings[i];
        m_inputBindings[i]++;
        break;
      }
    }
  }
}

void PSCode::allocOutputs(const PSLimits& limits)
{
  // First, try to assign some "special" output register bindings
  for (int i = 0; psBindingSpecs(true, m_unit)[i].binding != SH_PS_REG_NONE; i++) {
	if (m_environment & SH_PS_VS_3_0) // Vertex shader
	    bindSpecial(m_shader->outputs.begin(), m_shader->outputs.end(),
                psBindingSpecs(true, m_unit)[i], m_outputBindings,
                SH_PS_REG_VSOUTPUT, m_numOutputs);
	else if (m_environment & SH_PS_PS_3_0) // Pixel shader
	    bindSpecial(m_shader->outputs.begin(), m_shader->outputs.end(),
                psBindingSpecs(true, m_unit)[i], m_outputBindings,
                SH_PS_REG_PSOUTPUT, m_numOutputs);
  }

  for (ShProgramNode::VarList::const_iterator I = m_shader->outputs.begin();
       I != m_shader->outputs.end(); ++I) {

    ShVariableNodePtr node = *I;
    if (m_registers.find(node) != m_registers.end()) continue;
	if (m_environment & SH_PS_VS_3_0) // Vertex shader
	    m_registers[node] = new PSReg(SH_PS_REG_VSOUTPUT, m_numOutputs++, node->name());
	else if (m_environment & SH_PS_PS_3_0) // Fragment shader
	    m_registers[node] = new PSReg(SH_PS_REG_PSOUTPUT, m_numOutputs++, node->name());
    m_reglist.push_back(m_registers[node]);
    
    // Binding
    for (int i = 0; psBindingSpecs(true, m_unit)[i].binding != SH_PS_REG_NONE; i++) {
      const PSBindingSpecs& specs = psBindingSpecs(true, m_unit)[i];

      if (specs.allowGeneric && m_outputBindings[i] < specs.maxBindings) {
        m_registers[node]->binding = specs.binding;
        m_registers[node]->bindingIndex = m_outputBindings[i];
        m_outputBindings[i]++;
        break;
      }
    }
  }
}

void PSCode::allocParam(const PSLimits& limits, const ShVariableNodePtr& node)
{
  // TODO: Check if we reached maximum
  if (m_registers.find(node) != m_registers.end()) return;
  m_registers[node] = new PSReg(SH_PS_REG_PARAM, m_numParams, node->name());
  //m_registers[node]->binding = SH_PS_REG_PARAM;
  m_registers[node]->bindingIndex = m_numParams;
  m_reglist.push_back(m_registers[node]);
  m_numParams++;
}

void PSCode::allocConsts(const PSLimits& limits)
{
  for (ShProgramNode::VarList::const_iterator I = m_shader->constants.begin();
       I != m_shader->constants.end(); ++I) {
    ShVariableNodePtr node = *I;

    // TODO: improve efficiency
    RegMap::const_iterator J;
    for (J = m_registers.begin(); J != m_registers.end(); ++J) {
      if (J->second->type != SH_PS_REG_CONST) continue;
      int f = 0;
      for (int i = 0; i < node->size(); i++) {
        if (J->second->values[i] == node->getValue(i)) f++;
      }
      if (f == node->size()) break;
    }
    if (J == m_registers.end()) {
      m_registers[node] = new PSReg(SH_PS_REG_CONST, m_numParams, node->name());
      m_reglist.push_back(m_registers[node]);
      for (int i = 0; i < 4; i++) {
        m_registers[node]->values[i] = (float)(i < node->size() ? node->getValue(i) : 0.0);
      }
      m_numParams++;
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

struct PSScope {
  PSScope(int start)
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

void PSCode::allocTemps(const PSLimits& limits)
{
  typedef std::list<PSScope> ScopeStack;
  ScopeStack scopestack;
  
  ShLinearAllocator allocator(this);

  {
    ScopeStack scopestack;
    // First do a backwards traversal to find loop nodes that need to be
    // marked due to later uses of assignments

    // push root stack
    scopestack.push_back(m_instructions.size() - 1);
    
    for (int i = (int)m_instructions.size() - 1; i >= 0; --i) {
      PSInst instr = m_instructions[i];
      /*if (instr.op == SH_ARB_ENDREP) {
        scopestack.push_back((int)i);
      }
      if (instr.op == SH_ARB_REP) {
        const ArbScope& scope = scopestack.back();
        for (ArbScope::MarkList::const_iterator I = scope.need_mark.begin();
             I != scope.need_mark.end(); ++I) {
          mark(allocator, *I, (int)i);
        }
        scopestack.pop_back();
      }*/

      if (markable(instr.dest.node())) {
        std::bitset<4> writemask;
        for (int k = 0; k < instr.dest.size(); k++) {
          writemask[instr.dest.swizzle()[k]] = true;
        }
        std::bitset<4> used;
        for (ScopeStack::iterator S = scopestack.begin(); S != scopestack.end(); ++S) {
          PSScope& scope = *S;

          if ((used & writemask).any()) {
            mark(allocator, instr.dest.node().object(), scope.start);
            scope.need_mark.insert(instr.dest.node().object());
          }
          
          used |= scope.usage_map[instr.dest.node().object()];
        }

        PSScope& scope = scopestack.back();
        scope.usage_map[instr.dest.node().object()] &= ~writemask;
      }
      
      for (int j = 0; j < 3; j++) {
        if (!markable(instr.src[j].node())) continue;
        std::bitset<4> usemask;
        for (int k = 0; k < instr.src[j].size(); k++) {
          usemask[instr.src[j].swizzle()[k]] = true;
        }
        PSScope& scope = scopestack.back();
        scope.usage_map[instr.src[j].node().object()] |= usemask;
      }
    }
  }

  for (std::size_t i = 0; i < m_instructions.size(); i++) {
    PSInst instr = m_instructions[i];
    /*if (instr.op == SH_ARB_REP) {
      scopestack.push_back((int)i);
    }
    if (instr.op == SH_ARB_ENDREP) {
      const ArbScope& scope = scopestack.back();
      for (ArbScope::MarkList::const_iterator I = scope.need_mark.begin();
           I != scope.need_mark.end(); ++I) {
        mark(allocator, *I, (int)i);
      }
      scopestack.pop_back();
    }*/

    if (mark(allocator, instr.dest.node(), (int)i)) {
      for (ScopeStack::iterator S = scopestack.begin(); S != scopestack.end(); ++S) {
        PSScope& scope = *S;
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
    
    for (int j = 0; j < 3; j++) {
      if (mark(allocator, instr.src[j].node(), (int)i)) {
        for (ScopeStack::iterator S = scopestack.begin(); S != scopestack.end(); ++S) {
          PSScope& scope = *S;
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
  for (int i = 0; i < limits.regs(); i++) {
    m_tempRegs.push_back(i);
  }
  
  allocator.allocate();
  
  m_tempRegs.clear();
}

void PSCode::allocTextures(const PSLimits& limits)
{
  for (ShProgramNode::TexList::const_iterator I = m_shader->textures.begin();
       I != m_shader->textures.end(); ++I) {
    ShTextureNodePtr node = *I;
    int index;
    index = m_numTextures;
    m_registers[node] = new PSReg(SH_PS_REG_TEXTURE, index, node->name());
    m_reglist.push_back(m_registers[node]);
    m_numTextures++;
  }
}

void PSCode::bindTextures()
{
  /*for (ShProgramNode::TexList::const_iterator I = m_shader->textures.begin();
       I != m_shader->textures.end(); ++I) {
    m_textures->bindTexture(*I, GL_TEXTURE0 + m_registers[*I]->index);
  }*/
}

}
