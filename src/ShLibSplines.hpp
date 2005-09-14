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
#include "ShLib.hpp" // ShLibSplines needs to be included from within ShLib

#ifndef SHLIBSPLINES_HPP
#define SHLIBSPLINES_HPP

#include "ShGeneric.hpp"

#ifndef WIN32
namespace SH {

/** \defgroup lib_splines Spline functions
 * @ingroup library
 * @{
 */

/** Evaluation of the bernstein (Bezier) basis functions of order N at
 * parameter a. */
template<int N, typename T>
ShGeneric<N, T> bernstein(const ShGeneric<1, T>& a);

/*@}*/

}
#endif

#include "ShLibSplinesImpl.hpp"

#endif
