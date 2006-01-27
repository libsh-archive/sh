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
#ifndef SHQUATERNION_HPP
#define SHQUATERNION_HPP

#include <iostream>
#include "ShLib.hpp"

namespace SH {

/** A Quaternion.
 */
template<ShBindingType B, typename T = float>
class ShQuaternion
{
  template <ShBindingType B2, typename T2>
  friend std::ostream& operator<<(std::ostream& out, 
                                  const ShQuaternion<B2, T2>& q);
public:
  typedef typename ShHostType<T>::type HostType;
  
  /** \brief Constructor for ShQuaternion.
   *    
   *  Creates a identity ShQuaternion
   */
  ShQuaternion();

  /** \brief Constructor for ShQuaternion with a quaternion as parameter.
   *
   * Creates a ShQuaternion where each element is equal to the one in the 
   * parameters
   * \param other the quaternion from which we will get the values from 
   */  
  template<ShBindingType B2> 
  ShQuaternion(const ShQuaternion<B2, T>& other);

  
  /** \brief Constructor for ShQuaternion with a 4-vector as parameter.
   *
   * Creates a ShQuaternion where each element is equal to the one in 
   * the parameters
   * \param values 4-vector from which we will get the values from 
   */  
  template<ShBindingType B2>
  ShQuaternion(const ShAttrib<4, B2, T, SH_VECTOR>& values);
  
  
  /** \brief Constructor for ShQuaternion with an angle and axis of rotation.
   *
   * Creates a unit ShQuaternion defined by a rotation
   * \param angle angle in radians of the rotation
   * \param axis axis of rotation
   */  
  template<ShBindingType B2, ShBindingType B3>
  ShQuaternion(const ShAttrib<1, B2, T>& angle, 
               const ShAttrib<3, B3, T, SH_VECTOR>& axis);
  
  
  /** \brief Constructor for ShQuaternion with a rotation matrix.
   *
   * Creates a unit ShQuaternion defined by a rotation
   * \param mat matrix defining the rotation
   * \pre det(mat) = 1
   */  
  template<ShBindingType B2>
  ShQuaternion(const ShMatrix<4, 4, B2, T>& mat);
  
  /** \brief Definition of assignment to another quaternion
   *
   * Returns the address of a quaternion from which the values were copied from
   * other
   * \param other ShQuaternion from which we will get the values from
   */  
  template<ShBindingType B2> 
  ShQuaternion& operator=(const ShQuaternion<B2, T>& other);

  /** \brief Definition of the add-assign operation with another quaternion
   *
   * Returns the address of a quaternion where the result is the current 
   * quaternion + right
   * \param right the other quaternion added to this one
   */  
  template<ShBindingType B2>
  ShQuaternion& operator+=(const ShQuaternion<B2, T>& right);
  
  /** \brief Definition of the minus-assign operation with another quaternion
   *
   * Returns the address of a quaternion where the result is the current 
   * quaternion - right
   * \param right the other quaternion subtracted from this one
   */  
  template<ShBindingType B2>
  ShQuaternion& operator-=(const ShQuaternion<B2, T>& right);
  
  /** \brief Definition of the times-assign operation with another quaternion
   *
   * Returns the address of a quaternion where the result is the current 
   * quaternion * right 
   * \param right the other quaternion multiplied to this one
   */  
  template<ShBindingType B2>
  ShQuaternion& operator*=(const ShQuaternion<B2, T>& right);

  
  /** \brief Definition of the times-assign operation with a scalar
   *
   * Returns the address of a quaternion where the result is the current 
   * quaternion (each component) multiplied by right
   * \param right the scalar multiplied to this quaternion
   */  
  template<ShBindingType B2>
  ShQuaternion& operator*=(const ShAttrib<1, B2, T>& right);
  
  /** \brief Definition of the times-assign operation with a 3-vector
   *
   * Returns the address of a quaternion where the result is the current 
   * quaternion * ShQuaternion(0.0, right)
   * \param right 3-vector converted to a quaternion multiplied to this one
   */  
  template<ShBindingType B2>
  ShQuaternion& operator*=(const ShAttrib<3, B2, T, SH_VECTOR>& right);
  
  /** \brief Definition of the times-assign operation with a 3-normal
   *
   * Returns the address of a quaternion where the result is the current 
   * quaternion * ShQuaternion(0.0, right)
   * \param right 3-normal converted to a quaternion multiplied to this one
   */  
  template<ShBindingType B2>
  ShQuaternion& operator*=(const ShAttrib<3, B2, T, SH_NORMAL>& right);

  /** \brief Definition of the add operation with another quaternion
   *
   * Returns a new ShQuaternion equals to the current quaternion + q2
   * \param q2 the other quaternion added to this one
   */  
  template<ShBindingType B2>
  ShQuaternion<SH_TEMP, T> operator+(const ShQuaternion<B2, T>& q2);
  
  /** \brief Definition of the subtract operation with another quaternion
   *
   * Returns a new ShQuaternion equals to the current quaternion - q2
   * \param q2 the other quaternion subtracted from this one
   */  
  template<ShBindingType B2>
  ShQuaternion<SH_TEMP, T> operator-(const ShQuaternion<B2, T>& q2);

  /** \brief Definition of the multiply operation with another quaternion
   *
   * Returns a new ShQuaternion equals to the current quaternion * q2
   * \param q2 the other quaternion multiplied to this one
   */  
  template<ShBindingType B2>
  ShQuaternion<SH_TEMP, T> operator*(const ShQuaternion<B2, T>& q2);
  
  /** \brief Definition of the multiply operation with a scalar
   *
   * Returns a new ShQuaternion equals to the current quaternion * c
   * \param c the scalar multiplied to this one
   */  
  template<ShBindingType B2>
  ShQuaternion<SH_TEMP, T> operator*(const ShAttrib<1, B2, T>& c);
  
  /** \brief Definition of the times operation with a 3-vector
   *
   * Returns a new ShQuaternion equals to the current 
   * quaternion * ShQuaternion(0.0, right)
   * \param q2 3-vector converted to a quaternion multiplied to this one
   */  
  template<ShBindingType B2>
  ShQuaternion<SH_TEMP, T> operator*(const ShAttrib<3, B2, T, SH_VECTOR>& q2);
  
  /** \brief Definition of the times operation with a 3-normal
   *
   * Returns a new ShQuaternion equals to the current 
   * quaternion * ShQuaternion(0.0, right)
   * \param q2 3-normal converted to a quaternion multiplied to this one
   */  
  template<ShBindingType B2>
  ShQuaternion<SH_TEMP, T> operator*(const ShAttrib<3, B2, T, SH_NORMAL>& q2);

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
  template<ShBindingType B2>
  ShAttrib<1, SH_TEMP, T> dot(const ShQuaternion<B2, T>& q) const;
  
  /** \brief Definition of the conjugate function
   * 
   * Returns the conjugate of this quaternion
   */
  ShQuaternion<SH_TEMP, T> conjugate() const;
  
  /** \brief Definition of the inverse function
   * 
   * Returns the inverse of this quaternion (same as conjugate if unit)
   */
  ShQuaternion<SH_TEMP, T> inverse() const;
  
  /** \brief Definition of the getMatrix function
   * 
   * Returns the rotation matrix defined by this quaternion
   * \pre this quaternion is unit
   */
  ShMatrix<4, 4, SH_TEMP, T> getMatrix() const;

  /** \brief Definition of the getVector function
   * 
   * Returns the values of this quaternion as a vector
   */
  ShAttrib<4, SH_TEMP, T, SH_VECTOR> getVector() const;
private:
  ShAttrib<4, B, T, SH_VECTOR> m_data;
};

template<ShBindingType B, typename T, ShBindingType B2>
extern ShQuaternion<SH_TEMP, T> 
operator*(const ShAttrib<1, B2, T>& c, const ShQuaternion<B, T>& q); 

template<ShBindingType B1, ShBindingType B2, typename T>
extern ShQuaternion<SH_TEMP, T>
slerp(const ShQuaternion<B1, T>& q1, const ShQuaternion<B2, T>& q2, 
    const ShAttrib1f& t);

typedef ShQuaternion<SH_INPUT, float> ShInputQuaternionf;
typedef ShQuaternion<SH_OUTPUT, float> ShOutputQuaternionf;
typedef ShQuaternion<SH_TEMP, float> ShQuaternionf;
}

#include "ShQuaternionImpl.hpp"

#endif