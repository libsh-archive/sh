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
#ifndef SHINSTRUCTIONS_HPP
#define SHINSTRUCTIONS_HPP

#include "ShDllExport.hpp"
#include "ShVariable.hpp"

namespace SH {

/** @defgroup instructions Intermediate-representation instructions
 * You probably don't ever need to call any of these.
 * They are mostly used internally.
 * Instead, use the library functions provided by ShLib.hpp.
 * @{
 */

SH_DLLEXPORT
void shASN(const ShVariable& dest, const ShVariable& src);
//void shNEG(const ShVariable& dest, const ShVariable& src);
SH_DLLEXPORT
void shADD(const ShVariable& dest, const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shMUL(const ShVariable& dest, const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shDIV(const ShVariable& dest, const ShVariable& a, const ShVariable& b);

SH_DLLEXPORT
void shSLT(const ShVariable& dest, const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shSLE(const ShVariable& dest, const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shSGT(const ShVariable& dest, const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shSGE(const ShVariable& dest, const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shSEQ(const ShVariable& dest, const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shSNE(const ShVariable& dest, const ShVariable& a, const ShVariable& b);

SH_DLLEXPORT
void shABS(const ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shACOS(const ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shASIN(const ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shATAN(const ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shATAN2(const ShVariable& dest, const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shCBRT(const ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shCEIL(const ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shCOS(const ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shCMUL(const ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shCSUM(const ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shDOT(const ShVariable& dest, const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shDX(const ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shDY(const ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shEXP(const ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shEXP2(const ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shEXP10(const ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shFLR(const ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shFRAC(const ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shLOG(const ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shLOG2(const ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shLOG10(const ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shLRP(const ShVariable& dest, const ShVariable& alpha,
           const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shMAD(const ShVariable& dest, const ShVariable& a,
           const ShVariable& b, const ShVariable& c);
SH_DLLEXPORT
void shMAX(const ShVariable& dest, const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shMIN(const ShVariable& dest, const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shMOD(const ShVariable& dest, const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shPOW(const ShVariable& dest, const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shRCP(const ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shRND(const ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shRSQ(const ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shSGN(const ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shSIN(const ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shSQRT(const ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shTAN(const ShVariable& dest, const ShVariable& a);

SH_DLLEXPORT
void shNORM(const ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shXPD(const ShVariable& dest, const ShVariable& a, const ShVariable& b);

SH_DLLEXPORT
void shCOND(const ShVariable& dest, const ShVariable& cond,
            const ShVariable& a, const ShVariable& b);

// interval arithmetic ops 
SH_DLLEXPORT
void shLO(const ShVariable& dest, const ShVariable& a);

SH_DLLEXPORT
void shHI(const ShVariable& dest, const ShVariable& a);

SH_DLLEXPORT
void shWIDTH(const ShVariable& dest, const ShVariable& a);

SH_DLLEXPORT
void shRADIUS(const ShVariable& dest, const ShVariable& a);

SH_DLLEXPORT
void shCENTER(const ShVariable& dest, const ShVariable& a);

SH_DLLEXPORT
void shIVAL(const ShVariable& dest, const ShVariable& a, const ShVariable& b);

SH_DLLEXPORT
void shUNION(const ShVariable& dest, const ShVariable& a, const ShVariable& b);

SH_DLLEXPORT
void shISCT(const ShVariable& dest, const ShVariable& a, const ShVariable& b);

SH_DLLEXPORT
void shCONTAINS(const ShVariable& dest, const ShVariable& a, const ShVariable& b);

SH_DLLEXPORT
void shERRFROM(const ShVariable& dest, const ShVariable& a, const ShVariable& b);

SH_DLLEXPORT
void shLASTERR(const ShVariable& dest, const ShVariable& a, const ShVariable& b);


SH_DLLEXPORT
void shKIL(const ShVariable& cond);

/*@}*/

}

#endif
