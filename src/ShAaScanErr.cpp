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
#include "ShSyntax.hpp"
#include "ShStream.hpp"
#include "ShRecStream.hpp"
#include "ShStructural.hpp"
#include "ShSection.hpp"
#include "ShInclusion.hpp"
#include "ShAaScanCore.hpp"
#include "ShAaScanErr.hpp"

#ifdef SH_DBG_AA
#define SH_DBG_ASECANERR
#endif
#define SH_DBG_ASECANERR

#ifdef SH_DBG_ASECANERR
#define SH_DEBUG_PRINT_ASE(x) { SH_DEBUG_PRINT(x); }
#else
#define SH_DEBUG_PRINT_ASE(x) {}
#endif

namespace {
using namespace SH;
ShProgram rgbToLightness(ShVariable output) 
{
  ShProgram result = SH_BEGIN_PROGRAM() {
    ShVariable rgbIn(output.node()->clone(SH_INPUT));
    ShVariable lightnessOut(output.node()->clone(SH_OUTPUT, 1)); 
    lightnessOut.name("lightness");

    // RGB -> Y (luminance) (for D65 illuminant)
    // Taken from www.easyrgb.com (probably should go to CIE site...)
    ShVariable Y(output.node()->clone(SH_TEMP, 1)); 
    Y.name("Y");

    ShConstAttrib3f rgbToY(0.2126f, 0.7152f, 0.0722f);
    shDOT(Y, rgbIn, rgbToY);

    // Y -> L  
    // L = 7.787 * (Y / 100.0)^(1/3) + 16/116 
    shMUL(Y, Y, ShConstAttrib1f(0.01f)); 
    shPOW(Y, Y, ShConstAttrib1f(1.0f/3.0f));
    shMAD(lightnessOut, Y, ShConstAttrib1f(7.787), ShConstAttrib1f(16.0/116.0));
  } SH_END;
  return result;
}

struct ScanErrDumper {
  ShAaScanErrMap& errMap;
  ShIndexStmtMap& indexStmt;
  double maxErr; // max error (absolute value) 

  ScanErrDumper(ShAaScanErrMap& errMap, ShIndexStmtMap& indexStmt)
    : errMap(errMap), indexStmt(indexStmt) {
    maxErr = 0;
    // @todo only include statement indices from original program  
    for(ShAaScanErrMap::iterator I = errMap.begin(); I != errMap.end(); ++I) {
      if(indexStmt.find(I->first) == indexStmt.end()) {
        //SH_DEBUG_PRINT_ASE("Ignoring " << *I->second.stmt << " for max err calculation.");
        continue;
      }
      maxErr = std::max(maxErr, I->second.err);
    }
    SH_DEBUG_PRINT_ASE("Max error = " << maxErr);
  }

  void operator()(std::ostream& out, ShSectionNodePtr section)
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

  void operator()(std::ostream& out, ShCtrlGraphNodePtr node)
  {
    out << "[label=<";

    if(node->block) {
      out << "<TABLE BORDER=\"0\">\n";
      for(ShBasicBlock::iterator I = node->block->begin(); I != node->block->end(); ++I) {
        ShStatement& stmt = *I; 
        ShStmtIndex* stmtIndex = stmt.get_info<ShStmtIndex>();

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

ShAaStmtErr::ShAaStmtErr()
  : stmt(0), err(0)
{
}

ShAaStmtErr::ShAaStmtErr(ShStmtIndex index, ShStatement* stmt, double err) 
  : index(index), stmt(stmt), err(err)
{
}

std::ostream& operator<<(std::ostream& out, const ShAaStmtErr& stmterr)
{
  out << stmterr.index.index() << " err=" << stmterr.err 
    << " " << *stmterr.stmt; 
  return out;
}

ShAaScanErrMap shAaScanLightnessErr(ShProgram program)
{
  // Check that program 
  if(program.node()->outputs.size() != 1) {
    SH_DEBUG_PRINT_ASE("Cannot analyze programs with more than one output");
  }

  ShVariable output = *program.outputs_begin();
  if(output.size() != 3 || output.node()->specialType() != SH_COLOR) {
    SH_DEBUG_PRINT_ASE("Cannot handle non-RGB ShColor3x outputs");
  }

  // Add indices
  add_stmt_indices(program);

  ShProgram progClone(program.node()->clone());

  // Tack on RGB->luminance conversion
  progClone = rgbToLightness(output) << progClone;

  // Pass to scan core
  ShAaScanCore scanCore(progClone);

  ShStream input; // null ==> all default bounds

  ShAaScanCore::AaVarVec resultPattern;
  ShRecord resultRecord;
  ShStream result;

  result = scanCore.doit(0, input, resultPattern, resultRecord);

  //fetch one into resultRecord/resultPattern 
  lookup(resultRecord, result, 0);

  SH_DEBUG_PRINT_ASE("Results:");
  for(unsigned int i = 0; i < resultPattern.size(); ++i) {
    SH_DEBUG_PRINT_ASE(i << ": " << resultPattern[i].name() << "=" << resultPattern[i]); 
  }

  // Pull out results into  
  const ShAaVariable& aaOutput = resultPattern.front();
  const ShAaIndexSet& syms = aaOutput.use(0); 

  int i = 0; 
  ShVariable aaErr = aaOutput.err(0);
  ShAaScanErrMap errMap;
  for(ShAaIndexSet::iterator I = syms.begin(); I != syms.end(); ++I, ++i) {
    int index = *I; // noise symbol index
    double err;
    // this is kind of messy with the variants
    ShVariantPtr errVariant = shVariantFactory(SH_DOUBLE, SH_HOST)->
      generate(&err, 1, false);
    errVariant->set(aaErr.getVariant(i));
    err = std::abs(err);
    SH_DEBUG_PRINT_ASE("noise sym " << index << " err " << err);

    // now figure out which stmt this comes from and stick it in the map 
    // (add onto previous error if there is any)
    if(scanCore.isStmtSym(index)) {
      ShAaScanCore::StmtIntPair matchingStmt = scanCore.getStmt(index);
      ShStatement* stmt = matchingStmt.first;
      ShStmtIndex* matchingIdx = stmt->get_info<ShStmtIndex>();
      SH_DEBUG_ASSERT(matchingIdx);

      if(errMap.find(*matchingIdx) != errMap.end()) {
        errMap[*matchingIdx].err += err;
      } else {
        errMap[*matchingIdx] = ShAaStmtErr(*matchingIdx, stmt, err); 
      }
      SH_DEBUG_PRINT_ASE("Updating " << errMap[*matchingIdx]);
    }
  }

  return errMap;
}

void shAaScanErrDump(const std::string& filename, ShAaScanErrMap& errMap, ShProgram program)
{
  ShStructural structural(program.node()->ctrlGraph);
  ShSectionTree sectionTree(structural);

  ShIndexStmtMap indexStmt = gather_indices(program);

  ScanErrDumper sed(errMap, indexStmt);
  std::string dotfile = filename + ".dot";
  std::string psfile = filename + ".ps";

  std::ofstream fout(dotfile.c_str());
  sectionTree.dump(fout, sed); 
  fout.close();

  std::string cmd = std::string("dot -Tps < ") + dotfile + " > " + psfile;
  system(cmd.c_str());
}

}
