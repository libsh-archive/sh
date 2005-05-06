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
struct ShInterval {
  typedef T DataType;
  T m_lo;
  T m_hi;

  /** Constructs an interval with undefined value */
  ShInterval();

  /** Constructs an interval [value,value] */
  ShInterval(const T& value);

  /** Constructs an interval with the given bounds */
  ShInterval(const T& lo, const T& hi);

  template<typename T2>
  ShInterval(const ShInterval<T2> &other);

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
  ShInterval& operator=(const T &value);
  ShInterval& operator=(const ShInterval<T> &other);
  ShInterval& operator+=(const T &value);
  ShInterval& operator+=(const ShInterval<T> &other);
  ShInterval& operator-=(const T &value);
  ShInterval& operator-=(const ShInterval<T> &other);
  ShInterval& operator*=(const T &value);
  ShInterval& operator*=(const ShInterval<T> &other);
  ShInterval& operator/=(const T &value);
  ShInterval& operator/=(const ShInterval<T> &other);

  /** Float modulus - result is always positive 
   *@{*/
  ShInterval& operator%=(const T &value);
  ShInterval& operator%=(const ShInterval<T> &other);
  // @}

  /** Scalar arithmetic operators **/

  /** Negation **/
  ShInterval operator-() const;

  /** Output operator **/
  template<typename TT>
  friend std::ostream& operator<<(std::ostream& out, const ShInterval<TT> &value);


  /** Input operator (format matches output) **/
  template<typename TT>
  friend std::istream& operator>>(std::istream& out, ShInterval<TT> &value);

  /** Arithmetic operators **/
  // TODO fill in the remaining interval with scalar ops
  template<typename TT>
  friend ShInterval<TT> operator+(const ShInterval<TT> &a, const ShInterval<TT> &b);

  template<typename TT>
  friend ShInterval<TT> operator-(const ShInterval<TT> &a, const ShInterval<TT> &b);

  template<typename TT>
  friend ShInterval<TT> operator*(const ShInterval<TT> &a, const ShInterval<TT> &b);

  template<typename TT>
  friend ShInterval<TT> operator/(const ShInterval<TT> &a, const ShInterval<TT> &b);

  template<typename TT>
  friend ShInterval<TT> operator%(const ShInterval<TT> &a, const ShInterval<TT> &b);

  template<typename TT>
  friend ShInterval<TT> cbrt(const ShInterval<TT> &a);

  template<typename TT>
  friend ShInterval<TT> exp(const ShInterval<TT> &a);

  template<typename TT>
  friend ShInterval<TT> exp2(const ShInterval<TT> &a);

  template<typename TT>
  friend ShInterval<TT> exp10(const ShInterval<TT> &a);

  template<typename TT>
  friend ShInterval<TT> log(const ShInterval<TT> &a);

  template<typename TT>
  friend ShInterval<TT> log2(const ShInterval<TT> &a);

  template<typename TT>
  friend ShInterval<TT> log10(const ShInterval<TT> &a);

  template<typename TT>
  friend ShInterval<TT> frac(const ShInterval<TT> &a);

  template<typename TT>
  friend ShInterval<TT> fmod(const ShInterval<TT> &a, const ShInterval<TT> &b);

  template<typename TT>
  friend ShInterval<TT> pow(const ShInterval<TT> &a, const ShInterval<TT> &b);

  template<typename TT>
  friend ShInterval<TT> rcp(const ShInterval<TT> &a);

  template<typename TT>
  friend ShInterval<TT> rsq(const ShInterval<TT> &a);

  template<typename TT>
  friend ShInterval<TT> sgn(const ShInterval<TT> &a);

  template<typename TT>
  friend ShInterval<TT> sqrt(const ShInterval<TT> &a);

  /** Trig Operators */
  template<typename TT>
  friend ShInterval<TT> acos(const ShInterval<TT> &a);

  template<typename TT>
  friend ShInterval<TT> asin(const ShInterval<TT> &a);

  template<typename TT>
  friend ShInterval<TT> atan(const ShInterval<TT> &a);

  template<typename TT>
  friend ShInterval<TT> atan2(const ShInterval<TT> &a, const ShInterval<TT> &b);

  template<typename TT>
  friend ShInterval<TT> cos(const ShInterval<TT> &a); 
  template<typename TT>
  friend ShInterval<TT> cosh(const ShInterval<TT> &a); 
  template<typename TT>
  friend ShInterval<TT> sin(const ShInterval<TT> &a);
  template<typename TT>
  friend ShInterval<TT> sinh(const ShInterval<TT> &a);

  template<typename TT>
  friend ShInterval<TT> tan(const ShInterval<TT> &a);

  /** Comparison Operators **/
  // @todo should think about how to represent tri-state logic values.
  // For now output is interval (follows the t x t -> t convention of
  // types for the standard operators)
  template<typename TT>
  friend ShInterval<TT> operator<(const ShInterval<TT> &a, const ShInterval<TT> &b);

  template<typename TT>
  friend ShInterval<TT> operator<=(const ShInterval<TT> &a, const ShInterval<TT> &b);

  template<typename TT>
  friend ShInterval<TT> operator>(const ShInterval<TT> &a, const ShInterval<TT> &b);

  template<typename TT>
  friend ShInterval<TT> operator>(const ShInterval<TT> &a, const TT& b); 

  template<typename TT>
  friend ShInterval<TT> operator>=(const ShInterval<TT> &a, const ShInterval<TT> &b);

  template<typename TT>
  friend ShInterval<TT> operator==(const ShInterval<TT> &a, const ShInterval<TT> &b);

  template<typename TT>
  friend ShInterval<TT> operator!=(const ShInterval<TT> &a, const ShInterval<TT> &b);

  /// Returns true iff lo = a.lo and hi = a.hi 
  template<typename TT>
  bool boundsEqual(const ShInterval<TT> &a);


  /** Clamping operators **/
  template<typename TT>
  friend ShInterval<TT> min(const ShInterval<TT> &a, const ShInterval<TT> &b);

  template<typename TT>
  friend ShInterval<TT> max(const ShInterval<TT> &a, const ShInterval<TT> &b);

  template<typename TT>
  friend ShInterval<TT> floor(const ShInterval<TT> &a);

  template<typename TT>
  friend ShInterval<TT> ceil(const ShInterval<TT> &a);

  template<typename TT>
  friend ShInterval<TT> rnd(const ShInterval<TT> &a);

  template<typename TT>
  friend ShInterval<TT> abs(const ShInterval<TT> &a);

  /** Misc Operators **/
  template<typename TT>
  friend ShInterval<TT> cond(const ShInterval<TT> &a, const ShInterval<TT> &b, const ShInterval<TT> &c);

  template<typename TT>
  friend ShInterval<TT> lerp(const ShInterval<TT> &a, const ShInterval<TT> &b, const ShInterval<TT> &c);

  /** Special Interval Ops **/
  template<typename TT>
  friend ShInterval<TT> range_union(const ShInterval<TT> &a, const ShInterval<TT> &b);

  template<typename TT>
  friend ShInterval<TT> range_isct(const ShInterval<TT> &a, const ShInterval<TT> &b);

  template<typename TT>
  friend ShInterval<TT> range_contains(const ShInterval<TT> &a, const ShInterval<TT> &b);

};


}

#include "ShIntervalImpl.hpp"
  
#endif
