// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#ifndef SHPARSER_HPP
#define SHPARSER_HPP

#include "ShDllExport.hpp"
#include "ShCtrlGraph.hpp"
#include "ShToken.hpp"

namespace SH {

/** Recursive-descent parser for control structures.
 * This parser takes a list of blocks containing tokens and basic
 * blocks and parses it into a control graph.
 */
class
SH_DLLEXPORT ShParser {
public:
  /** Parse blocks into the control graph between head and tail.
   */
  static void parse(ShCtrlGraphNodePtr& head,
                    ShCtrlGraphNodePtr& tail,
                    ShBlockListPtr blocks);
  
private:
  static void parseBlock(ShCtrlGraphNodePtr& head, 
                         ShCtrlGraphNodePtr& tail,
                         ShBasicBlockPtr block);
  static void parseStmts(ShCtrlGraphNodePtr& head,
                         ShCtrlGraphNodePtr& tail,
                         ShBlockListPtr blocks);
  static void parseIf(ShCtrlGraphNodePtr& head,
                      ShCtrlGraphNodePtr& tail,
                      ShBlockListPtr blocks);
  static void parseFor(ShCtrlGraphNodePtr& head,
                       ShCtrlGraphNodePtr& tail,
                       ShBlockListPtr blocks);
  static void parseWhile(ShCtrlGraphNodePtr& head,
                         ShCtrlGraphNodePtr& tail,
                         ShBlockListPtr blocks);
  static void parseDo(ShCtrlGraphNodePtr& head,
                      ShCtrlGraphNodePtr& tail,
                      ShBlockListPtr blocks);
  static void parseSection(ShCtrlGraphNodePtr& head,
                           ShCtrlGraphNodePtr& tail,
                           ShBlockListPtr blocks);

  /* Just pops a token */
  static ShTokenPtr popToken(ShBlockListPtr blocks);

  /* Checks if front of block list matches expected, pops, and returns */
  static ShTokenPtr popToken(ShBlockListPtr blocks, ShTokenType expectedType, unsigned int expectedArgs=0);


  // NOT IMPLEMENTED
  ShParser();
  ShParser(const ShParser&);
  ~ShParser();
};
 
}

#endif
