// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Bryan Chan 
//          
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
#ifndef SHLA_MATRIX_HPP 
#define SHLA_MATRIX_HPP 

#include <map>
#include "ShlaVector.hpp"

namespace Shla {

using namespace SH;

/** \file ShlaMatrix.hpp
 * This is an implementation of a square matrix with M*N rows and
 * M*N columns.
 */

#if 0
template<typename T, int M, int N> 
class ShlaMatrix
{
  public:
    /** \brief Constructor for ShlaMatrix
     */
    ShlaMatrix();

    /** \brief Destructor for ShPerlin.
     * Destroys a Perlin noise generator.
     */
    virtual ~ShlaMatrix();

// functions that work on the GPU
    template<typename VecT>
    virtual ShlaVector<VecT, M, N> operator|( const ShlaVector<VecT, M, N> &v ) const = 0; 

// functions that work on memory on the host 
// (eventually might work directly on elements in the GPU)
    // TODO explain i, element (element is used when vector holds vector data)
    // TODO change float to T::ValueType (requires some thought, as all the Sh
    // texture stuff is hardcoded to use float currently)
    virtual const float& operator()( int r, int c, int element ) const = 0; 
    virtual float& operator()( int r, int c, int element ) = 0;
};

#endif


// TODO do not allow copy constructor/assignment op on Matrices 

template<typename T, int M, int N>
class ShlaDenseMatrix  {
  public:
    /** \brief Constructs a ShlaDenseMatrix of all zero entries
     * with N*N rows and N*N columns
     */
    ShlaDenseMatrix();

    /** \brief Destroys this ShlaDenseMatrix.
     */
    ~ShlaDenseMatrix();

    template<typename VecT>
    ShlaVector<VecT, M, N> operator|( const ShlaVector<VecT, M, N> &v ) const; 

    const float& operator()( int r, int c, int element ) const;
    float& operator()( int r, int c, int element );

    // extract one column of the matrix
    const ShlaVector<T, M, N> &operator[]( int c ) const; 
    ShlaVector<T, M, N> &operator[]( int c ); 

  private:
    ShlaVector<T, M, N> m_column[M * N];
};

template<typename T, int M, int N>
class ShlaBandedMatrix {
  public:
    /** \brief Constructs an empty banded matrix.
     * The ith diagonal that holds matrix entries (i, (d+i) % N^2 )
     * for 0 <= i < N^2 is not created until either the ()
     * or [] operators are used to access it.
     */
    ShlaBandedMatrix();

    /** \brief Destroys a ShlaBandedMatrix
     */
    ~ShlaBandedMatrix();

    template<typename VecT>
    ShlaVector<VecT, M, N> operator|( const ShlaVector<VecT, M, N> &v ) const; 

    const float& operator()( int r, int c, int element ) const;
    float& operator()( int r, int c, int element );

    /** \brief Extract one diagonal of the matrix
      * The dth diagonal contains the matrix entries (i, (d+i)%(N*N))
      * in sequence from i = 0 to N * N.
      */
    const ShlaVector<T, M, N>& operator[]( int d ) const; 
    ShlaVector<T, M, N>& operator[]( int d ); 

  private:
    typedef std::map< int, ShlaVector<T, M, N> > DiagonalMap;
    DiagonalMap m_diagonal;
};

}

#include "ShlaMatrixImpl.hpp"

#endif
