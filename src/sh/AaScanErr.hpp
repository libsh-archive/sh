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
#ifndef SHAASCANERR_HPP
#define SHAASCANERR_HPP

#include <map>
#include <iostream>
#include "DllExport.hpp"
#include "Inclusion.hpp"
#include "Program.hpp"

/** @file AaScanErr.hpp 
 *
 * Uses the functionalities provided by ScanCore to derive an error metric
 * measuring the effect of each statement (and section).
 * 
 */

namespace SH {

struct AaStmtErr {
  AaStmtErr();
  AaStmtErr(StmtIndex index, Statement* stmt, double err);

  friend std::ostream& operator<<(std::ostream& out, const AaStmtErr& stmterr);

  bool operator<(const AaStmtErr& other) { return index < other.index; }

  StmtIndex index;
  Statement* stmt;
  double err;
};
typedef std::map<StmtIndex, AaStmtErr> AaScanErrMap; 

/* Very basic error metric - only handles one RGB output and does no domain
 * splitting. 
 *
 * Error is relative to perceived lightness 
 */
AaScanErrMap aaScanLightnessErr(Program program);

/* @todo 
 * - Handles lightness of one RGB output, splits domain uniformly 
 * - Handles lightness of one RGB output, splits domain, bandpass frequency analysis, CSF
 */

/* Dumps out the program using the given  */   
void aaScanErrDump(const std::string& filename, AaScanErrMap& errMap, Program program);

}

#endif
