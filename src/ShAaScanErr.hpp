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
#include "ShDllExport.hpp"
#include "ShInclusion.hpp"
#include "ShProgram.hpp"

/** @file ShAaScanErr.hpp 
 *
 * Uses the functionalities provided by ScanCore to derive an error metric
 * measuring the effect of each statement (and section).
 * 
 */

namespace SH {

struct ShAaStmtErr {
  ShAaStmtErr();
  ShAaStmtErr(ShStmtIndex index, ShStatement* stmt, double err);

  friend std::ostream& operator<<(std::ostream& out, const ShAaStmtErr& stmterr);

  bool operator<(const ShAaStmtErr& other) { return index < other.index; }

  ShStmtIndex index;
  ShStatement* stmt;
  double err;
};
typedef std::map<ShStmtIndex, ShAaStmtErr> ShAaScanErrMap; 

/* Very basic error metric - only handles one RGB output and does no domain
 * splitting. 
 *
 * Error is relative to perceived lightness 
 */
ShAaScanErrMap shAaScanLightnessErr(ShProgram program);

/* @todo 
 * - Handles lightness of one RGB output, splits domain uniformly 
 * - Handles lightness of one RGB output, splits domain, bandpass frequency analysis, CSF
 */

}

#endif
