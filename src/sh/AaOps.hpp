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

#include "DllExport.hpp"
#include "Program.hpp"
#include "AaSyms.hpp"
#include "AaSymPlacer.hpp"
#include "AaVariable.hpp"

namespace SH {

/** @file AaInstructions.hpp
 * Performs affine operations using regular tuples wrapped in AaVariable
 * objects.  This is used primarily during code generation for platforms 
 * that do not support affine types directly, and handles affine arithmetic
 * without merging of unique symbols only. 
 *
 * In each case, dest must be assigned the correct symbols for the operation
 * before hand.
 *
 * @see AaVariable.hpp
 * @see AaOpHandler.hpp
 */

SH_DLLEXPORT 
AaVariable aaADD(const AaVariable& src0, const AaVariable& src1); 
SH_DLLEXPORT 
AaVariable aaADD(const AaVariable& src0, const Variable& src1); 

SH_DLLEXPORT 
AaVariable aaMUL(const AaVariable& src0, const AaVariable& src1, const AaSyms &newsyms); 
SH_DLLEXPORT 
AaVariable aaMUL(const AaVariable& src0, const Variable& src1);
// @todo range - special case for non-aa src

//SH_DLLEXPORT 
//AaVariable aaDIV(const AaVariable& src0, const AaVariable& src1, const AaSyms &newsyms); 

#if 0 // copied from Instructions.hpp
AaVariable aaASN(const AaVariable& src);
//AaVariable aaNEG(const AaVariable& src);

#endif

AaVariable aaSLT(const AaVariable& a, const AaVariable& b, const AaSyms &newsyms);
AaVariable aaSLE(const AaVariable& a, const AaVariable& b, const AaSyms &newsyms);
AaVariable aaSGT(const AaVariable& a, const AaVariable& b, const AaSyms &newsyms);
AaVariable aaSGE(const AaVariable& a, const AaVariable& b, const AaSyms &newsyms);
AaVariable aaSEQ(const AaVariable& a, const AaVariable& b, const AaSyms &newsyms);
AaVariable aaSNE(const AaVariable& a, const AaVariable& b, const AaSyms &newsyms);


AaVariable aaABS(const AaVariable& a, const AaSyms& newsyms);
AaVariable aaACOS(const AaVariable& a, const AaSyms& newsyms);
AaVariable aaASIN(const AaVariable& a, const AaSyms& newsyms);
AaVariable aaATAN(const AaVariable& a, const AaSyms& newsyms);
#if 0
AaVariable aaATAN2(const AaVariable& a, const AaVariable& b);
AaVariable aaCBRT(const AaVariable& a);
AaVariable aaCEIL(const AaVariable& a);
#endif
AaVariable aaCOS(const AaVariable& a, const AaSyms& newsyms);
#if 0
AaVariable aaCMUL(const AaVariable& a);
#endif
AaVariable aaCSUM(const AaVariable& a);
AaVariable aaDOT(const AaVariable& a, const AaVariable& b, const AaSyms& newsyms);
#if 0
AaVariable aaDX(const AaVariable& a);
AaVariable aaDY(const AaVariable& a);
#endif
AaVariable aaEXP(const AaVariable& a, const AaSyms& newsyms);
AaVariable aaEXP2(const AaVariable& a, const AaSyms& newsyms);
AaVariable aaEXP10(const AaVariable& a, const AaSyms& newsyms);
AaVariable aaFLR(const AaVariable& a, const AaSyms& newsyms);
AaVariable aaFRAC(const AaVariable& a, const AaSyms& newsyms);
AaVariable aaLOG(const AaVariable& a, const AaSyms& newsyms);
AaVariable aaLOG2(const AaVariable& a, const AaSyms& newsyms);
AaVariable aaLOG10(const AaVariable& a, const AaSyms& newsyms);

// @todo range - scalar version 
AaVariable aaLRP(const AaVariable& alpha, const AaVariable& a, 
                   const AaVariable& b, const AaSyms& newsyms);
// @todo range - scalar version 
AaVariable aaMAD(const AaVariable& a, const AaVariable& b, 
                   const AaVariable& c, const AaSyms& newsyms);

AaVariable aaPOS(const AaVariable& a, const AaSyms& newsyms);

SH_DLLEXPORT
AaVariable aaMAX(const AaVariable& a, const AaVariable& b, const AaSyms& newsyms);
SH_DLLEXPORT
AaVariable aaMIN(const AaVariable& a, const AaVariable& b, const AaSyms& newsyms);
#if 0
AaVariable aaMOD(const AaVariable& a, const AaVariable& b);
#endif
AaVariable aaPOW(const AaVariable& a, const AaVariable& b, const AaSyms& newsyms);
AaVariable aaRCP(const AaVariable& a, const AaSyms& newsyms);
#if 0
AaVariable aaRND(const AaVariable& a);
#endif
AaVariable aaRSQ(const AaVariable& a, const AaSyms& newsyms);
#if 0
AaVariable aaSGN(const AaVariable& a);
#endif
AaVariable aaSIN(const AaVariable& a, const AaSyms& newsyms);
AaVariable aaSQRT(const AaVariable& a, const AaSyms& newsyms);
#if 0
AaVariable aaTAN(const AaVariable& a);
#endif

AaVariable aaNORM(const AaVariable& a, const AaSyms& newsyms);

#if 0
AaVariable aaXPD(const AaVariable& a, const AaVariable& b);
#endif

// @todo range shouldn't really use destsyms 
AaVariable aaTEX(const Variable& texVar, const AaVariable& coord, 
                   const AaSyms& destsyms, const AaSyms& newsyms);
AaVariable aaTEXI(const Variable& texVar, const AaVariable& coord, 
                   const AaSyms& destsyms, const AaSyms& newsyms);

#if 0
AaVariable aaCOND(const AaVariable& cond,
            const AaVariable& a, const AaVariable& b);

#endif
// special escaped variable join function
// @todo range will have to make this better in the future to handle
//  a) joint shape on input ranges
//  b) make output an affine approx of section inputs 

AaVariable aaESCJOIN(const AaVariable& a, const AaSyms& destsyms, const AaSyms& newsyms); 

#if 0
// interval arithmetic ops 
AaVariable aaLO(const AaVariable& a);

AaVariable aaHI(const AaVariable& a);

AaVariable aaWIDTH(const AaVariable& a);

AaVariable aaRADIUS(const AaVariable& a);

AaVariable aaCENTER(const AaVariable& a);

#endif
AaVariable aaIVAL(const Variable& a, const Variable& b, 
    const AaSyms& newsyms);

AaVariable aaUNION(const AaVariable& a, const AaVariable& b, 
    const AaSyms& newsyms);

#if 0
AaVariable aaISCT(const AaVariable& a, const AaVariable& b);

AaVariable aaCONTAINS(const AaVariable& a, const AaVariable& b);
#endif
Variable aaLASTERR(const AaVariable& a, const AaSyms& syms); 
#if 0


AaVariable aaKIL(const AaVariable& cond);
#endif

/* Some special operations 
 * @todo range - may want to move these into AaVariable proper*/

// Conversions from and to interval variables
AaVariable aaFROMIVAL(const Variable& a, const AaSyms& newsyms);
Variable aaTOIVAL(const AaVariable& a);

// Conversion from regular variable
AaVariable aaFROMTUPLE(const Variable& a);

// Affine arithmetic unique merging operation
void aaUNIQUE_MERGE(AaVariable& mergeDest, AaVariable& dest, const AaStmtSyms* syms);
}
#endif
