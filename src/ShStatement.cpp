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

ShStatement::ShStatement(ShOperation op)
  : src(3), op(op), marked(false)
{
}

ShStatement::ShStatement(ShVariable dest, ShOperation op)
  : dest(dest), src(3), op(op), marked(false)
{
}

ShStatement::ShStatement(ShVariable dest, ShOperation op, ShVariable src1)
  : dest(dest), src(3), op(op), marked(false)
{
  if(op == SH_OP_ASN && dest.size() != src1.size()) {
    SH_DEBUG_PRINT("SH_OP_ASN dest.size() != src.size() (" << dest.size() << " != " << src1.size());
    SH_DEBUG_PRINT("  dest=" << dest.name() << " src=" << src1.name());
    SH_DEBUG_ASSERT(dest.size() == src1.size());
  }
  src[0] = src1;
}

ShStatement::ShStatement(ShVariable dest, ShVariable src0, ShOperation op, ShVariable src1)
  : dest(dest), src(3), op(op), marked(false)
{
  src[0] = src0;
  src[1] = src1;
}

ShStatement::ShStatement(ShVariable dest, ShOperation op, ShVariable src0, ShVariable src1, ShVariable src2)
  : dest(dest), src(3), op(op), marked(false)
{
  src[0] = src0;
  src[1] = src1;
  src[2] = src2;
}

std::ostream& operator<<(std::ostream& out, const ShStatement& stmt)
{
  if(stmt.op == SH_OP_COMMENT) return out;
  ShStatement::dumpVar(out, stmt.dest) << " := ";
  if(stmt.op != SH_OP_ASN) out << opInfo[stmt.op].name << " ";
  for(int i = 0; i < opInfo[stmt.op].arity; ++i) {
    if(i != 0) out << ", ";
    ShStatement::dumpVar(out, stmt.src[i]); 
  }
  return out;
}

std::ostream& ShStatement::dumpVar(std::ostream &out, const ShVariable& var)
{
  if(var.null()) {
    out << "[null]";
  } else {
    out << (var.neg() ? "-" : "") << var.name() << var.swizzle();
  }
  return out;
}

} // namespace SH

