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
#include "ShBasicBlock.hpp"
#include <iostream>
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
  for (ShStmtList::const_iterator I = m_statements.begin();
       I != m_statements.end(); ++I) {
    shPrintIndent(out, indent + 2);
    out << *I << endl;
  }
  shPrintIndent(out, indent);
  out << "}" << endl;
}

void ShBasicBlock::graphvizDump(std::ostream& out) const
{
  for (ShStmtList::const_iterator I = m_statements.begin();
       I != m_statements.end(); ++I) {
    out << *I << "\\n";
  }
}

void ShBasicBlock::addStatement(const ShStatement& stmt)
{
  m_statements.push_back(stmt);
}

ShBasicBlock::ShStmtList::const_iterator ShBasicBlock::begin() const
{
  return m_statements.begin();
}

ShBasicBlock::ShStmtList::const_iterator ShBasicBlock::end() const
{
  return m_statements.end();
}


}
