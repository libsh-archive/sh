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
#include "ShAaSyms.hpp"
#include "ShAaSymPlacer.hpp"
#include "ShAaVariable.hpp"
#include "ShAaOpHandler.hpp"

#define SH_DEBUG_AAOPS

#ifdef SH_DEBUG_AAOPS
#define SH_DEBUG_PRINT_AOP(x) { SH_DEBUG_PRINT(x) }
#else
#define SH_DEBUG_PRINT_AOP(X)
#endif

namespace {
using namespace SH;

void affineApprox(ShAaVariable &dest, ShAaVariable src, 
                  ShVariable alpha, ShVariable beta, 
                  ShVariable delta, const ShAaSyms &newsyms)
{
  dest.ASN(aaMUL(src, alpha)).ADD(beta).setErr(delta, newsyms);
}


void affineApprox(ShAaVariable &dest,
                  const ShAaVariable &a, const ShAaVariable &b,
                  ShVariable alpha, ShVariable beta, ShVariable gamma, 
                  ShVariable delta, const ShAaSyms &newsyms)
{
  ShAaSyms isct = a.use() & b.use();
  ShAaSyms bonly = b.use() - a.use();
  SH_DEBUG_PRINT_AOP("Set Ops: a=" << a.use() << " b=" << b.use() 
      << " isct=" << isct << " bonly=" << bonly);

  dest.ASN(aaMUL(a, alpha));
  dest.ASN(aaMUL(b, beta), bonly, false); 
  dest.MAD(b, beta, isct, true);
  dest.ADD(gamma);
  dest.setErr(delta, newsyms);
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
  ShVariable alpha = b.center(); 
  ShVariable beta = a.center(); 
  ShVariable gamma(result.temp("gamma"));
  ShVariable delta(result.temp("delta"));

  // @todo range - this is bad numerically
  // should just assign the new center to this
  //shMUL(gamma, -alpha, beta);
  shMUL(delta, a.radius(), b.radius());
  affineApprox(result, a, b, alpha, beta, gamma, delta, newsyms);
  shMUL(result.center(), alpha, beta);
  return result;
}

ShAaVariable aaMUL(const ShAaVariable &a, const ShVariable &b)
{
  ShAaVariable result(a.clone());
  return result.MUL(b);
}

ShAaVariable aaMAD(const ShAaVariable &a, const ShAaVariable &b,
           const ShAaVariable &c, const ShAaSyms &newsyms)
{
  ShAaVariable ab = aaMUL(a, b, newsyms);
  return aaADD(ab, c);
}

ShAaVariable aaDOT(const ShAaVariable &a, const ShAaVariable &b,
           const ShAaSyms &newsyms)
{
  SH_DEBUG_ASSERT(a.size() == b.size());
  ShAaVariable result;
  SH_DEBUG_ASSERT(0 && "DOT not done");
#if 0
  = aaMUL(a(0), b(0), newsyms);
  for(int i = 1; i <= a.size(); ++i) {
    result = aaADD(result, aaMUL(a(i), b(i), newsyms));
  }
#endif
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
  shLRP(result.center(), ShConstAttrib1f(0.5f), ahi, alo);

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
  shASN(result.center(), a);
  return result;
}


}

