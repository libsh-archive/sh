#include <cmath>
#include "ShInstructions.hpp"
#include "ShStatement.hpp"
#include "ShContext.hpp"
#include "ShDebug.hpp"
#include "ShError.hpp"

namespace {

using namespace SH;

bool immediate()
{
  return !ShContext::current()->parsing();
}

void sizes_match(const ShVariable& a, const ShVariable& b)
{
  SH_DEBUG_ASSERT(a.size() == b.size());
}

void sizes_match(const ShVariable& a, const ShVariable& b,
                 const ShVariable& c, bool scalar_b = false, bool scalar_c = false)
{
  SH_DEBUG_ASSERT(((scalar_b && b.size() == 1) || a.size() == b.size()) && 
                  ((scalar_c && c.size() == 1) || a.size() == c.size()));
}

void has_values(const ShVariable& a)
{
  SH_DEBUG_ASSERT(a.hasValues());
}

void has_values(const ShVariable& a, const ShVariable& b)
{
  SH_DEBUG_ASSERT(a.hasValues());
  SH_DEBUG_ASSERT(b.hasValues());
}

void has_values(const ShVariable& a, const ShVariable& b, const ShVariable& c)
{
  SH_DEBUG_ASSERT(a.hasValues());
  SH_DEBUG_ASSERT(b.hasValues());
  SH_DEBUG_ASSERT(c.hasValues());
}

void has_values(const ShVariable& a, const ShVariable& b,
                const ShVariable& c, const ShVariable& d)
{
  SH_DEBUG_ASSERT(a.hasValues());
  SH_DEBUG_ASSERT(b.hasValues());
  SH_DEBUG_ASSERT(c.hasValues());
  SH_DEBUG_ASSERT(d.hasValues());
}

void addStatement(const ShStatement& stmt)
{
  ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
}

}

namespace SH {

#define SHINST_UNARY_OP(op)\
void sh ## op(ShVariable& dest, const ShVariable& src)\
{\
  sizes_match(dest, src);\
  if(immediate()) {\
    SH_DEBUG_PRINT(__FILE__ << ":" << __LINE__); \
    has_values(dest, src);\
    ShCloakPtr result = dest.cloak();\
    (*ShEval::instance())(SH_OP_ ## op, result, src.cloak(), 0, 0);\
    dest.setCloak(result);\
  } else {\
    ShStatement stmt(dest, SH_OP_ ## op, src);\
    addStatement(stmt);\
  }\
}

#define SHINST_BINARY_OP_CORE(op)\
  if(immediate()) {\
    has_values(dest, a, b);\
    ShCloakPtr result = dest.cloak();\
    (*ShEval::instance())(SH_OP_ ## op, result, a.cloak(), b.cloak(), 0);\
    dest.setCloak(result);\
  } else {\
    ShStatement stmt(dest, a, SH_OP_ ## op, b);\
    addStatement(stmt);\
  }\

#define SHINST_BINARY_OP(op, scalar_a, scalar_b)\
void sh ## op(ShVariable& dest, const ShVariable& a, const ShVariable &b)\
{\
  sizes_match(dest, a, b, scalar_a, scalar_b);\
  SHINST_BINARY_OP_CORE(op);\
}


#define SHINST_TERNARY_OP(op, condition)\
void sh ## op(ShVariable& dest, const ShVariable& a, const ShVariable &b, const ShVariable &c)\
{\
  SH_DEBUG_ASSERT(condition);\
  if(immediate()) {\
    has_values(dest, a, b, c);\
    ShCloakPtr result = dest.cloak();\
    (*ShEval::instance())(SH_OP_ ## op, result, a.cloak(), b.cloak(), c.cloak());\
    dest.setCloak(result);\
  } else {\
    ShStatement stmt(dest, SH_OP_ ## op, a, b, c);\
    addStatement(stmt);\
  }\
}

// TODO 
// intelligent type selection for operators
// (Instead of just selecting evaluator based on dest.typeIndex,
// select based on some kind of type hierarchy)

SHINST_UNARY_OP(ASN);

SHINST_BINARY_OP(ADD, true, true);
SHINST_BINARY_OP(MUL, true, true);
SHINST_BINARY_OP(DIV, true, true);
SHINST_BINARY_OP(SLT, true, true);
SHINST_BINARY_OP(SLE, true, true);
SHINST_BINARY_OP(SGT, true, true);
SHINST_BINARY_OP(SGE, true, true);
SHINST_BINARY_OP(SEQ, true, true);
SHINST_BINARY_OP(SNE, true, true);

SHINST_UNARY_OP(ABS);
SHINST_UNARY_OP(ACOS);
SHINST_UNARY_OP(ASIN);
SHINST_UNARY_OP(ATAN);
SHINST_BINARY_OP(ATAN2, false, false);
SHINST_UNARY_OP(CEIL);
SHINST_UNARY_OP(COS);

void shDOT(ShVariable& dest, const ShVariable& a, const ShVariable& b)
{
  SH_DEBUG_ASSERT(dest.size() == 1);
  sizes_match(a, b);
  SHINST_BINARY_OP_CORE(DOT);
}

void shDX(ShVariable &dest, const ShVariable& a)
{
  if(immediate()) {
      shError(ShScopeException("Cannot take derivatives in immediate mode"));
  }
  ShStatement stmt(dest, SH_OP_DX, a);
  addStatement(stmt);
}

void shDY(ShVariable &dest, const ShVariable& a)
{
  if(immediate()) {
      shError(ShScopeException("Cannot take derivatives in immediate mode"));
  }
  SH_DEBUG_ASSERT(!immediate());
  ShStatement stmt(dest, SH_OP_DY, a);
  addStatement(stmt);
}

SHINST_UNARY_OP(EXP);
SHINST_UNARY_OP(EXP2);
SHINST_UNARY_OP(EXP10);
SHINST_UNARY_OP(FLR);
SHINST_BINARY_OP(MOD, false, false);
SHINST_UNARY_OP(FRAC);
SHINST_TERNARY_OP(LRP,
    (dest.size() == b.size() &&
    dest.size() == c.size() &&
    (dest.size() == a.size() || a.size() == 1)));

SHINST_UNARY_OP(LOG);
SHINST_UNARY_OP(LOG2);
SHINST_UNARY_OP(LOG10);
  
SHINST_TERNARY_OP(MAD,
    (dest.size() == c.size() &&
    (dest.size() == a.size() || (dest.size() == b.size() && a.size() == 1)) &&
    (dest.size() == b.size() || (dest.size() == a.size() && b.size() == 1))));

SHINST_BINARY_OP(MAX, false, false);
SHINST_BINARY_OP(MIN, false, false);
SHINST_BINARY_OP(POW, false, true);
SHINST_UNARY_OP(RCP);
SHINST_UNARY_OP(RSQ);
SHINST_UNARY_OP(SGN);
SHINST_UNARY_OP(SIN);
SHINST_UNARY_OP(SQRT);
SHINST_UNARY_OP(TAN);
SHINST_UNARY_OP(NORM);


void shXPD(ShVariable& dest, const ShVariable& a, const ShVariable& b)
{
  SH_DEBUG_ASSERT(dest.size() == 3 && a.size() == 3 && b.size() == 3);
  SHINST_BINARY_OP_CORE(XPD);
}

SHINST_TERNARY_OP(COND,
    (dest.size() == b.size() &&
    dest.size() == c.size() &&
    (dest.size() == a.size() || a.size() == 1)));

void shLO(ShVariable& dest, const ShVariable& src)
{
  sizes_match(dest, src); // TODO check types are okay
  if(immediate()) {
    has_values(dest, src);
    ShCloakPtr result = dest.cloak();
    (*ShEval::instance())(SH_OP_LO, result, src.cloak(), 0, 0);
    dest.setCloak(result);
  } else {
    ShStatement stmt(dest, SH_OP_LO, src);
    addStatement(stmt);
  }
}

void shHI(ShVariable& dest, const ShVariable& src)
{
  sizes_match(dest, src); // TODO check types are okay
  if(immediate()) {
    has_values(dest, src);
    ShCloakPtr result = dest.cloak();
    (*ShEval::instance())(SH_OP_LO, result, src.cloak(), 0, 0);
    dest.setCloak(result);
  } else {
    ShStatement stmt(dest, SH_OP_LO, src);
    addStatement(stmt);
  }
}

void shSETLO(ShVariable& dest, const ShVariable& src)
{
  sizes_match(dest, src); // TODO check types are okay
  if(immediate()) {
    has_values(dest, src);
    ShCloakPtr result = dest.cloak();
    (*ShEval::instance())(SH_OP_LO, result, src.cloak(), 0, 0);
    dest.setCloak(result);
  } else {
    ShStatement stmt(dest, SH_OP_LO, src);
    addStatement(stmt);
  }
}

void shSETHI(ShVariable& dest, const ShVariable& src)
{
  sizes_match(dest, src); // TODO check types are okay
  if(immediate()) {
    has_values(dest, src);
    ShCloakPtr result = dest.cloak();
    (*ShEval::instance())(SH_OP_LO, result, src.cloak(), 0, 0);
    dest.setCloak(result);
  } else {
    ShStatement stmt(dest, SH_OP_LO, src);
    addStatement(stmt);
  }
}

void shKIL(const ShVariable& a)
{
  if(immediate()) {
      shError(ShScopeException("Cannot kill in immediate mode"));
  }
  SH_DEBUG_ASSERT(!immediate());
  ShStatement stmt(a, SH_OP_KIL, a);
  addStatement(stmt);
}

}
