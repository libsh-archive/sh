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

#include "ShSyntax.hpp"
#include "ShVariableNode.hpp"
#include "ShAttrib.hpp"
#include "ShInstructions.hpp"
#include "ShTransformer.hpp"
#include "ShCtrlGraphWranglers.hpp"
#include "ShOptimizations.hpp"
#include "ShAaSyms.hpp"
#include "ShAaSymPlacer.hpp"
#include "ShAaVariable.hpp"
#include "ShAaOps.hpp"
#include "ShAaOpHandler.hpp"

#ifdef SH_DBG_AA
#define SH_DEBUG_AAOPHANDLER
#endif
//#define SH_DEBUG_AAOPHANDLER

#ifdef SH_DEBUG_AAOPHANDLER
#define SH_DEBUG_PRINT_AHO(x) { SH_DEBUG_PRINT(x) }
#else
#define SH_DEBUG_PRINT_AHO(X)
#endif

// @todo rnage
// Possible problems:
//  cases where dest = src[i] may not work

namespace {
using namespace SH;

/* Allocates regular tuples for affine variables and interval arithmetic sources
 * involved in affine computations.
 *
 * The program must have valid ShInfo elements placed using the SymPlacer
 */
struct SymAllocator {
  SymAllocator(ShProgramNodePtr p) 
    : m_program(p)
  { 
    ShAaProgramSyms* progSyms = m_program->get_info<ShAaProgramSyms>();

    SH_DEBUG_PRINT_AHO("Allocating regular tuples");
    // Allocate affine variables
    ShAaVarSymsMap::iterator A = progSyms->vars.begin();
    for(; A != progSyms->vars.end(); ++A) {
      ShVariableNodePtr node = A->first;
      ShAaSyms& varSyms = A->second;
      SH_DEBUG_PRINT_AHO("  affine variable " << node->name() << " syms=" << varSyms); 
      m_nodes[node] = new ShAaVariableNode(node, varSyms);
    }

    ShProgramNode::VarList::iterator I;

    // Transform input list
    ShProgram inputMapper = SH_BEGIN_PROGRAM() {
      for(I = m_program->inputs.begin(); I != m_program->inputs.end();) {
        ShVariableNodePtr node = *I;
        if(progSyms->inputs.find(node) == progSyms->inputs.end()) {
          ++I;
          continue;
        }

        ShAaSyms& inputSyms = progSyms->inputs[node];
        ShAaVariableNodePtr inputAaNode = new ShAaVariableNode(node, inputSyms, SH_INPUT, "_saIn"); 
        inputAaNode->varlistInsert(m_program->inputs, I);
        I = m_program->inputs.erase(I);

        // if the variable is used in the program, then it should be in vars as
        // well and we need to do an assignment from input to temp
        if(progSyms->vars.find(node) == progSyms->vars.end()) continue;
        ShAaVariable inputVar(inputAaNode);
        ShAaVariable tempVar(m_nodes[node]);
        tempVar.ZERO();
        tempVar.ASN(inputVar);
      }
    } SH_END;
    m_program->ctrlGraph->prepend(inputMapper.node()->ctrlGraph);

    // Transform input list
    ShProgram outputMapper= SH_BEGIN_PROGRAM() {
      for(I = m_program->outputs.begin(); I != m_program->outputs.end();) {
        ShVariableNodePtr node = *I;
        if(progSyms->outputs.find(node) == progSyms->outputs.end()) {
          ++I;
          continue;
        }

        ShAaSyms& outputSyms = progSyms->outputs[node];
        ShAaVariableNodePtr outputAaNode = new ShAaVariableNode(node, outputSyms, SH_OUTPUT, "_saOut"); 
        outputAaNode->varlistInsert(m_program->outputs, I);
        I = m_program->outputs.erase(I);

        // if the variable is used in the program, then it should be in vars as
        // well and we need to do an assignment from output to temp
        if(progSyms->vars.find(node) == progSyms->vars.end()) {
          SH_DEBUG_PRINT("Output was not initialized");
          SH_DEBUG_PRINT("  " << (*I)->name());
          continue;
        }
        ShAaVariable outputVar(outputAaNode);
        ShAaVariable tempVar(m_nodes[node]);
        // @todo range not necessary - outputVar.ZERO();
        outputVar.ASN(tempVar, outputSyms);
      }
    } SH_END;
    m_program->ctrlGraph->append(outputMapper.node()->ctrlGraph);
  }

  /* Returns the unswizzled, unnegated AaVariable corresponding to node with all
   * symbols in use 
   * @{ */
  ShAaVariable operator[](ShVariableNodePtr node) {
    SH_DEBUG_ASSERT(m_nodes.find(node) != m_nodes.end());
    SH_DEBUG_PRINT_AHO("alloc - lookup on " << node->name());
    return ShAaVariable(m_nodes[node]); 
  }
  // @}

  /** Returns the AaVariable swizzled and negated in the same way as the given
   * variable.  If use is provided, then the AaVariable takes on the
   * indicated usage of error symbols (which should be a subset of the
   * assigned ones)
   *
   * @{*/
  ShAaVariable operator()(const ShVariable& var, const ShAaSyms& use) {
    SH_DEBUG_PRINT_AHO("alloc - lookup on " << var.name());
    SH_DEBUG_ASSERT(m_nodes.find(var.node()) != m_nodes.end());

    ShAaVariableNodePtr node = m_nodes[var.node()];
    return ShAaVariable(node, var.swizzle(), var.neg(), use); 
  }

  ShAaVariable operator()(const ShVariable& var) {
    if(m_nodes.find(var.node()) == m_nodes.end()) {
      SH_DEBUG_PRINT("Cannot find " << var.name());
      SH_DEBUG_ASSERT(0);
    }
    SH_DEBUG_PRINT_AHO("alloc - lookup on " << var.name());

    ShAaVariableNodePtr node = m_nodes[var.node()];
    if(var.swizzle().identity()) {
      return ShAaVariable(node, var.swizzle(), var.neg(), node->syms()); 
    } else {
      return ShAaVariable(node, var.swizzle(), var.neg(), 
          node->syms().swizSyms(var.swizzle())); 
    }
  }
  // @}

  private:
    ShProgramNodePtr m_program;

    typedef std::map<ShVariableNodePtr, ShAaVariableNodePtr> AaNodeMap; 
    AaNodeMap m_nodes; 
};

/* Returns a program representing the affine computation for a given statement.
 * (Must be a statement that can be translated (not one of the special null dest
 * ones))
 */
ShProgram getProgram(ShStatement& stmt, SymAllocator& alloc) {
  const ShAaStmtSyms *stmtSyms = stmt.get_info<ShAaStmtSyms>();
  if(!stmtSyms) {
    SH_DEBUG_PRINT("No stmtSyms for " << stmt);
    SH_DEBUG_ASSERT(0);
  }

  int arity = opInfo[stmt.op].arity;

  /* handle special cases first - ops that assign to non-affine variables, or
   * sourced from non-affine variables */
  bool special = true;
  ShProgram result = SH_BEGIN_PROGRAM() {
    switch(stmt.op) {
      case SH_OP_LO: 
        SH_DEBUG_ASSERT(shIsAffine(stmt.src[0].valueType()));
        shASN(stmt.dest, alloc(stmt.src[0]).lo()); 
        break;
      case SH_OP_HI: 
        SH_DEBUG_ASSERT(shIsAffine(stmt.src[0].valueType()));
        shASN(stmt.dest, alloc(stmt.src[0]).hi()); 
        break;
      case SH_OP_WIDTH: 
        SH_DEBUG_ASSERT(shIsAffine(stmt.src[0].valueType()));
        shASN(stmt.dest, alloc(stmt.src[0]).width()); 
        break;
      case SH_OP_RADIUS: 
        SH_DEBUG_ASSERT(shIsAffine(stmt.src[0].valueType()));
        shASN(stmt.dest, alloc(stmt.src[0]).radius()); 
        break;
      case SH_OP_CENTER: 
        SH_DEBUG_ASSERT(shIsAffine(stmt.src[0].valueType()));
        shASN(stmt.dest, alloc(stmt.src[0]).center()); 
        break;
      case SH_OP_IVAL:
        alloc(stmt.dest).ASN(aaIVAL(stmt.src[0], stmt.src[1], stmtSyms->newdest));
        break;
      case SH_OP_LASTERR:
        {
          SH_DEBUG_ASSERT(shIsRegularValueType(stmt.dest.valueType()));
          shASN(stmt.dest, aaLASTERR(alloc(stmt.src[0], stmtSyms->src[0]), stmtSyms->src[1])); 
        }
        break;
      case SH_OP_ASN:
        if(shIsAffine(stmt.dest.valueType())) {
          ShValueType srcvt = stmt.src[0].valueType();
          if(shIsAffine(srcvt)) { // handled below 
            special = false;
          } else if (shIsInterval(srcvt)) { // assign to new errsyms
            alloc(stmt.dest).ASN(aaFROMIVAL(stmt.src[0], stmtSyms->newdest));
          } else { //  
            if(!shIsRegularValueType(srcvt)) {
              SH_DEBUG_PRINT("Expecting regular src[0], actual type = "
                  << srcvt << " " << shValueTypeName(srcvt) 
                  << " stmt = " << stmt);
              SH_DEBUG_ASSERT(0);
            }
            shASN(alloc(stmt.dest).center(), stmt.src[0]);
          }
        } else if(shIsInterval(stmt.dest.valueType())) {
          SH_DEBUG_ASSERT(shIsAffine(stmt.src[0].valueType()));
          shASN(stmt.dest, aaTOIVAL(alloc(stmt.src[0], stmtSyms->src[0]))); 
        } else { 
          SH_DEBUG_ASSERT(shIsAffine(stmt.src[0].valueType()));
          // shouldn't be here...this ASN doesn't work.
          SH_DEBUG_PRINT("Offensive statement: " << stmt);
          SH_DEBUG_ASSERT(0 && "It's the end of the world! Is your data properly backed up off-site?");
        }
        break;
      default:
        special = false;
        break;
    }
  } SH_END;
  if(special) return result;
 
  /* handle all affine ops */
  result = SH_BEGIN_PROGRAM() {

    ShAaVariable dest; 
    ShValueType destValueType = stmt.dest.valueType();
    if(shIsAffine(destValueType)) {
      dest = alloc(stmt.dest); 
    } else if(shIsInterval(destValueType)) {
      dest = ShAaVariable(new ShAaVariableNode(stmt.dest.node(), stmtSyms->dest));  
    } else {
      // @todo range - handle IA dests 
      SH_DEBUG_ASSERT(0 && "Cannot handle non-IA/AA dests yet");
    }

    ShAaVariable *src;
    if(arity > 0) src = new ShAaVariable[arity];
    for(int i = 0; i < arity; ++i) {
      /* @todo range handle IA inputs */
      ShValueType srcValueType = stmt.src[i].valueType();
      if(shIsAffine(srcValueType)) {
        src[i] = alloc(stmt.src[i], stmtSyms->src[i]);

        /* Handle when src is wrapping same variables as dest */ 
        if(src[i].node() == dest.node()) {
          src[i] = src[i].clone();
        }
      } else if (shIsInterval(srcValueType)) {
        src[i] = aaFROMIVAL(stmt.src[i], stmtSyms->src[i]); 
      } else {
        // @todo range - handle these properly with special case ops 
        src[i] = aaFROMTUPLE(stmt.src[i]);
      }

      /* @todo range handle vectorization of scalars */
      if(opInfo[stmt.op].result_source == ShOperationInfo::LINEAR && stmt.src[i].size() < stmt.dest.size() && stmt.src[i].size() == 1) {
        SH_DEBUG_ASSERT(0 && "Cannot vectorize scalar sources yet");
      //  src[i] = src[i].swiz(ShSwizzle(stmt.src[i].swizzle(), stmt.dest.size())); 
      }
      
    }

    dest.ZERO(); // always zero out dest - dead code will deal with some of the inefficiency
                 // @todo range - implement SSA... 
                 // @todo range - check if perhaps this can be handled before
                 // this
    switch(stmt.op) {
      case SH_OP_ASN:   dest.ASN(src[0]); break; 
      case SH_OP_ADD:   dest.ASN(aaADD(src[0], src[1]));  break;
      case SH_OP_MUL:   dest.ASN(aaMUL(src[0], src[1], stmtSyms->newdest)); break;
    //  case SH_OP_DIV:   return affineDIV(stmt, alloc);

#if 0
      case SH_OP_SLT:   return affineSLT(N, valueType);
      case SH_OP_SLE:   return affineSLE(N, valueType);
      case SH_OP_SGT:   return affineSGT(N, valueType);
      case SH_OP_SGE:   return affineSGE(N, valueType);

      case SH_OP_CEIL:   return affineBinaryMonotonic<SH_OP_CEIL>(N, valueType);
      case SH_OP_CSUM:   return affineCSUM(N, valueType);
#endif
      case SH_OP_DOT:   dest.ASN(aaDOT(src[0], src[1], stmtSyms->newdest)); break; 
      case SH_OP_EXP:   dest.ASN(aaEXP(src[0], stmtSyms->newdest)); break; 
      case SH_OP_EXP2:  dest.ASN(aaEXP2(src[0], stmtSyms->newdest)); break;  
      case SH_OP_EXP10: dest.ASN(aaEXP10(src[0], stmtSyms->newdest)); break;   
#if 0
      case SH_OP_FLR:   return affineBinaryMonotonic<SH_OP_FLR>(N, valueType);
#endif
      case SH_OP_LOG:   dest.ASN(aaLOG(src[0], stmtSyms->newdest)); break; 
      case SH_OP_LOG2:  dest.ASN(aaLOG2(src[0], stmtSyms->newdest)); break; 
      case SH_OP_LOG10: dest.ASN(aaLOG10(src[0], stmtSyms->newdest)); break; 
      case SH_OP_LRP:   dest.ASN(aaLRP(src[0], src[1], src[2], stmtSyms->newdest)); break; 
      case SH_OP_MAD:   dest.ASN(aaMAD(src[0], src[1], src[2], stmtSyms->newdest)); break; 
#if 0
      case SH_OP_MAX:   return affineBinaryMonotonic<SH_OP_MAX>(N, valueType);
      case SH_OP_MIN:   return affineBinaryMonotonic<SH_OP_MIN>(N, valueType);
#endif
      case SH_OP_POW:   dest.ASN(aaPOW(src[0], src[1], stmtSyms->newdest)); break;
      case SH_OP_RCP:   dest.ASN(aaRCP(src[0], stmtSyms->newdest)); break; 
#if 0
      case SH_OP_RND:   return affineBinaryMonotonic<SH_OP_RND>(N, valueType);
      case SH_OP_SGN:   return affineBinaryMonotonic<SH_OP_SGN>(N, valueType);
#endif
      case SH_OP_SQRT:  dest.ASN(aaSQRT(src[0], stmtSyms->newdest)); break; 
#if 0

      case SH_OP_UNION:   return affineUNION(N, valueType);
      case SH_OP_ISCT:   return affineISCT(N, valueType);
      case SH_OP_CONTAINS:   avCONTAINS(dest, src[0]); break;
      // @todo type add in some more operations
#endif
      case SH_OP_ESCJOIN: dest.ASN(aaESCJOIN(src[0], stmtSyms->dest, stmtSyms->newdest)); break;
      case SH_OP_ERRFROM: dest.ASN(src[0], stmtSyms->dest, false); break;
      default:
        SH_DEBUG_PRINT("Unable to translate op: " << opInfo[stmt.op].name << " in " << stmt);
        shError(ShTransformerException(
              "Cannot translate affine arithmetic operator" ));
    }
    if(arity > 0) delete[] src;

    if(shIsInterval(destValueType)) {
      shASN(stmt.dest, aaTOIVAL(dest));
    }
  } SH_END;
  return result; 
}

/** Splits up operations on intervals into separate non-interval ops
 * replaces LO, HI, SETLO, SETHI instructions with appropriatly.
 *
 * Assumes that optimization is disabled 
 */
struct SymOpSplitterBase: public ShTransformerParent {
  void init(SymAllocator *alloc) 
  {
    m_alloc = alloc;
  }

  /* Splits a statement.  Returns whether the operator requires splicing in a new program
   * fragment. 
   */
  bool handleStmt(ShBasicBlock::ShStmtList::iterator& I, ShCtrlGraphNodePtr node)
  {
    ShStatement& stmt = *I;
    if(stmt.dest.null()) return false;

    if(!shIsAffine(stmt.dest.valueType())) {
      bool hasAffine = false;
      for(int i = 0; i < opInfo[stmt.op].arity; ++i) {
        hasAffine |= shIsAffine(stmt.src[i].valueType());
      }
      if(!hasAffine) return false;
    }

    m_changed = true;

    // replace stmt with a new program fragment
    ShProgram newProgram = getProgram(stmt, *m_alloc); 
    SH_DEBUG_ASSERT(newProgram.node());
    
    spliceProgram(node, I, newProgram);
    I = node->block->erase(I); 

    return true;
  }

  SymAllocator* m_alloc;
};
typedef ShDefaultTransformer<SymOpSplitterBase> SymOpSplitter;

void dump(ShProgramNodePtr p, const char* name)
{
#ifdef SH_DEBUG_AAOPHANDLER
  p->collectVariables();
  p->dump(name);
#endif
}
} // namespace 

namespace SH {

bool handleAaOps(ShProgramNodePtr programNode) {
  bool changed = false;

  ShAaProgramSyms* progSyms = programNode->get_info<ShAaProgramSyms>();
  SH_DEBUG_ASSERT(progSyms && "Run placeAaSyms before calling this"); 

  dump(programNode, "aho_start");
  
  /* replace branches first with non-range types
   * @todo range - leave this out for now - it has been tested on
   * simple test cases and should work, but I'm not sure yet
   * how some transformations affect the validity of error term
   * values relative to the original program. */
  //m_changed |= fixRangeBranches(p);
  //p->dump("aho_fixbranch");

  /* Transform into a nice canonical form without automatic promotion casts, and 
   * automatic vectorization of scalars done explicitly. 
   * @todo range - cut out these transformations as it doesn't make
   * anything much easier, and the transformations may make symbol assingments
   * invalid. */
  //convertInputOutput();
  //extractCasts<AffineCanCast>();
  //vectorizeScalars<AffineDoVec>();
  //optimize(p);
  //p->dump("aho_canon");

  // Disable optimizations - some of the program fragment insertions may break
  // with dead code removal taking away too much for example.
  //
  // @todo range - check if this is still the case now with tracking of 
  // temporary declaration points.  If a program fragment does assign to 
  // a temp variable that may escape the program scope, it should not be dead-code
  // removed any more.
  int oldOptimization = ShContext::current()->optimization();
  ShContext::current()->optimization(0);
  ShContext::current()->enter(programNode);

  // Allocates regular tuples corresponding to assigned error symbols 
  // Change affine inputs, outputs to these regular tuples
  SymAllocator sa(programNode);
  dump(programNode, "aho_alloc");

  // Fix operations within the program 
  SymOpSplitter sos;
  sos.init(&sa);
  changed |= sos.transform(programNode);
  dump(programNode, "aho_opfix");

  // Clean up symbol assignments, since they're almost certainly no longer valid
  clearAaSyms(programNode);

  ShContext::current()->exit();
  ShContext::current()->optimization(oldOptimization);
 
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

  optimize(programNode); // should do this here since there are likely lots of program fragments that could use straightening start later work
  dump(programNode, "aho_done");
  return changed;
}

}

