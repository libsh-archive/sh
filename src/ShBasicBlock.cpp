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
