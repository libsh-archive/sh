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
  {"FRAC", 1},
  {"POW", 2},
  {"SIN", 1},
  {"SQRT", 1},

  {"NORM", 1},

  {"TEX", 2},

  {"COND", 3},
  
  {0, 0}
};

ShStatement::ShStatement(ShVariable dest, ShOperation op, ShVariable src)
  : dest(dest), src1(src), src2(0), op(op)
{
}

ShStatement::ShStatement(ShVariable dest, ShVariable src1, ShOperation op, ShVariable src2)
  : dest(dest), src1(src1), src2(src2), op(op)
{
}

ShStatement::ShStatement(ShVariable dest, ShOperation op, ShVariable src1, ShVariable src2, ShVariable src3)
  : dest(dest), src1(src1), src2(src2), src3(src3), op(op)
{
}

std::ostream& operator<<(std::ostream& out, const ShStatement& stmt)
{
  if (stmt.op == SH::SH_OP_ASN) {
    // Special case for assignment
    out << stmt.dest.name() << stmt.dest.swizzle() << " := " << stmt.src1.name() << stmt.src1.swizzle();
    return out;
  }
  
  switch (SH::opInfo[stmt.op].arity) {
  case 1:
    out << stmt.dest.name() << stmt.dest.swizzle() << " := " << SH::opInfo[stmt.op].name
        << " " << stmt.src1.name() << stmt.src1.swizzle();
    break;
  case 2:
    out << stmt.dest.name() << stmt.dest.swizzle() << " := " << stmt.src1.name() << stmt.src1.swizzle()
        << " " << SH::opInfo[stmt.op].name << " " << stmt.src2.name() << stmt.src2.swizzle();
    break;
  case 3:
    out << stmt.dest.name() << stmt.dest.swizzle() << " := " << SH::opInfo[stmt.op].name << " "
        << stmt.src1.name() << stmt.src1.swizzle() << ", " 
        << stmt.src2.name() << stmt.src2.swizzle() << ", "
        << stmt.src3.name() << stmt.src3.swizzle();
    break;
  default:
    out << "<<<Unknown arity>>>";
    break;
  }
  return out;
}

} // namespace SH

