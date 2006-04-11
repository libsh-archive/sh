// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
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

#include "DllExport.hpp"
#include "Variable.hpp"

namespace SH {

/** @defgroup instructions Intermediate-representation instructions
 * You probably don't ever need to call any of these.
 * They are mostly used internally.
 * Instead, use the library functions provided by Lib.hpp.
 * @{
 */

SH_DLLEXPORT
void shASN(Variable& dest, const Variable& src);
//void shNEG(Variable& dest, const Variable& src);
SH_DLLEXPORT
void shADD(Variable& dest, const Variable& a, const Variable& b);
SH_DLLEXPORT
void shMUL(Variable& dest, const Variable& a, const Variable& b);
SH_DLLEXPORT
void shDIV(Variable& dest, const Variable& a, const Variable& b);

SH_DLLEXPORT
void shSLT(Variable& dest, const Variable& a, const Variable& b);
SH_DLLEXPORT
void shSLE(Variable& dest, const Variable& a, const Variable& b);
SH_DLLEXPORT
void shSGT(Variable& dest, const Variable& a, const Variable& b);
SH_DLLEXPORT
void shSGE(Variable& dest, const Variable& a, const Variable& b);
SH_DLLEXPORT
void shSEQ(Variable& dest, const Variable& a, const Variable& b);
SH_DLLEXPORT
void shSNE(Variable& dest, const Variable& a, const Variable& b);

SH_DLLEXPORT
void shABS(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shACOS(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shACOSH(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shASIN(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shASINH(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shATAN(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shATAN2(Variable& dest, const Variable& a, const Variable& b);
SH_DLLEXPORT
void shATANH(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shCBRT(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shCEIL(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shCOS(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shCOSH(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shCMUL(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shCSUM(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shDOT(Variable& dest, const Variable& a, const Variable& b);
SH_DLLEXPORT
void shDX(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shDY(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shEXP(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shEXP2(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shEXP10(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shFLR(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shFRAC(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shLIT(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shLOG(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shLOG2(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shLOG10(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shLRP(Variable& dest, const Variable& alpha,
           const Variable& a, const Variable& b);
SH_DLLEXPORT
void shHASH(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shMAD(Variable& dest, const Variable& a,
           const Variable& b, const Variable& c);
SH_DLLEXPORT
void shMAX(Variable& dest, const Variable& a, const Variable& b);
SH_DLLEXPORT
void shMIN(Variable& dest, const Variable& a, const Variable& b);
SH_DLLEXPORT
void shMOD(Variable& dest, const Variable& a, const Variable& b);
SH_DLLEXPORT
void shNOISE(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shPOW(Variable& dest, const Variable& a, const Variable& b);
SH_DLLEXPORT
void shRCP(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shRND(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shRSQ(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shSGN(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shSIN(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shSINH(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shSQRT(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shTAN(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shTANH(Variable& dest, const Variable& a);

SH_DLLEXPORT
void shNORM(Variable& dest, const Variable& a);
SH_DLLEXPORT
void shXPD(Variable& dest, const Variable& a, const Variable& b);

SH_DLLEXPORT
void shCOND(Variable& dest, const Variable& cond,
            const Variable& a, const Variable& b);
SH_DLLEXPORT
void shKIL(const Variable& cond);
SH_DLLEXPORT
void shRET(const Variable& cond);

/*@}*/

}

#endif
