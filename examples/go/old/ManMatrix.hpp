#ifndef MANMATRIX_HPP
#define MANMATRIX_HPP

#include <cassert>
#include <cstdarg>

#include <iostream>
#include <sh/sh.hpp>
#include "Man.hpp"


/* Parameterized matrix */

template<int I, int R, int C> 
struct ManMatrix: public SH::Program {
  static const int In = I;
  static const int Rows = R; 
  static const int Cols = C; 
  typedef SH::Attrib<I, SH::SH_INPUT, float, SH::SH_ATTRIB> InType; 
  typedef SH::Matrix<R, C, SH::SH_OUTPUT, float> OutType; 

  ManMatrix() {}
  ManMatrix(const SH::Program& p) { operator=(p); }

  template<SH::BindingType Binding> 
  ManMatrix(const SH::Matrix<R, C, Binding, float>& val) {
    *this = SH_BEGIN_PROGRAM() {
      InType in;
      OutType v = val;
    } SH_END;
  }

  ManMatrix& operator=(const ManMatrix& m) { SH::Program::operator=(m); return *this; }

  ManMatrix& operator=(const Program& p) { 
    SH::Program::operator=(p); 
    checkSizes();
    return *this; 
  }

  template<int I1>
  ManMatrix<I1, R, C> operator()(const Man<I1, I>& input) const { return connect(input, *this); }

  void checkSizes() {
    if(node()->inputs.size() != 1) std::cerr << "Too many inputs: " << node()->inputs.size() << std::endl; 
    if(node()->outputs.size() != R) std::cerr << "Wrong number of outputs: " << node()->outputs.size() << " expected R=" << R << std::endl; 
    if(node()->inputs.size() == 1 && node()->outputs.size() == R) { 
      int pi = (*begin_inputs())->size();
      if(pi != I) { 
        std::cerr << "Element count mismatch! ";
        std::cerr << "I=" << I << " p.inputs=" << pi << std::endl; 
      }
      for(SH::ProgramNode::VarList::iterator V = begin_outputs(); V != end_outputs(); ++V) {
        if((*V)->size() != C) {
          std::cerr << "Wrong number of elements in matrix output size=" << (*V)->size() << " C=" << C << std::endl;
        }
      }
    }
  }
};

template<int R, int C, SH::BindingType Binding>
ManMatrix<0, R, C> m_(const SH::Matrix<R, C, Binding, float>& value) { return ManMatrix<0, R, C>(value); }

template<int I, int R, int C>
std::ostream& operator<<(std::ostream& out, const ManMatrix<I, R, C>& m) {
  out << "ManMatrix<" << I << "," << R << "," << C << ">";
  return out;
}

#endif
