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
#include "ShDllExport.hpp"
#include "ShRefCount.hpp"
#include "ShBlock.hpp"

namespace SH {

struct ShTokenArgument;
  
/** Possible types a token can have.
 * If you add to this list or change it, be sure to change tokenNames in
 * ShToken.cpp.
 */
enum ShTokenType {
  SH_TOKEN_IF,
  SH_TOKEN_ELSE,
  SH_TOKEN_ENDIF,
  SH_TOKEN_WHILE,
  SH_TOKEN_ENDWHILE,
  SH_TOKEN_DO,
  SH_TOKEN_UNTIL,
  SH_TOKEN_FOR,
  SH_TOKEN_ENDFOR,
  SH_TOKEN_BREAK,
  SH_TOKEN_CONTINUE,
  SH_TOKEN_STARTSEC,
  SH_TOKEN_ENDSEC,
  SH_TOKEN_STARTSWITCH,
  SH_TOKEN_ENDSWITCH,
  SH_TOKEN_CASE
};

/** A token in the (unparsed) parse tree.
 * This represents a token such as SH_IF. The token can optionally
 * have some arguments, see ShTokenArgument. Later these tokens
 * will be parsed into real control structures by the parser.
 */
class 
SH_DLLEXPORT ShToken : public ShBlock {
public:
  ShToken(ShTokenType type);
  ~ShToken();

  /// Return the type of this token.
  ShTokenType type();
  
  void print(std::ostream& out, int indent) const;

  /// Any arguments bound to the token. May be empty.
  std::vector<ShTokenArgument> arguments;
  
private:
  ShTokenType m_type;
};

typedef ShPointer<ShToken> ShTokenPtr;
typedef ShPointer<const ShToken> ShTokenCPtr;


}

#endif
