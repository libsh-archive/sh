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

#include <string>
#include <sstream>
#include "Syntax.hpp"
#include "VariableNode.hpp"
#include "Attrib.hpp"
#include "Instructions.hpp"
#include "Transformer.hpp"
#include "CtrlGraphWranglers.hpp"
#include "Optimizations.hpp"
#include "AaSyms.hpp"
#include "AaSymPlacer.hpp"
#include "AaVariable.hpp"
#include "AaOps.hpp"
#include "AaOpHandler.hpp"

using namespace std;

#ifdef DBG_AA
bool AohDebugOn = true;
#else
bool AohDebugOn = false; 
#endif

#define SH_DEBUG_PRINT_AHO(x) { if (AohDebugOn) {SH_DEBUG_PRINT(x);} }

// @todo rnage
// Possible problems:
//  cases where dest = src[i] may not work

namespace {
using namespace SH;

string dumpPrefix = ""; 

/* Allocates regular tuples for affine variables and interval arithmetic sources
 * involved in affine computations.
 *
 * The program must have valid Info elements placed using the SymPlacer
 */
struct SymAllocator {
  SymAllocator(ProgramNodePtr p) 
    : m_program(p)
  { 
    AaProgramSyms* progSyms = m_program->get_info<AaProgramSyms>();

    SH_DEBUG_PRINT_AHO("Allocating regular tuples");
    // Allocate affine variables
    AaVarSymsMap::iterator A = progSyms->vars.begin();
    for(; A != progSyms->vars.end(); ++A) {
      VariableNodePtr node = A->first;
      AaSyms& varSyms = A->second;
      SH_DEBUG_PRINT_AHO("  affine variable " << node->name() << " syms=" << varSyms); 
      m_nodes[node] = new AaVariableNode(node, varSyms);
    }

    ProgramNode::VarList::iterator I;

    // Transform input list
    Program inputMapper = SH_BEGIN_PROGRAM() {
      for(I = m_program->inputs.begin(); I != m_program->inputs.end();) {
        VariableNodePtr node = *I;
        if(progSyms->inputs.find(node) == progSyms->inputs.end()) {
          ++I;
          continue;
        }

        AaSyms& inputSyms = progSyms->inputs[node];
        AaVariableNodePtr inputAaNode = new AaVariableNode(node, inputSyms, SH_INPUT, "_saIn"); 
        inputAaNode->varlistInsert(m_program->inputs, I);
        I = m_program->inputs.erase(I);

        // if the variable is used in the program, then it should be in vars as
        // well and we need to do an assignment from input to temp
        if(progSyms->vars.find(node) == progSyms->vars.end()) continue;
        AaVariable inputVar(inputAaNode);
        AaVariable tempVar(m_nodes[node]);
        tempVar.ZERO();
        tempVar.ASN(inputVar);
      }
    } SH_END;
    // @todo range clone is inefficient
    auto_ptr<CtrlGraph> ingr(inputMapper.node()->ctrlGraph->clone());
    m_program->ctrlGraph->prepend(ingr);

    // Transform output list
    Program outputMapper= SH_BEGIN_PROGRAM() {
      for(I = m_program->outputs.begin(); I != m_program->outputs.end();) {
        VariableNodePtr node = *I;
        if(progSyms->outputs.find(node) == progSyms->outputs.end()) {
          ++I;
          continue;
        }

        AaSyms& outputSyms = progSyms->outputs[node];
        AaVariableNodePtr outputAaNode = new AaVariableNode(node, outputSyms, SH_OUTPUT, "_saOut"); 
        outputAaNode->varlistInsert(m_program->outputs, I);
        I = m_program->outputs.erase(I);

        // if the variable is used in the program, then it should be in vars as
        // well and we need to do an assignment from output to temp
        if(progSyms->vars.find(node) == progSyms->vars.end()) {
          SH_DEBUG_PRINT("Output was not initialized");
          SH_DEBUG_PRINT("  " << (*I)->name());
          continue;
        }
        AaVariable outputVar(outputAaNode);
        AaVariable tempVar(m_nodes[node]);
        // @todo range not necessary - outputVar.ZERO();
        outputVar.ASN(tempVar, outputSyms);
      }
    } SH_END;
    //@todo range clone() is inefficient
    auto_ptr<CtrlGraph> outgr(outputMapper.node()->ctrlGraph->clone());
    m_program->ctrlGraph->append(outgr);
  }

  /* Returns the unswizzled, unnegated AaVariable corresponding to node with all
   * symbols in use 
   * @{ */
  AaVariable operator[](VariableNodePtr node) {
    SH_DEBUG_PRINT_AHO("alloc - lookup on " << node->name());
    SH_DEBUG_ASSERT_PRINT(m_nodes.find(node) != m_nodes.end(),
                          "Variable " << node->nameOfType() << " " << node->name() << " not allocated");
    return AaVariable(m_nodes[node]); 
  }
  // @}

  /** Returns the AaVariable swizzled and negated in the same way as the given
   * variable.  If use is provided, then the AaVariable takes on the
   * indicated usage of error symbols (which should be a subset of the
   * assigned ones)
   *
   * @{*/
  AaVariable operator()(const Variable& var, const AaSyms& use) {
    SH_DEBUG_PRINT_AHO("alloc - lookup on " << var.name());
    SH_DEBUG_ASSERT_PRINT(m_nodes.find(var.node()) != m_nodes.end(),
                          "Variable " << var.node()->nameOfType() << " " << var.name() << " not allocated");

    AaVariableNodePtr node = m_nodes[var.node()];
    return AaVariable(node, var.swizzle(), var.neg(), use); 
  }

  AaVariable operator()(const Variable& var) {
    SH_DEBUG_PRINT_AHO("alloc - lookup on " << var.name());
    SH_DEBUG_ASSERT_PRINT(m_nodes.find(var.node()) != m_nodes.end(), 
                          "Variable " << var.node()->nameOfType() << " " << var.name() << " not allocated");

    AaVariableNodePtr node = m_nodes[var.node()];
    if(var.swizzle().identity()) {
      return AaVariable(node, var.swizzle(), var.neg(), node->syms()); 
    } else {
      return AaVariable(node, var.swizzle(), var.neg(), 
          node->syms().swizSyms(var.swizzle())); 
    }
  }
  // @}

  private:
    ProgramNodePtr m_program;

    typedef map<VariableNodePtr, AaVariableNodePtr> AaNodeMap; 
    AaNodeMap m_nodes; 
};

/* Returns a program representing the affine computation for a given statement.
 * (Must be a statement that can be translated (not one of the special null dest
 * ones))
 */
Program getProgram(Statement& stmt, SymAllocator& alloc) {
  const AaStmtSyms *stmtSyms = stmt.get_info<AaStmtSyms>();
  if(!stmtSyms) {
    SH_DEBUG_PRINT("No stmtSyms for " << stmt);
    SH_DEBUG_ASSERT(0);
  }

  int arity = opInfo[stmt.op].arity;

  /* handle special cases first - ops that assign to non-affine variables, or
   * sourced from non-affine variables */
  bool special = true;
  Program result = SH_BEGIN_PROGRAM() {
    // make a section
    ostringstream secOut;
    secOut << stmt;
    SH_BEGIN_SECTION(secOut.str()) {
      switch(stmt.op) {
        case OP_LO: 
          SH_DEBUG_ASSERT(isAffine(stmt.src[0].valueType()));
          shASN(stmt.dest, alloc(stmt.src[0]).lo()); 
          break;
        case OP_HI: 
          SH_DEBUG_ASSERT(isAffine(stmt.src[0].valueType()));
          shASN(stmt.dest, alloc(stmt.src[0]).hi()); 
          break;
        case OP_WIDTH: 
          SH_DEBUG_ASSERT(isAffine(stmt.src[0].valueType()));
          shASN(stmt.dest, alloc(stmt.src[0]).width()); 
          break;
        case OP_RADIUS: 
          SH_DEBUG_ASSERT(isAffine(stmt.src[0].valueType()));
          shASN(stmt.dest, alloc(stmt.src[0]).radius()); 
          break;
        case OP_CENTER: 
          SH_DEBUG_ASSERT(isAffine(stmt.src[0].valueType()));
          shASN(stmt.dest, alloc(stmt.src[0]).center()); 
          break;
        case OP_IVAL:
          alloc(stmt.dest).ASN(aaIVAL(stmt.src[0], stmt.src[1], stmtSyms->newdest));
          break;
        case OP_LASTERR:
          {
            SH_DEBUG_ASSERT(isRegularValueType(stmt.dest.valueType()));
            shASN(stmt.dest, aaLASTERR(alloc(stmt.src[0], stmtSyms->src[0]), stmtSyms->src[1])); 
          }
          break;
        case OP_ASN:
          if(isAffine(stmt.dest.valueType())) {
            ValueType srcvt = stmt.src[0].valueType();
            if(isAffine(srcvt)) { // handled below 
              special = false;
            } else if (isInterval(srcvt)) { // assign to new errsyms
              alloc(stmt.dest).ASN(aaFROMIVAL(stmt.src[0], stmtSyms->src[0]));
            } else { //  
              if(!isRegularValueType(srcvt)) {
                SH_DEBUG_PRINT("Expecting regular src[0], actual type = "
                    << srcvt << " " << valueTypeName(srcvt) 
                    << " stmt = " << stmt);
                SH_DEBUG_ASSERT(0);
              }
              Variable destCenter = alloc(stmt.dest).center();
              shASN(destCenter, stmt.src[0]);
            }
          } else if(isInterval(stmt.dest.valueType())) {
            SH_DEBUG_ASSERT(isAffine(stmt.src[0].valueType()));
            shASN(stmt.dest, aaTOIVAL(alloc(stmt.src[0], stmtSyms->src[0]))); 
          } else { 
            SH_DEBUG_ASSERT(isAffine(stmt.src[0].valueType()));
            // shouldn't be here...this ASN doesn't work.
            SH_DEBUG_PRINT("Offensive statement: " << stmt);
            SH_DEBUG_ASSERT(0 && "It's the end of the world! Is your data properly backed up off-site?");
          }
          break;
        default:
          special = false;
          break;
      }
    } SH_END_SECTION;
  } SH_END;
  if(special) return result;
 
  /* handle all affine ops */
  result = SH_BEGIN_PROGRAM() {
    ostringstream secOut;
    secOut << stmt;
    SH_BEGIN_SECTION(secOut.str()) {
      enum {
        AA_DEFAULT,
        AA_MERGE,
        IA_MERGE
      } mergeCase;
      AaVariable dest; 
      ValueType destValueType = stmt.dest.valueType();

      if(isAffine(destValueType)) {
        if(stmtSyms->mergeRep.empty()) {
          mergeCase = AA_DEFAULT;
          dest = alloc(stmt.dest); 
        } else {
          mergeCase = AA_MERGE;
          dest = AaVariable(new AaVariableNode(stmt.dest.node(), stmtSyms->dest));  
        }
      } else if(isInterval(destValueType)) {
        mergeCase = IA_MERGE;
        dest = AaVariable(new AaVariableNode(stmt.dest.node(), stmtSyms->dest));  
      } else {
        // @todo range - handle IA dests 
        SH_DEBUG_PRINT("non-IA/AA dest on stmt " << stmt); 
        SH_DEBUG_ASSERT(0 && "Cannot handle non-IA/AA dests yet");
      }

      AaVariable *src = 0;
      if(arity > 0) src = new AaVariable[arity];
      for(int i = 0; i < arity; ++i) {
        /* ignore tex sources */
        if(stmt.src[i].node()->kind() == SH_TEXTURE) {
          continue;
        }

        /* @todo range handle IA inputs */
        ValueType srcValueType = stmt.src[i].valueType();
        if(isAffine(srcValueType)) {
          src[i] = alloc(stmt.src[i], stmtSyms->src[i]);

          /* Handle when src is wrapping same variables as dest */ 
          if(src[i].node() == dest.node()) {
            src[i] = src[i].clone();
          }
        } else if (isInterval(srcValueType)) {
          src[i] = aaFROMIVAL(stmt.src[i], stmtSyms->src[i]); 
        } else {
          // @todo range - handle these properly with special case ops 
          src[i] = aaFROMTUPLE(stmt.src[i]);
        }

        /* @todo range handle vectorization of scalars */
        if(opInfo[stmt.op].result_source == OperationInfo::LINEAR && 
            stmt.src[i].size() < stmt.dest.size() && stmt.src[i].size() == 1) {
          src[i] = src[i].repeat(stmt.dest.size());
        }
        
      }

      //dest.ZERO(); // always zero out dest - dead code will deal with some of the inefficiency
                   // @todo range - implement SSA... 
                   // @todo range - check if perhaps this can be handled before
                   // this
      switch(stmt.op) {
        case OP_ASN:   dest.ASN(src[0]); break; 
        case OP_ADD:   dest.ASN(aaADD(src[0], src[1]));  break;
        case OP_MUL:   dest.ASN(aaMUL(src[0], src[1], stmtSyms->newdest)); break;
      //  case OP_DIV:   return affineDIV(stmt, alloc);

        case OP_SLT:   dest.ASN(aaSLT(src[0], src[1], stmtSyms->newdest)); break; 
        case OP_SLE:   dest.ASN(aaSLE(src[0], src[1], stmtSyms->newdest)); break; 
        case OP_SGT:   dest.ASN(aaSGT(src[0], src[1], stmtSyms->newdest)); break; 
        case OP_SGE:   dest.ASN(aaSGE(src[0], src[1], stmtSyms->newdest)); break; 
        case OP_SEQ:   dest.ASN(aaSEQ(src[0], src[1], stmtSyms->newdest)); break; 
        case OP_SNE:   dest.ASN(aaSNE(src[0], src[1], stmtSyms->newdest)); break; 

#if 0
        case OP_CEIL:   return affineBinaryMonotonic<OP_CEIL>(N, valueType);
#endif
        case OP_ACOS:  dest.ASN(aaACOS(src[0], stmtSyms->newdest)); break;
        case OP_ASIN:  dest.ASN(aaASIN(src[0], stmtSyms->newdest)); break;
        case OP_ATAN:  dest.ASN(aaATAN(src[0], stmtSyms->newdest)); break;
        case OP_COS:   dest.ASN(aaCOS(src[0], stmtSyms->newdest)); break;
        case OP_CSUM:  dest.ASN(aaCSUM(src[0])); break;
        case OP_DOT:   dest.ASN(aaDOT(src[0], src[1], stmtSyms->newdest)); break; 
        case OP_EXP:   dest.ASN(aaEXP(src[0], stmtSyms->newdest)); break; 
        case OP_EXP2:  dest.ASN(aaEXP2(src[0], stmtSyms->newdest)); break;  
        case OP_EXP10: dest.ASN(aaEXP10(src[0], stmtSyms->newdest)); break;   
        case OP_FLR:   dest.ASN(aaFLR(src[0], stmtSyms->newdest)); break;   
        case OP_FRAC:  dest.ASN(aaFRAC(src[0], stmtSyms->newdest)); break;   
        case OP_LOG:   dest.ASN(aaLOG(src[0], stmtSyms->newdest)); break; 
        case OP_LOG2:  dest.ASN(aaLOG2(src[0], stmtSyms->newdest)); break; 
        case OP_LOG10: dest.ASN(aaLOG10(src[0], stmtSyms->newdest)); break; 
        case OP_LRP:   dest.ASN(aaLRP(src[0], src[1], src[2], stmtSyms->newdest)); break; 
        case OP_MAD:   dest.ASN(aaMAD(src[0], src[1], src[2], stmtSyms->newdest)); break; 
        case OP_MAX:   dest.ASN(aaMAX(src[0], src[1], stmtSyms->newdest)); break; 
        case OP_MIN:   dest.ASN(aaMIN(src[0], src[1], stmtSyms->newdest)); break; 
        case OP_NORM:  dest.ASN(aaNORM(src[0], stmtSyms->newdest)); break; 
        case OP_POW:   dest.ASN(aaPOW(src[0], src[1], stmtSyms->newdest)); break;
        case OP_RCP:   dest.ASN(aaRCP(src[0], stmtSyms->newdest)); break; 
        case OP_RSQ:   dest.ASN(aaRSQ(src[0], stmtSyms->newdest)); break; 
        case OP_SIN:   dest.ASN(aaSIN(src[0], stmtSyms->newdest)); break;
#if 0
        case OP_RND:   return affineBinaryMonotonic<OP_RND>(N, valueType);
        case OP_SGN:   return affineBinaryMonotonic<OP_SGN>(N, valueType);
#endif
        case OP_SQRT:  dest.ASN(aaSQRT(src[0], stmtSyms->newdest)); break; 
        case OP_TEX:   dest.ASN(aaTEX(stmt.src[0], src[1], stmtSyms->dest, stmtSyms->newdest)); break; 
        case OP_TEXI:  dest.ASN(aaTEXI(stmt.src[0], src[1], stmtSyms->dest, stmtSyms->newdest)); break; 
        case OP_UNION: dest.ASN(aaUNION(src[0], src[1], stmtSyms->newdest)); break;
#if 0
        case OP_ISCT:   return affineISCT(N, valueType);
        case OP_CONTAINS:   avCONTAINS(dest, src[0]); break;
        // @todo type add in some more operations
#endif
        case OP_ESCJOIN: dest.ASN(aaESCJOIN(src[0], stmtSyms->dest, stmtSyms->newdest)); break;
        case OP_ERRFROM: dest.ASN(src[0], stmtSyms->dest, false); break;
        default:
          SH_DEBUG_PRINT("Unable to translate op: " << opInfo[stmt.op].name << " in " << stmt);
          error(TransformerException(
                "Cannot translate affine arithmetic operator" ));
      }
      if(arity > 0) delete[] src;

      switch(mergeCase) {
        case AA_DEFAULT:
          break;
        case AA_MERGE: {
            AaVariable mergeDest = alloc(stmt.dest);
            aaUNIQUE_MERGE(mergeDest, dest, stmtSyms); 
          }
          break;
        case IA_MERGE:
          shASN(stmt.dest, aaTOIVAL(dest));
          break;
      }
    } SH_END_SECTION;
  } SH_END;
  return result; 
}

/** Splits up operations on intervals into separate non-interval ops
 * replaces LO, HI, SETLO, SETHI instructions with appropriatly.
 *
 * Assumes that optimization is disabled 
 */
struct SymOpSplitterBase: public TransformerParent {
  void init(SymAllocator *alloc) 
  {
    m_alloc = alloc;
  }

  /* Splits a statement.  Returns whether the operator requires splicing in a new program
   * fragment. 
   */
  bool handleStmt(BasicBlock::StmtList::iterator& I, CtrlGraphNode* node)
  {
    Statement& stmt = *I;
    if(stmt.dest.null()) return false;

    if(!isAffine(stmt.dest.valueType())) {
      bool hasAffine = false;
      for(int i = 0; i < opInfo[stmt.op].arity; ++i) {
        hasAffine |= isAffine(stmt.src[i].valueType());
      }
      if(!hasAffine) return false;
    }

    m_changed = true;
    SH_DEBUG_PRINT_AHO("replacing " << stmt); 

    // replace stmt with a new program fragment
    Program newProgram = getProgram(stmt, *m_alloc); 
    SH_DEBUG_ASSERT(newProgram.node());
    
    spliceProgram(node, I, newProgram);
    I = node->block->erase(I); 

    return true;
  }

  SymAllocator* m_alloc;
};
typedef DefaultTransformer<SymOpSplitterBase> SymOpSplitter;

struct SectionRemoverBase: public TransformerParent {
  /* Splits a statement.  Returns whether the operator requires splicing in a new program
   * fragment. 
   */
  bool handleStmt(BasicBlock::StmtList::iterator& I, CtrlGraphNode* node)
  {
    Statement& stmt = *I;
    if(stmt.op == OP_STARTSEC || stmt.op == OP_ENDSEC) {
      I = node->block->erase(I);
      return true;
    }
    return false;
  }
};
typedef DefaultTransformer<SectionRemoverBase> SectionRemover;

void dump(ProgramNodePtr p, string name) 
{
  if (AohDebugOn) {
    p->collectAll();
    p->dump(dumpPrefix + name);
  }
}


} // namespace 

namespace SH {

bool handleAaOps(ProgramNodePtr programNode) {
  bool changed = false;
  bool disableHier = !Context::current()->get_flag("aa_enable_hier"); 
  bool disableUniqMerge = !Context::current()->get_flag("aa_enable_um"); 
  AohDebugOn = !Context::current()->get_flag("aa_disable_debug"); 

  ostringstream idout;
  idout << programNode->name() /*<< "_" << programNode.object()*/;
  if(disableHier) idout << "_nohier";
  if(disableUniqMerge) idout << "_noum";
  idout << "_";
  dumpPrefix = idout.str();

  AaProgramSyms* progSyms = programNode->get_info<AaProgramSyms>();
  SH_DEBUG_ASSERT(progSyms && "Run placeAaSyms before calling this"); 

  dump(programNode, "_aho-0-start");
  
  /* replace branches first with non-range types
   * @todo range - leave this out for now - it has been tested on
   * simple test cases and should work, but I'm not sure yet
   * how some transformations affect the validity of error term
   * values relative to the original program. */
  //m_changed |= fixRangeBranches(p);
  //p->dump("_aho_fixbranch");

  /* Transform into a nice canonical form without automatic promotion casts, and 
   * automatic vectorization of scalars done explicitly. 
   * @todo range - cut out these transformations as it doesn't make
   * anything much easier, and the transformations may make symbol assingments
   * invalid. */
  //convertInputOutput();
  //extractCasts<AffineCanCast>();
  //vectorizeScalars<AffineDoVec>();
  //optimize(p);
  //p->dump("_aho_canon");

  // Disable optimizations - some of the program fragment insertions may break
  // with dead code removal taking away too much for example.
  //
  // @todo range - check if this is still the case now with tracking of 
  // temporary declaration points.  If a program fragment does assign to 
  // a temp variable that may escape the program scope, it should not be dead-code
  // removed any more.
  int oldOptimization = Context::current()->optimization();
  Context::current()->optimization(0);
  Context::current()->enter(programNode);

  // Allocates regular tuples corresponding to assigned error symbols 
  // Change affine inputs, outputs to these regular tuples
  SymAllocator sa(programNode);
  dump(programNode, "_aho-1-alloc");

  // Fix operations within the program 
  SymOpSplitter sos;
  sos.init(&sa);
  changed |= sos.transform(programNode);

  dump(programNode, "_aho-2-opfix");

  Context::current()->exit();
  Context::current()->optimization(oldOptimization);
  optimize(programNode); // should do this here since there are likely lots of program fragments that could use straightening start later work

  dump(programNode, "_aho-3-opfix_opt");

  // Clean up symbol assignments, since they're almost certainly no longer valid
  clearAaSyms(programNode);

  // Remove sections 
  SectionRemover sr;
  sr.transform(programNode);

  optimize(programNode); // should do this here since there are likely lots of program fragments that could use straightening start later work
  dump(programNode, "_aho-4-sectremove_opt");

 
  // Now here's the process
  // 1) Change inputs to intervals 
  //    Insert code at beginning to turn intervals into affine (center + 1 err
  //    sym) 
  // @todo range - may want inputs to be more flexible, but don't know
  // yet how to make interface with error symbols
  //
  // 2) Change affine uniforms/constants to interval types  
  //
  // 3) For each operation
  //  a) if affine, insert replacement code
  //  b) if not, find extra err sym, insert replacement code and gen delta err
  //  Should handle conversion of src if its an interval or a regular type 

  return changed;
}

}

