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
#ifndef SHLIBTEXCOORD_HPP
#define SHLIBTEXCOORD_HPP

#include "ShTexCoord.hpp"
#include "ShLib.hpp"

namespace SH {

SH_SHLIB_BINARY_OPERATION(ShTexCoord, operator+, N);
SH_SHLIB_BINARY_OPERATION(ShTexCoord, operator-, N);
SH_SHLIB_UNEQ_BINARY_OPERATION(ShTexCoord, operator*, N);

SH_SHLIB_LEFT_SCALAR_OPERATION(ShTexCoord, operator*);

SH_SHLIB_UNEQ_BINARY_OPERATION(ShTexCoord, operator/, N);
SH_SHLIB_BINARY_OPERATION(ShTexCoord, operator^, N);
SH_SHLIB_BINARY_OPERATION(ShTexCoord, operator<, N);
SH_SHLIB_BINARY_OPERATION(ShTexCoord, operator<=, N);
SH_SHLIB_BINARY_OPERATION(ShTexCoord, operator>, N);
SH_SHLIB_BINARY_OPERATION(ShTexCoord, operator>=, N);
SH_SHLIB_BINARY_OPERATION(ShTexCoord, operator==, N);
SH_SHLIB_BINARY_OPERATION(ShTexCoord, operator!=, N);

SH_SHLIB_UNARY_OPERATION(ShTexCoord, abs, N);
SH_SHLIB_UNARY_OPERATION(ShTexCoord, acos, N);
SH_SHLIB_UNARY_OPERATION(ShTexCoord, asin, N);
SH_SHLIB_UNARY_OPERATION(ShTexCoord, cos, N);
SH_SHLIB_UNARY_OPERATION(ShTexCoord, frac, N);
SH_SHLIB_UNARY_OPERATION(ShTexCoord, sin, N);
SH_SHLIB_UNARY_OPERATION(ShTexCoord, sqrt, N);
SH_SHLIB_UNARY_OPERATION(ShTexCoord, normalize, N);

SH_SHLIB_LEFT_MATRIX_OPERATION(ShTexCoord, operator|, M);

}

#endif
