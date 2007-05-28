#ifndef MAN_OPS_HPP
#define MAN_OPS_HPP

#include <vector>
#include <sh/sh.hpp>
#include "Man.hpp"

/*************************************
 * Utility functions
 * (SH::should probably look into using boost::mpl if this gets any messier)
 *************************************/

template<int I, int O>
Man<I, O> Man<I, O>::operator-() const {
  SH::Program negger = SH_BEGIN_PROGRAM() {
    typename Man<I, O>::OutType::InOutType val; 
    val = -val;
  } SH_END;
  return negger << *this;
}

/* Returns a swizzle of the output */
template<int I, int O>
Man<I, 1> Man<I, O>::operator()(int i0) const {
  SH::Program swizzer = SH_BEGIN_PROGRAM() {
    typename Man<I, O>::OutType::InputType in; 
    SH::Variable out(in.node()->clone(SH::SH_OUTPUT, 1));
    SH::shASN(out, in(i0));
  } SH_END;
  return swizzer << *this;
};

template<int I, int O>
Man<I, 2> Man<I, O>::operator()(int i0, int i1) const {
  SH::Program swizzer = SH_BEGIN_PROGRAM() {
    typename Man<I, O>::OutType::InputType in; 
    SH::Variable out(in.node()->clone(SH::SH_OUTPUT, 2));
    SH::shASN(out, in(i0, i1));
  } SH_END;
  return swizzer << *this;
};

template<int I, int O>
Man<I, 3> Man<I, O>::operator()(int i0, int i1, int i2) const {
  SH::Program swizzer = SH_BEGIN_PROGRAM() {
    typename Man<I, O>::OutType::InputType in; 
    SH::Variable out(in.node()->clone(SH::SH_OUTPUT, 3));
    SH::shASN(out, in(i0, i1, i2));
  } SH_END;
  return swizzer << *this;
};

template<int I, int O>
Man<I, 4> Man<I, O>::operator()(int i0, int i1, int i2, int i3) const {
  SH::Program swizzer = SH_BEGIN_PROGRAM() {
    typename Man<I, O>::OutType::InputType in; 
    SH::Variable out(in.node()->clone(SH::SH_OUTPUT, 4));
    SH::shASN(out, in(i0, i1, i2, i3));
  } SH_END;
  return swizzer << *this;
};

/* Adds to the number of inputs by sticking in extra 0 inputs */ 
template<int I, int I1, int O1>
Man<I, O1> m_param_cast(const Man<I1, O1>& a) {
  if(I1 == 0) {
    SH::Program extra = SH_BEGIN_PROGRAM() {
      typename Man<I, O1>::InType in; 
    } SH_END;
    return (a & extra);
  } 
  return a << SH::cast<typename Man<I, O1>::InType, typename Man<I1, O1>::InType>("param_cast");
}

/* Casts by adding 0s */
template<int O, int I1, int O1>
Man<I1, O> m_output_cast(const Man<I1, O1>& a) {
  return SH::cast<typename Man<I1, O1>::OutType, typename Man<I1, O>::OutType>("output_cast") << a;
}

/* Replicates last output element to reach size output size O */ 
template<int O, int I1, int O1>
Man<I1, O> m_output_fill(const Man<I1, O1>& a) {
  return SH::fillcast<typename Man<I1, O1>::OutType, typename Man<I1, O>::OutType>("output_cast") << a;
}

template<class M1, class M2>
struct MaxInOut {
  static const int I = IntOp<M1::In, M2::In>::max; 
  static const int O = IntOp<M1::Out, M2::Out>::max; 
  typedef Man<I, O> type;
};

/* Does both param_cast and output cast */ 
template<class M, class M1>
M m_incast_outfill(const M1& a) {
  return m_param_cast<M::In>(m_output_fill<M::Out>(a));
}

template<class M, class M1>
M m_cast_both(const M1& a) {
  return m_param_cast<M::In>(m_output_cast<M::Out>(a));
}

/* Combine operator, plus some extra versions for a few more params 
 * Equivalent to the GENMOD @() operator */ 
template<int I1, int I2, int O1, int O2>
Man<IntOp<I1, I2>::max, O1 + O2> m_combine(const Man<I1, O1>& a, const Man<I2, O2>& b) {
  static const int I = IntOp<I1, I2>::max;
  Man<I, O1> af = m_param_cast<I>(a);
  Man<I, O2> bf = m_param_cast<I>(b);
  return SH::join<typename Man<I, O1>::OutType, typename Man<I, O2>::OutType >("m_combine") <<
         (af & bf) << SH::dup<typename Man<I, O1>::InType>(); 
};

template<int I1, int I2, int I3, int O1, int O2, int O3>
Man<IntOp<I1, I2, I3>::max,  /* this is clearly a bit ugly */ O1 + O2 + O3> 
m_combine(const Man<I1, O1>& a, const Man<I2, O2>& b, const Man<I3, O3>& c) {
  return m_combine(a, m_combine(b, c));
}

template<int I1, int I2, int I3, int I4, int O1, int O2, int O3, int O4>
Man<IntOp<I1, I2, I3, I4>::max, O1 + O2 + O3 + O4>
m_combine(const Man<I1, O1>& a, const Man<I2, O2>& b, const Man<I3, O3>& c, const Man<I4, O4>& d) {
  return m_combine(a, m_combine(b, c, d));
}

/* Differentiate a relative to it's i'th input */
template<int I, int O>
Man<I, O>
m_differentiate(const Man<I, O>& a, int i) {
  SH::Variable in(*a.begin_inputs());
  return differentiate(a, in(i));
}


/*************************************
 * Nibbles 
 *************************************/

#define MAN_UNARY_FUNC(nibble)\
  template<int I, int O>\
  Man<I, O> nibble(const Man<I, O>& a) {\
    typedef typename Man<I, O>::OutType OutType;\
    return SH::nibble<OutType>(#nibble) << a;\
  }

MAN_UNARY_FUNC(abs);
MAN_UNARY_FUNC(acos);
MAN_UNARY_FUNC(acosh);
MAN_UNARY_FUNC(asin);
MAN_UNARY_FUNC(asinh);
MAN_UNARY_FUNC(atan);
MAN_UNARY_FUNC(atanh);
MAN_UNARY_FUNC(cbrt);
MAN_UNARY_FUNC(ceil);
MAN_UNARY_FUNC(cos);
MAN_UNARY_FUNC(cosh);
//MAN_UNARY_FUNC(dx);
//MAN_UNARY_FUNC(dy);
MAN_UNARY_FUNC(exp);
MAN_UNARY_FUNC(expm1);
MAN_UNARY_FUNC(exp2);
MAN_UNARY_FUNC(exp10);
MAN_UNARY_FUNC(floor);
MAN_UNARY_FUNC(frac);
//MAN_UNARY_FUNC(fwidth);
MAN_UNARY_FUNC(log);
MAN_UNARY_FUNC(logp1);
MAN_UNARY_FUNC(log2);
MAN_UNARY_FUNC(log10);
MAN_UNARY_FUNC(normalize);
//MAN_UNARY_FUNC(logical_not);
MAN_UNARY_FUNC(pos);
MAN_UNARY_FUNC(rcp);
MAN_UNARY_FUNC(round);
MAN_UNARY_FUNC(rsqrt);
MAN_UNARY_FUNC(sat);
MAN_UNARY_FUNC(sign);
MAN_UNARY_FUNC(sin);
MAN_UNARY_FUNC(sinh);
MAN_UNARY_FUNC(sort);
MAN_UNARY_FUNC(sqrt);
MAN_UNARY_FUNC(tan);
MAN_UNARY_FUNC(tanh);

/* arithmetic operators */
#define MAN_BINARY_OP(op, nibble)\
  template<int I1, int O1, int I2, int O2>\
  typename MaxInOut<Man<I1, O1>, Man<I2, O2> >::type  operator op(const Man<I1, O1>& a, const Man<I2, O2>& b) {\
    typedef typename MaxInOut<Man<I1, O1>, Man<I2, O2> >::type M;\
    M ac = m_incast_outfill<M>(a);\
    M bc = m_incast_outfill<M>(b);\
    return SH::nibble<typename M::OutType>() << (ac & bc) << SH::dup<typename M::InType>();\
  }\
  template<int I1, int O1>\
  typename MaxInOut<Man<I1, O1>, Man<0, 1> >::type  operator op(const Man<I1, O1>& a, float b) { return operator op(a, m_(b)); }\
  template<int I2, int O2>\
  typename MaxInOut<Man<0, 1>, Man<I2, O2> >::type  operator op(float a, const Man<I2, O2>& b) { return operator op(m_(a), b); }\
  template<int I1, int O1, int O2>\
  typename MaxInOut<Man<I1, O1>, Man<0, O2> >::type  operator op(const Man<I1, O1>& a, const SH::Generic<O2, float>& b) { return operator op(a, m_(b)); }\
  template<int O1, int I2, int O2>\
  typename MaxInOut<Man<0, O1>, Man<I2, O2> >::type  operator op(const SH::Generic<O1, float>& a, const Man<I2, O2>& b) { return operator op(m_(a), b); }

#define MAN_BINARY_FUNC(nibble)\
  template<int I1, int O1, int I2, int O2>\
  typename MaxInOut<Man<I1, O1>, Man<I2, O2> >::type nibble(const Man<I1, O1>& a, const Man<I2, O2>& b) {\
    typedef typename MaxInOut<Man<I1, O1>, Man<I2, O2> >::type M;\
    M ac = m_incast_outfill<M>(a);\
    M bc = m_incast_outfill<M>(b);\
    return SH::nibble<typename M::OutType>() << (ac & bc) << SH::dup<typename M::InType>();\
  }\
  template<int I1, int O1>\
  typename MaxInOut<Man<I1, O1>, Man<0, 1> >::type  nibble(const Man<I1, O1>& a, float b) { return nibble(a, m_(b)); }\
  template<int I2, int O2>\
  typename MaxInOut<Man<0, 1>, Man<I2, O2> >::type  nibble(float a, const Man<I2, O2>& b) { return nibble(m_(a), b); }\
  template<int I1, int O1, int O2>\
  typename MaxInOut<Man<I1, O1>, Man<0, O2> >::type  nibble(const Man<I1, O1>& a, const SH::Generic<O2, float>& b) { return nibble(a, m_(b)); }\
  template<int O1, int I2, int O2>\
  typename MaxInOut<Man<0, O2>, Man<I2, O2> >::type  nibble(const SH::Generic<O2, float>& a, const Man<I2, O2>& b) { return nibble(m_(a), b); }

MAN_BINARY_OP(+, add);
MAN_BINARY_OP(-, sub);
MAN_BINARY_OP(*, mul);
MAN_BINARY_OP(/, div);
MAN_BINARY_OP(%, mod);
MAN_BINARY_OP(&&, logical_and);
MAN_BINARY_OP(||, logical_or);
MAN_BINARY_OP(==, seq);
MAN_BINARY_OP(>=, sge);
MAN_BINARY_OP(>, sgt);
MAN_BINARY_OP(<=, sle);
MAN_BINARY_OP(<, slt);
MAN_BINARY_OP(!=, sne);

/* what about stuff like dot, cross product? */

MAN_BINARY_FUNC(atan2);
MAN_BINARY_FUNC(cross);
MAN_BINARY_FUNC(faceforward);
MAN_BINARY_FUNC(max);
MAN_BINARY_FUNC(min);
MAN_BINARY_FUNC(pow);
MAN_BINARY_FUNC(reflect);


#endif
