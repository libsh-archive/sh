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
#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include "Algebra.hpp"
#include "CtrlGraph.hpp"
#include "Debug.hpp"
#include "Error.hpp"
#include "Optimizations.hpp"
#include "Internals.hpp"
#include "Context.hpp"
#include "Manipulator.hpp"
#include "FixedManipulator.hpp"

namespace SH {

Program connect(Program pa, Program pb)
{
  ProgramNodePtr a = pa.node();
  ProgramNodePtr b = pb.node();

  if( !a || !b ) SH_DEBUG_WARN( "Connecting with a null Program" );
  if( !a ) return b;
  if( !b ) return a;
  
  int aosize = a->outputs.size();
  int bisize = b->inputs.size() - pb.binding_spec.size();
  std::string rtarget;

  if (a->target().empty()) {
    rtarget = b->target(); // A doesn't have a target. Use b's.
  } else {
    if (b->target().empty() || a->target() == b->target()) {
      rtarget = a->target(); // A has a target, b doesn't
    } else {
      SH_DEBUG_WARN("Connecting two different targets. Using empty target for result.");
      rtarget = ""; // Connecting different targets.
    }
  }

  ProgramNodePtr program = new ProgramNode(rtarget);

  CtrlGraphNodePtr heada, taila, headb, tailb;

  a->ctrlGraph->copy(heada, taila);
  b->ctrlGraph->copy(headb, tailb);

  taila->append(headb);

  CtrlGraphPtr new_graph = new CtrlGraph(heada, tailb);
  program->ctrlGraph = new_graph;

  ProgramNode::VarList::iterator a_free_inputs = a->inputs.begin();
  std::advance(a_free_inputs, pa.binding_spec.size());
  ProgramNode::VarList::iterator b_free_inputs = b->inputs.begin();
  std::advance(b_free_inputs, pb.binding_spec.size());

  std::copy(a->inputs.begin(), a_free_inputs, std::back_inserter(program->inputs));
  std::copy(b->inputs.begin(), b_free_inputs, std::back_inserter(program->inputs));
  std::copy(a_free_inputs, a->inputs.end(), std::back_inserter(program->inputs));

  // push back extra inputs from b if aosize < bisize
  if(aosize < bisize) {
    ProgramNode::VarList::iterator I = b_free_inputs;
    std::advance(I, aosize);
    std::copy(I, b->inputs.end(), std::back_inserter(program->inputs));
  }

  program->outputs = b->outputs;

  // push back extra outputs from a if aosize > bisize
  if(aosize > bisize) { 
    ProgramNode::VarList::iterator I = a->outputs.begin();
    std::advance(I, bisize);
    std::copy(I, a->outputs.end(), std::back_inserter(program->outputs));
  }
  
  VarMap varMap;

  Context::current()->enter(program);
  
  ProgramNode::VarList::const_iterator I, J;  

  ProgramNode::VarList InOutInputs;
  ProgramNode::VarList InOutOutputs;

  // replace outputs and inputs connected together by temps 
  for (I = a->outputs.begin(), J = b_free_inputs; 
      I != a->outputs.end() && J != b->inputs.end(); ++I, ++J) { 
    if((*I)->size() != (*J)->size()) {
      std::ostringstream err;
      err << "Cannot smash variables "  
          << (*I)->nameOfType() << " " << (*I)->name() << " and " 
          << (*J)->nameOfType() << " " << (*J)->name() << " with different sizes" << std::endl;
      err << "while connecting outputs: ";
      ProgramNode::print(err, a->outputs) << std::endl;
      err << "to inputs: ";
      ProgramNode::print(err, b->inputs) << std::endl;
      Context::current()->exit();
      error(AlgebraException(err.str()));
      return Program(ProgramNodePtr(0));
    }
    VariableNodePtr n = (*I)->clone(SH_TEMP);
    varMap[*I] = n;
    varMap[*J] = n;

    if((*I)->kind() == SH_INOUT) InOutInputs.push_back((*I)); 
    if((*J)->kind() == SH_INOUT) InOutOutputs.push_back((*J)); 
  }

  // Change connected InOut variables to either Input or Output only
  // (since they have been connected and turned into temps internally)
  CtrlGraphNodePtr graphEntry;
  for (I = InOutInputs.begin(); I != InOutInputs.end(); ++I) {
    if(!graphEntry) graphEntry = program->ctrlGraph->prependEntry();
    VariableNodePtr newInput((*I)->clone(SH_INPUT)); 

    std::replace(program->inputs.begin(), program->inputs.end(),
        (*I), newInput);
    program->inputs.pop_back();

    graphEntry->block->addStatement(Statement(
        Variable(varMap[*I]), OP_ASN, Variable(newInput)));
  }

  CtrlGraphNodePtr graphExit;
  for (I = InOutOutputs.begin(); I != InOutOutputs.end(); ++I) {
    if(!graphExit) graphExit = program->ctrlGraph->appendExit();
    VariableNodePtr newOutput((*I)->clone(SH_OUTPUT));
    
    std::replace(program->outputs.begin(), program->outputs.end(),
        (*I), newOutput);
    program->outputs.pop_back();

    graphExit->block->addStatement(Statement(
        Variable(newOutput), OP_ASN, Variable(varMap[*I])));
  }

  Context::current()->exit();

  VariableReplacer replacer(varMap);
  program->ctrlGraph->dfs(replacer);

  program->collectVariables();

  optimize(program);
  
  Program prg(program);  
  prg.binding_spec = pa.binding_spec;
  std::copy(pb.binding_spec.begin(), pb.binding_spec.end(),
            std::back_inserter(prg.binding_spec));
  prg.stream_inputs = pa.stream_inputs & pb.stream_inputs;
  prg.uniform_inputs.append(pa.uniform_inputs);
  prg.uniform_inputs.append(pb.uniform_inputs);

  return prg;
}

Program combine(Program pa, Program pb)
{
  ProgramNodePtr a = pa.node();
  ProgramNodePtr b = pb.node();
  
  std::string rtarget;
  if( !a || !b ) SH_DEBUG_WARN( "Connecting with a null Program" );
  if (!a) return b;
  if (!b) return a;

  if (a->target().empty()) {
    rtarget = b->target(); // A doesn't have a target. Use b's.
  } else {
    if (b->target().empty() || a->target() == b->target()) {
      rtarget = a->target(); // A has a target, b doesn't
    } else { 
      rtarget = ""; // Connecting different targets.
    }
  }

  ProgramNodePtr program = new ProgramNode(rtarget);

  CtrlGraphNodePtr heada, taila, headb, tailb;

  a->ctrlGraph->copy(heada, taila);
  b->ctrlGraph->copy(headb, tailb);

  taila->append(headb);

  CtrlGraphPtr new_graph = new CtrlGraph(heada, tailb);
  program->ctrlGraph = new_graph;

  ProgramNode::VarList::iterator a_free_inputs = a->inputs.begin();
  std::advance(a_free_inputs, pa.binding_spec.size());
  ProgramNode::VarList::iterator b_free_inputs = b->inputs.begin();
  std::advance(b_free_inputs, pb.binding_spec.size());
  std::copy(a->inputs.begin(), a_free_inputs, std::back_inserter(program->inputs));
  std::copy(b->inputs.begin(), b_free_inputs, std::back_inserter(program->inputs));
  std::copy(a_free_inputs, a->inputs.end(), std::back_inserter(program->inputs));
  std::copy(b_free_inputs, b->inputs.end(), std::back_inserter(program->inputs));

  program->outputs = a->outputs;
  program->outputs.insert(program->outputs.end(), b->outputs.begin(), b->outputs.end());

  program->collectVariables();
  optimize(program);

  Program prg(program);  
  prg.binding_spec = pa.binding_spec;
  std::copy(pb.binding_spec.begin(), pb.binding_spec.end(),
            std::back_inserter(prg.binding_spec));
  prg.stream_inputs = pa.stream_inputs & pb.stream_inputs;
  prg.uniform_inputs.append(pa.uniform_inputs);
  prg.uniform_inputs.append(pb.uniform_inputs);

  return prg;
}

// Duplicates to inputs with matching name/type
Program mergeNames(Program p)
{
  typedef std::pair<std::string, int> InputType;
  typedef std::map< InputType, int > FirstOccurenceMap;  // position of first occurence of an input type
  typedef std::vector< std::vector<int> > Duplicates;
  FirstOccurenceMap firsts;
  // dups[i] stores the set of positions that have matching input types with position i.
  // The whole set is stored in the smallest i position.
  Duplicates dups(p.node()->inputs.size() - p.binding_spec.size(), std::vector<int>()); 

  std::size_t i = 0;
  ProgramNode::VarList::const_iterator I = p.node()->inputs.begin();
  std::advance(I, p.binding_spec.size());
  for (; I != p.node()->inputs.end(); ++I, ++i) {
    InputType it( (*I)->name(), (*I)->size() );
    if( firsts.find( it ) != firsts.end() ) { // duplicate
      dups[ firsts[it] ].push_back(i); 
    } else {
      firsts[it] = i;
      dups[i].push_back(i);
    }
  }
  std::vector<int> swiz;
  FixedManipulator duplicator;
  for(i = 0; i < dups.size(); ++i) {
    if( dups[i].empty() ) continue;
    for(std::size_t j = 0; j < dups[i].size(); ++j) swiz.push_back(dups[i][j]);
    if( duplicator ) duplicator = duplicator & dup(dups[i].size());
    else duplicator = dup(dups[i].size());
  }
  Program result = p << swizzle(swiz);
  if( duplicator ) result = result << duplicator;
  return result.node(); 
}

Program namedCombine(Program a, Program b)
{
  return mergeNames(combine(a, b));
}

Program namedConnect(Program pa, Program pb, bool keepExtra)
{
  ProgramNodeCPtr a = pa.node();
  ProgramNodeCPtr b = pb.node();
  // positions of a pair of matched a output and b input 
  typedef std::map<int, int> MatchedChannelMap; 

  std::vector<bool> aMatch(a->outputs.size(), false);
  std::vector<bool> bMatch(b->inputs.size() - pb.binding_spec.size(), false);
  MatchedChannelMap mcm;
  std::size_t i, j;
  ProgramNode::VarList::const_iterator I, J;
  ProgramNode::VarList::const_iterator b_free_inputs = b->inputs.begin();
  std::advance(b_free_inputs, pb.binding_spec.size());

  i = 0;
  for(I = a->outputs.begin(); I != a->outputs.end(); ++I, ++i) {
    j = 0;
    for(J = b_free_inputs; J != b->inputs.end(); ++J, ++j) {
      if(bMatch[j]) continue;
      if((*I)->name() != (*J)->name()) continue;
      if((*I)->size() != (*J)->size()) {
        SH_DEBUG_WARN("Named connect matched channel name " << (*I)->name() 
            << " but output size " << (*I)->size() << " != " << " input size " << (*J)->size() );
        continue;
      }
      mcm[i] = j;
      aMatch[i] = true;
      bMatch[j] = true;
    }
  }

  std::vector<int> swiz(b->inputs.size() - pb.binding_spec.size(), 0); 
  for(MatchedChannelMap::iterator mcmit = mcm.begin(); mcmit != mcm.end(); ++mcmit) {
    swiz[mcmit->second] = mcmit->first;
  }

  // swizzle unmatched inputs and make a pass them through properly
  Program passer = SH_BEGIN_PROGRAM() {} SH_END;
  int newInputIdx = a->outputs.size(); // index of next new input added to a
  for(j = 0, J = b_free_inputs; J != b->inputs.end(); ++J, ++j) {
    if( !bMatch[j] ) {
      Program passOne = SH_BEGIN_PROGRAM() {
        Variable var((*J)->clone(SH_INOUT));
      } SH_END;
      passer = passer & passOne; 
      swiz[j] = newInputIdx++;
    }
  }
  Program aPass = combine(pa, passer);

  if( keepExtra ) {
    for(i = 0; i < aMatch.size(); ++i) {
      if( !aMatch[i] ) swiz.push_back(i);
    }
  }
   
  return mergeNames(pb << ( swizzle(swiz) << aPass )); 
}

Program renameInput(Program a, const std::string& oldName,
                    const std::string& newName)
{
  Program renamer = SH_BEGIN_PROGRAM() {
    ProgramNode::VarList::const_iterator I = a.node()->inputs.begin();
    std::advance(I, a.binding_spec.size());
    for (; I != a.node()->inputs.end(); ++I) {
      Variable var((*I)->clone(SH_INOUT));

      if (!(*I)->has_name()) continue;
      std::string name = (*I)->name();
      if( name == oldName ) {
        var.name(newName);
      } else {
        var.name(name);
      }
    }
  } SH_END;
  return connect(renamer, a);
}

// TODO factor out common code from renameInput, renameOutput
Program renameOutput(Program a, const std::string& oldName, 
                     const std::string& newName)
{
  Program renamer = SH_BEGIN_PROGRAM() {
    for(ProgramNode::VarList::const_iterator I = a.node()->outputs.begin();
        I != a.node()->outputs.end(); ++I) {
      Variable var((*I)->clone(SH_INOUT));

      if (!(*I)->has_name()) continue;
      std::string name = (*I)->name();
      if( name == oldName ) {
        var.name(newName);
      } else {
        var.name(name);
      }
    }
  } SH_END;
  return connect(a, renamer);
}

Program namedAlign(Program a, Program b)
{
  Manipulator<std::string> ordering;

  ProgramNode::VarList::const_iterator I = b.node()->inputs.begin();
  std::advance(I, b.binding_spec.size());
  for (; I != b.node()->inputs.end(); ++I) {
    ordering((*I)->name());
  }

  return ordering << a; 
}

Program operator<<(Program a, Program b)
{
  return connect(b,a);
}

Program operator>>(Program a, Program b)
{
  return connect(a,b);
}

Program operator&(Program a, Program b)
{
  return combine(a, b);
}

Program operator>>(Program p, const Variable &var) { 
  return replaceVariable(p, var);
}

Program replaceVariable(Program a, const Variable& v)
{
  Program program(a.node()->clone()); 
  
  VarMap varMap;

  Context::current()->enter(program.node());

  // make a new input
  VariableNodePtr newInput(v.node()->clone(SH_INPUT)); 
  varMap[v.node()] = newInput;

  Context::current()->exit();

  VariableReplacer replacer(varMap);
  program.node()->ctrlGraph->dfs(replacer);

  optimize(program);
  return program;
}

Program operator<<(Program a, const Variable& var)
{
  Program result = a;
  result.binding_spec.push_back(Program::UNIFORM);
  result.uniform_inputs.append(var);
  return result;
}

}
