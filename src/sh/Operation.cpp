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
#include <iostream>
#include "Operation.hpp"

namespace SH {

/** Update this if you add or change operations in Operation.
 * @see Operation
 */
const OperationInfo opInfo[] = {
  {"ASN", 1, OperationInfo::LINEAR, false},
  
  {"NEG", 1, OperationInfo::LINEAR, false},
  {"ADD", 2, OperationInfo::LINEAR, true},
  {"MUL", 2, OperationInfo::LINEAR, true},
  {"DIV", 2, OperationInfo::LINEAR, false},

  {"SLT", 2, OperationInfo::LINEAR, false},
  {"SLE", 2, OperationInfo::LINEAR, false},
  {"SGT", 2, OperationInfo::LINEAR, false},
  {"SGE", 2, OperationInfo::LINEAR, false},
  {"SEQ", 2, OperationInfo::LINEAR, false},
  {"SNE", 2, OperationInfo::LINEAR, false},
  
  {"ABS", 1, OperationInfo::LINEAR, false},
  {"ACOS", 1, OperationInfo::LINEAR, false},
  {"ACOSH", 1, OperationInfo::LINEAR, false},
  {"ASIN", 1, OperationInfo::LINEAR, false},
  {"ASINH", 1, OperationInfo::LINEAR, false},
  {"ATAN", 1, OperationInfo::LINEAR, false},
  {"ATAN2", 2, OperationInfo::LINEAR, false},
  {"ATANH", 1, OperationInfo::LINEAR, false},

  {"CBRT", 1, OperationInfo::LINEAR, false},
  {"CEIL", 1, OperationInfo::LINEAR, false},
  {"COS", 1, OperationInfo::LINEAR, false},
  {"COSH", 1, OperationInfo::LINEAR, false},
  {"CMUL", 1, OperationInfo::ALL, false},
  {"CSUM", 1, OperationInfo::ALL, false},
  {"DOT", 2, OperationInfo::ALL, true},
  {"DX", 1, OperationInfo::EXTERNAL, false},
  {"DY", 1, OperationInfo::EXTERNAL, false},

  {"EXP", 1, OperationInfo::LINEAR, false},
  {"EXP2", 1, OperationInfo::LINEAR, false},
  {"EXP10", 1, OperationInfo::LINEAR, false},
  {"FLR", 1, OperationInfo::LINEAR, false},
  {"FRAC", 1, OperationInfo::LINEAR, false},

  {"HASH", 1, OperationInfo::LINEAR, false},

  {"LIT", 1, OperationInfo::ALL, false},
  {"LOG", 1, OperationInfo::LINEAR, false},
  {"LOG2", 1, OperationInfo::LINEAR, false},
  {"LOG10", 1, OperationInfo::LINEAR, false},
  {"LRP", 3, OperationInfo::LINEAR, false},

  {"MAD", 3, OperationInfo::LINEAR, false},
  {"MAX", 2, OperationInfo::LINEAR, false},
  {"MIN", 2, OperationInfo::LINEAR, true},
  {"MOD", 2, OperationInfo::LINEAR, true},

  {"NOISE", 1, OperationInfo::LINEAR, false},

  {"POW", 2, OperationInfo::LINEAR, false},
  {"RCP", 1, OperationInfo::LINEAR, false},
  {"RND", 1, OperationInfo::LINEAR, false},
  {"RSQ", 1, OperationInfo::LINEAR, false},

  {"SIN", 1, OperationInfo::LINEAR, false},
  {"SINH", 1, OperationInfo::LINEAR, false},
  {"SGN", 1, OperationInfo::LINEAR, false},
  {"SQRT", 1, OperationInfo::LINEAR, false},
  {"TAN", 1, OperationInfo::LINEAR, false},
  {"TANH", 1, OperationInfo::LINEAR, false},

  {"NORM", 1, OperationInfo::ALL, false},
  {"XPD", 2, OperationInfo::ALL, false}, // Not quite true, but probably good enough

  {"TEX", 2, OperationInfo::EXTERNAL, false},
  {"TEXI", 2, OperationInfo::EXTERNAL, false},
  {"TEXD", 3, OperationInfo::EXTERNAL, false},
  {"TEXLOD", 3, OperationInfo::EXTERNAL, false},
  {"TEXBIAS", 3, OperationInfo::EXTERNAL, false},

  {"COND", 3, OperationInfo::LINEAR, false},

  {"KIL", 1, OperationInfo::IGNORE, false},

  {"OPTBRA", 1, OperationInfo::IGNORE, false},
  {"DECL", 0, OperationInfo::IGNORE, false},

  {"STARTSEC", 0, OperationInfo::IGNORE, false},
  {"ENDSEC", 0, OperationInfo::IGNORE, false},

  {"FETCH", 1, OperationInfo::EXTERNAL, false},
  {"LOOKUP", 2, OperationInfo::EXTERNAL, false},

  {"PAL", 2, OperationInfo::EXTERNAL, false},

  {"COMMENT", 0, OperationInfo::IGNORE, false}, // special comment tag

  {"RET", 1, OperationInfo::IGNORE, false},

  {0, 0, OperationInfo::IGNORE, false}
};


} // namespace SH

