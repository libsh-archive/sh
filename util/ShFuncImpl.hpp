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
#ifndef SHUTIL_FUNCIMPL_HPP 
#define SHUTIL_FUNCIMPL_HPP 

#include <cmath>
#include <numeric>
#include "sh/ShAttrib.hpp"
#include "sh/ShSwizzle.hpp" 
#include "sh/ShVariable.hpp"
#include "sh/ShLib.hpp"
#include "ShFunc.hpp"

namespace ShUtil {

using namespace SH;

/** \brief Given 3 orthonormal basis vectors b0, b1, b2, specified relative to a coordinate space C, 
 * this does a change of basis on a vector v in space C to the orthonormal basis
 */
template<typename T>
ShGeneric<3, T> changeBasis(const ShGeneric<3, T> &b0, const ShGeneric<3, T> &b1,
                            const ShGeneric<3, T> &b2, const ShGeneric<3, T> &v)
{
  ShAttrib<3, SH_TEMP, T> result;
  result(0) = b0 | v;
  result(1) = b1 | v;
  result(2) = b2 | v;
  return result;
}

}

#endif // SHUTIL_FUNCIMPL_HPP 
