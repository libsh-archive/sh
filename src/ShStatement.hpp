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
#ifndef SHSTATEMENT_HPP
#define SHSTATEMENT_HPP

#include <iosfwd>
#include <set>
#include <list>
#include "ShOperation.hpp"
#include "ShVariable.hpp"
#include "ShStmtData.hpp"

namespace SH {

/** A single statement.
 * Represent a statement of the form 
 * <pre>dest := src[0] op src[1]</pre>
 * or, for unary operators: 
 * <pre>dest := op src[0]</pre>
 * or, for op == SH_OP_ASN:
 * <pre>dest := src[0]</pre>
 */
class ShStatement {
public:
  ShStatement(ShVariable dest, ShOperation op);
  ShStatement(ShVariable dest, ShOperation op, ShVariable src);
  ShStatement(ShVariable dest, ShVariable src0, ShOperation op, ShVariable src1);
  ShStatement(ShVariable dest, ShOperation op, ShVariable src0, ShVariable src1, ShVariable src2);

  // retrieves statement data of the given class (T must be a ShStmtDataPtr)
  // Assumes that there is a unique matching element in the list
  // or returns 0 if there is no matching element
  template<typename T>
  T data(); 
  
  ShVariable dest;
  ShVariable src[3];
  
  ShOperation op;


  // The following are used for the optimizer.
  
  std::set<ShStatement*> ud[3];
  std::set<ShStatement*> du;


  bool marked;

protected:
  std::list<ShStmtDataPtr> m_data;
};

std::ostream& operator<<(std::ostream& out, const SH::ShStatement& stmt);

template<typename T>
T ShStatement::data()
{
  T result;
  for(std::list<ShStmtDataPtr>::iterator I = m_data.begin(); I != m_data.end(); ++I) {
    if((result = dynamic_cast<T>(*I))) return result; 
  }
  return 0;
}

} // namespace SH

#endif
