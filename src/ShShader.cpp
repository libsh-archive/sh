#include "ShShader.hpp"
#include <cassert>
#include <algorithm>
#include "ShBackend.hpp"
#include "ShEnvironment.hpp"

namespace SH {

ShShaderNode::ShShaderNode(int kind)
  : m_kind(kind)
{
}

ShRefCount<ShBackendCode> ShShaderNode::code(ShRefCount<ShBackend>& backend) {
  if (!backend) return 0;
  assert(!ShEnvironment::insideShader);

  if (m_code.find(backend) == m_code.end()) {
    ShEnvironment::shader = this;
    ShEnvironment::insideShader = true;
    ShBackendCodePtr code = backend->generateCode(this);
#ifdef SH_DEBUG
    code->print(std::cerr);
#endif
    ShEnvironment::insideShader = false;
    m_code[backend] = code;
  }

  return m_code[backend];
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
    if (std::find(uniforms.begin(), uniforms.end(), var) != uniforms.end()) uniforms.push_back(var);
  } else switch (var->kind()) {
  case SH_VAR_INPUT:
  case SH_VAR_OUTPUT:
    // Taken care of by ShVariableNode constructor
    break;
  case SH_VAR_TEMP:
    if (std::find(temps.begin(), temps.end(), var) != temps.end()) temps.push_back(var);
    break;
  case SH_VAR_CONST:
    if (std::find(constants.begin(), constants.end(), var) != constants.end()) constants.push_back(var);
    break;
  }
}

}
