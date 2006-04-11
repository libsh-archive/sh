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
#ifndef SHPARSER_HPP
#define SHPARSER_HPP

#include <stack>
#include "DllExport.hpp"
#include "CtrlGraph.hpp"
#include "Token.hpp"

namespace SH {

/** Recursive-descent parser for control structures.
 * This parser takes a list of blocks containing tokens and basic
 * blocks and parses it into a control graph.
 */
class
DLLEXPORT Parser {
public:
  /// Return a pointer to the singleton instance
  static Parser* instance();

  /** Parse blocks into the control graph between head and tail.
   */
  void parse(CtrlGraphNodePtr& head, CtrlGraphNodePtr& tail, const BlockListPtr& blocks);
  
private:
  // This class is a singleton
  Parser() {}

  void parseBlock(CtrlGraphNodePtr& head, CtrlGraphNodePtr& tail, const BasicBlockPtr& block);
  void parseStmts(CtrlGraphNodePtr& head, CtrlGraphNodePtr& tail, const BlockListPtr& blocks);
  void parseIf(CtrlGraphNodePtr& head, CtrlGraphNodePtr& tail, const BlockListPtr& blocks);
  void parseFor(CtrlGraphNodePtr& head, CtrlGraphNodePtr& tail, const BlockListPtr& blocks);
  void parseWhile(CtrlGraphNodePtr& head, CtrlGraphNodePtr& tail, const BlockListPtr& blocks);
  void parseDo(CtrlGraphNodePtr& head, CtrlGraphNodePtr& tail, const BlockListPtr& blocks);
  void parseSection(CtrlGraphNodePtr& head, CtrlGraphNodePtr& tail, const BlockListPtr& blocks);
  void parse_break(CtrlGraphNodePtr& head, CtrlGraphNodePtr& tail, const BlockListPtr& blocks);
  void parse_continue(CtrlGraphNodePtr& head, CtrlGraphNodePtr& tail, const BlockListPtr& blocks);

  /// Just pops a token
  TokenPtr popToken(const BlockListPtr& blocks);
  /// Checks if front of block list matches expected, pops, and returns
  TokenPtr popToken(const BlockListPtr& blocks, TokenType expectedType, unsigned int expectedArgs=0);

  /// Push the break and continue scope
  void push_scope(CtrlGraphNodePtr& break_node, CtrlGraphNodePtr& continue_node);
  /// Pop the break and continue scope
  void pop_scope();

  // Stacks that keep track of the current scope
  std::stack<CtrlGraphNodePtr> m_break_node;    // for SH_BREAK
  std::stack<CtrlGraphNodePtr> m_continue_node; // for SH_CONTINUE

  // NOT IMPLEMENTED
  Parser(const Parser&);
};
 
}

#endif
