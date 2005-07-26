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

