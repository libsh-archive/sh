// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
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
#include "Operation.hpp"

namespace SH {

/** Update this if you add or change operations in Operation.
 * @see Operation
 */
const OperationInfo opInfo[] = {
  {"ASN", 1, OperationInfo::LINEAR, true, false, false},
  
  {"NEG", 1, OperationInfo::LINEAR, true, false, false},
  {"ADD", 2, OperationInfo::LINEAR, true, false, true},
  {"MUL", 2, OperationInfo::LINEAR, true, true, true},
  {"DIV", 2, OperationInfo::LINEAR, true, true, false},

  {"SLT", 2, OperationInfo::LINEAR, false, true, false},
  {"SLE", 2, OperationInfo::LINEAR, false, true, false},
  {"SGT", 2, OperationInfo::LINEAR, false, true, false},
  {"SGE", 2, OperationInfo::LINEAR, false, true, false},
  {"SEQ", 2, OperationInfo::LINEAR, false, true, false},
  {"SNE", 2, OperationInfo::LINEAR, false, true, false},
  
  {"ABS", 1, OperationInfo::LINEAR, true, true, false},
  {"ACOS", 1, OperationInfo::LINEAR, true, true, false},
  {"ACOSH", 1, OperationInfo::LINEAR, true, true, false},
  {"ASIN", 1, OperationInfo::LINEAR, true, true, false},
  {"ASINH", 1, OperationInfo::LINEAR, true, true, false},
  {"ATAN", 1, OperationInfo::LINEAR, true, true, false},
  {"ATAN2", 2, OperationInfo::LINEAR, true, true, false},
  {"ATANH", 1, OperationInfo::LINEAR, true, true, false},

  {"CBRT", 1, OperationInfo::LINEAR, true, true, false},
  {"CEIL", 1, OperationInfo::LINEAR, true, true, false},
  {"COS", 1, OperationInfo::LINEAR, true, true, false},
  {"COSH", 1, OperationInfo::LINEAR, true, true, false},
  {"CMUL", 1, OperationInfo::ALL, true, true, false},
  {"CSUM", 1, OperationInfo::ALL, true, false, false},
  {"DOT", 2, OperationInfo::ALL, true, true, true},
  {"DX", 1, OperationInfo::EXTERNAL, true, true, false},
  {"DY", 1, OperationInfo::EXTERNAL, true, true, false},

  {"EXP", 1, OperationInfo::LINEAR, true, true, false},
  {"EXP2", 1, OperationInfo::LINEAR, true, true, false},
  {"EXP10", 1, OperationInfo::LINEAR, true, true, false},
  {"FLR", 1, OperationInfo::LINEAR, true, true, false},
  {"FRAC", 1, OperationInfo::LINEAR, true, true, false},

  {"HASH", 1, OperationInfo::LINEAR, true, true, false},

  {"LIT", 1, OperationInfo::ALL, true, true, false},
  {"LOG", 1, OperationInfo::LINEAR, true, true, false},
  {"LOG2", 1, OperationInfo::LINEAR, true, true, false},
  {"LOG10", 1, OperationInfo::LINEAR, true, true, false},
  {"LRP", 3, OperationInfo::LINEAR, true, true, false},

  {"MAD", 3, OperationInfo::LINEAR, true, true, false},
  {"MAX", 2, OperationInfo::LINEAR, true, true, true},
  {"MIN", 2, OperationInfo::LINEAR, true, true, true},
  {"MOD", 2, OperationInfo::LINEAR, true, true, true},

  /* @todo range - check if this is correct */
  {"NOISE", 1, OperationInfo::LINEAR, true, true, false},

  {"POW", 2, OperationInfo::LINEAR, true, true, false},
  {"RCP", 1, OperationInfo::LINEAR, true, true, false},
  {"RND", 1, OperationInfo::LINEAR, true, true, false},
  {"RSQ", 1, OperationInfo::LINEAR, true, true, false},

  {"SIN", 1, OperationInfo::LINEAR, true, true, false},
  {"SINH", 1, OperationInfo::LINEAR, true, true, false},
  {"SGN", 1, OperationInfo::LINEAR, true, true, false},
  {"SQRT", 1, OperationInfo::LINEAR, true, true, false},
  {"TAN", 1, OperationInfo::LINEAR, true, true, false},
  {"TANH", 1, OperationInfo::LINEAR, false},

  {"NORM", 1, OperationInfo::ALL, true, true, false},
  {"XPD", 2, OperationInfo::ALL, true, true, false}, // Not quite true, but probably good enough

  {"TEX", 2, OperationInfo::EXTERNAL, true, true, false},
  {"TEXI", 2, OperationInfo::EXTERNAL, true, true, false},
  {"TEXD", 3, OperationInfo::EXTERNAL, true, true, false},
  {"TEXLOD", 3, OperationInfo::EXTERNAL, true, true, false},
  {"TEXBIAS", 3, OperationInfo::EXTERNAL, true, true, false},

  {"COND", 3, OperationInfo::LINEAR, true, true, false},

  {"KIL", 1, OperationInfo::IGNORE, false, false, false},

  {"DBG", 1, OperationInfo::IGNORE, false, false, false},

  {"OPTBRA", 1, OperationInfo::IGNORE, false, false, false},
  {"DECL", 0, OperationInfo::IGNORE, false, false, false},
  {"PHI", 1, OperationInfo::LINEAR, false, false, false},
  {"ESCJOIN", 1, OperationInfo::IGNORE, false, true, false},
  {"ESCSAV", 1, OperationInfo::IGNORE, false, false, false},

  {"STARTSEC", 0, OperationInfo::IGNORE, false, false, false},
  {"ENDSEC", 0, OperationInfo::IGNORE, false, false, false},

  {"FETCH", 1, OperationInfo::EXTERNAL, true, true, false},
  {"LOOKUP", 2, OperationInfo::EXTERNAL, true, true, false},

  {"PAL", 2, OperationInfo::EXTERNAL, true, true, false},

  {"LO", 1, OperationInfo::LINEAR, true, false, false},
  {"HI", 1, OperationInfo::LINEAR, true, false, false},
  {"WIDTH", 1, OperationInfo::LINEAR, false, false, false},
  {"RADIUS", 1, OperationInfo::LINEAR, false, false, false},
  {"CENTER", 1, OperationInfo::LINEAR, false, false, false},
  {"IVAL", 2, OperationInfo::LINEAR, true, true, false},
  {"UNION", 2, OperationInfo::LINEAR, true, true, true},
  {"ISCT", 2, OperationInfo::LINEAR, true, false, true},
  {"CONTAINS", 2, OperationInfo::LINEAR, false, true, false},

  {"ERRFROM", 2, OperationInfo::IGNORE, false, false, false}, // this is a special case, ignore the affine_ flags 
  {"LASTERR", 2, OperationInfo::IGNORE, false, false, false}, // this is a special case, ignore the affine_ flags 

  {"COMMENT", 0, OperationInfo::IGNORE, false, false, false}, // special comment tag
  
  {"RET", 1, OperationInfo::IGNORE, false, false, false},
  {0, 0, OperationInfo::IGNORE, true, true, false}
};


} // namespace SH

