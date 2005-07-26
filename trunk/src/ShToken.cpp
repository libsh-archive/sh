// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#include "ShToken.hpp"
#include "ShTokenizer.hpp"
#include "ShUtility.hpp"

namespace SH {

/** Names of token types.
 * Change this if you change the ShTokenType enumeration.
 * @see ShTokenType
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

ShToken::ShToken(ShTokenType type)
  : m_type(type)
{
}

ShToken::~ShToken()
{
}

ShTokenType ShToken::type()
{
  return m_type;
  
}


void ShToken::print(std::ostream& out, int indent) const
{
  shPrintIndent(out, indent);
  out << tokenNames[m_type];
  if (!arguments.empty()) out << "<";
  out << std::endl;
  
  for (std::vector<ShTokenArgument>::const_iterator I = arguments.begin();
       I != arguments.end(); ++I) {
    shPrintIndent(out, indent + 2);
    out << I->result.name() << ", initialized using" << std::endl;
    I->blockList->print(out, indent + 2);
  }
  if (!arguments.empty()) out << '>' << std::endl;
}

}
