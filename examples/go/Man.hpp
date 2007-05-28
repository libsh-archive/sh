#ifndef MAN_HPP
#define MAN_HPP


#include <cassert>
#include <iostream>
#include <sstream>
#include <sh/sh.hpp>


/* Generative modelling constructs (a very very small subset of GENMOD)
 *
 * Basic constructs are manifolds (parametric functions with known input/output sizes) 
 * and constants/uniforms (i.e. basic Sh variables)
 *
 * (This is just a bunch of nibble/algebra wrappers that let you use
 * the usual Sh Attrib operators on programs) 
 *
 * In GENMOD, manifolds act just like functions, however they can have
 * their inputs bound to special tagged variables for the parametric coordinates. 
 *
 * Here, the inputs are simply the parametric coordinates, and algebra ops
 * for mangling inputs are used to change the parametric
 * coordinates to an existing function (e.g. changing a curve to 
 * use the second instead of first coordinate).
 */

/* A Man is a wrapper around a program that defines its parametric 
 * function 
 *
 * I = dimension of inputs
 * O = dimension of outputs
 *
 * Each parameter value in the domain varies betweeen [0, 1] */  

struct Man: public SH::Program {

  /* Constructors */
  Man(): m_in(-1), m_out(-1) {}
  Man(const SH::Program& p, const std::string& n="man") { operator=(p); name(n); }

  Man(const SH::Variable& value): m_in(0), m_out(value.size()) {
    buildProgram(value);
  }

  Man(float value): m_in(0), m_out(1) {
    buildProgram(SH::ConstAttrib1f(value));
  }


  Man& operator=(const Program& p); 

  Man operator-() const;

  /* swizzle outputs */
  Man operator()(int i0) const;
  Man operator()(int i0, int i1) const; 
  Man operator()(int i0, int i1, int i2) const; 
  Man operator()(int i0, int i1, int i2, int i3) const; 

  /* Sets name and returns self */
  Man& name(const std::string& n) { node()->name(n); return *this; } 
  Man& name(const char* n) { node()->name(n); return *this; } 
  std::string name() const { return node()->name(); }

  /* resize outputs (repeat last or fill with zeros) */ 
  Man repeat(int size) const;
  Man fill(int size) const;

  /* resize inputs */
  Man resize_inputs(int size) const;

  /* resize both */ 
  Man resize_repeat(int in_size, int out_size) const {
    return resize_inputs(in_size).repeat(out_size);
  }

  Man resize_fill(int in_size, int out_size) const {
    return resize_inputs(in_size).fill(out_size);
  }

  int input_size() const { return m_in; }
  int output_size() const { return m_out; }

  /* Matching function */
  bool input_matches(int size) const { return m_in <= size; }
  bool output_matches(int size) const { return m_out <= size; } 
  bool size_matches(int in_size, int out_size) const {
    return input_matches(in_size) && output_matches(out_size);
  }

  /* function call */
  Man operator()(const SH::Variable& input) const { 
    assert(input_size() == input.size());
    return SH::Program::operator()(input);
  }

  Man operator()(const Man& input) const { return connect(input, *this); }

  /* declares a new input or output variable */
  SH::Variable insize_var(SH::BindingType bindingType) const 
  { return SH::Variable(new SH::VariableNode(bindingType, m_in, SH::SH_FLOAT)); } 

  SH::Variable outsize_var(SH::BindingType bindingType) const 
  { return SH::Variable(new SH::VariableNode(bindingType, m_out, SH::SH_FLOAT)); } 

  SH::Variable var(SH::BindingType bindingType, int size=0) const
  { return SH::Variable(new SH::VariableNode(bindingType, size, SH::SH_FLOAT)); } 

  private:
    void setCount(); 
    int m_in, m_out;

    void buildProgram(const SH::Variable& value); 
};

/* Basic creation methods */
inline Man m_(const SH::Variable& value) { return Man(value); }
inline Man m_(float value) { return m_(SH::ConstAttrib1f(value)); }

/* Parameter reference methods 
 * m_u(i) references the i'th input parameter 
 * (this is the same as GENMOD's mx_, except while u is commonly used as the first parametric coordinate,
 * x is overloaded in too many confusing ways) */ 
Man m_u(int i0);

/* It is a brief way to indicate for some library functions what
 * the expected parameter/return functions will look like,
 * but no actual type checking goes on */

typedef Man Point2D; /* 0 used input, 2 outputs */
typedef Man Point3D; /* 0 used input, 3 outputs */
typedef Man Curve2D; /* 1 used input, 2 outputs */
typedef Man Curve3D; /* 1 used input, 2 outputs */
typedef Man Surface3D; /* 2 used inputs, 3 outputs */

#endif
