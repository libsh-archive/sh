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
#include "ShInternals.hpp"
#include "ShDebug.hpp"

namespace SH { 

ShVariableReplacer::ShVariableReplacer(ShVarMap& v)
  : varMap(v) 
{
}

void ShVariableReplacer::operator()(ShCtrlGraphNodePtr node) 
{
  if (!node) return;
  ShBasicBlockPtr block = node->block;
  if (!block) return;
  for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
    if(!I->dest.null()) repVar(I->dest);
    for (int i = 0; i < 3; i++) {
      if( !I->src[i].null() ) repVar(I->src[i]);
    }
  }
}

void ShVariableReplacer::operator()(ShProgramNode::VarList &varList) 
{
  ShProgramNode::VarList::iterator I;
  for(I = varList.begin(); I != varList.end();) {
    if(varMap.count(*I) > 0) {
      varList.insert(I, varMap[*I]);
      I = varList.erase(I);
    } else ++I;
  }
}

void ShVariableReplacer::repVar(ShVariable& var) 
{
  ShVarMap::iterator I = varMap.find(var.node());
  if (I == varMap.end()) return;
  var = ShVariable(I->second, var.swizzle(), var.neg());
}

}


