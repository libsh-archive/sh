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
#include "Inclusion.hpp"
#include "Statement.hpp"

namespace {
template<typename T>
int id(T* x) {
  long long xa = reinterpret_cast<long long>(x);
  return (xa & 7) + ((xa >> 3) & 7) + ((xa >> 6) & 7);
}
}

namespace SH {

Statement::Statement(Operation op)
  : src(3), op(op), marked(false)
{
}

Statement::Statement(Variable dest, Operation op)
  : dest(dest), src(3), op(op), marked(false)
{
}

Statement::Statement(Variable dest, Operation op, Variable src1)
  : dest(dest), src(3), op(op), marked(false)
{
  if(op == OP_ASN && dest.size() != src1.size()) {
    SH_DEBUG_PRINT("OP_ASN dest.size() != src.size() (" << dest.size() << " != " << src1.size());
    SH_DEBUG_PRINT("  dest=" << dest.name() << " src=" << src1.name());
    SH_DEBUG_ASSERT(dest.size() == src1.size());
  }
  src[0] = src1;
}

Statement::Statement(Variable dest, Variable src0, Operation op, Variable src1)
  : dest(dest), src(3), op(op), marked(false)
{
  src[0] = src0;
  src[1] = src1;
}

Statement::Statement(Variable dest, Operation op, Variable src0, Variable src1)
  : dest(dest), src(3), op(op), marked(false)
{
  src[0] = src0;
  src[1] = src1;
}

Statement::Statement(Variable dest, Operation op, Variable src0, Variable src1, Variable src2)
  : dest(dest), src(3), op(op), marked(false)
{
  src[0] = src0;
  src[1] = src1;
  src[2] = src2;
}

std::ostream& operator<<(std::ostream& out, const Statement& stmt)
{
  if(stmt.op == OP_COMMENT) return out;
  Statement::dumpVar(out, stmt.dest) << " := ";
  if(stmt.op != OP_ASN) out << opInfo[stmt.op].name << " ";
  for(int i = 0; i < opInfo[stmt.op].arity; ++i) {
    if(i != 0) out << ", ";
    Statement::dumpVar(out, stmt.src[i]); 
  }
  const StmtIndex* idx = stmt.get_info<StmtIndex>();
  if(idx) {
    out << " # " << idx->index();
  }
  return out;
}

std::ostream& Statement::dumpVar(std::ostream &out, const Variable& var)
{
  if(var.null()) {
    out << "[null]";
  } else {
    out << (var.neg() ? "-" : "") << var.name() << var.swizzle() << " " << id(var.node().object()); 
  }
  return out;
}

} // namespace SH

