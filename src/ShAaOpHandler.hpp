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
#ifndef SHAAOPHANDLER_HPP
#define SHAAOPHANDLER_HPP

#include "ShProgram.hpp"
#include "ShAaVariable.hpp"
#include "ShDllExport.hpp"

namespace SH {

/** @file ShAaOpHandler.hpp
 * Classes used to turn a ShProgram involving affine arithmetic types
 * into an equivalent one involving only non-affine types.  This depends
 * on having syms placed immediately beforehand, as we cannot handle any
 * cfg or stmt altering transformations at this point.
 *
 * @see ShAaSymPlacer.hpp
 */

/** Splits up affine ops in a program and changes inputs/outputs to hold
 * the assigned error symbols.  If you want the error symbol -> statement
 * mappings to remain true, this should be called on a clone of the original
 * program.  The results will still match the original program, but the
 * program structure itself may be transformed significantly.
 *
 *
 *
 * @return whether anything was changed 
 */
SH_DLLEXPORT bool handleAaOps(ShProgramNodePtr programNode);


}

#endif
