// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
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
void shASN(ShVariable& dest, const ShVariable& src);
//void shNEG(ShVariable& dest, const ShVariable& src);
SH_DLLEXPORT
void shADD(ShVariable& dest, const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shMUL(ShVariable& dest, const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shDIV(ShVariable& dest, const ShVariable& a, const ShVariable& b);

SH_DLLEXPORT
void shSLT(ShVariable& dest, const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shSLE(ShVariable& dest, const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shSGT(ShVariable& dest, const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shSGE(ShVariable& dest, const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shSEQ(ShVariable& dest, const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shSNE(ShVariable& dest, const ShVariable& a, const ShVariable& b);

SH_DLLEXPORT
void shABS(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shACOS(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shACOSH(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shASIN(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shASINH(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shATAN(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shATAN2(ShVariable& dest, const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shATANH(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shCBRT(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shCEIL(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shCOS(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shCOSH(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shCMUL(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shCSUM(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shDOT(ShVariable& dest, const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shDX(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shDY(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shEXP(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shEXP2(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shEXP10(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shFLR(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shFRAC(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shLIT(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shLOG(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shLOG2(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shLOG10(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shLRP(ShVariable& dest, const ShVariable& alpha,
           const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shHASH(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shMAD(ShVariable& dest, const ShVariable& a,
           const ShVariable& b, const ShVariable& c);
SH_DLLEXPORT
void shMAX(ShVariable& dest, const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shMIN(ShVariable& dest, const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shMOD(ShVariable& dest, const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shNOISE(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shPOW(ShVariable& dest, const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shRCP(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shRND(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shRSQ(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shSGN(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shSIN(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shSINH(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shSQRT(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shTAN(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shTANH(ShVariable& dest, const ShVariable& a);

SH_DLLEXPORT
void shNORM(ShVariable& dest, const ShVariable& a);
SH_DLLEXPORT
void shXPD(ShVariable& dest, const ShVariable& a, const ShVariable& b);

SH_DLLEXPORT
void shCOND(ShVariable& dest, const ShVariable& cond,
            const ShVariable& a, const ShVariable& b);
SH_DLLEXPORT
void shKIL(const ShVariable& cond);
SH_DLLEXPORT
void shRET(const ShVariable& cond);

/*@}*/

}

#endif
