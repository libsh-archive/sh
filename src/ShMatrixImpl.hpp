// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Viberiu S. Popa,
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
#ifndef SHMATRIXIMPL_HPP
#define SHMATRIXIMPL_HPP

#include <iostream>
#include <algorithm>
#include "ShMatrix.hpp"
#include "ShUtility.hpp"

namespace SH {

//Constructors, destructors
template<int Rows, int Cols, ShBindingType Binding, ShValueType V>
ShMatrix<Rows, Cols, Binding, V>::ShMatrix()
{
  for (int i = 0; i < std::min(Rows, Cols); i++)
    m_data[i][i] = 1.0;
}

template<int Rows, int Cols, ShBindingType Binding, ShValueType V>
ShMatrix<Rows, Cols, Binding, V>::ShMatrix(const ShMatrix<Rows, Cols, Binding, V>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] = other[i];
}

template<int Rows, int Cols, ShBindingType Binding, ShValueType V>
template<ShBindingType Binding2>
ShMatrix<Rows, Cols, Binding, V>::ShMatrix(const ShMatrix<Rows, Cols, Binding2, V>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] = other[i];
}

template<int Rows, int Cols, ShBindingType Binding, ShValueType V>
ShMatrix<Rows, Cols, Binding, V>::~ShMatrix()
{
}

//Operators
template<int Rows, int Cols, ShBindingType Binding, ShValueType V>
ShMatrix<Rows, Cols, Binding, V>&
ShMatrix<Rows, Cols, Binding, V>::operator=(const ShMatrix<Rows, Cols, Binding, V>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] = other[i];
  return *this;
}

template<int Rows, int Cols, ShBindingType Binding, ShValueType V>
template<ShBindingType Binding2>
ShMatrix<Rows, Cols, Binding, V>&
ShMatrix<Rows, Cols, Binding, V>::operator=(const ShMatrix<Rows, Cols, Binding2, V>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] = other[i];
  return *this;
}

template<int Rows, int Cols, ShBindingType Binding, ShValueType V>
ShAttrib<Cols, Binding, V>& ShMatrix<Rows, Cols, Binding, V>::operator[](int i)
{
  return m_data[i];
}

template<int Rows, int Cols, ShBindingType Binding, ShValueType V>
const ShAttrib<Cols, Binding, V>& ShMatrix<Rows, Cols, Binding, V>::operator[](int i) const
{
  return m_data[i];
}

template<int Rows, int Cols, ShBindingType Binding, ShValueType V>
template<ShBindingType Binding2>
ShMatrix<Rows, Cols, Binding, V>&
ShMatrix<Rows, Cols, Binding, V>::operator+=(const ShMatrix<Rows, Cols, Binding2, V>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] += other[i];
  return *this;
}

template<int Rows, int Cols, ShBindingType Binding, ShValueType V>
template<ShBindingType Binding2>
ShMatrix<Rows, Cols, Binding, V>&
ShMatrix<Rows, Cols, Binding, V>::operator-=(const ShMatrix<Rows, Cols, Binding2, V>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] -= other[i];
  return *this;
}

template<int Rows, int Cols, ShBindingType Binding, ShValueType V>
template<ShBindingType Binding2>
ShMatrix<Rows, Cols, Binding, V>&
ShMatrix<Rows, Cols, Binding, V>::operator/=(const ShMatrix<Rows, Cols, Binding2, V>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] /= other[i];
  return *this;
}

template<int Rows, int Cols, ShBindingType Binding, ShValueType V>
ShMatrix<Rows, Cols, Binding, V>&
ShMatrix<Rows, Cols, Binding, V>::operator*=(const ShGeneric<1, V>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] *= other;
  return *this;
}
  
template<int Rows, int Cols, ShBindingType Binding, ShValueType V>
ShMatrix<Rows, Cols, Binding, V>&
ShMatrix<Rows, Cols, Binding, V>::operator/=(const ShGeneric<1, V>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] /= other;
  return *this;
}

template<int Rows, int Cols, ShBindingType Binding, ShValueType V>
ShMatrixRows<Rows, Cols, V>
ShMatrix<Rows, Cols, Binding, V>::operator()() const
{
  return ShMatrixRows<Rows, Cols, V>(*this);
}

template<int Rows, int Cols, ShBindingType Binding, ShValueType V>
ShMatrixRows<1, Cols, V>
ShMatrix<Rows, Cols, Binding, V>::operator()(int i0) const
{
  return ShMatrixRows<1, Cols, V>(*this, i0);
}

template<int Rows, int Cols, ShBindingType Binding, ShValueType V>
ShMatrixRows<2, Cols, V>
ShMatrix<Rows, Cols, Binding, V>::operator()(int i0, int i1) const
{
  return ShMatrixRows<2, Cols, V>(*this, i0, i1);
}

template<int Rows, int Cols, ShBindingType Binding, ShValueType V>
ShMatrixRows<3, Cols, V>
ShMatrix<Rows, Cols, Binding, V>::operator()(int i0, int i1, int i2) const
{
  return ShMatrixRows<3, Cols, V>(*this, i0, i1, i2);
}

template<int Rows, int Cols, ShBindingType Binding, ShValueType V>
ShMatrixRows<4, Cols, V>
ShMatrix<Rows, Cols, Binding, V>::operator()(int i0, int i1, int i2, int i3) const
{
  return ShMatrixRows<4, Cols, V>(*this, i0, i1, i2, i3);
}

template<int Rows, int Cols, ShBindingType Binding, ShValueType V>
std::ostream& operator<<(std::ostream& out,
                         const ShMatrix<Rows, Cols, Binding, V>& m)
{
  for (int k = 0; k < Rows; k++) {   
    out << m[k];
    out << std::endl;
  }
    
  return out;
}
  
  
template<int Rows, int Cols, ShBindingType Binding, ShValueType V>
ShMatrix<Rows - 1, Cols -1, SH_TEMP, V>
ShMatrix<Rows, Cols, Binding, V>::subMatrix(int rowToRemove,
                                            int columnToRemove) const
{
  ShMatrix<Rows - 1, Cols - 1, SH_TEMP, V> myMatrix;
    
  int indices[Cols - 1];
  for (int i = 0; i < columnToRemove; i++) 
    indices[i] = i;
  for (int i = columnToRemove + 1; i < Cols; i++) 
    indices[i - 1] = i;
    
  for (int i = 0; i < rowToRemove; i++) {
    myMatrix[i].clone(m_data[i].template swiz<Cols - 1>(indices));
  }
    
  for (int i = rowToRemove + 1; i < Rows; i++) {
    myMatrix[i - 1].clone(m_data[i].template swiz<Cols - 1>(indices));
  }
    
  return myMatrix;
}

template<int Rows,int Cols,ShBindingType Binding, ShValueType V>
void ShMatrix<Rows, Cols, Binding, V>::setTranslation(const ShGeneric<Rows-1, V>& trans){
							
  for(int i = 0;i<(Rows-1);i++)
    m_data[i][(Cols-1)] = trans[i];
}
  
  
template<int Rows,int Cols,ShBindingType Binding, ShValueType V>
void ShMatrix<Rows, Cols, Binding, V>::setScaling(const ShGeneric<Rows-1, V>& scale){
  for(int i = 0;i<(Rows-1);i++)
    m_data[i][i] = scale[i];
}

////////////////////////
// ShMatrixRows stuff //
////////////////////////


template<int Rows, int Cols, ShValueType V>
template<int OR, ShBindingType Binding>
ShMatrixRows<Rows, Cols, V>::ShMatrixRows(const ShMatrix<OR, Cols, Binding, V>& source,
                                          int i0)
{
  SH_STATIC_CHECK(Rows == 1, Constructing_Non_1_Row_Matrix_From_1_Row);
  
  m_data[0].clone(source[i0]);
}

template<int Rows, int Cols, ShValueType V>
template<int OR, ShBindingType Binding>
ShMatrixRows<Rows, Cols, V>::ShMatrixRows(const ShMatrix<OR, Cols, Binding, V>& source,
                                          int i0, int i1)
{
  SH_STATIC_CHECK(Rows == 2, Constructing_Non_2_Row_Matrix_From_2_Rows);
  
  m_data[0].clone(source[i0]);
  m_data[1].clone(source[i1]);
}

template<int Rows, int Cols, ShValueType V>
template<int OR, ShBindingType Binding>
ShMatrixRows<Rows, Cols, V>::ShMatrixRows(const ShMatrix<OR, Cols, Binding, V>& source,
                                          int i0, int i1, int i2)
{
  SH_STATIC_CHECK(Rows == 3, Constructing_Non_3_Row_Matrix_From_3_Rows);
  
  m_data[0].clone(source[i0]);
  m_data[1].clone(source[i1]);
  m_data[2].clone(source[i2]);
}

template<int Rows, int Cols, ShValueType V>
template<int OR, ShBindingType Binding>
ShMatrixRows<Rows, Cols, V>::ShMatrixRows(const ShMatrix<OR, Cols, Binding, V>& source,
                                          int i0, int i1, int i2, int i3)
{
  SH_STATIC_CHECK(Rows == 4, Constructing_Non_4_Row_Matrix_From_4_Rows);
  
  m_data[0].clone(source[i0]);
  m_data[1].clone(source[i1]);
  m_data[2].clone(source[i2]);
  m_data[3].clone(source[i3]);
}

template<int Rows, int Cols, ShValueType V>
ShMatrixRows<Rows, Cols, V>::ShMatrixRows(const ShMatrixRows<Rows, Cols, V>& other)
{
  // TODO: clone?
  for (int i = 0; i < Rows; i++)
    m_data[i] = other.m_data[i];
}

template<int Rows, int Cols, ShValueType V>
ShMatrixRows<Rows, Cols, V>&
ShMatrixRows<Rows, Cols, V>::operator=(const ShMatrixRows<Rows, Cols, V>& other)
{
  // TODO: clone?
  for (int i = 0; i < Rows; i++)
    m_data[i] = other.m_data[i];
}

template<int Rows, int Cols, ShValueType V>
ShMatrix<Rows, Cols, SH_TEMP, V>
ShMatrixRows<Rows, Cols, V>::operator()() const
{
  ShMatrix<Rows, Cols, SH_TEMP, V> r;
  for (int i = 0; i < Rows; i++) r[i].clone(m_data[i]);
  return r;
}

template<int Rows, int Cols, ShValueType V>
ShMatrix<Rows, 1, SH_TEMP, V>
ShMatrixRows<Rows, Cols, V>::operator()(int i0) const
{
  ShMatrix<Rows, 1, SH_TEMP, V> r;
  for (int i = 0; i < Rows; i++) r[i].clone(m_data[i](i0));
  return r;
}

template<int Rows, int Cols, ShValueType V>
ShMatrix<Rows, 2, SH_TEMP, V>
ShMatrixRows<Rows, Cols, V>::operator()(int i0, int i1) const
{
  ShMatrix<Rows, 2, SH_TEMP, V> r;
  for (int i = 0; i < Rows; i++) r[i].clone(m_data[i](i0, i1));
  return r;
}

template<int Rows, int Cols, ShValueType V>
ShMatrix<Rows, 3, SH_TEMP, V>
ShMatrixRows<Rows, Cols, V>::operator()(int i0, int i1, int i2) const
{
  ShMatrix<Rows, 3, SH_TEMP, V> r;
  for (int i = 0; i < Rows; i++) r[i].clone(m_data[i](i0, i1, i2));
  return r;
}

template<int Rows, int Cols, ShValueType V>
ShMatrix<Rows, 4, SH_TEMP, V>
ShMatrixRows<Rows, Cols, V>::operator()(int i0, int i1, int i2, int i3) const
{
  ShMatrix<Rows, 4, SH_TEMP, V> r;
  for (int i = 0; i < Rows; i++) r[i].clone(m_data[i](i0, i1, i2, i3));
  return r;
}

template<int Rows, int Cols, ShBindingType Binding, ShValueType V>
void ShMatrix<Rows, Cols, Binding, V>::name(const std::string& name)
{
  for (int i = 0; i < Rows; i++) {
    std::ostringstream os;
    os << name << "_row" << i;
    m_data[i].name(os.str());
    m_data[i].meta("matrixname", name);
  }
}

template<int Rows, int Cols, ShBindingType Binding, ShValueType V>
std::string ShMatrix<Rows, Cols, Binding, V>::name() const
{
  return m_data[0].meta("matrixname");
}

template<int Rows, int Cols, ShBindingType Binding, ShValueType V>
void ShMatrix<Rows, Cols, Binding, V>::range(H low, H high)
{
  for (int i = 0; i < Rows; i++) m_data[i].range(low, high);
}

template<int Rows, int Cols, ShBindingType Binding, ShValueType V>
void ShMatrix<Rows, Cols, Binding, V>::internal(bool setting)
{
  for (int i = 0; i < Rows; i++) m_data[i].internal(setting);
}


}

#endif
