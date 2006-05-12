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
SH_DLLEXPORT Parser {
public:
  Parser(CtrlGraph *graph)
    : m_graph(graph)
  {}

  /** Parse blocks into the control graph between head and tail.
   */
  void parse(CtrlGraphNode*& head, CtrlGraphNode*& tail, const BlockListPtr& blocks);
  
private:
  void parseBlock(CtrlGraphNode*& head, CtrlGraphNode*& tail, const BasicBlockPtr& block);
  void parseStmts(CtrlGraphNode*& head, CtrlGraphNode*& tail, const BlockListPtr& blocks);
  void parseIf(CtrlGraphNode*& head, CtrlGraphNode*& tail, const BlockListPtr& blocks);
  void parseFor(CtrlGraphNode*& head, CtrlGraphNode*& tail, const BlockListPtr& blocks);
  void parseWhile(CtrlGraphNode*& head, CtrlGraphNode*& tail, const BlockListPtr& blocks);
  void parseDo(CtrlGraphNode*& head, CtrlGraphNode*& tail, const BlockListPtr& blocks);
  void parseSection(CtrlGraphNode*& head, CtrlGraphNode*& tail, const BlockListPtr& blocks);
  void parse_break(CtrlGraphNode*& head, CtrlGraphNode*& tail, const BlockListPtr& blocks);
  void parse_continue(CtrlGraphNode*& head, CtrlGraphNode*& tail, const BlockListPtr& blocks);

  /// Just pops a token
  TokenPtr popToken(const BlockListPtr& blocks);
  /// Checks if front of block list matches expected, pops, and returns
  TokenPtr popToken(const BlockListPtr& blocks, TokenType expectedType, unsigned int expectedArgs=0);

  /// Push the break and continue scope
  void push_scope(CtrlGraphNode*& break_node, CtrlGraphNode*& continue_node);
  /// Pop the break and continue scope
  void pop_scope();

  // Stacks that keep track of the current scope
  std::stack<CtrlGraphNode*> m_break_node;    // for SH_BREAK
  std::stack<CtrlGraphNode*> m_continue_node; // for SH_CONTINUE

  // The current control graph that new nodes should be owned by
  CtrlGraph *m_graph;

  // NOT IMPLEMENTED
  Parser(const Parser&);
};
 
}

#endif
