// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
//////////////////////////////////////////////////////////////////////////////
#ifndef SHFRACTIONALIMPL_HPP
#define SHFRACTIONALIMPL_HPP

#include <cmath>
#include "Math.hpp"
#include "Fraction.hpp"

namespace SH {

#define _CompType typename Fraction<T>::CompType 
#define _LongType typename Fraction<T>::LongType 
#define _SignedLongType typename Fraction<T>::SignedLongType 

// @todo replace uses of std::fabs 

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T>::Fraction()
{}

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T>::Fraction(double value)
  : m_val(clamp_val(value))
{
}

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> Fraction<T>::make_fraction(CompType value)
{
  Fraction result;
  result.m_val = clamp_val(value);
  return result; 
}

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> Fraction<T>::make_fraction_signed(SignedLongType value)
{
  Fraction result;
  result.m_val = clamp_val_signed(value);
  return result; 
}

template<typename T/* @todo clamp , bool Clamp */>
template<typename T2>
Fraction<T>::Fraction(const Fraction<T2> &other)
  : m_val(clamp_val(other.get_double())) 
{
}

/** accessor methods **/
template<typename T/* @todo clamp , bool Clamp */>
Fraction<T>::operator double() const
{
  return get_double(); 
}

template<typename T/* @todo clamp , bool Clamp */>
T& Fraction<T>::val()
{
  return m_val;
}

template<typename T/* @todo clamp , bool Clamp */>
T Fraction<T>::val() const
{
  return m_val;
}

/** Arithmetic operators **/
template<typename T/* @todo clamp , bool Clamp */>
Fraction<T>& Fraction<T>::operator=(double value)
{
  m_val = clamp_val(value);
  return *this;
}

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T>& Fraction<T>::operator=(const Fraction& other)
{
  m_val = other.m_val;
  return *this;
}

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T>& Fraction<T>::operator+=(double value)
{
  return operator+=(Fraction(value));
}

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T>& Fraction<T>::operator+=(const Fraction& other)
{
  m_val = clamp_val(CompType(m_val) + CompType(other.m_val));
  return *this;
}

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T>& Fraction<T>::operator-=(double value)
{
  return operator-=(Fraction(value));
}

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T>& Fraction<T>::operator-=(const Fraction& other)
{
  m_val = clamp_val_signed(SignedLongType(m_val) - SignedLongType(other.m_val));
  return *this;
}

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T>& Fraction<T>::operator*=(double value)
{
  return operator*=(Fraction(value)); 
}

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T>& Fraction<T>::operator*=(const Fraction& other)
{
  m_val = clamp_val(CompType(m_val) * CompType(other.m_val));
  return *this;
}

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T>& Fraction<T>::operator/=(double value)
{
  return operator/=(Fraction(value)); 
}

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T>& Fraction<T>::operator/=(const Fraction& other)
{
  LongType numerator = LongType(m_val) << BITS;   
  LongType denom = LongType(other.m_val);
  m_val = clamp_val(numerator / denom);
  return (*this); 
}

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T>& Fraction<T>::operator%=(double value)
{
  return operator%=(Fraction(value));
}

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T>& Fraction<T>::operator%=(const Fraction& other)
{
  // @todo range - should need no clamping for this
  m_val = m_val % other.m_val;
  if(m_val < 0) m_val += other.m_val;
}

/** Negation **/
template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> Fraction<T>::operator-() const 
{
  if(is_signed /* @todo clamp && Clamp */) return make_fraction(0); 
  return make_fraction(-m_val);
}

template<typename TT>
std::ostream& operator<<(std::ostream &out, const Fraction<TT> &value)
{
  out << double(value);
  return out;
}

template<typename TT>
std::istream& operator>>(std::istream &in, Fraction<TT> &value)
{
  double temp;
  in >> temp;
  value = temp;
  return in;
}

template<typename T/* @todo clamp , bool Clamp */>
inline
double Fraction<T>::get_double() const
{
  return double(m_val) / double(ONE);
}

template<typename T/* @todo clamp , bool Clamp */>
inline
T Fraction<T>::clamp_val(double value)
{
  double temp = value * ONE;

/* @todo clamp  if(Clamp) { */
    temp = std::max(std::min(temp, double(MAX)), double(MIN));
/* @todo clamp  } */
  return T(temp);
}

template<typename T/* @todo clamp , bool Clamp */>
inline
T Fraction<T>::clamp_val(CompType value)
{
  /* @todo clamp if(Clamp) { */
    value = std::max(std::min(value, CompType(MAX)), CompType(MIN));
  /* @todo clamp } */
  return T(value);
}

template<typename T/* @todo clamp , bool Clamp */>
inline
T Fraction<T>::clamp_val_signed(SignedLongType value)
{
  /* @todo clamp if(Clamp) { */
    value = std::max(std::min(value, SignedLongType(MAX)), SignedLongType(MIN));
  /* @todo clamp } */
  return T(value);
}

/** Arithmetic operators **/
template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> operator+(const Fraction<T> &a, const Fraction<T> &b) 
{
  return Fraction<T>::make_fraction(_CompType(a.m_val) + _CompType(b.m_val));
}


template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> operator-(const Fraction<T> &a, const Fraction<T> &b) 
{
  return Fraction<T>::make_fraction_signed(
      _SignedLongType(a.m_val) - 
      _SignedLongType(b.m_val));
}

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> operator*(const Fraction<T> &a, const Fraction<T> &b) 
{
  return Fraction<T>::make_fraction(_CompType(a.m_val) * _CompType(b.m_val));
}


template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> operator/(const Fraction<T> &a, const Fraction<T> &b) 
{
  _LongType numerator = _LongType(a.m_val) << Fraction<T>::BITS;   
  _LongType denom = _LongType(b.m_val);
  return Fraction<T>::make_fraction(numerator / denom);
}


template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> operator%(const Fraction<T> &a, const Fraction<T> &b) 
{
  T temp = a.m_val % b.m_val;
  if(temp < 0) temp += b.m_val; 
  return Fraction<T>(temp);
}

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> cbrt(const Fraction<T> &a) 
{
  return Fraction<T>(std::pow(double(a), 1.0 / 3.0)); 
}


template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> exp(const Fraction<T> &a) 
{
  return Fraction<T>(std::exp(double(a))); 
}


template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> exp2(const Fraction<T> &a) 
{
  return Fraction<T>(std::pow(2.0, double(a))); 
}


template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> exp10(const Fraction<T> &a) 
{
  return Fraction<T>(std::pow(10.0, double(a))); 
}


template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> log(const Fraction<T> &a) 
{
  return Fraction<T>(std::log(double(a))); 
}


template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> log2(const Fraction<T> &a) 
{
  return Fraction<T>(log2f(double(a))); 
}


template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> log10(const Fraction<T> &a) 
{
  return Fraction<T>(log10f(double(a))); 
}

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> frac(const Fraction<T> &a)
{
  T result = a.m_val;
  if(result < 0) {
    result += Fraction<T>::ONE;
  }
  return Fraction<T>(result);
}

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> fmod(const Fraction<T> &a, const Fraction<T> &b) 
{
  return a % b;
}


template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> pow(const Fraction<T> &a, const Fraction<T> &b) 
{
  // @todo check if this is optimal
  // @todo do integer special cases? - see NuS.cc
  return Fraction<T>(std::pow(double(a), double(b))); 
}


// not a good function for fractional types...
// guaranteed to overflow...DOH
template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> rcp(const Fraction<T> &a) 
{
  if(a.m_val > 0) return Fraction<T>(Fraction<T>::MAX);
  return Fraction<T>(Fraction<T>::MIN);
}

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> rsq(const Fraction<T> &a) 
{
  return rcp(a); // same bad behaviour 
}

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> sgn(const Fraction<T> &a) 
{
  return Fraction<T>(a.m_val > 0 ? 1 : a.m_val == 0 ? 0 : -1); 
}

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> sqrt(const Fraction<T> &a) 
{
  return Fraction<T>(std::sqrt(double(a)));
}


/** Trig Operators */
template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> acos(const Fraction<T> &a) 
{
  return Fraction<T>(std::acos(double(a)));
}


template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> asin(const Fraction<T> &a) 
{
  return Fraction<T>(std::asin(double(a)));
}


template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> atan(const Fraction<T> &a) 
{
  return Fraction<T>(std::atan(double(a)));
}


template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> atan2(const Fraction<T> &a, const Fraction<T> &b) 
{
  return Fraction<T>(std::atan2(double(a), double(b)));
}


template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> cos(const Fraction<T> &a) 
{
  return Fraction<T>(std::cos(double(a)));
}

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> sin(const Fraction<T> &a) 
{
  return Fraction<T>(std::sin(double(a)));
}


template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> tan(const Fraction<T> &a) 
{
  return Fraction<T>(std::tan(double(a)));
}


/** Comparison Operators **/
template<typename T/* @todo clamp , bool Clamp */>
bool operator<(const Fraction<T> &a, const Fraction<T> &b) 
{
  return (a.m_val < b.m_val);
}

template<typename T/* @todo clamp , bool Clamp */>
bool operator<=(const Fraction<T> &a, const Fraction<T> &b) 
{
  return (a.m_val <= b.m_val);
}

template<typename T/* @todo clamp , bool Clamp */>
bool operator>(const Fraction<T> &a, const Fraction<T> &b) 
{
  return (a.m_val > b.m_val);
}

template<typename T/* @todo clamp , bool Clamp */>
bool operator>=(const Fraction<T> &a, const Fraction<T> &b) 
{
  return (a.m_val >= b.m_val);
}

template<typename T/* @todo clamp , bool Clamp */>
bool operator==(const Fraction<T> &a, const Fraction<T> &b) 
{
  return (a.m_val == b.m_val);
}

template<typename T/* @todo clamp , bool Clamp */>
bool operator!=(const Fraction<T> &a, const Fraction<T> &b) 
{
  return (a.m_val != b.m_val);
}

/** Clamping operators **/
template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> min(const Fraction<T> &a, const Fraction<T> &b) 
{
  return Fraction<T>(std::min(a.m_val, b.m_val));
}

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> max(const Fraction<T> &a, const Fraction<T> &b) 
{
  return Fraction<T>(std::max(a.m_val, b.m_val));
}

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> floor(const Fraction<T> &a) 
{

  T result = 0; 
  if(a.m_val == Fraction<T>::ONE) {
    result = Fraction<T>::ONE;
  } else if(Fraction<T>::is_signed && a.m_val < 0) {
    a.m_val = -Fraction<T>::ONE;
  }
  return Fraction<T>(result); 
}

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> ceil(const Fraction<T> &a) 
{
  T ONE = Fraction<T>::ONE;
  T result = 0; 
  if(a.m_val > 0) {
    result = ONE;
  } else if(Fraction<T>::is_signed && 
      a.m_val == -ONE) {
    result = -ONE;
  }
  return Fraction<T>(result);
}

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> rnd(const Fraction<T> &a) 
{
  T ONE = Fraction<T>::ONE;
  T HALF = ONE >> 1; // slightly less than half
  T result;
  if(a.m_val > HALF) {
    result = ONE;
  } else if(!Fraction<T>::is_signed || result > -HALF) {
    result = 0;
  } else {
    result = -ONE;
  }
  return Fraction<T>(result); 
}

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> abs(const Fraction<T> &a) 
{
  return Fraction<T>(a.m_val < 0 ? -a.m_val: a.m_val);
}

/** Misc operators **/
template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> cond(const Fraction<T> &a, const Fraction<T> &b, 
    const Fraction<T> &c)
{
  return Fraction<T>(a.m_val > 0 ? b.m_val: c.m_val); 
}

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> lerp(const Fraction<T> &a, const Fraction<T> &b, const Fraction<T> &c) 
{
  T ONE = Fraction<T>(Fraction<T>::ONE);
  return a * b + (ONE - a) * c;
}

#undef _CompType
#undef _LongType
#undef _SignedLongType
}

#endif
