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
