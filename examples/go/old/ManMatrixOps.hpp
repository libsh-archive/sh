#ifndef MANMATRIX_OPS_HPP
#define MANMATRIX_OPS_HPP

#include <vector>
#include <sh/sh.hpp>
#include "man.hpp"
#include "ManMatrix.hpp"

/* These are the analog of LibMatrix.hpp in the sh library */

/* Adds to the number of inputs by sticking in extra 0 inputs */ 
template<int I, int I1, int R, int C>
ManMatrix<I, R, C> m_param_cast(const ManMatrix<I1, R, C>& m) {
  if(I1 == 0) {
    SH::Program extra = SH_BEGIN_PROGRAM() {
      typename ManMatrix<I, R, C>::InType in; 
    } SH_END;
    return (m & extra);
  } 
  return m << SH::cast<typename ManMatrix<I, R, C>::InType, typename ManMatrix<I1, R, C>::InType>("param_cast");
}

template<int I>
ManMatrix<I, 4, 4> 
m_scale(const Man<I, 3> &s) {
  SH::Program p = SH_BEGIN_PROGRAM() {
    typename Man<I, 3>::OutType::InputType in; 
    typename ManMatrix<I, 4, 4>::OutputType out;
    out = scale(in);
  } SH_END;
  return p << s; 
}

template<int I>
ManMatrix<I, 4, 4> m_translate(const Man<I, 3> &t) {
  SH::Program p = SH_BEGIN_PROGRAM() {
    typename Man<I, 3>::OutType::InputType in; 
    typename ManMatrix<I, 4, 4>::OutputType out;
    out = translate(in); 
  } SH_END;
  return p << t; 
}

template<int I1, int I2>
ManMatrix<IntOp<I1, I2>::max, 4, 4> m_rotate(const Man<I1, 3> &axis, const Man<I2, 1>& angle) {
  static const int I = IntOp<I1, I2>::max;
  Man<I, 3> axisf = m_param_cast<I>(axis);
  Man<I, 1> anglef = m_param_cast<I>(angle);
  SH::Program p = SH_BEGIN_PROGRAM() {
    typename Man<I, 3>::InputType in;
    typename ManMatrix<I, 4, 4>::OutputType out;
    typename Man<I, 3>::OutType::TempType ax; 
    typename Man<I, 1>::OutType::TempType ang; 

    ax = axisf(in);
    ang = anglef(in);
    out = rotate(ax, ang); 
  } SH_END;
  return p; 
}

template<int I1, int I2, int N> 
Man<IntOp<I1, I2>::max, N-1> operator|(const ManMatrix<I1, N, N>& m, const Man<I2, N-1>& x) {
  static const int I = IntOp<I1, I2>::max;
  ManMatrix<I, N, N> mc = m_param_cast<I>(m);
  Man<I, N-1> xc = m_param_cast<I>(x);

  SH::Program p = SH_BEGIN_PROGRAM() {
    typename ManMatrix<I, N, N>::OutType::InputType SH_DECL(inMatrix);
    typename Man<I, N - 1>::OutType::InputType SH_DECL(inVector);
    typename Man<I, N - 1>::OutType SH_DECL(result);
    result = inMatrix | inVector;
  } SH_END;
  return p << (m & x);
}

template<int I1, int I2, int N> 
Man<IntOp<I1, I2>::max, N> operator|(const ManMatrix<I1, N, N>& m, const Man<I2, N>& x) {
  static const int I = IntOp<I1, I2>::max;
  ManMatrix<I, N, N> mc = m_param_cast<I>(m);
  Man<I, N> xc = m_param_cast<I>(x);

  SH::Program p = SH_BEGIN_PROGRAM() {
    typename ManMatrix<I, N, N>::OutType::InputType SH_DECL(inMatrix);
    typename Man<I, N>::OutType::InputType SH_DECL(inVector);
    typename Man<I, N>::OutType SH_DECL(result);
    result = inMatrix | inVector;
  } SH_END;
  return p << (m & x);
}



#endif
