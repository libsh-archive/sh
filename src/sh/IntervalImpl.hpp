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
#ifndef SHINTERVALIMPL_HPP
#define SHINTERVALIMPL_HPP

#include <cmath>
#include "Math.hpp"
#include "Interval.hpp"

namespace SH {

// @todo replace uses of std::fabs 

/** Constructs an interval with undefined value */
template<typename T>
Interval<T>::Interval()
{}

/** Constructs an interval [value,value] */
template<typename T>
Interval<T>::Interval(const T& value)
  : m_lo(value), m_hi(value)
{}

/** Constructs an interval with the given bounds */
template<typename T>
Interval<T>::Interval(const T& lo, const T& hi)
  : m_lo(lo), m_hi(hi)
{}

template<typename T>
template<typename T2>
Interval<T>::Interval(const Interval<T2> &other)
  : m_lo((T)(other.lo())), m_hi((T)(other.hi()))
{}

/** accessor methods **/
template<typename T>
T& Interval<T>::lo()
{
  return m_lo;
}

template<typename T>
const T& Interval<T>::lo() const
{
  return m_lo;
}

template<typename T>
T& Interval<T>::hi()
{
  return m_hi;
}

template<typename T>
const T& Interval<T>::hi() const
{
  return m_hi;
}

/** Useful helpers **/
template<typename T>
const T Interval<T>::width() const
{
  return m_hi - m_lo;
}

template<typename T>
const T Interval<T>::center() const
{
  return (m_hi + m_lo) / 2; 
}

template<typename T>
const T Interval<T>::radius() const
{
  return width() / 2;
}

/** Arithmetic operators **/
template<typename T>
Interval<T>& Interval<T>::operator=(const T &value)
{
  m_lo = m_hi = value;
  return *this;
}

template<typename T>
Interval<T>& Interval<T>::operator=(const Interval<T> &other)
{
  m_lo = other.m_lo;
  m_hi = other.m_hi;
  return *this;
}

template<typename T>
Interval<T>& Interval<T>::operator+=(const T &value)
{
  m_lo += value;
  m_hi += value;
}

template<typename T>
Interval<T>& Interval<T>::operator+=(const Interval<T> &other)
{
  m_lo += other.m_lo;
  m_hi += other.m_hi;
  return *this;
}

template<typename T>
Interval<T>& Interval<T>::operator-=(const T &value)
{
  m_lo -= value;
  m_hi -= value;
}

template<typename T>
Interval<T>& Interval<T>::operator-=(const Interval<T> &other)
{
  m_lo -= other.m_hi;
  m_hi -= other.m_lo;
  return *this;
}

template<typename T>
Interval<T>& Interval<T>::operator*=(const T &value)
{
  m_lo = m_lo * value;
  m_hi = m_hi * value;
  if(value < 0) std::swap(m_lo, m_hi); 
  return *this;
}

template<typename T>
Interval<T>& Interval<T>::operator*=(const Interval<T> &other)
{
  T ll, lh, hl, hh;
  ll = m_lo * other.m_lo;
  lh = m_lo * other.m_hi;
  hl = m_hi * other.m_lo;
  hh = m_hi * other.m_hi;

  m_lo = std::min(std::min(ll, lh), std::min(hl, hh));
  m_hi = std::max(std::max(ll, lh), std::max(hl, hh));

  return *this;
}

template<typename T>
Interval<T>& Interval<T>::operator/=(const T &value)
{
  return (*this) *= (1 / value);
}

template<typename T>
Interval<T>& Interval<T>::operator/=(const Interval<T> &other)
{
  return (*this) *= rcp(other);
}

template<typename T>
Interval<T>& Interval<T>::operator%=(const T &value)
{
  // @todo
  m_hi = std::fabs(value);
  m_lo = -m_hi;
}

template<typename T>
Interval<T>& Interval<T>::operator%=(const Interval<T> &other)
{
  // @todo
  m_hi = std::max(std::fabs(other.lo), std::fabs(other.hi));
  m_lo = -m_hi;
}

/** Negation **/
template<typename T>
Interval<T> Interval<T>::operator-() const 
{
  return Interval<T>(-m_hi, -m_lo);
}

template<typename TT>
std::ostream& operator<<(std::ostream &out, const Interval<TT> &value)
{
  out << "[" << value.m_lo << "," << value.m_hi << "]";
  return out;
}

template<typename TT>
std::istream& operator>>(std::istream &in, Interval<TT> &value)
{
  // @todo this isn't exactly bullet-proof right now...
  in.ignore(1, '[');
  in >> value.m_lo;
  in.ignore(1, ',');
  in >> value.m_hi;
  in.ignore(1, ']');
  return in;
}

/** Arithmetic operators **/
template<typename TT>
Interval<TT> operator+(const Interval<TT> &a, const Interval<TT> &b) 
{
  return Interval<TT>(a.m_lo + b.m_lo, a.m_hi + b.m_hi);
}


template<typename TT>
Interval<TT> operator-(const Interval<TT> &a, const Interval<TT> &b) 
{
  return Interval<TT>(a.m_lo - b.m_hi, a.m_hi - b.m_lo);
}

template<typename TT>
Interval<TT> operator*(const Interval<TT> &a, const Interval<TT> &b) 
{
  TT ll, lh, hl, hh;
  ll = a.m_lo * b.m_lo;
  lh = a.m_lo * b.m_hi;
  hl = a.m_hi * b.m_lo;
  hh = a.m_hi * b.m_hi;

  return Interval<TT>(std::min(std::min(ll, lh), std::min(hl, hh)),
      std::max(std::max(ll, lh), std::max(hl, hh)));
}


template<typename TT>
Interval<TT> operator/(const Interval<TT> &a, const Interval<TT> &b) 
{
  return a * rcp(b);
}


template<typename TT>
Interval<TT> operator%(const Interval<TT> &a, const Interval<TT> &b) 
{
  TT large = std::max(std::fabs(b.m_lo), std::fabs(b.m_hi));
  return Interval<TT>(-large, large); 
}

template<typename TT>
Interval<TT> cbrt(const Interval<TT> &a) 
{
  // monotonic
  // @todo type check this one 
  return pow(a, Interval<TT>(1.0f / 3.0f)); 
}


template<typename TT>
Interval<TT> exp(const Interval<TT> &a) 
{
  // monotonic function
  return Interval<TT>(std::exp(a.m_lo), std::exp(a.m_hi));
}


template<typename TT>
Interval<TT> exp2(const Interval<TT> &a) 
{
  // monotonic function
  return Interval<TT>(exp2f(a.m_lo), exp2f(a.m_hi));
}


template<typename TT>
Interval<TT> exp10(const Interval<TT> &a) 
{
  // monotonic function
return Interval<TT>(exp10f(a.m_lo), exp10f(a.m_hi)); 
}


template<typename TT>
Interval<TT> log(const Interval<TT> &a) 
{
  // monotonic, except <= 0 gives NaN 
  // @todo handle <= 0 with exception?
  return Interval<TT>(std::log(a.m_lo), std::log(a.m_hi));
}


template<typename TT>
Interval<TT> log2(const Interval<TT> &a) 
{
  // monotonic, except <= 0 gives NaN 
  // @todo handle <= 0 with exception or clamp?
  return Interval<TT>(log2f(a.m_lo), log2f(a.m_hi));
}


template<typename TT>
Interval<TT> log10(const Interval<TT> &a) 
{
  // monotonic, except <= 0 gives NaN 
  // @todo handle <= 0 with exception or clamp?
  return Interval<TT>(log10f(a.m_lo), log10f(a.m_hi));
}

template<typename TT>
Interval<TT> frac(const Interval<TT> &a)
{
  // @todo - if width > 1, then return below,
  // otherwise do some more figuring
  return Interval<TT>(0, 1);
}

template<typename TT>
Interval<TT> fmod(const Interval<TT> &a, const Interval<TT> &b) 
{
  return a % b;
}


template<typename TT>
Interval<TT> pow(const Interval<TT> &a, const Interval<TT> &b) 
{
  // @todo check if this is optimal
  // @todo do integer special cases? - see NuS.cc
  return exp(log(a)*b);
}


template<typename TT>
Interval<TT> rcp(const Interval<TT> &a) 
{
  if(a.m_lo <= 0 && a.m_hi >= 0) {
    // @todo handle infinities
    // This results in a -inf, inf interval
  }
  return Interval<TT>(1.0 / a.m_hi, 1.0 / a.m_lo);
}

template<typename TT>
Interval<TT> rsq(const Interval<TT> &a) 
{
  // @todo something better?
  return rcp(sqrt(a)); 
}

template<typename TT>
Interval<TT> sgn(const Interval<TT> &a) 
{
  // @todo have constants for positve, negative, zero values

  // the usual sgn function is monotonic:
  return Interval<TT>(a.m_lo < 0 ? -1 : a.m_lo > 0 ? 1 : 0,
                       a.m_hi < 0 ? -1 : a.m_hi > 0 ? 1 : 0);
}

template<typename TT>
Interval<TT> sqrt(const Interval<TT> &a) 
{
  // monotonic, except <= 0 gives NaN
  // @todo handle <= 0 with exception or clamp
  return Interval<TT>(std::sqrt(a.m_lo), std::sqrt(a.m_hi));
}


/** Trig Operators */
template<typename TT>
Interval<TT> acos(const Interval<TT> &a) 
{
  // @todo
  return Interval<TT>(0, M_PI);
}

template<typename TT>
Interval<TT> acosh(const Interval<TT> &a) 
{
  // @todo
  return Interval<TT>(0, 1e8);
}


template<typename TT>
Interval<TT> asin(const Interval<TT> &a) 
{
  // @todo
  return Interval<TT>(-M_PI/2, M_PI/2);
}

template<typename TT>
Interval<TT> asinh(const Interval<TT> &a) 
{
  // @todo
  return Interval<TT>(-1e8, 1e8);
}


template<typename TT>
Interval<TT> atan(const Interval<TT> &a) 
{
  // @todo
  return Interval<TT>(-M_PI/2, M_PI/2);
}


template<typename TT>
Interval<TT> atan2(const Interval<TT> &a, const Interval<TT> &b) 
{
  // @todo
  return Interval<TT>(-M_PI, M_PI);
}

template<typename TT>
Interval<TT> atanh(const Interval<TT> &a) 
{
  // @todo
  return Interval<TT>(-1e8, 1e8);
}


template<typename TT>
Interval<TT> cos(const Interval<TT> &a) 
{
  //@todo
  return Interval<TT>(-1, 1);
}

template<typename TT>
Interval<TT> cosh(const Interval<TT> &a) 
{
  //@todo
  return lerp(Interval<TT>(0.5), exp(a), exp(-a));
}

template<typename TT>
Interval<TT> sin(const Interval<TT> &a) 
{
  // @todo
  return Interval<TT>(-1, 1);
}

template<typename TT>
Interval<TT> sinh(const Interval<TT> &a) 
{
  //@todo
  return lerp(Interval<TT>(0.5), exp(a), -exp(-a));
}


template<typename TT>
Interval<TT> tan(const Interval<TT> &a) 
{
  // @todo
  return Interval<TT>(-1, 1); // @todo this is totally wrong...
}

template<typename TT>
Interval<TT> tanh(const Interval<TT> &a) 
{
  // @todo
  return Interval<TT>(-1, 1); // @todo this is totally wrong...
}


/** Comparison Operators **/
template<typename TT>
Interval<TT> __boolean_op(bool trueCond, bool falseCond) {
  // @todo fix this assumption that false < true
  // @todo special value for maybe? 
  // @todo make this return something special (other than Interval<TT>)?
  
  if(trueCond) return Interval<TT>(1);
  if(falseCond) return Interval<TT>(0);
  return Interval<TT>(0, 1);
}

template<typename TT>
Interval<TT> operator<(const Interval<TT> &a, const Interval<TT> &b) 
{
  return __boolean_op<TT>(a.m_hi < b.m_lo, a.m_lo > b.m_hi);
}

template<typename TT>
Interval<TT> operator<=(const Interval<TT> &a, const Interval<TT> &b) 
{
  return __boolean_op<TT>(a.m_hi <= b.m_lo, a.m_lo > b.m_hi);
}

template<typename TT>
Interval<TT> operator>(const Interval<TT> &a, const Interval<TT> &b) 
{
  return __boolean_op<TT>(a.m_hi > b.m_lo, a.m_lo < b.m_hi);
}

template<typename TT>
Interval<TT> operator>(const Interval<TT> &a, const TT &b) 
{
  return __boolean_op<TT>(a.m_hi > b, a.m_lo < b);
}

template<typename TT>
Interval<TT> operator>=(const Interval<TT> &a, const Interval<TT> &b) 
{
  return __boolean_op<TT>(a.m_hi >= b.m_lo, a.m_lo < b.m_hi);
}

template<typename TT>
Interval<TT> operator==(const Interval<TT> &a, const Interval<TT> &b) 
{
  return __boolean_op<TT>(
      a.m_hi == b.m_hi && a.m_lo == b.m_lo && a.m_lo == a.m_hi,
      a.m_hi < b.m_lo || a.m_lo > b.m_hi);
}

template<typename TT>
Interval<TT> operator!=(const Interval<TT> &a, const Interval<TT> &b) 
{
  // @todo should use not(a==b) once we have not
  return __boolean_op<TT>(
      a.m_hi < b.m_lo || a.m_lo > b.m_hi,
      a.m_hi == b.m_hi && a.m_lo == b.m_lo && a.m_lo == a.m_hi);
}

template<typename TT>
bool boundsEqual(const Interval<TT> &a, const Interval<TT> &b) 
{
  return (a.lo() == b.lo()) && (a.hi() == b.hi());
}



/** Clamping operators **/
template<typename TT>
Interval<TT> min(const Interval<TT> &a, const Interval<TT> &b) 
{
  return Interval<TT>(std::min(a.m_lo, b.m_lo), std::min(a.m_hi, b.m_hi));
}

template<typename TT>
Interval<TT> max(const Interval<TT> &a, const Interval<TT> &b) 
{
  return Interval<TT>(std::max(a.m_lo, b.m_lo), std::max(a.m_hi, b.m_hi));
}

template<typename TT>
Interval<TT> floor(const Interval<TT> &a) 
{
  // monotonic
  return Interval<TT>(std::floor(a.m_lo), std::floor(a.m_hi));
}

template<typename TT>
Interval<TT> ceil(const Interval<TT> &a) 
{
  // monotonic
  return Interval<TT>(std::ceil(a.m_lo), std::ceil(a.m_hi));
}

template<typename TT>
Interval<TT> rnd(const Interval<TT> &a) 
{
  // monotonic
  // @todo type handle this in general
  return Interval<TT>(std::floor(a.m_lo + 0.5f), std::floor(a.m_hi + 0.5f));
}

template<typename TT>
Interval<TT> abs(const Interval<TT> &a) 
{
  TT lo, hi;

  hi = std::max(std::fabs(a.m_lo), std::fabs(a.m_hi));
  lo = std::min(std::fabs(a.m_lo), std::fabs(a.m_hi));
  if(a.m_lo <= 0 && a.m_hi >= 0) lo = 0;
  return Interval<TT>(lo, hi);
}

/** Misc operators **/
template<typename TT>
Interval<TT> cond(const Interval<TT> &a, const Interval<TT> &b, 
    const Interval<TT> &c)
{
  return lerp(a > (TT)(0.0), b, c);
}

template<typename TT>
Interval<TT> lerp(const Interval<TT> &a, const Interval<TT> &b, const Interval<TT> &c) 
{
  // @todo range - check that this is as stable as possible for intervals
  return a * (b - c) + c; 
}

template<typename TT>
Interval<TT> range_union(const Interval<TT> &a, const Interval<TT> &b)
{
  return Interval<TT>(std::min(a.m_lo, b.m_lo),
                       std::max(a.m_hi, b.m_hi));
}

template<typename TT>
Interval<TT> range_isct(const Interval<TT> &a, const Interval<TT> &b)
{
  return Interval<TT>(std::max(a.m_lo, b.m_lo),
                       std::min(a.m_hi, b.m_hi));
}

// @todo range fix this to have the proper return type
template<typename TT>
Interval<TT> range_contains(const Interval<TT> &a, const Interval<TT> &b)
{
  TT result = a.m_lo <= b.m_lo && b.m_hi <= a.m_hi ? 1 : 0; 
  return Interval<TT>(result);
}

}

#endif
