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
#ifndef SHAAOPS_HPP
#define SHAAOPS_HPP

#include "ShDllExport.hpp"
#include "ShProgram.hpp"
#include "ShAaSyms.hpp"
#include "ShAaVariable.hpp"

namespace SH {

/** @file ShAaInstructions.hpp
 * Performs affine operations using regular tuples wrapped in ShAaVariable
 * objects.  This is used primarily during code generation for platforms 
 * that do not support affine types directly. 
 *
 * In each case, dest must be assigned the correct symbols for the operation
 * before hand.
 *
 * @see ShAaVariable.hpp
 * @see ShAaOpHandler.hpp
 */

SH_DLLEXPORT 
ShAaVariable aaADD(const ShAaVariable& src0, const ShAaVariable& src1); 
SH_DLLEXPORT 
ShAaVariable aaADD(const ShAaVariable& src0, const ShVariable& src1); 

SH_DLLEXPORT 
ShAaVariable aaMUL(const ShAaVariable& src0, const ShAaVariable& src1, const ShAaSyms &newsyms); 
SH_DLLEXPORT 
ShAaVariable aaMUL(const ShAaVariable& src0, const ShVariable& src1);
// @todo range - special case for non-aa src

//SH_DLLEXPORT 
//ShAaVariable aaDIV(const ShAaVariable& src0, const ShAaVariable& src1, const ShAaSyms &newsyms); 

#if 0 // copied from ShInstructions.hpp
ShAaVariable aaASN(const ShAaVariable& src);
//ShAaVariable aaNEG(const ShAaVariable& src);

ShAaVariable aaSLT(const ShAaVariable& a, const ShAaVariable& b);
ShAaVariable aaSLE(const ShAaVariable& a, const ShAaVariable& b);
ShAaVariable aaSGT(const ShAaVariable& a, const ShAaVariable& b);
ShAaVariable aaSGE(const ShAaVariable& a, const ShAaVariable& b);
ShAaVariable aaSEQ(const ShAaVariable& a, const ShAaVariable& b);
ShAaVariable aaSNE(const ShAaVariable& a, const ShAaVariable& b);

ShAaVariable aaABS(const ShAaVariable& a);
ShAaVariable aaACOS(const ShAaVariable& a);
ShAaVariable aaASIN(const ShAaVariable& a);
ShAaVariable aaATAN(const ShAaVariable& a);
ShAaVariable aaATAN2(const ShAaVariable& a, const ShAaVariable& b);
ShAaVariable aaCBRT(const ShAaVariable& a);
ShAaVariable aaCEIL(const ShAaVariable& a);
ShAaVariable aaCOS(const ShAaVariable& a);
ShAaVariable aaCMUL(const ShAaVariable& a);
ShAaVariable aaCSUM(const ShAaVariable& a);
#endif
ShAaVariable aaDOT(const ShAaVariable& a, const ShAaVariable& b, const ShAaSyms& newsyms);
#if 0
ShAaVariable aaDX(const ShAaVariable& a);
ShAaVariable aaDY(const ShAaVariable& a);
ShAaVariable aaEXP(const ShAaVariable& a);
ShAaVariable aaEXP2(const ShAaVariable& a);
ShAaVariable aaEXP10(const ShAaVariable& a);
ShAaVariable aaFLR(const ShAaVariable& a);
ShAaVariable aaFRAC(const ShAaVariable& a);
ShAaVariable aaLOG(const ShAaVariable& a);
ShAaVariable aaLOG2(const ShAaVariable& a);
ShAaVariable aaLOG10(const ShAaVariable& a);
ShAaVariable aaLRP(const ShAaVariable& alpha,
           const ShAaVariable& a, const ShAaVariable& b);
#endif
ShAaVariable aaMAD(const ShAaVariable& a, const ShAaVariable& b, 
                   const ShAaVariable& c, const ShAaSyms& newsyms);
#if 0
ShAaVariable aaMAX(const ShAaVariable& a, const ShAaVariable& b);
ShAaVariable aaMIN(const ShAaVariable& a, const ShAaVariable& b);
ShAaVariable aaMOD(const ShAaVariable& a, const ShAaVariable& b);
ShAaVariable aaPOW(const ShAaVariable& a, const ShAaVariable& b);
ShAaVariable aaRCP(const ShAaVariable& a);
ShAaVariable aaRND(const ShAaVariable& a);
ShAaVariable aaRSQ(const ShAaVariable& a);
ShAaVariable aaSGN(const ShAaVariable& a);
ShAaVariable aaSIN(const ShAaVariable& a);
ShAaVariable aaSQRT(const ShAaVariable& a);
ShAaVariable aaTAN(const ShAaVariable& a);

ShAaVariable aaNORM(const ShAaVariable& a);
ShAaVariable aaXPD(const ShAaVariable& a, const ShAaVariable& b);

ShAaVariable aaCOND(const ShAaVariable& cond,
            const ShAaVariable& a, const ShAaVariable& b);

// interval arithmetic ops 
ShAaVariable aaLO(const ShAaVariable& a);

ShAaVariable aaHI(const ShAaVariable& a);

ShAaVariable aaWIDTH(const ShAaVariable& a);

ShAaVariable aaCENTER(const ShAaVariable& a);

#endif
ShAaVariable aaIVAL(const ShVariable& a, const ShVariable& b, 
    const ShAaSyms& newsyms);
#if 0

ShAaVariable aaUNION(const ShAaVariable& a, const ShAaVariable& b);

ShAaVariable aaISCT(const ShAaVariable& a, const ShAaVariable& b);

ShAaVariable aaCONTAINS(const ShAaVariable& a, const ShAaVariable& b);


ShAaVariable aaKIL(const ShAaVariable& cond);
#endif

/* Some special operations 
 * @todo range - may want to move these into ShAaVariable proper*/

// Conversions from and to interval variables
ShAaVariable aaFROMIVAL(const ShVariable& a, const ShAaSyms& newsyms);
ShVariable aaTOIVAL(const ShAaVariable& a);

// Conversion from regular variable
ShAaVariable aaFROMTUPLE(const ShVariable& a);

}
#endif
