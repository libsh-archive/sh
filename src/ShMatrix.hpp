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
#ifndef SH_SHMATRIX_HPP
#define SH_SHMATRIX_HPP

#include "ShVariable.hpp"
#include "ShAttrib.hpp"
#include "ShRefCount.hpp"

namespace SH {

template<int Rows, int Cols, ShBindingType Binding, typename T>
class ShMatrix;

template<int Rows, int Cols, typename T>
class ShMatrixRows;

/** A matrix of Rows by Cols elements.
 * A matrix is a representation of a linear operator.  In Sh, this
 * class represents SMALL matrices that will fit in registers.   For
 * large amounts of data, use an ShArray.
 * @see ShArray
 */
template<int Rows, int Cols, ShBindingType Binding, typename T>
class ShMatrix {
public:
    
  /** \brief Constructor for ShMatrix.
   *
   * Creates a identity ShMatrix
   */
  ShMatrix();

  /** \brief Constructor for ShMatrix with a matrix as parameter.
   *
   * Creates a ShMatrix where each element is equal to the one in the parameters
   * \param other the matrix from which we will get the values from 
   */
  ShMatrix(const ShMatrix<Rows, Cols, Binding, T>& other);


  /** \brief Constructor for ShMatrix with a matrix as parameter. (template kind2)
   *
   * Creates a ShMatrix where each element is equal to the one in the parameters
   * \param other the matrix from which we will get the values from 
   */
  template<ShBindingType Binding2>
  ShMatrix(const ShMatrix<Rows, Cols, Binding2, T>& other);
  

  /** \brief Destructor for ShMatrix
   *
   * Destroys a ShMatrix 
   */
  ~ShMatrix();

  /** \brief Definition of the equal operation with another matrix
   *
   * Returns the address of a matrix from which the values were taken from the ones of the matrix in the parameters
   * \param other the matrix from which we will get the values from 
   */
  ShMatrix& operator=(const ShMatrix<Rows, Cols, Binding, T>& other);

  /** \brief Definition of the equal operation with another matrix (template kind2)
   *
   * Returns the address of a matrix from which the values were taken from the ones of the matrix in the parameters (template kind2)
   * \param other the matrix from which we will get the values from 
   */
  template<ShBindingType Binding2>
  ShMatrix& operator=(const ShMatrix<Rows, Cols, Binding2, T>& other);

  /** \brief Definition of the bracket [] operation
   *
   * Returns the row of our current matrix where the row number is given by the parameter i
   * \param i The number of the row (ranges from 0 to Rows-1)
   */
  ShAttrib<Cols, Binding, T>& operator[](int i);

  /** \brief Definition of the bracket [] operation (const)
   *
   * Returns the row of our current matrix where the row number is given by the parameter i (const)
   * \param i The number of the row (ranges from 0 to Rows-1)
   */
  const ShAttrib<Cols, Binding, T>& operator[](int i) const;

  /** \brief Definition of the += operation with another matrix
   *
   * Returns the address of a matrix where the result is the current matrix += (for each element) with the matrix in the parameters
   * \param other The other matrix that is used in the +=operation
   */
  template<ShBindingType Binding2>
  ShMatrix& operator+=(const ShMatrix<Rows, Cols, Binding2, T>& other);

  /** \brief Definition of the -= operation
   *
   * Returns the address of a matrix where the result is the current matrix += (for each element) with the matrix in the parameters
   * \param other The other matrix that is used in the -=operation
   */
  template<ShBindingType Binding2>
  ShMatrix& operator-=(const ShMatrix<Rows, Cols, Binding2, T>& other);

  /** \brief Definition of the *= operation
   *
   * Returns the address of a matrix where the result is the current matrix += (for each element) with the matrix in the parameters
   * \param other The other matrix that is used in the *=operation
   */
  template<ShBindingType Binding2>
  ShMatrix& operator*=(const ShMatrix<Rows, Cols, Binding2, T>& other);

  /** \brief Definition of the /= operation
   *
   * Returns the address of a matrix where the result is the current matrix += (for each element) with the matrix in the parameters
   * \param other The other matrix that is used in the /=operation
   */
  template<ShBindingType Binding2>
  ShMatrix& operator/=(const ShMatrix<Rows, Cols, Binding2, T>& other);    
   
  /** \brief Operator << (used for iostream)
   *
   * This operator uses the << operator defined in ShAttrib on each of its rows to give the output
   */
  template<int R, int C, ShBindingType B, typename Ty>
  friend std::ostream& operator<<(std::ostream& out, const ShMatrix<R, C, B, Ty>& shMatrixToPrint);

  /** \brief Returns a submatrix of the original matrix where the row (first parameter), and the column (second parameter) has been removed
   *
   * Returns a ShMatrix that is the matrix where the row (first parameter), and the column (second parameter) has been removed, the matrix has a size of Rows-1, Cols -1
   * \param other The matrix from which we want to get the submatrix
   */
  ShMatrix<Rows - 1, Cols -1, SH_TEMP, T> subMatrix(int,int) const;

  void setTranslation(const ShGeneric<Rows-1, T>& trans);
  
  void setScaling(const ShGeneric<Rows-1, T>& scale);


  /** \brief Definition of the scalar multiplication operation
   *
   * Returns the address of a matrix where the result is the current matrix * (for each element) with the scalar in the parameters
   * \param other The other matrix that is used in the multiplication operation with the scalar
   */
  ShMatrix& operator*=(const ShGeneric<1, T>& other);
    
  /** \brief Definition of the scalar division operation
   *
   * Returns the address of a matrix where the result is the current matrix * (for each element) with the scalar in the parameters
   * \param other The other matrix that is used in the division operation with the scalar
   */
  ShMatrix& operator/=(const ShGeneric<1, T>& other);

  /**@name Swizzling
   * Retrieve some set of rows from this matrix. These must be
   * swizzled again to operate on them.
   */
  //@{
  ShMatrixRows<Rows, Cols, T> operator()() const; ///< Identity swizzle
  ShMatrixRows<1, Cols, T> operator()(int) const;
  ShMatrixRows<2, Cols, T> operator()(int, int) const;
  ShMatrixRows<3, Cols, T> operator()(int, int, int) const;
  ShMatrixRows<4, Cols, T> operator()(int, int, int, int) const;
  //@}
  
  /**@name Metadata
   * This data is useful for various things, including asset
   * management.
   */
  //@{

  /// Set this matrix's name. If set to the empty string, defaults
  /// to the type and id of the variable.
  void name(const std::string& name);
  std::string name() const; ///< Get this matrix's name
  
  /// Set a range of values for this matrix
  void range(T low, T high);

  /// If this is true, this matrix should not be able to be set by
  /// e.g. a user in a UI. For example the model-view matrix should
  /// probably have this set to true.
  /// This is false by default.
  void internal(bool setting);

  //@}
  
  
private:
  /** \brief The Matrix itself
   *
   * The data structure that hold the data
   */
  ShAttrib<Cols, Binding, T> m_data[Rows];
  
};//end ShMatrix
  

  /** A few rows from a matrix.
   * This is an intermediate structure representing some rows that have
   * just been swizzled from a matrix. The only useful operation is to
   * swizzle these rows again to obtain a submatrix with the given rows
   * and columns.
   */
template<int Rows, int Cols, typename T>
class ShMatrixRows {
public:
  template<ShBindingType Binding>
  ShMatrixRows(const ShMatrix<Rows, Cols, Binding, T>& source);
  
  template<int OR, ShBindingType Binding>
  ShMatrixRows(const ShMatrix<OR, Cols, Binding, T>& source,
               int idx0);
  template<int OR, ShBindingType Binding>
  ShMatrixRows(const ShMatrix<OR, Cols, Binding, T>& source,
               int idx0, int idx1);
  template<int OR, ShBindingType Binding>
  ShMatrixRows(const ShMatrix<OR, Cols, Binding, T>& source,
               int idx0, int idx1, int idx2);
  template<int OR, ShBindingType Binding>
  ShMatrixRows(const ShMatrix<OR, Cols, Binding, T>& source,
               int idx0, int idx1, int idx2, int idx3);

  ShMatrixRows(const ShMatrixRows<Rows, Cols, T>& other);
  
  ShMatrixRows& operator=(const ShMatrixRows<Rows, Cols, T>& other);

  /**@name Swizzling
   * Retrieve a matrix consisting of these rows and the given columns.
   */
  //@{
  ShMatrix<Rows, Cols, SH_TEMP, T> operator()() const; ///< Identity swizzle
  ShMatrix<Rows, 1, SH_TEMP, T> operator()(int) const;
  ShMatrix<Rows, 2, SH_TEMP, T> operator()(int, int) const;
  ShMatrix<Rows, 3, SH_TEMP, T> operator()(int, int, int) const;
  ShMatrix<Rows, 4, SH_TEMP, T> operator()(int, int, int, int) const;
  //@}
private:
  ShAttrib<Cols, SH_TEMP, T> m_data[Rows];
};

    
  /** \brief Returns the determinant for the matrix in parameter
   *
   * Returns a ShAttrib1f containing the value of the determinant of the matrix. NOTE: This method uses an naive recursive algorithm in O(n!) which could take a long time with large matrices
   * \param other The matrix from which we want the determinant (need to be a square matrix to compile)
   */
  template<ShBindingType Binding2, typename T2>
   ShAttrib1f det(const ShMatrix<1, 1, Binding2, T2>& matrix);

  template<ShBindingType Binding2, typename T2>
   ShAttrib1f det(const ShMatrix<2, 2, Binding2, T2>& matrix);
    
  template <int RowsCols, ShBindingType Binding2, typename T2>
   ShAttrib1f det(const ShMatrix<RowsCols, RowsCols, Binding2, T2>& matrix);
    
    
  /** \brief Returns the matrix of cofactors for the matrix in parameter
   *
   * Returns a ShMatrix containing the same amount of Rows/Colsvalue as the matrix in parameter 
   * \param other The matrix from which we want the matrix of cofactors (need to be a square matrix to compile)
   */
  template <int RowsCols, ShBindingType Binding2, typename T2>
  ShMatrix<RowsCols, RowsCols, Binding2, T2> cofactors(const ShMatrix<RowsCols, RowsCols, Binding2, T2>& matrix);
    
  /** \brief Returns the transposed matrix for the matrix in parameter
   *
   * Returns a ShMatrix that is the transposed of the one in parameters
   * \param other The matrix from which we want the matrix of cofactors (need to be a square matrix to compile, we might want to change that...)
   */
  template <int RowsCols, ShBindingType Binding2, typename T2>
  ShMatrix<RowsCols, RowsCols, Binding2, T2> trans(const ShMatrix<RowsCols, RowsCols, Binding2, T2>& matrix);
  
  /** \brief Returns the adjacent matrix for the matrix in parameter
   *
   * Returns a ShMatrix that is the adjacent of the one in parameters
   * \param other The matrix from which we want the adjacent matrix (need to be a square matrix to compile)
   */
  template <int RowsCols, ShBindingType Binding2, typename T2>
  ShMatrix<RowsCols, RowsCols, Binding2, T2> adj(const ShMatrix<RowsCols, RowsCols, Binding2, T2>& matrix);

  /** \brief Returns the inverse matrix for the matrix in parameter
   *
   * Returns a ShMatrix that is the inverse of the one in parameters (NOTE: There is no error handling if the determinant is 0, NOTE #2: We might want to rewrite this method if we have the * operator with a ShVariableN)
   * \param other The matrix from which we want the inverse matrix (need to be a square matrix to compile)
   */
  template <int RowsCols, ShBindingType Binding2, typename T2>
  ShMatrix<RowsCols,RowsCols, Binding2, T2> inv(const ShMatrix<RowsCols, RowsCols, Binding2, T2>& matrix);


typedef ShMatrix<1, 1, SH_INPUT, float> ShInputMatrix1x1f;
typedef ShMatrix<1, 1, SH_OUTPUT, float> ShOutputMatrix1x1f;
typedef ShMatrix<1, 1, SH_INOUT, float> ShInOutMatrix1x1f;
typedef ShMatrix<1, 1, SH_TEMP, float> ShMatrix1x1f;
typedef ShMatrix<1, 2, SH_INPUT, float> ShInputMatrix1x2f;
typedef ShMatrix<1, 2, SH_OUTPUT, float> ShOutputMatrix1x2f;
typedef ShMatrix<1, 2, SH_INOUT, float> ShInOutMatrix1x2f;
typedef ShMatrix<1, 2, SH_TEMP, float> ShMatrix1x2f;
typedef ShMatrix<1, 3, SH_INPUT, float> ShInputMatrix1x3f;
typedef ShMatrix<1, 3, SH_OUTPUT, float> ShOutputMatrix1x3f;
typedef ShMatrix<1, 3, SH_INOUT, float> ShInOutMatrix1x3f;
typedef ShMatrix<1, 3, SH_TEMP, float> ShMatrix1x3f;
typedef ShMatrix<1, 4, SH_INPUT, float> ShInputMatrix1x4f;
typedef ShMatrix<1, 4, SH_OUTPUT, float> ShOutputMatrix1x4f;
typedef ShMatrix<1, 4, SH_INOUT, float> ShInOutMatrix1x4f;
typedef ShMatrix<1, 4, SH_TEMP, float> ShMatrix1x4f;

typedef ShMatrix<2, 1, SH_INPUT, float> ShInputMatrix2x1f;
typedef ShMatrix<2, 1, SH_OUTPUT, float> ShOutputMatrix2x1f;
typedef ShMatrix<2, 1, SH_INOUT, float> ShInOutMatrix2x1f;
typedef ShMatrix<2, 1, SH_TEMP, float> ShMatrix2x1f;
typedef ShMatrix<2, 2, SH_INPUT, float> ShInputMatrix2x2f;
typedef ShMatrix<2, 2, SH_OUTPUT, float> ShOutputMatrix2x2f;
typedef ShMatrix<2, 2, SH_INOUT, float> ShInOutMatrix2x2f;
typedef ShMatrix<2, 2, SH_TEMP, float> ShMatrix2x2f;
typedef ShMatrix<2, 3, SH_INPUT, float> ShInputMatrix2x3f;
typedef ShMatrix<2, 3, SH_OUTPUT, float> ShOutputMatrix2x3f;
typedef ShMatrix<2, 3, SH_INOUT, float> ShInOutMatrix2x3f;
typedef ShMatrix<2, 3, SH_TEMP, float> ShMatrix2x3f;
typedef ShMatrix<2, 4, SH_INPUT, float> ShInputMatrix2x4f;
typedef ShMatrix<2, 4, SH_OUTPUT, float> ShOutputMatrix2x4f;
typedef ShMatrix<2, 4, SH_INOUT, float> ShInOutMatrix2x4f;
typedef ShMatrix<2, 4, SH_TEMP, float> ShMatrix2x4f;

typedef ShMatrix<3, 1, SH_INPUT, float> ShInputMatrix3x1f;
typedef ShMatrix<3, 1, SH_OUTPUT, float> ShOutputMatrix3x1f;
typedef ShMatrix<3, 1, SH_INOUT, float> ShInOutMatrix3x1f;
typedef ShMatrix<3, 1, SH_TEMP, float> ShMatrix3x1f;
typedef ShMatrix<3, 2, SH_INPUT, float> ShInputMatrix3x2f;
typedef ShMatrix<3, 2, SH_OUTPUT, float> ShOutputMatrix3x2f;
typedef ShMatrix<3, 2, SH_INOUT, float> ShInOutMatrix3x2f;
typedef ShMatrix<3, 2, SH_TEMP, float> ShMatrix3x2f;
typedef ShMatrix<3, 3, SH_INPUT, float> ShInputMatrix3x3f;
typedef ShMatrix<3, 3, SH_OUTPUT, float> ShOutputMatrix3x3f;
typedef ShMatrix<3, 3, SH_INOUT, float> ShInOutMatrix3x3f;
typedef ShMatrix<3, 3, SH_TEMP, float> ShMatrix3x3f;
typedef ShMatrix<3, 4, SH_INPUT, float> ShInputMatrix3x4f;
typedef ShMatrix<3, 4, SH_OUTPUT, float> ShOutputMatrix3x4f;
typedef ShMatrix<3, 4, SH_INOUT, float> ShInOutMatrix3x4f;
typedef ShMatrix<3, 4, SH_TEMP, float> ShMatrix3x4f;

typedef ShMatrix<4, 1, SH_INPUT, float> ShInputMatrix4x1f;
typedef ShMatrix<4, 1, SH_OUTPUT, float> ShOutputMatrix4x1f;
typedef ShMatrix<4, 1, SH_INOUT, float> ShInOutMatrix4x1f;
typedef ShMatrix<4, 1, SH_TEMP, float> ShMatrix4x1f;
typedef ShMatrix<4, 2, SH_INPUT, float> ShInputMatrix4x2f;
typedef ShMatrix<4, 2, SH_OUTPUT, float> ShOutputMatrix4x2f;
typedef ShMatrix<4, 2, SH_INOUT, float> ShInOutMatrix4x2f;
typedef ShMatrix<4, 2, SH_TEMP, float> ShMatrix4x2f;
typedef ShMatrix<4, 3, SH_INPUT, float> ShInputMatrix4x3f;
typedef ShMatrix<4, 3, SH_OUTPUT, float> ShOutputMatrix4x3f;
typedef ShMatrix<4, 3, SH_INOUT, float> ShInOutMatrix4x3f;
typedef ShMatrix<4, 3, SH_TEMP, float> ShMatrix4x3f;
typedef ShMatrix<4, 4, SH_INPUT, float> ShInputMatrix4x4f;
typedef ShMatrix<4, 4, SH_OUTPUT, float> ShOutputMatrix4x4f;
typedef ShMatrix<4, 4, SH_INOUT, float> ShInOutMatrix4x4f;
typedef ShMatrix<4, 4, SH_TEMP, float> ShMatrix4x4f;

}

#include "ShMatrixImpl.hpp"

#endif







