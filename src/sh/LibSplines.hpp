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
#include "Lib.hpp" // LibSplines needs to be included from within Lib

#ifndef SHLIBSPLINES_HPP
#define SHLIBSPLINES_HPP

#include "Generic.hpp"

#ifndef _WIN32
namespace SH {

/** \defgroup lib_splines Spline functions
 * @ingroup library
 * @{
 */

/** Evaluate the bernstein (Bezier) basis functions of order N at
 * parameter a. */
template<int N, typename T>
Generic<N, T> bernstein(const Generic<1, T>& a);

/** Evaluate the cubic Bezier spline.
 */
template <int N, typename T>
Generic<N, T> bezier(const Generic<1, T>& t, const Generic<N, T>& p);

/** Evaluate the cubic Hermite spline at parameter a interpolating b
 *  at 0 with tangent c and d at 1 with tangent e. */
template <int N, typename T>
Generic<N, T> hermite(const Generic<1, T>& a, const Generic<N, T>& b, 
                        const Generic<N, T>& c, const Generic<N, T>& d, 
                        const Generic<N, T>& e);

/*@}*/

}
#endif

#include "LibSplinesImpl.hpp"

#endif
