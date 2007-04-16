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
#include "Evaluate.hpp"
#include "Eval.hpp"
#include "Debug.hpp"
#include "Context.hpp"
#include "Variant.hpp"

namespace SH {

void evaluate(Statement& stmt)
{
  if(stmt.dest.null()) return;
  // TODO: Maybe not do this _every_ time we call evaluate...
  stmt.dest.node()->addVariant();
  int arity = opInfo[stmt.op].arity;
  for (int i = 0; i < arity; i++) {
    stmt.src[i].node()->addVariant();
  }
  // Make sure we are outside of a program definition
  Context::current()->enter(0);
  Variant *dv, *sv[3];
  dv = sv[0] = sv[1] = sv[2] = 0;
  bool newd, news[3];
  if(stmt.op == OP_KIL) {
    newd = false;
  } else {
    newd = stmt.dest.loadVariant(dv);
  }
  for(int i = 0; i < arity; ++i) {
    news[i] = stmt.src[i].loadVariant(sv[i]);
  }
  (*Eval::instance())(stmt.op, dv, sv[0], sv[1], sv[2]);
  if(newd) {
    stmt.dest.setVariant(dv);
    delete dv;
  } else {
    stmt.dest.updateVariant();
  }
  for(int i = 0; i < arity; ++i) {
    if(news[i]) delete sv[i];
  }
  Context::current()->exit();
}

void evaluate(const ProgramNodeCPtr& p)
{
  CtrlGraphNode* node = p->ctrlGraph->entry();

  while (node != p->ctrlGraph->exit()) {
    if (!node) break; // Should never happen!
    
    BasicBlockPtr block = node->block;
    if (block) {
      for (BasicBlock::StmtList::iterator I = block->begin(); I != block->end(); ++I) {
        evaluate(*I);
      }
    }

    bool done = false;
    
    for (CtrlGraphNode::SuccessorIt I = node->successors_begin(); I != node->successors_end(); ++I) {
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

    node = node->follower();
  }
}

}

