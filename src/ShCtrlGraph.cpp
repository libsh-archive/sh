// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
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
#include "ShCtrlGraph.hpp"
#include <iostream>
#include <cassert>
#include "ShBasicBlock.hpp"
#include "ShToken.hpp"
#include "ShUtility.hpp"
#include "ShParser.hpp"
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
  for (std::vector<ShCtrlGraphBranch>::const_iterator I = successors.begin();
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
  if (block) {
    out << " [label=\"";
    block->graphvizDump(out);
    out << "\", shape=box]";
  } else {
    out << " [label=\"\", shape=circle, height=0.25]";
  }
  out << ";" << std::endl;

  for (std::vector<ShCtrlGraphBranch>::const_iterator I = successors.begin();
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
  
  for (std::vector<ShCtrlGraphBranch>::const_iterator I = successors.begin();
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
    
    for (std::vector<ShCtrlGraphBranch>::iterator I = node->successors.begin();
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


}

