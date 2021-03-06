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
#ifndef SHUTIL_FUNC_HPP 
#define SHUTIL_FUNC_HPP 

#include "sh/Variable.hpp"
#include "sh/Attrib.hpp"

#ifndef _WIN32
namespace ShUtil {

using namespace SH;

/** \file Func.hpp
 * \brief Miscellaneous small Sh utility functions.  
 */

/** \brief Given orthonormal basis b0, b1, b2 and vector v relative to coordinate space C,
 * does change of basis on v to the orthonormal basis b0, b1, b2
 */
template<typename T>
Generic<3, T> changeBasis(const Generic<3, T> &b0, const Generic<3, T> &b1,
                            const Generic<3, T> &b2, const Generic<3, T> &v); 

} // namespace
#endif

#include "FuncImpl.hpp"

#endif // SHUTIL_FUNC_HPP 
