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
#ifndef SHLIBPOINT_HPP
#define SHLIBPOINT_HPP

#include "ShAttrib.hpp"
#include "ShLib.hpp"

namespace SH {

SH_SHLIB_USUAL_OPERATIONS(SH_POINT)
SH_SHLIB_BINARY_RETTYPE_OPERATION(SH_POINT, operator-, SH_VECTOR, N)
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(SH_POINT, operator-, SH_VECTOR, 1)

SH_SHLIB_LEFT_MATRIX_OPERATION(SH_POINT, operator|, M)

}

#endif
