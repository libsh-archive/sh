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
#ifndef SHMATRIXIMPL_HPP
#define SHMATRIXIMPL_HPP

#include <iostream>
#include "ShMatrix.hpp"
#include "ShUtility.hpp"

namespace SH {

//Constructors, destructors
template<int Rows, int Cols, ShBindingType Binding, typename T>
ShMatrix<Rows, Cols, Binding, T>::ShMatrix()
{
  if (Rows == Cols) {
    for (int i = 0; i < Rows; i++)
      m_data[i][i] = 1.0;
  }
}

template<int Rows, int Cols, ShBindingType Binding, typename T>
ShMatrix<Rows, Cols, Binding, T>::ShMatrix(const ShMatrix<Rows, Cols, Binding, T>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] = other[i];
}

template<int Rows, int Cols, ShBindingType Binding, typename T>
template<ShBindingType Binding2>
ShMatrix<Rows, Cols, Binding, T>::ShMatrix(const ShMatrix<Rows, Cols, Binding2, T>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] = other[i];
}

template<int Rows, int Cols, ShBindingType Binding, typename T>
ShMatrix<Rows, Cols, Binding, T>::~ShMatrix()
{
}

//Operators
template<int Rows, int Cols, ShBindingType Binding, typename T>
ShMatrix<Rows, Cols, Binding, T>&
ShMatrix<Rows, Cols, Binding, T>::operator=(const ShMatrix<Rows, Cols, Binding, T>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] = other[i];
  return *this;
}

template<int Rows, int Cols, ShBindingType Binding, typename T>
template<ShBindingType Binding2>
ShMatrix<Rows, Cols, Binding, T>&
ShMatrix<Rows, Cols, Binding, T>::operator=(const ShMatrix<Rows, Cols, Binding2, T>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] = other[i];
  return *this;
}

template<int Rows, int Cols, ShBindingType Binding, typename T>
ShAttrib<Cols, Binding, T>& ShMatrix<Rows, Cols, Binding, T>::operator[](int i)
{
  return m_data[i];
}

template<int Rows, int Cols, ShBindingType Binding, typename T>
const ShAttrib<Cols, Binding, T>& ShMatrix<Rows, Cols, Binding, T>::operator[](int i) const
{
  return m_data[i];
}

template<int Rows, int Cols, ShBindingType Binding, typename T>
template<ShBindingType Binding2>
ShMatrix<Rows, Cols, Binding, T>&
ShMatrix<Rows, Cols, Binding, T>::operator+=(const ShMatrix<Rows, Cols, Binding2, T>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] += other[i];
  return *this;
}

template<int Rows, int Cols, ShBindingType Binding, typename T>
template<ShBindingType Binding2>
ShMatrix<Rows, Cols, Binding, T>&
ShMatrix<Rows, Cols, Binding, T>::operator-=(const ShMatrix<Rows, Cols, Binding2, T>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] -= other[i];
  return *this;
}

template<int Rows, int Cols, ShBindingType Binding, typename T>
template<ShBindingType Binding2>
ShMatrix<Rows, Cols, Binding, T>&
ShMatrix<Rows, Cols, Binding, T>::operator*=(const ShMatrix<Rows, Cols, Binding2, T>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] *= other[i];
  return *this;
}

template<int Rows, int Cols, ShBindingType Binding, typename T>
template<ShBindingType Binding2>
ShMatrix<Rows, Cols, Binding, T>&
ShMatrix<Rows, Cols, Binding, T>::operator/=(const ShMatrix<Rows, Cols, Binding2, T>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] /= other[i];
  return *this;
}

template<int Rows, int Cols, ShBindingType Binding, typename T>
ShMatrix<Rows, Cols, Binding, T>&
ShMatrix<Rows, Cols, Binding, T>::operator*=(const ShGeneric<1, T>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] *= other;
  return *this;
}
  
template<int Rows, int Cols, ShBindingType Binding, typename T>
ShMatrix<Rows, Cols, Binding, T>&
ShMatrix<Rows, Cols, Binding, T>::operator/=(const ShGeneric<1, T>& other)
{
  for (int i = 0; i < Rows; i++)
    m_data[i] /= other;
  return *this;
}

template<int Rows, int Cols, ShBindingType Binding, typename T>
ShMatrixRows<Rows, Cols, T>
ShMatrix<Rows, Cols, Binding, T>::operator()() const
{
  return ShMatrixRows<Rows, Cols, T>(*this);
}

template<int Rows, int Cols, ShBindingType Binding, typename T>
ShMatrixRows<1, Cols, T>
ShMatrix<Rows, Cols, Binding, T>::operator()(int i0) const
{
  return ShMatrixRows<1, Cols, T>(*this, i0);
}

template<int Rows, int Cols, ShBindingType Binding, typename T>
ShMatrixRows<2, Cols, T>
ShMatrix<Rows, Cols, Binding, T>::operator()(int i0, int i1) const
{
  return ShMatrixRows<2, Cols, T>(*this, i0, i1);
}

template<int Rows, int Cols, ShBindingType Binding, typename T>
ShMatrixRows<3, Cols, T>
ShMatrix<Rows, Cols, Binding, T>::operator()(int i0, int i1, int i2) const
{
  return ShMatrixRows<3, Cols, T>(*this, i0, i1, i2);
}

template<int Rows, int Cols, ShBindingType Binding, typename T>
ShMatrixRows<4, Cols, T>
ShMatrix<Rows, Cols, Binding, T>::operator()(int i0, int i1, int i2, int i3) const
{
  return ShMatrixRows<4, Cols, T>(*this, i0, i1, i2, i3);
}

template<int Rows, int Cols, ShBindingType Binding, typename T>
std::ostream& operator<<(std::ostream& out, const ShMatrix<Rows, Cols, Binding, T>& shMatrixToPrint){
    
  if (!shMatrixToPrint.m_data) {
    out << "[null]";
    return out;
  }
    
  for (int k = 0; k < Rows; k++) {   
    out << shMatrixToPrint.m_data[k];
    out << std::endl;
  }
    
  return out;
}
  
template<ShBindingType Binding, typename T>
ShAttrib1f
det(const ShMatrix<1, 1, Binding, T>& matrix)
{
  return matrix[0][0];
}

  
template<ShBindingType Binding, typename T>
ShAttrib1f
det(const ShMatrix<2, 2, Binding, T>& matrix)
{
  return (matrix[0][0]*matrix[1][1] - matrix[0][1] * matrix[1][0]);
}
 
template<int RowsCols, ShBindingType Binding, typename T>
ShAttrib1f
det(const ShMatrix<RowsCols, RowsCols, Binding, T>& matrix)
{
  ShAttrib1f ret = 0.0;   
  ShAttrib1f dir = RowsCols % 2 ? 1.0 : -1.0;

  for (int i = 0; i < RowsCols; i++) {
    ret += dir * matrix[RowsCols - 1][i] * det(matrix.subMatrix(RowsCols - 1, i));
    dir *= -1.0;
  }
  return ret;
}

//Matrix of Cofactors
template<ShBindingType Binding, typename T>
ShMatrix<1,1,Binding,T>
cofactors(const ShMatrix<1, 1, Binding, T>& matrix){
  return matrix;
}
    
template<ShBindingType Binding, typename T>
ShMatrix<2,2,Binding,T>
cofactors(const ShMatrix<2, 2, Binding, T>& matrix)
{
  ShMatrix<2,2,Binding,T> matrixToReturn;
  matrixToReturn.m_data[0][0]= matrix[1][1];
  matrixToReturn.m_data[1][0]=-matrix[0][1];
  matrixToReturn.m_data[0][1]=-matrix[1][0];
  matrixToReturn.m_data[1][1]= matrix[0][0];
    
  return matrixToReturn;
  //return matrix;
}
  
template<int RowsCols,ShBindingType Binding, typename T>
ShMatrix<RowsCols,RowsCols,Binding,T>
cofactors(const ShMatrix<RowsCols,RowsCols, Binding, T>& matrix)
{
  ShMatrix<RowsCols,RowsCols,Binding,T> matrixToReturn;
  ShAttrib1f minusOne = -1.0;

  for (int i = 0; i < RowsCols; i++) {
    for (int j = 0; j < RowsCols; j++) {
      if( (i+j)%2 ==0)	  
        matrixToReturn[i][j]=         det(matrix.subMatrix(i,j));
      else
        matrixToReturn[i][j]=minusOne*det(matrix.subMatrix(i,j));
    }
  }
  return matrixToReturn;
}

//Transpose
template<int RowsCols, ShBindingType Binding, typename T>
ShMatrix<RowsCols, RowsCols, Binding, T>
trans(const ShMatrix<RowsCols, RowsCols, Binding, T>& matrix){    
   
  ShMatrix<RowsCols,RowsCols,Binding,T> matrixToReturn;
    
  for (int i = 0; i < RowsCols; i++) {
    for (int j = 0; j < RowsCols; j++) {	  
      matrixToReturn[i][j]= matrix[j][i];	
    }
  }
    
  return matrixToReturn;
}

// Adjoint
template<int RowsCols, ShBindingType Binding, typename T>
ShMatrix<RowsCols, RowsCols, Binding, T>
adj(const ShMatrix<RowsCols, RowsCols, Binding, T>& matrix){
  return trans(cofactors(matrix));
    
}
  
//Inverse
template<int RowsCols, ShBindingType Binding, typename T>
ShMatrix<RowsCols,RowsCols, Binding, T>
inv(const ShMatrix<RowsCols, RowsCols, Binding, T>& matrix){
  ShMatrix<RowsCols, RowsCols, Binding, T> temp (adj(matrix));
  temp*=(1.0f/det(matrix));
  return temp;
}
  
template<int Rows, int Cols, ShBindingType Binding, typename T>
ShMatrix<Rows - 1, Cols -1, SH_TEMP, T>
ShMatrix<Rows, Cols, Binding, T>::subMatrix(int rowToRemove,int columnToRemove) const
{
  ShMatrix<Rows-1, Cols-1,SH_TEMP,T> myMatrix;
    
  int* indices = new int[Cols - 1];
  for (int i = 0; i < columnToRemove; i++) 
    indices[i] = i;
  for (int i = columnToRemove + 1; i < Cols; i++) 
    indices[i - 1] = i;
    
  ShSwizzle swizzle(Cols, Cols - 1, indices);
    
  for(int i=0;i<rowToRemove;i++){
    myMatrix[i]=(ShAttrib<Cols - 1, SH_TEMP, T, false>(m_data[i].node(), swizzle, m_data[i].neg())); 
  }
    
  for(int i=rowToRemove+1;i<Rows;i++){
    myMatrix[i-1]=(ShAttrib<Cols - 1, SH_TEMP, T, false>(m_data[i].node(), swizzle, m_data[i].neg()));
  }
    
  delete [] indices;
  return myMatrix;
}


/*
  template<ShBindingType Binding, typename T>
  void
  ShMatrix<4,4, Binding, T>::setTranslation(const ShGeneric<1, T>& transX,
  const ShGeneric<1, T>& transY,
  const ShGeneric<1, T>& transZ){
  m_data[0][3]=transX;
  m_data[1][3]=transY;
  m_data[2][3]=transZ;
  }

  template<ShBindingType Binding, typename T>
  void
  ShMatrix<4,4, Binding, T>::setScaling(const ShGeneric<1, T>& scaleX,
  const ShGeneric<1, T>& scaleY,
  const ShGeneric<1, T>& scaleZ){
    
  m_data[0][0]=scaleX;
  m_data[1][1]=scaleY;
  m_data[2][2]=scaleZ;
    
  }
*/

template<int Rows,int Cols,ShBindingType Binding, typename T>
void ShMatrix<Rows, Cols, Binding, T>::setTranslation(const ShGeneric<Rows-1, T>& trans){
							
  for(int i=0;i<(Rows-1);i++)
    m_data[i][(Rows-1)]=trans[i];
}
  
  
template<int Rows,int Cols,ShBindingType Binding, typename T>
void ShMatrix<Rows, Cols, Binding, T>::setScaling(const ShGeneric<Rows-1, T>& scale){
  for(int i=0;i<(Rows-1);i++)
    m_data[i][i]=scale[i];
}

/*
  template<ShBindingType Binding, typename T>
  void
  ShMatrix<4, 4, Binding, T>::setRotationX(const ShGeneric<1, T>& angle){
    
    
  }

  template<ShBindingType Binding, typename T>
  void
  ShMatrix<4, 4, Binding, T>::setRotationY(const ShGeneric<1, T>& angle){
    
    
  }

  template<ShBindingType Binding, typename T>
  void
  ShMatrix<4, 4, Binding, T>::setRotationZ(const ShGeneric<1, T>& angle){
    
    
  }



*/

////////////////////////
// ShMatrixRows stuff //
////////////////////////


template<int Rows, int Cols, typename T>
template<int OR, ShBindingType Binding>
ShMatrixRows<Rows, Cols, T>::ShMatrixRows(const ShMatrix<OR, Cols, Binding, T>& source,
                                          int i0)
{
  SH_STATIC_CHECK(Rows == 1, Constructing_Non_1_Row_Matrix_From_1_Row);
  
  m_data[0] = source[i0];
}

template<int Rows, int Cols, typename T>
template<int OR, ShBindingType Binding>
ShMatrixRows<Rows, Cols, T>::ShMatrixRows(const ShMatrix<OR, Cols, Binding, T>& source,
                                          int i0, int i1)
{
  SH_STATIC_CHECK(Rows == 2, Constructing_Non_2_Row_Matrix_From_2_Rows);
  
  m_data[0] = source[i0];
  m_data[1] = source[i1];
}

template<int Rows, int Cols, typename T>
template<int OR, ShBindingType Binding>
ShMatrixRows<Rows, Cols, T>::ShMatrixRows(const ShMatrix<OR, Cols, Binding, T>& source,
                                          int i0, int i1, int i2)
{
  SH_STATIC_CHECK(Rows == 3, Constructing_Non_3_Row_Matrix_From_3_Rows);
  
  m_data[0] = source[i0];
  m_data[1] = source[i1];
  m_data[2] = source[i2];
}

template<int Rows, int Cols, typename T>
template<int OR, ShBindingType Binding>
ShMatrixRows<Rows, Cols, T>::ShMatrixRows(const ShMatrix<OR, Cols, Binding, T>& source,
                                          int i0, int i1, int i2, int i3)
{
  SH_STATIC_CHECK(Rows == 4, Constructing_Non_4_Row_Matrix_From_4_Rows);
  
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
ShMatrix<Rows, Cols, SH_TEMP, T>
ShMatrixRows<Rows, Cols, T>::operator()() const
{
  ShMatrix<Rows, Cols, SH_TEMP, T> r;
  for (int i = 0; i < Rows; i++) r[i] = m_data[i];
  return r;
}

template<int Rows, int Cols, typename T>
ShMatrix<Rows, 1, SH_TEMP, T>
ShMatrixRows<Rows, Cols, T>::operator()(int i0) const
{
  ShMatrix<Rows, 1, SH_TEMP, T> r;
  for (int i = 0; i < Rows; i++) r[i] = m_data[i](i0);
  return r;
}

template<int Rows, int Cols, typename T>
ShMatrix<Rows, 2, SH_TEMP, T>
ShMatrixRows<Rows, Cols, T>::operator()(int i0, int i1) const
{
  ShMatrix<Rows, 2, SH_TEMP, T> r;
  for (int i = 0; i < Rows; i++) r[i] = m_data[i](i0, i1);
  return r;
}

template<int Rows, int Cols, typename T>
ShMatrix<Rows, 3, SH_TEMP, T>
ShMatrixRows<Rows, Cols, T>::operator()(int i0, int i1, int i2) const
{
  ShMatrix<Rows, 3, SH_TEMP, T> r;
  for (int i = 0; i < Rows; i++) r[i] = m_data[i](i0, i1, i2);
  return r;
}

template<int Rows, int Cols, typename T>
ShMatrix<Rows, 4, SH_TEMP, T>
ShMatrixRows<Rows, Cols, T>::operator()(int i0, int i1, int i2, int i3) const
{
  ShMatrix<Rows, 4, SH_TEMP, T> r;
  for (int i = 0; i < Rows; i++) r[i] = m_data[i](i0, i1, i2, i3);
  return r;
}

template<int Rows, int Cols, ShBindingType Binding, typename T>
void ShMatrix<Rows, Cols, Binding, T>::name(const std::string& name)
{
  // TODO: add a row number
  for (int i = 0; i < Rows; i++) r[i] = m_data[i].name(name);
}

template<int Rows, int Cols, ShBindingType Binding, typename T>
std::string ShMatrix<Rows, Cols, Binding, T>::name() const
{
  // TODO: Maybe keep a name for the whole matrix?
  return m_data[0].name();
}

template<int Rows, int Cols, ShBindingType Binding, typename T>
void ShMatrix<Rows, Cols, Binding, T>::range(T low, T high)
{
  for (int i = 0; i < Rows; i++) m_data[i].range(low, high);
}

template<int Rows, int Cols, ShBindingType Binding, typename T>
void ShMatrix<Rows, Cols, Binding, T>::internal(bool setting)
{
  for (int i = 0; i < Rows; i++) m_data[i].internal(setting);
}


}

#endif
