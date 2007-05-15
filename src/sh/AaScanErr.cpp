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
#include <fstream>
#include <sstream>
#include "Syntax.hpp"
#include "Stream.hpp"
//#include "RecStream.hpp"
#include "Structural.hpp"
#include "Section.hpp"
#include "Inclusion.hpp"
#include "AaScanCore.hpp"
#include "AaScanErr.hpp"

#ifdef DBG_AA
#define DBG_ASECANERR
#endif
#define DBG_ASECANERR

#ifdef DBG_ASECANERR
#define SH_DEBUG_PRINT_ASE(x) { SH_DEBUG_PRINT(x); }
#else
#define SH_DEBUG_PRINT_ASE(x) {}
#endif

namespace {
using namespace SH;
Program rgbToLightness(Variable output) 
{
  Program result = SH_BEGIN_PROGRAM() {
    Variable rgbIn(output.node()->clone(SH_INPUT));
    Variable lightnessOut(output.node()->clone(SH_OUTPUT, 1)); 
    lightnessOut.name("lightness");

    // RGB -> Y (luminance) (for D65 illuminant)
    // Taken from www.easyrgb.com (probably should go to CIE site...)
    Variable Y(output.node()->clone(SH_TEMP, 1)); 
    Y.name("Y");

    ConstAttrib3f rgbToY(0.2126f, 0.7152f, 0.0722f);
    shDOT(Y, rgbIn, rgbToY);

    // Y -> L  
    // L = 7.787 * (Y / 100.0)^(1/3) + 16/116 
    shMUL(Y, Y, ConstAttrib1f(0.01f)); 
    shPOW(Y, Y, ConstAttrib1f(1.0f/3.0f));
    shMAD(lightnessOut, Y, ConstAttrib1f(7.787), ConstAttrib1f(16.0/116.0));
  } SH_END;
  return result;
}

struct ScanErrDumper {
  AaScanErrMap& errMap;
  IndexStmtMap& indexStmt;
  double maxErr; // max error (absolute value) 

  ScanErrDumper(AaScanErrMap& errMap, IndexStmtMap& indexStmt)
    : errMap(errMap), indexStmt(indexStmt) {
    maxErr = 0;
    // @todo only include statement indices from original program  
    for(AaScanErrMap::iterator I = errMap.begin(); I != errMap.end(); ++I) {
      if(indexStmt.find(I->first) == indexStmt.end()) {
        //SH_DEBUG_PRINT_ASE("Ignoring " << *I->second.stmt << " for max err calculation.");
        continue;
      }
      maxErr = std::max(maxErr, I->second.err);
    }
    SH_DEBUG_PRINT_ASE("Max error = " << maxErr);
  }

  void operator()(std::ostream& out, SectionNodePtr section)
  {
    // @todo range - grab error from ESCJOINs before end of section
    out << "label=\"" << section->name() << "\";" << std::endl;
  }

  /* prints an integer value between 0-255 in 2 digit hex */
  std::ostream& hex(std::ostream& out, int val)
  {
    if(val < 16) out << "0";
    out << std::hex << val;
    return out;
  }

  std::ostream& colorOf(std::ostream& out, double error)
  {
    out << "#";
    int val = static_cast<int>(255 * error / maxErr);

    // simple green -> yellow -> red map (linear)
    if(val < 127) {
      hex(out, val * 2);
      hex(out, 255);
    } else {
      hex(out, 255);
      hex(out, (255 - val) * 2);
    }
    hex(out, 0);
    return out;
  }

  void operator()(std::ostream& out, CtrlGraphNode* node)
  {
    out << "[label=<";

    if(node->block) {
      out << "<TABLE BORDER=\"0\">\n";
      for(BasicBlock::iterator I = node->block->begin(); I != node->block->end(); ++I) {
        Statement& stmt = *I; 
        StmtIndex* stmtIndex = stmt.get_info<StmtIndex>();

        SH_DEBUG_PRINT_ASE("Handling stmt " << stmt); 

        double error = 0;
        if(!stmtIndex) {
          SH_DEBUG_PRINT_ASE("  no stmt index" << stmt);
        } else if (errMap.find(*stmtIndex) == errMap.end()) {
          SH_DEBUG_PRINT_ASE("  no error info!");
        } else {
          error = errMap.find(*stmtIndex)->second.err; 
          SH_DEBUG_PRINT_ASE("  error " << error);
        }

        out << "  <TR><TD BGCOLOR=\"";
        colorOf(out, error) << "\">";
        if(!stmt.dest.null()) {
          out << stmt.dest.name() << " := ";
        }
        out << opInfo[stmt.op].name;
        out << "</TD></TR>";
      }
      out << "</TABLE>>, margin=\"0.0,0.0\", shape=box]";
    } else {
      out << ">, shape=circle, height=0.25]";
    }
  }

};
  
}

namespace SH {

AaStmtErr::AaStmtErr()
  : stmt(0), err(0)
{
}

AaStmtErr::AaStmtErr(StmtIndex index, Statement* stmt, double err) 
  : index(index), stmt(stmt), err(err)
{
}

std::ostream& operator<<(std::ostream& out, const AaStmtErr& stmterr)
{
  out << stmterr.index.index() << " err=" << stmterr.err 
    << " " << *stmterr.stmt; 
  return out;
}

AaScanErrMap aaScanLightnessErr(Program program)
{
  // Check that program 
  if(program.node()->outputs.size() != 1) {
    SH_DEBUG_PRINT_ASE("Cannot analyze programs with more than one output");
  }

  Variable output = *program.begin_outputs();
  if(output.size() != 3 || output.node()->specialType() != SH_COLOR) {
    SH_DEBUG_PRINT_ASE("Cannot handle non-RGB Color3x outputs");
  }

  // Add indices
  add_stmt_indices(program);

  Program progClone(program.node()->clone(false));

  // Tack on RGB->luminance conversion
  progClone = rgbToLightness(output) << progClone;

  // Pass to scan core
  AaScanCore scanCore(progClone);

  Stream input; // null ==> all default bounds

  AaScanCore::AaVarVec resultPattern;
  Record resultRecord;
  Stream result;

  result = scanCore.doit(0, input, resultPattern, resultRecord);

  //fetch one into resultRecord/resultPattern 
  lookup(resultRecord, result, 0);

  SH_DEBUG_PRINT_ASE("Results:");
  for(unsigned int i = 0; i < resultPattern.size(); ++i) {
    SH_DEBUG_PRINT_ASE(i << ": " << resultPattern[i].name() << "=" << resultPattern[i]); 
  }

  // Pull out results into  
  const AaVariable& aaOutput = resultPattern.front();
  const AaIndexSet& syms = aaOutput.use(0); 

  int i = 0; 
  Variable aaErr = aaOutput.err(0);
  AaScanErrMap errMap;
  for(AaIndexSet::iterator I = syms.begin(); I != syms.end(); ++I, ++i) {
    int index = *I; // noise symbol index
    double err;
    // this is kind of messy with the variants
    VariantPtr errVariant = variantFactory(SH_DOUBLE, HOST)->generate(1, &err, false);
    errVariant->set(aaErr.getVariant(i));
    err = std::abs(err);
    SH_DEBUG_PRINT_ASE("noise sym " << index << " err " << err);

    // now figure out which stmt this comes from and stick it in the map 
    // (add onto previous error if there is any)
    if(scanCore.isStmtSym(index)) {
      AaScanCore::StmtIntPair matchingStmt = scanCore.getStmt(index);
      Statement* stmt = matchingStmt.first;
      StmtIndex* matchingIdx = stmt->get_info<StmtIndex>();
      SH_DEBUG_ASSERT(matchingIdx);

      if(errMap.find(*matchingIdx) != errMap.end()) {
        errMap[*matchingIdx].err += err;
      } else {
        errMap[*matchingIdx] = AaStmtErr(*matchingIdx, stmt, err); 
      }
      SH_DEBUG_PRINT_ASE("Updating " << errMap[*matchingIdx]);
    }
  }

  return errMap;
}

void aaScanErrDump(const std::string& filename, AaScanErrMap& errMap, Program program)
{
  Structural structural(program.node()->ctrlGraph);
  SectionTree sectionTree(structural);

  IndexStmtMap indexStmt = gather_indices(program);

  ScanErrDumper sed(errMap, indexStmt);

  std::ostringstream sout;
  sectionTree.dump(sout, sed); 
  dotGen(sout.str(), filename);
}

}
