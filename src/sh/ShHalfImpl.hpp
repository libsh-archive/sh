// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#ifndef SHHALFALIMPL_HPP
#define SHHALFALIMPL_HPP

#include <iostream>
#include <cmath>
#include "ShMath.hpp"
#include "ShHalf.hpp"

namespace SH {

inline
ShHalf::ShHalf()
{}

inline
ShHalf::ShHalf(double value)
  : m_val(to_val(value))
{
}

inline
ShHalf::operator double() const
{
  return get_double();
}


/** Arithmetic operators **/
inline
ShHalf& ShHalf::operator=(double value)
{
  m_val = to_val(value);
  return *this;
}

inline
ShHalf& ShHalf::operator=(const ShHalf& other)
{
  m_val = other.m_val;
  return *this;
}

inline
ShHalf& ShHalf::operator+=(double value)
{
  set_val(get_double() + value); 
  return *this;
}

inline
ShHalf& ShHalf::operator+=(const ShHalf& other)
{
  set_val(get_double() + other.get_double());
  return *this;
}

inline
ShHalf& ShHalf::operator-=(double value)
{
  set_val(get_double() - value);
  return *this; 
}

inline
ShHalf& ShHalf::operator-=(const ShHalf& other)
{
  set_val(get_double() - other.get_double());
  return *this; 
}

inline
ShHalf& ShHalf::operator*=(double value)
{
  set_val(get_double() * value);
  return *this; 
}

inline
ShHalf& ShHalf::operator*=(const ShHalf& other)
{
  set_val(get_double() * other.get_double());
  return *this;
}

inline
ShHalf& ShHalf::operator/=(double value)
{
  set_val(get_double() / value);
  return *this; 
}

inline
ShHalf& ShHalf::operator/=(const ShHalf& other)
{
  set_val(get_double() / other.get_double());
  return *this;
}

inline
ShHalf& ShHalf::operator%=(double value)
{
  set_val(std::fmod(get_double(), value));
  return *this; 
}

inline
ShHalf& ShHalf::operator%=(const ShHalf& other)
{
  set_val(std::fmod(get_double(), other.get_double()));
  return *this;
}

/** Negation **/
inline
ShHalf ShHalf::operator-() const 
{
  //twiddle sign bit
  T result = m_val ^ S;
  return make_half(result);
}

inline
std::ostream& operator<<(std::ostream &out, const ShHalf &value)
{
  out << double(value); 
  return out;
}

inline
std::istream& operator>>(std::istream &in, ShHalf &value)
{
  double temp;
  in >> temp;
  value = temp;
  return in;
}

inline
ShHalf ShHalf::make_half(T value)
{
  ShHalf result;
  result.m_val = value; 
  return result;
}

inline
ShHalf::T ShHalf::to_val(double value) {
  int exponent;
  double fraction = frexp(value, &exponent);
  short sign = fraction < 0;
  fraction = sign ? -fraction : fraction;
  T result;

  // @todo range - use OpenEXR's version since this doesn't handle NaN and 
  // INF in value, and is rather slow. 
  result = (sign << 15);
  if(fraction == 0) { // zero
  } else if(fabs(value) > 65504) { // INF
    result |= 31 << 10;
  } else if(exponent < -13) { // denormalized 
    int significand = (int)(fraction * (1LL << (exponent + 24)));
    result |= significand;
  } else { // normalized 
    int significand = (int)((fraction - 0.5) * (1 << 11));
    result |= ((exponent + 14) << 10) | significand;
  }
  return result;
}

inline
void ShHalf::set_val(double value) {
  m_val = to_val(value);
}

inline
double ShHalf::get_double() const {
  // @todo range - use OpenEXR's version since this is probably slow
  short sign = -((m_val >> 14) & 2) + 1; // -1 for negative, +1 for positive                   
  short exponent = (m_val >> 10) & 0x1F;
  short significand = m_val & 0x3FF;
  double fraction = sign * ((exponent ? 1 : 0) + significand / (double)(1 << 10));

  return ldexp(fraction, (exponent ? exponent - 15 : -14));
}

}

#endif
