// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////
#ifndef SHAFFINEIMPL_HPP
#define SHAFFINEIMPL_HPP

#include <cmath>
#include <numeric>
#include "Math.hpp"
#include "Affine.hpp"
#include "Debug.hpp"

namespace SH {

template<typename T>
typename AffineSymbol<T>::index_type AffineSymbol<T>::m_max_index = 0;

template<typename T>
inline
AffineSymbol<T>::AffineSymbol()
{}

template<typename T>
inline
AffineSymbol<T>::AffineSymbol(index_type index, data_type err)
  : m_index(index), m_err(err)
{
}

template<typename T>
inline
AffineSymbol<T>::AffineSymbol(data_type err)
  : m_index(m_max_index++), m_err(err)
{
}

template<typename T>
template<typename T2>
inline
AffineSymbol<T>::AffineSymbol(const AffineSymbol<T2> &other)
  : m_index(other.m_index), m_err(other.m_err)
{
}

template<typename T>
inline typename AffineSymbol<T>::data_type&
AffineSymbol<T>::err()
{
  return m_err;
}

template<typename T>
inline typename AffineSymbol<T>::data_type
AffineSymbol<T>::err() const
{
  return m_err;
}

template<typename T>
inline typename AffineSymbol<T>::index_type&
AffineSymbol<T>::index()
{
  return m_index;
}

template<typename T>
inline typename AffineSymbol<T>::index_type
AffineSymbol<T>::index() const
{
  return m_index;
}

template<typename T>
inline typename AffineSymbol<T>::data_type
AffineSymbol<T>::abs_err() const
{
  return m_err > 0 ? m_err : -m_err;
}

template<typename T>
inline AffineSymbol<T>& AffineSymbol<T>::operator=(const AffineSymbol<T>& other) 
{
  m_index = other.m_index;
  m_err = other.m_err;
  return *this;
}

template<typename T>
template<typename T2>
inline AffineSymbol<T>& AffineSymbol<T>::operator=(const AffineSymbol<T2>& other) 
{
  m_index = other.m_index;
  m_err = other.m_err;
  return *this;
}

template<typename T>
inline bool AffineSymbol<T>::operator<(const AffineSymbol& other) const
{
  return m_index < other.m_index;
}

template<typename T>
inline AffineSymbol<T> AffineSymbol<T>::operator-() const
{
  return AffineSymbol(m_index, -m_err); 
}

template<typename T>
inline AffineSymbol<T> AffineSymbol<T>::operator+(const AffineSymbol& other) const
{
  return AffineSymbol(m_index, m_err + other.m_err); 
}

template<typename T>
inline AffineSymbol<T> AffineSymbol<T>::operator-(const AffineSymbol& other) const
{
  return AffineSymbol(m_index, m_err - other.m_err); 
}

template<typename T>
inline AffineSymbol<T> AffineSymbol<T>::operator*(const T& scale) const
{
  return AffineSymbol(m_index, m_err * scale); 
}

template<typename T>
inline AffineSymbol<T>& AffineSymbol<T>::operator*=(const T& scale) 
{
  m_err *= scale;
  return *this;
}

template<typename T>
inline bool AffineSymbol<T>::operator==(const AffineSymbol& other) const
{
  return m_index == other.m_index && m_err == other.m_err; 
}

template<typename T>
inline bool AffineSymbol<T>::operator!=(const AffineSymbol& other) const
{
  return !((*this) == other);
}

template<typename TT>
std::ostream& operator<<(std::ostream& out, const AffineSymbol<TT>& sym)
{
  TT err = sym.err();
  if(err >= 0) {
    out << "+";
  }
  out << sym.err() << "_" << sym.index();
  return out;
}

template<typename TT>
std::istream& operator>>(std::istream& in, AffineSymbol<TT>& sym)
{
  TT err;
  typename AffineSymbol<TT>::index_type index;
  in >> err;
  in.ignore(1, '_');
  in >> index;
  return in;
}

/** Constructs an affine interval with undefined value */
template<typename T>
Affine<T>::Affine()
  : m_center(0)
{}

/** Constructs an affine interval [value,value] */
template<typename T>
inline
Affine<T>::Affine(const T& value)
  : m_center(value)
{}

/** Constructs an affine interval with the given bounds */
template<typename T>
inline
Affine<T>::Affine(const T& lo, const T& hi)
  : m_center((lo + hi) * 0.5)
{
  add((hi - lo) * 0.5);
}

template<typename T>
inline
Affine<T>::Affine(const Affine<T>& other)
  : m_center(other.m_center), m_syms(other.m_syms)
{
}

template<typename T>
template<typename T2>
inline
Affine<T>::Affine(const Affine<T2>& other)
  : m_center(other.m_center)
{
  m_syms.resize(other.m_syms.size());
  std::copy(other.begin(), other.end(), begin());
}

template<typename T>
template<typename T2>
inline
Affine<T>::Affine(const Interval<T2>& other)
  : m_center(other.center())
{
  add(other.radius());
}

template<typename T>
inline
Affine<T>::~Affine()
{
}

template<typename T>
inline
Affine<T>::operator typename Affine<T>::interval_type() const
{
  return to_interval();
}

template<typename T>
inline
typename Affine<T>::interval_type Affine<T>::to_interval() const
{
  T r = radius();
  return interval_type(m_center - r, m_center + r);
}

template<typename T>
inline
typename Affine<T>::iterator Affine<T>::begin() 
{
  return m_syms.begin();
}

template<typename T>
inline
typename Affine<T>::const_iterator Affine<T>::begin() const
{
  return m_syms.begin();
}

template<typename T>
inline
typename Affine<T>::iterator Affine<T>::end() 
{
  return m_syms.end();
}

template<typename T>
inline
typename Affine<T>::const_iterator Affine<T>::end() const
{
  return m_syms.end();
}

template<typename T>
inline
typename Affine<T>::error_type& Affine<T>::operator[](int i) 
{
  // @todo range - no check of index validity
  return m_syms[i];
}

template<typename T>
inline
const typename Affine<T>::error_type& Affine<T>::operator[](int i) const
{
  return m_syms[i];
}

template<typename T>
inline
int Affine<T>::size() const
{
  return m_syms.size(); 
}

template<typename T>
inline
T Affine<T>::lo() const
{
  return m_center - radius();
}

template<typename T>
inline
T Affine<T>::hi() const
{
  return m_center + radius();
}

/** Useful helpers **/
template<typename T>
inline
const T Affine<T>::width() const
{
  return radius() * 2; 
}

template<typename T>
inline
const T Affine<T>::center() const
{
  return m_center; 
}

template<typename T>
inline
const T Affine<T>::radius() const
{
  T result = 0;
  for(const_iterator I = begin(); I != end(); ++I) result += I->abs_err(); 
  return result; 
}

/** Arithmetic operators **/
template<typename T>
inline
Affine<T>& Affine<T>::operator=(const T& value)
{
  m_center = value;
  m_syms.clear();
  return *this;
}

template<typename T>
Affine<T>& Affine<T>::operator=(const Affine<T>& other)
{
  if(&other != this) {
    m_center = other.m_center;
    resize(other.size());
    std::copy(other.begin(), other.end(), begin());
  }
  return *this;
}

template<typename T>
inline
Affine<T>& Affine<T>::operator+=(const T& value)
{
  m_center += value;
}

template<typename T>
inline
Affine<T>& Affine<T>::operator+=(const Affine<T>& other)
{
  *this = *this + other;
  return *this;
}

template<typename T>
inline
Affine<T>& Affine<T>::operator-=(const T& value)
{
  m_center -= value;
}

template<typename T>
inline
Affine<T>& Affine<T>::operator-=(const Affine<T>& other)
{
  *this = *this - other;
  return *this;
}

template<typename T>
inline
Affine<T>& Affine<T>::operator*=(const T& value)
{
  m_center *= value;
  for(iterator I = begin(); I != end(); ++I) (*I) *= value;
  return *this;
}

template<typename T>
inline
Affine<T>& Affine<T>::operator*=(const Affine<T>& other)
{
  *this = *this * other;
  return *this;
}

template<typename T>
inline
Affine<T>& Affine<T>::operator/=(const T& value)
{
  T inv = 1.0 / value;
  m_center *= inv;
  for(iterator I = begin(); I != end(); ++I) I->err() *= inv;
  return *this; 
}

template<typename T>
inline
Affine<T>& Affine<T>::operator/=(const Affine<T>& other)
{
  *this = *this / other;
  return *this; 
}

template<typename T>
Affine<T>& Affine<T>::operator%=(const T& value)
{
  // % is linear in each interval [n * value, (n+1) * value),
  // then the approximation is perfect.
  // otherwise, the best we can do is value / 2 
  Interval<T> bounds = to_interval();

  int ilow = int(bounds.lo() / value); // takes floor 
  T dhigh = bounds.hi() / value; 

  if(dhigh - ilow > 1) {
    m_center = value * 0.5;
    clear();
    add(value * 0.5);
  } else {
    m_center -= ilow * value;
  }
  return *this;
}

template<typename T>
inline
Affine<T>& Affine<T>::operator%=(const Affine<T>& other)
{
  *this = *this % other;
  return *this;
}

/** Negation **/
template<typename T>
Affine<T> Affine<T>::operator-() const 
{
  Affine result(-m_center);
  result.resize(size());
  const_iterator I = begin();
  iterator R = result.begin();
  for(;I != end(); ++I, ++R) {
    (*R) = -(*I);
  }
  return result; 
}


template<typename T>
inline
void Affine<T>::add(const data_type &err)
{
  m_syms.push_back(error_type(err));
}

template<typename T>
inline
void Affine<T>::add(const error_type& err)
{
  m_syms.push_back(err);
}

template<typename T>
inline
void Affine<T>::add(const_iterator first, const_iterator last)
{
  m_syms.insert(end(), first, last);
}

template<typename T>
inline
void Affine<T>::clear()
{
  m_syms.clear();
}

template<typename T>
inline
void Affine<T>::resize(int size)
{
  return m_syms.resize(size); 
}

template<typename T>
inline
void Affine<T>::reserve(int size)
{
  return m_syms.reserve(size); 
}

/* Convex approx works using the Alternation theorem to find a Chebyshev
 * approximation to the function F::f(x).
 *
 * (Assume f(x) is not linear, but must be convex over the range of the affine
 * interval [lo, hi])
 *
 * For a convex function, the best linear approximation a(x) = alpha & x + beta 
 * over the range of the interval satisfies the following: 
 *
 * Let e(x) = a(x) - f(x), then e(x) attains its max absolute value delta exactly 
 * three times, and two of these are at the endpoints. 
 *
 * Also, the sign of e(x) at these three points alternates.
 *
 * This gives a set of three equations with three unknowns (alpha, beta, delta)
 * I) delta = e(lo) = a(lo) - f(lo) = alpha * lo + beta - f(lo)
 * II) delta = e(hi) = a(hi) - f(hi) = alpha * hi + beta - f(hi)
 * III) -delta = e(ss) = a(ss) - f(ss) = alpha * ss + beta - f(ss)
 *      for some ss in (a,b)
 * (here we let delta be negative since we can take abs(delta) at the end for
 * the error term we really want).
 *
 * First, (I) - (II) gives alpha
 *
 * We can find ss since the error must be a maximum at ss, hence e'(ss) = 0
 * ==> alpha - f'(ss) = 0
 * ==> alpha = f'(ss)
 * Since F::g = (f') inverse
 * ==> g(alpha) = ss
 *
 * Now all that's left to do is to solve the three linear eq'n to get alpha,
 * beta, delta
 * bmd = beta - delta = f(lo) - alpha * lo = f(hi) - alpha * hi (from (I), (II))
 * bpd = beta + delta = f(ss) - alpha * ss (from (III)) 
 *
 * See Cheney's Introduction to Approximation Ch.3, Sec. 4 for more info.
 */
template<typename T>
template<typename F>
inline
Affine<T> Affine<T>::convex_approx() const
{
  return convex_approx<F>(to_interval());
}

template<typename T>
template<typename F>
Affine<T> Affine<T>::convex_approx(const interval_type& bounds) const
{
  T lo = bounds.lo();
  T hi = bounds.hi();
  T flo = F::f(lo);
  T fhi = F::f(hi);

  std::cout << "  convex_approx, lo = " << lo << " hi =  " << hi << std::endl;
  std::cout << "    flo = " << flo << " fhi = " << fhi << std::endl;

  T alpha, ss, bpd, bmd, beta, delta; 
  alpha = (fhi - flo)/(hi - lo); 
  ss = F::dfinv(alpha, lo, hi);
  if(lo > ss || hi < ss) std::cout << "  WARNING - ss out of bounds" << std::endl;
  bmd = flo - alpha * lo; 
  if(F::use_fdf) {
    bpd = F::fdfinv(alpha) - alpha * ss;
  } else {
    bpd = F::f(ss) - alpha * ss;
  }
  beta = (bpd + bmd) / 2;
  delta = (bpd - bmd) / 2;
  std::cout << "  elo = " << (alpha * lo + beta - flo) << std::endl;
  std::cout << "  ehi = " << (alpha * hi + beta - fhi) << std::endl;
  std::cout << "  ess = " << (alpha * ss + beta - F::f(ss)) << std::endl;
  std::cout << "  alpha = " << alpha << " ss = " << ss << std::endl;
  std::cout << "  beta = " << beta << " delta = " << delta << std::endl;

  if(delta < 0) delta = -delta;
  return affine_approx(alpha, beta, delta);
}

template<typename T>
Affine<T> Affine<T>::affine_approx(const T& alpha, const T& beta, const T& delta) const
{
  // generate new affine interval
  Affine result(alpha * m_center + beta);
  result.resize(size() + 1);

  const_iterator I = begin();
  iterator R = result.begin();
  for(;I != end(); ++I, ++R) (*R) = (*I) * alpha;

  // add error term
  (*R) = delta;
  return result;
}

template<typename TT>
std::ostream& operator<<(std::ostream& out, const Affine<TT>& value)
{
  out << "(" << value.center(); 
  typename Affine<TT>::const_iterator I = value.begin();
  for(;I != value.end(); ++I) {
    out << " " << *I; 
  }
  out << ")";
  return out;
}

template<typename TT>
std::istream& operator>>(std::istream& in, Affine<TT>& value)
{
  typedef typename Affine<TT>::error_type error_type; 
  typedef typename Affine<TT>::index_type index_type;
  typedef typename Affine<TT>::data_type  data_type;

  data_type center;
  AffineSymbol<TT> sym;

  // @todo this isn't exactly bullet-proof right now...
  in.ignore(1, '(');
  in >> center;
  Affine<TT> result(center);
  for(;in.peek() != ')';) { 
    in.ignore(1, ' ');
    in >> sym; 
    result.add(sym);
  }
  in.ignore(1, ')');
  return in;
}

/** Arithmetic operators **/
template<typename TT>
Affine<TT> operator+(const Affine<TT>& a, const Affine<TT>& b) 
{
  // start result with correct constant term 
  Affine<TT> result(a.center() + b.center());
  
  // set up capacity for worst case
  result.reserve(a.size() + b.size());

  // + is affine, so match up error syms 
  typename Affine<TT>::const_iterator A, B;
  for(A = a.begin(), B = b.begin(); A != a.end() && B != b.end();) {
    if(*A < *B) {
      result.add(*(A++));
    } else if (*B < *A) {
      result.add(*(B++));
    } else { // *A == *B
      result.add(*(A++) + *(B++));
    }
  }
  if(A != a.end()) result.add(A, a.end());
  if(B != b.end()) result.add(B, b.end());
  return result; 
}


template<typename TT>
Affine<TT> operator-(const Affine<TT>& a, const Affine<TT>& b) 
{
  // start result with correct constant term 
  Affine<TT> result(a.center() - b.center());
  
  // set up capacity for worst case
  result.reserve(a.size() + b.size());

  // - is affine, so match up error syms 
  typename Affine<TT>::const_iterator A, B;
  for(A = a.begin(), B = b.begin(); A != a.end() && B != b.end();) {
    if(*A < *B) {
      result.add(*(A++));
    } else if (*B < *A) {
      result.add(-(*(B++)));
    } else { // *A == *B
      result.add(*(A++) - *(B++));
    }
  }
  if(A != a.end()) result.add(A, a.end());
  while(B != b.end()) result.add(-(*(B++))); 
  return result; 
}

template<typename TT>
Affine<TT> operator*(const Affine<TT>& a, const Affine<TT>& b) 
{
  typedef typename Affine<TT>::error_type error_type; 
  // multiply centers 
  TT ac = a.center();
  TT bc = b.center();
  Affine<TT> result(ac * bc);

  // reserve for worst case 
  result.reserve(a.size() + b.size() + 1);

  // affine approximation 
  typename Affine<TT>::const_iterator A, B;
  for(A = a.begin(), B = b.begin(); A != a.end() && B != b.end();) {
    if(*A < *B) {
      result.add(*(A++) * bc);
    } else if (*B < *A) {
      result.add(*(B++) * ac);
    } else { // *A == *B
      result.add(error_type(A->index(), 
          A->err() * bc + 
          B->err() * ac));
      ++A; ++B;
    }
  }
  while(A != a.end()) result.add(*(A++) * bc);
  while(B != b.end()) result.add(*(B++) * ac);

  // add error symbol
  result.add(a.radius() * b.radius());

  return result;
}

template<typename TT>
Affine<TT> operator*(const TT& a, const Affine<TT>& b) 
{
  Affine<TT> result(a * b.center());

  result.reserve(b.size());
  typename Affine<TT>::const_iterator B;
  for(B = b.begin(); B != b.end(); ++B) {
    result.add((*B) * a);
  }
  return result;
}

template<typename TT>
inline
Affine<TT> operator*(const Affine<TT>& a, const TT& b) 
{
  return b * a; 
}

template<typename TT>
Affine<TT> operator/(const Affine<TT>& a, const Affine<TT>& b) 
{
  return a * rcp(b);
}

template<typename TT>
Affine<TT> operator/(const Affine<TT>& a, const TT& b) 
{
  return a * (1/b); 
}


template<typename TT>
Affine<TT> operator%(const Affine<TT>& a, const Affine<TT>& b) 
{
  // @todo range - is this optimal? - probably not
  Affine<TT> quotient = a / b; 
  return quotient - floor(quotient);
}

template<typename TT>
Affine<TT> cbrt(const Affine<TT>& a) 
{
  // @todo range fix this,  have integer power special case?
  return Affine<TT>(cbrt(a.to_interval()));
}

template<typename TT>
struct __sh_aa_exp {
  static const bool use_fdf = true;
  static TT f(TT x) { return std::exp(x); }
  static TT dfinv(TT x, TT low=0, TT high=0) { return std::log(x); }
  static TT fdfinv(TT x) { return x; }
};

template<typename TT>
Affine<TT> exp(const Affine<TT>& a) 
{
  return a.template convex_approx<__sh_aa_exp<TT> >();
}


template<typename TT>
Affine<TT> exp2(const Affine<TT>& a) 
{
  return exp(static_cast<TT>(std::log(2.0)) * a); 
}


template<typename TT>
Affine<TT> exp10(const Affine<TT>& a) 
{
  return exp(static_cast<TT>(std::log(10.0)) * a); 
}

template<typename TT>
struct __sh_aa_log {
  static const bool use_fdf = true;
  static TT f(TT x) { return std::log(x); }
  static TT dfinv(TT x, TT low=0, TT high=0) { return 1.0/x; }
  static TT fdfinv(TT x) { return -std::log(x); }
};

template<typename TT>
Affine<TT> log(const Affine<TT>& a) 
{
  return a. template convex_approx<__sh_aa_log<TT> >();
}


template<typename TT>
Affine<TT> log2(const Affine<TT>& a) 
{
  return log(a) / std::log(2.0); 
}


template<typename TT>
Affine<TT> log10(const Affine<TT>& a) 
{
  return log(a) / static_cast<TT>(std::log(10.0)); 
}

template<typename TT>
Affine<TT> frac(const Affine<TT>& a)
{
  Affine<TT> result = a;
  result %= static_cast<TT>(1.0);
  return result; 
}

template<typename TT>
Affine<TT> fmod(const Affine<TT>& a, const Affine<TT>& b) 
{
  return a % b;
}


template<typename TT>
Affine<TT> pow(const Affine<TT>& a, const Affine<TT>& b) 
{
  // @todo check if this is optimal
  // @todo do integer special cases? - see NuS.cc
  return exp(log(a)*b);
}

template<typename TT>
Affine<TT> pow(const Affine<TT>& a,  const TT& b) 
{
  // @todo check if this is optimal
  // @todo do integer special cases? - see NuS.cc
  return exp(log(a)*b);
}

template<typename TT>
struct __sh_aa_rcp {
  static const bool use_fdf = false;
  static TT f(TT x) { return 1.0 / x; }
  static TT dfinv(TT x, TT low=0, TT high=0) { 
    TT result = std::sqrt(-1.0 / x); 
    if(result > high) return -result;
    return result;
  }
  static TT fdfinv(TT x) { return std::sqrt(-x); }
};

template<typename TT>
Affine<TT> rcp(const Affine<TT>& a) 
{
  Interval<TT> bounds = a;

  if(bounds.lo() <= 0 && bounds.hi() >= 0) {
    // @todo handle infinities
    // This results in a -inf, inf 
  }

  return a. template convex_approx<__sh_aa_rcp<TT> >(bounds);
}

template<typename TT>
Affine<TT> rsq(const Affine<TT>& a) 
{
  // @todo range fix this 
  return rcp(sqrt(a)); 
}

template<typename TT>
Affine<TT> sgn(const Affine<TT>& a) 
{
  // step function
  Interval<TT> bounds = a;
  if(bounds.lo() > 0) {
    return Affine<TT>(static_cast<TT>(1.0));
  } else if(bounds.hi() < 0) {
    return Affine<TT>(static_cast<TT>(-1.0));
  } 
  return Affine<TT>(-1.0, 1.0);
}

template<typename TT>
struct __sh_aa_sqrt {
  static const bool use_fdf = true;
  static TT f(TT x) { return std::sqrt(x); }
  static TT dfinv(TT x, TT low=0, TT high=0) { return 1.0 / (4 * x * x); }
  static TT fdfinv(TT x) { return 0.5 / x; }
};

template<typename TT>
Affine<TT> sqrt(const Affine<TT>& a) 
{
  Interval<TT> bounds = a;

  if(bounds.lo() < 0) {
    // @todo range NaN case
  }

  return a. template convex_approx<__sh_aa_sqrt<TT> >(bounds);
}


/** Trig Operators */
template<typename TT>
Affine<TT> acos(const Affine<TT>& a) 
{
  // @todo range fix this
  return Affine<TT>(acos(a.to_interval()));
}

template<typename TT>
Affine<TT> acosh(const Affine<TT>& a) 
{
  // @todo range fix this
  return Affine<TT>(acosh(a.to_interval()));
}


template<typename TT>
Affine<TT> asin(const Affine<TT>& a) 
{
  // @todo range fix this
  return Affine<TT>(asin(a.to_interval()));
}

template<typename TT>
Affine<TT> asinh(const Affine<TT>& a) 
{
  // @todo range fix this
  return Affine<TT>(asinh(a.to_interval()));
}


template<typename TT>
Affine<TT> atan(const Affine<TT>& a) 
{
  // @todo range fix this
  return Affine<TT>(atan(a.to_interval()));
}


template<typename TT>
Affine<TT> atan2(const Affine<TT>& a, const Affine<TT>& b) 
{
  // @todo range fix this
  return Affine<TT>(atan2(a.to_interval(), b.to_interval()));
}

template<typename TT>
Affine<TT> atanh(const Affine<TT>& a) 
{
  // @todo range fix this
  return Affine<TT>(atanh(a.to_interval()));
}


template<typename TT>
Affine<TT> cos(const Affine<TT>& a) 
{
  // @todo range fix this
  return Affine<TT>(cos(a.to_interval()));
}

template<typename TT>
Affine<TT> cosh(const Affine<TT>& a) 
{
  // @todo range fix this
  return lerp(Affine<TT>(0.5), exp(a), exp(-a));
}

template<typename TT>
Affine<TT> sin(const Affine<TT>& a) 
{
  // @todo range fix this
  return Affine<TT>(sin(a.to_interval()));
}

template<typename TT>
Affine<TT> sinh(const Affine<TT>& a) 
{
  // @todo range fix this
  return lerp(Affine<TT>(0.5), exp(a), -exp(-a));
}

template<typename TT>
Affine<TT> tan(const Affine<TT>& a) 
{
  // @todo range fix this
  return Affine<TT>(tan(a.to_interval()));
}

template<typename TT>
Affine<TT> tanh(const Affine<TT>& a) 
{
  // @todo range fix this
  return Affine<TT>(tanh(a.to_interval()));
}


/** Comparison Operators **/

template<typename TT>
Affine<TT> operator<(const Affine<TT>& a, const Affine<TT>& b) 
{
  return a.to_interval() < b.to_interval(); 
}

template<typename TT>
Affine<TT> operator<=(const Affine<TT>& a, const Affine<TT>& b) 
{
  return a.to_interval() <= b.to_interval(); 
}

template<typename TT>
Affine<TT> operator>(const Affine<TT>& a, const Affine<TT>& b) 
{
  return a.to_interval() > b.to_interval(); 
}

template<typename TT>
Affine<TT> operator>(const Affine<TT>& a, const TT& b) 
{
  return a.to_interval() > b;
}

template<typename TT>
Affine<TT> operator>=(const Affine<TT>& a, const Affine<TT>& b) 
{
  return a.to_interval() >= b.to_interval();
}

template<typename TT>
Affine<TT> operator==(const Affine<TT>& a, const Affine<TT>& b) 
{
  return a.to_interval() == b.to_interval();
}

template<typename TT>
Affine<TT> operator!=(const Affine<TT>& a, const Affine<TT>& b) 
{
  return a.to_interval() != b.to_interval();
}

template<typename TT>
bool boundsEqual(const Affine<TT>& a, const Affine<TT>& b) 
{
  if(a.center() != b.center() || a.size() != b.size()) return false;
  typename Affine<TT>::const_iterator A, B;
  for(A = a.begin(), B = b.begin(); A != a.end() && B != b.end();) {
    if(A->index() != B->index() || A->err() != B->err()) return false;
  }
  return true;
}

/** Clamping operators **/

// @todo range - note these two ops are not commutative, 
// but should be...
template<typename TT>
Affine<TT> min(const Affine<TT>& a, const Affine<TT>& b) 
{
  return a - pos(a - b);
}

template<typename TT>
Affine<TT> max(const Affine<TT>& a, const Affine<TT>& b) 
{
  return pos(a - b) + b;
}

template<typename TT>
Affine<TT> pos(const Affine<TT>& a)
{
  Interval<TT> abounds = a;
  TT lo = abounds.lo();
  TT hi = abounds.hi();
  if(lo > 0) return a; 
  if(hi <= 0) return Affine<TT>(static_cast<TT>(0.0));
  TT scaling = 0.5 * hi / (hi - lo); 

  Affine<TT> result = a;
  result *= scaling;
  result.m_center = hi / 2;

  return result; 
}

// If floor is constant across a, then return a constant
// Else, one best approx for floor(x) is the line y = x - 0.5
// (Approx is not unique, but this is an easy one to use)
template<typename TT>
Affine<TT> floor(const Affine<TT>& a) 
{
  Interval<TT> floor_a= floor(a.to_interval()); 
  TT flo = floor_a.lo(); 
  TT fhi = floor_a.hi(); 

  // assume no float errors
  // @todo range - check if float errors are possible
  if(flo == fhi) return Affine<TT>(static_cast<TT>(flo)); 

  Affine<TT> result(a);
  result.m_center -= 0.5;
  return result; 
}

// Like floor, except use y = x + 0.5 
template<typename TT>
Affine<TT> ceil(const Affine<TT>& a) 
{
  Interval<TT> ceil_a = ceil(a.to_interval()); 
  TT clo = ceil_a.lo(); 
  TT chi = ceil_a.hi(); 

  if(clo == chi) return Affine<TT>(static_cast<TT>(clo)); 

  Affine<TT> result(a);
  result.m_center += 0.5;
  return result; 
}

template<typename TT>
Affine<TT> rnd(const Affine<TT>& a) 
{
  // @todo range fix this
  return Affine<TT>(rnd(a.to_interval()));
}

template<typename TT>
Affine<TT> abs(const Affine<TT>& a) 
{
  Interval<TT> bounds = a; 
  if(bounds.lo() >= 0) return a;
  if(bounds.hi() <= 0) return -a;

  // convex function
  // Don't bother with convex_approx though, since this is simple
  // max error is at endpoints and x = 0

  TT hi = bounds.hi();
  TT lo = bounds.lo();
  TT abshi = hi;
  TT abslo = -lo;
  TT alpha, beta, delta; // beta = delta in this case
  alpha = (abshi - abslo) / bounds.width();
  beta = delta = 0.5 * (hi - alpha * hi); 

  return a.affine_approx(alpha, beta, beta); 
}

/** Misc operators **/
template<typename TT>
Affine<TT> cond(const Affine<TT>& a, const Affine<TT>& b, 
    const Affine<TT>& c)
{
  return lerp(a > (TT)(0), b, c);
}

template<typename TT>
Affine<TT> lerp(const Affine<TT>& a, const Affine<TT>& b, const Affine<TT>& c) 
{
  return a * (b - c) + c; 
}

template<typename TT>
Affine<TT> range_union(const Affine<TT>& a, const Affine<TT>& b)
{
  Interval<TT> ival_union = range_union(a.to_interval(), b.to_interval());
  TT ival_radius = ival_union.radius();

  Affine<TT> result(ival_union.center());
  result.reserve(a.size() + b.size());

  typename Affine<TT>::const_iterator A, B;
  for(A = a.begin(), B = b.begin(); A != a.end() && B != b.end();) {
    if(*A < *B) {
      result.add(*(A++));
    } else if (*B < *A) {
      result.add(*(B++));
    } else { // *A == *B
      // @todo range - what to do?
      AffineSymbol<TT> newsym(A->index(), std::max(A->abs_err(), B->abs_err()));
      result.add(newsym);
      ++A; ++B;
    }
  }
  if(A != a.end()) result.add(A, a.end());
  if(B != b.end()) result.add(B, b.end());

  TT new_radius = result.radius();
  if(new_radius < ival_radius) {
    TT scale = ival_radius / new_radius;
    result *= scale;
  }

  return result; 
}

template<typename TT>
Affine<TT> range_isct(const Affine<TT>& a, const Affine<TT>& b)
{
  Interval<TT> ival_isct = range_isct(a.to_interval(), b.to_interval());
  TT ival_radius = ival_isct.radius();

  Affine<TT> result(ival_isct.center());
  result.reserve(a.size() + b.size());

  typename Affine<TT>::const_iterator A, B;
  for(A = a.begin(), B = b.begin(); A != a.end() && B != b.end();) {
    if(*A < *B) {
      result.add(*(A++));
    } else if (*B < *A) {
      result.add(*(B++));
    } else { // *A == *B
      // @todo range - what to do?
      AffineSymbol<TT> newsym(A->index(), std::min(A->abs_err(), B->abs_err()));
      result.add(newsym);
      ++A; ++B;
    }
  }
  if(A != a.end()) result.add(A, a.end());
  if(B != b.end()) result.add(B, b.end());

  TT new_radius = result.radius();
  if(new_radius > ival_radius) {
    TT scale = ival_radius / new_radius;
    result *= scale;
  }

  return result; 
}

// @todo range fix this to have the proper return type
template<typename TT>
Affine<TT> range_contains(const Affine<TT>& a, const Affine<TT>& b)
{
  return Affine<TT>(range_contains(a.to_interval(), b.to_interval()));
}

}

#endif
