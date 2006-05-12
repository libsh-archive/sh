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
  ShParser(ShCtrlGraph *graph)
    : m_graph(graph)
  {}

  /** Parse blocks into the control graph between head and tail.
   */
  void parse(ShCtrlGraphNode*& head, ShCtrlGraphNode*& tail, const ShBlockListPtr& blocks);
  
private:
  void parseBlock(ShCtrlGraphNode*& head, ShCtrlGraphNode*& tail, const ShBasicBlockPtr& block);
  void parseStmts(ShCtrlGraphNode*& head, ShCtrlGraphNode*& tail, const ShBlockListPtr& blocks);
  void parseIf(ShCtrlGraphNode*& head, ShCtrlGraphNode*& tail, const ShBlockListPtr& blocks);
  void parseFor(ShCtrlGraphNode*& head, ShCtrlGraphNode*& tail, const ShBlockListPtr& blocks);
  void parseWhile(ShCtrlGraphNode*& head, ShCtrlGraphNode*& tail, const ShBlockListPtr& blocks);
  void parseDo(ShCtrlGraphNode*& head, ShCtrlGraphNode*& tail, const ShBlockListPtr& blocks);
  void parseSection(ShCtrlGraphNode*& head, ShCtrlGraphNode*& tail, const ShBlockListPtr& blocks);
  void parse_break(ShCtrlGraphNode*& head, ShCtrlGraphNode*& tail, const ShBlockListPtr& blocks);
  void parse_continue(ShCtrlGraphNode*& head, ShCtrlGraphNode*& tail, const ShBlockListPtr& blocks);

  /// Just pops a token
  ShTokenPtr popToken(const ShBlockListPtr& blocks);
  /// Checks if front of block list matches expected, pops, and returns
  ShTokenPtr popToken(const ShBlockListPtr& blocks, ShTokenType expectedType, unsigned int expectedArgs=0);

  /// Push the break and continue scope
  void push_scope(ShCtrlGraphNode*& break_node, ShCtrlGraphNode*& continue_node);
  /// Pop the break and continue scope
  void pop_scope();

  // Stacks that keep track of the current scope
  std::stack<ShCtrlGraphNode*> m_break_node;    // for SH_BREAK
  std::stack<ShCtrlGraphNode*> m_continue_node; // for SH_CONTINUE

  // The current control graph that new nodes should be owned by
  ShCtrlGraph *m_graph;

  // NOT IMPLEMENTED
  ShParser(const ShParser&);
};
 
}

#endif
