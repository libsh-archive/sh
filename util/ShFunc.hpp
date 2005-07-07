// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#ifndef SHUTIL_FUNC_HPP 
#define SHUTIL_FUNC_HPP 

#include "ShVariable.hpp"
#include "ShAttrib.hpp"

#ifndef WIN32
namespace ShUtil {

using namespace SH;

/** \file ShFunc.hpp
 * \brief Miscellaneous small Sh utility functions.  
 */

/** \brief Cubic interpolated step between 0 and 1. 
 * Returns 0 if x < a,
 * 1 if x > b, 
 * cubic interpolation between 0 and 1 otherwise
 */
template<int N, typename T>
ShGeneric<N, T> deprecated_smoothstep(const ShGeneric<N, T>& a, const ShGeneric<N, T>& b,
    const ShGeneric<N, T> x); 

/** \brief Parallel linear congruential generator
 * 
 * This does not work very well right now.  Use hashmrg instead.
 *
 * \sa  template<int N, typename T> ShGeneric<N, T> hashmrg(const ShGeneric<N, T>& p)
 */
// TODO: may not work as intended on 24-bit floats
// since there may not be enough precision 
template<int N, typename T>
ShGeneric<N, T> hashlcg(const ShGeneric<N, T>& p); 

/** \brief MRG style pseudorandom vector generator
 *
 * Generates a random vector using a multiple-recursive generator style. (LCG on steroids)
 * Treat x,y,z,w as seeds a0, a1, a2, a3
 * and repeatedly apply an = b * (an-1, an-2, an-3, an-4), where b is a vector
 * Take as output (an, an-1, an-2, an3) after suitable number of iterations.
 *
 * This appears to reduce correlation in the output components when input components are 
 * similar, but the behaviour needs to be studied further.
 *
 * \sa template<int N, typename T> ShGeneric<N, T> hashlcg(const ShGeneric<N, T>& p)
 */
template<int N, typename T>
ShGeneric<N, T> hashmrg(const ShGeneric<N, T>& p); 

/** \brief Given orthonormal basis b0, b1, b2 and vector v relative to coordinate space C,
 * does change of basis on v to the orthonormal basis b0, b1, b2
 */
template<typename T>
ShGeneric<3, T> changeBasis(const ShGeneric<3, T> &b0, 
    const ShGeneric<3, T> &b1, const ShGeneric<3, T> &b2, const ShGeneric<3, T> &v); 

}
#endif

#include "ShFuncImpl.hpp"

#endif // SHUTIL_FUNC_HPP 
