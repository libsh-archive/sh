#include "ShCtrlGraph.hpp"
#include <iostream>
#include <cassert>
#include "ShBasicBlock.hpp"
#include "ShToken.hpp"
#include "ShUtility.hpp"
#include "ShParser.hpp"

namespace SH {

ShCtrlGraphNode::ShCtrlGraphNode()
  : follower(0), m_marked(false)
{
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

void ShCtrlGraphNode::append(const ShRefCount<ShCtrlGraphNode>& node)
{
  if (!node) return;
  assert(!follower);
  follower = node;
}

void ShCtrlGraphNode::append(const ShRefCount<ShCtrlGraphNode>& node,
                             ShVariable cond)
{
  if (node) successors.push_back(ShCtrlGraphBranch(node, cond));
}

ShCtrlGraphBranch::ShCtrlGraphBranch(const ShRefCount<ShCtrlGraphNode>& node,
                                     ShVariable cond)
  : node(node), cond(cond)
{
}

ShCtrlGraph::ShCtrlGraph(ShBlockListPtr blocks)
  : m_entry(new ShCtrlGraphNode()),
    m_exit(new ShCtrlGraphNode())
{
  ShCtrlGraphNodePtr head, tail;

  ShParser::parse(head, tail, blocks);
  m_entry->append(head);
  if (tail) tail->append(m_exit);
}

ShCtrlGraph::~ShCtrlGraph()
{
}

ShCtrlGraphNodePtr ShCtrlGraph::entry() const
{
  return m_entry;
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

}

