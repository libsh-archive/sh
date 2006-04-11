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

DLLEXPORT
void shASN(Variable& dest, const Variable& src);
//void shNEG(Variable& dest, const Variable& src);
DLLEXPORT
void shADD(Variable& dest, const Variable& a, const Variable& b);
DLLEXPORT
void shMUL(Variable& dest, const Variable& a, const Variable& b);
DLLEXPORT
void shDIV(Variable& dest, const Variable& a, const Variable& b);

DLLEXPORT
void shSLT(Variable& dest, const Variable& a, const Variable& b);
DLLEXPORT
void shSLE(Variable& dest, const Variable& a, const Variable& b);
DLLEXPORT
void shSGT(Variable& dest, const Variable& a, const Variable& b);
DLLEXPORT
void shSGE(Variable& dest, const Variable& a, const Variable& b);
DLLEXPORT
void shSEQ(Variable& dest, const Variable& a, const Variable& b);
DLLEXPORT
void shSNE(Variable& dest, const Variable& a, const Variable& b);

DLLEXPORT
void shABS(Variable& dest, const Variable& a);
DLLEXPORT
void shACOS(Variable& dest, const Variable& a);
DLLEXPORT
void shACOSH(Variable& dest, const Variable& a);
DLLEXPORT
void shASIN(Variable& dest, const Variable& a);
DLLEXPORT
void shASINH(Variable& dest, const Variable& a);
DLLEXPORT
void shATAN(Variable& dest, const Variable& a);
DLLEXPORT
void shATAN2(Variable& dest, const Variable& a, const Variable& b);
DLLEXPORT
void shATANH(Variable& dest, const Variable& a);
DLLEXPORT
void shCBRT(Variable& dest, const Variable& a);
DLLEXPORT
void shCEIL(Variable& dest, const Variable& a);
DLLEXPORT
void shCOS(Variable& dest, const Variable& a);
DLLEXPORT
void shCOSH(Variable& dest, const Variable& a);
DLLEXPORT
void shCMUL(Variable& dest, const Variable& a);
DLLEXPORT
void shCSUM(Variable& dest, const Variable& a);
DLLEXPORT
void shDOT(Variable& dest, const Variable& a, const Variable& b);
DLLEXPORT
void shDX(Variable& dest, const Variable& a);
DLLEXPORT
void shDY(Variable& dest, const Variable& a);
DLLEXPORT
void shEXP(Variable& dest, const Variable& a);
DLLEXPORT
void shEXP2(Variable& dest, const Variable& a);
DLLEXPORT
void shEXP10(Variable& dest, const Variable& a);
DLLEXPORT
void shFLR(Variable& dest, const Variable& a);
DLLEXPORT
void shFRAC(Variable& dest, const Variable& a);
DLLEXPORT
void shLIT(Variable& dest, const Variable& a);
DLLEXPORT
void shLOG(Variable& dest, const Variable& a);
DLLEXPORT
void shLOG2(Variable& dest, const Variable& a);
DLLEXPORT
void shLOG10(Variable& dest, const Variable& a);
DLLEXPORT
void shLRP(Variable& dest, const Variable& alpha,
           const Variable& a, const Variable& b);
DLLEXPORT
void shHASH(Variable& dest, const Variable& a);
DLLEXPORT
void shMAD(Variable& dest, const Variable& a,
           const Variable& b, const Variable& c);
DLLEXPORT
void shMAX(Variable& dest, const Variable& a, const Variable& b);
DLLEXPORT
void shMIN(Variable& dest, const Variable& a, const Variable& b);
DLLEXPORT
void shMOD(Variable& dest, const Variable& a, const Variable& b);
DLLEXPORT
void shNOISE(Variable& dest, const Variable& a);
DLLEXPORT
void shPOW(Variable& dest, const Variable& a, const Variable& b);
DLLEXPORT
void shRCP(Variable& dest, const Variable& a);
DLLEXPORT
void shRND(Variable& dest, const Variable& a);
DLLEXPORT
void shRSQ(Variable& dest, const Variable& a);
DLLEXPORT
void shSGN(Variable& dest, const Variable& a);
DLLEXPORT
void shSIN(Variable& dest, const Variable& a);
DLLEXPORT
void shSINH(Variable& dest, const Variable& a);
DLLEXPORT
void shSQRT(Variable& dest, const Variable& a);
DLLEXPORT
void shTAN(Variable& dest, const Variable& a);
DLLEXPORT
void shTANH(Variable& dest, const Variable& a);

DLLEXPORT
void shNORM(Variable& dest, const Variable& a);
DLLEXPORT
void shXPD(Variable& dest, const Variable& a, const Variable& b);

DLLEXPORT
void shCOND(Variable& dest, const Variable& cond,
            const Variable& a, const Variable& b);
DLLEXPORT
void shKIL(const Variable& cond);
DLLEXPORT
void shRET(const Variable& cond);

/*@}*/

}

#endif
