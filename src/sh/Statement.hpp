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
#ifndef SHSTATEMENT_HPP
#define SHSTATEMENT_HPP

#include <iosfwd>
#include <set>
#include <list>
#include "Operation.hpp"
#include "DllExport.hpp"
#include "Info.hpp"
#include "Variable.hpp"

namespace SH {


/** A single statement.
 * Represent a statement of the form 
 * <pre>dest := src[0] op src[1]</pre>
 * or, for unary operators: 
 * <pre>dest := op src[0]</pre>
 * or, for op == OP_ASN:
 * <pre>dest := src[0]</pre>
 */
class
SH_DLLEXPORT Statement: public InfoHolder {
public:
  /** Constructs a statement with the given operator, src(s), and dest
   * variables. Sets marked to false.
   * @{ */
  Statement(Operation op);
  Statement(Variable dest, Operation op);
  Statement(Variable dest, Operation op, Variable src);
  Statement(Variable dest, Variable src0, Operation op, Variable src1);
  Statement(Variable dest, Operation op, Variable src0, Variable src1);
  Statement(Variable dest, Operation op, Variable src0, Variable src1, Variable src2);
  // @}

  Variable dest;

  typedef std::vector<Variable> VariableVec;
  
  //< by default always allocates 3 Variables, but may hold more for future internal use.
  VariableVec src; 
                     
  
  Operation op;

  bool marked;

  friend SH_DLLEXPORT std::ostream& operator<<(std::ostream& out, const SH::Statement& stmt);

  private:
    static std::ostream& dumpVar(std::ostream &out, const Variable& var);
};

} // namespace SH

#endif
