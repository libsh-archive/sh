#ifndef SHMATRIXIMPL_HPP
#define SHMATRIXIMPL_HPP

#include "ShMatrix.hpp"
#include "ShUtility.hpp"

namespace SH {

template<int Rows, int Cols, int Kind, typename T>
ShMatrix<Rows, Cols, Kind, T>::ShMatrix()
{
}

template<int Rows, int Cols, int Kind, typename T>
ShMatrix<Rows, Cols, Kind, T>::ShMatrix(const ShMatrix<Rows, Cols, Kind, T>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] = other[i];
}

template<int Rows, int Cols, int Kind, typename T>
template<int Kind2>
ShMatrix<Rows, Cols, Kind, T>::ShMatrix(const ShMatrix<Rows, Cols, Kind2, T>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] = other[i];
}

template<int Rows, int Cols, int Kind, typename T>
ShMatrix<Rows, Cols, Kind, T>::~ShMatrix()
{
}

template<int Rows, int Cols, int Kind, typename T>
ShMatrix<Rows, Cols, Kind, T>&
ShMatrix<Rows, Cols, Kind, T>::operator=(const ShMatrix<Rows, Cols, Kind, T>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] = other[i];
  return *this;
}

template<int Rows, int Cols, int Kind, typename T>
template<int Kind2>
ShMatrix<Rows, Cols, Kind, T>&
ShMatrix<Rows, Cols, Kind, T>::operator=(const ShMatrix<Rows, Cols, Kind2, T>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] = other[i];
  return *this;
}

template<int Rows, int Cols, int Kind, typename T>
ShAttrib<Cols, Kind, T>& ShMatrix<Rows, Cols, Kind, T>::operator[](int i)
{
  return m_data[i];
}

template<int Rows, int Cols, int Kind, typename T>
const ShAttrib<Cols, Kind, T>& ShMatrix<Rows, Cols, Kind, T>::operator[](int i) const
{
  return m_data[i];
}

template<int Rows, int Cols, int Kind, typename T>
template<int Kind2>
ShMatrix<Rows, Cols, Kind, T>&
ShMatrix<Rows, Cols, Kind, T>::operator+=(const ShMatrix<Rows, Cols, Kind2, T>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] += other[i];
  return *this;
}

template<int Rows, int Cols, int Kind, typename T>
template<int Kind2>
ShMatrix<Rows, Cols, Kind, T>&
ShMatrix<Rows, Cols, Kind, T>::operator-=(const ShMatrix<Rows, Cols, Kind2, T>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] -= other[i];
  return *this;
}

template<int Rows, int Cols, int Kind, typename T>
template<int Kind2>
ShMatrix<Rows, Cols, Kind, T>&
ShMatrix<Rows, Cols, Kind, T>::operator*=(const ShMatrix<Rows, Cols, Kind2, T>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] *= other[i];
  return *this;
}

template<int Rows, int Cols, int Kind, typename T>
template<int Kind2>
ShMatrix<Rows, Cols, Kind, T>&
ShMatrix<Rows, Cols, Kind, T>::operator/=(const ShMatrix<Rows, Cols, Kind2, T>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] /= other[i];
  return *this;
}

template<int Rows, int Cols, int Kind, typename T>
ShMatrix<Rows, Cols, Kind, T>&
ShMatrix<Rows, Cols, Kind, T>::operator*=(const ShVariableN<1, T>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] *= other;
  return *this;
}

template<int Rows, int Cols, int Kind, typename T>
ShMatrix<Rows, Cols, Kind, T>&
ShMatrix<Rows, Cols, Kind, T>::operator/=(const ShVariableN<1, T>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] /= other;
  return *this;
}

template<int Rows, int Cols, int Kind, typename T>
ShMatrixRows<Rows, Cols, T>
ShMatrix<Rows, Cols, Kind, T>::operator()() const
{
  return ShMatrixRows<Rows, Cols, T>(*this);
}

template<int Rows, int Cols, int Kind, typename T>
ShMatrixRows<1, Cols, T>
ShMatrix<Rows, Cols, Kind, T>::operator()(int i0) const
{
  return ShMatrixRows<1, Cols, T>(*this, i0);
}

template<int Rows, int Cols, int Kind, typename T>
ShMatrixRows<2, Cols, T>
ShMatrix<Rows, Cols, Kind, T>::operator()(int i0, int i1) const
{
  return ShMatrixRows<2, Cols, T>(*this, i0, i1);
}

template<int Rows, int Cols, int Kind, typename T>
ShMatrixRows<3, Cols, T>
ShMatrix<Rows, Cols, Kind, T>::operator()(int i0, int i1, int i2) const
{
  return ShMatrixRows<3, Cols, T>(*this, i0, i1, i2);
}

template<int Rows, int Cols, int Kind, typename T>
ShMatrixRows<4, Cols, T>
ShMatrix<Rows, Cols, Kind, T>::operator()(int i0, int i1, int i2, int i3) const
{
  return ShMatrixRows<4, Cols, T>(*this, i0, i1, i2, i3);
}

////////////////////////
// ShMatrixRows stuff //
////////////////////////

template<int Rows, int Cols, typename T>
template<int Kind>
ShMatrixRows<Rows, Cols, T>::ShMatrixRows(const ShMatrix<Rows, Cols, Kind, T>& source)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] = source[i];
}

template<int Rows, int Cols, typename T>
template<int OR, int Kind>
ShMatrixRows<Rows, Cols, T>::ShMatrixRows(const ShMatrix<OR, Cols, Kind, T>& source,
                                                int i0)
{
  ShIntEqual<Rows, 1>();
  
  m_data[0] = source[i0];
}

template<int Rows, int Cols, typename T>
template<int OR, int Kind>
ShMatrixRows<Rows, Cols, T>::ShMatrixRows(const ShMatrix<OR, Cols, Kind, T>& source,
                                                int i0, int i1)
{
  ShIntEqual<Rows, 2>();
  
  m_data[0] = source[i0];
  m_data[1] = source[i1];
}

template<int Rows, int Cols, typename T>
template<int OR, int Kind>
ShMatrixRows<Rows, Cols, T>::ShMatrixRows(const ShMatrix<OR, Cols, Kind, T>& source,
                                                int i0, int i1, int i2)
{
  ShIntEqual<Rows, 3>();
  
  m_data[0] = source[i0];
  m_data[1] = source[i1];
  m_data[2] = source[i2];
}

template<int Rows, int Cols, typename T>
template<int OR, int Kind>
ShMatrixRows<Rows, Cols, T>::ShMatrixRows(const ShMatrix<OR, Cols, Kind, T>& source,
                                                int i0, int i1, int i2, int i3)
{
  ShIntEqual<Rows, 4>();
  
  m_data[0] = source[i0];
  m_data[1] = source[i1];
  m_data[2] = source[i2];
  m_data[3] = source[i3];
}

template<int Rows, int Cols, typename T>
ShMatrixRows<Rows, Cols, T>::ShMatrixRows(const ShMatrixRows<Rows, Cols, T>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] = other.m_data[i];
}

template<int Rows, int Cols, typename T>
ShMatrixRows<Rows, Cols, T>&
ShMatrixRows<Rows, Cols, T>::operator=(const ShMatrixRows<Rows, Cols, T>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] = other.m_data[i];
}

template<int Rows, int Cols, typename T>
ShMatrix<Rows, Cols, SH_VAR_TEMP, T>
ShMatrixRows<Rows, Cols, T>::operator()() const
{
  ShMatrix<Rows, Cols, SH_VAR_TEMP, T> r;
  for (int i = 0; i < Rows; i++) r[i] = m_data[i];
  return r;
}

template<int Rows, int Cols, typename T>
ShMatrix<Rows, 1, SH_VAR_TEMP, T>
ShMatrixRows<Rows, Cols, T>::operator()(int i0) const
{
  ShMatrix<Rows, 1, SH_VAR_TEMP, T> r;
  for (int i = 0; i < Rows; i++) r[i] = m_data[i](i0);
  return r;
}

template<int Rows, int Cols, typename T>
ShMatrix<Rows, 2, SH_VAR_TEMP, T>
ShMatrixRows<Rows, Cols, T>::operator()(int i0, int i1) const
{
  ShMatrix<Rows, 2, SH_VAR_TEMP, T> r;
  for (int i = 0; i < Rows; i++) r[i] = m_data[i](i0, i1);
  return r;
}

template<int Rows, int Cols, typename T>
ShMatrix<Rows, 3, SH_VAR_TEMP, T>
ShMatrixRows<Rows, Cols, T>::operator()(int i0, int i1, int i2) const
{
  ShMatrix<Rows, 3, SH_VAR_TEMP, T> r;
  for (int i = 0; i < Rows; i++) r[i] = m_data[i](i0, i1, i2);
  return r;
}

template<int Rows, int Cols, typename T>
ShMatrix<Rows, 4, SH_VAR_TEMP, T>
ShMatrixRows<Rows, Cols, T>::operator()(int i0, int i1, int i2, int i3) const
{
  ShMatrix<Rows, 4, SH_VAR_TEMP, T> r;
  for (int i = 0; i < Rows; i++) r[i] = m_data[i](i0, i1, i2, i3);
  return r;
}


}

#endif
