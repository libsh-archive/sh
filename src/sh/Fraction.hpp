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
#ifndef SHFRACTION_HPP
#define SHFRACTION_HPP

#include <limits>
#include "Utility.hpp"

namespace SH {

/** Determines the computation used for intermediate values 
 * This means that the maximum fractiona type T supported has half as many bits
 * as the largets integer type supported in hardware */ 
template<typename T> struct FractionLongType { typedef T type; };

// @todo range - making certain assumptions about sizes of type 
// Make sure these are valid under all circumstances
// May want to use int instead of short in some cases if hardware needs
// to convert shorts.
template<> struct FractionLongType<int> { typedef long long type; };
template<> struct FractionLongType<short> { typedef int type; };
template<> struct FractionLongType<char> { typedef short type; };
template<> struct FractionLongType<unsigned int> { typedef unsigned long long type; };
template<> struct FractionLongType<unsigned short> { typedef unsigned int type; };
template<> struct FractionLongType<unsigned char> { typedef unsigned short type; };

template<typename T> struct FractionSignedLongType { typedef T type; };

// @todo range - making certain assumptions about sizes of type; 
// Make sure these are valid under all circumstances
template<> struct FractionSignedLongType<int> { typedef long long type; };
template<> struct FractionSignedLongType<short> { typedef int type; };
template<> struct FractionSignedLongType<char> { typedef short type; };
template<> struct FractionSignedLongType<unsigned int> { typedef long long type; };
template<> struct FractionSignedLongType<unsigned short> { typedef int type; };
template<> struct FractionSignedLongType<unsigned char> { typedef short type; };

/*** Sh class for fraction types represented in integers.  
 *
 * Inspired by NuFixed.hh and NuFixed.cc from the MetaMedia project.
 *
 * @param T integer type to use
 */

/** 
 * This param does not exist any more because the default param broke some stuff 
 * like IsFraction in StorageType.hpp under VC.NET, and I don't have time to 
 * fix it right now.  
 *
 * All Fractions are by default clamped.
 * Everything that was commented out has been marked with a @todo clamp
 *
 *
 * @param Clamp whether to clamp to avoid overflow.  If this is true, then
 * during computation we always use a temporary type with enough bits to hold
 * the result.  If this is false, then we only use temporaries with extra bits
 * if an intermediate value may overflow.  If the result itself overflows,
 * then the value stored is implementation defined (wraps around for 2's complement). 
 */

template<typename T /* @todo clamp , bool Clamp=true */>
struct Fraction {

  // Type to use for operations that require temps with twice the bits
  // (and when clamping is on)
  typedef typename FractionLongType<T>::type LongType; 
  typedef typename FractionSignedLongType<T>::type SignedLongType; 

  // The usual type used in computation
  // @todo clamp typedef typename SelectType<Clamp, LongType, T>::type CompType; 
  typedef LongType CompType; 

  // Some information about the type and constant values representable by
  // this fraction type
  static const bool is_signed = std::numeric_limits<T>::is_signed;
  static const int BITS = sizeof(T) * 8; ///< number of bits
  static const T ONE; ///< representation of ONE
  static const T MAX; ///< maximum representable value
  static const T MIN;  ///< minumum representable value

  T m_val;

  /** Constructs an fraction with undefined value */
  Fraction();

  /** Constructs an fraction */
  Fraction(double value);

  /** Makes a fraction and clamps from the computation type */
  static Fraction make_fraction(CompType value);

  /** Makes a fraction and clamps from the signed type */
  static Fraction make_fraction_signed(SignedLongType value);

  template<typename T2>
  Fraction(const Fraction<T2> &other);

  /** accessor methods **/
  operator double() const;
  T& val();
  T val() const;


  /** Arithmetic operators **/
  Fraction& operator=(double value);
  Fraction& operator=(const Fraction& other);
  Fraction& operator+=(double value);
  Fraction& operator+=(const Fraction& other);
  Fraction& operator-=(double value);
  Fraction& operator-=(const Fraction& other);
  Fraction& operator*=(double value);
  Fraction& operator*=(const Fraction& other);
  Fraction& operator/=(double value);
  Fraction& operator/=(const Fraction& other);

  /** Float modulus - result is always positive 
   *@{*/
  Fraction& operator%=(double value);
  Fraction& operator%=(const Fraction& other);
  // @}

  /** Scalar arithmetic operators **/

  /** Negation **/
  Fraction operator-() const;

  /** Output operator **/
  template<typename TT>
  friend std::ostream& operator<<(std::ostream& out, const Fraction<TT> &value);


  /** Input operator (format matches output) **/
  template<typename TT>
  friend std::istream& operator>>(std::istream& out, Fraction<TT> &value);

  private:
    // convert value to double
    double get_double() const;

    // convert double to value (may clamp)
    static T clamp_val(double value);

    // convert temporary computation type to value (may clamp) 
    // this works just fine for the non-clamp case as well as well
    static T clamp_val(CompType temp);

    // convert temporary computation type to value (may clamp) 
    static T clamp_val_signed(SignedLongType temp);
};

template<typename T/* @todo clamp , bool Clamp */>
const T Fraction<T>::ONE = std::numeric_limits<T>::max(); 

template<typename T/* @todo clamp , bool Clamp */>
const T Fraction<T>::MAX = Fraction<T>::ONE; 

template<typename T/* @todo clamp , bool Clamp */>
const T Fraction<T>::MIN = is_signed ? -Fraction<T>::ONE : 0;  

/** Arithmetic operators **/
template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> operator+(const Fraction<T> &a, const Fraction<T> &b);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> operator-(const Fraction<T> &a, const Fraction<T> &b);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> operator*(const Fraction<T> &a, const Fraction<T> &b);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> operator/(const Fraction<T> &a, const Fraction<T> &b);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> operator%(const Fraction<T> &a, const Fraction<T> &b);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> cbrt(const Fraction<T> &a);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> exp(const Fraction<T> &a);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> exp2(const Fraction<T> &a);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> exp10(const Fraction<T> &a);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> log(const Fraction<T> &a);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> log2(const Fraction<T> &a);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> log10(const Fraction<T> &a);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> frac(const Fraction<T> &a);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> fmod(const Fraction<T> &a, const Fraction<T> &b);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> pow(const Fraction<T> &a, const Fraction<T> &b);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> rcp(const Fraction<T> &a);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> rsq(const Fraction<T> &a);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> sgn(const Fraction<T> &a);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> sqrt(const Fraction<T> &a);

/** Trig Operators */
template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> acos(const Fraction<T> &a);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> asin(const Fraction<T> &a);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> atan(const Fraction<T> &a);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> atan2(const Fraction<T> &a, const Fraction<T> &b);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> cos(const Fraction<T> &a); 
template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> sin(const Fraction<T> &a);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> tan(const Fraction<T> &a);

/** Comparison Operators **/
template<typename T/* @todo clamp , bool Clamp */>
bool operator<(const Fraction<T> &a, const Fraction<T> &b);

template<typename T/* @todo clamp , bool Clamp */>
bool operator<=(const Fraction<T> &a, const Fraction<T> &b);

template<typename T/* @todo clamp , bool Clamp */>
bool operator>(const Fraction<T> &a, const Fraction<T> &b);

template<typename T/* @todo clamp , bool Clamp */>
bool operator>=(const Fraction<T> &a, const Fraction<T> &b);

template<typename T/* @todo clamp , bool Clamp */>
bool  operator==(const Fraction<T> &a, const Fraction<T> &b);

template<typename T/* @todo clamp , bool Clamp */>
bool operator!=(const Fraction<T> &a, const Fraction<T> &b);

/** Clamping operators **/
template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> min(const Fraction<T> &a, const Fraction<T> &b);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> max(const Fraction<T> &a, const Fraction<T> &b);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> floor(const Fraction<T> &a);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> ceil(const Fraction<T> &a);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> rnd(const Fraction<T> &a);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> abs(const Fraction<T> &a);

/** Misc Operators **/
template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> cond(const Fraction<T> &a, const Fraction<T> &b, const Fraction<T> &c);

template<typename T/* @todo clamp , bool Clamp */>
Fraction<T> lerp(const Fraction<T> &a, const Fraction<T> &b, const Fraction<T> &c);

typedef Fraction<int> FracInt;
typedef Fraction<short> FracShort;
typedef Fraction<char> FracByte;

typedef Fraction<unsigned int> FracUInt;
typedef Fraction<unsigned short> FracUShort;
typedef Fraction<unsigned char> FracUByte;

}


#include "FractionImpl.hpp"
  
#endif
