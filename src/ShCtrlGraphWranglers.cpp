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
#include "ShCtrlGraphWranglers.hpp"

namespace SH  {

void spliceProgram(ShCtrlGraphNodePtr node, ShBasicBlock::ShStmtList::iterator stmt, ShProgram &program) 
{
  ShCtrlGraphNodePtr before = node;  
  ShCtrlGraphNodePtr after = node->split(stmt);

  // must do this first to ensure after gets refcount inc'd
  program.node()->ctrlGraph->exit()->append(after); 
  before->follower = program.node()->ctrlGraph->entry();
}

void replaceStmt(ShCtrlGraphNodePtr node, ShBasicBlock::ShStmtList::iterator stmt, ShProgram &program)
{
  ShBasicBlockPtr &entryBlock = program.node()->ctrlGraph->entry()->block;
  ShBasicBlockPtr &exitBlock = program.node()->ctrlGraph->exit()->block;
  if(!entryBlock) entryBlock = new ShBasicBlock();
  if(!exitBlock) exitBlock = new ShBasicBlock();

  ShProgramNode::VarList::const_iterator I = program.inputs_begin(); 
  ShProgramNode::VarList::const_iterator O = program.outputs_begin(); 

  for(int i = 0; i < opInfo[stmt->op].arity; ++i, ++I) {
    // @todo type - deal with scalar -> tuple expansion?
    entryBlock->prependStatement(ShStatement(ShVariable(*I), SH_OP_ASN, stmt->src[i])); 
  }
  exitBlock->addStatement(ShStatement(stmt->dest, SH_OP_ASN, ShVariable(*O)));

  spliceProgram(node, stmt, program);

  // remove stmt from node
  node->block->erase(stmt);
}

}
