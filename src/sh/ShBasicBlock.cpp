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
#include "ShBasicBlock.hpp"
#include "ShInfo.hpp"
#include "ShUtility.hpp"

namespace SH {

ShBasicBlock::~ShBasicBlock()
{
}

void ShBasicBlock::print(std::ostream& out, int indent) const
{
  using std::endl;
  
  shPrintIndent(out, indent);
  out << "{" << endl;
  for (const_iterator I = m_statements.begin();
       I != m_statements.end(); ++I) {
    shPrintIndent(out, indent + 2);
    out << *I << endl;
  }
  shPrintIndent(out, indent);
  out << "}" << endl;
}

void ShBasicBlock::graphvizDump(std::ostream& out) const
{
  for (const_iterator I = m_statements.begin();
       I != m_statements.end(); ++I) {
    const ShInfoComment* comment = I->get_info<ShInfoComment>();    
    out << *I; 
    if(comment) {
      out << " // " << comment->comment;
    }
    out << "\\n";
  }
}

void ShBasicBlock::addStatement(const ShStatement& stmt)
{
  m_statements.push_back(stmt);
}

void ShBasicBlock::prependStatement(const ShStatement& stmt)
{
  m_statements.push_front(stmt);
}

}
