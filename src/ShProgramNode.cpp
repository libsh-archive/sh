// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#include "ShProgramNode.hpp"
#include <cassert>
#include <algorithm>
#include <sstream>
#include <fstream>
#include "ShBackend.hpp"
#include "ShEnvironment.hpp"
#include "ShContext.hpp"
#include "ShDebug.hpp"
#include "ShTextureNode.hpp"
#include "ShPaletteNode.hpp"
#include "ShCtrlGraph.hpp"
#include "ShError.hpp"

namespace SH {

ShProgramNode::ShProgramNode(const std::string& target)
  : m_target(target), m_finished(false),
    m_assigned_var(0)
{
}

ShProgramNode::~ShProgramNode()
{
  // TODO: maybe need some cleanup here, although refcounting ought to
  // take care of it.
}

void ShProgramNode::compile(const ShPointer<ShBackend>& backend)
{
  if (m_target.empty()) shError( ShException( "Invalid ShProgram target" ) );
  compile(m_target, backend);
}

void ShProgramNode::compile(const std::string& target, const ShPointer<ShBackend>& backend)
{
  if (!backend) return;

  ShContext::current()->enter(this);
  ShBackendCodePtr code = 0;
  try {
    collectDecls();
    collectVariables();
    code = backend->generate_code(target, this);
#ifdef SH_DEBUG
    // code->print(std::cerr);
#endif
  } catch (...) {
    ShContext::current()->exit();
    throw;
  }
  ShContext::current()->exit();
  m_code[std::make_pair(target, backend)] = code;
}

ShPointer<ShBackendCode> ShProgramNode::code() {
  return code(ShEnvironment::backend);
}

ShPointer<ShBackendCode> ShProgramNode::code(const ShPointer<ShBackend>& backend) {
  if (m_target.empty()) shError( ShException( "Invalid ShProgram target" ) );

  return code(m_target, backend);
}

ShPointer<ShBackendCode> ShProgramNode::code(const std::string& target, const ShPointer<ShBackend>& backend) {
  if (!backend) return 0;

  if (m_code.find(std::make_pair(target, backend)) == m_code.end()) compile(target, backend);

  return m_code[std::make_pair(target, backend)];
}

std::string ShProgramNode::describe_interface() const
{
  std::ostringstream os;
  os << "Interface for ";
  if (has_name()) {
    os << name();
  } else {
    os << "<anonymous program>";
  }
  os << std::endl;
  os << std::endl;
  os << "Inputs:" << std::endl;
  os << describe(inputs) << std::endl;
  os << "Outputs:" << std::endl;
  os << describe(outputs) << std::endl;
  os << "Channels:" << std::endl;
  os << describe(channels) << std::endl;
  os << "Uniforms:" << std::endl;
  os << describe(uniforms) << std::endl;

  return os.str();
}

std::string ShProgramNode::describe_vars() const
{
  std::ostringstream os;
  os << describe_interface(); 
  os << "Temps:" << std::endl;
  os << describe(temps) << std::endl;
  os << "Constants:" << std::endl;
  os << describe(constants) << std::endl;
  return os.str();
}

std::string ShProgramNode::describe_decls() const
{
  std::ostringstream os;
  os << "Temp Declarations:" << std::endl;
  for(VarSet::const_iterator I = tempDecls.begin(); I != tempDecls.end(); ++I) {
    os << (*I)->nameOfType() << " " << (*I)->name() << std::endl;
  }
  return os.str();
}

void ShProgramNode::dump(std::string filename) const
{
  SH_DEBUG_PRINT("Dumping " << filename);
  std::string varfile = filename + ".vars";
  std::ofstream varout(varfile.c_str());
  varout << "ShProgram "; 
  varout << describe_vars(); 
  varout << describe_decls();

  std::ostringstream sout;
  ctrlGraph->graphvizDump(sout);
  shDotGen(sout.str(), filename);
}

void ShProgramNode::updateUniform(const ShVariableNodePtr& uniform)
{
  for (CodeMap::iterator I = m_code.begin(); I != m_code.end(); ++I) {
    I->second->updateUniform(uniform);
  }
}

void ShProgramNode::collectVariables()
{
  temps.clear();
  uniforms.clear();
  constants.clear();
  textures.clear();
  channels.clear();
  palettes.clear();
  if (ctrlGraph->entry()) {
    ctrlGraph->entry()->clearMarked();
    collectNodeVars(ctrlGraph->entry());
    ctrlGraph->entry()->clearMarked();
  }
}

void ShProgramNode::collectDecls()
{
  tempDecls.clear();
  // @todo range - use collectVariables temps result
  // to pare out unnecessary declarations (i.e. variables
  // that may have disappeared already due to transformations)
  // (may actually want to put this cleaning up step in collectVariables
  // since some temps might be removed during optimizations)
  if (ctrlGraph->entry()) {
    ctrlGraph->entry()->clearMarked();
    collectNodeDecls(ctrlGraph->entry());
    ctrlGraph->entry()->clearMarked();
  }
}

bool ShProgramNode::hasDecl(ShVariableNodePtr node) const {
  return tempDecls.find(node) != tempDecls.end();
}

void ShProgramNode::addDecl(ShVariableNodePtr node, ShCtrlGraphNodePtr cfgNode) {
  tempDecls.insert(node);
  SH_DEBUG_ASSERT(ctrlGraph->entry());
  cfgNode->addDecl(node);
}

void ShProgramNode::addDecl(ShVariableNodePtr node) {
  addDecl(node, ctrlGraph->entry());
}

void ShProgramNode::collectNodeDecls(const ShCtrlGraphNodePtr& node)
{
  if(node->marked()) return;
  node->mark();
  tempDecls.insert(node->decl_begin(), node->decl_end());
  /* @todo range
  for(ShCtrlGraphNode::DeclIt I = node->decl_begin(); I != node->decl_end(); ++I) {
    SH_DEBUG_PRINT("  collectDecls - " << (*I)->name());
  }
  */

  for (std::vector<ShCtrlGraphBranch>::const_iterator J = node->successors.begin();
       J != node->successors.end(); ++J) {
    collectNodeDecls(J->node);
  }
  
  if (node->follower) collectNodeDecls(node->follower);
}

void ShProgramNode::collectNodeVars(const ShCtrlGraphNodePtr& node)
{
  if (node->marked()) return;
  node->mark();

  if (node->block) {
    for (ShBasicBlock::ShStmtList::const_iterator I = node->block->begin();
         I != node->block->end(); ++I) {
      
      collectVar(I->dest.node());
      collectVar(I->src[0].node());
      collectVar(I->src[1].node());
      collectVar(I->src[2].node());
    }
  }

  for (std::vector<ShCtrlGraphBranch>::const_iterator J = node->successors.begin();
       J != node->successors.end(); ++J) {
    collectNodeVars(J->node);
  }
  
  if (node->follower) collectNodeVars(node->follower);
}

void ShProgramNode::collectVar(const ShVariableNodePtr& var)
{
  if (!var) return;
  if (var->uniform()) {
    if (std::find(uniforms.begin(), uniforms.end(), var) == uniforms.end()) {
      uniforms.push_back(var);
    }
  } else switch (var->kind()) {
  case SH_INPUT:
  case SH_OUTPUT:
  case SH_INOUT:
    // Taken care of by ShVariableNode constructor
    break;
  case SH_TEMP:
    if (std::find(temps.begin(), temps.end(), var) == temps.end()) {
      temps.push_back(var);
    }
    break;
  case SH_CONST:
    if (std::find(constants.begin(), constants.end(), var) == constants.end()) {
      constants.push_back(var);
    }
    break;
  case SH_TEXTURE:
    if (std::find(textures.begin(), textures.end(),
                  shref_dynamic_cast<ShTextureNode>(var)) == textures.end()) {
      textures.push_back(shref_dynamic_cast<ShTextureNode>(var));
    }    
    break;
  case SH_STREAM:
    if (std::find(channels.begin(), channels.end(),
                  shref_dynamic_cast<ShChannelNode>(var)) == channels.end()) {
      channels.push_back(shref_dynamic_cast<ShChannelNode>(var));
    }
  case SH_PALETTE:
    if (std::find(palettes.begin(), palettes.end(),
                  shref_dynamic_cast<ShPaletteNode>(var)) == palettes.end()) {
      palettes.push_back(shref_dynamic_cast<ShPaletteNode>(var));
    }
    break;
  default:
    SH_DEBUG_ASSERT(0);
  }
}

std::ostream& ShProgramNode::print(std::ostream& out,
                                   const ShProgramNode::VarList& varList)
{
  out << "(";
  for(VarList::const_iterator it = varList.begin(); it != varList.end(); ++it) {
    if( it != varList.begin() ) out << ", ";
    out << (*it)->nameOfType() << " " << (*it)->name(); 
  }
  out << ")";
  return out;
}

ShPointer<ShProgramNode> ShProgramNode::clone() const
{
  ShCtrlGraphNodePtr head, tail;
  ctrlGraph->copy(head, tail);

  ShProgramNodePtr result = new ShProgramNode(target());
  result->ShInfoHolder::operator=(*this);
  result->ShMeta::operator=(*this);
  result->ctrlGraph = new ShCtrlGraph(head, tail);
  result->inputs = inputs;
  result->outputs = outputs;
  result->collectDecls();
  result->collectVariables();
  return result;
}

ShProgramNode::VarList::const_iterator ShProgramNode::inputs_begin() const
{
  return inputs.begin();
}

ShProgramNode::VarList::const_iterator ShProgramNode::inputs_end() const
{
  return inputs.end();
}

ShProgramNode::VarList::const_iterator ShProgramNode::outputs_begin() const
{
  return outputs.begin();
}

ShProgramNode::VarList::const_iterator ShProgramNode::outputs_end() const
{
  return outputs.end();
}

ShProgramNode::VarList::const_iterator ShProgramNode::temps_begin() const
{
  return temps.begin();
}

ShProgramNode::VarList::const_iterator ShProgramNode::temps_end() const
{
  return temps.end();
}

ShProgramNode::VarList::const_iterator ShProgramNode::constants_begin() const
{
  return constants.begin();
}

ShProgramNode::VarList::const_iterator ShProgramNode::constants_end() const
{
  return constants.end();
}

ShProgramNode::VarList::const_iterator ShProgramNode::uniforms_begin() const
{
  return uniforms.begin();
}

ShProgramNode::VarList::const_iterator ShProgramNode::uniforms_end() const
{
  return uniforms.end();
}

ShProgramNode::TexList::const_iterator ShProgramNode::textures_begin() const
{
  return textures.begin();
}

ShProgramNode::TexList::const_iterator ShProgramNode::textures_end() const
{
  return textures.end();
}

ShProgramNode::ChannelList::const_iterator ShProgramNode::channels_begin() const
{
  return channels.begin();
}

ShProgramNode::ChannelList::const_iterator ShProgramNode::channels_end() const
{
  return channels.end();
}

ShProgramNode::PaletteList::const_iterator ShProgramNode::palettes_begin() const
{
  return palettes.begin();
}

ShProgramNode::PaletteList::const_iterator ShProgramNode::palettes_end() const
{
  return palettes.end();
}

bool ShProgramNode::finished() const
{
  return m_finished;
}

void ShProgramNode::finish()
{
  m_finished = true;
  if (m_assigned_var) {
    m_assigned_var->attach(this);
    m_assigned_var = 0;
  }
}

void ShProgramNode::assign(const ShVariableNodePtr& var) const
{
  m_assigned_var = var;
}

std::string ShProgramNode::describe(const VarList &varlist) {
  std::ostringstream os;
  for(VarList::const_iterator I = varlist.begin(); I != varlist.end(); ++I) {
    os << "  " << (*I)->nameOfType() << " " << (*I)->name() << std::endl;
  }
  return os.str();
}

std::string ShProgramNode::describe(const ChannelList& chanlist) {
  std::ostringstream os;
  for(ChannelList::const_iterator I = chanlist.begin(); I != chanlist.end(); ++I) {
    os << "  " << (*I)->nameOfType() << " " << (*I)->name() << std::endl;
  }
  return os.str();
}

}
