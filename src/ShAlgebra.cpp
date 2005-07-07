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
#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include "ShAlgebra.hpp"
#include "ShCtrlGraph.hpp"
#include "ShDebug.hpp"
#include "ShError.hpp"
#include "ShOptimizations.hpp"
#include "ShInternals.hpp"
#include "ShContext.hpp"
#include "ShManipulator.hpp"
#include "ShFixedManipulator.hpp"

namespace SH {

ShProgram connect(ShProgram pa, ShProgram pb)
{
  ShProgramNodePtr a = pa.node();
  ShProgramNodePtr b = pb.node();
  
  if( !a || !b ) SH_DEBUG_WARN( "Connecting with a null ShProgram" );
  if( !a ) return b;
  if( !b ) return a;
  
  int aosize = a->outputs.size();
  int bisize = b->inputs.size();
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

  ShProgramNodePtr program = new ShProgramNode(rtarget);

  ShCtrlGraphNodePtr heada, taila, headb, tailb;

  a->ctrlGraph->copy(heada, taila);
  b->ctrlGraph->copy(headb, tailb);

  taila->append(headb);

  ShCtrlGraphPtr new_graph = new ShCtrlGraph(heada, tailb);
  program->ctrlGraph = new_graph;

  program->inputs = a->inputs;

// push back extra inputs from b if aosize < bisize
  if(aosize < bisize) {
    ShProgramNode::VarList::const_iterator II = b->inputs.begin();
    for(int i = 0; i < aosize; ++i, ++II); 
    for(; II != b->inputs.end(); ++II) {
      program->inputs.push_back(*II);
    }
  }
  program->outputs = b->outputs;

  // push back extra outputs from a if aosize > bisize
  if(aosize > bisize) { 
    ShProgramNode::VarList::const_iterator II = a->outputs.begin();
    for(int i = 0; i < bisize; ++i, ++II); 
    for(; II != a->outputs.end(); ++II) {
      program->outputs.push_back(*II);
    }
  }
  
  ShVarMap varMap;

  ShContext::current()->enter(program);
  
  ShProgramNode::VarList::const_iterator I, J;  

  ShProgramNode::VarList InOutInputs;
  ShProgramNode::VarList InOutOutputs;

  // replace outputs and inputs connected together by temps 
  for (I = a->outputs.begin(), J = b->inputs.begin(); 
      I != a->outputs.end() && J != b->inputs.end(); ++I, ++J) { 
    if((*I)->size() != (*J)->size()) {
      std::ostringstream err;
      err << "Cannot smash variables "  
          << (*I)->nameOfType() << " " << (*I)->name() << " and " 
          << (*J)->nameOfType() << " " << (*J)->name() << " with different sizes" << std::endl;
      err << "while connecting outputs: ";
      ShProgramNode::print(err, a->outputs) << std::endl;
      err << "to inputs: ";
      ShProgramNode::print(err, b->inputs) << std::endl;
      ShContext::current()->exit();
      shError(ShAlgebraException(err.str()));
      return ShProgram(ShProgramNodePtr(0));
    }
    ShVariableNodePtr n = (*I)->clone(SH_TEMP);
    varMap[*I] = n;
    varMap[*J] = n;

    if((*I)->kind() == SH_INOUT) InOutInputs.push_back((*I)); 
    if((*J)->kind() == SH_INOUT) InOutOutputs.push_back((*J)); 
  }

  // Change connected InOut variables to either Input or Output only
  // (since they have been connected and turned into temps internally)
  ShCtrlGraphNodePtr graphEntry;
  for (I = InOutInputs.begin(); I != InOutInputs.end(); ++I) {
    if(!graphEntry) graphEntry = program->ctrlGraph->prependEntry();
    ShVariableNodePtr newInput((*I)->clone(SH_INPUT)); 

    std::replace(program->inputs.begin(), program->inputs.end(),
        (*I), newInput);
    program->inputs.pop_back();

    graphEntry->block->addStatement(ShStatement(
        ShVariable(varMap[*I]), SH_OP_ASN, ShVariable(newInput)));
  }

  ShCtrlGraphNodePtr graphExit;
  for (I = InOutOutputs.begin(); I != InOutOutputs.end(); ++I) {
    if(!graphExit) graphExit = program->ctrlGraph->appendExit();
    ShVariableNodePtr newOutput((*I)->clone(SH_OUTPUT));
    
    std::replace(program->outputs.begin(), program->outputs.end(),
        (*I), newOutput);
    program->outputs.pop_back();

    graphExit->block->addStatement(ShStatement(
        ShVariable(newOutput), SH_OP_ASN, ShVariable(varMap[*I])));
  }

  ShContext::current()->exit();

  ShVariableReplacer replacer(varMap);
  program->ctrlGraph->dfs(replacer);

  optimize(program);
  return program;
}

ShProgram combine(ShProgram pa, ShProgram pb)
{
  ShProgramNodePtr a = pa.node();
  ShProgramNodePtr b = pb.node();
  
  std::string rtarget;
  if( !a || !b ) SH_DEBUG_WARN( "Connecting with a null ShProgram" );
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

  ShProgramNodePtr program = new ShProgramNode(rtarget);

  ShCtrlGraphNodePtr heada, taila, headb, tailb;

  a->ctrlGraph->copy(heada, taila);
  b->ctrlGraph->copy(headb, tailb);

  taila->append(headb);

  ShCtrlGraphPtr new_graph = new ShCtrlGraph(heada, tailb);
  program->ctrlGraph = new_graph;

  program->inputs = a->inputs;
  program->inputs.insert(program->inputs.end(), b->inputs.begin(), b->inputs.end());
  program->outputs = a->outputs;
  program->outputs.insert(program->outputs.end(), b->outputs.begin(), b->outputs.end());

  optimize(program);
  return program;
}

// Duplicates to inputs with matching name/type
ShProgram mergeNames(ShProgram p)
{
  typedef std::pair<std::string, int> InputType;
  typedef std::map< InputType, int > FirstOccurenceMap;  // position of first occurence of an input type
  typedef std::vector< std::vector<int> > Duplicates;
  FirstOccurenceMap firsts;
  // dups[i] stores the set of positions that have matching input types with position i.
  // The whole set is stored in the smallest i position.
  Duplicates dups( p.node()->inputs.size(), std::vector<int>()); 

  std::size_t i = 0;
  for(ShProgramNode::VarList::const_iterator I = p.node()->inputs.begin();
      I != p.node()->inputs.end(); ++I, ++i) {
    InputType it( (*I)->name(), (*I)->size() );
    if( firsts.find( it ) != firsts.end() ) { // duplicate
      dups[ firsts[it] ].push_back(i); 
    } else {
      firsts[it] = i;
      dups[i].push_back(i);
    }
  }
  std::vector<int> swizzle;
  ShFixedManipulator duplicator;
  for(i = 0; i < dups.size(); ++i) {
    if( dups[i].empty() ) continue;
    for(std::size_t j = 0; j < dups[i].size(); ++j) swizzle.push_back(dups[i][j]);
    if( duplicator ) duplicator = duplicator & shDup(dups[i].size());
    else duplicator = shDup(dups[i].size());
  }
  ShProgram result = p << shSwizzle(swizzle);
  if( duplicator ) result = result << duplicator;
  return result.node(); 
}

ShProgram namedCombine(ShProgram a, ShProgram b) {
  return mergeNames(combine(a, b));
}

ShProgram namedConnect(ShProgram pa, ShProgram pb, bool keepExtra)
{
  ShProgramNodeCPtr a = pa.node();
  ShProgramNodeCPtr b = pb.node();
  // positions of a pair of matched a output and b input 
  typedef std::map<int, int> MatchedChannelMap; 

  std::vector<bool> aMatch(a->outputs.size(), false);
  std::vector<bool> bMatch(b->inputs.size(), false);
  MatchedChannelMap mcm;
  std::size_t i, j;
  ShProgramNode::VarList::const_iterator I, J;

  i = 0;
  for(I = a->outputs.begin(); I != a->outputs.end(); ++I, ++i) {
    j = 0;
    for(J = b->inputs.begin(); J != b->inputs.end(); ++J, ++j) {
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

  std::vector<int> swiz(b->inputs.size(), 0); 
  for(MatchedChannelMap::iterator mcmit = mcm.begin(); mcmit != mcm.end(); ++mcmit) {
    swiz[mcmit->second] = mcmit->first;
  }

  // swizzle unmatched inputs and make a pass them through properly
  ShProgram passer = SH_BEGIN_PROGRAM() {} SH_END;
  int newInputIdx = a->outputs.size(); // index of next new input added to a
  for(j = 0, J= b->inputs.begin(); J != b->inputs.end(); ++J, ++j) {
    if( !bMatch[j] ) {
      ShProgram passOne = SH_BEGIN_PROGRAM() {
        ShVariable var((*J)->clone(SH_INOUT));
      } SH_END;
      passer = passer & passOne; 
      swiz[j] = newInputIdx++;
    }
  }
  ShProgram aPass = combine(pa, passer);

  if( keepExtra ) {
    for(i = 0; i < aMatch.size(); ++i) {
      if( !aMatch[i] ) swiz.push_back(i);
    }
  }
   
  return mergeNames(pb << ( shSwizzle(swiz) << aPass )); 
}

ShProgram renameInput(ShProgram a,
                      const std::string& oldName, const std::string& newName) {
  ShProgram renamer = SH_BEGIN_PROGRAM() {
    for(ShProgramNode::VarList::const_iterator I = a.node()->inputs.begin();
        I != a.node()->inputs.end(); ++I) {
      ShVariable var((*I)->clone(SH_INOUT));

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
ShProgram renameOutput(ShProgram a,
                       const std::string& oldName, const std::string& newName) {
  ShProgram renamer = SH_BEGIN_PROGRAM() {
    for(ShProgramNode::VarList::const_iterator I = a.node()->outputs.begin();
        I != a.node()->outputs.end(); ++I) {
      ShVariable var((*I)->clone(SH_INOUT));

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

ShProgram namedAlign(ShProgram a, ShProgram b) {
  ShManipulator<std::string> ordering;

  for(ShProgramNode::VarList::const_iterator I = b.node()->inputs.begin();
      I != b.node()->inputs.end(); ++I) {
    ordering((*I)->name());
  }

  return ordering << a; 
}

ShProgram operator<<(ShProgram a, ShProgram b)
{
  return connect(b,a);
}

ShProgram operator>>(ShProgram a, ShProgram b)
{
  return connect(a,b);
}

ShProgram operator&(ShProgram a, ShProgram b)
{
  return combine(a, b);
}

ShProgram operator>>(ShProgram p, const ShVariable &var) { 
  return replaceVariable(p, var);
}

ShProgram replaceVariable(ShProgram a, const ShVariable& v)
{
  ShProgram program(a.node()->clone()); 
  
  ShVarMap varMap;

  ShContext::current()->enter(program.node());

  // make a new input
  ShVariableNodePtr newInput(v.node()->clone(SH_INPUT)); 
  varMap[v.node()] = newInput;

  ShContext::current()->exit();

  ShVariableReplacer replacer(varMap);
  program.node()->ctrlGraph->dfs(replacer);

  optimize(program);
  return program;
}

ShProgram operator<<(ShProgram a, const ShVariable& var) {
  ShProgram vNibble = SH_BEGIN_PROGRAM() {
    ShVariable out(var.node()->clone(SH_OUTPUT, var.size(), var.valueType(), SH_SEMANTICTYPE_END, true, false));
    shASN(out, var);
  } SH_END_PROGRAM;
  return connect(vNibble, a); 
}

}
