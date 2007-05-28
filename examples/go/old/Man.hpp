#ifndef MAN_HPP
#define MAN_HPP

#include <cassert>
#include <cstdarg>

#include <iostream>
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
 * The classes here do something similar, with manifolds acting like normal functions
 * when some value is applied as their inputs.  In cases where we
 * use the function outputs, the inputs are implicitly assumed to
 * be the parametric coordinates (zero-filled as necessary for unused
 * coordinates when multiple inputs have a different number).
 *
 */

/* A Man is a wrapper around a program that defines its parametric 
 * function 
 *
 * I = dimension of inputs
 * O = dimension of outputs
 *
 * Each parameter value in the domain varies betweeen [0, 1] */  

template<class M, class M1> M m_cast_both(const M1& a); 
template<int I, int O>
struct Man: public SH::Program {
  static const int In = I;
  static const int Out = O;
  typedef SH::Attrib<I, SH::SH_INPUT, float, SH::SH_ATTRIB> InType; 
  typedef SH::Attrib<O, SH::SH_OUTPUT, float, SH::SH_ATTRIB> OutType; 

  Man() {}
  Man(const SH::Program& p) { operator=(p); }
  Man& operator=(const Man& m) { SH::Program::operator=(m); return *this; }

  template<int II, int OO>
  Man(const Man<II, OO>& m) {
    operator=(m);
  }

  Man(const SH::Generic<Out, float>& value) {
    Program p = SH_BEGIN_PROGRAM() {
      if(I > 0) {
        InType in;
      }
      OutType v = value;
    } SH_END;
    *this = p;
  }

  template<int II, int OO>
  Man& operator=(const Man<II, OO>& m) {
    if(II != I) {
      std::cerr << "Warning - assigning man with input size " << II << " to size " << I << std::endl;
    }
    if(OO != O) {
      std::cerr << "Warning - assigning man with output size " << OO << " to size " << O << std::endl;
    }
      
    SH::Program::operator=(m_cast_both<Man>(m));
    return *this;
  }
  Man& operator=(const Program& p) { 
    SH::Program::operator=(p); 
    checkSizes();
    return *this; 
  }

  Man operator-() const;
  Man<I, 1> operator()(int i0) const;
  Man<I, 2> operator()(int i0, int i1) const; 
  Man<I, 3> operator()(int i0, int i1, int i2) const; 
  Man<I, 4> operator()(int i0, int i1, int i2, int i3) const; 

  Man<0, O> operator()(const SH::Generic<I, float>& input) const { 
    Program result = SH::Program::operator()(input);
    return result;
  }

  template<int I1>
  Man<I1, O> operator()(const Man<I1, I>& input) const { return connect(input, *this); }

  void checkSizes() {
    if(node()->outputs.size() == 1) {
      int po = (*begin_outputs())->size();
      if(po != O) {
        std::cerr << "Mismatch O=" << O << " p.outputs=" << po << std::endl;
      }
    } else {
      std::cerr << "Too many outputs: " << node()->outputs.size() << std::endl; 
    }

    if(I == 0 && free_input_count() != 0) {
      std::cerr << "Expecting no inputs, found " << free_input_count() << std::endl;
    }

    switch(free_input_count()) {
      case 0: 
      break;
      case 1: { 
        int pi = (*begin_free_inputs())->size();
        if(pi != I) {
          std::cerr << "Mismatch I=" << I << " p.inputs=" << pi << std::endl;
        }
        break;
      }
      default:
        std::cerr << "Too many free inputs: " << free_input_count() << std::endl; 
    }
  }

};

template<int I, int O>
std::ostream& operator<<(std::ostream& out, const Man<I, O>& m) {
  out << "Man<" << I << "," << O << ">";
  return out;
}

template<int O>
Man<0, O> m_(const SH::Generic<O, float>& value) { return Man<0, O>(value); }

inline Man<0, 1> m_(float value) { return m_(SH::ConstAttrib1f(value)); }

typedef Man<0, 2> Point2D; 
typedef Man<1, 2> Curve2D; 
typedef Man<2, 2> Surface2D; 

typedef Man<0, 3> Point3D; 
typedef Man<1, 3> Curve3D; 
typedef Man<2, 3> Surface3D; 
typedef Man<3, 3> Solid3D; 

/* A Man that returns the i'th parameter */ 
template<int I, int O>
struct ManParam: public Man<I, O> {
  ManParam(int i0, ...) {
    SH::Program p = SH_BEGIN_PROGRAM() {
      typename Man<I, O>::InType SH_NAMEDECL(in, "x");
      typename Man<I, O>::OutType out;
      va_list arguments;
      out[0] = in(i0);
      va_start(arguments, i0);
      for(int j = 1; j < O; ++j) {
        int i = va_arg(arguments, int);
        out[i] = in(i); 
      }
      va_end(arguments);
    } SH_END;
    SH::Program::operator=(p);
  }
};
template<int I> ManParam<I, 1> mx_(int i0) { return ManParam<I, 1>(i0); }
template<int I> ManParam<I, 2> mx_(int i0, int i1) { return ManParam<I, 2>(i0, i1); }
template<int I> ManParam<I, 3> mx_(int i0, int i1, int i2) { return ManParam<I, 3>(i0, i1, i2); }
template<int I> ManParam<I, 4> mx_(int i0, int i1, int i2, int i3) { return ManParam<I, 4>(i0, i1, i2, i3); }

/* The stuff for handling conversions between manifolds */  

/* Basic integer ops for unsigned integers*/

template<int I1, int I2=0, int I3=0, int I4=0>
struct IntOp {
  static const int max = IntOp<I1, IntOp<I2, IntOp<I3, I4>::max12>::max12>::max12; 
  static const int max12 = I1 > I2 ? I1 : I2;
};


#endif
