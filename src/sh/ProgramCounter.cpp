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

#include "Attrib.hpp"
#include "CtrlGraph.hpp"
#include "Manipulator.hpp"
#include "ProgramCounter.hpp"

using namespace SH;

namespace {

struct AddScalarCount {
    Generic<2, float>& counter;
    AddScalarCount(Generic<2, float>& counter): counter(counter) {}

    void operator()(CtrlGraphNode* node) {
      if(!node->block) return; 
      size_t result = 0;
      for (BasicBlock::const_iterator I = node->block->begin(); I != node->block->end(); ++I) {
        const Statement& stmt = *I;
        int stmtScalarSize = 0; 
        for(int i = 0; i < opInfo[stmt.op].arity; ++i) {
          stmtScalarSize = std::max(stmtScalarSize, stmt.src[i].size());
        }
        result += stmtScalarSize;
      }
      Statement incstmt(counter(0), OP_ADD, counter(0), ConstAttrib1f(result)); 
      node->block->addStatement(incstmt);
    }
};


}

namespace SH {

Program instructionCount(const Program& a) {
  Program result = a.clone(true);
  Context::current()->enter(result.node()); 
  {
      /* initialize counter to 0 */
      CtrlGraphNode* entry = result.node()->ctrlGraph->prepend_entry();
      OutputAttrib2f counter;
      Statement initStmt(counter, OP_ASN, ConstAttrib2f(0.0f, 0.0f));
      entry->block->prependStatement(initStmt);

      /* insert count incrementers */
      AddScalarCount asc(counter);
      result.node()->ctrlGraph->dfs(asc);

     /* modify outputs so they are not removed by dead code by shunting them all into the second output */
     CtrlGraphNode* exit = result.node()->ctrlGraph->append_exit();
     ProgramNode::VarList::const_iterator I;
     for(I = result.begin_outputs(); I != result.end_outputs(); ++I) {
        Attrib1f temp;
        exit->block->addStatement(Statement(temp, OP_DOT, *I, *I));
        exit->block->addStatement(Statement(counter(1), OP_ADD, counter(1), temp));
     }
  }
  Context::current()->exit();
  result = swizzle(-1) << result;
  return result;
}

}
