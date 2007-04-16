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
template<typename F>
void convexApprox(AaVariable &dest, const AaVariable &src, const AaSyms &newsyms, bool pos=false)
{
  // Translated directly from AffineImpl.hpp
  Variable lo, hi; 
  src.lohi(lo, hi);

  if(pos) {
    // EPS should really depend on the type of the variable and the platform.
    // Here we just pick something that should be representable even for
    // half-floats.
    Variable EPS = ConstAttrib1f(1e-7f); 
    shMAX(lo, lo, EPS.repeat(lo.size()));

    // @todo range make sure hi > lo...may not always be what we want though?
    shMAX(hi, lo, hi); 
  }
  Variable flo = src.temp("fhi"); 
  Variable fhi = src.temp("flo"); 
  F::f(flo, lo);
  F::f(fhi, hi);

  Variable alpha, ss, bpd, bmd, beta, delta; 
  alpha = src.temp("alpha");

  Variable hilo = src.temp("hi-lo");
  Variable fhiflo = src.temp("fhi-flo");
  shADD(hilo, hi, -lo);
  shADD(fhiflo, fhi, -flo);
  shDIV(alpha, fhiflo, hilo);
  ss = src.temp("ss");
  F::dfinv(ss, alpha, lo, hi);

  //if(lo > ss || hi < ss) std::cout << "  WARNING - ss out of bounds" << std::endl;
  bmd = src.temp("bmd"); 
  shMAD(bmd, -alpha, lo, flo);

  bpd = src.temp("bpd");
  Variable fdfinv = src.temp("fdfinv");
  if(F::use_fdf) {
    F::fdfinv(fdfinv, alpha);
  } else {
    F::f(fdfinv, ss);
  }
  shMAD(bpd, -alpha, ss, fdfinv); 

  beta = src.temp("beta");
  shLRP(beta, ConstAttrib1f(0.5f), bpd, bmd);
  delta = src.temp("delta");
  shLRP(delta, ConstAttrib1f(0.5f), bpd, -bmd);

  shABS(delta, delta);
  return affineApprox(dest, src, alpha, beta, delta, newsyms);
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

AaVariableNodePtr makeMergeNode(const char* name, const AaVariable& a,
    const AaVariable& b)
{
  return new AaVariableNode(name, SH_TEMP,
        a.valueType(), a.specialType(), mergeSyms(a, b));
}

AaVariableNodePtr makeMergeNode(const char* name, const AaVariable& a,
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
  return result.ADD(b);
}

/* Sets dest = a * b */
AaVariable aaMUL(const AaVariable &a, const AaVariable &b,
           const AaSyms &newsyms)
{
  AaVariable result(makeMergeNode("aopMULt", a, b, newsyms));

  // @see AffineImpl.hpp
  // This is just a vectorized copy of operator* from AffineImpl
  Variable alpha(result.temp("aopMULalpha"));
  shASN(alpha, b.center()); 

  Variable beta(result.temp("aopMULbeta"));
  shASN(beta, a.center()); 

  Variable gamma(result.temp("aopMULgamma"));
  Variable delta(result.temp("aopMULdelta"));

  // @todo range - this is bad numerically
  // should just assign the new center to this
  //shMUL(gamma, -alpha, beta);
  shMUL(delta, a.radius(), b.radius());
  affineApprox(result, a, b, alpha, beta, gamma, delta, newsyms);
  Variable resultCenter = result.center();
  shMUL(resultCenter, alpha, beta);
  return result;
}

AaVariable aaMUL(const AaVariable &a, const Variable &b)
{
  AaVariable result(a.clone());
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

  // if lo < 0 && hi > 0, scale radius to hi / 2 and center = hi / 2
  // if hi <= 0, set to 0
  // else keep
  ConstAttrib1f ZERO(0.0f);
  ConstAttrib1f HALF(0.5f);

  Variable loLtZero = a.temp(a.name() + "_lo-le-0");
  shSLT(loLtZero, aLo, ZERO.repeat(aLo.size()));
  //AaVariable scaledResult(a.clone());
  AaVariable scaledResult(result.clone());
  Variable scaling = a.temp(a.name() + "_scaling");
  Variable halfHi = a.temp(a.name() + "_half_hi");

  shMUL(halfHi, aHi, HALF.repeat(aHi.size()));

  // @todo range fix
#if 0
  shRCP(scaling, aRadius);
  shMUL(scaling, scaling, halfHi);
  scaledResult.MUL(scaling);
  shASN(scaledResult.center(), halfHi);
#endif
  Variable scaledResultCenter = scaledResult.center();
  shASN(scaledResultCenter, halfHi);
  scaledResult.setErr(halfHi, newsyms);
  result.COND(loLtZero, scaledResult);

  Variable hiLeZero = a.temp(a.name() + "_hi-lt-0");
  shSLT(hiLeZero, aHi, ZERO.repeat(aHi.size()));
  result.COND(hiLeZero, ZERO.repeat(a.size()));

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

struct __aaop_rcp {
  typedef Variable V; 
  static const bool use_fdf = false;
  static void f(V r, V x) { shRCP(r, x); }
  static void dfinv(V r, V x, V lo, V hi) { 
    shRSQ(r, -x); 
    V rSgtHi(r.node()->clone(SH_TEMP, r.size()));
    shSGT(rSgtHi, r, hi);
    shCOND(r, rSgtHi, -r, r);
  }
  static void fdfinv(V r, V x) { shSQRT(r, -x); }
};


AaVariable aaPOW(const AaVariable& a, const AaVariable& b, const AaSyms& newsyms)
{
  return aaEXP(aaMUL(aaLOG(a, newsyms), b, newsyms), newsyms);
}

AaVariable aaRCP(const AaVariable& a, const AaSyms& newsyms)
{
  AaVariable result(new AaVariableNode(*a.node(), a.use() | newsyms));

  // check if 0 is in the range 
  Variable lo, hi; 
  a.lohi(lo, hi);
  Variable hasZero = a.temp("hasZero");  // whether each tuple element spans 0
  Attrib1f SH_DECL(someZero);  // whether any tuple element spans 0
  Variable lolt  = a.temp("lolt"); 
  Variable higt = a.temp("higt"); 
  ConstAttrib1f ZERO(0.0f);
  ConstAttrib1f INF(1e6); // @todo figure out how to represent this

  shSLT(lolt, lo, ZERO);
  shSGT(higt, hi, ZERO);
  shMUL(hasZero, lolt, higt);
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

struct __aaop_rsq {
  typedef Variable V; 
  static const bool use_fdf = false;
  static void f(V r, V x) { shRSQ(r, x); }
  static void dfinv(V r, V x, V lo, V hi) { 
    shMUL(r, x, ConstAttrib1f(-2.0f)); 
    shPOW(r, r, ConstAttrib1f(-2.0f/3.0f));
  }
  static void fdfinv(V r, V x) {}
};


AaVariable aaRSQ(const AaVariable& a, const AaSyms& newsyms)
{
  AaVariable result(new AaVariableNode(*a.node(), a.use() | newsyms));
  convexApprox<__aaop_rsq>(result, a, newsyms, true); 
  return result;
}

struct __aaop_sqrt {
  typedef Variable V; 
  static const bool use_fdf = true;
  static void f(V r, V x) { shSQRT(r, x); }
  static void dfinv(V r, V x, V lo, V hi) { 
    shMUL(r, x, x); 
    shMUL(r, r, ConstAttrib1f(4.0f));
    shRCP(r, r);
  }
  static void fdfinv(V r, V x) { 
    shRCP(r, x); 
    shMUL(r, r, ConstAttrib1f(0.5f));
  }
};


AaVariable aaSQRT(const AaVariable& a, const AaSyms& newsyms)
{
  // deal with bounds < 0 case...
  AaVariable result(new AaVariableNode(*a.node(), a.use() | newsyms));
  convexApprox<__aaop_sqrt>(result, a, newsyms, true); 
  return result;
}

AaVariable aaCSUM(const AaVariable &a)
{
  if(a.size() == 1) return a;
  AaVariable result = aaADD(a(0), a(1)); 
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
  for(int i = 1; i < a.size(); ++i) {
    // @todo range - should use MAD, but then we just need to change
    // some of the functions to allow adding to existing error symbols
    // (after taking absolute values of them...)
    result = aaADD(result, aaMUL(a(i), b(i), newsyms));
  }
  return result;
}

struct __aaop_exp {
  typedef Variable V; 
  static const bool use_fdf = true;
  static void f(V r, V x) { shEXP(r, x); }
  static void dfinv(V r, V x, V lo, V hi) { shLOG(r, x); }
  static void fdfinv(V r, V x) { shASN(r, x); }
};


AaVariable aaEXP(const AaVariable& a, const AaSyms& newsyms)
{
  AaVariable result(new AaVariableNode(*a.node(), a.use() | newsyms));
  convexApprox<__aaop_exp>(result, a, newsyms);
  return result;
}

struct __aaop_exp2 {
  typedef Variable V; 
  static const bool use_fdf = true;
  static void f(V r, V x) { shEXP2(r, x); }
  static void dfinv(V r, V x, V lo, V hi) { shLOG2(r, x); }
  static void fdfinv(V r, V x) { shASN(r, x); }
};

AaVariable aaEXP2(const AaVariable& a, const AaSyms& newsyms)
{
  AaVariable result(new AaVariableNode(*a.node(), a.use() | newsyms));
  convexApprox<__aaop_exp2>(result, a, newsyms); 
  return result;
}

struct __aaop_exp10 {
  typedef Variable V; 
  static const bool use_fdf = true;
  static void f(V r, V x) { shEXP10(r, x); }
  static void dfinv(V r, V x, V lo, V hi) { shLOG10(r, x); }
  static void fdfinv(V r, V x) { shASN(r, x); }
};

AaVariable aaEXP10(const AaVariable& a, const AaSyms& newsyms)
{
  AaVariable result(new AaVariableNode(*a.node(), a.use() | newsyms));
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
  Variable one = ConstAttrib1f(1.0f).repeat(a.size());
  shSGE(keep, keep, one); 

  // come up with the two options
  AaVariable result(new AaVariableNode(*a.node(), a.use() | newsyms));
  result.ZERO();
  Variable resultCenter = result.center();
  shASN(resultCenter, flr_lo);

  Variable half = ConstAttrib1f(0.5f).repeat(a.size());
  AaVariable resultB(new AaVariableNode(*a.node(), a.use() | newsyms));
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
  Variable one = ConstAttrib1f(1.0f).repeat(a.size());
  shSGE(keep, keep, one); 

  // come up with the two options
  AaVariable result(new AaVariableNode(*a.node(), a.use() | newsyms));
  result.ZERO().ASN(a);
  Variable resultCenter = result.center();
  shFRAC(resultCenter, resultCenter);


  Variable half = ConstAttrib1f(0.5f).repeat(a.size());
  AaVariable resultB(new AaVariableNode(*a.node(), a.use() | newsyms));
  resultB.ZERO().setErr(half, newsyms);
  Variable resultBCenter = resultB.center();
  shASN(resultBCenter, half);

  result.COND(keep, resultB);
  return result;
}

struct __aaop_log {
  typedef Variable V; 
  static const bool use_fdf = true;
  static void f(V r, V x) { shLOG(r, x); }
  static void dfinv(V r, V x, V lo, V hi) { shRCP(r, x); }
  static void fdfinv(V r, V x) { 
    shLOG(r, x); 
    shASN(r, -r);
  }
};


AaVariable aaLOG(const AaVariable& a, const AaSyms& newsyms)
{
  AaVariable result(new AaVariableNode(*a.node(), a.use() | newsyms));
  convexApprox<__aaop_log>(result, a, newsyms, true); // @todo range - fix this
  return result;
}

struct __aaop_log2 {
  typedef Variable V; 
  static const bool use_fdf = true;
  static void f(V r, V x) { shLOG2(r, x); }
  static void dfinv(V r, V x, V lo, V hi) { shRCP(r, x); }
  static void fdfinv(V r, V x) { 
    shLOG2(r, x); 
    shASN(r, -r);
  }
};


AaVariable aaLOG2(const AaVariable& a, const AaSyms& newsyms)
{
  AaVariable result(new AaVariableNode(*a.node(), a.use() | newsyms));
  convexApprox<__aaop_log2>(result, a, newsyms); // @todo range - fix this
  return result;
}

struct __aaop_log10 {
  typedef Variable V; 
  static const bool use_fdf = true;
  static void f(V r, V x) { shLOG10(r, x); }
  static void dfinv(V r, V x, V lo, V hi) { shRCP(r, x); }
  static void fdfinv(V r, V x) { 
    shLOG10(r, x); 
    shASN(r, -r);
  }
};


AaVariable aaLOG10(const AaVariable& a, const AaSyms& newsyms)
{
  AaVariable result(new AaVariableNode(*a.node(), a.use() | newsyms));
  convexApprox<__aaop_log10>(result, a, newsyms); // @todo range - fix this
  return result;
}

AaVariable aaTEX(const Variable& texVar, const AaVariable& coord, const AaSyms& destsyms, const AaSyms& newsyms)
{
  assert(false);
  AaVariable result(new AaVariableNode(texVar.node(), destsyms)); 

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
  shLRP(resultCenter, ConstAttrib1f(0.5f), ahi, alo);

  // assign hi - lo / 2 to the input error symbol
  Variable aerr = result.temp(a.name() + "_err");
  shLRP(aerr, ConstAttrib1f(0.5f), ahi, -alo);
  return result.setErr(aerr, newsyms); 
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
    Variable uniquei = dest.err(i, syms->unique[i]);
    Variable absUniquei(uniquei.node()->clone(SH_TEMP, uniquei.size()));
    Variable mergei = mergeDest.err(i, syms->mergeRep[i]);

    shABS(absUniquei, uniquei);
    shCSUM(mergei, absUniquei);
  }
}


}

