#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include "ShAlgebra.hpp"
#include "ShCtrlGraph.hpp"
#include "ShDebug.hpp"
#include "ShError.hpp"
#include "ShOptimizer.hpp"
#include "ShInternals.hpp"
#include "ShEnvironment.hpp"
#include "ShManipulator.hpp"
#include "ShFixedManipulator.hpp"

namespace SH {

ShProgram connect(const ShProgram& a, const ShProgram& b)
{
  int aosize, bisize;
  if( !a || !b ) SH_DEBUG_WARN( "Connecting with a null ShProgram" );
  if( !a ) return b;
  if( !b ) return a;
  aosize = a->outputs.size();
  bisize = b->inputs.size();
  std::string rtarget;

  if (a->target().empty()) {
    rtarget = b->target(); // A doesn't have a target. Use b's.
  } else {
    if (b->target().empty() || a->target() == b->target()) {
      rtarget = a->target(); // A has a target, b doesn't
    } else {
      rtarget = ""; // Connecting different targets.
    }
  }

  ShProgram program = new ShProgramNode(rtarget);

  ShCtrlGraphNodePtr heada, taila, headb, tailb;
  
  copyCtrlGraph(a->ctrlGraph->entry(), a->ctrlGraph->exit(), heada, taila);
  copyCtrlGraph(b->ctrlGraph->entry(), b->ctrlGraph->exit(), headb, tailb);

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
  
  ShVariableReplacer::VarMap varMap;

  
  ShEnvironment::shader = program;
  ShEnvironment::insideShader = true;
  
  ShProgramNode::VarList::const_iterator I, J;  

  ShProgramNode::VarList InOutInputs;
  ShProgramNode::VarList InOutOutputs;

  // replace outputs and inputs connected together by temps 
  for (I = a->outputs.begin(), J = b->inputs.begin(); 
      I != a->outputs.end() && J != b->inputs.end(); ++I, ++J) { 
    if((*I)->size() != (*J)->size()) {
      ShError(ShAlgebraException("Cannot smash variables " + 
            (*I)->nameOfType() + " " + (*I)->name() + " and " + 
            (*J)->nameOfType() + " " + (*J)->name() + " with different sizes."));
    }
    ShVariableNodePtr n = new ShVariableNode(SH_TEMP, (*I)->size());
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
    ShVariableNodePtr newInput(new ShVariableNode(SH_INPUT, (*I)->size(), 
          (*I)->specialType()));
    newInput->name((*I)->name());
    std::replace(program->inputs.begin(), program->inputs.end(),
        (*I), newInput);
    program->inputs.pop_back();

    graphEntry->block->addStatement(ShStatement(
        ShVariable(varMap[*I]), SH_OP_ASN, ShVariable(newInput)));
  }

  ShCtrlGraphNodePtr graphExit;
  for (I = InOutOutputs.begin(); I != InOutOutputs.end(); ++I) {
    if(!graphExit) graphExit = program->ctrlGraph->appendExit();
    ShVariableNodePtr newOutput(new ShVariableNode(SH_OUTPUT, (*I)->size(), 
          (*I)->specialType()));
    newOutput->name((*I)->name());
    std::replace(program->outputs.begin(), program->outputs.end(),
        (*I), newOutput);
    program->outputs.pop_back();

    graphExit->block->addStatement(ShStatement(
        ShVariable(newOutput), SH_OP_ASN, ShVariable(varMap[*I])));
  }

  ShEnvironment::shader = 0;
  ShEnvironment::insideShader = false;

  ShVariableReplacer replacer(varMap);
  program->ctrlGraph->dfs(replacer);

  ShOptimizer optimizer(program->ctrlGraph);
  optimizer.optimize(ShEnvironment::optimizationLevel);
  
  program->collectVariables();
  return program;
}

ShProgram combine(const ShProgram& a, const ShProgram& b)
{
  std::string rtarget;
  if( !a || !b ) SH_DEBUG_WARN( "Connecting with a null ShProgram" );
  if(!a) return b;
  if(!b) return a;

  if (a->target().empty()) {
    rtarget = b->target(); // A doesn't have a target. Use b's.
  } else {
    if (b->target().empty() || a->target() == b->target()) {
      rtarget = a->target(); // A has a target, b doesn't
    } else { 
      rtarget = ""; // Connecting different targets.
    }
  }

  ShProgram program = new ShProgramNode(rtarget);

  ShCtrlGraphNodePtr heada, taila, headb, tailb;

  copyCtrlGraph(a->ctrlGraph->entry(), a->ctrlGraph->exit(), heada, taila);
  copyCtrlGraph(b->ctrlGraph->entry(), b->ctrlGraph->exit(), headb, tailb);

  taila->append(headb);

  ShCtrlGraphPtr new_graph = new ShCtrlGraph(heada, tailb);
  program->ctrlGraph = new_graph;

  program->inputs = a->inputs;
  program->inputs.insert(program->inputs.end(), b->inputs.begin(), b->inputs.end());
  program->outputs = a->outputs;
  program->outputs.insert(program->outputs.end(), b->outputs.begin(), b->outputs.end());

  ShOptimizer optimizer(program->ctrlGraph);
  optimizer.optimize(ShEnvironment::optimizationLevel); 
 
  program->collectVariables();
  
  return program;
}

ShProgram namedCombine(const ShProgram &a, const ShProgram &b) {
  ShProgram ab = combine(a, b);

  typedef std::pair<std::string, int> InputType;
  typedef std::map< InputType, int > FirstOccurenceMap;  // position of first occurence of an input type
  typedef std::vector< std::vector<int> > Duplicates;
  FirstOccurenceMap firsts;
  // dups[i] stores the set of positions that have matching input types with position i.
  // The whole set is stored in the smallest i position.
  Duplicates dups( ab->inputs.size(), std::vector<int>()); 

  int i = 0;
  for(ShProgramNode::VarList::const_iterator I = ab->inputs.begin();
      I != ab->inputs.end(); ++I, ++i) {
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
    for(int j = 0; j < dups[i].size(); ++j) swizzle.push_back(dups[i][j]);
    if( duplicator ) duplicator = duplicator & shDup(dups[i].size());
    else duplicator = shDup(dups[i].size());
  }
  return ab << shSwizzle(swizzle) << duplicator; 
}

ShProgram namedConnect(const ShProgram &a, const ShProgram &b, bool keepExtra) {
  // positions of a pair of matched a output and b input 
  typedef std::map<int, int> MatchedChannelMap; 

  std::vector<bool> aMatch(a->outputs.size(), false);
  std::vector<bool> bMatch(b->inputs.size(), false);
  MatchedChannelMap mcm;
  int i, j;
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
        ShVariable var(new ShVariableNode(SH_INOUT, (*J)->size(), 
              (*J)->specialType()));
        var.name((*J)->name());
      } SH_END;
      passer = passer & passOne; 
      swiz[j] = newInputIdx++;
    }
  }
  ShProgram aPass = combine(a, passer);

  if( keepExtra ) {
    for(i = 0; i < aMatch.size(); ++i) {
      if( !aMatch[i] ) swiz.push_back(i);
    }
  }
   
  return b << ( shSwizzle(swiz) << aPass ); 
}

ShProgram operator<<(const ShProgram& a, const ShProgram& b)
{
  return connect(b,a);
}

ShProgram operator>>(const ShProgram& a, const ShProgram& b)
{
  return connect(a,b);
}

ShProgram operator&(const ShProgram& a, const ShProgram& b)
{
  return combine(a, b);
}

ShProgram operator>>(const ShProgram &p, const ShVariable &var) { 
  return replaceUniform(p, var);
}

ShProgram replaceUniform(const ShProgram& a, const ShVariable& v)
{
  if(!v.uniform()) {
    ShError(ShAlgebraException("Cannot replace non-uniform variable"));
  }

  ShProgram program = cloneProgram(a); 
  
  ShVariableReplacer::VarMap varMap;

  ShEnvironment::shader = program;
  ShEnvironment::insideShader = true;
  // make a new input
  ShVariableNodePtr newInput = new ShVariableNode(SH_INPUT, v.size(), v.node()->specialType()); 
  varMap[v.node()] = newInput;

  ShEnvironment::shader = 0;
  ShEnvironment::insideShader = false;

  ShVariableReplacer replacer(varMap);
  program->ctrlGraph->dfs(replacer);

  ShOptimizer optimizer(program->ctrlGraph);
  optimizer.optimize(ShEnvironment::optimizationLevel);
  
  program->collectVariables();

  return program;
}

}
