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
#include "ShStatement.hpp"

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
  {"SIN", 1},
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

ShStatement::ShStatement(ShVariable dest, ShOperation op)
  : dest(dest), op(op)
{
}

ShStatement::ShStatement(ShVariable dest, ShOperation op, ShVariable src1)
  : dest(dest), op(op)
{
  src[0] = src1;
}

ShStatement::ShStatement(ShVariable dest, ShVariable src0, ShOperation op, ShVariable src1)
  : dest(dest), op(op)
{
  src[0] = src0;
  src[1] = src1;
}

ShStatement::ShStatement(ShVariable dest, ShOperation op, ShVariable src0, ShVariable src1, ShVariable src2)
  : dest(dest), op(op)
{
  src[0] = src0;
  src[1] = src1;
  src[2] = src2;
}

std::ostream& operator<<(std::ostream& out, const ShStatement& stmt)
{
  if (stmt.op == SH::SH_OP_ASN) {
    // Special case for assignment
    out << (stmt.dest.neg() ? "-" : "") << stmt.dest.name() << stmt.dest.swizzle() << " := " << stmt.src[0].name() << stmt.src[0].swizzle();
    return out;
  }
  
  switch (SH::opInfo[stmt.op].arity) {
  case 0:
    out << SH::opInfo[stmt.op].name << " " << (stmt.dest.neg() ? "-" : "") << stmt.dest.name() << stmt.dest.swizzle();
    break;
  case 1:
    out << (stmt.dest.neg() ? "-" : "") << stmt.dest.name() << stmt.dest.swizzle() << " := " << SH::opInfo[stmt.op].name
        << " " << (stmt.src[0].neg() ? "-" : "") << stmt.src[0].name() << stmt.src[0].swizzle();
    break;
  case 2:
    out << (stmt.dest.neg() ? "-" : "") << stmt.dest.name() << stmt.dest.swizzle() << " := " << (stmt.src[0].neg() ? "-" : "") << stmt.src[0].name() << stmt.src[0].swizzle()
        << " " << SH::opInfo[stmt.op].name << " " <<(stmt.src[1].neg() ? "-" : "") <<  stmt.src[1].name() << stmt.src[1].swizzle();
    break;
  case 3:
    out << (stmt.dest.neg() ? "-" : "") << stmt.dest.name() << stmt.dest.swizzle() << " := " << SH::opInfo[stmt.op].name << " "
        << (stmt.src[0].neg() ? "-" : "") << stmt.src[0].name() << stmt.src[0].swizzle() << ", " 
        << (stmt.src[1].neg() ? "-" : "") << stmt.src[1].name() << stmt.src[1].swizzle() << ", "
        << (stmt.src[2].neg() ? "-" : "") << stmt.src[2].name() << stmt.src[2].swizzle();
    break;
  default:
    out << "<<<Unknown arity>>>";
    break;
  }
  return out;
}

} // namespace SH

