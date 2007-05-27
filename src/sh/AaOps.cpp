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

#include <string>
#include <cassert>
#include "AaOps.hpp"
#include "Syntax.hpp"
#include "VariableNode.hpp"
#include "Attrib.hpp"
#include "Instructions.hpp"
#include "Transformer.hpp"
#include "CtrlGraphWranglers.hpp"
#include "Optimizations.hpp"
/* @todo get affine textures working again at some point? */
//#include "AffineTexture.hpp"
#include "AaSyms.hpp"
#include "AaSymPlacer.hpp"
#include "AaVariable.hpp"
#include "AaOpHandler.hpp"

#ifdef DBG_AA
#define SH_DEBUG_AAOPS
#endif
// #define SH_DEBUG_AAOPS

#ifdef SH_DEBUG_AAOPS
#define SH_DEBUG_PRINT_AOP(x) { SH_DEBUG_PRINT(x) }
#else
#define SH_DEBUG_PRINT_AOP(X)
#endif

namespace {
using namespace SH;
using namespace std;

ConstAttrib1f ZERO(0.0f);
ConstAttrib1f HALF(0.5f);
ConstAttrib1f ONE(1.0f);
ConstAttrib1f TWO(2.0f);
ConstAttrib1f EPS(1e-6);
ConstAttrib1f INF(1e6); // @todo figure out how to represent this
ConstAttrib1f PI(M_PI);
ConstAttrib1f PI_2(M_PI_2);
ConstAttrib1f PI_INV(M_1_PI);
ConstAttrib1f LN2(M_LN2);
ConstAttrib1f LN10(M_LN10);
ConstAttrib1f LN2_INV(1.0/M_LN2);
ConstAttrib1f LN10_INV(1.0/M_LN10);
ConstAttrib1f LOG2_LN2(-0.5287663729448977); 
ConstAttrib1f LOG10_LN10(0.36221568869946325);

/* Returns a variable holding whether each range includes a given value */
Variable affineHasValue(const AaVariable& a, const Variable& value) { 
  // check if 0 is in the range 
  Variable lo, hi; 
  a.lohi(lo, hi);
  Variable hasValue = a.temp("hasValue");  // whether each tuple element spans value
  Variable lolt  = a.temp("lolt"); 
  Variable loeps  = a.temp("loeps"); 
  Variable higt = a.temp("higt"); 
  Variable hieps = a.temp("hieps"); 

  shADD(loeps, lo, -EPS);
  shADD(hieps, hi, EPS);
   
  shSLT(lolt, loeps, value); 
  shSGT(higt, hieps, value); 
  shMUL(hasValue, lolt, higt);

  return hasValue;
}


Variable affineIsNegative(const AaVariable& a) {
  Variable hi = a.hi();
  Variable hiNegative = a.temp("hiNeg"); 
  shSLT(hiNegative, hi, ZERO);
  return hiNegative;
}

Variable affineMaybeNegative(const AaVariable& a) {
  Variable lo = a.lo();
  Variable loNegative = a.temp("loNeg"); 
  shSLT(loNegative, lo, ZERO);
  return loNegative; 
}


/* Affine approximation of arbitrary dimension.
 * Each src[i] must be the same size as dest
 */
typedef std::vector<const AaVariable*> AaVarVec;
void affineApprox(AaVariable &dest,
                  const AaVarVec &src,
                  const Record& coeff, const AaSyms &newsyms)
{
  // @todo debug
  if(src.size() != coeff.size() - 2) {
    SH_DEBUG_PRINT("src.size = " << src.size() << " coeff.size() - 2 = " << (coeff.size() - 2));
  }
  SH_DEBUG_ASSERT(src.size() == coeff.size() - 2);
  SH_DEBUG_ASSERT(src.size() > 0);

  Record::const_iterator C = coeff.begin();
  dest.ASN(aaMUL(*src[0], *C++));
  AaSyms assigned = src[0]->use(); // keeps track of symbols assigned to so far

  for(size_t i = 1; i < src.size(); ++i, ++C) {
    AaSyms add = src[i]->use() - assigned;
    AaSyms isct = src[i]->use() & assigned;

    SH_DEBUG_ASSERT(src[i]->size() == dest.size());

    dest.ASN(aaMUL(*src[i], *C), add, false);
    dest.MAD(*src[i], *C, isct, true);

    assigned |= add;
  }

  dest.ADD(*C++); // const factor
  if((assigned & newsyms).empty()) {
    dest.setErr(*C, newsyms);
  } else {
    dest.addErr(*C, newsyms);
  }
}

// Affine approximation
// Detects if src already has newsyms, in which case it adds the error on
// as an absolute value
void affineApprox(AaVariable &dest, const AaVariable &src, 
                  Variable alpha, Variable beta, 
                  Variable delta, const AaSyms &newsyms)
{
  AaVarVec srcVec;
  srcVec.push_back(&src);
  affineApprox(dest, srcVec, alpha & beta & delta, newsyms);
}


// Affine approximation 
// Detects if a or b already has newsyms, in which case it adds the error on
// as an absolute value
void affineApprox(AaVariable &dest,
                  const AaVariable &a, const AaVariable &b,
                  Variable alpha, Variable beta, Variable gamma, 
                  Variable delta, const AaSyms &newsyms)
{
  AaVarVec srcVec;
  srcVec.push_back(&a);
  srcVec.push_back(&b);
  affineApprox(dest, srcVec, alpha & beta & gamma & delta, newsyms);
}



/* Given a functor F that defines three functions similar to Affine.hpp's
 * convexApprox (except the arguments are now Variable tuples),
 * this does line for line the same thing.
 *
 * @see Affine.hpp
 */
template<bool FDF=false, bool FIX=false>
struct __aaop {
  static const bool use_fdf = FDF; 
  static void f(Variable& r, Variable& x) {} 
  static void dfinv(Variable& r, Variable& x, Variable& lo, Variable& hi) {}
  static void fdfinv(Variable& r, Variable& x, Variable& lo, Variable& hi) { SH_DEBUG_ASSERT(false);} 
  static const bool fix_range = FIX; 
  static void fix(Variable& lo, Variable& hi) { SH_DEBUG_ASSERT(false); }
};

template<typename F>
void convexApprox(AaVariable &dest, const AaVariable &src, const AaSyms &newsyms)
{
  // Translated directly from AffineImpl.hpp
  Variable lo, hi; 
  src.lohi(lo, hi);

  /* @todo this shouldn't be here...  */
  shADD(lo, lo, -EPS.repeat(lo.size()));
  shADD(hi, hi, EPS.repeat(hi.size()));

  if(F::fix_range) {
    F::fix(lo, hi); /* compute the approximation only on this range */
  }

  Variable flo = src.temp("fhi"); 
  Variable fhi = src.temp("flo"); 
  F::f(flo, lo);
  F::f(fhi, hi);

  Variable alpha, ss, bpd, bmd, beta, delta; 
  alpha = src.temp("alpha");

  //Variable width = src.width(); 
  Variable width = src.temp("width");
  shADD(width, hi, -lo); 
  shMAX(width, width, EPS.repeat(width.size()));

  Variable fhiflo = src.temp("fhi-flo");
  shADD(fhiflo, fhi, -flo);
  shDIV(alpha, fhiflo, width);
  ss = src.temp("ss");
  F::dfinv(ss, alpha, lo, hi);

  //if(lo > ss || hi < ss) std::cout << "  WARNING - ss out of bounds" << std::endl;
  bmd = src.temp("bmd"); 
  shMAD(bmd, -alpha, lo, flo);

  bpd = src.temp("bpd");
  Variable fdfinv = src.temp("fdfinv");
  if(F::use_fdf) {
    F::fdfinv(fdfinv, alpha, lo, hi);
  } else {
    F::f(fdfinv, ss);
  }
  shMAD(bpd, -alpha, ss, fdfinv); 

  beta = src.temp("beta");
  shLRP(beta, HALF, bpd, bmd);
  delta = src.temp("delta");
  shLRP(delta, HALF, bpd, -bmd);

  shABS(delta, delta);
  affineApprox(dest, src, alpha, beta, delta, newsyms);
}

/* Return the union of the two syms */
AaSyms mergeSyms(const AaVariable& a, const AaVariable& b) 
{
  SH_DEBUG_ASSERT(a.size() == b.size());
  int size = a.size();
  AaSyms result(size);
  for(int i = 0; i < size; ++i) {
    result[i] = a.use(i);
    result[i] |= b.use(i);
  }
  return result;
}

/* Return the union of the two syms with additional syms */
AaSyms mergeSyms(const AaVariable& a, const AaVariable& b, const AaSyms &addSyms) 
{
  AaSyms result = mergeSyms(a, b);
  result |= addSyms;
  return result;
}

AaVariableNodePtr makeMergeNode(const string& name, const AaVariable& a,
    const AaVariable& b)
{
  return new AaVariableNode(name, SH_TEMP,
        a.valueType(), a.specialType(), mergeSyms(a, b));
}

AaVariableNodePtr makeMergeNode(const string& name, const AaVariable& a,
    const AaVariable& b, const AaSyms &addSyms)
{
  return new AaVariableNode(name, SH_TEMP,
        a.valueType(), a.specialType(), mergeSyms(a, b, addSyms));
}
}
namespace SH {

// @todo range - remove ZEROS, and properly determine sym set intersections,
// differences

// @todo range - determine result value types properly
AaVariable aaADD(const AaVariable& a, const AaVariable& b)
{
  AaVariable result(makeMergeNode("aopADDt", a, b));
  AaSyms isct = a.use() & b.use();
  AaSyms bonly = b.use() - a.use(); 
  SH_DEBUG_PRINT_AOP("Set Ops: a=" << a.use() << " b=" << b.use() 
      << " isct=" << isct << " bonly=" << bonly);
  result.ASN(a);
  result.ASN(b, bonly, false);
  result.ADD(b, isct, true); 
  return result;
}

AaVariable aaADD(const AaVariable& a, const Variable& b)
{
  AaVariable result(a.clone());
  result.name("aopADDvart");
  return result.ADD(b);
}

/* Sets dest = a * b */
AaVariable aaMUL(const AaVariable &a, const AaVariable &b,
           const AaSyms &newsyms)
{
  string mulName = "aopMULt";
  AaVariable result(makeMergeNode(mulName, a, b, newsyms));

  // @see AffineImpl.hpp
  // This is just a vectorized copy of operator* from AffineImpl
  Variable alpha(result.temp(mulName + "alpha"));
  shASN(alpha, b.center()); 

  Variable beta(result.temp(mulName + "beta"));
  shASN(beta, a.center()); 

  Variable gamma(result.temp(mulName + "gamma"));
  Variable delta(result.temp(mulName + "delta"));

  shMUL(delta, a.radius(), b.radius());
  affineApprox(result, a, b, alpha, beta, gamma, delta, newsyms);
  Variable resultCenter = result.center();
  shMUL(resultCenter, alpha, beta);
  return result;
}

AaVariable aaMUL(const AaVariable &a, const Variable &b)
{
  AaVariable result(a.clone());
  result.name("aopMULvart");
  return result.MUL(b);
}

AaVariable aaLRP(const AaVariable &a, const AaVariable &b,
           const AaVariable &c, const AaSyms &newsyms)
{
  return aaMAD(a, aaADD(b, c.NEG()), c, newsyms);
}

AaVariable aaMAD(const AaVariable &a, const AaVariable &b,
           const AaVariable &c, const AaSyms &newsyms)
{
  AaVariable ab = aaMUL(a, b, newsyms);
  return aaADD(ab, c);
}

AaVariable aaPOS(const AaVariable& a, const AaSyms& newsyms)
{
  Variable aCenter = a.center();
  Variable aRadius = a.radius(); 

  Variable aLo = a.temp(a.name() + "_lo");
  Variable aHi = a.temp(a.name() + "_hi");
  shADD(aLo, aCenter, -aRadius);
  shADD(aHi, aCenter, aRadius);

  AaVariable result(new AaVariableNode(*a.node(), a.use() | newsyms));
  result.name("aopPOSt");
  result.ASN(a);

  // if lo < 0 && hi > 0, scale radius to hi / 2 and center = hi / 2
  // if hi <= 0, set to 0
  // else keep

  Variable loLtZero = a.temp(a.name() + "_lo-le-0");
  shSLT(loLtZero, aLo, ZERO.repeat(aLo.size()));

  AaVariable scaledResult(result.clone());
  Variable scaling = a.temp(a.name() + "_scaling");
  Variable halfHi = a.temp(a.name() + "_half_hi");

  shMUL(halfHi, aHi, HALF.repeat(aHi.size()));

  Variable scaledResultCenter = scaledResult.center();
  shASN(scaledResultCenter, halfHi);
  scaledResult.setErr(halfHi, newsyms);
  result.COND(loLtZero, scaledResult);

  Variable hiLtZero = a.temp(a.name() + "_hi-lt-0");
  shSLT(hiLtZero, aHi, ZERO.repeat(aHi.size()));
  result.COND(hiLtZero, ZERO.repeat(a.size()));

  return result;
}

AaVariable aaMAX(const AaVariable& a, const AaVariable& b, const AaSyms& newsyms)
{
  return aaADD(aaPOS(aaADD(a, b.NEG()), newsyms), b); 
}

AaVariable aaMIN(const AaVariable& a, const AaVariable& b, const AaSyms& newsyms)
{
  return aaADD(a, aaPOS(aaADD(a, b.NEG()), newsyms).NEG()); 
}

AaVariable aaNORM(const AaVariable& a, const AaSyms& newsyms)
{
  // need to do some weird stuff to get the right syms
  AaSyms mulSyms = newsyms.last();
  AaSyms scalarSyms = (newsyms - mulSyms).swizSyms(Swizzle(1));
  AaVariable normSq = aaDOT(a, a, scalarSyms);
  AaVariable invnorm = aaRSQ(normSq, scalarSyms);
  return aaMUL(a, normSq.repeat(a.size()), mulSyms);
}

AaVariable aaPOW(const AaVariable& a, const AaVariable& b, const AaSyms& newsyms)
{
  return aaEXP(aaMUL(aaLOG(a, newsyms), b, newsyms), newsyms);
}

/* Make sure 0 is not in the range first */
struct __aaop_rcp: public __aaop<true> {
  static void f(Variable& r, Variable& x) { shRCP(r, x); }
  static void dfinv(Variable& r, Variable& x, Variable& lo, Variable& hi) { 
    shRSQ(r, -x); 
    Variable rSgtHi(r.node()->clone(SH_TEMP, r.size()));
    shSGT(rSgtHi, r, hi);
    shCOND(r, rSgtHi, -r, r);
  }
  static void fdfinv(Variable& r, Variable& x, Variable& lo, Variable& hi) { 
    Variable invr(r.node()->clone(SH_TEMP, r.size()));
    shRSQ(invr, -x);
    shSQRT(r, -x); 
    Variable needFlip(r.node()->clone(SH_TEMP, r.size()));
    shSGT(needFlip, invr, hi);
    shCOND(r, needFlip, -r, r);
  }
};


AaVariable aaRCP(const AaVariable& a, const AaSyms& newsyms)
{
  AaVariable result(new AaVariableNode(*a.node(), a.use() | newsyms));
  result.name("aopRCPt");

  // check if 0 is in the range 
  Variable hasZero = affineHasValue(a, ZERO); 
  Attrib1f SH_DECL(someZero);  // whether any tuple element spans 0

  // @todo should really use the unary max function in LibClamp (but that won't work with Variable) 
  shASN(someZero, hasZero(0));
  for(int i = 1; i < hasZero.size(); ++i) {
    shMAX(someZero, someZero, hasZero(i));
  }
  
  // @todo this is rather inefficient
  SH_IF(someZero) {
    int asize  = a.size();
    for(int i = 0; i < asize; ++i) {
      AaSyms newsymsi = newsyms.swizSyms(Swizzle(asize, i));
      SH_IF(hasZero(i)) {
        Variable centeri = result.center()(i);
        shASN(centeri, ZERO);

        Variable erri = result.err(i);
        shASN(erri, ZERO.repeat(erri.size()));
        
        result(i).addErr(INF, newsymsi);
      } SH_ELSE {
        AaVariable resulti = result(i);
        convexApprox<__aaop_rcp>(resulti, a(i), newsymsi); // @todo range - fix this
      } SH_ENDIF;
    }
  } SH_ELSE {
    convexApprox<__aaop_rcp>(result, a, newsyms); // @todo range - fix this
  } SH_ENDIF;

  return result;
}

struct __aaop_rsq: public __aaop<> {
  static void f(Variable& r, Variable& x) { shRSQ(r, x); }
  static void dfinv(Variable& r, Variable& x, Variable& lo, Variable& hi) { 
    shMUL(r, x, ConstAttrib1f(-2.0f).repeat(r.size())); 
    shPOW(r, r, ConstAttrib1f(-2.0f/3.0f).repeat(r.size()));
  }
};


AaVariable aaRSQ(const AaVariable& a, const AaSyms& newsyms)
{
  AaVariable result(new AaVariableNode(*a.node(), a.use() | newsyms));
  result.name("aopRSQt");

  Variable hasZero = affineHasValue(a, ZERO); 
  hasZero.name("hasZero");

  SH_IF(hasZero) {
    result.ZERO();
    result.setErr(INF, newsyms);
  } SH_ELSE {
    convexApprox<__aaop_rsq>(result, a, newsyms); 
  } SH_ENDIF;
  return result;
}

struct __aaop_sin: public __aaop<> {
  static void f(Variable& r, Variable& x) { shSIN(r, x); }
  static void dfinv(Variable& r, Variable& x, Variable& lo, Variable& hi) { 
    /* r \in [0, PI] */
    shACOS(r, x);

    /* put result in between lo and hi */
    Variable offset(r.node()->clone(SH_TEMP)); /* how much to offset */
    Variable flip(r.node()->clone(SH_TEMP));
    Variable flipr(r.node()->clone(SH_TEMP));
    Variable half = HALF.repeat(r.size());
    Variable zero = ZERO.repeat(r.size());
    Variable pi = PI.repeat(r.size());
    Variable pi_inv = PI_INV.repeat(r.size());

    shMUL(offset, lo, pi_inv); 
    shFLR(offset, offset);

    shMUL(flip, offset, half); 
    shFRAC(flip, flip);
    shABS(flip, flip);
    shSEQ(flip, flip, half); 
    shADD(flipr, pi, -r);
    shCOND(r, flip, flipr, r);

    shMUL(offset, offset, pi); 
    shADD(r, r, offset);
  }
};

AaVariable aaSIN(const AaVariable& a, const AaSyms& newsyms) {
  AaVariable result(new AaVariableNode(*a.node(), a.use() | newsyms));
  result.name("aopSINt");
  int N = a.size();
  Variable zero = ZERO.repeat(N);
  Variable half = HALF.repeat(N);
  Variable one = ONE.repeat(N);
  Variable two = TWO.repeat(N);

  /* Figure out if we are in a convex section */
  Variable lo, hi; 
  a.lohi(lo, hi);
  Variable flr_lo = a.temp("aaSIN_loflr");
  Variable flr_hi = a.temp("aaSIN_hiflr");
  Variable frac_lo = a.temp("aaSIN_lofrac");
  Variable frac_hi = a.temp("aaSIN_hifrac");
  Variable flr_diff = a.temp("aaSIN_flr_diff");
  Variable isNonconvex = a.temp("aaSIN_isNonconvex"); 
  Variable isSamePeriod = a.temp("aaSIN_isSamePeriod"); /* lo, hi in same period - chance result should not be [-1, 1] */
  Variable pi_inv = PI_INV.repeat(N);

  shMUL(flr_lo, lo, pi_inv); 
  shFRAC(frac_lo, flr_lo);
  shFLR(flr_lo, flr_lo); 

  shMUL(flr_hi, hi, pi_inv); 
  shFRAC(frac_hi, flr_hi);
  shFLR(flr_hi, flr_hi); 

  shSNE(isNonconvex, flr_lo, flr_hi);

  shADD(flr_diff, flr_hi, -flr_lo);
  shSEQ(isSamePeriod, flr_diff, one); 

  convexApprox<__aaop_sin>(result, a, newsyms); 

  /* if in the same period, compute the interval result 
   * if frac_lo <= 0.5, then the min/max in it's period is included in the range
   * if frac_hi >= 0.5, then similarly the min/max in it's period is included in the range */
  AaVariable resultSamePeriod(new AaVariableNode(*a.node(), a.use() | newsyms));
  Variable flo = a.temp("flo");
  Variable fhi = a.temp("fhi");
  Variable lo_extrema = a.temp("lo_extrema");
  Variable lo_cond = a.temp("lo_cond");
  Variable hi_extrema = a.temp("hi_extrema");
  Variable hi_cond = a.temp("hi_cond");
  Variable minf = a.temp("minf");
  Variable maxf = a.temp("maxf");
  shSIN(flo, lo);
  shSIN(fhi, hi);

  shMOD(lo_extrema, flr_lo, two); 
  shABS(lo_extrema, lo_extrema);
  shCOND(lo_extrema, lo_extrema, -one, one); 
  shSLT(lo_cond, frac_lo, half); 

  shMOD(hi_extrema, flr_hi, two); 
  shABS(hi_extrema, hi_extrema);
  shCOND(hi_extrema, hi_extrema, -one, one); 
  shSGT(hi_cond, frac_hi, half); 

  shCOND(flo, lo_cond, lo_extrema, flo); 
  shCOND(fhi, hi_cond, hi_extrema, fhi); 

  shMIN(minf, flo, fhi);
  shMAX(maxf, flo, fhi);

  resultSamePeriod.name("aopSINsp");
  resultSamePeriod.ZERO();
  Variable rspCenter = resultSamePeriod.center();
  shADD(rspCenter, minf, maxf);
  shMUL(rspCenter, rspCenter, HALF);
  Variable rspDelta = a.temp("rspDelta");
  shADD(rspDelta, maxf, -minf);
  shMUL(rspDelta, rspDelta, half);
  resultSamePeriod.setErr(rspDelta, newsyms);

  /* otherwise, it's not going to work out, just use 0 + 1 \varepsilon_n */ 

  AaVariable resultNonconvex(new AaVariableNode(*a.node(), a.use() | newsyms));
  resultNonconvex.name("aopSINnct");
  resultNonconvex.ZERO();
  Variable resultCenter = resultNonconvex.center();
  shASN(resultCenter, zero);
  resultNonconvex.setErr(one, newsyms);

  result.COND(isNonconvex, resultNonconvex);
  result.COND(isSamePeriod, resultSamePeriod);

  return result;
}

struct __aaop_sqrt: public __aaop<true, true> {
  typedef Variable V; 
  static void f(Variable& r, Variable& x) { shSQRT(r, x); }
  static void dfinv(Variable& r, Variable& x, Variable& lo, Variable& hi) { 
    shMUL(r, x, x); 
    shMUL(r, r, ConstAttrib1f(4.0f));
    shRCP(r, r);
  }
  static void fdfinv(Variable& r, Variable& x, Variable& lo, Variable& hi) { 
    shRCP(r, x); 
    shMUL(r, r, HALF);
  }

  static void fix(Variable& lo, Variable& hi) { 
    shMAX(lo, lo, ZERO.repeat(lo.size()));
    //assume hi is okay
  }
};


AaVariable aaSQRT(const AaVariable& a, const AaSyms& newsyms)
{
  // deal with bounds < 0 case...
  AaVariable result(new AaVariableNode(*a.node(), a.use() | newsyms));
  result.name("aopSQRTt");
  SH_IF(affineIsNegative(a)) { // we're screwed - return some invalid form 
    Variable resultCenter = result.center();
    shASN(resultCenter, INF.repeat(result.size())); 
    result.setErr(ZERO, newsyms);
  } SH_ELSE {
    convexApprox<__aaop_sqrt>(result, a, newsyms); 
  } SH_ENDIF;
  return result;
}

struct __aaop_acos: public __aaop<false, true> {
  static void f(Variable& r, Variable& x) { shACOS(r, x); }
  static void dfinv(Variable& r, Variable& x, Variable& lo, Variable& hi) { 
    shMUL(r, x, x); 
    shRCP(r, r);
    Variable one = ONE.repeat(r.size());
    shADD(r, one, -r); 
    shSQRT(r, r);
    Variable signlo(r.node()->clone(SH_TEMP));
    shSGN(signlo, lo);
    shMUL(r, r, signlo);
  }
  static void fix(Variable& lo, Variable& hi) {
    shMAX(lo, lo, -ONE.repeat(lo.size()));
    shMAX(hi, hi, -ONE.repeat(hi.size()));
    shMIN(lo, lo, ONE.repeat(lo.size()));
    shMIN(hi, hi, ONE.repeat(hi.size()));
  }
};

AaVariable aaACOS(const AaVariable& a, const AaSyms& newsyms) {
  AaVariable result(new AaVariableNode(*a.node(), a.use() | newsyms));
  result.name("aopACOSt");

  Variable zero = ZERO.repeat(a.size());
  Variable pi = PI.repeat(a.size());
  Variable pi2 = PI_2.repeat(a.size());
  Variable isNonconvex = affineHasValue(a, zero); 
  isNonconvex.name("isNonconvex");

  convexApprox<__aaop_acos>(result, a, newsyms);

  AaVariable resultNonconvex(new AaVariableNode(*a.node(), a.use() | newsyms));
  result.name("aopACOSnct");
  resultNonconvex.ZERO();
  Variable resultCenter = resultNonconvex.center();
  shASN(resultCenter, pi2);
  resultNonconvex.setErr(pi2, newsyms);

  result.COND(isNonconvex, resultNonconvex);
  return result;
}

struct __aaop_asin: public __aaop<false, true> {
  static void f(Variable& r, Variable& x) { shASIN(r, x); }
  static void dfinv(Variable& r, Variable& x, Variable& lo, Variable& hi) { 
    shMUL(r, x, x); 
    shRCP(r, r);
    Variable one = ONE.repeat(r.size());
    shADD(r, one, -r); 
    shSQRT(r, r);
    Variable signlo(r.node()->clone(SH_TEMP));
    shSGN(signlo, lo);
    shMUL(r, r, signlo);
  }
  static void fix(Variable& lo, Variable& hi) {
    shMAX(lo, lo, -ONE.repeat(lo.size()));
    shMAX(hi, hi, -ONE.repeat(hi.size()));
    shMIN(hi, hi, ONE.repeat(hi.size()));
    shMIN(lo, lo, ONE.repeat(lo.size()));
  }
};

AaVariable aaASIN(const AaVariable& a, const AaSyms& newsyms) {
  AaVariable result(new AaVariableNode(*a.node(), a.use() | newsyms));
  result.name("aopASINt");

  Variable zero = ZERO.repeat(a.size());
  Variable pi = PI.repeat(a.size());
  Variable pi2 = PI_2.repeat(a.size());
  Variable isNonconvex = affineHasValue(a, zero); 
  isNonconvex.name("isNonconvex");


  convexApprox<__aaop_asin>(result, a, newsyms); 

  AaVariable resultNonconvex(new AaVariableNode(*a.node(), a.use() | newsyms));
  result.name("aopASINnct");
  resultNonconvex.ZERO();
  Variable resultCenter = resultNonconvex.center();
  shASN(resultCenter, zero);
  resultNonconvex.setErr(pi2, newsyms);

  result.COND(isNonconvex, resultNonconvex);
  return result;
}

struct __aaop_atan: public __aaop<> {
  static void f(Variable& r, Variable& x) { shATAN(r, x); }
  static void dfinv(Variable& r, Variable& x, Variable& lo, Variable& hi) { 
    shRCP(r, x);
    Variable one = ONE.repeat(r.size());
    shADD(r, r, -one); 
    shSQRT(r, r);
    Variable signlo(r.node()->clone(SH_TEMP));
    shSGN(signlo, lo);
    shMUL(r, r, signlo);
  }
};

AaVariable aaATAN(const AaVariable& a, const AaSyms& newsyms) {
  AaVariable result(new AaVariableNode(*a.node(), a.use() | newsyms));
  result.name("aopATANt");

  Variable zero = ZERO.repeat(a.size());
  Variable pi = PI.repeat(a.size());
  Variable pi2 = PI_2.repeat(a.size());

  Variable isNonconvex = affineHasValue(a, zero); 
  isNonconvex.name("isNonconvex");

  convexApprox<__aaop_atan>(result, a, newsyms); 

  AaVariable resultNonconvex(new AaVariableNode(*a.node(), a.use() | newsyms));
  result.name("aopATANnct");
  resultNonconvex.ZERO();
  Variable resultCenter = resultNonconvex.center();
  shASN(resultCenter, zero);
  resultNonconvex.setErr(pi2, newsyms);

  result.COND(isNonconvex, resultNonconvex);
  return result;
}

AaVariable aaSLT(const AaVariable& a, const AaVariable& b, const AaSyms &newsyms) {
  string name = "aopSLTt";
  AaVariable result(new AaVariableNode(*a.node(), newsyms));
  result.name(name);

  Variable alo, ahi, blo, bhi; 
  a.lohi(alo, ahi);
  b.lohi(blo, bhi);

  /* three possible cases */
  Variable yes = a.temp("aaSLTyes");
  Variable no = a.temp("aaSLTno");
  Variable maybe = a.temp("aaSLTmaybe");

  shSLT(yes, ahi, blo);
  shSGE(no, alo, bhi); 
  shADD(maybe, yes, no);
  shSEQ(maybe, maybe, ZERO.repeat(result.size()));

  Variable resultCenter = result.center(); 
  shCOND(resultCenter, yes, ONE.repeat(result.size()), HALF.repeat(result.size()));
  shCOND(resultCenter, no, ZERO.repeat(result.size()), resultCenter); 
  Variable delta = a.temp("aaSLTdelta");
  shCOND(delta, maybe, HALF.repeat(result.size()), ZERO.repeat(result.size()));
  result.ASN(delta, newsyms);
  return result;
}

AaVariable aaSLE(const AaVariable& a, const AaVariable& b, const AaSyms &newsyms) {
  string name = "aopSLEt";
  AaVariable result(new AaVariableNode(*a.node(), newsyms));
  result.name(name);

  Variable alo, ahi, blo, bhi; 
  a.lohi(alo, ahi);
  b.lohi(blo, bhi);

  /* three possible cases */
  Variable yes = a.temp("aaSLEyes");
  Variable no = a.temp("aaSLEno");
  Variable maybe = a.temp("aaSLEmaybe");

  shSLE(yes, ahi, blo);
  shSGT(no, alo, bhi); 
  shADD(maybe, yes, no);
  shSEQ(maybe, maybe, ZERO.repeat(result.size()));

  Variable resultCenter = result.center(); 
  shCOND(resultCenter, yes, ONE.repeat(result.size()), HALF.repeat(result.size()));
  shCOND(resultCenter, no, ZERO.repeat(result.size()), resultCenter); 
  Variable delta = a.temp("aaSLEdelta");
  shCOND(delta, maybe, HALF.repeat(result.size()), ZERO.repeat(result.size()));
  result.ASN(delta, newsyms);
  return result;
}
AaVariable aaSGT(const AaVariable& a, const AaVariable& b, const AaSyms &newsyms) {
  string name = "aopSGTt";
  AaVariable result(new AaVariableNode(*a.node(), newsyms));
  result.name(name);

  Variable alo, ahi, blo, bhi; 
  a.lohi(alo, ahi);
  b.lohi(blo, bhi);

  /* three possible cases */
  Variable yes = a.temp("aaSGTyes");
  Variable no = a.temp("aaSGTno");
  Variable maybe = a.temp("aaSGTmaybe");

  shSGT(yes, alo, bhi);
  shSLE(no, ahi, blo); 
  shADD(maybe, yes, no);
  shSEQ(maybe, maybe, ZERO.repeat(result.size()));

  Variable resultCenter = result.center(); 
  shCOND(resultCenter, yes, ONE.repeat(result.size()), HALF.repeat(result.size()));
  shCOND(resultCenter, no, ZERO.repeat(result.size()), resultCenter); 
  Variable delta = a.temp("aaSGTdelta");
  shCOND(delta, maybe, HALF.repeat(result.size()), ZERO.repeat(result.size()));
  result.ASN(delta, newsyms);
  return result;
}

AaVariable aaSGE(const AaVariable& a, const AaVariable& b, const AaSyms &newsyms) {
  string name = "aopSGEt";
  AaVariable result(new AaVariableNode(*a.node(), newsyms));
  result.name(name);

  Variable alo, ahi, blo, bhi; 
  a.lohi(alo, ahi);
  b.lohi(blo, bhi);

  /* three possible cases */
  Variable yes = a.temp("aaSGEyes");
  Variable no = a.temp("aaSGEno");
  Variable maybe = a.temp("aaSGEmaybe");

  shSGE(yes, alo, bhi);
  shSLT(no, ahi, blo); 
  shADD(maybe, yes, no);
  shSEQ(maybe, maybe, ZERO.repeat(result.size()));

  Variable resultCenter = result.center(); 
  shCOND(resultCenter, yes, ONE.repeat(result.size()), HALF.repeat(result.size()));
  shCOND(resultCenter, no, ZERO.repeat(result.size()), resultCenter); 
  Variable delta = a.temp("aaSGEdelta");
  shCOND(delta, maybe, HALF.repeat(result.size()), ZERO.repeat(result.size()));
  result.ASN(delta, newsyms);
  return result;
}

AaVariable aaSEQ(const AaVariable& a, const AaVariable& b, const AaSyms &newsyms) {
  SH_DEBUG_ASSERT(false);
}

AaVariable aaSNE(const AaVariable& a, const AaVariable& b, const AaSyms &newsyms) {
  SH_DEBUG_ASSERT(false);
}

AaVariable aaABS(const AaVariable& a, const AaSyms& newsyms) {
  AaVariable result(new AaVariableNode(*a.node(), a.use() | newsyms));
  result.name("aopABSt");

  Variable lo, hi, hiNeg, loPos, spansZero; 
  a.lohi(lo, hi);
  hiNeg = a.temp("aopABShiNeg");
  loPos = a.temp("aopABSloPos");
  spansZero = a.temp("aopABSspansZero");

  shSLE(hiNeg, hi, ZERO.repeat(hi.size()));
  shSGE(loPos, lo, ZERO.repeat(lo.size()));
  shADD(spansZero, hiNeg, loPos);
  shADD(spansZero, ONE.repeat(spansZero.size()), -spansZero);

  result.ZERO();
  result.ASN(a);
  result.COND(hiNeg, a.NEG());

  AaVariable zeroResult(new AaVariableNode(*a.node(), a.use() | newsyms));
  Variable width, alpha, beta, delta;
  width = a.temp("aopABSwidth");
  alpha = a.temp("aopABSalpha");
  beta = a.temp("aopABSbeta");
  delta = a.temp("aopABSdelta");

  shADD(width, hi, -lo); 
  shADD(alpha, hi, lo); // lo < 0, abs(lo) = -lo, so abs(hi) - abs(lo) = hi + lo 
  shDIV(alpha, alpha, width);
  shMAD(beta, -alpha, hi, hi);
  shMUL(beta, beta, HALF.repeat(beta.size()));
  shASN(delta, beta);

  affineApprox(zeroResult, a, alpha, beta, delta, newsyms);
  result.COND(spansZero, zeroResult); 
  return result;
}

AaVariable aaCOS(const AaVariable& a, const AaSyms& newsyms) {
  AaVariable acopy(new AaVariableNode(*a.node(), a.use()));
  acopy.name("aopCOSt");
  acopy.ASN(a);
  Variable acCenter = acopy.center();
  shADD(acCenter, acCenter, PI_2.repeat(acCenter.size())); 
  return aaSIN(acopy, newsyms);
}

AaVariable aaCSUM(const AaVariable &a)
{
  if(a.size() == 1) return a;
  AaVariable result = aaADD(a(0), a(1)); 
  result.name("aopCSUMt");
  for(int i = 2; i < a.size(); ++i) {
    result = aaADD(result, a(i));
  }
  return result;
}

AaVariable aaDOT(const AaVariable &a, const AaVariable &b,
           const AaSyms &newsyms)
{
  SH_DEBUG_ASSERT(a.size() == b.size());
  AaVariable result = aaMUL(a(0), b(0), newsyms);
  result.name("aopDOTt");
  for(int i = 1; i < a.size(); ++i) {
    // @todo range - should use MAD, but then we just need to change
    // some of the functions to allow adding to existing error symbols
    // (after taking absolute values of them...)
    result = aaADD(result, aaMUL(a(i), b(i), newsyms));
  }
  return result;
}

struct __aaop_exp: public __aaop<true> {
  static void f(Variable& r, Variable& x) { shEXP(r, x); }
  static void dfinv(Variable& r, Variable& x, Variable& lo, Variable& hi) { shLOG(r, x); }
  static void fdfinv(Variable& r, Variable& x, Variable& lo, Variable& hi) { shASN(r, x); }
};


AaVariable aaEXP(const AaVariable& a, const AaSyms& newsyms)
{
  AaVariable result(new AaVariableNode(*a.node(), a.use() | newsyms));
  result.name("aopEXPt");
  convexApprox<__aaop_exp>(result, a, newsyms);
  return result;
}

struct __aaop_exp2: public __aaop<true> {
  static void f(Variable& r, Variable& x) { shEXP2(r, x); }
  static void dfinv(Variable& r, Variable& x, Variable& lo, Variable& hi) { 
    shMUL(r, x, LN2_INV.repeat(x.size()));
    shLOG2(r, r); 
  }
  static void fdfinv(Variable& r, Variable& x, Variable& lo, Variable& hi) { 
    shMUL(r, x, LN2_INV.repeat(x.size()));
  }
};

AaVariable aaEXP2(const AaVariable& a, const AaSyms& newsyms)
{
  AaVariable result(new AaVariableNode(*a.node(), a.use() | newsyms));
  result.name("aopEXP2t");
  convexApprox<__aaop_exp2>(result, a, newsyms); 
  return result;
}

struct __aaop_exp10: public __aaop<true> {
  static void f(Variable& r, Variable& x) { shEXP10(r, x); }
  static void dfinv(Variable& r, Variable& x, Variable& lo, Variable& hi) { 
    shMUL(r, x, LN10_INV.repeat(x.size()));
    shLOG10(r, r); 
  }
  static void fdfinv(Variable& r, Variable& x, Variable& lo, Variable& hi) { 
    shMUL(r, x, LN10_INV.repeat(x.size()));
  }
};

AaVariable aaEXP10(const AaVariable& a, const AaSyms& newsyms)
{
  AaVariable result(new AaVariableNode(*a.node(), a.use() | newsyms));
  result.name("aopEXP10t");
  convexApprox<__aaop_exp10>(result, a, newsyms); // @todo range - fix this
  return result;
}

AaVariable aaFLR(const AaVariable& a, const AaSyms& newsyms)
{
  // A) if between n, n + 1 then set to n 
  // B) if not between 
  //    otherwise, best approx is (a - 0.5) 
  Variable lo, hi; 
  a.lohi(lo, hi);

  Variable flr_lo = a.temp("aaFLR_loflr");
  Variable keep = a.temp("aaFLR_keep");
  shFLR(flr_lo, lo); 
  shADD(keep, hi, -flr_lo);
  Variable one = ONE.repeat(a.size());
  shSGE(keep, keep, one); 

  // come up with the two options
  AaVariable result(new AaVariableNode(*a.node(), a.use() | newsyms));
  result.name("aopFLRt");
  result.ZERO();
  Variable resultCenter = result.center();
  shASN(resultCenter, flr_lo);

  Variable half = HALF.repeat(a.size());
  AaVariable resultB(new AaVariableNode(*a.node(), a.use() | newsyms));
  result.name("aopFLRbt");
  resultB.ASN(a).ADD(-half).setErr(half, newsyms);

  result.COND(keep, resultB);
  return result;
}

AaVariable aaFRAC(const AaVariable& a, const AaSyms& newsyms)
{
  // A) if between n, n + 1 then 
  //  set center to be frac(center), no err
  // B) if not, best approx is 0.5, err = 0.5
  Variable lo, hi; 
  a.lohi(lo, hi);

  Variable flr_lo = a.temp("aaFRAC_loflr");
  Variable keep = a.temp("aaFRAC_keep");
  shFLR(flr_lo, lo); 
  shADD(keep, hi, -flr_lo);
  Variable one = ONE.repeat(a.size());
  shSGE(keep, keep, one); 

  // come up with the two options
  AaVariable result(new AaVariableNode(*a.node(), a.use() | newsyms));
  result.name("aopFRACt");
  result.ZERO().ASN(a);
  Variable resultCenter = result.center();
  shFRAC(resultCenter, resultCenter);


  Variable half = HALF.repeat(a.size());
  AaVariable resultB(new AaVariableNode(*a.node(), a.use() | newsyms));
  result.name("aopFRACbt");
  resultB.ZERO().setErr(half, newsyms);
  Variable resultBCenter = resultB.center();
  shASN(resultBCenter, half);

  result.COND(keep, resultB);
  return result;
}

struct __aaop_log: public __aaop<true> {
  static void f(Variable& r, Variable& x) { shLOG(r, x); }
  static void dfinv(Variable& r, Variable& x, Variable& lo, Variable& hi) { shRCP(r, x); }
  static void fdfinv(Variable& r, Variable& x, Variable& lo, Variable& hi) { 
    shLOG(r, x); 
    shASN(r, -r);
  }
};


AaVariable aaLOG(const AaVariable& a, const AaSyms& newsyms)
{
  AaVariable result(new AaVariableNode(*a.node(), a.use() | newsyms));
  result.name("aopLOGt");
  SH_IF(affineMaybeNegative(a)) {
    Variable resultCenter = result.center();
    shASN(resultCenter, INF.repeat(result.size())); 
    result.setErr(ZERO, newsyms);
  } SH_ELSE {
    convexApprox<__aaop_log>(result, a, newsyms); // @todo range - fix this
  } SH_ENDIF;
  return result;
}

struct __aaop_log2: public __aaop_log {
  static void f(Variable& r, Variable& x) { shLOG2(r, x); }
  static void dfinv(Variable& r, Variable& x, Variable& lo, Variable& hi) { 
    shRCP(r, x); 
    shMUL(r, r, LN2_INV.repeat(r.size()));
  }
  static void fdfinv(Variable& r, Variable& x, Variable& lo, Variable& hi) { 
    shLOG2(r, x); 
    shASN(r, -r);
    shADD(r, r, -LOG2_LN2.repeat(r.size()));
  }
};


AaVariable aaLOG2(const AaVariable& a, const AaSyms& newsyms)
{
  AaVariable result(new AaVariableNode(*a.node(), a.use() | newsyms));
  result.name("aopLOG2t");
  SH_IF(affineMaybeNegative(a)) {
    Variable resultCenter = result.center();
    shASN(resultCenter, INF.repeat(result.size())); 
    result.setErr(ZERO, newsyms);
  } SH_ELSE {
    convexApprox<__aaop_log2>(result, a, newsyms); // @todo range - fix this
  } SH_ENDIF;
  return result;
}

struct __aaop_log10: public __aaop_log {
  static void f(Variable& r, Variable& x) { shLOG10(r, x); }
  static void dfinv(Variable& r, Variable& x, Variable& lo, Variable& hi) { 
    shRCP(r, x); 
    shMUL(r, r, LN10_INV.repeat(r.size()));
  }
  static void fdfinv(Variable& r, Variable& x, Variable& lo, Variable& hi) { 
    shLOG10(r, x); 
    shASN(r, -r);
    shADD(r, r, -LOG10_LN10.repeat(r.size()));
  }
};


AaVariable aaLOG10(const AaVariable& a, const AaSyms& newsyms)
{
  AaVariable result(new AaVariableNode(*a.node(), a.use() | newsyms));
  result.name("aopLOG10t");
  SH_IF(affineMaybeNegative(a)) {
    Variable resultCenter = result.center();
    shASN(resultCenter, INF.repeat(result.size())); 
    result.setErr(ZERO, newsyms);
  } SH_ELSE {
    convexApprox<__aaop_log10>(result, a, newsyms); // @todo range - fix this
  } SH_ENDIF;
  return result;
}

AaVariable aaTEX(const Variable& texVar, const AaVariable& coord, const AaSyms& destsyms, const AaSyms& newsyms)
{
  assert(false);
  AaVariable result(new AaVariableNode(texVar.node(), destsyms)); 
  result.name("aopTEXt");

#if 0
  TextureNodePtr texnode = shref_dynamic_cast<TextureNode>(texVar.node());
  SH_DEBUG_ASSERT(texnode);
  //AffineTexturePtr aatex = AffineTexture::convert(texnode);
  SH_DEBUG_ASSERT(aatex);

  Record coeff(aatex->lookup(coord.lo(), coord.hi()));

  AaVarVec srcVec;
  for(int i = 0; i < coord.size(); ++i) {
    srcVec.push_back(new AaVariable(coord(i).repeat(result.size())));
  }

  affineApprox(result, srcVec, coeff, newsyms); 

  for(int i = 0; i < coord.size(); ++i) {
    delete srcVec[i];
  }
#endif
  return result;
}

AaVariable aaTEXI(const Variable& texVar, const AaVariable& coord, 
                    const AaSyms& destsyms, const AaSyms& newsyms)
{
  assert(false);
  AaVariable result(new AaVariableNode(texVar.node(), destsyms)); 
  result.name("aopTEXIt");

#if 0
  TextureNodePtr texnode = shref_dynamic_cast<TextureNode>(texVar.node());
  SH_DEBUG_ASSERT(texnode);
  //AffineTexturePtr aatex = AffineTexture::convert(texnode);
  assert(false);
  SH_DEBUG_ASSERT(aatex);

  Record coeff(aatex->rect_lookup(coord.lo(), coord.hi()));

  AaVarVec srcVec;
  for(int i = 0; i < coord.size(); ++i) {
    srcVec.push_back(new AaVariable(coord(i).repeat(result.size())));
  }

  affineApprox(result, srcVec, coeff, newsyms); 

  for(int i = 0; i < coord.size(); ++i) {
    delete srcVec[i];
  }
#endif
  return result;
}

AaVariable aaESCJOIN(const AaVariable& a, const AaSyms& destsyms, const AaSyms& newsyms) 
{
  // only keep the nonhier syms, and merge the rest
  AaVariable result(new AaVariableNode(*a.node(), destsyms));
  result.name("aopESCJOINt");
  AaSyms isct = a.use() & destsyms; 
  AaSyms joinSyms = a.use() - destsyms;
  result.ASN(a, isct, true); 

  Variable joinerr = result.temp(a.name() + "_joinerr");
  for(int i = 0; i < a.size(); ++i) {
    Variable erri = a.err(i, joinSyms[i]);
    Variable absErri = a.node()->makeTemp(erri.size(), "_jointemp");
    shABS(absErri, erri); 

    Variable joinerri = joinerr(i); 
    shCSUM(joinerri, absErri); 
  }
  result.setErr(joinerr, newsyms);
  return result;
}

AaVariable aaIVAL(const Variable& a, const Variable& b, 
    const AaSyms& newsyms) 
{
  SH_DEBUG_ASSERT(a.size() == b.size());
  Variable iaValue(a.node()->clone(SH_TEMP, a.size(), 
        intervalValueType(a.valueType())));
  shIVAL(iaValue, a, b);
  return aaFROMIVAL(iaValue, newsyms);
}

Variable aaLASTERR(const AaVariable& a, const AaSyms& syms)
{
  // @todo range handl case where syms is empty
  SH_DEBUG_ASSERT(!syms[0].empty());
  int idx = syms[0].last();
  Variable result(a.temp(a.name() + "_lasterr"));
  for(int i = 0; i < a.size(); ++i) {
    Variable resulti = result(i);
    shASN(resulti, a.err(i, idx)); 
  }
  return result;
}

AaVariable aaFROMIVAL(const Variable& a, const AaSyms& newsyms)
{
  AaVariable result(new AaVariableNode(a.node(), newsyms,
      SH_TEMP, "_ia2aa"));
  // split into lo/hi 
  Variable alo = result.temp(a.name() + "_lo");
  Variable ahi = result.temp(a.name() + "_hi");
  shLO(alo, a);
  shHI(ahi, a);

  // assign lo + hi / 2 to center
  Variable resultCenter = result.center();
  shLRP(resultCenter, HALF, ahi, alo);

  // assign hi - lo / 2 to the input error symbol
  Variable aerr = result.temp(a.name() + "_err");
  shLRP(aerr, HALF, ahi, -alo);
  return result.setErr(aerr, newsyms); 
}

AaVariable aaUNION(const AaVariable& a, const AaVariable& b, const AaSyms& newsyms)
{
  SH_DEBUG_PRINT("a.size=" << a.size() << " b.size=" << b.size());
  AaVariable result(makeMergeNode("aopUNIONt", a, b, newsyms));
  Variable delta = result.temp("aaUNIONdelta");

  AaSyms common = a.use() & b.use();
  shADD(delta, a.center(), -b.center());
  shABS(delta, delta);
  for(int i = 0; i < a.size(); ++i) {
    SH_DEBUG_PRINT("i=" << i << " common=" << common[i]);
    if(common[i].empty()) continue;
    Variable aerri = a.err(i, common[i]);
    Variable berri = b.err(i, common[i]);
    Variable temp(aerri.node()->clone(SH_TEMP, aerri.size()));
    Variable sum(temp.node()->clone(SH_TEMP, 1));
    shADD(temp, aerri, -berri); 
    shABS(temp, temp);
    shCSUM(sum, temp);
    Variable deltai = delta(i);
    shADD(deltai, deltai, sum); 
  }
  shMUL(delta, delta, HALF.repeat(result.size()));

  /* take an average of a & b for center and common syms,
   * add abs differences to delta */
  result.ZERO();
  result.ADD(a);
  result.ADD(b);
  result.MUL(HALF.repeat(result.size()));
  result.setErr(delta, newsyms);
  return result;
}

Variable aaTOIVAL(const AaVariable& a)
{
  Variable result(a.center().node()->clone(SH_TEMP, 0,
        intervalValueType(a.center().valueType())));
  result.name(a.name() + "_2ia");

  Variable aRadius = a.radius(); 
  Variable aCenter = a.center(); 
  Variable aLo = a.temp(a.name() + "_lo");
  Variable aHi = a.temp(a.name() + "_hi");
  shADD(aLo, aCenter, -aRadius);
  shADD(aHi, aCenter, aRadius);
  shIVAL(result, aLo, aHi);
  return result;
}

AaVariable aaFROMTUPLE(const Variable& a)
{
  AaVariable result(new AaVariableNode(a.node(), AaSyms(a.size()),
      SH_TEMP, "_2aa"));
  Variable resultCenter = result.center();
  shASN(resultCenter, a);
  return result;
}

void aaUNIQUE_MERGE(AaVariable& mergeDest, AaVariable& dest, const AaStmtSyms* syms)
{
  AaSyms passThrough = syms->mergeDest - syms->mergeRep; 
  mergeDest.ASN(dest, passThrough);

  for(int i = 0; i < dest.size(); ++i) {
    if(syms->mergeRep[i].empty()) {
      continue;
    }
    // need an ABS on unique, ASN to mergerep in mergeDest
    // assign rest from dest to mergeDest
    AaIndexSet usi = syms->unique[i] & syms->dest[i];
    Variable uniquei = dest.err(i, usi);
    Variable absUniquei(uniquei.node()->clone(SH_TEMP, uniquei.size()));
    Variable mergei = mergeDest.err(i, syms->mergeRep[i]);

    shABS(absUniquei, uniquei);
    shCSUM(mergei, absUniquei);
  }
}


}

