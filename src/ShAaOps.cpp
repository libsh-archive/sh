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

#include "ShAaOps.hpp"
#include "ShSyntax.hpp"
#include "ShVariableNode.hpp"
#include "ShAttrib.hpp"
#include "ShInstructions.hpp"
#include "ShTransformer.hpp"
#include "ShCtrlGraphWranglers.hpp"
#include "ShOptimizations.hpp"
#include "ShAffineTexture.hpp"
#include "ShAaSyms.hpp"
#include "ShAaSymPlacer.hpp"
#include "ShAaVariable.hpp"
#include "ShAaOpHandler.hpp"

#ifdef SH_DBG_AA
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
typedef std::vector<const ShAaVariable*> AaVarVec;
void affineApprox(ShAaVariable &dest,
                  const AaVarVec &src,
                  const ShRecord& coeff, const ShAaSyms &newsyms)
{
  // @todo debug
  if(src.size() != coeff.size() - 2) {
    SH_DEBUG_PRINT("src.size = " << src.size() << " coeff.size() - 2 = " << (coeff.size() - 2));
  }
  SH_DEBUG_ASSERT(src.size() == coeff.size() - 2);
  SH_DEBUG_ASSERT(src.size() > 0);

  ShRecord::const_iterator C = coeff.begin();
  dest.ASN(aaMUL(*src[0], *C++));
  ShAaSyms assigned = src[0]->use(); // keeps track of symbols assigned to so far

  for(size_t i = 1; i < src.size(); ++i, ++C) {
    ShAaSyms add = src[i]->use() - assigned;
    ShAaSyms isct = src[i]->use() & assigned;

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
void affineApprox(ShAaVariable &dest, const ShAaVariable &src, 
                  ShVariable alpha, ShVariable beta, 
                  ShVariable delta, const ShAaSyms &newsyms)
{
  AaVarVec srcVec;
  srcVec.push_back(&src);
  affineApprox(dest, srcVec, alpha & beta & delta, newsyms);
}


// Affine approximation 
// Detects if a or b already has newsyms, in which case it adds the error on
// as an absolute value
void affineApprox(ShAaVariable &dest,
                  const ShAaVariable &a, const ShAaVariable &b,
                  ShVariable alpha, ShVariable beta, ShVariable gamma, 
                  ShVariable delta, const ShAaSyms &newsyms)
{
  AaVarVec srcVec;
  srcVec.push_back(&a);
  srcVec.push_back(&b);
  affineApprox(dest, srcVec, alpha & beta & gamma & delta, newsyms);
}



/* Given a functor F that defines three functions similar to ShAffine.hpp's
 * convexApprox (except the arguments are now ShVariable tuples),
 * this does line for line the same thing.
 *
 * @see ShAffine.hpp
 */
template<typename F>
void convexApprox(ShAaVariable &dest, const ShAaVariable &src, const ShAaSyms &newsyms, bool pos=false)
{
  // Translated directly from ShAffineImpl.hpp
  ShVariable lo = src.lo();
  ShVariable hi = src.hi();
  if(pos) {
    // EPS should really depend on the type of the variable and the platform.
    // Here we just pick something that should be representable even for
    // half-floats.
    ShVariable EPS = ShConstAttrib1f(1e-7f); 
    shMAX(lo, lo, EPS.repeat(lo.size()));

    // @todo range make sure hi > lo...may not always be what we want though?
    shMAX(hi, lo, hi); 
  }
  ShVariable flo = src.temp("fhi"); 
  ShVariable fhi = src.temp("flo"); 
  F::f(flo, lo);
  F::f(fhi, hi);

  ShVariable alpha, ss, bpd, bmd, beta, delta; 
  alpha = src.temp("alpha");

  ShVariable hilo = src.temp("hi-lo");
  ShVariable fhiflo = src.temp("fhi-flo");
  shADD(hilo, hi, -lo);
  shADD(fhiflo, fhi, -flo);
  shDIV(alpha, fhiflo, hilo);
  ss = src.temp("ss");
  F::dfinv(ss, alpha, lo, hi);

  //if(lo > ss || hi < ss) std::cout << "  WARNING - ss out of bounds" << std::endl;
  bmd = src.temp("bmd"); 
  shMAD(bmd, -alpha, lo, flo);

  bpd = src.temp("bpd");
  ShVariable fdfinv = src.temp("fdfinv");
  if(F::use_fdf) {
    F::fdfinv(fdfinv, alpha);
  } else {
    F::f(fdfinv, ss);
  }
  shMAD(bpd, -alpha, ss, fdfinv); 

  beta = src.temp("beta");
  shLRP(beta, ShConstAttrib1f(0.5f), bpd, bmd);
  delta = src.temp("delta");
  shLRP(delta, ShConstAttrib1f(0.5f), bpd, -bmd);

  shABS(delta, delta);
  return affineApprox(dest, src, alpha, beta, delta, newsyms);
}

/* Return the union of the two syms */
ShAaSyms mergeSyms(const ShAaVariable& a, const ShAaVariable& b) 
{
  SH_DEBUG_ASSERT(a.size() == b.size());
  int size = a.size();
  ShAaSyms result(size);
  for(int i = 0; i < size; ++i) {
    result[i] = a.use(i);
    result[i] |= b.use(i);
  }
  return result;
}

/* Return the union of the two syms with additional syms */
ShAaSyms mergeSyms(const ShAaVariable& a, const ShAaVariable& b, const ShAaSyms &addSyms) 
{
  ShAaSyms result = mergeSyms(a, b);
  result |= addSyms;
  return result;
}

ShAaVariableNodePtr makeMergeNode(const char* name, const ShAaVariable& a,
    const ShAaVariable& b)
{
  return new ShAaVariableNode(name, SH_TEMP,
        a.valueType(), a.specialType(), mergeSyms(a, b));
}

ShAaVariableNodePtr makeMergeNode(const char* name, const ShAaVariable& a,
    const ShAaVariable& b, const ShAaSyms &addSyms)
{
  return new ShAaVariableNode(name, SH_TEMP,
        a.valueType(), a.specialType(), mergeSyms(a, b, addSyms));
}
}
namespace SH {

// @todo range - remove ZEROS, and properly determine sym set intersections,
// differences

// @todo range - determine result value types properly
ShAaVariable aaADD(const ShAaVariable& a, const ShAaVariable& b)
{
  ShAaVariable result(makeMergeNode("aopADDt", a, b));
  ShAaSyms isct = a.use() & b.use();
  ShAaSyms bonly = b.use() - a.use(); 
  SH_DEBUG_PRINT_AOP("Set Ops: a=" << a.use() << " b=" << b.use() 
      << " isct=" << isct << " bonly=" << bonly);
  result.ASN(a);
  result.ASN(b, bonly, false);
  result.ADD(b, isct, true); 
  return result;
}

ShAaVariable aaADD(const ShAaVariable& a, const ShVariable& b)
{
  ShAaVariable result(a.clone());
  return result.ADD(b);
}

/* Sets dest = a * b */
ShAaVariable aaMUL(const ShAaVariable &a, const ShAaVariable &b,
           const ShAaSyms &newsyms)
{
  ShAaVariable result(makeMergeNode("aopMULt", a, b, newsyms));

  // @see ShAffineImpl.hpp
  // This is just a vectorized copy of operator* from ShAffineImpl
  ShVariable alpha(result.temp("aopMULalpha"));
  shASN(alpha, b.center()); 

  ShVariable beta(result.temp("aopMULbeta"));
  shASN(beta, a.center()); 

  ShVariable gamma(result.temp("aopMULgamma"));
  ShVariable delta(result.temp("aopMULdelta"));

  // @todo range - this is bad numerically
  // should just assign the new center to this
  //shMUL(gamma, -alpha, beta);
  shMUL(delta, a.radius(), b.radius());
  affineApprox(result, a, b, alpha, beta, gamma, delta, newsyms);
  ShVariable resultCenter = result.center();
  shMUL(resultCenter, alpha, beta);
  return result;
}

ShAaVariable aaMUL(const ShAaVariable &a, const ShVariable &b)
{
  ShAaVariable result(a.clone());
  return result.MUL(b);
}

ShAaVariable aaLRP(const ShAaVariable &a, const ShAaVariable &b,
           const ShAaVariable &c, const ShAaSyms &newsyms)
{
  return aaMAD(a, aaADD(b, c.NEG()), c, newsyms);
}

ShAaVariable aaMAD(const ShAaVariable &a, const ShAaVariable &b,
           const ShAaVariable &c, const ShAaSyms &newsyms)
{
  ShAaVariable ab = aaMUL(a, b, newsyms);
  return aaADD(ab, c);
}

ShAaVariable aaPOS(const ShAaVariable& a, const ShAaSyms& newsyms)
{
  ShVariable aCenter = a.center();
  ShVariable aRadius = a.radius(); 

  ShVariable aLo = a.temp(a.name() + "_lo");
  ShVariable aHi = a.temp(a.name() + "_hi");
  shADD(aLo, aCenter, -aRadius);
  shADD(aHi, aCenter, aRadius);

  ShAaVariable result(new ShAaVariableNode(*a.node(), a.use() | newsyms));

  // if lo < 0 && hi > 0, scale radius to hi / 2 and center = hi / 2
  // if hi <= 0, set to 0
  // else keep
  ShConstAttrib1f ZERO(0.0f);
  ShConstAttrib1f HALF(0.5f);

  ShVariable loLtZero = a.temp(a.name() + "_lo-le-0");
  shSLT(loLtZero, aLo, ZERO.repeat(aLo.size()));
  //ShAaVariable scaledResult(a.clone());
  ShAaVariable scaledResult(result.clone());
  ShVariable scaling = a.temp(a.name() + "_scaling");
  ShVariable halfHi = a.temp(a.name() + "_half_hi");

  shMUL(halfHi, aHi, HALF.repeat(aHi.size()));

  // @todo range fix
#if 0
  shRCP(scaling, aRadius);
  shMUL(scaling, scaling, halfHi);
  scaledResult.MUL(scaling);
  shASN(scaledResult.center(), halfHi);
#endif
  ShVariable scaledResultCenter = scaledResult.center();
  shASN(scaledResultCenter, halfHi);
  scaledResult.setErr(halfHi, newsyms);
  result.COND(loLtZero, scaledResult);

  ShVariable hiLeZero = a.temp(a.name() + "_hi-lt-0");
  shSLT(hiLeZero, aHi, ZERO.repeat(aHi.size()));
  result.COND(hiLeZero, ZERO.repeat(a.size()));

  return result;
}

ShAaVariable aaMAX(const ShAaVariable& a, const ShAaVariable& b, const ShAaSyms& newsyms)
{
  return aaADD(aaPOS(aaADD(a, b.NEG()), newsyms), b); 
}

ShAaVariable aaMIN(const ShAaVariable& a, const ShAaVariable& b, const ShAaSyms& newsyms)
{
  return aaADD(a, aaPOS(aaADD(a, b.NEG()), newsyms).NEG()); 
}

ShAaVariable aaNORM(const ShAaVariable& a, const ShAaSyms& newsyms)
{
  // need to do some weird stuff to get the right syms
  ShAaSyms mulSyms = newsyms.last();
  ShAaSyms scalarSyms = (newsyms - mulSyms).swizSyms(ShSwizzle(1));
  ShAaVariable normSq = aaDOT(a, a, scalarSyms);
  ShAaVariable invnorm = aaRSQ(normSq, scalarSyms);
  return aaMUL(a, normSq.repeat(a.size()), mulSyms);
}

struct __aaop_rcp {
  typedef ShVariable V; 
  static const bool use_fdf = false;
  static void f(V r, V x) { shRCP(r, x); }
  static void dfinv(V r, V x, V lo, V hi) { 
    shRSQ(r, -x); 
    V rSgtHi(r.node()->clone(SH_TEMP));
    shSGT(rSgtHi, r, hi);
    shCOND(r, rSgtHi, -r, r);
  }
  static void fdfinv(V r, V x) { shSQRT(r, -x); }
};


ShAaVariable aaPOW(const ShAaVariable& a, const ShAaVariable& b, const ShAaSyms& newsyms)
{
  return aaEXP(aaMUL(aaLOG(a, newsyms), b, newsyms), newsyms);
}

ShAaVariable aaRCP(const ShAaVariable& a, const ShAaSyms& newsyms)
{
  ShAaVariable result(new ShAaVariableNode(*a.node(), a.use() | newsyms));
  convexApprox<__aaop_rcp>(result, a, newsyms); // @todo range - fix this
  return result;
}

struct __aaop_rsq {
  typedef ShVariable V; 
  static const bool use_fdf = false;
  static void f(V r, V x) { shRSQ(r, x); }
  static void dfinv(V r, V x, V lo, V hi) { 
    shMUL(r, x, ShConstAttrib1f(-2.0f)); 
    shPOW(r, r, ShConstAttrib1f(-2.0f/3.0f));
  }
  static void fdfinv(V r, V x) {}
};


ShAaVariable aaRSQ(const ShAaVariable& a, const ShAaSyms& newsyms)
{
  ShAaVariable result(new ShAaVariableNode(*a.node(), a.use() | newsyms));
  convexApprox<__aaop_rsq>(result, a, newsyms, true); 
  return result;
}

struct __aaop_sqrt {
  typedef ShVariable V; 
  static const bool use_fdf = true;
  static void f(V r, V x) { shSQRT(r, x); }
  static void dfinv(V r, V x, V lo, V hi) { 
    shMUL(r, x, x); 
    shMUL(r, r, ShConstAttrib1f(4.0f));
    shRCP(r, r);
  }
  static void fdfinv(V r, V x) { 
    shRCP(r, x); 
    shMUL(r, r, ShConstAttrib1f(0.5f));
  }
};


ShAaVariable aaSQRT(const ShAaVariable& a, const ShAaSyms& newsyms)
{
  // deal with bounds < 0 case...
  ShAaVariable result(new ShAaVariableNode(*a.node(), a.use() | newsyms));
  convexApprox<__aaop_sqrt>(result, a, newsyms, true); 
  return result;
}

ShAaVariable aaCSUM(const ShAaVariable &a)
{
  if(a.size() == 1) return a;
  ShAaVariable result = aaADD(a(0), a(1)); 
  for(int i = 2; i < a.size(); ++i) {
    result = aaADD(result, a(i));
  }
  return result;
}

ShAaVariable aaDOT(const ShAaVariable &a, const ShAaVariable &b,
           const ShAaSyms &newsyms)
{
  SH_DEBUG_ASSERT(a.size() == b.size());
  ShAaVariable result = aaMUL(a(0), b(0), newsyms);
  for(int i = 1; i < a.size(); ++i) {
    // @todo range - should use MAD, but then we just need to change
    // some of the functions to allow adding to existing error symbols
    // (after taking absolute values of them...)
    result = aaADD(result, aaMUL(a(i), b(i), newsyms));
  }
  return result;
}

struct __aaop_exp {
  typedef ShVariable V; 
  static const bool use_fdf = true;
  static void f(V r, V x) { shEXP(r, x); }
  static void dfinv(V r, V x, V lo, V hi) { shLOG(r, x); }
  static void fdfinv(V r, V x) { shASN(r, x); }
};


ShAaVariable aaEXP(const ShAaVariable& a, const ShAaSyms& newsyms)
{
  ShAaVariable result(new ShAaVariableNode(*a.node(), a.use() | newsyms));
  convexApprox<__aaop_exp>(result, a, newsyms);
  return result;
}

struct __aaop_exp2 {
  typedef ShVariable V; 
  static const bool use_fdf = true;
  static void f(V r, V x) { shEXP2(r, x); }
  static void dfinv(V r, V x, V lo, V hi) { shLOG2(r, x); }
  static void fdfinv(V r, V x) { shASN(r, x); }
};

ShAaVariable aaEXP2(const ShAaVariable& a, const ShAaSyms& newsyms)
{
  ShAaVariable result(new ShAaVariableNode(*a.node(), a.use() | newsyms));
  convexApprox<__aaop_exp2>(result, a, newsyms); 
  return result;
}

struct __aaop_exp10 {
  typedef ShVariable V; 
  static const bool use_fdf = true;
  static void f(V r, V x) { shEXP10(r, x); }
  static void dfinv(V r, V x, V lo, V hi) { shLOG10(r, x); }
  static void fdfinv(V r, V x) { shASN(r, x); }
};

ShAaVariable aaEXP10(const ShAaVariable& a, const ShAaSyms& newsyms)
{
  ShAaVariable result(new ShAaVariableNode(*a.node(), a.use() | newsyms));
  convexApprox<__aaop_exp10>(result, a, newsyms); // @todo range - fix this
  return result;
}

ShAaVariable aaFLR(const ShAaVariable& a, const ShAaSyms& newsyms)
{
  // A) if between n, n + 1 then set to n 
  // B) if not between 
  //    otherwise, best approx is (a - 0.5) 
  ShVariable lo, hi; 
  a.lohi(lo, hi);

  ShVariable flr_lo = a.temp("aaFLR_loflr");
  ShVariable keep = a.temp("aaFLR_keep");
  shFLR(flr_lo, lo); 
  shADD(keep, hi, -flr_lo);
  ShVariable one = ShConstAttrib1f(1.0f).repeat(a.size());
  shSGE(keep, keep, one); 

  // come up with the two options
  ShAaVariable result(new ShAaVariableNode(*a.node(), a.use() | newsyms));
  result.ZERO();
  ShVariable resultCenter = result.center();
  shASN(resultCenter, flr_lo);

  ShVariable half = ShConstAttrib1f(0.5f).repeat(a.size());
  ShAaVariable resultB(new ShAaVariableNode(*a.node(), a.use() | newsyms));
  resultB.ASN(a).ADD(-half).setErr(half, newsyms);

  result.COND(keep, resultB);
  return result;
}

ShAaVariable aaFRAC(const ShAaVariable& a, const ShAaSyms& newsyms)
{
  // A) if between n, n + 1 then 
  //  set center to be frac(center), no err
  // B) if not, best approx is 0.5, err = 0.5
  ShVariable lo, hi; 
  a.lohi(lo, hi);

  ShVariable flr_lo = a.temp("aaFRAC_loflr");
  ShVariable keep = a.temp("aaFRAC_keep");
  shFLR(flr_lo, lo); 
  shADD(keep, hi, -flr_lo);
  ShVariable one = ShConstAttrib1f(1.0f).repeat(a.size());
  shSGE(keep, keep, one); 

  // come up with the two options
  ShAaVariable result(new ShAaVariableNode(*a.node(), a.use() | newsyms));
  result.ZERO().ASN(a);
  ShVariable resultCenter = result.center();
  shFRAC(resultCenter, resultCenter);


  ShVariable half = ShConstAttrib1f(0.5f).repeat(a.size());
  ShAaVariable resultB(new ShAaVariableNode(*a.node(), a.use() | newsyms));
  resultB.ZERO().setErr(half, newsyms);
  ShVariable resultBCenter = resultB.center();
  shASN(resultBCenter, half);

  result.COND(keep, resultB);
  return result;
}

struct __aaop_log {
  typedef ShVariable V; 
  static const bool use_fdf = true;
  static void f(V r, V x) { shLOG(r, x); }
  static void dfinv(V r, V x, V lo, V hi) { shRCP(r, x); }
  static void fdfinv(V r, V x) { 
    shLOG(r, x); 
    shASN(r, -r);
  }
};


ShAaVariable aaLOG(const ShAaVariable& a, const ShAaSyms& newsyms)
{
  ShAaVariable result(new ShAaVariableNode(*a.node(), a.use() | newsyms));
  convexApprox<__aaop_log>(result, a, newsyms, true); // @todo range - fix this
  return result;
}

struct __aaop_log2 {
  typedef ShVariable V; 
  static const bool use_fdf = true;
  static void f(V r, V x) { shLOG2(r, x); }
  static void dfinv(V r, V x, V lo, V hi) { shRCP(r, x); }
  static void fdfinv(V r, V x) { 
    shLOG2(r, x); 
    shASN(r, -r);
  }
};


ShAaVariable aaLOG2(const ShAaVariable& a, const ShAaSyms& newsyms)
{
  ShAaVariable result(new ShAaVariableNode(*a.node(), a.use() | newsyms));
  convexApprox<__aaop_log2>(result, a, newsyms); // @todo range - fix this
  return result;
}

struct __aaop_log10 {
  typedef ShVariable V; 
  static const bool use_fdf = true;
  static void f(V r, V x) { shLOG10(r, x); }
  static void dfinv(V r, V x, V lo, V hi) { shRCP(r, x); }
  static void fdfinv(V r, V x) { 
    shLOG10(r, x); 
    shASN(r, -r);
  }
};


ShAaVariable aaLOG10(const ShAaVariable& a, const ShAaSyms& newsyms)
{
  ShAaVariable result(new ShAaVariableNode(*a.node(), a.use() | newsyms));
  convexApprox<__aaop_log10>(result, a, newsyms); // @todo range - fix this
  return result;
}

ShAaVariable aaTEX(const ShVariable& texVar, const ShAaVariable& coord, const ShAaSyms& destsyms, const ShAaSyms& newsyms)
{
  ShAaVariable result(new ShAaVariableNode(texVar.node(), destsyms)); 

  ShTextureNodePtr texnode = shref_dynamic_cast<ShTextureNode>(texVar.node());
  SH_DEBUG_ASSERT(texnode);
  ShAffineTexturePtr aatex = ShAffineTexture::convert(texnode);
  SH_DEBUG_ASSERT(aatex);

  ShRecord coeff(aatex->lookup(coord.lo(), coord.hi()));

  AaVarVec srcVec;
  for(int i = 0; i < coord.size(); ++i) {
    srcVec.push_back(new ShAaVariable(coord(i).repeat(result.size())));
  }

  affineApprox(result, srcVec, coeff, newsyms); 

  for(int i = 0; i < coord.size(); ++i) {
    delete srcVec[i];
  }
  return result;
}

ShAaVariable aaTEXI(const ShVariable& texVar, const ShAaVariable& coord, 
                    const ShAaSyms& destsyms, const ShAaSyms& newsyms)
{
  ShAaVariable result(new ShAaVariableNode(texVar.node(), destsyms)); 

  ShTextureNodePtr texnode = shref_dynamic_cast<ShTextureNode>(texVar.node());
  SH_DEBUG_ASSERT(texnode);
  ShAffineTexturePtr aatex = ShAffineTexture::convert(texnode);
  SH_DEBUG_ASSERT(aatex);

  ShRecord coeff(aatex->rect_lookup(coord.lo(), coord.hi()));

  AaVarVec srcVec;
  for(int i = 0; i < coord.size(); ++i) {
    srcVec.push_back(new ShAaVariable(coord(i).repeat(result.size())));
  }

  affineApprox(result, srcVec, coeff, newsyms); 

  for(int i = 0; i < coord.size(); ++i) {
    delete srcVec[i];
  }
  return result;
}

ShAaVariable aaESCJOIN(const ShAaVariable& a, const ShAaSyms& destsyms, const ShAaSyms& newsyms) 
{
  // only keep the nonhier syms, and merge the rest
  ShAaVariable result(new ShAaVariableNode(*a.node(), destsyms));
  ShAaSyms isct = a.use() & destsyms; 
  ShAaSyms joinSyms = a.use() - destsyms;
  result.ASN(a, isct, true); 

  ShVariable joinerr = result.temp(a.name() + "_joinerr");
  for(int i = 0; i < a.size(); ++i) {
    ShVariable erri = a.err(i, joinSyms[i]);
    ShVariable absErri = a.node()->makeTemp(erri.size(), "_jointemp");
    shABS(absErri, erri); 

    ShVariable joinerri = joinerr(i); 
    shCSUM(joinerri, absErri); 
  }
  result.setErr(joinerr, newsyms);
  return result;
}

ShAaVariable aaIVAL(const ShVariable& a, const ShVariable& b, 
    const ShAaSyms& newsyms) 
{
  SH_DEBUG_ASSERT(a.size() == b.size());
  ShVariable iaValue(a.node()->clone(SH_TEMP, 0, 
        shIntervalValueType(a.valueType())));
  shIVAL(iaValue, a, b);
  return aaFROMIVAL(iaValue, newsyms);
}

ShVariable aaLASTERR(const ShAaVariable& a, const ShAaSyms& syms)
{
  // @todo range handl case where syms is empty
  SH_DEBUG_ASSERT(!syms[0].empty());
  int idx = syms[0].last();
  ShVariable result(a.temp(a.name() + "_lasterr"));
  for(int i = 0; i < a.size(); ++i) {
    ShVariable resulti = result(i);
    shASN(resulti, a.err(i, idx)); 
  }
  return result;
}

ShAaVariable aaFROMIVAL(const ShVariable& a, const ShAaSyms& newsyms)
{
  ShAaVariable result(new ShAaVariableNode(a.node(), newsyms,
      SH_TEMP, "_ia2aa"));
  // split into lo/hi 
  ShVariable alo = result.temp(a.name() + "_lo");
  ShVariable ahi = result.temp(a.name() + "_hi");
  shLO(alo, a);
  shHI(ahi, a);

  // assign lo + hi / 2 to center
  ShVariable resultCenter = result.center();
  shLRP(resultCenter, ShConstAttrib1f(0.5f), ahi, alo);

  // assign hi - lo / 2 to the input error symbol
  ShVariable aerr = result.temp(a.name() + "_err");
  shLRP(aerr, ShConstAttrib1f(0.5f), ahi, -alo);
  return result.setErr(aerr, newsyms); 
}

ShVariable aaTOIVAL(const ShAaVariable& a)
{
  ShVariable result(a.center().node()->clone(SH_TEMP, 0,
        shIntervalValueType(a.center().valueType())));
  result.name(a.name() + "_2ia");

  ShVariable aRadius = a.radius(); 
  ShVariable aCenter = a.center(); 
  ShVariable aLo = a.temp(a.name() + "_lo");
  ShVariable aHi = a.temp(a.name() + "_hi");
  shADD(aLo, aCenter, -aRadius);
  shADD(aHi, aCenter, aRadius);
  shIVAL(result, aLo, aHi);
  return result;
}

ShAaVariable aaFROMTUPLE(const ShVariable& a)
{
  ShAaVariable result(new ShAaVariableNode(a.node(), ShAaSyms(a.size()),
      SH_TEMP, "_2aa"));
  ShVariable resultCenter = result.center();
  shASN(resultCenter, a);
  return result;
}


}

