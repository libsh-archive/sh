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
#ifndef SHLA_VECTOR_HPP 
#define SHLA_VECTOR_HPP 

#include "ShMemoryObject.hpp"
#include "ShUberbuffer.hpp"

namespace Shla {

using namespace SH;

/** \file ShlaVector.hpp
 * This is an implementation of vector. 
 */

/** \brief A vector with M * N elements of type T stored in a 
 * 2D texture of width M and height N. T must be a subclass of 
 * ShAttrib.
 *
 * This class keeps an associated ShTexture2D that automatically changes 
 * data between a ShDataMemoryObject and ShUberBuffer. 
 *
 * The ith element of the vector (0 <= i < M * N) is stored in 
 * 2D location (i % M, i / M)
 */
template<typename T, int M, int N>
class ShlaVector
{
  public:
    /** \brief Constructor for ShlaVector
     * Creates a vector containing undefined data of length M * N. 
     */
    ShlaVector();
    
    /** \brief Copy constructor
     * Makes a copy of b's data/memory object
     */
    ShlaVector( const ShlaVector<T, M, N> &b );

    /** \brief Destructor for ShPerlin.
     * Destroys a Perlin noise generator.
     */
    ~ShlaVector();

    /** \brief assignment operator
     * Makes a copy of b's data/memory object
     */
    ShlaVector<T, M, N>& operator=( const ShlaVector<T, M, N> &b );

    /** \brief Returns a memory object for the vector
     */
    ShUberbufferPtr getMem() const;

    // TODO explain i, element (element is used when vector holds vector data)
    // TODO change float to T::ValueType (requires some thought, as all the Sh
    // texture stuff is hardcoded to use float currently)
    T operator()( int i ) const; 

    // function to set data from a floating point array
    void setData( const float* data ); 
    void setData( int w, int h, const T& elm );

    // function sets data to a zeroed floating point array
    void zeroData();

  private:
    ShUberbufferPtr m_mem;

    static float* zeros;
};

}

#include "ShlaVectorImpl.hpp"

#endif
