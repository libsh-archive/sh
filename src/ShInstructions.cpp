#include <cmath>
#include "ShInstructions.hpp"
#include "ShStatement.hpp"
#include "ShEnvironment.hpp"
#include "ShDebug.hpp"

namespace {

using namespace SH;

bool immediate()
{
  return !ShEnvironment::insideShader;
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
  ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
}

typedef float T;

T slt(T a, T b) { return a < b ? 1.0f : 0.0f; }
T sle(T a, T b) { return a <= b ? 1.0f : 0.0f; }
T sgt(T a, T b) { return a > b ? 1.0f : 0.0f; }
T sge(T a, T b) { return a >= b ? 1.0f : 0.0f; }
T seq(T a, T b) { return a == b ? 1.0f : 0.0f; }
T sne(T a, T b) { return a != b ? 1.0f : 0.0f; }
T max(T a, T b) { return a > b ? a : b; }
T min(T a, T b) { return a < b ? a : b; }
T lrp(T alpha, T a, T b) { return alpha*a + (1.0f-alpha)*b; }
T cond(T alpha, T a, T b) { return alpha > 0.0f ? a : b; }
T mad(T a, T b, T c) { return a * b + c; }
T frac(T a) { return fmodf(a, 1.0f); }
}

#define CWISE_BINARY_OP(d, a, b, op) \
do {\
  has_values(d, a, b);\
  T v_d[d.size()];\
  T v_a[a.size()];\
  a.getValues(v_a);\
  T v_b[b.size()];\
  b.getValues(v_b);\
  for (int i = 0; i < d.size(); i++) v_d[i] = op(v_a[(a.size() == 1 ? 0 : i)],\
                                                 v_b[(b.size() == 1 ? 0 : i)]);\
  d.setValues(v_d);\
} while (0)

#define CWISE_BINARY_INLOP(d, a, b, op) \
do {\
  has_values(d, a, b);\
  T v_d[d.size()];\
  T v_a[a.size()];\
  a.getValues(v_a);\
  T v_b[b.size()];\
  b.getValues(v_b);\
  for (int i = 0; i < d.size(); i++) v_d[i] = v_a[(a.size() == 1 ? 0 : i)] op\
                                              v_b[(b.size() == 1 ? 0 : i)];\
  d.setValues(v_d);\
} while (0)

#define CWISE_TRINARY_OP(d, a, b, c, op) \
do {\
  has_values(d, a, b, c);\
  T v_a[a.size()];\
  a.getValues(v_a);\
  T v_b[b.size()];\
  b.getValues(v_b);\
  T v_c[c.size()];\
  c.getValues(v_c);\
  T v_d[d.size()];\
  for (int i = 0; i < d.size(); i++) v_d[i] = op(v_a[(a.size() == 1 ? 0 : i)],\
                                                 v_b[(b.size() == 1 ? 0 : i)],\
                                                 v_c[(c.size() == 1 ? 0 : i)]\
                                                 );\
  d.setValues(v_d);\
} while (0)

#define CWISE_UNARY_OP(d, a, op) \
do {\
  has_values(d, a);\
  T v_a[a.size()];\
  T v_d[d.size()];\
  a.getValues(v_a);\
  for (int i = 0; i < d.size(); i++) v_d[i] = op(v_a[(a.size() == 1 ? 0 : i)]);\
  d.setValues(v_d);\
} while (0)

namespace SH {

void shASN(ShVariable& dest, const ShVariable& src)
{
  sizes_match(dest, src);
  if (immediate()) {
    has_values(dest, src);
    T vals[src.size()];
    src.getValues(vals);
    dest.setValues(vals);
  } else {
    ShStatement stmt(dest, SH_OP_ASN, src);
    addStatement(stmt);
  }
}

void shADD(ShVariable& dest, const ShVariable& a, const ShVariable& b)
{
  sizes_match(dest, a, b);
  if (immediate()) {
    CWISE_BINARY_INLOP(dest, a, b, +);
  } else {
    ShStatement stmt(dest, a, SH_OP_ADD, b);
    addStatement(stmt);
  }
}

void shMUL(ShVariable& dest, const ShVariable& a, const ShVariable& b)
{
  sizes_match(dest, a, b, true, true);
  if (immediate()) {
    CWISE_BINARY_INLOP(dest, a, b, *);
  } else {
    ShStatement stmt(dest, a, SH_OP_MUL, b);
    addStatement(stmt);
  }
}

void shDIV(ShVariable& dest, const ShVariable& a, const ShVariable& b)
{
  sizes_match(dest, a, b, false, true);
  if (immediate()) {
    CWISE_BINARY_INLOP(dest, a, b, /);
  } else {
    ShStatement stmt(dest, a, SH_OP_DIV, b);
    addStatement(stmt);
  }
}

void shSLT(ShVariable& dest, const ShVariable& a, const ShVariable& b)
{
  sizes_match(dest, a, b, true, true);
  if (immediate()) {
    CWISE_BINARY_OP(dest, a, b, slt);
  } else {
    ShStatement stmt(dest, a, SH_OP_SLT, b);
    addStatement(stmt);
  }
}

void shSLE(ShVariable& dest, const ShVariable& a, const ShVariable& b)
{
  sizes_match(dest, a, b, true, true);
  if (immediate()) {
    CWISE_BINARY_OP(dest, a, b, sle);
  } else {
    ShStatement stmt(dest, a, SH_OP_SLE, b);
    addStatement(stmt);
  }
}

void shSGT(ShVariable& dest, const ShVariable& a, const ShVariable& b)
{
  sizes_match(dest, a, b, true, true);
  if (immediate()) {
    CWISE_BINARY_OP(dest, a, b, sgt);
  } else {
    ShStatement stmt(dest, a, SH_OP_SGT, b);
    addStatement(stmt);
  }
}

void shSGE(ShVariable& dest, const ShVariable& a, const ShVariable& b)
{
  sizes_match(dest, a, b, true, true);
  if (immediate()) {
    CWISE_BINARY_OP(dest, a, b, sge);
  } else {
    ShStatement stmt(dest, a, SH_OP_SGE, b);
    addStatement(stmt);
  }
}

void shSEQ(ShVariable& dest, const ShVariable& a, const ShVariable& b)
{
  sizes_match(dest, a, b, true, true);
  if (immediate()) {
    CWISE_BINARY_OP(dest, a, b, seq);
  } else {
    ShStatement stmt(dest, a, SH_OP_SEQ, b);
    addStatement(stmt);
  }
}

void shSNE(ShVariable& dest, const ShVariable& a, const ShVariable& b)
{
  sizes_match(dest, a, b, true, true);
  if (immediate()) {
    CWISE_BINARY_OP(dest, a, b, sne);
  } else {
    ShStatement stmt(dest, a, SH_OP_SNE, b);
    addStatement(stmt);
  }
}

void shABS(ShVariable& dest, const ShVariable& a)
{
  sizes_match(dest, a);
  if (immediate()) {
    CWISE_UNARY_OP(dest, a, std::fabs);
  } else {
    ShStatement stmt(dest, SH_OP_ABS, a);
    addStatement(stmt);
  }
}

void shACOS(ShVariable& dest, const ShVariable& a)
{
  sizes_match(dest, a);
  if (immediate()) {
    CWISE_UNARY_OP(dest, a, std::acos);
  } else {
    ShStatement stmt(dest, SH_OP_ACOS, a);
    addStatement(stmt);
  }
}

void shASIN(ShVariable& dest, const ShVariable& a)
{
  sizes_match(dest, a);
  if (immediate()) {
    CWISE_UNARY_OP(dest, a, std::asin);
  } else {
    ShStatement stmt(dest, SH_OP_ASIN, a);
    addStatement(stmt);
  }
}

void shATAN(ShVariable& dest, const ShVariable& a)
{
  sizes_match(dest, a);
  if (immediate()) {
    CWISE_UNARY_OP(dest, a, std::atan);
  } else {
    ShStatement stmt(dest, SH_OP_ATAN, a);
    addStatement(stmt);
  }
}

void shATAN2(ShVariable& dest, const ShVariable& a, const ShVariable& b)
{
  sizes_match(dest, a, b);
  if (immediate()) {
    CWISE_BINARY_OP(dest, a, b, atan2f);
  } else {
    ShStatement stmt(dest, a, SH_OP_ATAN2, b);
    addStatement(stmt);
  }
}

void shCEIL(ShVariable& dest, const ShVariable& a)
{
  sizes_match(dest, a);
  if (immediate()) {
    CWISE_UNARY_OP(dest, a, ceilf);
  } else {
    ShStatement stmt(dest, SH_OP_CEIL, a);
    addStatement(stmt);
  }
}

void shCOS(ShVariable& dest, const ShVariable& a)
{
  sizes_match(dest, a);
  if (immediate()) {
    CWISE_UNARY_OP(dest, a, std::cos);
  } else {
    ShStatement stmt(dest, SH_OP_COS, a);
    addStatement(stmt);
  }
}

void shDOT(ShVariable& dest, const ShVariable& a, const ShVariable& b)
{
  SH_DEBUG_ASSERT(dest.size() == 1);
  sizes_match(a, b);
  if (immediate()) {
    T v_a[a.size()];
    a.getValues(v_a);
    T v_b[a.size()];
    b.getValues(v_b);
    T f = 0.0;
    for (int i = 0; i < a.size(); i++) f += v_a[i] * v_b[i];
    dest.setValues(&f);
  } else {
    ShStatement stmt(dest, a, SH_OP_DOT, b);
    addStatement(stmt);
  }
}

void shFLR(ShVariable& dest, const ShVariable& a)
{
  sizes_match(dest, a);
  if (immediate()) {
    CWISE_UNARY_OP(dest, a, floorf);
  } else {
    ShStatement stmt(dest, SH_OP_FLR, a);
    addStatement(stmt);
  }
}

void shFMOD(ShVariable& dest, const ShVariable& a, const ShVariable& b)
{
  sizes_match(dest, a, b);
  if (immediate()) {
    CWISE_BINARY_OP(dest, a, b, fmodf);
  } else {
    ShStatement stmt(dest, a, SH_OP_FMOD, b);
    addStatement(stmt);
  }
}

void shFRAC(ShVariable& dest, const ShVariable& a)
{
  sizes_match(dest, a);
  if (immediate()) {
    CWISE_UNARY_OP(dest, a, frac);
  } else {
    ShStatement stmt(dest, SH_OP_FRAC, a);
    addStatement(stmt);
  }
}

void shLRP(ShVariable& dest, const ShVariable& alpha,
           const ShVariable& a, const ShVariable& b)
{
  SH_DEBUG_ASSERT(dest.size() == a.size() &&
                  dest.size() == b.size() &&
                  (dest.size() == alpha.size() || alpha.size() == 1));
  if (immediate()) {
    CWISE_TRINARY_OP(dest, alpha, a, b, lrp);
  } else {
    ShStatement stmt(dest, SH_OP_LRP, alpha, a, b);
    addStatement(stmt);
  }
}

void shMAD(ShVariable& dest, const ShVariable& a,
           const ShVariable& b, const ShVariable& c)
{
  SH_DEBUG_ASSERT(dest.size() == c.size() &&
                  (dest.size() == a.size() || (dest.size() == b.size() && a.size() == 1)) &&
                  (dest.size() == b.size() || (dest.size() == a.size() && b.size() == 1)));
  if (immediate()) {
    CWISE_TRINARY_OP(dest, a, b, c, mad);
  } else {
    ShStatement stmt(dest, SH_OP_MAD, a, b, c);
    addStatement(stmt);
  }
}

void shMAX(ShVariable& dest, const ShVariable& a, const ShVariable& b)
{
  sizes_match(dest, a, b);
  if (immediate()) {
    CWISE_BINARY_OP(dest, a, b, max);
  } else {
    ShStatement stmt(dest, a, SH_OP_MAX, b);
    addStatement(stmt);
  }
}

void shMIN(ShVariable& dest, const ShVariable& a, const ShVariable& b)
{
  sizes_match(dest, a, b);
  if (immediate()) {
    CWISE_BINARY_OP(dest, a, b, min);
  } else {
    ShStatement stmt(dest, a, SH_OP_MIN, b);
    addStatement(stmt);
  }
}

void shPOW(ShVariable& dest, const ShVariable& a, const ShVariable& b)
{
  sizes_match(dest, a, b, false, true);
  if (immediate()) {
    CWISE_BINARY_OP(dest, a, b, powf);
  } else {
    ShStatement stmt(dest, a, SH_OP_POW, b);
    addStatement(stmt);
  }
}

void shSIN(ShVariable& dest, const ShVariable& a)
{
  sizes_match(dest, a);
  if (immediate()) {
    CWISE_UNARY_OP(dest, a, std::sin);
  } else {
    ShStatement stmt(dest, SH_OP_SIN, a);
    addStatement(stmt);
  }
}

void shSQRT(ShVariable& dest, const ShVariable& a)
{
  sizes_match(dest, a);
  if (immediate()) {
    CWISE_UNARY_OP(dest, a, std::sqrt);
  } else {
    ShStatement stmt(dest, SH_OP_SQRT, a);
    addStatement(stmt);
  }
}

void shNORM(ShVariable& dest, const ShVariable& a)
{
  sizes_match(dest, a);
  if (immediate()) {
    T v_a[a.size()];
    a.getValues(v_a);
    T s = 0.0;
    for (int i = 0; i < a.size(); i++) s += v_a[i] * v_a[i];
    s = std::sqrt(s);
    for (int i = 0; i < a.size(); i++) v_a[i] /= s;
    dest.setValues(v_a);
  } else {
    ShStatement stmt(dest, SH_OP_NORM, a);
    addStatement(stmt);
  }
}

void shXPD(ShVariable& dest, const ShVariable& a, const ShVariable& b)
{
  SH_DEBUG_ASSERT(dest.size() == 3 && a.size() == 3 && b.size() == 3);
  if (immediate()) {
    T v_a[3];
    a.getValues(v_a);
    T v_b[3];
    b.getValues(v_b);
    T result[3];
    result[0] = v_a[1] * v_b[2] - v_a[2] * v_b[1];
    result[1] = -(v_a[0] * v_b[2] - v_a[2] * v_b[0]);
    result[2] = v_a[0] * v_b[1] - v_a[1] * v_b[0];
    dest.setValues(result);
  } else {
    ShStatement stmt(dest, a, SH_OP_XPD, b);
    addStatement(stmt);
  }
}

void shCOND(ShVariable& dest, const ShVariable& alpha,
            const ShVariable& a, const ShVariable& b)
{
  SH_DEBUG_ASSERT(dest.size() == a.size() &&
                  dest.size() == b.size() &&
                  (dest.size() == alpha.size() || alpha.size() == 1));
  if (immediate()) {
    CWISE_TRINARY_OP(dest, alpha, a, b, cond);
  } else {
    ShStatement stmt(dest, SH_OP_COND, alpha, a, b);
    addStatement(stmt);
  }
}

void shKIL(const ShVariable& a)
{
  SH_DEBUG_ASSERT(!immediate());
  ShStatement stmt(a, SH_OP_KIL, a);
  addStatement(stmt);
}

}
