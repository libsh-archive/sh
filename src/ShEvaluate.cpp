// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////
#include "ShEvaluate.hpp"
#include "ShEval.hpp"
#include "ShDebug.hpp"
#include "ShContext.hpp"
#include "ShVariant.hpp"

namespace SH {

void evaluate(ShStatement& stmt)
{
  if(stmt.dest.null()) return;
  // TODO: Maybe not do this _every_ time we call evaluate...
  stmt.dest.node()->addVariant();
  int arity = opInfo[stmt.op].arity;
  for (int i = 0; i < arity; i++) {
    stmt.src[i].node()->addVariant();
  }
  // Make sure we are outside of a program definition
  ShContext::current()->enter(0);
  ShVariant *dv, *sv[3];
  dv = sv[0] = sv[1] = sv[2] = 0;
  bool newd, news[3];
  if(stmt.op == SH_OP_KIL) {
    newd = false;
  } else {
    newd = stmt.dest.loadVariant(dv);
  }
  for(int i = 0; i < arity; ++i) {
    news[i] = stmt.src[i].loadVariant(sv[i]);
  }
  (*ShEval::instance())(stmt.op, dv, sv[0], sv[1], sv[2]);
  if(newd) {
    stmt.dest.setVariant(dv);
    delete dv;
  } else {
    stmt.dest.updateVariant();
  }
  for(int i = 0; i < arity; ++i) {
    if(news[i]) delete sv[i];
  }
  ShContext::current()->exit();
}

void evaluate(const ShProgramNodePtr& p)
{
  ShCtrlGraphNodePtr node = p->ctrlGraph->entry();

  while (node != p->ctrlGraph->exit()) {
    if (!node) break; // Should never happen!
    
    ShBasicBlockPtr block = node->block;
    if (block) {
      for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
        evaluate(*I);
      }
    }

    bool done = false;
    
    for (std::vector<ShCtrlGraphBranch>::const_iterator I = node->successors.begin();
         I != node->successors.end(); ++I) {
      I->cond.node()->addVariant(); // Make sure the condition has values.
      bool jmp = false;
      for (int i = 0; i < I->cond.size(); i++) if (I->cond.getVariant()->isTrue()) jmp = true;
      if (jmp) {
        done = true;
        node = I->node;
        break;
      }
    }
    if (done) continue;

    node = node->follower;
  }
}

}

