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
#ifndef SHHALF_HPP
#define SHHALF_HPP

#include <limits>
#include "ShUtility.hpp"

namespace SH {

struct ShHalf {
  typedef unsigned short T; 

  static const int S = 1 << 15; // sign bit
  static const int E = 1 << 10; // exponent 

  T m_val;

  /** Constructs an half with undefined value */
  ShHalf();

  /** Constructs an half */
  ShHalf(double value); 

  operator double() const;

  /** Arithmetic operators **/
  ShHalf& operator=(double value);
  ShHalf& operator=(const ShHalf& other);
  ShHalf& operator+=(double value);
  ShHalf& operator+=(const ShHalf& other);
  ShHalf& operator-=(double value);
  ShHalf& operator-=(const ShHalf& other);
  ShHalf& operator*=(double value);
  ShHalf& operator*=(const ShHalf& other);
  ShHalf& operator/=(double value);
  ShHalf& operator/=(const ShHalf& other);

  /** Float modulus - result is always positive 
   *@{*/
  ShHalf& operator%=(double value);
  ShHalf& operator%=(const ShHalf& other);
  // @}

  /** Negation **/
  ShHalf operator-() const;

  /** Output operator **/
  friend std::ostream& operator<<(std::ostream& out, const ShHalf &value);


  /** Input operator (format matches output) **/
  friend std::istream& operator>>(std::istream& out, ShHalf &value);

  private:
    /** Constructs a half */
    static ShHalf make_half(T value);

    static T to_val(double value);
    void set_val(double value);
    double get_double() const;
};

}

#include "ShHalfImpl.hpp"
  
#endif
