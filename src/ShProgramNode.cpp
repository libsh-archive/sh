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
#include "ShProgramNode.hpp"
#include <cassert>
#include <algorithm>
#include <sstream>
#include <fstream>
#include "ShBackend.hpp"
#include "ShContext.hpp"
#include "ShDebug.hpp"
#include "ShTextureNode.hpp"
#include "ShPaletteNode.hpp"
#include "ShCtrlGraph.hpp"
#include "ShError.hpp"

namespace {

using namespace SH;

std::string describe(const ShProgramNode::VarList &varlist)
{
  std::ostringstream os;
  for (ShProgramNode::VarList::const_iterator I = varlist.begin(); I != varlist.end(); ++I) {
    os << "  " << (*I)->nameOfType() << " " << (*I)->name() << std::endl;
  }
  return os.str();
}

std::string describe(const ShProgramNode::TexList &texlist)
{
  std::ostringstream os;
  for (ShProgramNode::TexList::const_iterator I = texlist.begin(); I != texlist.end(); ++I) {
    os << "  " << (*I)->nameOfType() << " " << (*I)->name() << std::endl;
  }
  return os.str();
}

std::string describe(const ShProgramNode::ChannelList& chanlist)
{
  std::ostringstream os;
  for (ShProgramNode::ChannelList::const_iterator I = chanlist.begin(); I != chanlist.end(); ++I) {
    os << "  " << (*I)->nameOfType() << " " << (*I)->name() << std::endl;
  }
  return os.str();
}

}

namespace SH {

ShProgramNode::ShProgramNode(const std::string& target)
  : m_backend_name(""), m_target(target), m_finished(false),
    m_assigned_var(0)
{
}

ShProgramNode::~ShProgramNode()
{
}

void ShProgramNode::compile(const ShPointer<ShBackend>& backend)
{
  if (m_target.empty()) shError(ShException("Empty ShProgram target"));
  compile(m_target, backend);
}

void ShProgramNode::compile(const std::string& target, const ShPointer<ShBackend>& backend)
{
  if (!backend) return;
  if (target.empty()) shError(ShException("Empty ShProgram target"));
  if (!ctrlGraph || !ctrlGraph->entry()) {
    shError(ShException("Impossible to compile an empty ShProgram."));
  } 

  ShContext::current()->enter(this);
  ShBackendCodePtr code = 0;
  try {
    collectDecls();
    collectVariables();
    code = backend->generate_code(target, this);
  } catch (...) {
    ShContext::current()->exit();
    throw;
  }
  ShContext::current()->exit();
  if (code) {
    m_code[std::make_pair(target, backend)] = code;
    m_backend_name = backend->name();
  }
}

bool ShProgramNode::is_compiled() const
{
  if (m_target.empty()) shError( ShException( "Invalid ShProgram target" ) );

  return is_compiled(m_target, ShBackend::get_backend(m_target));
}

bool ShProgramNode::is_compiled(const std::string& target) const
{
  if (target.empty()) shError( ShException( "Invalid compilation target" ) );

  return is_compiled(target, ShBackend::get_backend(target));
}

bool ShProgramNode::is_compiled(const std::string& target, const ShPointer<ShBackend>& backend) const
{
  // Try for a perfect match first
  if (m_code.find(std::make_pair(target, backend)) != m_code.end()) return true;

  // Look for a derived target
  std::list<std::string> derived_targets = ShBackend::derived_targets(target);
  for (std::list<std::string>::const_iterator i = derived_targets.begin(); 
       i != derived_targets.end(); i++) {
    if (m_code.find(std::make_pair(*i, backend)) != m_code.end()) return true;
  }

  return false;
}

ShPointer<ShBackendCode> ShProgramNode::code()
{
  return code(ShBackend::get_backend(m_target));
}

ShPointer<ShBackendCode> ShProgramNode::code(const ShPointer<ShBackend>& backend)
{
  if (m_target.empty()) shError( ShException( "Invalid ShProgram target" ) );

  return code(m_target, backend);
}

ShPointer<ShBackendCode> ShProgramNode::code(const std::string& target, const ShPointer<ShBackend>& backend)
{
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
  os << describe(all_uniforms) << std::endl;
  os << "Arrays:" << std::endl;
  os << describe(textures) << std::endl;

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
  for (VarSet::const_iterator I = tempDecls.begin(); I != tempDecls.end(); ++I) {
    os << (*I)->nameOfType() << " " << (*I)->name() << std::endl;
  }
  return os.str();
}

std::string ShProgramNode::describe_bindings()
{
  std::ostringstream os;
  os << "Bindings for ";
  if (has_name()) {
    os << name();
  } else {
    os << "<anonymous program>";
  }
  os << std::endl;
  os << std::endl;
  code()->describe_bindings(os);
  return os.str();
}

std::string ShProgramNode::describe_bindings(const std::string& target)
{
  std::ostringstream os;
  os << "Bindings for ";
  if (has_name()) {
    os << name();
  } else {
    os << "<anonymous program>";
  }
  os << std::endl;
  os << std::endl;
  ShBackendPtr backend = ShBackend::get_backend(target);
  code(target, backend)->describe_bindings(os);
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

  std::string dotfile = filename + ".dot";
  std::string psfile = filename + ".ps";
  std::ofstream dotout(dotfile.c_str());
  ctrlGraph->graphvizDump(dotout);
  std::string cmd = std::string("dot -Tps < ") + dotfile + " > " + psfile; 
  system(cmd.c_str());
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
  all_uniforms.clear();
  constants.clear();
  textures.clear();
  channels.clear();
  palettes.clear();
  if (ctrlGraph->entry()) {
    ctrlGraph->entry()->clearMarked();
    collect_node_vars(ctrlGraph->entry());
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
    collect_node_decls(ctrlGraph->entry());
    ctrlGraph->entry()->clearMarked();
  }
}

bool ShProgramNode::hasDecl(const ShVariableNodePtr& node) const
{
  return tempDecls.find(node) != tempDecls.end();
}

void ShProgramNode::addDecl(const ShVariableNodePtr& node, const ShCtrlGraphNodePtr& cfgNode)
{
  tempDecls.insert(node);
  SH_DEBUG_ASSERT(ctrlGraph->entry());
  cfgNode->addDecl(node);
}

void ShProgramNode::addDecl(const ShVariableNodePtr& node)
{
  addDecl(node, ctrlGraph->entry());
}

void ShProgramNode::collect_node_decls(const ShCtrlGraphNodePtr& node)
{
  if (node->marked()) return;
  node->mark();
  tempDecls.insert(node->decl_begin(), node->decl_end());
  /* @todo range
  for(ShCtrlGraphNode::DeclIt I = node->decl_begin(); I != node->decl_end(); ++I) {
    SH_DEBUG_PRINT("  collectDecls - " << (*I)->name());
  }
  */

  for (std::vector<ShCtrlGraphBranch>::const_iterator J = node->successors.begin();
       J != node->successors.end(); ++J) {
    collect_node_decls(J->node);
  }
  
  if (node->follower) collect_node_decls(node->follower);
}

void ShProgramNode::collect_node_vars(const ShCtrlGraphNodePtr& node)
{
  if (node->marked()) return;
  node->mark();

  if (node->block) {
    for (ShBasicBlock::ShStmtList::const_iterator I = node->block->begin();
         I != node->block->end(); ++I) {
      
      collect_var(I->dest.node());
      collect_var(I->src[0].node());
      collect_var(I->src[1].node());
      collect_var(I->src[2].node());
    }
  }

  for (std::vector<ShCtrlGraphBranch>::const_iterator J = node->successors.begin();
       J != node->successors.end(); ++J) {
    collect_node_vars(J->node);
  }
  
  if (node->follower) collect_node_vars(node->follower);
}

void ShProgramNode::collect_dependent_uniform(const ShVariableNodePtr& var)
{
  all_uniforms.push_back(var);

  // Get all of the recursively dependent uniforms
  if (var->evaluator()) {
    for (ShProgramNode::VarList::const_iterator I = var->evaluator()->uniforms.begin();
         I != var->evaluator()->uniforms.end(); ++I) {
      if (std::find(all_uniforms.begin(), all_uniforms.end(), *I) == all_uniforms.end()) {
        collect_dependent_uniform(*I);
      }
    }
  }
}

void ShProgramNode::collect_var(const ShVariableNodePtr& var)
{
  if (!var) return;
  if (var->uniform()) {
    if (std::find(uniforms.begin(), uniforms.end(), var) == uniforms.end()) {
      uniforms.push_back(var);
      collect_dependent_uniform(var);
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
  for (VarList::const_iterator it = varList.begin(); it != varList.end(); ++it) {
    if (it != varList.begin()) out << ", ";
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

void ShProgramNode::finish()
{
  m_finished = true;
  if (m_assigned_var) {
    m_assigned_var->attach(this);
    m_assigned_var = 0;
  }
}

}
