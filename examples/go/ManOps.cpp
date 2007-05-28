#include <cctype>
#include <sh/sh.hpp>
#include "Man.hpp"
#include "ManOps.hpp"

using namespace std;
using namespace SH;

/*************************************
 * Utility functions
 *************************************/
Man m_combine(const Man& a, const Man& b) {
  Man af = m_resize_inputs(a, b); 
  Man bf = m_resize_inputs(b, a); 
  int ao = af.output_size();
  int bo = bf.output_size();
  int* swiz = new int[ao + bo];
  for(int i = 0; i < ao + bo; ++i) swiz[i] = i;
  Program p = SH_BEGIN_PROGRAM() {
    Variable in_a = af.outsize_var(SH_INPUT);
    Variable in_b = bf.outsize_var(SH_INPUT);
    Variable out = af.var(SH_OUTPUT, ao + bo); 
    Variable out_prefix = out(ao, swiz);
    Variable out_suffix = out(bo, swiz + ao);
    shASN(out_prefix, in_a);
    shASN(out_suffix, in_b);
  } SH_END;
  delete[] swiz;
  Program result = p << (af & bf);
  if(af.input_size() > 0) result = result << SH::dup();
  return Man(result, "m_combine(" + a.name() + "," + b.name() + ")");
};

Man m_unary_op(Operation op_code, const std::string& op_name, const Man& a) {
  Program p = SH_BEGIN_PROGRAM() {
    Variable inout = a.outsize_var(SH_INOUT);
    Context::current()->parsing()->tokenizer.blockList()->addStatement(
      Statement(inout, op_code, inout));
  } SH_END;
  return Man(p << a, op_name + "(" + a.name() + ")"); 
}

Man m_binary_op(Operation op_code, const std::string& op_name, const Man& a, const Man& b, int result_size) {
  Man af = m_resize_both_repeat(a, b); 
  Man bf = m_resize_both_repeat(b, a); 
  Program p = SH_BEGIN_PROGRAM() {
    Variable in_a = af.outsize_var(SH_INPUT);
    Variable in_b = bf.outsize_var(SH_INPUT);
    Variable out;
    if(result_size == 0) out = af.outsize_var(SH_OUTPUT);
    else out = af.var(SH_OUTPUT, result_size);
    Context::current()->parsing()->tokenizer.blockList()->addStatement(
      Statement(out, in_a, op_code, in_b));
  } SH_END;
  std::string name = isalpha(op_name[0]) ? (op_name + "(" + a.name() + "," + b.name() + ")"):
                     (a.name() + op_name + b.name());
  Program result = p << (af & bf);
  if(af.input_size() > 0) result = result << SH::dup();
  return Man(result, name);
}

Man m_ternary_op(Operation op_code, const std::string& op_name, const Man& a, const Man& b, const Man& c) {
  int maxIn = max(a.input_size(), b.input_size());
  maxIn = max(maxIn, c.input_size());
  int maxOut = max(a.output_size(), b.output_size());
  maxOut = max(maxOut, c.output_size());

  Man af = a.resize_inputs(maxIn).repeat(maxOut); 
  Man bf = b.resize_inputs(maxIn).repeat(maxOut); 
  Man cf = c.resize_inputs(maxIn).repeat(maxOut); 

  Program p = SH_BEGIN_PROGRAM() {
    Variable in_a = af.outsize_var(SH_INPUT);
    Variable in_b = bf.outsize_var(SH_INPUT);
    Variable in_c = cf.outsize_var(SH_INPUT);
    Variable out = af.outsize_var(SH_OUTPUT);
    Context::current()->parsing()->tokenizer.blockList()->addStatement(
      Statement(out, op_code, in_a, in_b, in_c));
  } SH_END;
  Program result = p << (af & bf & cf);
  if(af.input_size() > 0) result = result << SH::dup(3);
  return Man(result, op_name + "(" + a.name() + "," + b.name() + "," + c.name()); 
}
