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
#ifndef SH_SHMATRIX_HPP
#define SH_SHMATRIX_HPP

#include "ShVariable.hpp"
#include "ShAttrib.hpp"
#include "ShRefCount.hpp"

namespace SH {

template<int Rows, int Cols, ShBindingType Binding, ShValueType V>
class ShMatrix;

template<int Rows, int Cols, ShValueType V>
class ShMatrixRows;

/** A matrix of Rows by Cols elements.
 * A matrix is a representation of a linear operator.  In Sh, this
 * class represents SMALL matrices that will fit in registers.   For
 * large amounts of data, use an ShArray.
 * @see ShArray
 */
template<int Rows, int Cols, ShBindingType Binding, ShValueType V>
class ShMatrix {
public:
  static const ShValueType ValueType = V;
  typedef typename ShHostType<V>::type H; 
  typedef H HostType; 
    
  /** \brief Identity constructor.
   *
   * Constructs an identity matrix.
   * For non-square matrices, the largest square upper-left submatrix
   * possible is made an identity matrix, with the rest left zero.
   *
   */
  ShMatrix();

  /** \brief Copy constructor.
   *
   * Construct a matrix with the same contents as the given matrix.
   */
  ShMatrix(const ShMatrix<Rows, Cols, Binding, V>& other);


  /** \brief Copy constructor.
   *
   * Construct a matrix with the same contents as the given matrix.
   */
  template<ShBindingType Binding2>
  ShMatrix(const ShMatrix<Rows, Cols, Binding2, V>& other);
  
  ~ShMatrix();

  /** \brief Assignment.
   *
   * Replace each entry in this matrix with the corresponding entry of
   * the given matrix.
   */
  ShMatrix& operator=(const ShMatrix<Rows, Cols, Binding, V>& other);

  /** \brief Assignment.
   *
   * Replace each entry in this matrix with the corresponding entry of
   * the given matrix.
   */
  template<ShBindingType Binding2>
  ShMatrix& operator=(const ShMatrix<Rows, Cols, Binding2, V>& other);

  /** \brief Attribute row access.
   *
   * Return a reference to the given row as an ShAttrib.
   */
  ShAttrib<Cols, Binding, V>& operator[](int i);

  /** \brief Attribute row access.
   *
   * Return a reference to the given row as an ShAttrib.
   */
  const ShAttrib<Cols, Binding, V>& operator[](int i) const;

  /** \brief Modifying componentwise addition
   *
   * Add each entry in the given matrix to each entry in this
   * matrix matching its row and column index.
   */
  template<ShBindingType Binding2>
  ShMatrix& operator+=(const ShMatrix<Rows, Cols, Binding2, V>& other);

  /** \brief Modifying componentwise subtraction
   *
   * Subtract each entry in the given matrix from each entry in this
   * matrix matching its row and column index.
   */
  template<ShBindingType Binding2>
  ShMatrix& operator-=(const ShMatrix<Rows, Cols, Binding2, V>& other);


  /** \brief Modifying componentwise division
   *
   * Divide each entry in this matrix by the entry in the given matrix
   * matching its row and column index.
   *
   */
  template<ShBindingType Binding2>
  ShMatrix& operator/=(const ShMatrix<Rows, Cols, Binding2, V>& other);    
   

  /** \brief Obtain a submatrix of this matrix.
   *
   * Return a copy of this matrix not containing the given row and column.
   */
  ShMatrix<Rows - 1, Cols -1, SH_TEMP, V> subMatrix(int,int) const;

  void setTranslation(const ShGeneric<Rows-1, V>& trans);
  
  void setScaling(const ShGeneric<Rows-1, V>& scale);


  /** \brief Modifying scalar multiplicatoin
   *
   * Multiply the given scalar attribute with each component of this
   * matrix.
   */
  ShMatrix& operator*=(const ShGeneric<1, V>& a);
    
  /** \brief Modifying scalar division
   *
   * Divide each component of this matrix by the given attribute.
   */
  ShMatrix& operator/=(const ShGeneric<1, V>& a);

  /**@name Swizzling
   * Retrieve some set of rows from this matrix. These must be
   * swizzled again to operate on them.
   */
  //@{
  ShMatrixRows<Rows, Cols, V> operator()() const; ///< Identity swizzle
  ShMatrixRows<1, Cols, V> operator()(int) const;
  ShMatrixRows<2, Cols, V> operator()(int, int) const;
  ShMatrixRows<3, Cols, V> operator()(int, int, int) const;
  ShMatrixRows<4, Cols, V> operator()(int, int, int, int) const;
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
  void range(H low, H high);

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
  ShAttrib<Cols, Binding, V> m_data[Rows];
  
};
/** \brief Matrix output operator
 *
 * Print a representation of the given (uniform or constant) matrix's
 * values to the given stream.
 */

template<int R, int C, ShBindingType B, ShValueType Vy>
std::ostream& operator<<(std::ostream& out,
                         const ShMatrix<R, C, B, Vy>& m);

/** A few rows from a matrix.
 * This is an intermediate structure representing some rows that have
 * just been swizzled from a matrix. The only useful operation is to
 * swizzle these rows again to obtain a submatrix with the given rows
 * and columns.
 */
template<int Rows, int Cols, ShValueType V>
class ShMatrixRows {
public:
  template<ShBindingType Binding>
  ShMatrixRows(const ShMatrix<Rows, Cols, Binding, V>& source);
  
  template<int OR, ShBindingType Binding>
  ShMatrixRows(const ShMatrix<OR, Cols, Binding, V>& source,
               int idx0);
  template<int OR, ShBindingType Binding>
  ShMatrixRows(const ShMatrix<OR, Cols, Binding, V>& source,
               int idx0, int idx1);
  template<int OR, ShBindingType Binding>
  ShMatrixRows(const ShMatrix<OR, Cols, Binding, V>& source,
               int idx0, int idx1, int idx2);
  template<int OR, ShBindingType Binding>
  ShMatrixRows(const ShMatrix<OR, Cols, Binding, V>& source,
               int idx0, int idx1, int idx2, int idx3);

  ShMatrixRows(const ShMatrixRows<Rows, Cols, V>& other);
  
  ShMatrixRows& operator=(const ShMatrixRows<Rows, Cols, V>& other);

  /**@name Swizzling
   * Retrieve a matrix consisting of these rows and the given columns.
   */
  //@{
  ShMatrix<Rows, Cols, SH_TEMP, V> operator()() const; ///< Identity swizzle
  ShMatrix<Rows, 1, SH_TEMP, V> operator()(int) const;
  ShMatrix<Rows, 2, SH_TEMP, V> operator()(int, int) const;
  ShMatrix<Rows, 3, SH_TEMP, V> operator()(int, int, int) const;
  ShMatrix<Rows, 4, SH_TEMP, V> operator()(int, int, int, int) const;
  //@}
private:
  ShAttrib<Cols, SH_TEMP, V> m_data[Rows];
};


typedef ShMatrix<1, 1, SH_INPUT, SH_FLOAT> ShInputMatrix1x1f;
typedef ShMatrix<1, 1, SH_OUTPUT, SH_FLOAT> ShOutputMatrix1x1f;
typedef ShMatrix<1, 1, SH_INOUT, SH_FLOAT> ShInOutMatrix1x1f;
typedef ShMatrix<1, 1, SH_TEMP, SH_FLOAT> ShMatrix1x1f;
typedef ShMatrix<1, 2, SH_INPUT, SH_FLOAT> ShInputMatrix1x2f;
typedef ShMatrix<1, 2, SH_OUTPUT, SH_FLOAT> ShOutputMatrix1x2f;
typedef ShMatrix<1, 2, SH_INOUT, SH_FLOAT> ShInOutMatrix1x2f;
typedef ShMatrix<1, 2, SH_TEMP, SH_FLOAT> ShMatrix1x2f;
typedef ShMatrix<1, 3, SH_INPUT, SH_FLOAT> ShInputMatrix1x3f;
typedef ShMatrix<1, 3, SH_OUTPUT, SH_FLOAT> ShOutputMatrix1x3f;
typedef ShMatrix<1, 3, SH_INOUT, SH_FLOAT> ShInOutMatrix1x3f;
typedef ShMatrix<1, 3, SH_TEMP, SH_FLOAT> ShMatrix1x3f;
typedef ShMatrix<1, 4, SH_INPUT, SH_FLOAT> ShInputMatrix1x4f;
typedef ShMatrix<1, 4, SH_OUTPUT, SH_FLOAT> ShOutputMatrix1x4f;
typedef ShMatrix<1, 4, SH_INOUT, SH_FLOAT> ShInOutMatrix1x4f;
typedef ShMatrix<1, 4, SH_TEMP, SH_FLOAT> ShMatrix1x4f;

typedef ShMatrix<2, 1, SH_INPUT, SH_FLOAT> ShInputMatrix2x1f;
typedef ShMatrix<2, 1, SH_OUTPUT, SH_FLOAT> ShOutputMatrix2x1f;
typedef ShMatrix<2, 1, SH_INOUT, SH_FLOAT> ShInOutMatrix2x1f;
typedef ShMatrix<2, 1, SH_TEMP, SH_FLOAT> ShMatrix2x1f;
typedef ShMatrix<2, 2, SH_INPUT, SH_FLOAT> ShInputMatrix2x2f;
typedef ShMatrix<2, 2, SH_OUTPUT, SH_FLOAT> ShOutputMatrix2x2f;
typedef ShMatrix<2, 2, SH_INOUT, SH_FLOAT> ShInOutMatrix2x2f;
typedef ShMatrix<2, 2, SH_TEMP, SH_FLOAT> ShMatrix2x2f;
typedef ShMatrix<2, 3, SH_INPUT, SH_FLOAT> ShInputMatrix2x3f;
typedef ShMatrix<2, 3, SH_OUTPUT, SH_FLOAT> ShOutputMatrix2x3f;
typedef ShMatrix<2, 3, SH_INOUT, SH_FLOAT> ShInOutMatrix2x3f;
typedef ShMatrix<2, 3, SH_TEMP, SH_FLOAT> ShMatrix2x3f;
typedef ShMatrix<2, 4, SH_INPUT, SH_FLOAT> ShInputMatrix2x4f;
typedef ShMatrix<2, 4, SH_OUTPUT, SH_FLOAT> ShOutputMatrix2x4f;
typedef ShMatrix<2, 4, SH_INOUT, SH_FLOAT> ShInOutMatrix2x4f;
typedef ShMatrix<2, 4, SH_TEMP, SH_FLOAT> ShMatrix2x4f;

typedef ShMatrix<3, 1, SH_INPUT, SH_FLOAT> ShInputMatrix3x1f;
typedef ShMatrix<3, 1, SH_OUTPUT, SH_FLOAT> ShOutputMatrix3x1f;
typedef ShMatrix<3, 1, SH_INOUT, SH_FLOAT> ShInOutMatrix3x1f;
typedef ShMatrix<3, 1, SH_TEMP, SH_FLOAT> ShMatrix3x1f;
typedef ShMatrix<3, 2, SH_INPUT, SH_FLOAT> ShInputMatrix3x2f;
typedef ShMatrix<3, 2, SH_OUTPUT, SH_FLOAT> ShOutputMatrix3x2f;
typedef ShMatrix<3, 2, SH_INOUT, SH_FLOAT> ShInOutMatrix3x2f;
typedef ShMatrix<3, 2, SH_TEMP, SH_FLOAT> ShMatrix3x2f;
typedef ShMatrix<3, 3, SH_INPUT, SH_FLOAT> ShInputMatrix3x3f;
typedef ShMatrix<3, 3, SH_OUTPUT, SH_FLOAT> ShOutputMatrix3x3f;
typedef ShMatrix<3, 3, SH_INOUT, SH_FLOAT> ShInOutMatrix3x3f;
typedef ShMatrix<3, 3, SH_TEMP, SH_FLOAT> ShMatrix3x3f;
typedef ShMatrix<3, 4, SH_INPUT, SH_FLOAT> ShInputMatrix3x4f;
typedef ShMatrix<3, 4, SH_OUTPUT, SH_FLOAT> ShOutputMatrix3x4f;
typedef ShMatrix<3, 4, SH_INOUT, SH_FLOAT> ShInOutMatrix3x4f;
typedef ShMatrix<3, 4, SH_TEMP, SH_FLOAT> ShMatrix3x4f;

typedef ShMatrix<4, 1, SH_INPUT, SH_FLOAT> ShInputMatrix4x1f;
typedef ShMatrix<4, 1, SH_OUTPUT, SH_FLOAT> ShOutputMatrix4x1f;
typedef ShMatrix<4, 1, SH_INOUT, SH_FLOAT> ShInOutMatrix4x1f;
typedef ShMatrix<4, 1, SH_TEMP, SH_FLOAT> ShMatrix4x1f;
typedef ShMatrix<4, 2, SH_INPUT, SH_FLOAT> ShInputMatrix4x2f;
typedef ShMatrix<4, 2, SH_OUTPUT, SH_FLOAT> ShOutputMatrix4x2f;
typedef ShMatrix<4, 2, SH_INOUT, SH_FLOAT> ShInOutMatrix4x2f;
typedef ShMatrix<4, 2, SH_TEMP, SH_FLOAT> ShMatrix4x2f;
typedef ShMatrix<4, 3, SH_INPUT, SH_FLOAT> ShInputMatrix4x3f;
typedef ShMatrix<4, 3, SH_OUTPUT, SH_FLOAT> ShOutputMatrix4x3f;
typedef ShMatrix<4, 3, SH_INOUT, SH_FLOAT> ShInOutMatrix4x3f;
typedef ShMatrix<4, 3, SH_TEMP, SH_FLOAT> ShMatrix4x3f;
typedef ShMatrix<4, 4, SH_INPUT, SH_FLOAT> ShInputMatrix4x4f;
typedef ShMatrix<4, 4, SH_OUTPUT, SH_FLOAT> ShOutputMatrix4x4f;
typedef ShMatrix<4, 4, SH_INOUT, SH_FLOAT> ShInOutMatrix4x4f;
typedef ShMatrix<4, 4, SH_TEMP, SH_FLOAT> ShMatrix4x4f;

}

#include "ShMatrixImpl.hpp"

#endif

