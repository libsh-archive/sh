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
#ifndef SHAAHIER_HPP
#define SHAAHIER_HPP

#include <vector>
#include <iosfwd>
#include "ShDllExport.hpp"
#include "ShInfo.hpp"
#include "ShStructural.hpp"

namespace SH {

struct ShInEscInfo: public ShInfo
{
  typedef std::vector<ShVariable> VarVec;
  VarVec in, esc;

  friend std::ostream& operator<<(std::ostream& out, const ShInEscInfo& iei); 

  ShInfo* clone() const;
};

/* Adds ShInEscInfo to the STARTSEC, ENDSEC statements.
 * Inserts SH_OP_ESCJOIN operations right before ENDSEC statements (using the 
 * same ShStmtIndex as the ENDSEC stmt) for escaping variables.
 */
SH_DLLEXPORT void inEscAnalysis(ShStructural &structural, ShProgramNodePtr p);


struct ShSectionInfo: public ShInfo
{
  // Constructs a section info for the program scope (no section, level=0)
  ShSectionInfo();

  // Constructions a section info for a particular section
  ShSectionInfo(ShStatement* start, ShStatement* end, int level); 

  int level; ///< nesting level 
  ShStatement *start, *end;

  friend std::ostream& operator<<(std::ostream& out, const ShSectionInfo& si);
  ShInfo* clone() const;
};
/* Adds ShSectionInfo objects to control graph nodes in sections 
 * Inserts SH_OP_ESCJOIN operations right before ENDSEC statements
 * for escaping variables.
 */
SH_DLLEXPORT void addSectionInfo(ShStructural &s, ShProgramNodePtr p);

}

#endif
