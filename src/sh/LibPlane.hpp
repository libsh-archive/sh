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
#ifndef SHLIBPLANE_HPP
#define SHLIBPLANE_HPP

#include "Attrib.hpp"
#include "Lib.hpp"

namespace SH {
SHLIB_USUAL_OPERATIONS(SH_PLANE);
SHLIB_USUAL_SUBTRACT(SH_PLANE);

SHLIB_LEFT_MATRIX_OPERATION(SH_PLANE, operator|, M);

}

#endif
