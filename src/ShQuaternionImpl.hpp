#include "ShQuaternion.hpp"

namespace SH {
template<int K, typename T>
ShQuaternion<K, T>::ShQuaternion() 
  : m_data(1.0,0.0,0.0,0.0)
{
  m_data.setUnit(true);
}

template<int K, typename T>
template<int K2>
ShQuaternion<K, T>::ShQuaternion(const ShQuaternion<K2, T>& other)
  : m_data(other.m_data)
{
}

template<int K, typename T>
template<int K2>
ShQuaternion<K, T>::ShQuaternion(const ShVector<4, K2, T>& values)
  : m_data(values)
{
}

template<int K, typename T>
template<int K2, int K3>   
ShQuaternion<K, T>::ShQuaternion(const ShAttrib<1, K2, T>& angle, 
                                 const ShVector<3, K3, T>& axis)
{
  m_data(0) = cos(angle/2.0);
  m_data(1,2,3) = normalize(axis);
  m_data(1,2,3) *= sin(angle/2.0);
  m_data.setUnit(true);
}

template<int K, typename T>
std::ostream& operator<<(std::ostream& out, const ShQuaternion<K, T>& q)
{
  float vals[4];
  q.m_data.getValues(vals);
  out << "ShQuaternion: " << vals[0] << " " << vals[1] << " " << vals[2] 
      << " " << vals[3];
  return out;
}

template<int K, typename T>
template<int K2>
ShQuaternion<K, T>& 
ShQuaternion<K, T>::operator=(const ShQuaternion<K2, T>& other) 
{
  m_data = other.m_data;
  return *this;
}

template<int K, typename T>
template<int K2>
ShQuaternion<K, T>& 
ShQuaternion<K, T>::operator+=(const ShQuaternion<K2, T>& right) 
{
  m_data += right.m_data;
  return *this;
}

template<int K, typename T>
template<int K2>
ShQuaternion<K, T>& 
ShQuaternion<K, T>::operator-=(const ShQuaternion<K2, T>& right) 
{
  m_data -= right.m_data;
  return *this;
}

template<int K, typename T>
template<int K2>
ShQuaternion<K, T>& 
ShQuaternion<K, T>::operator*=(const ShQuaternion<K2, T>& right) 
{
  ShVector4f result;
  result(0) = 
    m_data(0)*right.m_data(0) - dot(m_data(1,2,3), right.m_data(1,2,3));
  result(1,2,3) = 
    m_data(0)*right.m_data(1,2,3) + right.m_data(0)*m_data(1,2,3) + 
    cross(m_data(1,2,3), right.m_data(1,2,3));

  result.setUnit(m_data.isUnit() && right.m_data.isUnit());
  m_data = result;
  return *this;
}

template<int K, typename T>
template<int K2>
ShQuaternion<K, T>& 
ShQuaternion<K, T>::operator*=(const ShAttrib<1, K2, T>& right) 
{
  m_data = m_data*right;
  return *this;
}

template<int K, typename T>
ShQuaternion<SH_VAR_TEMP, T> ShQuaternion<K, T>::conjugate() const 
{
  ShVector4f conjData = -m_data;
  conjData(0) = -conjData(0);
  conjData.setUnit(m_data.isUnit());

  return ShQuaternion(conjData);
}

template<int K, typename T>
ShQuaternion<SH_VAR_TEMP, T> ShQuaternion<K, T>::inverse() const 
{
  if (m_data.isUnit()) {
    return conjugate();
  } else {
    ShAttrib1f norm = dot(m_data, m_data); 
    return conjugate() * (1.0 / norm);
  }
}

template<int K, typename T>
ShMatrix<4, 4, SH_VAR_TEMP, T> ShQuaternion<K, T>::getMatrix() const
{
  SH::ShMatrix4x4f m;
  ShAttrib1f xx, xy, xz, xw, yy, yz, yw, zz, zw;

  xx = m_data(1)*m_data(1);
  xy = m_data(1)*m_data(2);
  xz = m_data(1)*m_data(3);
  xw = m_data(1)*m_data(0);

  yy = m_data(2)*m_data(2);
  yz = m_data(2)*m_data(3);
  yw = m_data(2)*m_data(0);

  zz = m_data(3)*m_data(3);
  zw = m_data(3)*m_data(0);

  m[0](0) = 1 - 2 * (yy + zz);
  m[1](0) = 2 * (xy - zw);
  m[2](0) = 2 * (xz + yw);

  m[0](1) = 2 * (xy + zw);
  m[1](1) = 1 - 2 * (xx + zz);
  m[2](1) = 2 * (yz - xw);

  m[0](2) = 2 * (xz - yw);
  m[1](2) = 2 * (yz + xw);
  m[2](2) = 1 - 2 * (xx + yy);

  m[3](0) = m[3](1) = m[3](2) = m[0](3) = m[1](3) = m[2](3) = 0;
  m[3](3) = 1;

  return m;
}

template<int K, typename T>
template<int K2>
ShQuaternion<SH_VAR_TEMP, T> 
ShQuaternion<K, T>::operator+(const ShQuaternion<K2, T>& q)
{
  ShQuaternion<K, T> r = *this;
  return (r += q);
}

template<int K, typename T>
template<int K2>
ShQuaternion<SH_VAR_TEMP, T> 
ShQuaternion<K, T>::operator-(const ShQuaternion<K2, T>& q)
{
  ShQuaternion<K, T> r = *this;
  return (r -= q);
}
  
template<int K, typename T>
template<int K2>
ShQuaternion<SH_VAR_TEMP, T> 
ShQuaternion<K, T>::operator*(const ShQuaternion<K2, T>& q)
{
  ShQuaternion<K, T> r = *this;
  return (r *= q);
}

template<int K, typename T>
template<int K2>
ShQuaternion<SH_VAR_TEMP, T> 
ShQuaternion<K, T>::operator*(const ShAttrib<1, K2, T>& c)
{
  ShQuaternion<K, T> r = *this;
  return (r *= c);
}

template<int K, typename T, int K2>
ShQuaternion<SH_VAR_TEMP, T> 
operator*(const ShAttrib<1, K2, T>& c, const ShQuaternion<K, T>& q)
{
  ShQuaternion<K, T> r = q;
  return (r *= c);
}

}
