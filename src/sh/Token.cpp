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
#include "Token.hpp"
#include "Tokenizer.hpp"
#include "Utility.hpp"

namespace SH {

/** Names of token types.
 * Change this if you change the TokenType enumeration.
 * @see TokenType
 */
char* tokenNames[] = {
  "IF",
  "ELSE",
  "ENDIF",
  "WHILE",
  "ENDWHILE",
  "DO",
  "UNTIL",
  "FOR",
  "ENDFOR",
  "BREAK",
  "CONTINUE",
  "SEC",
  "ENDSEC",
  "SWITCH",
  "ENDSWITCH",
  "CASE",
  "DEFAULT"
};

Token::Token(TokenType type)
  : m_type(type)
{
}

Token::~Token()
{
}

TokenType Token::type()
{
  return m_type;
  
}


void Token::print(std::ostream& out, int indent) const
{
  printIndent(out, indent);
  out << tokenNames[m_type];
  if (!arguments.empty()) out << "<";
  out << std::endl;
  
  for (std::vector<TokenArgument>::const_iterator I = arguments.begin();
       I != arguments.end(); ++I) {
    printIndent(out, indent + 2);
    out << I->result.name() << ", initialized using" << std::endl;
    I->blockList->print(out, indent + 2);
  }
  if (!arguments.empty()) out << '>' << std::endl;
}

}
