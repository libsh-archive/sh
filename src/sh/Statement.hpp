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
#include "ShOperation.hpp"
#include "ShDllExport.hpp"
#include "ShInfo.hpp"
#include "ShVariable.hpp"

namespace SH {


/** A single statement.
 * Represent a statement of the form 
 * <pre>dest := src[0] op src[1]</pre>
 * or, for unary operators: 
 * <pre>dest := op src[0]</pre>
 * or, for op == SH_OP_ASN:
 * <pre>dest := src[0]</pre>
 */
class
SH_DLLEXPORT ShStatement: public ShInfoHolder {
public:
  /** Constructs a statement with the given operator, src(s), and dest
   * variables. Sets marked to false.
   * @{ */
  ShStatement(ShOperation op);
  ShStatement(ShVariable dest, ShOperation op);
  ShStatement(ShVariable dest, ShOperation op, ShVariable src);
  ShStatement(ShVariable dest, ShVariable src0, ShOperation op, ShVariable src1);
  ShStatement(ShVariable dest, ShOperation op, ShVariable src0, ShVariable src1);
  ShStatement(ShVariable dest, ShOperation op, ShVariable src0, ShVariable src1, ShVariable src2);
  // @}

  ShVariable dest;

  typedef std::vector<ShVariable> ShVariableVec;
  
  //< by default always allocates 3 ShVariables, but may hold more for future internal use.
  ShVariableVec src; 
                     
  
  ShOperation op;

  bool marked;

  friend SH_DLLEXPORT std::ostream& operator<<(std::ostream& out, const SH::ShStatement& stmt);

  private:
    static std::ostream& dumpVar(std::ostream &out, const ShVariable& var);
};

} // namespace SH

#endif
