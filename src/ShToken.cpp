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
