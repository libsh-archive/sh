#ifndef SHEVAL_HPP 
#define SHEVAL_HPP

#include <vector>
#include "ShCloak.hpp"
#include "ShRefCount.hpp"
#include "ShStatement.hpp"

namespace SH {

/* Type specific definitions for the internal instructions
 * as they are used in host computations.
 */

class ShEval: public ShRefCountable {
  public:
    virtual ~ShEval();

    /// unary op that has no dest (currently only KIL)
    // TODO this is probably not necessary right now since KIL 
    // makes no sense in immediate mode yet
    virtual void srcUnaryOp(ShOperation op, ShCloakPtr src) const = 0; 

    /// unary operation on cloaked data (properly swizzled/negated)
    virtual void unaryOp(ShOperation op, ShCloakPtr dest, ShCloakCPtr src) const = 0;

    /// binary operation on cloaked data (properly swizzled/negated)
    virtual void binaryOp(ShOperation op, ShCloakPtr dest, 
        ShCloakCPtr a, ShCloakCPtr b) const = 0;

    /// ternary operation on cloaked data (properly swizzled/negated)
    virtual void ternaryOp(ShOperation op, ShCloakPtr dest,
        ShCloakCPtr a, ShCloakCPtr b, ShCloakCPtr c) const = 0;
};

typedef ShPointer<ShEval> ShEvalPtr;
typedef ShPointer<const ShEval> ShEvalCPtr;

/*
 * ShEval is meant to perform an evaluation, lickety split.
 * It does not need to check that std::vectors are the right size
 * other than handling scalar operands.
 *
 * All the tough work figuring out type conversions and
 * applying swizzles/negations to elements is done beforehand. 
 */

template<typename T>
class ShDataEval: public ShEval {
  public:
    typedef std::vector<T> DataVec;

    void srcUnaryOp(ShOperation op, ShCloakPtr src) const; 
    void unaryOp(ShOperation op, ShCloakPtr dest, ShCloakCPtr src) const; 
    void binaryOp(ShOperation op, ShCloakPtr dest, 
        ShCloakCPtr a, ShCloakCPtr b) const; 
    void ternaryOp(ShOperation op, ShCloakPtr dest,
        ShCloakCPtr a, ShCloakCPtr b, ShCloakCPtr c) const; 

    static void ASN(DataVec & dest, const DataVec &src);

    //static void shNEG(DataVec& dest, const DataVec& src);
    static void ADD(DataVec &dest, const DataVec &a, const DataVec &b); 
    static void MUL(DataVec &dest, const DataVec &a, const DataVec &b);
    static void DIV(DataVec &dest, const DataVec &a, const DataVec &b);

    static void SLT(DataVec &dest, const DataVec &a, const DataVec &b);
    static void SLE(DataVec &dest, const DataVec &a, const DataVec &b);
    static void SGT(DataVec &dest, const DataVec &a, const DataVec &b);
    static void SGE(DataVec &dest, const DataVec &a, const DataVec &b);
    static void SEQ(DataVec &dest, const DataVec &a, const DataVec &b);
    static void SNE(DataVec &dest, const DataVec &a, const DataVec &b);

    static void ABS(DataVec & dest, const DataVec &src);
    static void ACOS(DataVec & dest, const DataVec &src);
    static void ASIN(DataVec & dest, const DataVec &src);
    static void ATAN(DataVec & dest, const DataVec &src);
    static void ATAN2(DataVec& dest, const DataVec& a, const DataVec& b);
    static void CEIL(DataVec & dest, const DataVec &src);
    static void COS(DataVec & dest, const DataVec &src);
    static void DOT(DataVec & dest, const DataVec &a, const DataVec& b);
    static void DX(DataVec & dest, const DataVec &src);
    static void DY(DataVec & dest, const DataVec &src);
    static void EXP(DataVec & dest, const DataVec &src);
    static void EXP2(DataVec & dest, const DataVec &src);
    static void EXP10(DataVec & dest, const DataVec &src);
    static void FLR(DataVec & dest, const DataVec &src);
    static void FRAC(DataVec & dest, const DataVec &src);
    static void LOG(DataVec & dest, const DataVec &src);
    static void LOG2(DataVec & dest, const DataVec &src);
    static void LOG10(DataVec & dest, const DataVec &src);
    static void LRP(DataVec& dest, const DataVec& alpha,
               const DataVec& a, const DataVec& b);
    static void MAD(DataVec& dest, const DataVec& a,
               const DataVec& b, const DataVec& c);
    static void MAX(DataVec& dest, const DataVec& a, const DataVec& b);
    static void MIN(DataVec& dest, const DataVec& a, const DataVec& b);
    static void MOD(DataVec& dest, const DataVec& a, const DataVec& b);
    static void POW(DataVec& dest, const DataVec& a, const DataVec& b);
    static void RCP(DataVec& dest, const DataVec& a);
    static void RSQ(DataVec& dest, const DataVec& a);
    static void SGN(DataVec& dest, const DataVec& a);
    static void SIN(DataVec& dest, const DataVec& a);
    static void SQRT(DataVec& dest, const DataVec& a);
    static void TAN(DataVec& dest, const DataVec& a);

    static void NORM(DataVec& dest, const DataVec& a);
    static void XPD(DataVec& dest, const DataVec& a, const DataVec& b);

    static void COND(DataVec& dest, const DataVec& cond,
                const DataVec& a, const DataVec& b);

    static void KIL(const DataVec& cond);

  protected:
    typedef ShDataCloak<T> CloakType;
    typedef const ShDataCloak<T> CloakCType;
    typedef ShPointer<ShDataCloak<T> > CloakPtr;
    typedef ShPointer<const ShDataCloak<T> > CloakCPtr;
};

}

#include "ShEvalImpl.hpp"

#endif
