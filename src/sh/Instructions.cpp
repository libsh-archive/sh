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
#include <cmath>
#include "Instructions.hpp"
#include "Statement.hpp"
#include "Context.hpp"
#include "Variant.hpp"
#include "Eval.hpp"
#include "Debug.hpp"
#include "Error.hpp"

namespace {

using namespace SH;

inline bool immediate()
{
  return !Context::current()->parsing();
}

inline void sizes_match(const Variable& a, const Variable& b)
{
  SH_DEBUG_ASSERT(a.size() == b.size());
}

inline void sizes_match(const Variable& a, const Variable& b,
                 const Variable& c, bool scalar_b = false, bool scalar_c = false)
{
  SH_DEBUG_ASSERT(((scalar_b && b.size() == 1) || a.size() == b.size()) && 
                  ((scalar_c && c.size() == 1) || a.size() == c.size()));
}

inline void has_values(const Variable& a)
{
  SH_DEBUG_ASSERT(a.hasValues());
}

inline void has_values(const Variable& a, const Variable& b)
{
  SH_DEBUG_ASSERT(a.hasValues());
  SH_DEBUG_ASSERT(b.hasValues());
}

inline void has_values(const Variable& a, const Variable& b, const Variable& c)
{
  SH_DEBUG_ASSERT(a.hasValues());
  SH_DEBUG_ASSERT(b.hasValues());
  SH_DEBUG_ASSERT(c.hasValues());
}

inline void has_values(const Variable& a, const Variable& b,
                const Variable& c, const Variable& d)
{
  SH_DEBUG_ASSERT(a.hasValues());
  SH_DEBUG_ASSERT(b.hasValues());
  SH_DEBUG_ASSERT(c.hasValues());
  SH_DEBUG_ASSERT(d.hasValues());
}

inline void addStatement(const Statement& stmt)
{
  Context::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
}

}

namespace SH {
#define INST_UNARY_OP_CORE(op)\
  if(immediate()) {\
    has_values(dest, src);\
    Variant *dv, *sv;\
    bool newd, news;\
    newd = dest.loadVariant(dv);\
    news = src.loadVariant(sv);\
    (*Eval::instance())(OP_ ## op, dv, sv, 0, 0);\
    if(newd) {\
      dest.setVariant(dv); \
      delete dv;\
    } else {\
      dest.updateVariant();\
    }\
    if(news) delete sv;\
  } else {\
    Statement stmt(dest, OP_ ## op, src);\
    addStatement(stmt);\
  }

#define INST_UNARY_OP(op)\
void sh ## op(Variable& dest, const Variable& src)\
{\
  sizes_match(dest, src);\
  INST_UNARY_OP_CORE(op);\
}

#define INST_BINARY_OP_CORE(op)\
  if(immediate()) {\
    has_values(dest, a, b);\
    Variant *dv, *av, *bv;\
    bool newd, newa, newb;\
    newd = dest.loadVariant(dv);\
    newa = a.loadVariant(av);\
    newb = b.loadVariant(bv);\
    (*Eval::instance())(OP_ ## op, dv, av, bv, 0);\
    if(newd) {\
      dest.setVariant(dv); \
      delete dv;\
    } else {\
      dest.updateVariant();\
    }\
    if(newa) delete av;\
    if(newb) delete bv;\
  } else {\
    Statement stmt(dest, a, OP_ ## op, b);\
    addStatement(stmt);\
  }\

#define INST_BINARY_OP(op, scalar_a, scalar_b)\
void sh ## op(Variable& dest, const Variable& a, const Variable &b)\
{\
  sizes_match(dest, a, b, scalar_a, scalar_b);\
  INST_BINARY_OP_CORE(op);\
}


#define INST_TERNARY_OP(op, condition)\
void sh ## op(Variable& dest, const Variable& a, const Variable &b, const Variable &c)\
{\
  SH_DEBUG_ASSERT(condition);\
  if(immediate()) {\
    has_values(dest, a, b, c);\
    Variant *dv, *av, *bv, *cv;\
    bool newd, newa, newb, newc;\
    newd = dest.loadVariant(dv);\
    newa = a.loadVariant(av);\
    newb = b.loadVariant(bv);\
    newc = c.loadVariant(cv);\
    (*Eval::instance())(OP_ ## op, dv, av, bv, cv);\
    if(newd) {\
      dest.setVariant(dv); \
      delete dv;\
    } else {\
      dest.updateVariant();\
    }\
    if(newa) delete av;\
    if(newb) delete bv;\
    if(newc) delete cv;\
  } else {\
    Statement stmt(dest, OP_ ## op, a, b, c);\
    addStatement(stmt);\
  }\
}

// TODO 
// intelligent type selection for operators
// (Instead of just selecting evaluator based on dest.valueType,
// select based on some kind of type hierarchy)

INST_UNARY_OP(ASN);

INST_BINARY_OP(ADD, true, true);
INST_BINARY_OP(MUL, true, true);
INST_BINARY_OP(DIV, true, true);
INST_BINARY_OP(SLT, true, true);
INST_BINARY_OP(SLE, true, true);
INST_BINARY_OP(SGT, true, true);
INST_BINARY_OP(SGE, true, true);
INST_BINARY_OP(SEQ, true, true);
INST_BINARY_OP(SNE, true, true);

INST_UNARY_OP(ABS);
INST_UNARY_OP(ACOS);
INST_UNARY_OP(ACOSH);
INST_UNARY_OP(ASIN);
INST_UNARY_OP(ASINH);
INST_UNARY_OP(ATAN);
INST_UNARY_OP(ATANH);
INST_BINARY_OP(ATAN2, false, false);
INST_UNARY_OP(CBRT);
INST_UNARY_OP(CEIL);
INST_UNARY_OP(COS);
INST_UNARY_OP(COSH);

void shCMUL(Variable& dest, const Variable& src)
{
  SH_DEBUG_ASSERT(dest.size() == 1);
  INST_UNARY_OP_CORE(CMUL);
}

void shCSUM(Variable& dest, const Variable& src)
{
  SH_DEBUG_ASSERT(dest.size() == 1);
  INST_UNARY_OP_CORE(CSUM);
}

void shDOT(Variable& dest, const Variable& a, const Variable& b)
{
  SH_DEBUG_ASSERT(dest.size() == 1);
  sizes_match(a, b);
  INST_BINARY_OP_CORE(DOT);
}

void shDX(Variable& dest, const Variable& a)
{
  if(immediate()) {
      error(ScopeException("Cannot take derivatives in immediate mode"));
  }
  Statement stmt(dest, OP_DX, a);
  addStatement(stmt);
}

void shDY(Variable& dest, const Variable& a)
{
  if(immediate()) {
      error(ScopeException("Cannot take derivatives in immediate mode"));
  }
  SH_DEBUG_ASSERT(!immediate());
  Statement stmt(dest, OP_DY, a);
  addStatement(stmt);
}

INST_UNARY_OP(EXP);
INST_UNARY_OP(EXP2);
INST_UNARY_OP(EXP10);
INST_UNARY_OP(FLR);
INST_BINARY_OP(MOD, true, true);
INST_UNARY_OP(FRAC);
INST_TERNARY_OP(LRP,
    (dest.size() == b.size() &&
    dest.size() == c.size() &&
    (dest.size() == a.size() || a.size() == 1)));

INST_UNARY_OP(LIT);
INST_UNARY_OP(LOG);
INST_UNARY_OP(LOG2);
INST_UNARY_OP(LOG10);
  
INST_TERNARY_OP(MAD,\
(\
  (dest.size() == c.size() &&\
    (dest.size() == a.size() ||\
      (dest.size() == b.size() && a.size() == 1\
      )\
    ) &&\
    (dest.size() == b.size() ||\
      (dest.size() == a.size() && b.size() == 1\
      )\
    ) \
  ) ||\
  (dest.size() == a.size() &&\
    (dest.size() == c.size() ||\
      (dest.size() == b.size() && c.size() == 1\
      )\
    ) &&\
    (dest.size() == b.size() ||\
      (dest.size() == c.size() && b.size() == 1\
      )\
    )\
  ) ||\
  (dest.size() == b.size() &&\
    (dest.size() == c.size() ||\
      (dest.size() == a.size() && c.size() == 1\
      )\
    ) &&\
    (dest.size() == a.size() ||\
      (dest.size() == c.size() && a.size() == 1\
      )\
    )\
  ) ||\
  (dest.size() == a.size() &&\
    (b.size() == 1 && c.size() == 1\
    )\
  ) ||\
  (dest.size() == b.size() &&\
    (a.size() == 1 && c.size() == 1\
    )\
  ) ||\
  (dest.size() == c.size() &&\
    (a.size() == 1 && b.size() == 1\
    )\
  ) \
)\
);

INST_BINARY_OP(MAX, false, false);
INST_BINARY_OP(MIN, false, false);
INST_BINARY_OP(POW, true, true);
INST_UNARY_OP(RCP);
INST_UNARY_OP(RND);
INST_UNARY_OP(RSQ);
INST_UNARY_OP(SGN);
INST_UNARY_OP(SIN);
INST_UNARY_OP(SINH);
INST_UNARY_OP(SQRT);
INST_UNARY_OP(TAN);
INST_UNARY_OP(TANH);
INST_UNARY_OP(NORM);
INST_UNARY_OP(NOISE);
INST_UNARY_OP(HASH);

void shXPD(Variable& dest, const Variable& a, const Variable& b)
{
  SH_DEBUG_ASSERT(dest.size() == 3 && a.size() == 3 && b.size() == 3);
  INST_BINARY_OP_CORE(XPD);
}

INST_TERNARY_OP(COND,
    (dest.size() == b.size() &&
    dest.size() == c.size() &&
    (dest.size() == a.size() || a.size() == 1)));

void shKIL(const Variable& a)
{
  if(immediate()) {
    error(ScopeException("Cannot kill in immediate mode"));
  }
  SH_DEBUG_ASSERT(!immediate());
  Statement stmt(a, OP_KIL, a);
  addStatement(stmt);
}

void shRET(const Variable& a)
{
  if(immediate()) {
    error(ScopeException("Cannot return in immediate mode"));
  }
  SH_DEBUG_ASSERT(!immediate());
  Statement stmt(a, OP_RET, a);
  addStatement(stmt);
}

}
