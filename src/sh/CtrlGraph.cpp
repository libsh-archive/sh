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
#include "CtrlGraph.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include "BasicBlock.hpp"
#include "Token.hpp"
#include "Tokenizer.hpp"
#include "Utility.hpp"
#include "Parser.hpp"
#include "Variable.hpp"
#include "Debug.hpp"

namespace SH {

CtrlGraphNode::CtrlGraphNode()
  : follower(0), m_marked(false)
{
}

CtrlGraphNode::~CtrlGraphNode() {
}

std::ostream& CtrlGraphNode::print(std::ostream& out, int indent) const
{
  if (marked()) return out;
  mark();
  
  if (block) block->print(out, indent);
  if (follower) {
    printIndent(out, indent);
    out << "->" << std::endl;
    follower->print(out, indent + 2);
  }
  for (SuccessorList::const_iterator I = successors.begin();
       I != successors.end(); ++I) {
    printIndent(out, indent);
    if (!I->cond.null()) {
      out << "[" << I->cond.name() << "]" << std::endl;
    }
    out << "->" << std::endl;
    I->node->print(out, indent + 2);
  }
  return out;
}

std::ostream& CtrlGraphNode::graphvizDump(std::ostream& out) const
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

bool CtrlGraphNode::marked() const
{
  return m_marked;
}

void CtrlGraphNode::mark() const
{
  m_marked = true;
}

void CtrlGraphNode::clearMarked() const
{
  if (!marked()) return ;
  m_marked = false;

  if (follower) follower->clearMarked();
  
  for (SuccessorList::const_iterator I = successors.begin();
       I != successors.end(); ++I) {
    I->node->clearMarked();
  }
}

void CtrlGraphNode::append(const Pointer<CtrlGraphNode>& node)
{
  if (!node) return;
  assert(!follower);
  follower = node;
}

void CtrlGraphNode::append(const Pointer<CtrlGraphNode>& node,
                             Variable cond)
{
  if (node) successors.push_back(CtrlGraphBranch(node, cond));
}

CtrlGraphNodePtr CtrlGraphNode::split(BasicBlock::StmtList::iterator stmt) 
{
  // make a new node to hold the statements after stmt
  // and move over the successors/follower info
  CtrlGraphNodePtr after = new CtrlGraphNode();
  after->successors = successors;
  after->follower = follower;
  successors.clear();

  // link up the two nodes
  follower = after;
  after->predecessors.push_back(this);

  // make a block for after and split up the statements
  after->block = new BasicBlock();
  ++stmt;
  after->block->splice(after->block->begin(), block->m_statements, stmt);

  return after;
}

void CtrlGraphNode::addDecl(const VariableNodePtr& node)
{
  m_decls.insert(node);
}

bool CtrlGraphNode::hasDecl(const VariableNodePtr& node) const
{
  return m_decls.find(node) != m_decls.end();
}

void CtrlGraphNode::insert_decls(DeclIt f, DeclIt l)
{
  m_decls.insert(f, l);
}
CtrlGraphNode::DeclIt CtrlGraphNode::decl_begin() const
{
  return m_decls.begin();
}

CtrlGraphNode::DeclIt CtrlGraphNode::decl_end() const
{
  return m_decls.end();
}

CtrlGraphBranch::CtrlGraphBranch(const Pointer<CtrlGraphNode>& node,
                                     Variable cond)
  : node(node), cond(cond)
{
}

CtrlGraph::CtrlGraph(const CtrlGraphNodePtr& head, const CtrlGraphNodePtr& tail)
  : m_entry(head),
    m_exit(tail)
{
}

CtrlGraph::CtrlGraph(const BlockListPtr& blocks)
  : m_entry(new CtrlGraphNode()),
    m_exit(new CtrlGraphNode())
{
  CtrlGraphNodePtr head, tail;

  Parser::instance()->parse(head, tail, blocks);

  if(!head && !tail) {
    m_entry->append(m_exit);
  } else {
    SH_DEBUG_ASSERT(head && tail);
    m_entry->append(head);
    tail->append(m_exit);
  }
}

CtrlGraph::~CtrlGraph()
{
}

CtrlGraphNodePtr CtrlGraph::entry() const
{
  return m_entry;
}

CtrlGraphNodePtr CtrlGraph::exit() const
{
  return m_exit;
}

CtrlGraphNodePtr CtrlGraph::prependEntry() {
  CtrlGraphNodePtr newEntry = new CtrlGraphNode();
  CtrlGraphNodePtr oldEntry = m_entry;
  newEntry->append(oldEntry);
  newEntry->mark();
  
  if( oldEntry->block ) {
    SH_DEBUG_WARN( "Old entry to control graph did not have an empty block!");
  } else {
    oldEntry->block = new BasicBlock();
  }
  m_entry = newEntry;
  return oldEntry;
}

CtrlGraphNodePtr CtrlGraph::appendExit() {
  CtrlGraphNodePtr newExit = new CtrlGraphNode();
  CtrlGraphNodePtr oldExit = m_exit;
  oldExit->append(newExit);
  
  if( oldExit->block ) {
    SH_DEBUG_WARN( "Old exit to control graph did not have an empty block!");
  } else {
    oldExit->block = new BasicBlock();
  }
  m_exit = newExit;
  return oldExit;
}

void CtrlGraph::prepend(Pointer<CtrlGraph> cfg)
{
  cfg->exit()->append(m_entry);
  m_entry = cfg->entry();
}

void CtrlGraph::append(Pointer<CtrlGraph> cfg)
{
  m_exit->append(cfg->entry());
  m_exit = cfg->exit();
}

std::ostream& CtrlGraph::print(std::ostream& out, int indent) const
{
  if (m_entry) {
    m_entry->clearMarked();
    m_entry->print(out, indent);
  }
  
  return out;
}

std::ostream& CtrlGraph::graphvizDump(std::ostream& out) const
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
  void operator()(const CtrlGraphNodePtr& node) {
    if (!node) return;
    node->predecessors.clear();
  }
};

struct ComputePreds {
  void operator()(CtrlGraphNode* node) {
    if (!node) return;
    
    for (CtrlGraphNode::SuccessorList::iterator I = node->successors.begin();
         I != node->successors.end(); ++I) {
      if (I->node) I->node->predecessors.push_back(node);
    }
    if (node->follower) node->follower->predecessors.push_back(node);
  }
};


void CtrlGraph::computePredecessors()
{
  ClearPreds clear;
  dfs(clear);

  ComputePreds comp;
  dfs(comp);
}

typedef std::map<CtrlGraphNodePtr, CtrlGraphNodePtr> CtrlGraphCopyMap;

struct CtrlGraphCopier {
  CtrlGraphCopier(CtrlGraphCopyMap& copyMap)
    : copyMap(copyMap)
  {
  }

  // assignment operator could not be generated: declaration only
  CtrlGraphCopier& operator=(CtrlGraphCopier const&);
  
  void operator()(const CtrlGraphNodePtr& node) {
    if (!node) return;
    CtrlGraphNodePtr newNode = new CtrlGraphNode(*node);
    copyMap[node] = newNode;
  }

  CtrlGraphCopyMap& copyMap;
};

void CtrlGraph::copy(CtrlGraphNodePtr& newHead, CtrlGraphNodePtr& newTail) const
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
    CtrlGraphNodePtr node = I->second; // Get the new node
    if (!node) continue;
    for (CtrlGraphNode::SuccessorList::iterator J = node->successors.begin();
         J != node->successors.end(); ++J) {
      J->node = copyMap[J->node];
    }
    node->follower = copyMap[node->follower];
    if (node->block) {
      BasicBlockPtr new_block = new BasicBlock(*node->block);
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

