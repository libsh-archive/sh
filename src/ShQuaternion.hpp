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
#ifndef SHQUATERNION_HPP
#define SHQUATERNION_HPP

#include <iostream>
#include "ShLib.hpp"

namespace SH {
template<int K, typename T=float>
class ShQuaternion
{
  template <int K2, typename T2>
  friend std::ostream& operator<<(std::ostream& out, 
                                  const ShQuaternion<K2, T2>& q);
public:
  ShQuaternion();
  template<int K2> 
  ShQuaternion(const ShQuaternion<K2, T>& other);
  template<int K2>
  ShQuaternion(const ShVector<4, K2, T>& values);
  template<int K2, int K3>
  ShQuaternion(const ShAttrib<1, K2, T>& angle, 
               const ShVector<3, K3, T>& axis);
  
  // create a quaternion using a matrix, the input matrix must be orthogonal
  // (ie. det(mat) = 1)
  template<int K2>
  ShQuaternion(const ShMatrix<4, 4, K2, T>& mat);
  template<int K2> 
  ShQuaternion& operator=(const ShQuaternion<K2, T>& other);
  template<int K2>
  ShQuaternion& operator+=(const ShQuaternion<K2, T>& right);
  template<int K2>
  ShQuaternion& operator-=(const ShQuaternion<K2, T>& right);
  template<int K2>
  ShQuaternion& operator*=(const ShQuaternion<K2, T>& right);
  template<int K2>
  ShQuaternion& operator*=(const ShAttrib<1, K2, T>& right);
  template<int K2>
  ShQuaternion<SH_TEMP, T> operator+(const ShQuaternion<K2, T>& q2);
  template<int K2>
  ShQuaternion<SH_TEMP, T> operator-(const ShQuaternion<K2, T>& q2);
  template<int K2>
  ShQuaternion<SH_TEMP, T> operator*(const ShQuaternion<K2, T>& q2);
  template<int K2>
  ShQuaternion<SH_TEMP, T> operator*(const ShAttrib<1, K2, T>& c);
  void normalize();
  void getValues(T values []) const;
  void setUnit(bool flag);
  /// Set this variable's name. If set to the empty string, defaults
  /// to the type and id of the variable.
  void name(const std::string& name);
  std::string name() const; ///< Get this variable's name


  template<int K2>
  ShAttrib<1, SH_TEMP, T> dot(const ShQuaternion<K2, T>& q) const;
  ShQuaternion<SH_TEMP, T> conjugate() const;
  ShQuaternion<SH_TEMP, T> inverse() const;
  ShMatrix<4, 4, SH_TEMP, T> getMatrix() const;
  ShVector<4, SH_TEMP, T> getVector() const;
private:
  ShVector<4, K, T> m_data;
};

template<int K, typename T, int K2>
extern ShQuaternion<SH_TEMP, T> 
operator*(const ShAttrib<1, K2, T>& c, const ShQuaternion<K, T>& q); 

template<int K1, int K2, typename T>
extern ShQuaternion<SH_TEMP, T>
slerp(const ShQuaternion<K1, T>& q1, const ShQuaternion<K2, T>& q2, 
    const ShAttrib1f& t);

typedef ShQuaternion<SH_INPUT, float> ShInputQuaternionf;
typedef ShQuaternion<SH_OUTPUT, float> ShOutputQuaternionf;
typedef ShQuaternion<SH_TEMP, float> ShQuaternionf;
}

#include "ShQuaternionImpl.hpp"

#endif
