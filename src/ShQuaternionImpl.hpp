#include "ShQuaternion.hpp"

namespace SH {
template<int K, typename T>
ShQuaternion<K, T>::ShQuaternion() 
{
  if (K == SH_TEMP) 
  {
    m_data = ShVector4f(1.0, 0.0, 0.0, 0.0);
    m_data.setUnit(true);
  }
}

template<int K, typename T>
template<int K2>
ShQuaternion<K, T>::ShQuaternion(const ShQuaternion<K2, T>& other)
  : m_data(other.getVector())
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
  m_data(1,2,3) = SH::normalize(axis);
  m_data(1,2,3) *= sin(angle/2.0);
  m_data.setUnit(true);
}

template<int K, typename T>
template<int K2>
ShQuaternion<K, T>::ShQuaternion(const ShMatrix<4, 4, K2, T>& mat)
{
  ShAttrib1f trace = 1.0 + mat[0](0) + mat[1](1) + mat[2](2);
  trace = (trace >= 0.0)*trace + (trace < 0.0)*0.0;
  ShAttrib1f c0 = (trace > 0.001);
  ShAttrib1f c1 = ((mat[0](0) > mat[1](1))*(mat[0](0) > mat[2](2)));
  ShAttrib1f c2 = ( mat[1](1) > mat[2](2) );
  ShVector4f res0, res1, res2, res3;
  ShAttrib1f S0 = sqrt(trace) * 2.0;
  S0 += (S0 == 0.0)*1.0;
  //float val;
  //S0.getValues(&val);
  //std::cout << "S0" << std::endl;
  //std::cout << val << std::endl;
  res0(0) = 0.25 * S0;
  res0(1) = (mat[1](2) - mat[2](1)) / S0;
  res0(2) = (mat[2](0) - mat[0](2)) / S0;
  res0(3) = (mat[0](1) - mat[1](0)) / S0;
  //std::cout << res0 << std::endl;
  
  trace = 1.0 + mat[0](0) - mat[1](1) - mat[2](2);
  trace = (trace >= 0.0)*trace + (trace < 0.0)*0.0;
  ShAttrib1f S1 = sqrt(trace) * 2.0;
  S1 += (S1 == 0.0)*1.0;
  //S1.getValues(&val);
  //std::cout << "S1" << std::endl;
  //std::cout << val << std::endl;
  res1(0) = (mat[2](1) - mat[1](2)) / S1;
  res1(1) = 0.25 * S1;
  res1(2) = (mat[0](1) + mat[1](0)) / S1;
  res1(3) = (mat[2](0) + mat[0](2)) / S1;
  //std::cout << res1 << std::endl;
  
  trace = 1.0 - mat[0](0) + mat[1](1) - mat[2](2);
  trace = (trace >= 0.0)*trace + (trace < 0.0)*0.0;
  ShAttrib1f S2 = sqrt(trace) * 2.0;
  S2 += (S2 == 0.0)*1.0;
  //S2.getValues(&val);
  //std::cout << "S2" << std::endl;
  //std::cout << val << std::endl;
  res2(0) = (mat[2](0) - mat[0](2)) / S2;
  res2(1) = (mat[0](1) + mat[1](0)) / S2;
  res2(2) = 0.25 * S2;
  res2(3) = (mat[1](2) + mat[2](1)) / S2;
  //std::cout << res2 << std::endl;
  
  trace = 1.0 - mat[0](0) - mat[1](1) + mat[2](2);
  trace = (trace >= 0.0)*trace + (trace < 0.0)*0.0;
  ShAttrib1f S3 = sqrt(trace) * 2.0;
  S3 += (S3 == 0.0)*1.0;
  //S3.getValues(&val);
  //std::cout << "S3" << std::endl;
  //std::cout << val << std::endl;
  res3(0) = (mat[1](0) - mat[0](1)) / S3;
  res3(1) = (mat[2](0) + mat[0](2)) / S3;
  res3(2) = (mat[1](2) + mat[2](1)) / S3;
  res3(3) = 0.25 * S3;
  //std::cout << res3 << std::endl;
  m_data = c0*res0 + 
    (c0 == 0.0)*(c1*res1 + (c1 == 0.0)*(c2*res2 + (c2 == 0.0)*res3));
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
  m_data = other.getVector();
  return *this;
}

template<int K, typename T>
template<int K2>
ShQuaternion<K, T>& 
ShQuaternion<K, T>::operator+=(const ShQuaternion<K2, T>& right) 
{
  m_data += right.getVector();
  return *this;
}

template<int K, typename T>
template<int K2>
ShQuaternion<K, T>& 
ShQuaternion<K, T>::operator-=(const ShQuaternion<K2, T>& right) 
{
  m_data -= right.getVector();
  return *this;
}

template<int K, typename T>
template<int K2>
ShQuaternion<K, T>& 
ShQuaternion<K, T>::operator*=(const ShQuaternion<K2, T>& right) 
{
  ShVector4f result;
  ShVector4f rightData = right.getVector();
  result(0) = 
    m_data(0)*rightData(0) - SH::dot(m_data(1,2,3), rightData(1,2,3));
  result(1,2,3) = 
    m_data(0)*rightData(1,2,3) + rightData(0)*m_data(1,2,3) + 
    cross(m_data(1,2,3), rightData(1,2,3));

  result.setUnit(m_data.isUnit() && rightData.isUnit());
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
template<int K2>
ShAttrib<1, SH_TEMP, T> 
ShQuaternion<K, T>::dot(const ShQuaternion<K2, T>& q) const 
{
  return SH::dot(m_data, q.getVector());
}

template<int K, typename T>
ShQuaternion<SH_TEMP, T> ShQuaternion<K, T>::conjugate() const 
{
  ShVector4f conjData;
  conjData(0) = m_data(0);
  conjData(1, 2, 3) = -m_data(1, 2, 3);
  conjData.setUnit(m_data.isUnit());

  return ShQuaternion<SH_TEMP>(conjData);
}

template<int K, typename T>
ShQuaternion<SH_TEMP, T> ShQuaternion<K, T>::inverse() const 
{
  if (m_data.isUnit()) {
    return conjugate();
  } else {
    ShAttrib1f norm = SH::dot(m_data, m_data); 
    return conjugate() * (1.0 / norm);
  }
}

template<int K, typename T>
ShMatrix<4, 4, SH_TEMP, T> ShQuaternion<K, T>::getMatrix() const
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
ShVector<4, SH_TEMP, T> ShQuaternion<K, T>::getVector() const
{
  return m_data;
}

template<int K, typename T>
template<int K2>
ShQuaternion<SH_TEMP, T> 
ShQuaternion<K, T>::operator+(const ShQuaternion<K2, T>& q)
{
  ShQuaternion<K, T> r = *this;
  return (r += q);
}

template<int K, typename T>
template<int K2>
ShQuaternion<SH_TEMP, T> 
ShQuaternion<K, T>::operator-(const ShQuaternion<K2, T>& q)
{
  ShQuaternion<K, T> r = *this;
  return (r -= q);
}
  
template<int K, typename T>
template<int K2>
ShQuaternion<SH_TEMP, T> 
ShQuaternion<K, T>::operator*(const ShQuaternion<K2, T>& q)
{
  ShQuaternion<K, T> r = *this;
  return (r *= q);
}

template<int K, typename T>
template<int K2>
ShQuaternion<SH_TEMP, T> 
ShQuaternion<K, T>::operator*(const ShAttrib<1, K2, T>& c)
{
  ShQuaternion<K, T> r = *this;
  return (r *= c);
}

template<int K, typename T>
void ShQuaternion<K, T>::normalize()
{
	m_data = normalize(m_data);
}

template<int K, typename T>
void ShQuaternion<K, T>::setUnit(bool flag)
{
	m_data.setUnit(flag);
}

template<int K, typename T>
void ShQuaternion<K, T>::getValues(T values[]) const
{
	m_data.getValues(values);
}

template<int K, typename T, int K2>
ShQuaternion<SH_TEMP, T> 
operator*(const ShAttrib<1, K2, T>& c, const ShQuaternion<K, T>& q)
{
  ShQuaternion<K, T> r = q;
  return (r *= c);
}

template<int K1, int K2, typename T>
extern ShQuaternion<SH_TEMP, T>
slerp(const ShQuaternion<K1, T>& q1, const ShQuaternion<K2, T>& q2, 
		const ShAttrib1f& t)
{
  //TODO::q1 and q2 must be unit quaternions, we cannot call normalize here
  //since it's not a const function.
  //TODO: when cosTheta is 1 or -1, we need to fallback to linear interpolation
  //not sure how to implement this efficiently yet
  ShAttrib<1, SH_TEMP, T> cosTheta = q1.dot(q2);
  ShAttrib<1, SH_TEMP, T> sinTheta = sqrt(1.0 - cosTheta*cosTheta);
  
  ShQuaternion<K2, T> q2prime = (cosTheta >= 0.0)*q2 - (cosTheta < 0.0)*q2;
  ShAttrib<1, SH_TEMP, T> theta = asin(sinTheta);

  return (sin((1.0 - t)*theta)/sinTheta)*q1 + (sin(t*theta)/sinTheta)*q2prime;
}

template<int K, typename T>
std::string ShQuaternion<K, T>::name() const
{
    return m_data.name();
}

template<int K, typename T>
void ShQuaternion<K, T>::name(const std::string& name)
{
    m_data.name(name);
}


}
