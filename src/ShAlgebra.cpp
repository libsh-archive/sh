#include <map>
#include <sstream>
#include "ShAlgebra.hpp"
#include "ShCtrlGraph.hpp"
#include "ShDebug.hpp"
#include "ShError.hpp"
#include "ShOptimizer.hpp"
#include "ShInternals.hpp"
#include "ShEnvironment.hpp"

namespace {

using namespace SH;

typedef std::map<ShCtrlGraphNodePtr, ShCtrlGraphNodePtr> CtrlGraphCopyMap;

struct CtrlGraphCopier {
  CtrlGraphCopier(CtrlGraphCopyMap& copyMap)
    : copyMap(copyMap)
  {
  }
  
  void operator()(ShCtrlGraphNodePtr node) {
    if (!node) return;
    ShCtrlGraphNodePtr newNode = new ShCtrlGraphNode(*node);
    copyMap[node] = newNode;
  }

  CtrlGraphCopyMap& copyMap;
};

void copyCtrlGraph(ShCtrlGraphNodePtr head, ShCtrlGraphNodePtr tail,
                   ShCtrlGraphNodePtr& newHead, ShCtrlGraphNodePtr& newTail)
{
  CtrlGraphCopyMap copyMap;
  copyMap[0] = 0;
  
  CtrlGraphCopier copier(copyMap);
  SH_DEBUG_ASSERT( tail ); // catch empty tails
  head->clearMarked();
  head->dfs(copier);

  // Replace the successors and followers in the new graph with their new equivalents
  for (CtrlGraphCopyMap::iterator I = copyMap.begin(); I != copyMap.end(); ++I) {
    ShCtrlGraphNodePtr node = I->second; // Get the new node
    if (!node) continue;
    for (std::vector<ShCtrlGraphBranch>::iterator J = node->successors.begin(); J != node->successors.end(); ++J) {
      J->node = copyMap[J->node];
    }
    node->follower = copyMap[node->follower];
    if (node->block) {
      ShBasicBlockPtr new_block = new ShBasicBlock(*node->block);
      node->block = new_block;
    }
  }
  newHead = copyMap[head];
  newTail = copyMap[tail];

  head->clearMarked();
}

}

namespace SH {

ShProgram connect(const ShProgram& a, const ShProgram& b)
{
  int aosize, bisize;
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

  for (ShProgramNode::VarList::const_iterator II = a->inputs.begin(); II != a->inputs.end(); ++II) {
    program->inputs.push_back(*II);
  }

// push back extra inputs from b if aosize < bisize
  if( aosize < bisize ) {
    ShProgramNode::VarList::const_iterator II = b->inputs.begin();
    for(int i = 0; i < aosize; ++i, ++II); 
    for(; II != b->inputs.end(); ++II) {
      program->inputs.push_back(*II);
    }
  }

  for (ShProgramNode::VarList::const_iterator II = b->outputs.begin(); II != b->outputs.end(); ++II) {
    program->outputs.push_back(*II);
  }

  // push back extra outputs from a if aosize > bisize
  if( aosize > bisize ) { 
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
  for (I = a->outputs.begin(), J = b->inputs.begin(); I != a->outputs.end() && J != b->inputs.end(); ++I, ++J) {
    if( (*I)->size() != (*J)->size() ) {
      ShError( ShAlgebraException( "Cannot smash variables " + 
            (*I)->nameOfType() + " " + (*I)->name() + " and " + 
            (*J)->nameOfType() + " " + (*J)->name() + " with different sizes." ) );
    }
    ShVariableNodePtr n = new ShVariableNode(SH_VAR_TEMP, (*I)->size());
    varMap[*I] = n;
    varMap[*J] = n;
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

  if (a->target().empty()) {
    rtarget = b->target(); // A doesn't have a target. Use b's.
  } else {
    if (b->target().empty() || a->target() == b->target() ) {
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

  for (ShProgramNode::VarList::const_iterator I = a->inputs.begin(); I != a->inputs.end(); ++I) {
    program->inputs.push_back(*I);
  }
  for (ShProgramNode::VarList::const_iterator I = a->outputs.begin(); I != a->outputs.end(); ++I) {
    program->outputs.push_back(*I);
  }
  for (ShProgramNode::VarList::const_iterator I = b->inputs.begin(); I != b->inputs.end(); ++I) {
    program->inputs.push_back(*I);
  }
  for (ShProgramNode::VarList::const_iterator I = b->outputs.begin(); I != b->outputs.end(); ++I) {
    program->outputs.push_back(*I);
  }

  ShOptimizer optimizer(program->ctrlGraph);
  optimizer.optimize(ShEnvironment::optimizationLevel); 
 
  program->collectVariables();
  
  return program;
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

ShProgram operator<<(const ShVariable &var, const ShProgram &p) { 
  return replaceUniform(p, var);
}

ShProgram replaceUniform(const ShProgram& a, const ShVariable& v)
{
  if(!v.uniform()) {
    ShError( ShAlgebraException( "Cannot replace non-uniform variable" ) );
  }

  ShProgram program = new ShProgramNode(a->target());

  ShCtrlGraphNodePtr heada, taila;
  
  copyCtrlGraph(a->ctrlGraph->entry(), a->ctrlGraph->exit(), heada, taila);

  ShCtrlGraphPtr new_graph = new ShCtrlGraph(heada, taila);
  program->ctrlGraph = new_graph;

  for (ShProgramNode::VarList::const_iterator II = a->inputs.begin(); II != a->inputs.end(); ++II) {
    program->inputs.push_back(*II);
  }
  for (ShProgramNode::VarList::const_iterator II = a->outputs.begin(); II != a->outputs.end(); ++II) {
    program->outputs.push_back(*II);
  }
  
  ShVariableReplacer::VarMap varMap;

  ShEnvironment::shader = program;
  ShEnvironment::insideShader = true;
  // make a new input
  ShVariableNodePtr newInput = new ShVariableNode(SH_VAR_INPUT, v.size(), v.node()->specialType()); 
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
