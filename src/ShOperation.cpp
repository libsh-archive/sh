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
  {"ASN", 1, ShOperationInfo::LINEAR, false},
  
  {"NEG", 1, ShOperationInfo::LINEAR, false},
  {"ADD", 2, ShOperationInfo::LINEAR, true},
  {"MUL", 2, ShOperationInfo::LINEAR, true},
  {"DIV", 2, ShOperationInfo::LINEAR, false},

  {"SLT", 2, ShOperationInfo::LINEAR, false},
  {"SLE", 2, ShOperationInfo::LINEAR, false},
  {"SGT", 2, ShOperationInfo::LINEAR, false},
  {"SGE", 2, ShOperationInfo::LINEAR, false},
  {"SEQ", 2, ShOperationInfo::LINEAR, false},
  {"SNE", 2, ShOperationInfo::LINEAR, false},
  
  {"ABS", 1, ShOperationInfo::LINEAR, false},
  {"ACOS", 1, ShOperationInfo::LINEAR, false},
  {"ASIN", 1, ShOperationInfo::LINEAR, false},
  {"ATAN", 1, ShOperationInfo::LINEAR, false},
  {"ATAN2", 2, ShOperationInfo::LINEAR, false},
  {"CBRT", 1, ShOperationInfo::LINEAR, false},
  {"CEIL", 1, ShOperationInfo::LINEAR, false},
  {"COS", 1, ShOperationInfo::LINEAR, false},
  {"CMUL", 1, ShOperationInfo::ALL, false},
  {"CSUM", 1, ShOperationInfo::ALL, false},
  {"DOT", 2, ShOperationInfo::ALL, true},
  {"DX", 1, ShOperationInfo::EXTERNAL, false},
  {"DY", 1, ShOperationInfo::EXTERNAL, false},
  {"EXP", 1, ShOperationInfo::LINEAR, false},
  {"EXP2", 1, ShOperationInfo::LINEAR, false},
  {"EXP10", 1, ShOperationInfo::LINEAR, false},
  {"FLR", 1, ShOperationInfo::LINEAR, false},
  {"FRAC", 1, ShOperationInfo::LINEAR, false},
  {"LRP", 3, ShOperationInfo::LINEAR, false},
  {"MAD", 3, ShOperationInfo::LINEAR, false},
  {"MAX", 2, ShOperationInfo::LINEAR, false},
  {"MIN", 2, ShOperationInfo::LINEAR, true},
  {"MOD", 2, ShOperationInfo::LINEAR, true},
  {"LOG", 1, ShOperationInfo::LINEAR, false},
  {"LOG2", 1, ShOperationInfo::LINEAR, false},
  {"LOG10", 1, ShOperationInfo::LINEAR, false},
  {"POW", 2, ShOperationInfo::LINEAR, false},
  {"RCP", 1, ShOperationInfo::LINEAR, false},
  {"RND", 1, ShOperationInfo::LINEAR, false},
  {"RSQ", 1, ShOperationInfo::LINEAR, false},
  {"SIN", 1, ShOperationInfo::LINEAR, false},
  {"SGN", 1, ShOperationInfo::LINEAR, false},
  {"SQRT", 1, ShOperationInfo::LINEAR, false},
  {"TAN", 1, ShOperationInfo::LINEAR, false},

  {"NORM", 1, ShOperationInfo::ALL, false},
  {"XPD", 2, ShOperationInfo::ALL, false}, // Not quite true, but probably good enough

  {"TEX", 2, ShOperationInfo::EXTERNAL, false},
  {"TEXI", 2, ShOperationInfo::EXTERNAL, false},
  {"TEXD", 3, ShOperationInfo::EXTERNAL, false},

  {"COND", 3, ShOperationInfo::LINEAR, false},

  {"KIL", 1, ShOperationInfo::IGNORE, false},

  {"OPTBRA", 1, ShOperationInfo::IGNORE, false},

  {"FETCH", 1, ShOperationInfo::EXTERNAL, false},

  {"PAL", 1, ShOperationInfo::EXTERNAL, false},

  {"LO", 1, ShOperationInfo::LINEAR, false},
  {"HI", 1, ShOperationInfo::LINEAR, false},
  {"SETLO", 1, ShOperationInfo::LINEAR, false},
  {"SETHI", 1, ShOperationInfo::LINEAR, false},
  
  {0, 0, ShOperationInfo::IGNORE, false}
};


} // namespace SH

