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
#include "BasicBlock.hpp"
#include "Info.hpp"
#include "Utility.hpp"
#include "Tag.hpp"

namespace SH {

BasicBlock::~BasicBlock()
{
}

void BasicBlock::print(std::ostream& out, int indent) const
{
  using std::endl;
  
  printIndent(out, indent);
  out << "{" << endl;
  for (const_iterator I = m_statements.begin();
       I != m_statements.end(); ++I) {
    printIndent(out, indent + 2);
    out << *I << endl;
  }
  printIndent(out, indent);
  out << "}" << endl;
}

void BasicBlock::graphviz_dump(std::ostream& out) const
{
  for (const_iterator I = m_statements.begin();
       I != m_statements.end(); ++I) {
    const InfoComment* comment = I->get_info<InfoComment>();    
    out << *I; 
    if(comment) {
      out << " // " << comment->comment;
    }
    out << "\\n";
  }
}

void BasicBlock::addStatement(const Statement& stmt)
{
  m_statements.push_back(stmt);
}

void BasicBlock::prependStatement(const Statement& stmt)
{
  m_statements.push_front(stmt);
}

// Place all the elements of l before the iterator I and removes them
// from l
void BasicBlock::splice(iterator I, StmtList &l) { 
  Tag::cloneToIt(l.begin(), l.end());
  m_statements.splice(I, l); 
}

// Places all the elements starting from lI in l before the iterator I and
// removes them from l
void BasicBlock::splice(iterator I, StmtList &l, iterator lI) {
  Tag::cloneToIt(lI, l.end());
  m_statements.splice(I, l, lI, l.end());
}

void BasicBlock::insert(iterator I, const Statement& stmt) { 
  Tag::cloneTo(*m_statements.insert(I, stmt)); 
}

}
