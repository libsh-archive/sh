#include "ShEvaluate.hpp"
#include "ShInstructions.hpp"
#include "ShDebug.hpp"
#include "ShContext.hpp"
#include "ShVariant.hpp"

namespace SH {

void evaluate(ShStatement& stmt)
{
  // TODO: Maybe not do this _every_ time we call evaluate...
  stmt.dest.node()->addVariant();
  for (int i = 0; i < opInfo[stmt.op].arity; i++) {
    stmt.src[i].node()->addVariant();
  }
  // Make sure we are outside of a program definition
  ShContext::current()->enter(0);
  switch (stmt.op) {
  case SH_OP_ASN:
    shASN(stmt.dest, stmt.src[0]);
    break;
  case SH_OP_ADD:
    shADD(stmt.dest, stmt.src[0], stmt.src[1]);
    break;
  case SH_OP_MUL:
    shMUL(stmt.dest, stmt.src[0], stmt.src[1]);
    break;
  case SH_OP_DIV:
    shDIV(stmt.dest, stmt.src[0], stmt.src[1]);
    break;
  case SH_OP_SLT:
    shSLT(stmt.dest, stmt.src[0], stmt.src[1]);
    break;
  case SH_OP_SLE:
    shSLE(stmt.dest, stmt.src[0], stmt.src[1]);
    break;
  case SH_OP_SGT:
    shSGT(stmt.dest, stmt.src[0], stmt.src[1]);
    break;
  case SH_OP_SGE:
    shSGE(stmt.dest, stmt.src[0], stmt.src[1]);
    break;
  case SH_OP_SEQ:
    shSEQ(stmt.dest, stmt.src[0], stmt.src[1]);
    break;
  case SH_OP_SNE:
    shSNE(stmt.dest, stmt.src[0], stmt.src[1]);
    break;
  case SH_OP_ABS:
    shABS(stmt.dest, stmt.src[0]);
    break;
  case SH_OP_ACOS:
    shACOS(stmt.dest, stmt.src[0]);
    break;
  case SH_OP_ASIN:
    shASIN(stmt.dest, stmt.src[0]);
    break;
  case SH_OP_ATAN:
    shATAN(stmt.dest, stmt.src[0]);
    break;
  case SH_OP_ATAN2:
    shATAN2(stmt.dest, stmt.src[0], stmt.src[1]);
    break;
  case SH_OP_CEIL:
    shCEIL(stmt.dest, stmt.src[0]);
    break;
  case SH_OP_COS:
    shCOS(stmt.dest, stmt.src[0]);
    break;
  case SH_OP_CMUL:
    shCMUL(stmt.dest, stmt.src[0]);
    break;
  case SH_OP_CSUM:
    shCSUM(stmt.dest, stmt.src[0]);
    break;
  case SH_OP_DOT:
    shDOT(stmt.dest, stmt.src[0], stmt.src[1]);
    break;
  case SH_OP_DX:
    shDX(stmt.dest, stmt.src[0]);
    break;
  case SH_OP_DY:
    shDY(stmt.dest, stmt.src[0]);
    break;
  case SH_OP_EXP:
    shEXP(stmt.dest, stmt.src[0]);
    break;
  case SH_OP_EXP2:
    shEXP2(stmt.dest, stmt.src[0]);
    break;
  case SH_OP_EXP10:
    shEXP10(stmt.dest, stmt.src[0]);
    break;
  case SH_OP_FLR:
    shFLR(stmt.dest, stmt.src[0]);
    break;
  case SH_OP_FRAC:
    shFRAC(stmt.dest, stmt.src[0]);
    break;
  case SH_OP_LOG:
    shLOG(stmt.dest, stmt.src[0]);
    break;
  case SH_OP_LOG2:
    shLOG2(stmt.dest, stmt.src[0]);
    break;
  case SH_OP_LOG10:
    shLOG10(stmt.dest, stmt.src[0]);
    break;
  case SH_OP_LRP:
    shLRP(stmt.dest, stmt.src[0], stmt.src[1], stmt.src[2]);
    break;
  case SH_OP_MAD:
    shMAD(stmt.dest, stmt.src[0], stmt.src[1], stmt.src[2]);
    break;
  case SH_OP_MAX:
    shMAX(stmt.dest, stmt.src[0], stmt.src[1]);
    break;
  case SH_OP_MIN:
    shMIN(stmt.dest, stmt.src[0], stmt.src[1]);
    break;
  case SH_OP_MOD:
    shMOD(stmt.dest, stmt.src[0], stmt.src[1]);
    break;
  case SH_OP_POW:
    shPOW(stmt.dest, stmt.src[0], stmt.src[1]);
    break;
  case SH_OP_RCP:
    shRCP(stmt.dest, stmt.src[0]);
    break;
  case SH_OP_RSQ:
    shRSQ(stmt.dest, stmt.src[0]);
    break;
  case SH_OP_SGN:
    shSGN(stmt.dest, stmt.src[0]);
    break;
  case SH_OP_SIN:
    shSIN(stmt.dest, stmt.src[0]);
    break;
  case SH_OP_SQRT:
    shSQRT(stmt.dest, stmt.src[0]);
    break;
  case SH_OP_TAN:
    shTAN(stmt.dest, stmt.src[0]);
    break;
  case SH_OP_NORM:
    shNORM(stmt.dest, stmt.src[0]);
    break;
  case SH_OP_XPD:
    shXPD(stmt.dest, stmt.src[0], stmt.src[1]);
    break;
  case SH_OP_COND:
    shCOND(stmt.dest, stmt.src[0], stmt.src[1], stmt.src[2]);
    break;
  case SH_OP_KIL:
    shKIL(stmt.src[0]);
    break;
  default:
    // TODO: Replace with shError().
    SH_DEBUG_ASSERT(0 && "Invalid Statement");
    break;
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

