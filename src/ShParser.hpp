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

#include <stack>
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
  /// Return a pointer to the singleton instance
  static ShParser* instance();

  /** Parse blocks into the control graph between head and tail.
   */
  void parse(ShCtrlGraphNodePtr& head, ShCtrlGraphNodePtr& tail, const ShBlockListPtr& blocks);
  
private:
  // This class is a singleton
  ShParser() {}

  void parseBlock(ShCtrlGraphNodePtr& head, ShCtrlGraphNodePtr& tail, const ShBasicBlockPtr& block);
  void parseStmts(ShCtrlGraphNodePtr& head, ShCtrlGraphNodePtr& tail, const ShBlockListPtr& blocks);
  void parseIf(ShCtrlGraphNodePtr& head, ShCtrlGraphNodePtr& tail, const ShBlockListPtr& blocks);
  void parseFor(ShCtrlGraphNodePtr& head, ShCtrlGraphNodePtr& tail, const ShBlockListPtr& blocks);
  void parseWhile(ShCtrlGraphNodePtr& head, ShCtrlGraphNodePtr& tail, const ShBlockListPtr& blocks);
  void parseDo(ShCtrlGraphNodePtr& head, ShCtrlGraphNodePtr& tail, const ShBlockListPtr& blocks);
  void parseSection(ShCtrlGraphNodePtr& head, ShCtrlGraphNodePtr& tail, const ShBlockListPtr& blocks);
  void parse_break(ShCtrlGraphNodePtr& head, ShCtrlGraphNodePtr& tail, const ShBlockListPtr& blocks);
  void parse_continue(ShCtrlGraphNodePtr& head, ShCtrlGraphNodePtr& tail, const ShBlockListPtr& blocks);

  /// Just pops a token
  ShTokenPtr popToken(const ShBlockListPtr& blocks);
  /// Checks if front of block list matches expected, pops, and returns
  ShTokenPtr popToken(const ShBlockListPtr& blocks, ShTokenType expectedType, unsigned int expectedArgs=0);

  /// Push the break and continue scope
  void push_scope(ShCtrlGraphNodePtr& break_node, ShCtrlGraphNodePtr& continue_node);
  /// Pop the break and continue scope
  void pop_scope();

  // Stacks that keep track of the current scope
  std::stack<ShCtrlGraphNodePtr> m_break_node;    // for SH_BREAK
  std::stack<ShCtrlGraphNodePtr> m_continue_node; // for SH_CONTINUE

  // NOT IMPLEMENTED
  ShParser(const ShParser&);
};
 
}

#endif
