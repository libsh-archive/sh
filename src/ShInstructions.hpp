#ifndef SHINSTRUCTIONS_HPP
#define SHINSTRUCTIONS_HPP

#include "ShVariable.hpp"

namespace SH {

/** @defgroup instructions Intermediate-representation instructions
 * You probably don't ever need to call any of these.
 * They are mostly used internally.
 * Instead, use the library functions provided by ShLib.hpp.
 * @{
 */

void shASN(ShVariable& dest, const ShVariable& src);
//void shNEG(ShVariable& dest, const ShVariable& src);
void shADD(ShVariable& dest, const ShVariable& a, const ShVariable& b);
void shMUL(ShVariable& dest, const ShVariable& a, const ShVariable& b);
void shDIV(ShVariable& dest, const ShVariable& a, const ShVariable& b);

void shSLT(ShVariable& dest, const ShVariable& a, const ShVariable& b);
void shSLE(ShVariable& dest, const ShVariable& a, const ShVariable& b);
void shSGT(ShVariable& dest, const ShVariable& a, const ShVariable& b);
void shSGE(ShVariable& dest, const ShVariable& a, const ShVariable& b);
void shSEQ(ShVariable& dest, const ShVariable& a, const ShVariable& b);
void shSNE(ShVariable& dest, const ShVariable& a, const ShVariable& b);

void shABS(ShVariable& dest, const ShVariable& a);
void shACOS(ShVariable& dest, const ShVariable& a);
void shASIN(ShVariable& dest, const ShVariable& a);
void shATAN(ShVariable& dest, const ShVariable& a);
void shATAN2(ShVariable& dest, const ShVariable& a, const ShVariable& b);
void shCEIL(ShVariable& dest, const ShVariable& a);
void shCOS(ShVariable& dest, const ShVariable& a);
void shDOT(ShVariable& dest, const ShVariable& a, const ShVariable& b);
void shEXP(ShVariable& dest, const ShVariable& a);
void shEXP2(ShVariable& dest, const ShVariable& a);
void shEXP10(ShVariable& dest, const ShVariable& a);
void shFLR(ShVariable& dest, const ShVariable& a);
void shFMOD(ShVariable& dest, const ShVariable& a, const ShVariable& b);
void shFRAC(ShVariable& dest, const ShVariable& a);
void shLOG(ShVariable& dest, const ShVariable& a);
void shLOG2(ShVariable& dest, const ShVariable& a);
void shLOG10(ShVariable& dest, const ShVariable& a);
void shLRP(ShVariable& dest, const ShVariable& alpha,
           const ShVariable& a, const ShVariable& b);
void shMAD(ShVariable& dest, const ShVariable& a,
           const ShVariable& b, const ShVariable& c);
void shMAX(ShVariable& dest, const ShVariable& a, const ShVariable& b);
void shMIN(ShVariable& dest, const ShVariable& a, const ShVariable& b);
void shPOW(ShVariable& dest, const ShVariable& a, const ShVariable& b);
void shSIN(ShVariable& dest, const ShVariable& a);
void shSQRT(ShVariable& dest, const ShVariable& a);

void shNORM(ShVariable& dest, const ShVariable& a);
void shXPD(ShVariable& dest, const ShVariable& a, const ShVariable& b);

void shCOND(ShVariable& dest, const ShVariable& cond,
            const ShVariable& a, const ShVariable& b);

void shKIL(const ShVariable& cond);

/*@}*/

}

#endif
