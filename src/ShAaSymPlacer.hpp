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
#ifndef SHAASYMPLACER_HPP
#define SHAASYMPLACER_HPP

#include <vector>
#include <map>
#include <iosfwd>
#include "ShInfo.hpp"
#include "ShDllExport.hpp"
#include "ShVariableNode.hpp"
#include "ShStatement.hpp"
#include "ShProgramNode.hpp"
#include "ShInclusion.hpp"
#include "ShAaSyms.hpp"

namespace SH {

/** @file SymPlacer.hpp
 * Staticaly assigns error symbols to the affine variables in 
 * an ShProgram.
 */

/** Holds symbol information for a statement */     
struct ShAaStmtSyms: public ShInfo {
  unsigned int level; // nesting level of this statement
  ShStatement *stmt;
  typedef std::vector<ShAaSyms> SymsVec; 

  // New symbols added for this statements non-affine  
  ShAaSyms newdest; 

  // Symbols required at dest for this computation 
  ShAaSyms dest;

  // Symbols for sources
  SymsVec src;

  // Generates empty syms sets with sizes based on stmt 
  ShAaStmtSyms(int level, ShStatement* stmt);

  ShInfo* clone() const; 

  friend std::ostream& operator<<(std::ostream& out, const ShAaStmtSyms& stmtSyms);
};

typedef std::map<ShVariableNodePtr, ShAaSyms> ShAaVarSymsMap;
std::ostream& operator<<(std::ostream& out, const ShAaVarSymsMap& vsmap);

typedef std::map<ShStmtIndex, ShAaSyms> ShAaStmtSymsMap;
std::ostream& operator<<(std::ostream& out, const ShAaStmtSymsMap& ssmap);

typedef std::set<ShVariableNodePtr> ShAaVarSet;

/** Holds symbol information for a ShProgramNode 
 * inputs holds symbols assigned for input variables at the start of program
 * vars holds union of all symbols assigned for a variable over the whole program 
 * outputs holds symbols assigned for output variables at the end of program
 *           
 */
struct ShAaProgramSyms: public ShInfo {
  ShAaVarSymsMap inputs, vars, outputs;
  ShAaStmtSymsMap stmts;

  // need association between extra outputs added for hierarchy,
  // and the symbols they save


  ShInfo* clone() const;

  friend std::ostream& operator<<(std::ostream& out, const ShAaProgramSyms& stmtSyms);
};

/** Places syms per statement and on affine program inputs/outputs.
 *
 * No further transformations should take place between this call
 * and compilation or affine-to-regular type transformation, otherwise
 * the error symbol assignments may become invalid.
 *
 * @todo range - worry about branches...when do we call fixRangeBranches?
 *
 * @param programNode program to place syms on
 * @param inputs Input symbol assignment to use.  Any affine input not in
 * this map, will be assigned a single unique error symbol per tuple element. 
 *               
 */
SH_DLLEXPORT 
void placeAaSyms(ShProgramNodePtr programNode);
SH_DLLEXPORT 
void placeAaSyms(ShProgramNodePtr programNode, const ShAaVarSymsMap& inputs);

/** Destroys all syms-related ShInfo objects. */
SH_DLLEXPORT
void clearAaSyms(ShProgramNodePtr programNode);

}

#endif
