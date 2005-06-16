// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
