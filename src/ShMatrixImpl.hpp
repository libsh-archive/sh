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
  template<int Rows, int Cols, int Kind, typename T>
  ShMatrix<Rows, Cols, Kind, T>::ShMatrix()
  {
    if (Rows == Cols) {
      for (int i = 0; i < Rows; i++)
	m_data[i][i] = 1.0;
    }
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




  //Operators
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











  template<int Rows, int Cols, int Kind, typename T>
  std::ostream& operator<<(std::ostream& _out, const ShMatrix<Rows, Cols, Kind, T>& shMatrixToPrint){
    
    if (!shMatrixToPrint.m_data){
      _out<<"[null]";
      return _out;
    }
    
    for (int k = 0; k < Rows; k++) {   
      _out<<shMatrixToPrint.m_data[k];
      _out<<endl;
   }
    
    return _out;
  }
  
 

  






 
  
  template<int Kind, typename T>
  ShAttrib1f
  det(const ShMatrix<1, 1, Kind, T>& matrix)
  {
    return matrix.m_data[0][0];
  }

  
  template<int Kind, typename T>
  ShAttrib1f
  det(const ShMatrix<2, 2, Kind, T>& matrix)
  {
    return (matrix.m_data[0][0]*matrix.m_data[1][1] - matrix.m_data[0][1] * matrix.m_data[1][0]);
  }
 
  template<int RowsCols, int Kind, typename T>
  ShAttrib1f
  det(const ShMatrix<RowsCols, RowsCols, Kind, T>& matrix)
  {
    ShAttrib1f ret = 0.0;   
    ShAttrib1f dir = RowsCols % 2 ? 1.0 : -1.0;

    for (int i = 0; i < RowsCols; i++) {
      ret += dir * matrix.m_data[RowsCols - 1][i] * det(matrix.subMatrix(RowsCols - 1, i));
      dir *= -1.0;
    }
    return ret;
  }
  








  




  //Matrix of Cofactors
  template<int Kind, typename T>
  ShMatrix<1,1,Kind,T>
  cofactors(const ShMatrix<1, 1, Kind, T>& matrix){
    return matrix;
  }
    
  template<int Kind, typename T>
  ShMatrix<2,2,Kind,T>
  cofactors(const ShMatrix<2, 2, Kind, T>& matrix)
  {
    ShMatrix<2,2,Kind,T> matrixToReturn;
    ShAttrib1f minusOne = -1.0;
    matrixToReturn.m_data[0][0]=         matrix[1][1];
    matrixToReturn.m_data[1][0]=minusOne*matrix[0][1];
    matrixToReturn.m_data[0][1]=minusOne*matrix[1][0];
    matrixToReturn.m_data[1][1]=         matrix[0][0];
    
    return matrixToReturn;
    //return matrix;
  }
  
  template<int RowsCols,int Kind, typename T>
  ShMatrix<RowsCols,RowsCols,Kind,T>
  cofactors(const ShMatrix<RowsCols,RowsCols, Kind, T>& matrix)
  {
    ShMatrix<RowsCols,RowsCols,Kind,T> matrixToReturn;
    ShAttrib1f minusOne = -1.0;

    for (int i = 0; i < RowsCols; i++) {
      for (int j = 0; j < RowsCols; j++) {
	if( (i+j)%2 ==0)	  
	  matrixToReturn.m_data[i][j]=         det(matrix.subMatrix(i,j));
	else
	  matrixToReturn.m_data[i][j]=minusOne*det(matrix.subMatrix(i,j));
      }
    }
    return matrixToReturn;
  }









  //Transpose
  template<int RowsCols, int Kind, typename T>
  ShMatrix<RowsCols, RowsCols, Kind, T>
  trans(const ShMatrix<RowsCols, RowsCols, Kind, T>& matrix){    
   
    ShMatrix<RowsCols,RowsCols,Kind,T> matrixToReturn;
    
    for (int i = 0; i < RowsCols; i++) {
      for (int j = 0; j < RowsCols; j++) {	  
	  matrixToReturn.m_data[i][j]= matrix[j][i];	
      }
    }
    
    return matrixToReturn;
  }
  

  // Adjoint
  template<int RowsCols, int Kind, typename T>
  ShMatrix<RowsCols, RowsCols, Kind, T>
  adj(const ShMatrix<RowsCols, RowsCols, Kind, T>& matrix){
    return trans(cofactors(matrix));
    
  }



  
  //Inverse
  template<int RowsCols, int Kind, typename T>
  ShMatrix<RowsCols,RowsCols, Kind, T>
  inv(const ShMatrix<RowsCols, RowsCols, Kind, T>& matrix){
    ShMatrix<RowsCols, RowsCols, Kind, T> temp (adj(matrix));
    temp*=(ShConstant1f(1.0)/det(matrix));
    return temp;
  }
    




  




  //Needs to be removed
  template<int Rows, int Cols, int Kind, typename T>
  void ShMatrix<Rows, Cols, Kind, T>::setRow(ShAttrib<Cols, SH_VAR_TEMP, T, false> rowToSet,int rowNumber){      
    m_data[rowNumber] = rowToSet;    
  }



  
  template<int Rows, int Cols, int Kind, typename T>
  ShMatrix<Rows - 1, Cols -1, SH_VAR_TEMP, T> ShMatrix<Rows, Cols, Kind, T>::subMatrix(int rowToRemove,int columnToRemove) const
  {
    ShMatrix<Rows-1, Cols-1,SH_VAR_TEMP,T> myMatrix;
    
    int* indices = new int[Cols - 1];
    for (int i = 0; i < columnToRemove; i++) 
      indices[i] = i;
    for (int i = columnToRemove + 1; i < Cols; i++) 
      indices[i - 1] = i;
    
    ShSwizzle swizzle(Cols, Cols - 1, indices);
    
    for(int i=0;i<rowToRemove;i++){
      myMatrix.setRow(ShAttrib<Cols - 1, SH_VAR_TEMP, T, false>(m_data[i].node(), swizzle, m_data[i].neg()),i); 
    }
    
    for(int i=rowToRemove+1;i<Rows;i++){
      myMatrix.setRow(ShAttrib<Cols - 1, SH_VAR_TEMP, T, false>(m_data[i].node(), swizzle, m_data[i].neg()),i-1);
    }
    
    delete [] indices;
    return myMatrix;
  }


  /*
  template<int Kind, typename T>
  void
  ShMatrix<4,4, Kind, T>::setTranslation(const ShVariableN<1, T>& transX,
						const ShVariableN<1, T>& transY,
						const ShVariableN<1, T>& transZ){
    m_data[0][3]=transX;
    m_data[1][3]=transY;
    m_data[2][3]=transZ;
  }

  template<int Kind, typename T>
  void
  ShMatrix<4,4, Kind, T>::setScaling(const ShVariableN<1, T>& scaleX,
					    const ShVariableN<1, T>& scaleY,
					    const ShVariableN<1, T>& scaleZ){
    
    m_data[0][0]=scaleX;
    m_data[1][1]=scaleY;
    m_data[2][2]=scaleZ;
    
  }
  
  
  template<int Kind, typename T>
  void
  ShMatrix<4, 4, Kind, T>::setRotationX(const ShVariableN<1, T>& angle){
    
    
  }

template<int Kind, typename T>
  void
  ShMatrix<4, 4, Kind, T>::setRotationY(const ShVariableN<1, T>& angle){
    
    
  }

template<int Kind, typename T>
  void
  ShMatrix<4, 4, Kind, T>::setRotationZ(const ShVariableN<1, T>& angle){
    
    
  }



*/










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
