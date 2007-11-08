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

CtrlGraphNode::CtrlGraphNode(CtrlGraph *owner)
  : m_owner(owner), m_follower(0), m_marked(false)
{
  assert(m_owner);
  m_owner->add_owned_node(this);
}

CtrlGraphNode::~CtrlGraphNode()
{
  // Replace references to us from our predecessors with references to our follower
  while (!predecessors_empty()) {  
    CtrlGraphNode* pred = m_predecessors.back();
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

std::ostream& CtrlGraphNode::print(std::ostream& out, int indent) const
{
  if (marked()) return out;
  mark();
  
  if (block) block->print(out, indent);
  if (m_follower) {
    printIndent(out, indent);
    out << "->" << std::endl;
    m_follower->print(out, indent + 2);
  }
  for (SuccessorList::const_iterator I = m_successors.begin();
       I != m_successors.end(); ++I) {
    printIndent(out, indent);
    if (!I->cond.null()) {
      out << "[" << I->cond.name() << "]" << std::endl;
    }
    out << "->" << std::endl;
    I->node->print(out, indent + 2);
  }
  return out;
}

std::ostream& CtrlGraphNode::graphviz_dump(std::ostream& out) const
{
  if (marked()) return out;
  mark();
  out << "\"" << this << "\" ";
  out << " [label=\"";
#if 1
  out << "object: " << this << "\\n";
  out << "owner: " << m_owner<< "\\n";
  out << "follower: " << m_follower << "\\n";
  out << "predecessors: "; 
  for(PredecessorList::const_iterator P = m_predecessors.begin(); P != m_predecessors.end(); ++P) {
    out << " " << *P;
  }
  out << "\\n";
  out << "successors:\\n";
  for(SuccessorList::const_iterator S = m_successors.begin(); S != m_successors.end(); ++S) {
    out << "    "; 
    if(!S->cond.null()) {
      out << S->cond.name() << " : ";
    }
    out << S->node << "\\n"; 
  }
#endif


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

bool CtrlGraphNode::marked() const
{
  return m_marked;
}


void CtrlGraphNode::append(CtrlGraphNode* node)
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

void CtrlGraphNode::append(CtrlGraphNode* node, Variable cond)
{
  if (node) {
    // Take ownership of the new subtree
    node->change_owner(m_owner, true);

    m_successors.push_back(CtrlGraphBranch(node, cond));
    // Update predecessors
    node->m_predecessors.push_back(this);
  }
}

CtrlGraphNode* CtrlGraphNode::split(BasicBlock::StmtList::iterator stmt) 
{
  // make a new node to hold the statements after stmt  
  CtrlGraphNode* after = new CtrlGraphNode(this->m_owner);

  // move over the successors/follower info
  for (SuccessorIt i = successors_begin(); i != successors_end(); ++i) {
    after->append(i->node, i->cond);
    remove_successor_reference(i->node);
  }
  m_successors.clear();
  after->append(m_follower);
  remove_successor_reference(m_follower);
  m_follower = 0;

  // link up the two nodes
  append(after);

  // make a block for after and split up the statements
  after->block = new BasicBlock();
  ++stmt;
  after->block->splice(after->block->begin(), block->m_statements, stmt);

  return after;
}

void CtrlGraphNode::remove_successor_reference(CtrlGraphNode* node)
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

CtrlGraphNode::SuccessorIt CtrlGraphNode::successors_erase(SuccessorIt i)
{
  remove_successor_reference(i->node);
  return m_successors.erase(i);
}

void CtrlGraphNode::replace_successor(SuccessorIt current, CtrlGraphNode* new_node)
{
  CtrlGraphNode *old_node = current->node;
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

void CtrlGraphNode::follower(CtrlGraphNode *new_follower)
{
  // Setup the new follower
  CtrlGraphNode *old_follower = m_follower;
  m_follower = 0;
  append(new_follower);

  // Release references to the old follower
  remove_successor_reference(old_follower);
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

void CtrlGraphNode::change_owner(CtrlGraph *new_owner, bool recursive)
{
  // NOTE: This does not handle the case where the ROOT owner matches but some recursive
  // owner does not. However this case should not arise, and this condition provides
  // an early-out to avoid traversing the whole cfg needlessly.
  // It is also an easy way to avoid looping endlessly in the cfg.
  if (new_owner == m_owner) return;

  //SH_DEBUG_PRINT("Changing owner from " << m_owner << " to " << new_owner);

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

CtrlGraphNode * CtrlGraphNode::clone(CloneMap *clone_map) const
{
  // Clone this node
  CtrlGraphNode *new_node = new CtrlGraphNode(m_owner);  
  new_node->m_marked = m_marked;
  new_node->m_decls = m_decls;
  new_node->block = block ? new BasicBlock(*block) : 0;
  

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

bool CtrlGraph::delete_enabled = true;
CtrlGraph* CtrlGraph::forced_owner = 0;

void CtrlGraph::clear_marked() const
{
  for(NodeSet::const_iterator N = m_owned_nodes.begin(); N != m_owned_nodes.end(); ++N) {
    (*N)->unmark();
  }
}

CtrlGraphNode * CtrlGraphNode::clone() const
{
  CloneMap *clone_map = new CloneMap();
  CtrlGraphNode *cloned = clone(clone_map);
  delete clone_map;
  return cloned;
}




CtrlGraph::CtrlGraph(CtrlGraphNode* head, CtrlGraphNode* tail)
  : m_entry(head), m_exit(tail)
{
  //SH_DEBUG_PRINT("Creating CtrlGraph with given head/tail " << this);
  // Take ownership of the given graph
  m_entry->change_owner(this, true);

  // Make sure the graph actually contained both the head and tail
  assert(m_owned_nodes.count(head) > 0 && m_owned_nodes.count(tail) > 0);
}

CtrlGraph::CtrlGraph(const BlockListPtr& blocks)
{
  //SH_DEBUG_PRINT("Creating CtrlGraph " << this);
  m_entry = new CtrlGraphNode(this);
  m_exit = new CtrlGraphNode(this);

  CtrlGraphNode* head = 0;
  CtrlGraphNode* tail = 0;
  Parser parser(this);
  parser.parse(head, tail, blocks);

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
  //SH_DEBUG_PRINT("Deleting CtrlGraph " << this);
  release_all_owned_nodes();
}

void CtrlGraph::release_owned_node(CtrlGraphNode* node, bool allow_delete)
{
  // Careful here so that we don't ever redestruct something currently being destructed
  // i.e. we have to handle the re-entrant case correctly.
  // Also ONLY destruct something that belongs to us!
  NodeSet::iterator i = m_owned_nodes.find(node);
  if (i != m_owned_nodes.end()) {
    // SH_DEBUG_PRINT(this << " releasing " << node);
    m_owned_nodes.erase(i);
    if (allow_delete && delete_enabled) {
      delete node;
    }
  }
}

void CtrlGraph::release_all_owned_nodes()
{
  // Remove one by one
  while (!m_owned_nodes.empty()) {
    release_owned_node(*m_owned_nodes.begin());
  }
}

CtrlGraphNode* CtrlGraph::prepend_entry()
{
  CtrlGraphNode* newEntry = new CtrlGraphNode(this);
  CtrlGraphNode* oldEntry = m_entry;
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

CtrlGraphNode* CtrlGraph::append_exit()
{
  CtrlGraphNode* newExit = new CtrlGraphNode(this);
  CtrlGraphNode* oldExit = m_exit;
  oldExit->append(newExit);
  
  if( oldExit->block ) {
    SH_DEBUG_WARN( "Old exit to control graph did not have an empty block!");
  } else {
    oldExit->block = new BasicBlock();
  }
  m_exit = newExit;
  return oldExit;
}

void CtrlGraph::prepend(const std::auto_ptr<CtrlGraph> cfg)
{
  // Take ownership of their graph
  cfg->m_entry->change_owner(this, true);
  
  // Attach it before us
  cfg->m_exit->append(m_entry);
  m_entry = cfg->m_entry;
}

void CtrlGraph::append(const std::auto_ptr<CtrlGraph> cfg)
{
  // Take ownership of their graph
  cfg->m_entry->change_owner(this, true);

  // Attach it after us
  m_exit->append(cfg->m_entry);
  m_exit = cfg->m_exit;
}

std::ostream& CtrlGraph::print(std::ostream& out, int indent) const
{
  if (m_entry) {
    clear_marked();
    m_entry->print(out, indent);
    clear_marked();
  }
  
  return out;
}

std::ostream& CtrlGraph::graphviz_dump(std::ostream& out) const
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

CtrlGraph * CtrlGraph::clone() const
{
  // Use our own clone map
  CtrlGraphNode::CloneMap *clone_map = new CtrlGraphNode::CloneMap();
  
  // Clone the graph and find the new tail
  CtrlGraphNode *newHead = m_entry->clone(clone_map);
  CtrlGraphNode *newTail = (*clone_map)[m_exit];

  assert(newHead && newTail);    // Check for a malformed control graph
  
  // Construct a new graph and return it
  delete clone_map;
  return new CtrlGraph(newHead, newTail);
}

}

