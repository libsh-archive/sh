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
#ifndef SHMATRIX_HPP
#define SHMATRIX_HPP

#include "Meta.hpp"
#include "Variable.hpp"
#include "Attrib.hpp"
#include "RefCount.hpp"

namespace SH {

template<int Rows, int Cols, BindingType Binding, typename T>
class Matrix;

template<int Rows, int Cols, typename T>
class MatrixRows;

/** A matrix of Rows by Cols elements.
 * A matrix is a representation of a linear operator.  In Sh, this
 * class represents SMALL matrices that will fit in registers.   For
 * large amounts of data, use an Array.
 * @see Array
 */
template<int Rows, int Cols, BindingType Binding, typename T>
class Matrix: public virtual Meta {
public:
  typedef T storage_type;
  typedef typename HostType<T>::type host_type;
  typedef typename MemType<T>::type mem_type;
  static const ValueType value_type = StorageTypeInfo<T>::value_type;
  static const BindingType binding_type = Binding;
  static const int rows = Rows;
  static const int cols = Cols;
  static const int typesize = Rows * Cols;

  typedef Matrix<Rows, Cols, SH_INPUT, T> InputType;
  typedef Matrix<Rows, Cols, SH_OUTPUT, T> OutputType;
  typedef Matrix<Rows, Cols, SH_INOUT, T> InOutType;
  typedef Matrix<Rows, Cols, SH_TEMP, T> TempType;
  typedef Matrix<Rows, Cols, SH_CONST, T> ConstType;

  /** \brief Identity constructor.
   *
   * Constructs an identity matrix.
   * For non-square matrices, the largest square upper-left submatrix
   * possible is made an identity matrix, with the rest left zero.
   *
   */
  Matrix();

  /** \brief Copy constructor.
   *
   * Construct a matrix with the same contents as the given matrix.
   */
  template<BindingType Binding2>
  Matrix(const Matrix<Rows, Cols, Binding2, T>& other);
  
  ~Matrix();

  /** \brief Assignment.
   *
   * Replace each entry in this matrix with the corresponding entry of
   * the given matrix.
   */
  Matrix& operator=(const Matrix<Rows, Cols, Binding, T>& other);

  /** \brief Assignment.
   *
   * Replace each entry in this matrix with the corresponding entry of
   * the given matrix.
   */
  template<BindingType Binding2>
  Matrix& operator=(const Matrix<Rows, Cols, Binding2, T>& other);

  /** \brief Assignment (scalar promotion).
   * 
   * Construct an identity matrix multiplied by the scalar.
   */
  Matrix& operator=(const T& scalar);

  /** \brief Assignment (scalar promotion).
   * 
   * Construct an identity matrix multiplied by the scalar.
   */
  Matrix& operator=(const Generic<1, T>& scalar);

  /** \brief Attribute row access.
   *
   * Return a reference to the given row as an Attrib.
   */
  Attrib<Cols, Binding, T>& operator[](int i);

  /** \brief Attribute row access.
   *
   * Return a reference to the given row as an Attrib.
   */
  const Attrib<Cols, Binding, T>& operator[](int i) const;

  /** \brief Modifying componentwise addition
   *
   * Add each entry in the given matrix to each entry in this
   * matrix matching its row and column index.
   */
  template<BindingType Binding2>
  Matrix& operator+=(const Matrix<Rows, Cols, Binding2, T>& other);

  /** \brief Modifying componentwise subtraction
   *
   * Subtract each entry in the given matrix from each entry in this
   * matrix matching its row and column index.
   */
  template<BindingType Binding2>
  Matrix& operator-=(const Matrix<Rows, Cols, Binding2, T>& other);


  /** \brief Modifying componentwise division
   *
   * Divide each entry in this matrix by the entry in the given matrix
   * matching its row and column index.
   *
   */
  template<BindingType Binding2>
  Matrix& operator/=(const Matrix<Rows, Cols, Binding2, T>& other);    

  /** \brief Modifying matrix multiplication
   *
   * Replace the matrix by the result of the matrix multiplied by the
   * other.  Note: the two matrices must have the same size.
   */
  template<BindingType Binding2>
  Matrix& operator*=(const Matrix<Rows, Cols, Binding2, T>& other);    
   
  /** \brief Negation
   *
   * Assign each entry to its negated value.
   */
  Matrix& operator-();

  /** \brief Obtain a submatrix of this matrix.
   *
   * Return a copy of this matrix not containing the given row and column.
   */
  Matrix<Rows - 1, Cols -1, SH_TEMP, T> subMatrix(int,int) const;

  void setTranslation(const Generic<Rows-1, T>& trans);
  
  void setScaling(const Generic<Rows-1, T>& scale);


  /** \brief Modifying scalar multiplicatoin
   *
   * Multiply the given scalar attribute with each component of this
   * matrix.
   */
  Matrix& operator*=(const Generic<1, T>& a);
    
  /** \brief Modifying scalar division
   *
   * Divide each component of this matrix by the given attribute.
   */
  Matrix& operator/=(const Generic<1, T>& a);

  /**@name Swizzling
   * Retrieve some set of rows from this matrix. These must be
   * swizzled again to operate on them.
   */
  //@{
  MatrixRows<Rows, Cols, T> operator()() const; ///< Identity swizzle
  MatrixRows<1, Cols, T> operator()(int) const;
  MatrixRows<2, Cols, T> operator()(int, int) const;
  MatrixRows<3, Cols, T> operator()(int, int, int) const;
  MatrixRows<4, Cols, T> operator()(int, int, int, int) const;
  //@}
  
  /**@name Metadata
   * This data is useful for various things, including asset
   * management.
   */
  //@{

  /// Set a range of values for this matrix
  void range(host_type low, host_type high);

  //@}
  
  virtual std::string name() const;
  virtual void name(const std::string& n);
  virtual bool has_name() const;
  
  virtual bool internal() const;
  virtual void internal(bool);

  virtual std::string title() const;
  virtual void title(const std::string& t);

  virtual std::string description() const;
  virtual void description(const std::string& d);

  virtual std::string meta(const std::string& key) const;
  virtual void meta(const std::string& key, const std::string& value);

  int size() const { return Rows * Cols; }
  
  virtual void getValues(host_type dest[]) const;

private:
  /** \brief The Matrix itself
   *
   * The data structure that hold the data
   */
  Attrib<Cols, Binding, T> m_data[Rows];
  
};
/** \brief Matrix output operator
 *
 * Print a representation of the given (uniform or constant) matrix's
 * values to the given stream.
 */

template<int R, int C, BindingType B, typename Ty>
std::ostream& operator<<(std::ostream& out,
                         const Matrix<R, C, B, Ty>& m);

/** A few rows from a matrix.
 * This is an intermediate structure representing some rows that have
 * just been swizzled from a matrix. The only useful operation is to
 * swizzle these rows again to obtain a submatrix with the given rows
 * and columns.
 */
template<int Rows, int Cols, typename T>
class MatrixRows {
public:
  template<BindingType Binding>
  MatrixRows(const Matrix<Rows, Cols, Binding, T>& source);
  
  template<int OR, BindingType Binding>
  MatrixRows(const Matrix<OR, Cols, Binding, T>& source,
               int idx0);
  template<int OR, BindingType Binding>
  MatrixRows(const Matrix<OR, Cols, Binding, T>& source,
               int idx0, int idx1);
  template<int OR, BindingType Binding>
  MatrixRows(const Matrix<OR, Cols, Binding, T>& source,
               int idx0, int idx1, int idx2);
  template<int OR, BindingType Binding>
  MatrixRows(const Matrix<OR, Cols, Binding, T>& source,
               int idx0, int idx1, int idx2, int idx3);

  MatrixRows(const MatrixRows<Rows, Cols, T>& other);
  
  MatrixRows& operator=(const MatrixRows<Rows, Cols, T>& other);

  /**@name Swizzling
   * Retrieve a matrix consisting of these rows and the given columns.
   */
  //@{
  Matrix<Rows, Cols, SH_TEMP, T> operator()() const; ///< Identity swizzle
  Matrix<Rows, 1, SH_TEMP, T> operator()(int) const;
  Matrix<Rows, 2, SH_TEMP, T> operator()(int, int) const;
  Matrix<Rows, 3, SH_TEMP, T> operator()(int, int, int) const;
  Matrix<Rows, 4, SH_TEMP, T> operator()(int, int, int, int) const;
  //@}
private:
  Attrib<Cols, SH_TEMP, T> m_data[Rows];
};


typedef Matrix<1, 1, SH_INPUT, float> InputMatrix1x1f;
typedef Matrix<1, 1, SH_OUTPUT, float> OutputMatrix1x1f;
typedef Matrix<1, 1, SH_INOUT, float> InOutMatrix1x1f;
typedef Matrix<1, 1, SH_TEMP, float> Matrix1x1f;
typedef Matrix<1, 2, SH_INPUT, float> InputMatrix1x2f;
typedef Matrix<1, 2, SH_OUTPUT, float> OutputMatrix1x2f;
typedef Matrix<1, 2, SH_INOUT, float> InOutMatrix1x2f;
typedef Matrix<1, 2, SH_TEMP, float> Matrix1x2f;
typedef Matrix<1, 3, SH_INPUT, float> InputMatrix1x3f;
typedef Matrix<1, 3, SH_OUTPUT, float> OutputMatrix1x3f;
typedef Matrix<1, 3, SH_INOUT, float> InOutMatrix1x3f;
typedef Matrix<1, 3, SH_TEMP, float> Matrix1x3f;
typedef Matrix<1, 4, SH_INPUT, float> InputMatrix1x4f;
typedef Matrix<1, 4, SH_OUTPUT, float> OutputMatrix1x4f;
typedef Matrix<1, 4, SH_INOUT, float> InOutMatrix1x4f;
typedef Matrix<1, 4, SH_TEMP, float> Matrix1x4f;

typedef Matrix<2, 1, SH_INPUT, float> InputMatrix2x1f;
typedef Matrix<2, 1, SH_OUTPUT, float> OutputMatrix2x1f;
typedef Matrix<2, 1, SH_INOUT, float> InOutMatrix2x1f;
typedef Matrix<2, 1, SH_TEMP, float> Matrix2x1f;
typedef Matrix<2, 2, SH_INPUT, float> InputMatrix2x2f;
typedef Matrix<2, 2, SH_OUTPUT, float> OutputMatrix2x2f;
typedef Matrix<2, 2, SH_INOUT, float> InOutMatrix2x2f;
typedef Matrix<2, 2, SH_TEMP, float> Matrix2x2f;
typedef Matrix<2, 3, SH_INPUT, float> InputMatrix2x3f;
typedef Matrix<2, 3, SH_OUTPUT, float> OutputMatrix2x3f;
typedef Matrix<2, 3, SH_INOUT, float> InOutMatrix2x3f;
typedef Matrix<2, 3, SH_TEMP, float> Matrix2x3f;
typedef Matrix<2, 4, SH_INPUT, float> InputMatrix2x4f;
typedef Matrix<2, 4, SH_OUTPUT, float> OutputMatrix2x4f;
typedef Matrix<2, 4, SH_INOUT, float> InOutMatrix2x4f;
typedef Matrix<2, 4, SH_TEMP, float> Matrix2x4f;

typedef Matrix<3, 1, SH_INPUT, float> InputMatrix3x1f;
typedef Matrix<3, 1, SH_OUTPUT, float> OutputMatrix3x1f;
typedef Matrix<3, 1, SH_INOUT, float> InOutMatrix3x1f;
typedef Matrix<3, 1, SH_TEMP, float> Matrix3x1f;
typedef Matrix<3, 2, SH_INPUT, float> InputMatrix3x2f;
typedef Matrix<3, 2, SH_OUTPUT, float> OutputMatrix3x2f;
typedef Matrix<3, 2, SH_INOUT, float> InOutMatrix3x2f;
typedef Matrix<3, 2, SH_TEMP, float> Matrix3x2f;
typedef Matrix<3, 3, SH_INPUT, float> InputMatrix3x3f;
typedef Matrix<3, 3, SH_OUTPUT, float> OutputMatrix3x3f;
typedef Matrix<3, 3, SH_INOUT, float> InOutMatrix3x3f;
typedef Matrix<3, 3, SH_TEMP, float> Matrix3x3f;
typedef Matrix<3, 4, SH_INPUT, float> InputMatrix3x4f;
typedef Matrix<3, 4, SH_OUTPUT, float> OutputMatrix3x4f;
typedef Matrix<3, 4, SH_INOUT, float> InOutMatrix3x4f;
typedef Matrix<3, 4, SH_TEMP, float> Matrix3x4f;

typedef Matrix<4, 1, SH_INPUT, float> InputMatrix4x1f;
typedef Matrix<4, 1, SH_OUTPUT, float> OutputMatrix4x1f;
typedef Matrix<4, 1, SH_INOUT, float> InOutMatrix4x1f;
typedef Matrix<4, 1, SH_TEMP, float> Matrix4x1f;
typedef Matrix<4, 2, SH_INPUT, float> InputMatrix4x2f;
typedef Matrix<4, 2, SH_OUTPUT, float> OutputMatrix4x2f;
typedef Matrix<4, 2, SH_INOUT, float> InOutMatrix4x2f;
typedef Matrix<4, 2, SH_TEMP, float> Matrix4x2f;
typedef Matrix<4, 3, SH_INPUT, float> InputMatrix4x3f;
typedef Matrix<4, 3, SH_OUTPUT, float> OutputMatrix4x3f;
typedef Matrix<4, 3, SH_INOUT, float> InOutMatrix4x3f;
typedef Matrix<4, 3, SH_TEMP, float> Matrix4x3f;
typedef Matrix<4, 4, SH_INPUT, float> InputMatrix4x4f;
typedef Matrix<4, 4, SH_OUTPUT, float> OutputMatrix4x4f;
typedef Matrix<4, 4, SH_INOUT, float> InOutMatrix4x4f;
typedef Matrix<4, 4, SH_TEMP, float> Matrix4x4f;

}

#include "MatrixImpl.hpp"

#endif // SHMATRIX_HPP
