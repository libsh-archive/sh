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
#ifndef SHINTERVAL_HPP
#define SHINTERVAL_HPP

#include <iostream>

namespace SH {

/*** Sh class for standard floating point interval arithmetic, without rounding
 *
 * Derived from NuS.hh and NuS.cc from the MetaMedia project.
 *
 * Currently does not handle rounding errors yet 
 */
template<typename T>
struct Interval {
  typedef T DataType;
  T m_lo;
  T m_hi;

  /** Constructs an interval with undefined value */
  Interval();

  /** Constructs an interval [value,value] */
  Interval(const T& value);

  /** Constructs an interval with the given bounds */
  Interval(const T& lo, const T& hi);

  template<typename T2>
  Interval(const Interval<T2> &other);

  /** accessor methods **/
  // @todo why are these even here if m_lo, m_hi are public?
  // @todo why are m_lo and m_hi public?
  T& lo();
  const T& lo() const;

  T& hi();
  const T& hi() const;

  /** Useful helpers **/ 
  /** Returns m_hi - m_lo **/
  const T width() const;

  /** Returns (m_hi + m_lo) / 2 **/
  const T center() const;

  /** Returns width() / 2 **/
  const T radius() const;

  /** Arithmetic operators **/
  Interval& operator=(const T &value);
  Interval& operator=(const Interval<T> &other);
  Interval& operator+=(const T &value);
  Interval& operator+=(const Interval<T> &other);
  Interval& operator-=(const T &value);
  Interval& operator-=(const Interval<T> &other);
  Interval& operator*=(const T &value);
  Interval& operator*=(const Interval<T> &other);
  Interval& operator/=(const T &value);
  Interval& operator/=(const Interval<T> &other);

  /** Float modulus - result is always positive 
   *@{*/
  Interval& operator%=(const T &value);
  Interval& operator%=(const Interval<T> &other);
  // @}

  /** Scalar arithmetic operators **/

  /** Negation **/
  Interval operator-() const;

  /** Output operator **/
  template<typename TT>
  friend std::ostream& operator<<(std::ostream& out, const Interval<TT> &value);


  /** Input operator (format matches output) **/
  template<typename TT>
  friend std::istream& operator>>(std::istream& out, Interval<TT> &value);

  /** Arithmetic operators **/
  // TODO fill in the remaining interval with scalar ops
  template<typename TT>
  friend Interval<TT> operator+(const Interval<TT> &a, const Interval<TT> &b);

  template<typename TT>
  friend Interval<TT> operator-(const Interval<TT> &a, const Interval<TT> &b);

  template<typename TT>
  friend Interval<TT> operator*(const Interval<TT> &a, const Interval<TT> &b);

  template<typename TT>
  friend Interval<TT> operator/(const Interval<TT> &a, const Interval<TT> &b);

  template<typename TT>
  friend Interval<TT> operator%(const Interval<TT> &a, const Interval<TT> &b);

  template<typename TT>
  friend Interval<TT> cbrt(const Interval<TT> &a);

  template<typename TT>
  friend Interval<TT> exp(const Interval<TT> &a);

  template<typename TT>
  friend Interval<TT> exp2(const Interval<TT> &a);

  template<typename TT>
  friend Interval<TT> exp10(const Interval<TT> &a);

  template<typename TT>
  friend Interval<TT> log(const Interval<TT> &a);

  template<typename TT>
  friend Interval<TT> log2(const Interval<TT> &a);

  template<typename TT>
  friend Interval<TT> log10(const Interval<TT> &a);

  template<typename TT>
  friend Interval<TT> frac(const Interval<TT> &a);

  template<typename TT>
  friend Interval<TT> fmod(const Interval<TT> &a, const Interval<TT> &b);

  template<typename TT>
  friend Interval<TT> pow(const Interval<TT> &a, const Interval<TT> &b);

  template<typename TT>
  friend Interval<TT> rcp(const Interval<TT> &a);

  template<typename TT>
  friend Interval<TT> rsq(const Interval<TT> &a);

  template<typename TT>
  friend Interval<TT> sgn(const Interval<TT> &a);

  template<typename TT>
  friend Interval<TT> sqrt(const Interval<TT> &a);

  /** Trig Operators */
  template<typename TT>
  friend Interval<TT> acos(const Interval<TT> &a);

  template<typename TT>
  friend Interval<TT> acosh(const Interval<TT> &a);

  template<typename TT>
  friend Interval<TT> asin(const Interval<TT> &a);

  template<typename TT>
  friend Interval<TT> asinh(const Interval<TT> &a);

  template<typename TT>
  friend Interval<TT> atan(const Interval<TT> &a);

  template<typename TT>
  friend Interval<TT> atan2(const Interval<TT> &a, const Interval<TT> &b);

  template<typename TT>
  friend Interval<TT> atanh(const Interval<TT> &a);

  template<typename TT>
  friend Interval<TT> cos(const Interval<TT> &a); 
  template<typename TT>
  friend Interval<TT> cosh(const Interval<TT> &a); 
  template<typename TT>
  friend Interval<TT> sin(const Interval<TT> &a);
  template<typename TT>
  friend Interval<TT> sinh(const Interval<TT> &a);

  template<typename TT>
  friend Interval<TT> tan(const Interval<TT> &a);

  template<typename TT>
  friend Interval<TT> tanh(const Interval<TT> &a);

  /** Comparison Operators **/
  // @todo should think about how to represent tri-state logic values.
  // For now output is interval (follows the t x t -> t convention of
  // types for the standard operators)
  template<typename TT>
  friend Interval<TT> operator<(const Interval<TT> &a, const Interval<TT> &b);

  template<typename TT>
  friend Interval<TT> operator<=(const Interval<TT> &a, const Interval<TT> &b);

  template<typename TT>
  friend Interval<TT> operator>(const Interval<TT> &a, const Interval<TT> &b);

  template<typename TT>
  friend Interval<TT> operator>(const Interval<TT> &a, const TT& b); 

  template<typename TT>
  friend Interval<TT> operator>=(const Interval<TT> &a, const Interval<TT> &b);

  template<typename TT>
  friend Interval<TT> operator==(const Interval<TT> &a, const Interval<TT> &b);

  template<typename TT>
  friend Interval<TT> operator!=(const Interval<TT> &a, const Interval<TT> &b);

  /// Returns true iff lo = a.lo and hi = a.hi 
  template<typename TT>
  bool boundsEqual(const Interval<TT> &a);


  /** Clamping operators **/
  template<typename TT>
  friend Interval<TT> min(const Interval<TT> &a, const Interval<TT> &b);

  template<typename TT>
  friend Interval<TT> max(const Interval<TT> &a, const Interval<TT> &b);

  template<typename TT>
  friend Interval<TT> floor(const Interval<TT> &a);

  template<typename TT>
  friend Interval<TT> ceil(const Interval<TT> &a);

  template<typename TT>
  friend Interval<TT> rnd(const Interval<TT> &a);

  template<typename TT>
  friend Interval<TT> abs(const Interval<TT> &a);

  /** Misc Operators **/
  template<typename TT>
  friend Interval<TT> cond(const Interval<TT> &a, const Interval<TT> &b, const Interval<TT> &c);

  template<typename TT>
  friend Interval<TT> lerp(const Interval<TT> &a, const Interval<TT> &b, const Interval<TT> &c);

  /** Special Interval Ops **/
  template<typename TT>
  friend Interval<TT> range_union(const Interval<TT> &a, const Interval<TT> &b);

  template<typename TT>
  friend Interval<TT> range_isct(const Interval<TT> &a, const Interval<TT> &b);

  template<typename TT>
  friend Interval<TT> range_contains(const Interval<TT> &a, const Interval<TT> &b);

};


}

#include "IntervalImpl.hpp"
  
#endif
