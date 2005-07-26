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
#include "ShCtrlGraph.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include "ShBasicBlock.hpp"
#include "ShToken.hpp"
#include "ShTokenizer.hpp"
#include "ShUtility.hpp"
#include "ShParser.hpp"
#include "ShVariable.hpp"
#include "ShDebug.hpp"

namespace SH {

ShCtrlGraphNode::ShCtrlGraphNode()
  : follower(0), m_marked(false)
{
}

ShCtrlGraphNode::~ShCtrlGraphNode() {
}

std::ostream& ShCtrlGraphNode::print(std::ostream& out, int indent) const
{
  if (marked()) return out;
  mark();
  
  if (block) block->print(out, indent);
  if (follower) {
    shPrintIndent(out, indent);
    out << "->" << std::endl;
    follower->print(out, indent + 2);
  }
  for (SuccessorList::const_iterator I = successors.begin();
       I != successors.end(); ++I) {
    shPrintIndent(out, indent);
    if (!I->cond.null()) {
      out << "[" << I->cond.name() << "]" << std::endl;
    }
    out << "->" << std::endl;
    I->node->print(out, indent + 2);
  }
  return out;
}

std::ostream& ShCtrlGraphNode::graphvizDump(std::ostream& out) const
{
  if (marked()) return out;
  mark();
  out << "\"" << this << "\" ";
  out << " [label=\"";

  if(!m_decls.empty()) {
    out << "DECLS:\\n";
    for(DeclIt I = decl_begin(); I != decl_end(); ++I) {
      out << (*I)->nameOfType() << " " << (*I)->name() << "\\n";
    }
    out << "\\n";
  }

  if (block) {
    out << "CODE:\\n";
    block->graphvizDump(out);
    out << "\", shape=box]";
  } else {
    out << " \", shape=circle]";
  }
  out << ";" << std::endl;

  for (SuccessorList::const_iterator I = successors.begin();
       I != successors.end(); ++I) {
    I->node->graphvizDump(out);
    out << "\"" << this << "\" ";
    out << "-> ";
    out << "\"" << I->node.object() << "\" ";

    out << "[style=dashed, label=\"" << I->cond.name() << "\"]";
    out << ";" << std::endl;
  }

  if (follower) {
    follower->graphvizDump(out);
    out << "\"" << this << "\" ";
    out << "-> ";
    out << "\"" << follower.object() << "\";" << std::endl;
  }
  
  return out;
}

bool ShCtrlGraphNode::marked() const
{
  return m_marked;
}

void ShCtrlGraphNode::mark() const
{
  m_marked = true;
}

void ShCtrlGraphNode::clearMarked() const
{
  if (!marked()) return ;
  m_marked = false;

  if (follower) follower->clearMarked();
  
  for (SuccessorList::const_iterator I = successors.begin();
       I != successors.end(); ++I) {
    I->node->clearMarked();
  }
}

void ShCtrlGraphNode::append(const ShPointer<ShCtrlGraphNode>& node)
{
  if (!node) return;
  assert(!follower);
  follower = node;
}

void ShCtrlGraphNode::append(const ShPointer<ShCtrlGraphNode>& node,
                             ShVariable cond)
{
  if (node) successors.push_back(ShCtrlGraphBranch(node, cond));
}

ShCtrlGraphNodePtr ShCtrlGraphNode::split(ShBasicBlock::ShStmtList::iterator stmt) 
{
  // make a new node to hold the statements after stmt
  // and move over the successors/follower info
  ShCtrlGraphNodePtr after = new ShCtrlGraphNode();
  after->successors = successors;
  after->follower = follower;
  successors.clear();

  // link up the two nodes
  follower = after;
  after->predecessors.push_back(this);

  // make a block for after and split up the statements
  after->block = new ShBasicBlock();
  ++stmt;
  after->block->splice(after->block->begin(), block->m_statements, stmt);

  return after;
}

void ShCtrlGraphNode::addDecl(ShVariableNodePtr node)
{
  m_decls.insert(node);
}

bool ShCtrlGraphNode::hasDecl(ShVariableNodePtr node) const
{
  return m_decls.find(node) != m_decls.end();
}

void ShCtrlGraphNode::insert_decls(DeclIt f, DeclIt l)
{
  m_decls.insert(f, l);
}
ShCtrlGraphNode::DeclIt ShCtrlGraphNode::decl_begin() const
{
  return m_decls.begin();
}

ShCtrlGraphNode::DeclIt ShCtrlGraphNode::decl_end() const
{
  return m_decls.end();
}

ShCtrlGraphBranch::ShCtrlGraphBranch(const ShPointer<ShCtrlGraphNode>& node,
                                     ShVariable cond)
  : node(node), cond(cond)
{
}

ShCtrlGraph::ShCtrlGraph(ShCtrlGraphNodePtr head, ShCtrlGraphNodePtr tail)
  : m_entry(head),
    m_exit(tail)
{
}

ShCtrlGraph::ShCtrlGraph(ShBlockListPtr blocks)
  : m_entry(new ShCtrlGraphNode()),
    m_exit(new ShCtrlGraphNode())
{
  ShCtrlGraphNodePtr head, tail;

  ShParser::parse(head, tail, blocks);

  if(!head && !tail) {
    m_entry->append(m_exit);
  } else {
    SH_DEBUG_ASSERT(head && tail);
    m_entry->append(head);
    tail->append(m_exit);
  }
}

ShCtrlGraph::~ShCtrlGraph()
{
}

ShCtrlGraphNodePtr ShCtrlGraph::entry() const
{
  return m_entry;
}

ShCtrlGraphNodePtr ShCtrlGraph::exit() const
{
  return m_exit;
}

ShCtrlGraphNodePtr ShCtrlGraph::prependEntry() {
  ShCtrlGraphNodePtr newEntry = new ShCtrlGraphNode();
  ShCtrlGraphNodePtr oldEntry = m_entry;
  newEntry->append(oldEntry);
  newEntry->mark();
  
  if( oldEntry->block ) {
    SH_DEBUG_WARN( "Old entry to control graph did not have an empty block!");
  } else {
    oldEntry->block = new ShBasicBlock();
  }
  m_entry = newEntry;
  return oldEntry;
}

ShCtrlGraphNodePtr ShCtrlGraph::appendExit() {
  ShCtrlGraphNodePtr newExit = new ShCtrlGraphNode();
  ShCtrlGraphNodePtr oldExit = m_exit;
  oldExit->append(newExit);
  
  if( oldExit->block ) {
    SH_DEBUG_WARN( "Old exit to control graph did not have an empty block!");
  } else {
    oldExit->block = new ShBasicBlock();
  }
  m_exit = newExit;
  return oldExit;
}

void ShCtrlGraph::prepend(ShPointer<ShCtrlGraph> cfg)
{
  cfg->exit()->append(m_entry);
  m_entry = cfg->entry();
}

void ShCtrlGraph::append(ShPointer<ShCtrlGraph> cfg)
{
  m_exit->append(cfg->entry());
  m_exit = cfg->exit();
}

std::ostream& ShCtrlGraph::print(std::ostream& out, int indent) const
{
  if (m_entry) {
    m_entry->clearMarked();
    m_entry->print(out, indent);
  }
  
  return out;
}

std::ostream& ShCtrlGraph::graphvizDump(std::ostream& out) const
{
  out << "digraph control {" << std::endl;
  
  if (m_entry) {
    m_entry->clearMarked();
    m_entry->graphvizDump(out);
  }
  
  out << "}" << std::endl;
  return out;
}

struct ClearPreds {
  void operator()(ShCtrlGraphNodePtr node) {
    if (!node) return;
    node->predecessors.clear();
  }
};

struct ComputePreds {
  void operator()(ShCtrlGraphNode* node) {
    if (!node) return;
    
    for (ShCtrlGraphNode::SuccessorList::iterator I = node->successors.begin();
         I != node->successors.end(); ++I) {
      if (I->node) I->node->predecessors.push_back(node);
    }
    if (node->follower) node->follower->predecessors.push_back(node);
  }
};


void ShCtrlGraph::computePredecessors()
{
  ClearPreds clear;
  dfs(clear);

  ComputePreds comp;
  dfs(comp);
}

typedef std::map<ShCtrlGraphNodePtr, ShCtrlGraphNodePtr> CtrlGraphCopyMap;

struct CtrlGraphCopier {
  CtrlGraphCopier(CtrlGraphCopyMap& copyMap)
    : copyMap(copyMap)
  {
  }
  
  void operator()(ShCtrlGraphNodePtr node) {
    if (!node) return;
    ShCtrlGraphNodePtr newNode = new ShCtrlGraphNode(*node);
    copyMap[node] = newNode;
  }

  CtrlGraphCopyMap& copyMap;
};

void ShCtrlGraph::copy(ShCtrlGraphNodePtr& newHead, ShCtrlGraphNodePtr& newTail) const
{
  CtrlGraphCopyMap copyMap;
  copyMap[0] = 0;
  
  CtrlGraphCopier copier(copyMap);
  SH_DEBUG_ASSERT(m_entry);
  SH_DEBUG_ASSERT(m_exit); // catch empty tails
  m_entry->clearMarked();
  m_entry->dfs(copier);

  // Replace the successors and followers in the new graph with their new equivalents
  for (CtrlGraphCopyMap::iterator I = copyMap.begin(); I != copyMap.end(); ++I) {
    ShCtrlGraphNodePtr node = I->second; // Get the new node
    if (!node) continue;
    for (ShCtrlGraphNode::SuccessorList::iterator J = node->successors.begin();
         J != node->successors.end(); ++J) {
      J->node = copyMap[J->node];
    }
    node->follower = copyMap[node->follower];
    if (node->block) {
      ShBasicBlockPtr new_block = new ShBasicBlock(*node->block);
      node->block = new_block;
    }
  }
  newHead = copyMap[m_entry];
  newTail = copyMap[m_exit];
  SH_DEBUG_ASSERT(newHead);
  SH_DEBUG_ASSERT(newTail);

  m_entry->clearMarked();
}



}

