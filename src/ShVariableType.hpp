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
#ifndef SHVARIABLETYPE_HPP
#define SHVARIABLETYPE_HPP

namespace SH {

/** The various ways variables can be bound.
 */
enum ShBindingType {
  SH_INPUT = 0,
  SH_OUTPUT = 1,
  SH_INOUT = 2,
  SH_TEMP = 3,
  SH_CONST = 4,
  SH_TEXTURE = 5,
  SH_STREAM = 6
};

/** The various ways semantic types for variables.
 */
enum ShSemanticType {
  SH_ATTRIB,
  SH_POINT,
  SH_VECTOR,
  SH_NORMAL,
  SH_COLOR,
  SH_TEXCOORD,
  SH_POSITION
};

}
#endif
