#include <map>
#include <sstream>
#include "ShAlgebra.hpp"
#include "ShCtrlGraph.hpp"
#include "ShDebug.hpp"
#include "ShError.hpp"
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
    if (node->block) node->block = new ShBasicBlock(*node->block);
  }
  newHead = copyMap[head];
  newTail = copyMap[tail];

  head->clearMarked();
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
        if( !I->src[i].null() ) repVar(I->src[i]);
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
  if (a->outputs.size() != b->inputs.size()) {
    std::ostringstream os;
    os << "Cannot connect programs. Number of outputs (" << a->outputs.size() << ") != number of inputs"
      << b->inputs.size() << std::endl;
    os << "Outputs: ";
    for( ShProgramNode::VarList::const_iterator it = a->outputs.begin();
        it != a->outputs.end(); ++it ) {
      os << ShVariableSpecialTypeName[ (*it)->specialType() ] << (*it)->size() << " ";
    }
    os << std::endl << "Inputs: "; 
    for( ShProgramNode::VarList::const_iterator it = b->inputs.begin();
        it != b->inputs.end(); ++it ) {
      os << ShVariableSpecialTypeName[ (*it)->specialType() ] << (*it)->size() << " ";
    }

    ShError( ShAlgebraException( os.str() ) ); 
  }

  std::string rtarget;

  if (a->target().empty()) {
    rtarget = b->target(); // A doesn't have a target. Use b's.
  } else {
    if (b->target().empty()) {
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
  
  program->ctrlGraph = new ShCtrlGraph(heada, tailb);

  for (ShProgramNode::VarList::const_iterator II = a->inputs.begin(); II != a->inputs.end(); ++II) {
    program->inputs.push_back(*II);
  }
  for (ShProgramNode::VarList::const_iterator II = b->outputs.begin(); II != b->outputs.end(); ++II) {
    program->outputs.push_back(*II);
  }
  
  VarMap varMap;

  SH_DEBUG_PRINT("Smashing variables together");
  
  ShEnvironment::shader = program;
  ShEnvironment::insideShader = true;
  
  ShProgramNode::VarList::const_iterator I, J;  
  for (I = a->outputs.begin(), J = b->inputs.begin(); I != a->outputs.end(); ++I, ++J) {
    SH_DEBUG_PRINT("Smashing a variable..");
    if( (*I)->size() != (*J)->size() ) {
      ShError( ShAlgebraException( "Cannot smash variables " + (*I)->name() +
            " and " + (*J)->name() + " with different sizes." ) );
    }
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
  std::string rtarget;

  if (a->target().empty()) {
    rtarget = b->target(); // A doesn't have a target. Use b's.
  } else {
    if (b->target().empty()) {
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

ShProgram operator&(const ShProgram& a, const ShProgram& b)
{
  return combine(a, b);
}

ShProgram replaceUniform(const ShProgram& a, const ShVariable& v)
{
  if(!v.uniform()) {
    ShError( ShAlgebraException( "Cannot replace non-uniform variable" ) );
  }

  ShProgram program = new ShProgramNode(a->target());

  ShCtrlGraphNodePtr heada, taila;
  
  copyCtrlGraph(a->ctrlGraph->entry(), a->ctrlGraph->exit(), heada, taila);

  program->ctrlGraph = new ShCtrlGraph(heada, taila);

  for (ShProgramNode::VarList::const_iterator II = a->inputs.begin(); II != a->inputs.end(); ++II) {
    program->inputs.push_back(*II);
  }
  for (ShProgramNode::VarList::const_iterator II = a->outputs.begin(); II != a->outputs.end(); ++II) {
    program->outputs.push_back(*II);
  }
  
  VarMap varMap;

  SH_DEBUG_PRINT("Adding a new input to replace the uniform");
  
  ShEnvironment::shader = program;
  ShEnvironment::insideShader = true;
  // make a new input
  ShVariableNodePtr newInput = new ShVariableNode(SH_VAR_INPUT, v.size(), v.node()->specialType()); 
  varMap[v.node()] = newInput;

  ShEnvironment::shader = 0;
  ShEnvironment::insideShader = false;

  VariableReplacer replacer(varMap);
  program->ctrlGraph->dfs(replacer);

  ShOptimizer optimizer(program->ctrlGraph);
  optimizer.optimize(ShEnvironment::optimizationLevel);
  
  program->collectVariables();

  return program;
}

ShProgram permute(const ShProgram& a, int size, ... ) {

}

}
