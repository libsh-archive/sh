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
  {"ASN", 1, ShOperationInfo::LINEAR},
  
  {"NEG", 1, ShOperationInfo::LINEAR},
  {"ADD", 2, ShOperationInfo::LINEAR},
  {"MUL", 2, ShOperationInfo::LINEAR},
  {"DIV", 2, ShOperationInfo::LINEAR},

  {"SLT", 2, ShOperationInfo::LINEAR},
  {"SLE", 2, ShOperationInfo::LINEAR},
  {"SGT", 2, ShOperationInfo::LINEAR},
  {"SGE", 2, ShOperationInfo::LINEAR},
  {"SEQ", 2, ShOperationInfo::LINEAR},
  {"SNE", 2, ShOperationInfo::LINEAR},
  
  {"ABS", 1, ShOperationInfo::LINEAR},
  {"ACOS", 1, ShOperationInfo::LINEAR},
  {"ASIN", 1, ShOperationInfo::LINEAR},
  {"ATAN", 1, ShOperationInfo::LINEAR},
  {"ATAN2", 2, ShOperationInfo::LINEAR},
  {"CEIL", 1, ShOperationInfo::LINEAR},
  {"COS", 1, ShOperationInfo::LINEAR},
  {"CMUL", 1, ShOperationInfo::ALL},
  {"CSUM", 1, ShOperationInfo::ALL},
  {"DOT", 2, ShOperationInfo::ALL},
  {"DX", 1, ShOperationInfo::EXTERNAL},
  {"DY", 1, ShOperationInfo::EXTERNAL},
  {"EXP", 1, ShOperationInfo::LINEAR},
  {"EXP2", 1, ShOperationInfo::LINEAR},
  {"EXP10", 1, ShOperationInfo::LINEAR},
  {"FLR", 1, ShOperationInfo::LINEAR},
  {"FRAC", 1, ShOperationInfo::LINEAR},
  {"LRP", 3, ShOperationInfo::LINEAR},
  {"MAD", 3, ShOperationInfo::LINEAR},
  {"MAX", 2, ShOperationInfo::LINEAR},
  {"MIN", 2, ShOperationInfo::LINEAR},
  {"MOD", 2, ShOperationInfo::LINEAR},
  {"LOG", 1, ShOperationInfo::LINEAR},
  {"LOG2", 1, ShOperationInfo::LINEAR},
  {"LOG10", 1, ShOperationInfo::LINEAR},
  {"POW", 2, ShOperationInfo::LINEAR},
  {"RCP", 1, ShOperationInfo::LINEAR},
  {"RSQ", 1, ShOperationInfo::LINEAR},
  {"SIN", 1, ShOperationInfo::LINEAR},
  {"SGN", 1, ShOperationInfo::LINEAR},
  {"SQRT", 1, ShOperationInfo::LINEAR},
  {"TAN", 1, ShOperationInfo::LINEAR},

  {"NORM", 1, ShOperationInfo::ALL},
  {"XPD", 2, ShOperationInfo::ALL}, // Not quite true, but probably good enough

  {"TEX", 2, ShOperationInfo::EXTERNAL},
  {"TEXI", 2, ShOperationInfo::EXTERNAL},

  {"COND", 3, ShOperationInfo::LINEAR},

  {"KIL", 1, ShOperationInfo::IGNORE},

  {"OPTBRA", 1, ShOperationInfo::IGNORE},

  {"FETCH", 1, ShOperationInfo::EXTERNAL},
  
  {0, 0, ShOperationInfo::IGNORE}
};

ShStatementInfo::ShStatementInfo()
{
}

ShStatementInfo::~ShStatementInfo()
{
}

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

ShStatement::ShStatement(const ShStatement& other)
  : dest(other.dest),
    op(other.op),
    marked(other.marked),
    du(other.du)
{
  for (int i = 0; i < 3; i++) src[i] = other.src[i];
  for (int i = 0; i < 3; i++) ud[i] = other.ud[i];  
  for (std::list<ShStatementInfo*>::const_iterator I = other.info.begin(); I != other.info.end(); ++I) {
    info.push_back((*I)->clone());
  }
}

ShStatement::~ShStatement()
{
  for (std::list<ShStatementInfo*>::iterator I = info.begin(); I != info.end(); ++I) {
    delete *I;
  }
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

void ShStatement::add_info(ShStatementInfo* new_info)
{
  info.push_back(new_info);
}

void ShStatement::remove_info(ShStatementInfo* old_info)
{
  info.remove(old_info);
}

} // namespace SH

