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
#ifndef SHQUATERNION_HPP
#define SHQUATERNION_HPP

#include <iostream>
#include "Lib.hpp"

namespace SH {

/** A Quaternion.
 */
template<BindingType B, typename T = float>
class Quaternion
{
  template <BindingType B2, typename T2>
  friend std::ostream& operator<<(std::ostream& out, 
                                  const Quaternion<B2, T2>& q);
public:
  typedef typename HostType<T>::type HostType;
  
  /** \brief Constructor for Quaternion.
   *    
   *  Creates a identity Quaternion
   */
  Quaternion();

  /** \brief Constructor for Quaternion with a quaternion as parameter.
   *
   * Creates a Quaternion where each element is equal to the one in the 
   * parameters
   * \param other the quaternion from which we will get the values from 
   */  
  template<BindingType B2> 
  Quaternion(const Quaternion<B2, T>& other);

  
  /** \brief Constructor for Quaternion with a 4-vector as parameter.
   *
   * Creates a Quaternion where each element is equal to the one in 
   * the parameters
   * \param values 4-vector from which we will get the values from 
   */  
  template<BindingType B2>
  Quaternion(const Attrib<4, B2, T, SH_VECTOR>& values);
  
  
  /** \brief Constructor for Quaternion with an angle and axis of rotation.
   *
   * Creates a unit Quaternion defined by a rotation
   * \param angle angle in radians of the rotation
   * \param axis axis of rotation
   */  
  template<BindingType B2, BindingType B3>
  Quaternion(const Attrib<1, B2, T>& angle, 
               const Attrib<3, B3, T, SH_VECTOR>& axis);
  
  
  /** \brief Constructor for Quaternion with a rotation matrix.
   *
   * Creates a unit Quaternion defined by a rotation
   * \param mat matrix defining the rotation
   * \pre det(mat) = 1
   */  
  template<BindingType B2>
  Quaternion(const Matrix<4, 4, B2, T>& mat);
  
  /** \brief Definition of assignment to another quaternion
   *
   * Returns the address of a quaternion from which the values were copied from
   * other
   * \param other Quaternion from which we will get the values from
   */  
  template<BindingType B2> 
  Quaternion& operator=(const Quaternion<B2, T>& other);

  /** \brief Definition of the add-assign operation with another quaternion
   *
   * Returns the address of a quaternion where the result is the current 
   * quaternion + right
   * \param right the other quaternion added to this one
   */  
  template<BindingType B2>
  Quaternion& operator+=(const Quaternion<B2, T>& right);
  
  /** \brief Definition of the minus-assign operation with another quaternion
   *
   * Returns the address of a quaternion where the result is the current 
   * quaternion - right
   * \param right the other quaternion subtracted from this one
   */  
  template<BindingType B2>
  Quaternion& operator-=(const Quaternion<B2, T>& right);
  
  /** \brief Definition of the times-assign operation with another quaternion
   *
   * Returns the address of a quaternion where the result is the current 
   * quaternion * right 
   * \param right the other quaternion multiplied to this one
   */  
  template<BindingType B2>
  Quaternion& operator*=(const Quaternion<B2, T>& right);

  
  /** \brief Definition of the times-assign operation with a scalar
   *
   * Returns the address of a quaternion where the result is the current 
   * quaternion (each component) multiplied by right
   * \param right the scalar multiplied to this quaternion
   */  
  template<BindingType B2>
  Quaternion& operator*=(const Attrib<1, B2, T>& right);
  
  /** \brief Definition of the times-assign operation with a 3-vector
   *
   * Returns the address of a quaternion where the result is the current 
   * quaternion * Quaternion(0.0, right)
   * \param right 3-vector converted to a quaternion multiplied to this one
   */  
  template<BindingType B2>
  Quaternion& operator*=(const Attrib<3, B2, T, SH_VECTOR>& right);
  
  /** \brief Definition of the times-assign operation with a 3-normal
   *
   * Returns the address of a quaternion where the result is the current 
   * quaternion * Quaternion(0.0, right)
   * \param right 3-normal converted to a quaternion multiplied to this one
   */  
  template<BindingType B2>
  Quaternion& operator*=(const Attrib<3, B2, T, SH_VECTOR>& right);

  /** \brief Definition of the add operation with another quaternion
   *
   * Returns a new Quaternion equals to the current quaternion + q2
   * \param q2 the other quaternion added to this one
   */  
  template<BindingType B2>
  Quaternion<SH_TEMP, T> operator+(const Quaternion<B2, T>& q2);
  
  /** \brief Definition of the subtract operation with another quaternion
   *
   * Returns a new Quaternion equals to the current quaternion - q2
   * \param q2 the other quaternion subtracted from this one
   */  
  template<BindingType B2>
  Quaternion<SH_TEMP, T> operator-(const Quaternion<B2, T>& q2);

  /** \brief Definition of the multiply operation with another quaternion
   *
   * Returns a new Quaternion equals to the current quaternion * q2
   * \param q2 the other quaternion multiplied to this one
   */  
  template<BindingType B2>
  Quaternion<SH_TEMP, T> operator*(const Quaternion<B2, T>& q2);
  
  /** \brief Definition of the multiply operation with a scalar
   *
   * Returns a new Quaternion equals to the current quaternion * c
   * \param c the scalar multiplied to this one
   */  
  template<BindingType B2>
  Quaternion<SH_TEMP, T> operator*(const Attrib<1, B2, T>& c);
  
  /** \brief Definition of the times operation with a 3-vector
   *
   * Returns a new Quaternion equals to the current 
   * quaternion * Quaternion(0.0, right)
   * \param q2 3-vector converted to a quaternion multiplied to this one
   */  
  template<BindingType B2>
  Quaternion<SH_TEMP, T> operator*(const Attrib<3, B2, T, SH_VECTOR>& q2);
  
  /** \brief Definition of the times operation with a 3-normal
   *
   * Returns a new Quaternion equals to the current 
   * quaternion * Quaternion(0.0, right)
   * \param q2 3-normal converted to a quaternion multiplied to this one
   */  
  template<BindingType B2>
  Quaternion<SH_TEMP, T> operator*(const Attrib<3, B2, T, SH_VECTOR>& q2);

  /** \brief Definition of the normalize function
   *
   * Normalizes the current quaternion which makes it unit
   */  
  void normalize();
  
  /** \brief Definition of the getValues function
   *
   * Outputs the current content of the quaternion as a T array
   * \param values output T array
   */  
  void getValues(HostType values []) const;

  /** \brief Definition of the setUnit function
   *
   * Manually indicate whether the quaternion is unit or non-unit
   * \param flag true or false
   */  
  void setUnit(bool flag);

  /** \brief Definition of the name function
   * 
   * Set this variable's name. If set to the empty string, defaults
   * to the type and id of the variable.
   * \param name the name string
   */
  void name(const std::string& name);
  
  /** \brief Definition of the name function
   * 
   * Returns this variable's name.
   */
  std::string name() const; 

  /** \brief Definition of the dot function
   * 
   * Returns the dot product between this quaternion and q
   * \param q quaternion we're taking the dot product with
   */
  template<BindingType B2>
  Attrib<1, SH_TEMP, T> dot(const Quaternion<B2, T>& q) const;
  
  /** \brief Definition of the conjugate function
   * 
   * Returns the conjugate of this quaternion
   */
  Quaternion<SH_TEMP, T> conjugate() const;
  
  /** \brief Definition of the inverse function
   * 
   * Returns the inverse of this quaternion (same as conjugate if unit)
   */
  Quaternion<SH_TEMP, T> inverse() const;
  
  /** \brief Definition of the getMatrix function
   * 
   * Returns the rotation matrix defined by this quaternion
   * \pre this quaternion is unit
   */
  Matrix<4, 4, SH_TEMP, T> getMatrix() const;

  /** \brief Definition of the getVector function
   * 
   * Returns the values of this quaternion as a vector
   */
  Attrib<4, SH_TEMP, T, SH_VECTOR> getVector() const;
private:
  Attrib<4, B, T, SH_VECTOR> m_data;
};

template<BindingType B, typename T, BindingType B2>
extern Quaternion<SH_TEMP, T> 
operator*(const Attrib<1, B2, T>& c, const Quaternion<B, T>& q); 

template<BindingType B1, BindingType B2, typename T>
extern Quaternion<SH_TEMP, T>
slerp(const Quaternion<B1, T>& q1, const Quaternion<B2, T>& q2, 
    const Attrib1f& t);

typedef Quaternion<SH_INPUT, float> InputQuaternionf;
typedef Quaternion<SH_OUTPUT, float> OutputQuaternionf;
typedef Quaternion<SH_TEMP, float> Quaternionf;
}

#include "QuaternionImpl.hpp"

#endif
