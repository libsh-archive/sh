#ifndef MANMATRIX_HPP
#define MANMATRIX_HPP

#include <cassert>
#include <cstdarg>

#include <iostream>
#include <sh/sh.hpp>
#include "Man.hpp"


/* Parameterized matrix (always 4x4 right now) */
struct ManMatrix: public SH::Program {
  typedef SH::Matrix<4, 4, SH::SH_OUTPUT, float> OutType; 

  ManMatrix() {}
  ManMatrix(const SH::Program& p) { operator=(p); }

  template<SH::BindingType Binding> 
  ManMatrix(const SH::Matrix<4, 4, Binding, float>& val) {
    SH::Program p = SH_BEGIN_PROGRAM() {
      OutType v = val;
    } SH_END;
    *this = p;
  }

  ManMatrix& operator=(const ManMatrix& m) { SH::Program::operator=(m); return *this; }

  ManMatrix& operator=(const Program& p) { 
    SH::Program::operator=(p); 
    setCount();
    return *this; 
  }

  /* resize inputs */
  ManMatrix resize_inputs(int size) const;

  int input_size() const { return m_in; }

  SH::Variable insize_var(SH::BindingType bindingType) const 
  { return SH::Variable(new SH::VariableNode(bindingType, m_in, SH::SH_FLOAT)); } 

  SH::Variable var(SH::BindingType bindingType, int size=0) const
  { return SH::Variable(new SH::VariableNode(bindingType, size, SH::SH_FLOAT)); } 


  ManMatrix operator()(const Man& input) const { return connect(input, *this); }

  private:
    void setCount(); 
    int m_in;
};

template<SH::BindingType Binding>
ManMatrix m_(const SH::Matrix<4, 4, Binding, float>& value) { return ManMatrix(value); }

template<typename T>
Man mul(const ManMatrix& m, const Man& x) {
  assert(x.output_matches(T::typesize));
  int maxIn = std::max(m.input_size(), x.input_size());
  ManMatrix mf = m.resize_inputs(maxIn);
  Man xf = x.resize_fill(maxIn, T::typesize);
  SH::Program p = SH_BEGIN_PROGRAM() {
    ManMatrix::OutType::InputType SH_DECL(inMatrix);
    typename T::InOutType SH_DECL(in);
    in = inMatrix | in;
  } SH_END;
  SH::Program result = p << (mf & xf); 
  if(maxIn > 0) return result << SH::dup(); 
  return result;
}

#endif
