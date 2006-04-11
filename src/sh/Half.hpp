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
#ifndef SHHALF_HPP
#define SHHALF_HPP

#include <limits>
#include "Utility.hpp"

namespace SH {

struct Half {
  typedef unsigned short T; 

  static const int S = 1 << 15; // sign bit
  static const int E = 1 << 10; // exponent 

  T m_val;

  /** Constructs an half with undefined value */
  Half();

  /** Constructs an half */
  Half(double value); 

  operator double() const;

  /** Arithmetic operators **/
  Half& operator=(double value);
  Half& operator=(const Half& other);
  Half& operator+=(double value);
  Half& operator+=(const Half& other);
  Half& operator-=(double value);
  Half& operator-=(const Half& other);
  Half& operator*=(double value);
  Half& operator*=(const Half& other);
  Half& operator/=(double value);
  Half& operator/=(const Half& other);

  /** Float modulus - result is always positive 
   *@{*/
  Half& operator%=(double value);
  Half& operator%=(const Half& other);
  // @}

  /** Negation **/
  Half operator-() const;

  /** Output operator **/
  friend std::ostream& operator<<(std::ostream& out, const Half &value);


  /** Input operator (format matches output) **/
  friend std::istream& operator>>(std::istream& out, Half &value);

  private:
    /** Constructs a half */
    static Half make_half(T value);

    static T to_val(double value);
    void set_val(double value);
    double get_double() const;
};

}

#include "HalfImpl.hpp"
  
#endif
