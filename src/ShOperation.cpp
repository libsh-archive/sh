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
  {"COSH", 1, ShOperationInfo::LINEAR, false},
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

  {"LIT", 1, ShOperationInfo::ALL, false},
  {"LOG", 1, ShOperationInfo::LINEAR, false},
  {"LOG2", 1, ShOperationInfo::LINEAR, false},
  {"LOG10", 1, ShOperationInfo::LINEAR, false},
  {"LRP", 3, ShOperationInfo::LINEAR, false},

  {"MAD", 3, ShOperationInfo::LINEAR, false},
  {"MAX", 2, ShOperationInfo::LINEAR, false},
  {"MIN", 2, ShOperationInfo::LINEAR, true},
  {"MOD", 2, ShOperationInfo::LINEAR, true},

  {"POW", 2, ShOperationInfo::LINEAR, false},
  {"RCP", 1, ShOperationInfo::LINEAR, false},
  {"RND", 1, ShOperationInfo::LINEAR, false},
  {"RSQ", 1, ShOperationInfo::LINEAR, false},

  {"SIN", 1, ShOperationInfo::LINEAR, false},
  {"SINH", 1, ShOperationInfo::LINEAR, false},
  {"SGN", 1, ShOperationInfo::LINEAR, false},
  {"SQRT", 1, ShOperationInfo::LINEAR, false},
  {"TAN", 1, ShOperationInfo::LINEAR, false},
  {"TANH", 1, ShOperationInfo::LINEAR, false},

  {"NORM", 1, ShOperationInfo::ALL, false},
  {"XPD", 2, ShOperationInfo::ALL, false}, // Not quite true, but probably good enough

  {"TEX", 2, ShOperationInfo::EXTERNAL, false},
  {"TEXI", 2, ShOperationInfo::EXTERNAL, false},
  {"TEXD", 3, ShOperationInfo::EXTERNAL, false},

  {"COND", 3, ShOperationInfo::LINEAR, false},

  {"KIL", 1, ShOperationInfo::IGNORE, false},

  {"OPTBRA", 1, ShOperationInfo::IGNORE, false},
  {"DECL", 0, ShOperationInfo::IGNORE, false},

  {"STARTSEC", 0, ShOperationInfo::IGNORE, false},
  {"ENDSEC", 0, ShOperationInfo::IGNORE, false},

  {"FETCH", 1, ShOperationInfo::EXTERNAL, false},
  {"LOOKUP", 2, ShOperationInfo::EXTERNAL, false},

  {"PAL", 2, ShOperationInfo::EXTERNAL, false},

  {"COMMENT", 0, ShOperationInfo::IGNORE, false}, // special comment tag

  {"RET", 1, ShOperationInfo::IGNORE, false},

  {0, 0, ShOperationInfo::IGNORE, false}
};


} // namespace SH

