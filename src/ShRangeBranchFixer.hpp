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
#ifndef SHRANGEBRANCHFIXER_HPP
#define SHRANGEBRANCHFIXER_HPP

#include "ShProgram.hpp"

/** @file ShRangeBranchFixer.hpp
 * Function that removes range conditional variables and replace them with non-range variables.  
 * Appropriate mangling using the structural graph ensues. 
 */

namespace SH {

/** Changes conditional branches on range types to branches on non-range types,
 * mangling the control flow graph appropriately.
 *
 * @param p The program to mangle
 * @return whether anything was changed
 *
 * @{ */
SH_DLLEXPORT
bool fixRangeBranches(ShProgramNodePtr p); 

SH_DLLEXPORT
bool fixRangeBranches(ShProgram &p); 
// @}

}

#endif
