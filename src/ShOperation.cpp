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
#include <iostream>
#include "ShOperation.hpp"

namespace SH {

/** Update this if you add or change operations in ShOperation.
 * @see ShOperation
 */
const ShOperationInfo opInfo[] = {
  {"ASN", 1},
  
  {"NEG", 1},
  {"ADD", 2},
  {"MUL", 2},
  {"DIV", 2},

  {"SLT", 2},
  {"SLE", 2},
  {"SGT", 2},
  {"SGE", 2},
  {"SEQ", 2},
  {"SNE", 2},
  
  {"ABS", 1},
  {"ACOS", 1},
  {"ASIN", 1},
  {"ATAN", 1},
  {"ATAN2", 2},
  {"CEIL", 1},
  {"COS", 1},
  {"DOT", 2},
  {"DX", 1},
  {"DY", 1},
  {"EXP", 1},
  {"EXP2", 1},
  {"EXP10", 1},
  {"FLR", 1},
  {"FRAC", 1},
  {"LRP", 3},
  {"MAD", 3},
  {"MAX", 2},
  {"MIN", 2},
  {"MOD", 2},
  {"LOG", 1},
  {"LOG2", 1},
  {"LOG10", 1},
  {"POW", 2},
  {"RCP", 1},
  {"RSQ", 1},
  {"SIN", 1},
  {"SGN", 1},
  {"SQRT", 1},
  {"TAN", 1},

  {"NORM", 1},
  {"XPD", 2},

  {"TEX", 2},
  {"TEXI", 2},

  {"COND", 3},

  {"KIL", 1},

  {"OPTBRA", 1},

  {"FETCH", 1},
  
  {0, 0}
};


} // namespace SH

