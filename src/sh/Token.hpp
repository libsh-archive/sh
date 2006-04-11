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
#ifndef SHTOKEN_HPP
#define SHTOKEN_HPP

#include <vector>
#include "DllExport.hpp"
#include "RefCount.hpp"
#include "Block.hpp"

namespace SH {

struct TokenArgument;
  
/** Possible types a token can have.
 * If you add to this list or change it, be sure to change tokenNames in
 * Token.cpp.
 */
enum TokenType {
  TOKEN_IF,
  TOKEN_ELSE,
  TOKEN_ENDIF,
  TOKEN_WHILE,
  TOKEN_ENDWHILE,
  TOKEN_DO,
  TOKEN_UNTIL,
  TOKEN_FOR,
  TOKEN_ENDFOR,
  TOKEN_BREAK,
  TOKEN_CONTINUE,
  TOKEN_STARTSEC,
  TOKEN_ENDSEC,
  TOKEN_STARTSWITCH,
  TOKEN_ENDSWITCH,
  TOKEN_CASE
};

/** A token in the (unparsed) parse tree.
 * This represents a token such as SH_IF. The token can optionally
 * have some arguments, see TokenArgument. Later these tokens
 * will be parsed into real control structures by the parser.
 */
class 
SH_DLLEXPORT Token : public Block {
public:
  Token(TokenType type);
  ~Token();

  /// Return the type of this token.
  TokenType type();
  
  void print(std::ostream& out, int indent) const;

  /// Any arguments bound to the token. May be empty.
  std::vector<TokenArgument> arguments;
  
private:
  TokenType m_type;
};

typedef Pointer<Token> TokenPtr;
typedef Pointer<const Token> TokenCPtr;


}

#endif
