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
#ifndef SHTEXCOORD_HPP
#define SHTEXCOORD_HPP

#include "ShAttrib.hpp"
#include "ShSpecialType.hpp"

namespace SH {

/** A texture coordinate with N dimensions.
 */
SH_SPECIAL_TYPE(ShTexCoord, texture coordinate, SH_VAR_TEXCOORD);

SH_SPECIAL_TYPE_TYPEDEFS(TexCoord);

SH_SPECIAL_TYPE_IMPL(ShTexCoord, SH_VAR_TEXCOORD);

}

#endif
