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
  {"ASN", 1, ShOperationInfo::LINEAR, true, false, false},
  
  {"NEG", 1, ShOperationInfo::LINEAR, true, false, false},
  {"ADD", 2, ShOperationInfo::LINEAR, true, false, true},
  {"MUL", 2, ShOperationInfo::LINEAR, true, true, true},
  {"DIV", 2, ShOperationInfo::LINEAR, true, true, false},

  {"SLT", 2, ShOperationInfo::LINEAR, false, true, false},
  {"SLE", 2, ShOperationInfo::LINEAR, false, true, false},
  {"SGT", 2, ShOperationInfo::LINEAR, false, true, false},
  {"SGE", 2, ShOperationInfo::LINEAR, false, true, false},
  {"SEQ", 2, ShOperationInfo::LINEAR, false, true, false},
  {"SNE", 2, ShOperationInfo::LINEAR, false, true, false},
  
  {"ABS", 1, ShOperationInfo::LINEAR, true, true, false},
  {"ACOS", 1, ShOperationInfo::LINEAR, true, true, false},
  {"ASIN", 1, ShOperationInfo::LINEAR, true, true, false},
  {"ATAN", 1, ShOperationInfo::LINEAR, true, true, false},
  {"ATAN2", 2, ShOperationInfo::LINEAR, true, true, false},

  {"CBRT", 1, ShOperationInfo::LINEAR, true, true, false},
  {"CEIL", 1, ShOperationInfo::LINEAR, true, true, false},
  {"COS", 1, ShOperationInfo::LINEAR, true, true, false},
  {"CMUL", 1, ShOperationInfo::ALL, true, true, false},
  {"CSUM", 1, ShOperationInfo::ALL, true, false, false},
  {"DOT", 2, ShOperationInfo::ALL, true, true, true},
  {"DX", 1, ShOperationInfo::EXTERNAL, true, true, false},
  {"DY", 1, ShOperationInfo::EXTERNAL, true, true, false},

  {"EXP", 1, ShOperationInfo::LINEAR, true, true, false},
  {"EXP2", 1, ShOperationInfo::LINEAR, true, true, false},
  {"EXP10", 1, ShOperationInfo::LINEAR, true, true, false},
  {"FLR", 1, ShOperationInfo::LINEAR, true, true, false},
  {"FRAC", 1, ShOperationInfo::LINEAR, true, true, false},

  {"LIT", 1, ShOperationInfo::ALL, true, true, false},
  {"LOG", 1, ShOperationInfo::LINEAR, true, true, false},
  {"LOG2", 1, ShOperationInfo::LINEAR, true, true, false},
  {"LOG10", 1, ShOperationInfo::LINEAR, true, true, false},
  {"LRP", 3, ShOperationInfo::LINEAR, true, true, false},

  {"MAD", 3, ShOperationInfo::LINEAR, true, true, false},
  {"MAX", 2, ShOperationInfo::LINEAR, true, true, false},
  {"MIN", 2, ShOperationInfo::LINEAR, true, true, true},
  {"MOD", 2, ShOperationInfo::LINEAR, true, true, true},

  {"POW", 2, ShOperationInfo::LINEAR, true, true, false},
  {"RCP", 1, ShOperationInfo::LINEAR, true, true, false},
  {"RND", 1, ShOperationInfo::LINEAR, true, true, false},
  {"RSQ", 1, ShOperationInfo::LINEAR, true, true, false},

  {"SIN", 1, ShOperationInfo::LINEAR, true, true, false},
  {"SGN", 1, ShOperationInfo::LINEAR, true, true, false},
  {"SQRT", 1, ShOperationInfo::LINEAR, true, true, false},
  {"TAN", 1, ShOperationInfo::LINEAR, true, true, false},

  {"NORM", 1, ShOperationInfo::ALL, true, true, false},
  {"XPD", 2, ShOperationInfo::ALL, true, true, false}, // Not quite true, but probably good enough

  {"TEX", 2, ShOperationInfo::EXTERNAL, true, true, false},
  {"TEXI", 2, ShOperationInfo::EXTERNAL, true, true, false},
  {"TEXD", 3, ShOperationInfo::EXTERNAL, true, true, false},

  {"COND", 3, ShOperationInfo::LINEAR, true, true, false},

  {"KIL", 1, ShOperationInfo::IGNORE, false, false, false},

  {"DBG", 1, ShOperationInfo::IGNORE, false, false, false},

  {"OPTBRA", 1, ShOperationInfo::IGNORE, false, false, false},
  {"DECL", 0, ShOperationInfo::IGNORE, false, false, false},
  {"PHI", 1, ShOperationInfo::LINEAR, false, false, false},
  {"ESCJOIN", 1, ShOperationInfo::IGNORE, false, true, false},
  {"ESCSAV", 1, ShOperationInfo::IGNORE, false, false, false},

  {"STARTSEC", 0, ShOperationInfo::IGNORE, false, false, false},
  {"ENDSEC", 0, ShOperationInfo::IGNORE, false, false, false},

  {"FETCH", 1, ShOperationInfo::EXTERNAL, true, true, false},
  {"LOOKUP", 2, ShOperationInfo::EXTERNAL, true, true, false},

  {"PAL", 2, ShOperationInfo::EXTERNAL, true, true, false},

  {"LO", 1, ShOperationInfo::LINEAR, true, false, false},
  {"HI", 1, ShOperationInfo::LINEAR, true, false, false},
  {"WIDTH", 1, ShOperationInfo::LINEAR, false, false, false},
  {"RADIUS", 1, ShOperationInfo::LINEAR, false, false, false},
  {"CENTER", 1, ShOperationInfo::LINEAR, false, false, false},
  {"IVAL", 2, ShOperationInfo::LINEAR, true, true, false},
  {"UNION", 2, ShOperationInfo::LINEAR, true, false, true},
  {"ISCT", 2, ShOperationInfo::LINEAR, true, false, true},
  {"CONTAINS", 2, ShOperationInfo::LINEAR, false, true, false},

  {"ERRFROM", 2, ShOperationInfo::IGNORE, false, false, false}, // this is a special case, ignore the affine_ flags 
  {"LASTERR", 2, ShOperationInfo::IGNORE, false, false, false}, // this is a special case, ignore the affine_ flags 

  {"COMMENT", 0, ShOperationInfo::IGNORE, false, false, false}, // special comment tag
  
  {0, 0, ShOperationInfo::IGNORE, true, true, false}
};


} // namespace SH

