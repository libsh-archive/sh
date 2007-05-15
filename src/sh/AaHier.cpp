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

#include <map>
#include "Transformer.hpp"
#include "Optimizations.hpp"
#include "Section.hpp"
#include "Inclusion.hpp"
#include "AaHier.hpp"

#ifdef DBG_AA
#define DBG_AAHIER
#endif
// #define DBG_AAHIER

#ifdef DBG_AAHIER
#define SH_DEBUG_PRINT_AH(x) { SH_DEBUG_PRINT(x); }
#else
#define SH_DEBUG_PRINT_AH(x) {}
#endif

using namespace SH;
using namespace std;

namespace {

struct EscStmtInserter
{
  EscStmtInserter(): curDepth(0) {}

  /* Because the DFS hits parent nodes before offspring, we can be sure
   * that an exit gets the least depth (nearest root of structural tree)
   *
   * Potentially, we may want to think of placing merges at the targets
   * instead of at the exit node, but it probably makes no difference */
  void operator()(StructuralNode* structnode) {
    BasicBlock::StmtList::iterator I;
    switch(structnode->type) {
      case StructuralNode::SECTION:
      case StructuralNode::SELFLOOP:
      case StructuralNode::WHILELOOP: {
        StructuralNode::CfgMatchList cfgExits; 

        /* Find live variables at each exit CFG node and insert OP_ESCJOIN */   
        structnode->getExits(cfgExits);
        for(StructuralNode::CfgMatchList::iterator C = cfgExits.begin(); C != cfgExits.end(); ++C) {
          CtrlGraphNode* cfgNode = C->from;
          LiveVars *alive = cfgNode->get_info<LiveVars>();

          if(!cfgNode->block) {
            cfgNode->block = new BasicBlock();
          }

          BasicBlock::StmtList escStmts; 
          for(LiveVars::ElementSet::iterator O = alive->out.begin(); O != alive->out.end(); ++O) {
            VariableNodePtr varNode = O->first;
            if(!isAffine(varNode->valueType())) continue;

            Variable var(varNode);
            Statement escJoin = Statement(var, OP_ESCJOIN, var);
            escJoin.add_info(new StmtDepth(curDepth));
            escStmts.push_back(escJoin);
          }

          I = cfgNode->block->end(); 
          if(I != cfgNode->block->begin() && I->op == OP_ENDSEC) --I;     
          cfgNode->block->splice(I, escStmts); 
          curDepth++; 
        }
        break;
      }
      default:
        // do nothing
        break;
    }

    /* assign depths to statements */
    if(structnode->cfg_node && structnode->cfg_node->block) {
      BasicBlockPtr block = structnode->cfg_node->block;
      for(I = block->begin(); I != block->end(); ++I) {
        if(I->op == OP_ESCJOIN) continue;
        I->destroy_info<StmtDepth>();
        I->add_info(new StmtDepth(curDepth));
      }
    }
  }

  void finish(StructuralNode* structnode) {
    switch(structnode->type) {
      case StructuralNode::SECTION:
      case StructuralNode::SELFLOOP:
      case StructuralNode::WHILELOOP:
        curDepth--;
        break;
      default:
        break;
    }
  }

  int curDepth;
  typedef map<CtrlGraphNode*, bool> CfgMap;
  CfgMap cfgEscaped; /* whether the cfg has already been escaped */
};

}

namespace SH {

void liveVarAnalysis(Structural &pstruct, Program& p) {
  // insert value tracking 
  insert_branch_instructions(p);
  find_live_vars(p);
  remove_branch_instructions(p);

  // Add special statements, and extra outputs.
  Context::current()->enter(p.node());
  EscStmtInserter esi;
  pstruct.dfs(esi);
  Context::current()->exit();
}



}
