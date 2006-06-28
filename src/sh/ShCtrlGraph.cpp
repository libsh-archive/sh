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

ShCtrlGraphNode::ShCtrlGraphNode(ShCtrlGraph *owner)
  : m_owner(owner), m_follower(0), m_marked(false)
{
  assert(m_owner);
  m_owner->add_owned_node(this);
}

ShCtrlGraphNode::~ShCtrlGraphNode()
{
  // Replace references to us from our predecessors with references to our follower
  while (!predecessors_empty()) {  
    ShCtrlGraphNode* pred = m_predecessors.back();
    for (SuccessorIt s = pred->successors_begin(); s != pred->successors_end(); ++s) {
      if (s->node == this) {
        // This is safe during iteration since it doesn't mess with the list item itself
        pred->replace_successor(s, follower());
      }
    }
    if (pred->follower() == this) {
      pred->follower(follower());
    }

    // That item should now be gone from the list (we've removed all successor references!)
    assert(m_predecessors.empty() || m_predecessors.back() != pred);
  }

  // Release all of our successor references
  for (SuccessorList::iterator i = m_successors.begin(); i != m_successors.end(); ++i) {
    remove_successor_reference(i->node);
  }
  remove_successor_reference(m_follower);
}

std::ostream& ShCtrlGraphNode::print(std::ostream& out, int indent) const
{
  if (marked()) return out;
  mark();
  
  if (block) block->print(out, indent);
  if (m_follower) {
    shPrintIndent(out, indent);
    out << "->" << std::endl;
    m_follower->print(out, indent + 2);
  }
  for (SuccessorList::const_iterator I = m_successors.begin();
       I != m_successors.end(); ++I) {
    shPrintIndent(out, indent);
    if (!I->cond.null()) {
      out << "[" << I->cond.name() << "]" << std::endl;
    }
    out << "->" << std::endl;
    I->node->print(out, indent + 2);
  }
  return out;
}

std::ostream& ShCtrlGraphNode::graphviz_dump(std::ostream& out) const
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
    block->graphviz_dump(out);
    out << "\", shape=box]";
  } else {
    out << " \", shape=circle]";
  }
  out << ";" << std::endl;

  for (SuccessorList::const_iterator I = m_successors.begin();
       I != m_successors.end(); ++I) {
    I->node->graphviz_dump(out);
    out << "\"" << this << "\" ";
    out << "-> ";
    out << "\"" << I->node << "\" ";

    out << "[style=dashed, label=\"" << I->cond.name() << "\"]";
    out << ";" << std::endl;
  }

  if (m_follower) {
    m_follower->graphviz_dump(out);
    out << "\"" << this << "\" ";
    out << "-> ";
    out << "\"" << m_follower << "\";" << std::endl;
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

void ShCtrlGraphNode::clear_marked(bool descend) const
{
  if (!marked()) return;
  m_marked = false;

  if (!descend) return;
  if (m_follower) m_follower->clear_marked();
  
  for (SuccessorList::const_iterator I = m_successors.begin();
       I != m_successors.end(); ++I) {
    I->node->clear_marked();
  }
}

void ShCtrlGraphNode::append(ShCtrlGraphNode* node)
{
  if (node) {
    assert(!m_follower);

    // Take ownership of the new subtree
    node->change_owner(m_owner, true);

    m_follower = node;
    // Update predecessors
    m_follower->m_predecessors.push_back(this);
  }
}

void ShCtrlGraphNode::append(ShCtrlGraphNode* node, ShVariable cond)
{
  if (node) {
    // Take ownership of the new subtree
    node->change_owner(m_owner, true);

    m_successors.push_back(ShCtrlGraphBranch(node, cond));
    // Update predecessors
    node->m_predecessors.push_back(this);
  }
}

//ShCtrlGraphNodePtr ShCtrlGraphNode::split(ShBasicBlock::ShStmtList::iterator stmt) 
//{
//  // make a new node to hold the statements after stmt  
//  ShCtrlGraphNodePtr after = new ShCtrlGraphNode();
//
//  // move over the successors/follower info
//  for (SuccessorIt i = successors_begin(); i != successors_end(); ++i) {
//    append(i->node, i->cond);
//  }
//  successors_clear();
//  after->append(m_follower);
//  m_follower = 0;
//
//  // link up the two nodes
//  append(after);
//
//  // make a block for after and split up the statements
//  after->block = new ShBasicBlock();
//  ++stmt;
//  after->block->splice(after->block->begin(), block->m_statements, stmt);
//
//  return after;
//}

void ShCtrlGraphNode::remove_successor_reference(ShCtrlGraphNode* node)
{
  if (node) {
    // Remove a single matching predecessor link
    PredecessorList::iterator i =
      std::find(node->m_predecessors.begin(), node->m_predecessors.end(), this);
    if (i != node->m_predecessors.end()) {
      node->m_predecessors.erase(i);
    }

    // Check if we were the only predecessor
    if (node->predecessors_empty()) {
      m_owner->release_owned_node(node);
    }
  }
}

ShCtrlGraphNode::SuccessorIt ShCtrlGraphNode::successors_erase(SuccessorIt i)
{
  remove_successor_reference(i->node);
  return m_successors.erase(i);
}

void ShCtrlGraphNode::replace_successor(SuccessorIt current, ShCtrlGraphNode* new_node)
{
  ShCtrlGraphNode *old_node = current->node;
  current->node = 0;

  // Take ownership of the new subtree
  if (new_node) {
    new_node->change_owner(m_owner, true);
    // Update predecessors
    new_node->m_predecessors.push_back(this);
  }
  current->node = new_node;
  
  // Release references to the old node
  remove_successor_reference(old_node);
}

void ShCtrlGraphNode::follower(ShCtrlGraphNode *new_follower)
{
  // Setup the new follower
  ShCtrlGraphNode *old_follower = m_follower;
  m_follower = 0;
  append(new_follower);

  // Release references to the old follower
  remove_successor_reference(old_follower);
}

void ShCtrlGraphNode::addDecl(const ShVariableNodePtr& node)
{
  m_decls.insert(node);
}

bool ShCtrlGraphNode::hasDecl(const ShVariableNodePtr& node) const
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

void ShCtrlGraphNode::change_owner(ShCtrlGraph *new_owner, bool recursive)
{
  // NOTE: This does not handle the case where the ROOT owner matches but some recursive
  // owner does not. However this case should not arise, and this condition provides
  // an early-out to avoid traversing the whole cfg needlessly.
  // It is also an easy way to avoid looping endlessly in the cfg.
  if (new_owner == m_owner) return;

  // Don't allow us to be deleted (naturally) as we're inserting ourselves into a new list
  m_owner->release_owned_node(this, false);
  m_owner = new_owner;
  m_owner->add_owned_node(this);
  

  // If recursive, call on our children (this MUST be last, after marking ourselves)
  if (recursive) {
    for (SuccessorList::iterator i = m_successors.begin(); i != m_successors.end(); ++i) {
      if (i->node) i->node->change_owner(new_owner, recursive);
    }
    if (m_follower) m_follower->change_owner(new_owner, recursive);
  }
}

ShCtrlGraphNode * ShCtrlGraphNode::clone(CloneMap *clone_map) const
{
  // Clone this node
  ShCtrlGraphNode *new_node = new ShCtrlGraphNode(m_owner);  
  new_node->m_marked = m_marked;
  new_node->m_decls = m_decls;
  new_node->block = block ? new ShBasicBlock(*block) : 0;
  

  (*clone_map)[this] = new_node;

  // We deal with successors and predecessors manually to maintain the
  // cloned graph structure.
  new_node->m_successors.clear();
  new_node->m_predecessors.clear();
  new_node->m_follower = 0;

  // Clone successors as necessary
  for (SuccessorList::const_iterator i = m_successors.begin();
       i != m_successors.end(); ++i) {
    if (i->node) {
      CloneMap::const_iterator j = clone_map->find(i->node);
      if (j == clone_map->end()) {
        // Create a new node
        new_node->append(i->node->clone(clone_map), i->cond);
      } else {
        // We've already cloned this successor, so just point to it
        new_node->append(j->second, i->cond);
      }
    }
  }

  // Clone follower
  if (m_follower) {
    CloneMap::const_iterator j = clone_map->find(m_follower);
    if (j == clone_map->end()) {
      new_node->append(m_follower->clone(clone_map));
    } else {
      new_node->append(j->second);
    }
  }

  // Return the new node
  return new_node;
}

ShCtrlGraphNode * ShCtrlGraphNode::clone() const
{
  CloneMap *clone_map = new CloneMap();
  ShCtrlGraphNode *cloned = clone(clone_map);
  delete clone_map;
  return cloned;
}




ShCtrlGraph::ShCtrlGraph(ShCtrlGraphNode* head, ShCtrlGraphNode* tail)
  : m_entry(head), m_exit(tail)
{
  // Take ownership of the given graph
  m_entry->change_owner(this, true);

  // Make sure the graph actually contained both the head and tail
  assert(m_owned_nodes.count(head) > 0 && m_owned_nodes.count(tail) > 0);
}

ShCtrlGraph::ShCtrlGraph(const ShBlockListPtr& blocks)
{
  m_entry = new ShCtrlGraphNode(this);
  m_exit = new ShCtrlGraphNode(this);

  ShCtrlGraphNode* head = 0;
  ShCtrlGraphNode* tail = 0;
  ShParser parser(this);
  parser.parse(head, tail, blocks);

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
  release_all_owned_nodes();
}

void ShCtrlGraph::release_owned_node(ShCtrlGraphNode* node, bool allow_delete)
{
  // Careful here so that we don't ever redestruct something currently being destructed
  // i.e. we have to handle the re-entrant case correctly.
  // Also ONLY destruct something that belongs to us!
  NodeSet::iterator i = m_owned_nodes.find(node);
  if (i != m_owned_nodes.end()) {
    m_owned_nodes.erase(i);
    if (allow_delete) delete node;
  }
}

void ShCtrlGraph::release_all_owned_nodes()
{
  // Remove one by one
  while (!m_owned_nodes.empty()) {
    release_owned_node(*m_owned_nodes.begin());
  }
}

ShCtrlGraphNode* ShCtrlGraph::prepend_entry()
{
  ShCtrlGraphNode* newEntry = new ShCtrlGraphNode(this);
  ShCtrlGraphNode* oldEntry = m_entry;
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

ShCtrlGraphNode* ShCtrlGraph::append_exit()
{
  ShCtrlGraphNode* newExit = new ShCtrlGraphNode(this);
  ShCtrlGraphNode* oldExit = m_exit;
  oldExit->append(newExit);
  
  if( oldExit->block ) {
    SH_DEBUG_WARN( "Old exit to control graph did not have an empty block!");
  } else {
    oldExit->block = new ShBasicBlock();
  }
  m_exit = newExit;
  return oldExit;
}

void ShCtrlGraph::prepend(const std::auto_ptr<ShCtrlGraph> cfg)
{
  // Take ownership of their graph
  cfg->m_entry->change_owner(this, true);
  
  // Attach it before us
  cfg->m_exit->append(m_entry);
  m_entry = cfg->m_entry;
}

void ShCtrlGraph::append(const std::auto_ptr<ShCtrlGraph> cfg)
{
  // Take ownership of their graph
  cfg->m_entry->change_owner(this, true);

  // Attach it after us
  m_exit->append(cfg->m_entry);
  m_exit = cfg->m_exit;
}

std::ostream& ShCtrlGraph::print(std::ostream& out, int indent) const
{
  if (m_entry) {
    clear_marked();
    m_entry->print(out, indent);
    clear_marked();
  }
  
  return out;
}

std::ostream& ShCtrlGraph::graphviz_dump(std::ostream& out) const
{
  out << "digraph control {" << std::endl;
  
  if (m_entry) {
    clear_marked();
    m_entry->graphviz_dump(out);
    clear_marked();
  }
  
  out << "}" << std::endl;
  return out;
}

void ShCtrlGraph::clear_marked() const
{
  for (NodeSet::const_iterator I = m_owned_nodes.begin(); I != m_owned_nodes.end(); ++I) {
    (*I)->clear_marked();
  }
}

ShCtrlGraph * ShCtrlGraph::clone() const
{
  // Use our own clone map
  ShCtrlGraphNode::CloneMap *clone_map = new ShCtrlGraphNode::CloneMap();
  
  // Clone the graph and find the new tail
  ShCtrlGraphNode *newHead = m_entry->clone(clone_map);
  ShCtrlGraphNode *newTail = (*clone_map)[m_exit];

  assert(newHead && newTail);    // Check for a malformed control graph
  
  // Construct a new graph and return it
  delete clone_map;
  return new ShCtrlGraph(newHead, newTail);
}

}

