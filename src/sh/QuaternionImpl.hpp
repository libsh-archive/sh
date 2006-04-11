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
#include "Quaternion.hpp"

namespace SH {
template<BindingType B, typename T>
Quaternion<B, T>::Quaternion() 
{
  if (B == SH_TEMP) 
    {
      m_data = Vector4f(1.0, 0.0, 0.0, 0.0);
      //m_data.setUnit(true);
    }
}

template<BindingType B, typename T>
template<BindingType B2>
Quaternion<B, T>::Quaternion(const Quaternion<B2, T>& other)
  : m_data(other.getVector())
{
}

template<BindingType B, typename T>
template<BindingType B2>
Quaternion<B, T>::Quaternion(const Attrib<4, B2, T, SH_VECTOR>& values)
  : m_data(values)
{
}

template<BindingType B, typename T>
template<BindingType B2, BindingType B3>   
Quaternion<B, T>::Quaternion(const Attrib<1, B2, T>& angle, 
                                 const Attrib<3, B3, T, SH_VECTOR>& axis)
{
  m_data(0) = cos(angle/2.0);
  m_data(1,2,3) = SH::normalize(axis);
  m_data(1,2,3) *= sin(angle/2.0);
  //m_data.setUnit(true);
}

template<BindingType B, typename T>
template<BindingType B2>
Quaternion<B, T>::Quaternion(const Matrix<4, 4, B2, T>& mat)
{
  Attrib1f trace = 1.0 + mat[0](0) + mat[1](1) + mat[2](2);
  trace = (trace >= 0.0)*trace + (trace < 0.0)*0.0;
  Attrib1f c0 = (trace > 0.001);
  Attrib1f c1 = ((mat[0](0) > mat[1](1))*(mat[0](0) > mat[2](2)));
  Attrib1f c2 = ( mat[1](1) > mat[2](2) );
  Vector4f res0, res1, res2, res3;
  Attrib1f S0 = sqrt(trace) * 2.0;
  S0 += (S0 == 0.0)*1.0;
  
  res0(0) = 0.25 * S0;
  res0(1) = (mat[1](2) - mat[2](1)) / S0;
  res0(2) = (mat[2](0) - mat[0](2)) / S0;
  res0(3) = (mat[0](1) - mat[1](0)) / S0;
  
  trace = 1.0 + mat[0](0) - mat[1](1) - mat[2](2);
  trace = (trace >= 0.0)*trace + (trace < 0.0)*0.0;
  Attrib1f S1 = sqrt(trace) * 2.0;
  S1 += (S1 == 0.0)*1.0;
  
  res1(0) = (mat[2](1) - mat[1](2)) / S1;
  res1(1) = 0.25 * S1;
  res1(2) = (mat[0](1) + mat[1](0)) / S1;
  res1(3) = (mat[2](0) + mat[0](2)) / S1;
  
  trace = 1.0 - mat[0](0) + mat[1](1) - mat[2](2);
  trace = (trace >= 0.0)*trace + (trace < 0.0)*0.0;
  Attrib1f S2 = sqrt(trace) * 2.0;
  S2 += (S2 == 0.0)*1.0;
  
  res2(0) = (mat[2](0) - mat[0](2)) / S2;
  res2(1) = (mat[0](1) + mat[1](0)) / S2;
  res2(2) = 0.25 * S2;
  res2(3) = (mat[1](2) + mat[2](1)) / S2;
  
  trace = 1.0 - mat[0](0) - mat[1](1) + mat[2](2);
  trace = (trace >= 0.0)*trace + (trace < 0.0)*0.0;
  Attrib1f S3 = sqrt(trace) * 2.0;
  S3 += (S3 == 0.0)*1.0;
  
  res3(0) = (mat[1](0) - mat[0](1)) / S3;
  res3(1) = (mat[2](0) + mat[0](2)) / S3;
  res3(2) = (mat[1](2) + mat[2](1)) / S3;
  res3(3) = 0.25 * S3;
  
  m_data = c0*res0 + 
    (c0 == 0.0)*(c1*res1 + (c1 == 0.0)*(c2*res2 + (c2 == 0.0)*res3));
  //m_data.setUnit(true);
}

template<BindingType B, typename T>
std::ostream& operator<<(std::ostream& out, const Quaternion<B, T>& q)
{
  float vals[4];
  q.m_data.getValues(vals);
  out << "Quaternion: " << vals[0] << " " << vals[1] << " " << vals[2] 
      << " " << vals[3];
  return out;
}

template<BindingType B, typename T>
template<BindingType B2>
Quaternion<B, T>& 
Quaternion<B, T>::operator=(const Quaternion<B2, T>& other) 
{
  m_data = other.getVector();
  return *this;
}

template<BindingType B, typename T>
template<BindingType B2>
Quaternion<B, T>& 
Quaternion<B, T>::operator+=(const Quaternion<B2, T>& right) 
{
  m_data += right.getVector();
  return *this;
}

template<BindingType B, typename T>
template<BindingType B2>
Quaternion<B, T>& 
Quaternion<B, T>::operator-=(const Quaternion<B2, T>& right) 
{
  m_data -= right.getVector();
  return *this;
}

template<BindingType B, typename T>
template<BindingType B2>
Quaternion<B, T>& 
Quaternion<B, T>::operator*=(const Quaternion<B2, T>& right) 
{
  Vector4f result;
  Vector4f rightData = right.getVector();
  result(0) = 
    m_data(0)*rightData(0) - SH::dot(m_data(1,2,3), rightData(1,2,3));
  result(1,2,3) = 
    m_data(0)*rightData(1,2,3) + rightData(0)*m_data(1,2,3) + 
    cross(m_data(1,2,3), rightData(1,2,3));

  //result.setUnit(m_data.isUnit() && rightData.isUnit());
  m_data = result;
  return *this;
}

template<BindingType B, typename T>
template<BindingType B2>
Quaternion<B, T>& 
Quaternion<B, T>::operator*=(const Attrib<1, B2, T>& right) 
{
  m_data = m_data*right;
  return *this;
}

template<BindingType B, typename T>
template<BindingType B2>
Quaternion<B, T>& 
Quaternion<B, T>::operator*=(const Attrib<3, B2, T, SH_VECTOR>& right) 
{
  Vector4f v;
  v(0) = 0.0;
  v(1,2,3) = right;
  //v.setUnit(right.isUnit());
  *this *= Quaternionf(v);
  return *this;
}

template<BindingType B, typename T>
template<BindingType B2>
Quaternion<B, T>& 
Quaternion<B, T>::operator*=(const Attrib<3, B2, T, SH_NORMAL>& right) 
{
  Vector4f v;
  v(0) = 0.0;
  v(1,2,3) = right;
  //v.setUnit(right.isUnit());
  *this *= Quaternionf(v);
  return *this;
}

template<BindingType B, typename T>
template<BindingType B2>
Attrib<1, SH_TEMP, T> 
Quaternion<B, T>::dot(const Quaternion<B2, T>& q) const 
{
  return SH::dot(m_data, q.getVector());
}

template<BindingType B, typename T>
Quaternion<SH_TEMP, T> Quaternion<B, T>::conjugate() const 
{
  Vector4f conjData;
  conjData(0) = m_data(0);
  conjData(1, 2, 3) = -m_data(1, 2, 3);
  //conjData.setUnit(m_data.isUnit());

  return Quaternion<SH_TEMP>(conjData);
}

template<BindingType B, typename T>
Quaternion<SH_TEMP, T> Quaternion<B, T>::inverse() const 
{
  //  if (m_data.isUnit()) {
  //    return conjugate();
  //  } else {
  Attrib1f norm = SH::dot(m_data, m_data); 
  return conjugate() * (1.0 / norm);
  //  }
}

template<BindingType B, typename T>
Matrix<4, 4, SH_TEMP, T> Quaternion<B, T>::getMatrix() const
{
  SH::Matrix4x4f m;
  Attrib4f x = m_data(1,1,1,1) * m_data(1,2,3,0);
  Attrib4f y = m_data(2,2,2,2) * m_data(0,2,3,0);
  Attrib4f z = m_data(3,3,3,3) * m_data(0,0,3,0);

  m[0](0) = 1 - 2 * (y(1) + z(2));
  m[1](0) = 2 * (x(1) - z(3));
  m[2](0) = 2 * (x(2) + y(3));

  m[0](1) = 2 * (x(2) + z(3));
  m[1](1) = 1 - 2 * (x(0) + z(2));
  m[2](1) = 2 * (y(2) - x(3));

  m[0](2) = 2 * (x(2) - y(3));
  m[1](2) = 2 * (y(2) + x(3));
  m[2](2) = 1 - 2 * (x(0) + y(1));

  return m;
}

template<BindingType B, typename T>
Attrib<4, SH_TEMP, T, SH_VECTOR> Quaternion<B, T>::getVector() const
{
  return m_data;
}

template<BindingType B, typename T>
template<BindingType B2>
Quaternion<SH_TEMP, T> 
Quaternion<B, T>::operator+(const Quaternion<B2, T>& q)
{
  Quaternion<B, T> r = *this;
  return (r += q);
}

template<BindingType B, typename T>
template<BindingType B2>
Quaternion<SH_TEMP, T> 
Quaternion<B, T>::operator-(const Quaternion<B2, T>& q)
{
  Quaternion<B, T> r = *this;
  return (r -= q);
}
  
template<BindingType B, typename T>
template<BindingType B2>
Quaternion<SH_TEMP, T> 
Quaternion<B, T>::operator*(const Quaternion<B2, T>& q)
{
  Quaternion<B, T> r = *this;
  return (r *= q);
}

template<BindingType B, typename T>
template<BindingType B2>
Quaternion<SH_TEMP, T> 
Quaternion<B, T>::operator*(const Attrib<1, B2, T>& c)
{
  Quaternion<B, T> r = *this;
  return (r *= c);
}

template<BindingType B, typename T>
template<BindingType B2>
Quaternion<SH_TEMP, T> 
Quaternion<B, T>::operator*(const Attrib<3, B2, T, SH_VECTOR>& v)
{
  Quaternion<B, T> r = *this;
  return (r *= v);
}

template<BindingType B, typename T>
template<BindingType B2>
Quaternion<SH_TEMP, T> 
Quaternion<B, T>::operator*(const Attrib<3, B2, T, SH_NORMAL>& v)
{
  Quaternion<B, T> r = *this;
  return (r *= v);
}

template<BindingType B, typename T>
void Quaternion<B, T>::normalize()
{
  m_data = SH::normalize(m_data);
}

template<BindingType B, typename T>
void Quaternion<B, T>::setUnit(bool flag)
{
  //m_data.setUnit(flag);
}

template<BindingType B, typename T>
void Quaternion<B, T>::getValues(HostType values[]) const
{
  m_data.getValues(values);
}

template<BindingType B, typename T, BindingType B2>
Quaternion<SH_TEMP, T> 
operator*(const Attrib<1, B2, T>& c, const Quaternion<B, T>& q)
{
  Quaternion<B, T> r = q;
  return (r *= c);
}

template<BindingType B1, BindingType B2, typename T>
extern Quaternion<SH_TEMP, T>
slerp(const Quaternion<B1, T>& q1, const Quaternion<B2, T>& q2, const Attrib1f& t)
{
  //TODO::q1 and q2 must be unit quaternions, we cannot call normalize here
  //since it's not a const function.
  //TODO: when cosTheta is 1 or -1, we need to fallback to linear interpolation
  //not sure how to implement this efficiently yet
  Attrib<1, SH_TEMP, T> cosTheta = q1.dot(q2);
  Attrib<1, SH_TEMP, T> sinTheta = sqrt(1.0 - cosTheta*cosTheta);
  
  Quaternion<B2, T> q2prime = (cosTheta >= 0.0)*q2 - (cosTheta < 0.0)*q2;
  Attrib<1, SH_TEMP, T> theta = asin(sinTheta);

  return (sin((1.0 - t)*theta)/sinTheta)*q1 + (sin(t*theta)/sinTheta)*q2prime;
}

template<BindingType B, typename T>
std::string Quaternion<B, T>::name() const
{
  return m_data.name();
}

template<BindingType B, typename T>
void Quaternion<B, T>::name(const std::string& name)
{
  m_data.name(name);
}


}
