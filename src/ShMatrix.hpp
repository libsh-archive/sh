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

template<int Rows, int Cols, typename T>
class ShMatrixRows;

/** A matrix of Rows by Cols elements.
 */
template<int Rows, int Cols, int Kind, typename T>
class ShMatrix {
public:
  ShMatrix();

  ShMatrix(const ShMatrix<Rows, Cols, Kind, T>& other);
  template<int Kind2>
  ShMatrix(const ShMatrix<Rows, Cols, Kind2, T>& other);
  
  ~ShMatrix();

  ShMatrix& operator=(const ShMatrix<Rows, Cols, Kind, T>& other);
  template<int Kind2>
  ShMatrix& operator=(const ShMatrix<Rows, Cols, Kind2, T>& other);

  /// Fetch a row from this matrix.
  ShAttrib<Cols, Kind, T>& operator[](int i);
  const ShAttrib<Cols, Kind, T>& operator[](int i) const;

  template<int Kind2>
  ShMatrix& operator+=(const ShMatrix<Rows, Cols, Kind2, T>& other);

  template<int Kind2>
  ShMatrix& operator-=(const ShMatrix<Rows, Cols, Kind2, T>& other);
  template<int Kind2>
  ShMatrix& operator*=(const ShMatrix<Rows, Cols, Kind2, T>& other);
  template<int Kind2>
  ShMatrix& operator/=(const ShMatrix<Rows, Cols, Kind2, T>& other);

  /// Scalar multiplication.
  ShMatrix& operator*=(const ShVariableN<1, T>& other);
  /// Scalar division.
  ShMatrix& operator/=(const ShVariableN<1, T>& other);

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
  
private:
  ShAttrib<Cols, Kind, T> m_data[Rows];
};

/** A few rows from a matrix.
 * This is an intermediate structure representing some rows that have
 * just been swizzled from a matrix. The only useful operation is to
 * swizzle these rows again to obtain a submatrix with the given rows
 * and columns.
 */
template<int Rows, int Cols, typename T>
class ShMatrixRows {
public:
  template<int Kind>
  ShMatrixRows(const ShMatrix<Rows, Cols, Kind, T>& source);
  
  template<int OR, int Kind>
  ShMatrixRows(const ShMatrix<OR, Cols, Kind, T>& source,
               int idx0);
  template<int OR, int Kind>
  ShMatrixRows(const ShMatrix<OR, Cols, Kind, T>& source,
               int idx0, int idx1);
  template<int OR, int Kind>
  ShMatrixRows(const ShMatrix<OR, Cols, Kind, T>& source,
               int idx0, int idx1, int idx2);
  template<int OR, int Kind>
  ShMatrixRows(const ShMatrix<OR, Cols, Kind, T>& source,
               int idx0, int idx1, int idx2, int idx3);

  ShMatrixRows(const ShMatrixRows<Rows, Cols, T>& other);
  
  ShMatrixRows& operator=(const ShMatrixRows<Rows, Cols, T>& other);

  /**@name Swizzling
   * Retrieve a matrix consisting of these rows and the given columns.
   */
  //@{
  ShMatrix<Rows, Cols, SH_VAR_TEMP, T> operator()() const; ///< Identity swizzle
  ShMatrix<Rows, 1, SH_VAR_TEMP, T> operator()(int) const;
  ShMatrix<Rows, 2, SH_VAR_TEMP, T> operator()(int, int) const;
  ShMatrix<Rows, 3, SH_VAR_TEMP, T> operator()(int, int, int) const;
  ShMatrix<Rows, 4, SH_VAR_TEMP, T> operator()(int, int, int, int) const;
  //@}
private:
  ShAttrib<Cols, SH_VAR_TEMP, T> m_data[Rows];
};

typedef ShMatrix<1, 1, SH_VAR_INPUT, double> ShInputMatrix1x1f;
typedef ShMatrix<1, 1, SH_VAR_OUTPUT, double> ShOutputMatrix1x1f;
typedef ShMatrix<1, 1, SH_VAR_TEMP, double> ShMatrix1x1f;
typedef ShMatrix<1, 2, SH_VAR_INPUT, double> ShInputMatrix1x2f;
typedef ShMatrix<1, 2, SH_VAR_OUTPUT, double> ShOutputMatrix1x2f;
typedef ShMatrix<1, 2, SH_VAR_TEMP, double> ShMatrix1x2f;
typedef ShMatrix<1, 3, SH_VAR_INPUT, double> ShInputMatrix1x3f;
typedef ShMatrix<1, 3, SH_VAR_OUTPUT, double> ShOutputMatrix1x3f;
typedef ShMatrix<1, 3, SH_VAR_TEMP, double> ShMatrix1x3f;
typedef ShMatrix<1, 4, SH_VAR_INPUT, double> ShInputMatrix1x4f;
typedef ShMatrix<1, 4, SH_VAR_OUTPUT, double> ShOutputMatrix1x4f;
typedef ShMatrix<1, 4, SH_VAR_TEMP, double> ShMatrix1x4f;

typedef ShMatrix<2, 1, SH_VAR_INPUT, double> ShInputMatrix2x1f;
typedef ShMatrix<2, 1, SH_VAR_OUTPUT, double> ShOutputMatrix2x1f;
typedef ShMatrix<2, 1, SH_VAR_TEMP, double> ShMatrix2x1f;
typedef ShMatrix<2, 2, SH_VAR_INPUT, double> ShInputMatrix2x2f;
typedef ShMatrix<2, 2, SH_VAR_OUTPUT, double> ShOutputMatrix2x2f;
typedef ShMatrix<2, 2, SH_VAR_TEMP, double> ShMatrix2x2f;
typedef ShMatrix<2, 3, SH_VAR_INPUT, double> ShInputMatrix2x3f;
typedef ShMatrix<2, 3, SH_VAR_OUTPUT, double> ShOutputMatrix2x3f;
typedef ShMatrix<2, 3, SH_VAR_TEMP, double> ShMatrix2x3f;
typedef ShMatrix<2, 4, SH_VAR_INPUT, double> ShInputMatrix2x4f;
typedef ShMatrix<2, 4, SH_VAR_OUTPUT, double> ShOutputMatrix2x4f;
typedef ShMatrix<2, 4, SH_VAR_TEMP, double> ShMatrix2x4f;

typedef ShMatrix<3, 1, SH_VAR_INPUT, double> ShInputMatrix3x1f;
typedef ShMatrix<3, 1, SH_VAR_OUTPUT, double> ShOutputMatrix3x1f;
typedef ShMatrix<3, 1, SH_VAR_TEMP, double> ShMatrix3x1f;
typedef ShMatrix<3, 2, SH_VAR_INPUT, double> ShInputMatrix3x2f;
typedef ShMatrix<3, 2, SH_VAR_OUTPUT, double> ShOutputMatrix3x2f;
typedef ShMatrix<3, 2, SH_VAR_TEMP, double> ShMatrix3x2f;
typedef ShMatrix<3, 3, SH_VAR_INPUT, double> ShInputMatrix3x3f;
typedef ShMatrix<3, 3, SH_VAR_OUTPUT, double> ShOutputMatrix3x3f;
typedef ShMatrix<3, 3, SH_VAR_TEMP, double> ShMatrix3x3f;
typedef ShMatrix<3, 4, SH_VAR_INPUT, double> ShInputMatrix3x4f;
typedef ShMatrix<3, 4, SH_VAR_OUTPUT, double> ShOutputMatrix3x4f;
typedef ShMatrix<3, 4, SH_VAR_TEMP, double> ShMatrix3x4f;

typedef ShMatrix<4, 1, SH_VAR_INPUT, double> ShInputMatrix4x1f;
typedef ShMatrix<4, 1, SH_VAR_OUTPUT, double> ShOutputMatrix4x1f;
typedef ShMatrix<4, 1, SH_VAR_TEMP, double> ShMatrix4x1f;
typedef ShMatrix<4, 2, SH_VAR_INPUT, double> ShInputMatrix4x2f;
typedef ShMatrix<4, 2, SH_VAR_OUTPUT, double> ShOutputMatrix4x2f;
typedef ShMatrix<4, 2, SH_VAR_TEMP, double> ShMatrix4x2f;
typedef ShMatrix<4, 3, SH_VAR_INPUT, double> ShInputMatrix4x3f;
typedef ShMatrix<4, 3, SH_VAR_OUTPUT, double> ShOutputMatrix4x3f;
typedef ShMatrix<4, 3, SH_VAR_TEMP, double> ShMatrix4x3f;
typedef ShMatrix<4, 4, SH_VAR_INPUT, double> ShInputMatrix4x4f;
typedef ShMatrix<4, 4, SH_VAR_OUTPUT, double> ShOutputMatrix4x4f;
typedef ShMatrix<4, 4, SH_VAR_TEMP, double> ShMatrix4x4f;

}

#include "ShMatrixImpl.hpp"

#endif
