#ifndef MAN_OPS_HPP
#define MAN_OPS_HPP

#include <vector>
#include <sh/sh.hpp>
#include "Man.hpp"

/*************************************
 * Utility functions
 *************************************/

/* Makes number of inputs in a match max(a, b) */ 
inline Man m_resize_inputs(const Man& a, const Man& b) {
  int maxIn = std::max(a.input_size(), b.input_size());
  return a.resize_inputs(maxIn);
}

inline Man m_resize_both_fill(const Man& a, const Man& b) {
  int maxIn = std::max(a.input_size(), b.input_size());
  int maxOut = std::max(a.output_size(), b.output_size());
  return a.resize_inputs(maxIn).fill(maxOut);
}

inline Man m_resize_both_repeat(const Man& a, const Man& b) {
  int maxIn = std::max(a.input_size(), b.input_size());
  int maxOut = std::max(a.output_size(), b.output_size());
  return a.resize_inputs(maxIn).repeat(maxOut);
}

/* Resizes inputs and joins outputs */
Man m_combine(const Man& a, const Man& b);

inline Man m_combine(const Man& a, const Man& b, const Man& c) {
  return m_combine(a, m_combine(b, c));
}

inline Man m_combine(const Man& a, const Man& b, const Man& c, const Man& d) {
  return m_combine(a, m_combine(b, c, d));
}

/* Differentiate a relative to it's i'th input */
inline Man m_differentiate(const Man& a, int i) {
  SH::Variable in(*a.begin_inputs());
  return differentiate(a, in(i));
}


/*************************************
 * standard operations 
 *************************************/
Man m_unary_op(SH::Operation op_code, const std::string& op_name, const Man& a); 
Man m_binary_op(SH::Operation op_code, const std::string& op_name, const Man& a, const Man& b, int result_size=0); 
Man m_ternary_op(SH::Operation op_code, const std::string& op_name, const Man& a, const Man& b, const Man&c); 

#define MAN_UNARY_FUNC(func, op_code, name) inline Man func(const Man& a) { return m_unary_op(op_code, name, a); }
MAN_UNARY_FUNC(abs, SH::OP_ABS, "abs");
MAN_UNARY_FUNC(acos, SH::OP_ACOS, "acos");
MAN_UNARY_FUNC(acosh, SH::OP_ACOSH, "acosh");
MAN_UNARY_FUNC(asin, SH::OP_ASIN, "asin");
MAN_UNARY_FUNC(asinh, SH::OP_ASINH, "asinh");
MAN_UNARY_FUNC(atan, SH::OP_ATAN, "atan");
MAN_UNARY_FUNC(atanh, SH::OP_ATANH, "atanh");
MAN_UNARY_FUNC(cbrt, SH::OP_CBRT, "cbrt");
MAN_UNARY_FUNC(ceil, SH::OP_CEIL, "ceil");
MAN_UNARY_FUNC(cos, SH::OP_COS, "cos");
MAN_UNARY_FUNC(cosh, SH::OP_COSH, "cosh");
//MAN_UNARY_FUNC(dx, SH::OP_DX, "dx");
//MAN_UNARY_FUNC(dy, SH::OP_DY, "dy");
MAN_UNARY_FUNC(exp, SH::OP_EXP, "exp");
//MAN_UNARY_FUNC(expm1, SH::OP_EXPM1, "expm1");
MAN_UNARY_FUNC(exp2, SH::OP_EXP2, "exp2");
MAN_UNARY_FUNC(exp10, SH::OP_EXP10, "exp10");
MAN_UNARY_FUNC(floor, SH::OP_FLR, "flr");
MAN_UNARY_FUNC(frac, SH::OP_FRAC, "frac");
//MAN_UNARY_FUNC(fwidth, SH::OP_FWIDTH, "fwidth");
MAN_UNARY_FUNC(log, SH::OP_LOG, "log");
//MAN_UNARY_FUNC(logp1, SH::OP_LOGP1, "logp1");
MAN_UNARY_FUNC(log2, SH::OP_LOG2, "log2");
MAN_UNARY_FUNC(log10, SH::OP_LOG10, "log10");
MAN_UNARY_FUNC(normalize, SH::OP_NORM, "norm");
//MAN_UNARY_FUNC(logical_not, SH::OP_LOGICAL_NOT, "!");
//MAN_UNARY_FUNC(pos, SH::OP_POS, "pos");
MAN_UNARY_FUNC(rcp, SH::OP_RCP, "rcp");
MAN_UNARY_FUNC(round, SH::OP_RND, "rnd");
MAN_UNARY_FUNC(rsqrt, SH::OP_RSQ, "rsq");
//MAN_UNARY_FUNC(sat, SH::OP_SAT, "sat");
MAN_UNARY_FUNC(sign, SH::OP_SGN, "sgn");
MAN_UNARY_FUNC(sin, SH::OP_SIN, "sin");
MAN_UNARY_FUNC(sinh, SH::OP_SINH, "sinh");
//MAN_UNARY_FUNC(sort, SH::OP_SORT, "sort");
MAN_UNARY_FUNC(sqrt, SH::OP_SQRT, "sqrt");
MAN_UNARY_FUNC(tan, SH::OP_TAN, "tan");
MAN_UNARY_FUNC(tanh, SH::OP_TANH, "tanh");

/* arithmetic operators */
#define MAN_BINARY_FUNC(func, op_code, name) inline Man func(const Man& a, const Man& b) { return m_binary_op(op_code, name, a, b); }

MAN_BINARY_FUNC(operator +, SH::OP_ADD, "+");
MAN_BINARY_FUNC(operator *, SH::OP_MUL, "*");
MAN_BINARY_FUNC(operator /, SH::OP_DIV, "/");
MAN_BINARY_FUNC(operator %, SH::OP_MOD, "%");
inline Man operator-(const Man& a, const Man& b) { return m_binary_op(SH::OP_ADD, "+", a, -b); }
/*
MAN_BINARY_FUNC(operator &&, SH::OP_MUL, a, b, "mul, a, b");
MAN_BINARY_FUNC(operator ||, SH::OP_ADD, a, b, "add, a, b");
*/
MAN_BINARY_FUNC(operator ==, SH::OP_SEQ, "==");
MAN_BINARY_FUNC(operator >=, SH::OP_SGE, ">=");
MAN_BINARY_FUNC(operator >, SH::OP_SGT, ">");
MAN_BINARY_FUNC(operator <=, SH::OP_SLE, "<=");
MAN_BINARY_FUNC(operator <, SH::OP_SLT, "<");
MAN_BINARY_FUNC(operator !=, SH::OP_SNE, "!=");

MAN_BINARY_FUNC(atan2, SH::OP_ATAN2, "atan2");
MAN_BINARY_FUNC(cross, SH::OP_XPD, "xpd");
inline Man dot(const Man& a, const Man& b) { return m_binary_op(SH::OP_DOT, "dot", a, b, 1); }
//MAN_BINARY_FUNC(faceforward);
MAN_BINARY_FUNC(max, SH::OP_MAX, "max");
MAN_BINARY_FUNC(min, SH::OP_MIN, "min");
MAN_BINARY_FUNC(pow, SH::OP_POW, "pow");
//MAN_BINARY_FUNC(reflect);

#define MAN_TERNARY_FUNC(func, op_code, name) inline Man func(const Man& a, const Man& b, const Man& c) { return m_ternary_op(op_code, name, a, b, c); }
MAN_TERNARY_FUNC(lerp, SH::OP_LRP, "lrp");
MAN_TERNARY_FUNC(cond, SH::OP_COND, "cond");

#endif
