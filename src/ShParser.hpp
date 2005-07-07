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
