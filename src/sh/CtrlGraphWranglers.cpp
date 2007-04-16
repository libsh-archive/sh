// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
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
#include "CtrlGraphWranglers.hpp"

namespace SH  {

void spliceProgram(CtrlGraphNode* node, BasicBlock::StmtList::iterator stmt, Program &program) 
{
  CtrlGraphNode* before = node;  
  CtrlGraphNode* after = node->split(stmt);
  CtrlGraph* owner = before->owner();

  /* Order here matters - make sure that the program's graph is owned by the node's CFG */
  program.node()->ctrlGraph->exit()->append(after); 
  before->change_owner(owner, true);
  before->follower(program.node()->ctrlGraph->entry());
}

void replaceStmt(CtrlGraphNode* node, BasicBlock::StmtList::iterator stmt, Program &program)
{
  BasicBlockPtr &entryBlock = program.node()->ctrlGraph->entry()->block;
  BasicBlockPtr &exitBlock = program.node()->ctrlGraph->exit()->block;
  if(!entryBlock) entryBlock = new BasicBlock();
  if(!exitBlock) exitBlock = new BasicBlock();

  ProgramNode::VarList::const_iterator I = program.begin_inputs(); 
  ProgramNode::VarList::const_iterator O = program.begin_outputs(); 

  for(int i = 0; i < opInfo[stmt->op].arity; ++i, ++I) {
    // @todo type - deal with scalar -> tuple expansion?
    entryBlock->prependStatement(Statement(Variable(*I), OP_ASN, stmt->src[i])); 
  }
  exitBlock->addStatement(Statement(stmt->dest, OP_ASN, Variable(*O)));

  spliceProgram(node, stmt, program);
  removeStmt(node, stmt);
}

void removeStmt(CtrlGraphNode* node, BasicBlock::StmtList::iterator stmt)
{
  node->block->erase(stmt);
}

void structSplit(StructuralNode* from, StructuralNode* to, CtrlGraphNode* fromCfg,
    CtrlGraphNode* toCfg) {
  StructuralNode::CfgMatchList cfgmatch; 

  for(StructuralNode::SuccessorList::iterator S = from->succs.begin();
      S != from->succs.end();) {
    if(S->second == to) {
      from->getSuccs(cfgmatch, *S);
      S = from->succs.erase(S);
    } else ++S;
  }

  // can only handle a single entry node in to right now
  CtrlGraphNode* toEntry = 0; 

  SH_DEBUG_PRINT("split cfgmatch size = " << cfgmatch.size()); 

  StructuralNode::CfgMatchList::iterator I;
  for(I = cfgmatch.begin(); I != cfgmatch.end(); ++I) {
    if(!toEntry) {
      toEntry = I->to;
    } else if(toEntry != I->to) {
      SH_DEBUG_ERROR("Cannot handle multiple-entries in to node");
    }
    I->from->follower(fromCfg);
    if(!I->isFollower()) I->from->successors_erase(I->S);
  }
  if(toEntry) toCfg->follower(toEntry);
}

void structReplaceExits(StructuralNode* node, CtrlGraphNode* head, CtrlGraphNode* tail) { 
  StructuralNode::CfgMatchList cfgmatch;

  node->getExits(cfgmatch);

  CtrlGraphNode* end = 0;

  SH_DEBUG_PRINT("structReplaceExits match size = " << cfgmatch.size());
  StructuralNode::CfgMatchList::iterator I;
  for(I = cfgmatch.begin(); I != cfgmatch.end(); ++I) {
    if(!end) {
      end = I->to;
    } else if(end != I->to) {
      SH_DEBUG_ERROR("Cannot handle branching exits from node");
    }
    I->from->follower(head);
    if(!I->isFollower()) {
      I->from->successors_erase(I->S);
    }
  }
  if(end && tail) tail->follower(end); 
}

void structReplaceEntries(StructuralNode* node, CtrlGraphNode* head, CtrlGraphNode* tail) { 
  StructuralNode::CfgMatchList cfgmatch; 

  node->getEntries(cfgmatch);

  CtrlGraphNode* entry = 0; // the entry into this node

  SH_DEBUG_PRINT("structReplaceEntries match size = " << cfgmatch.size());
  StructuralNode::CfgMatchList::iterator I;
  for(I = cfgmatch.begin(); I != cfgmatch.end(); ++I) {
    if(!entry) {
      entry = I->to;
    } else if(entry != I->to) {
      SH_DEBUG_ERROR("Cannot handle multiple-exits from node");
    }
    if(I->isFollower()) I->from->follower(head);
    else I->S->node = head;
  }
  if(entry && tail) tail->follower(entry); 
}

#if 0
void structRemove(StructuralNode* node) 
{
  CtrlGraphNode* empty = new CtrlGraphNode();
  empty->block = new BasicBlock();

  structReplaceExits(node, 0, empty);
  structReplaceEntries(node, empty, 0);
}
#endif

}
