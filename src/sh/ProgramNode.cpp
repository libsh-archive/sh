// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
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
#include "ProgramNode.hpp"
#include <cassert>
#include <algorithm>
#include <sstream>
#include <fstream>
#include "Backend.hpp"
#include "Context.hpp"
#include "Debug.hpp"
#include "TextureNode.hpp"
#include "PaletteNode.hpp"
#include "CtrlGraph.hpp"
#include "Error.hpp"
#include "Internals.hpp"
#include "Transformer.hpp"

#define CLEAN_DECLS true 

namespace {

using namespace SH;

std::string describe(const ProgramNode::VarList &varlist)
{
  std::ostringstream os;
  for (ProgramNode::VarIt I = varlist.begin(); I != varlist.end(); ++I) {
    os << "  " << (*I)->nameOfType() << " " << (*I)->name() << std::endl;
  }
  return os.str();
}

std::string describe(const ProgramNode::TexList &texlist)
{
  std::ostringstream os;
  for (ProgramNode::TexList::const_iterator I = texlist.begin(); I != texlist.end(); ++I) {
    os << "  " << (*I)->nameOfType() << " " << (*I)->name() << std::endl;
  }
  return os.str();
}

std::string describe_stats(const ProgramNode::VarList &varlist) {
  std::ostringstream os;
  std::vector<int> sizecount;
  for(ProgramNode::VarIt I = varlist.begin(); I != varlist.end(); ++I) {
    if((*I)->size() > static_cast<int>(sizecount.size())) {
      sizecount.resize((*I)->size());
    }
    sizecount[(*I)->size() - 1]++;
  }
  for(size_t i = 0; i < sizecount.size(); ++i) {
    if(sizecount[i] == 0) continue;
    os << "size " << i + 1 << sizecount[i] << std::endl;
  }
  return os.str();
}

struct VariableClonerBase: public TransformerParent {
  VarMap vmap;
  void handleVarList(ProgramNode::VarList &varlist, BindingType type) {
    if(type == SH_CONST) return;
    for(ProgramNode::VarList::iterator V = varlist.begin(); V != varlist.end(); ++V) {
      if((*V)->uniform()) continue;
      vmap[*V] = (*V)->clone(BINDINGTYPE_END, 0, VALUETYPE_END, SEMANTICTYPE_END, false);
    }
  }

};
typedef DefaultTransformer<VariableClonerBase> VariableCloner;

struct StatementCounterBase: public TransformerParent {
  /* Splits a statement.  Returns whether the operator requires splicing in a new program
   * fragment. 
   */
  int count;
  StatementCounterBase(): count(0) {}
  bool handleStmt(BasicBlock::StmtList::iterator& I, CtrlGraphNode* node)
  {
    count ++;
    return false;
  }
};
typedef DefaultTransformer<StatementCounterBase> StatementCounter;
}

namespace SH {

ProgramNode::ProgramNode(const std::string& target)
  : m_backend_name(""), m_target(target), m_finished(false),
    m_assigned_var(0)
{
}

ProgramNode::~ProgramNode()
{
}

void ProgramNode::compile(const Pointer<Backend>& backend)
{
  if (m_target.empty()) error(Exception("Empty Program target"));
  compile(m_target, backend);
}

void ProgramNode::compile(const std::string& target, const Pointer<Backend>& backend)
{
  if (!backend) return;
  if (target.empty()) error(Exception("Empty Program target"));
  if (!ctrlGraph || !ctrlGraph->entry()) {
    error(Exception("Impossible to compile an empty Program."));
  } 

  Context::current()->enter(this);
  BackendCodePtr code = 0;
  try {
    collectAll();
    code = backend->generate_code(target, this);
  } catch (...) {
    Context::current()->exit();
    throw;
  }
  Context::current()->exit();
  if (code) {
    m_code[std::make_pair(target, backend)] = code;
    m_backend_name = backend->name();
  }
}

bool ProgramNode::is_compiled() const
{
  if (m_target.empty()) error( Exception( "Invalid Program target" ) );

  return is_compiled(m_target, Backend::get_backend(m_target));
}

bool ProgramNode::is_compiled(const std::string& target) const
{
  if (target.empty()) error( Exception( "Invalid compilation target" ) );

  return is_compiled(target, Backend::get_backend(target));
}

bool ProgramNode::is_compiled(const std::string& target, const Pointer<Backend>& backend) const
{
  // Try for a perfect match first
  if (m_code.find(std::make_pair(target, backend)) != m_code.end()) return true;

  // Look for a derived target
  std::list<std::string> derived_targets = Backend::derived_targets(target);
  for (std::list<std::string>::const_iterator i = derived_targets.begin(); 
       i != derived_targets.end(); i++) {
    if (m_code.find(std::make_pair(*i, backend)) != m_code.end()) return true;
  }

  return false;
}

Pointer<BackendCode> ProgramNode::code()
{
  return code(Backend::get_backend(m_target));
}

Pointer<BackendCode> ProgramNode::code(const Pointer<Backend>& backend)
{
  if (m_target.empty()) error( Exception( "Invalid Program target" ) );

  return code(m_target, backend);
}

Pointer<BackendCode> ProgramNode::code(const std::string& target, const Pointer<Backend>& backend)
{
  if (!backend) return 0;

  if (m_code.find(std::make_pair(target, backend)) == m_code.end()) compile(target, backend);

  return m_code[std::make_pair(target, backend)];
}

std::string ProgramNode::describe_interface() const
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
  os << "Uniforms:" << std::endl;
  os << describe(all_uniforms) << std::endl;
  os << "Arrays:" << std::endl;
  os << describe(textures) << std::endl;

  return os.str();
}

std::string ProgramNode::describe_vars() const
{
  std::ostringstream os;
  os << describe_interface(); 
  os << "Temps:" << std::endl;
  os << describe(temps) << std::endl;
  os << "Constants:" << std::endl;
  os << describe(constants) << std::endl;
  return os.str();
}

std::string ProgramNode::describe_decls() const
{
  std::ostringstream os;
  os << "Temp Declarations:" << std::endl;
  for (VarSet::const_iterator I = tempDecls.begin(); I != tempDecls.end(); ++I) {
    os << (*I)->nameOfType() << " " << (*I)->name() << std::endl;
  }
  return os.str();
}

std::string ProgramNode::describe_bindings()
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

std::string ProgramNode::describe_bindings(const std::string& target)
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
  BackendPtr backend = Backend::get_backend(target);
  code(target, backend)->describe_bindings(os);
  return os.str();
}
std::string ProgramNode::describe_counts() 
{
  std::ostringstream os;
  // input counts
  os << "inputs:" << std::endl << std::endl;
  describe_stats(inputs);

  // output counts
  os << "outputs:" << std::endl << std::endl;
  describe_stats(outputs);
  
  // total instruction count, distribution
  OpStats ops;
  collect_stats(ops);
  os << "ops:" << std::endl;
  int total = 0;
  for(OpStats::iterator I = ops.begin(); I != ops.end(); ++I) {
    os << opInfo[I->first].name << ": " << I->second << std::endl; 
    total += I->second;
  }
  os << "total: " << total;
  return os.str();
}

void ProgramNode::dump(std::string filename) const
{
  SH_DEBUG_PRINT("Dumping " << filename);
  std::string varfile = filename + ".vars";
  std::ofstream varout(varfile.c_str());
  varout << "Program "; 
  varout << describe_vars(); 
  varout << describe_decls();

  std::ostringstream sout;
  ctrlGraph->graphviz_dump(sout);

#if 1
  std::ofstream fout((filename + ".dot").c_str());
  fout << sout.str();
  fout.close();
#else
  dotGen(sout.str(), filename);
#endif
}

void ProgramNode::updateUniform(const VariableNodePtr& uniform)
{
  for (CodeMap::iterator I = m_code.begin(); I != m_code.end(); ++I) {
    I->second->updateUniform(uniform);
  }
}

void ProgramNode::collectVariables()
{
  temps.clear();
  uniforms.clear();
  all_uniforms.clear();
  constants.clear();
  textures.clear();
  palettes.clear();
  if (ctrlGraph->entry()) {
    ctrlGraph->clear_marked();
    collect_node_vars(ctrlGraph->entry());
    ctrlGraph->clear_marked();
  }
}

void ProgramNode::collectDecls()
{
  tempDecls.clear();
  usedTempDecls.clear();
  // @todo range - use collectVariables temps result
  // to pare out unnecessary declarations (i.e. variables
  // that may have disappeared already due to transformations)
  // (may actually want to put this cleaning up step in collectVariables
  // since some temps might be removed during optimizations)
  if (ctrlGraph->entry()) {
    ctrlGraph->clear_marked();
    collect_node_decls(ctrlGraph->entry());
    ctrlGraph->clear_marked();
#if CLEAN_DECLS 
    clean_node_decls(ctrlGraph->entry());
    ctrlGraph->clear_marked();
    tempDecls.clear();
    tempDecls.swap(usedTempDecls);
#endif
  }
}

void ProgramNode::collectAll() {
  collectDecls();
  collectVariables();
}

bool ProgramNode::hasDecl(const VariableNodePtr& node) const
{
  return tempDecls.find(node) != tempDecls.end();
}

void ProgramNode::addDecl(const VariableNodePtr& node, CtrlGraphNode* cfgNode)
{
  tempDecls.insert(node);
  SH_DEBUG_ASSERT(ctrlGraph->entry());
  cfgNode->addDecl(node);
}

void ProgramNode::addDecl(const VariableNodePtr& node)
{
  addDecl(node, ctrlGraph->entry());
}

void ProgramNode::collect_node_decls(CtrlGraphNode* node)
{
  if (node->marked()) return;
  node->mark();
  tempDecls.insert(node->decl_begin(), node->decl_end());

  if (node->block) {
    for (BasicBlock::StmtList::const_iterator I = node->block->begin();
         I != node->block->end(); ++I) {
      collect_var_decl(I->dest.node());
      collect_var_decl(I->src[0].node());
      collect_var_decl(I->src[1].node());
      collect_var_decl(I->src[2].node());
    }
  }

  for (CtrlGraphNode::SuccessorIt J = node->successors_begin();
       J != node->successors_end(); ++J) {
    collect_node_decls(J->node);
  }
  
  if (node->follower()) collect_node_decls(node->follower());
}

void ProgramNode::clean_node_decls(CtrlGraphNode* node)
{
  if (node->marked()) return;
  node->mark();
  CtrlGraphNode::DeclIt I, nextI;
  for(CtrlGraphNode::DeclIt I = node->decl_begin(); I != node->decl_end(); I = nextI) {
    nextI = I;
    ++nextI;
    if(usedTempDecls.find(*I) == usedTempDecls.end()) node->removeDecl(I);
  }
  for (CtrlGraphNode::SuccessorIt J = node->successors_begin(); J != node->successors_end(); ++J) {
    clean_node_decls(J->node);
  }
  if (node->follower()) clean_node_decls(node->follower());
}

void ProgramNode::collect_node_vars(CtrlGraphNode* node)
{
  if (node->marked()) return;
  node->mark();

  if (node->block) {
    for (BasicBlock::StmtList::const_iterator I = node->block->begin();
         I != node->block->end(); ++I) {
      
      collect_var(I->dest.node());
      collect_var(I->src[0].node());
      collect_var(I->src[1].node());
      collect_var(I->src[2].node());
    }
  }

  for (CtrlGraphNode::SuccessorIt J = node->successors_begin();
       J != node->successors_end(); ++J) {
    collect_node_vars(J->node);
  }
  
  if (node->follower()) collect_node_vars(node->follower());
}

void ProgramNode::collect_dependent_uniform(const VariableNodePtr& var)
{
  all_uniforms.push_back(var);

  // Get all of the recursively dependent uniforms
  if (var->evaluator()) {
    for (VarIt I = var->evaluator()->uniforms.begin();
         I != var->evaluator()->uniforms.end(); ++I) {
      if (std::find(all_uniforms.begin(), all_uniforms.end(), *I) == all_uniforms.end()) {
        collect_dependent_uniform(*I);
      }
    }
  }
}

void ProgramNode::collect_var(const VariableNodePtr& var)
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
    // Taken care of by VariableNode constructor
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
                  shref_dynamic_cast<TextureNode>(var)) == textures.end()) {
      textures.push_back(shref_dynamic_cast<TextureNode>(var));
    }    
    break;
  case SH_PALETTE:
    if (std::find(palettes.begin(), palettes.end(),
                  shref_dynamic_cast<PaletteNode>(var)) == palettes.end()) {
      palettes.push_back(shref_dynamic_cast<PaletteNode>(var));
    }
    break;
  default:
    SH_DEBUG_ASSERT(0);
  }
}

void ProgramNode::collect_var_decl(const VariableNodePtr& var)
{
  if (!var) return;
  if (var->uniform()) return; 
  if (var->kind() != SH_TEMP) return;
  if(tempDecls.find(var) != tempDecls.end()) usedTempDecls.insert(var);
}

std::ostream& ProgramNode::print(std::ostream& out,
                                   const ProgramNode::VarList& varList) {
  return print(out, varList.begin(), varList.size());
}

std::ostream& ProgramNode::print(std::ostream& out, const VarIt& begin, size_t size) {
  out << "(";
  size_t i = 0;
  for (VarIt it = begin; i < size; ++it, ++i) {
    if (i > 0) out << ", ";
    out << (*it)->nameOfType() << " " << (*it)->name(); 
  }
  out << ")";
  return out;
}

Pointer<ProgramNode> ProgramNode::clone(bool cloneVariables) const
{
  ProgramNodePtr result = new ProgramNode(target());
  result->InfoHolder::operator=(*this);
  result->Meta::operator=(*this);
  result->ctrlGraph = ctrlGraph->clone();
  result->inputs = inputs;
  result->outputs = outputs;
  result->collectAll();
  if(cloneVariables) { /* replace non-uniform/non-const variables */  
    VariableCloner vc;
    vc.transform(result);

    VariableReplacer vr(vc.vmap);
    result->ctrlGraph->dfs(vr);
    vr(result->inputs);
    vr(result->outputs);
    result->collectAll();
  }
  return result;
}

/// Prints a statement count
int ProgramNode::statement_count() {
  StatementCounter sc;
  sc.transform(this);
  return sc.count;
}

void ProgramNode::finish()
{
  m_finished = true;
  if (m_assigned_var) {
    m_assigned_var->attach(this);
    m_assigned_var = 0;
  }
}

void ProgramNode::collect_stats(OpStats& stats) {
  if (ctrlGraph->entry()) {
    ctrlGraph->clear_marked();
    collect_stats_helper(ctrlGraph->entry(), stats);
    ctrlGraph->clear_marked();
  }
}

void ProgramNode::collect_stats_helper(const CtrlGraphNode* node, OpStats& stats) {
  if(node->marked()) return;
  node->mark();
 
  if(node->block) {
    for(BasicBlock::const_iterator B = node->block->begin(); B != node->block->end(); ++B) {
      stats[B->op]++;
    }
  }

  for (CtrlGraphNode::SuccessorConstIt J = node->successors_begin();
       J != node->successors_end(); ++J) {
    collect_stats_helper(J->node, stats);
  }
  
  if (node->follower()) collect_stats_helper(node->follower(), stats);
}

}
