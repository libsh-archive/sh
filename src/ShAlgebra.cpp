#include "ShAlgebra.hpp"
#include <map>
#include "ShCtrlGraph.hpp"
#include "ShDebug.hpp"
#include "ShOptimizer.hpp"
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
  head->dfs(copier);

  for (CtrlGraphCopyMap::iterator I = copyMap.begin(); I != copyMap.end(); ++I) {
    ShCtrlGraphNodePtr node = I->first;
    if (!node) continue;
    for (std::vector<ShCtrlGraphBranch>::iterator J = node->successors.begin(); J != node->successors.end(); ++J) {
      J->node = copyMap[J->node];
    }
    node->follower = copyMap[node->follower];
  }
  newHead = copyMap[head];
  newTail = copyMap[tail];
}

typedef std::map<ShVariableNodePtr, ShVariableNodePtr> VarMap;

struct VariableReplacer {
  VariableReplacer(VarMap& v)
    : varMap(v)
  {
  }

  void operator()(ShCtrlGraphNodePtr node)
  {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      repVar(I->dest);
      for (int i = 0; i < 3; i++) {
        repVar(I->src[i]);
      }
    }
  }

  void repVar(ShVariable& var)
  {
    VarMap::iterator I = varMap.find(var.node());
    if (I == varMap.end()) return;
    SH_DEBUG_PRINT("Replacing " << var.node()->name() << " with " << I->second->name());
    var.node() = I->second;
  }

  VarMap& varMap;
};

}

namespace SH {

ShProgram connect(const ShProgram& a, const ShProgram& b)
{
  SH_DEBUG_ASSERT(a->outputs.size() == b->inputs.size());

  int rkind;

  if (a->kind() < 0) {
    rkind = b->kind(); // A doesn't have a target. Use b's.
  } else {
    if (b->kind() < 0) {
      rkind = a->kind(); // A has a target, b doesn't
    } else {
      rkind = -1; // Connecting different targets.
    }
  }

  ShProgram program = new ShProgramNode(rkind);

  ShCtrlGraphNodePtr heada, taila, headb, tailb;
  
  copyCtrlGraph(a->ctrlGraph->entry(), a->ctrlGraph->exit(), heada, taila);
  copyCtrlGraph(b->ctrlGraph->entry(), b->ctrlGraph->exit(), headb, tailb);

  taila->append(headb);
  
  program->ctrlGraph = new ShCtrlGraph(heada, tailb);

  for (ShProgramNode::VarList::const_iterator I = a->inputs.begin(); I != a->inputs.end(); ++I) {
    program->inputs.push_back(*I);
  }
  for (ShProgramNode::VarList::const_iterator I = b->outputs.begin(); I != b->outputs.end(); ++I) {
    program->outputs.push_back(*I);
  }
  
  VarMap varMap;

  SH_DEBUG_PRINT("Smashing variables together");
  
  ShEnvironment::shader = program;
  ShEnvironment::insideShader = true;
  
  ShProgramNode::VarList::const_iterator I, J;  
  for (I = a->outputs.begin(), J = b->inputs.begin(); I != a->outputs.end(); ++I, ++J) {
    SH_DEBUG_PRINT("Smashing a variable..");
    SH_DEBUG_ASSERT((*I)->size() == (*J)->size());
    ShVariableNodePtr n = new ShVariableNode(SH_VAR_TEMP, (*I)->size());
    varMap[*I] = n;
    varMap[*J] = n;
  }

  ShEnvironment::shader = 0;
  ShEnvironment::insideShader = false;

  VariableReplacer replacer(varMap);
  program->ctrlGraph->dfs(replacer);

  ShOptimizer optimizer(program->ctrlGraph);
  optimizer.optimize(ShEnvironment::optimizationLevel);
  
  program->collectVariables();

  return program;
}

ShProgram combine(const ShProgram& a, const ShProgram& b)
{
  int rkind;

  if (a->kind() < 0) {
    rkind = b->kind(); // A doesn't have a target. Use b's.
  } else {
    if (b->kind() < 0) {
      rkind = a->kind(); // A has a target, b doesn't
    } else {
      rkind = -1; // Connecting different targets.
    }
  }

  ShProgram program = new ShProgramNode(rkind);

  ShCtrlGraphNodePtr heada, taila, headb, tailb;
  
  copyCtrlGraph(a->ctrlGraph->entry(), a->ctrlGraph->exit(), heada, taila);
  copyCtrlGraph(b->ctrlGraph->entry(), b->ctrlGraph->exit(), headb, tailb);

  taila->append(headb);
  
  program->ctrlGraph = new ShCtrlGraph(heada, tailb);

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
  return connect(a, b);
}

ShProgram operator+(const ShProgram& a, const ShProgram& b)
{
  return combine(a, b);
}

}
