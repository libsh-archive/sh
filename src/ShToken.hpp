#ifndef SHTOKEN_HPP
#define SHTOKEN_HPP

#include <vector>
#include "ShRefCount.hpp"
#include "ShBlock.hpp"

namespace SH {

class ShTokenArgument;
  
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
};

/** A token in the (unparsed) parse tree.
 * This represents a token such as SH_IF. The token can optionally
 * have some arguments, see ShTokenArgument. Later these tokens
 * will be parsed into real control structures by the parser.
 */
class ShToken : public ShBlock {
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

typedef ShRefCount<ShToken> ShTokenPtr;


}

#endif
