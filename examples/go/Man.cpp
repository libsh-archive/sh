#include <vector>
#include <cassert>
#include <cstdarg>
#include <sstream>
#include "Man.hpp"

using namespace SH;
using namespace std;

Man& Man::operator=(const Program& p) { 
  Program::operator=(p); 
  setCount();
  return *this; 
}

void Man::setCount() {
  /* update count */
  if(node()->outputs.size() == 1) {
    m_out = (*begin_outputs())->size();
  } else {
    std::cerr << "Too many outputs: " << node()->outputs.size() << " on " << name() << std::endl; 
    m_out = -1;
    assert(false);
  }

  switch(free_input_count()) {
    case 0: m_in = 0; 
    break;
    case 1: 
      m_in = (*begin_free_inputs())->size();
      break;
    default:
      m_in = -1;
      std::cerr << "Too many free inputs: " << free_input_count() << " on " << name() << std::endl; 
      assert(false);
  }
}

Man Man::operator-() const {
  SH::Program negger = SH_BEGIN_PROGRAM() {
    Variable inout = outsize_var(SH_INOUT); 
    shASN(inout, -inout);
  } SH_END;
  return negger << *this;
}

Man Man::operator()(int i0) const {
  SH::Program swizzer = SH_BEGIN_PROGRAM() {
    Variable in = outsize_var(SH_INPUT);
    Variable out = var(SH_OUTPUT, 1);
    SH::shASN(out, in(i0));
  } SH_END;
  return swizzer << *this;
};

Man Man::operator()(int i0, int i1) const {
  SH::Program swizzer = SH_BEGIN_PROGRAM() {
    Variable in = outsize_var(SH_INPUT); 
    Variable out = var(SH_OUTPUT, 2);
    SH::shASN(out, in(i0, i1));
  } SH_END;
  return swizzer << *this;
};

Man Man::operator()(int i0, int i1, int i2) const {
  SH::Program swizzer = SH_BEGIN_PROGRAM() {
    Variable in = outsize_var(SH_INPUT); 
    Variable out = var(SH_OUTPUT, 3);
    SH::shASN(out, in(i0, i1, i2));
  } SH_END;
  return swizzer << *this;
};

Man Man::operator()(int i0, int i1, int i2, int i3) const {
  SH::Program swizzer = SH_BEGIN_PROGRAM() {
    Variable in = outsize_var(SH_INPUT); 
    Variable out = var(SH_OUTPUT, 4);
    SH::shASN(out, in(i0, i1, i2, i3));
  } SH_END;
  return swizzer << *this;
};

Man Man::repeat(int size) const {
  assert(size >= m_out);
  if(size > m_out) {
    int* swiz = new int[size];
    for(int i = 0; i < size; ++i) swiz[i] = i < m_out ? i : m_out - 1; 
    SH::Program swizzer = SH_BEGIN_PROGRAM() {
      Variable in = outsize_var(SH_INPUT);
      Variable out = var(SH_OUTPUT, size);
      shASN(out, in(size, swiz));
    } SH_END;
    delete[] swiz;
    return swizzer << *this;
  } else {
    return *this;
  }
}

Man Man::fill(int size) const {
  assert(size >= m_out);
  if(size > m_out) {
    int* swiz = new int[m_out];
    for(int i = 0; i < m_out; ++i) swiz[i] = i; 
    SH::Program swizzer = SH_BEGIN_PROGRAM() {
      Variable in = outsize_var(SH_INPUT);
      Variable out = var(SH_OUTPUT, size);
      shASN(out, ConstAttrib1f(0.0f).repeat(out.size()));
      Variable out_prefix = out(m_out, swiz);
      shASN(out_prefix, in); 
    } SH_END;
    delete[] swiz;
    return swizzer << *this;
  } else {
    return *this;
  }
}

Man Man::resize_inputs(int size) const {
  assert(size >= m_in);
  if(size > m_in) {
    int* swiz = new int[m_in];
    for(int i = 0; i < m_in; ++i) swiz[i] = i; 
    SH::Program swizzer = SH_BEGIN_PROGRAM() {
      Variable in = var(SH_INPUT, size);
      if(m_in != 0) {
        Variable out = insize_var(SH_OUTPUT);
        shASN(out, in(m_in, swiz));
      }
    } SH_END;
    delete[] swiz;
    return *this << swizzer;
  } else {
    return *this;
  }
}

void Man::buildProgram(const SH::Variable& value) {
  Program p = SH_BEGIN_PROGRAM() {
    SH::Variable out = outsize_var(SH::SH_OUTPUT);
    SH::shASN(out, value);
  } SH_END;
  *this = p;
  std::ostringstream mout;
  mout << value;
  name(mout.str());
}

Man m_u(int i0) {
  ostringstream mout;
  mout << "u" << i0; 

  Program p = SH_BEGIN_PROGRAM() {
    Variable in(new VariableNode(SH_INPUT, i0 + 1, SH_FLOAT));
    Variable out(new VariableNode(SH_OUTPUT, 1, SH_FLOAT));
    out.name(mout.str());
    shASN(out, in(i0));
  } SH_END;
  return Man(p, mout.str());
}

