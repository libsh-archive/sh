#include "ShShader.hpp"

namespace SH {

ShShaderNode::ShShaderNode(int kind)
  : m_kind(kind)
{
}

void ShShaderNode::collectVariables()
{
  if (ctrlGraph->entry()) {
    ctrlGraph->entry()->clearMarked();
    collectNodeVars(ctrlGraph->entry());
    ctrlGraph->entry()->clearMarked();
  }
}

void ShShaderNode::collectNodeVars(const ShCtrlGraphNodePtr& node)
{
  if (node->marked()) return;
  node->mark();

  if (node->block) for (ShBasicBlock::ShStmtList::const_iterator I = node->block->begin();
       I != node->block->end(); ++I) {
    collectVar(I->dest.node());
    collectVar(I->src1.node());
    collectVar(I->src2.node());
  }

  for (std::vector<ShCtrlGraphBranch>::const_iterator J = node->successors.begin();
       J != node->successors.end(); ++J) {
    collectNodeVars(J->node);
  }
  
  if (node->follower) collectNodeVars(node->follower);
}

void ShShaderNode::collectVar(const ShVariableNodePtr& var) {
  if (!var) return;
  if (var->uniform()) {
    uniforms.insert(var);
  } else switch (var->kind()) {
  case SH_VAR_INPUT:
    inputs.insert(var);
  case SH_VAR_OUTPUT:
    outputs.insert(var);
  case SH_VAR_TEMP:
    temps.insert(var);
  case SH_VAR_CONST:
    constants.insert(var);
  }
}

}
