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
#include "Structural.hpp"
#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <algorithm>
#include "CtrlGraph.hpp"
#include "Debug.hpp"

// #define STRUCTURAL_DEBUG

#ifdef STRUCTURAL_DEBUG
#  define SH_STR_DEBUG_PRINT(x) SH_DEBUG_PRINT(x)
#else
#  define SH_STR_DEBUG_PRINT(x)
#endif

namespace {
const char* nodetypename[] ={
  "CFGNODE",
  "BLOCK",
  "SECTION",
  "IF",
  "IFELSE",
  "SELFLOOP",
  "WHILELOOP",
  "PROPINT"
};

std::string gvname(const SH::StructuralNode* p)
{
  std::ostringstream s;
  if (p->type == SH::StructuralNode::UNREDUCED) {
    s << '\"';
    s << p;
    s << '\"';
  } else {
    s << "cluster_";
    s << p;
  }
  return s.str();
}

std::string gvfrom(const SH::StructuralNode* p)
{
  if (p->type == SH::StructuralNode::UNREDUCED) {
    return gvname(p);
  } else {
    return gvname(p) + "_exit_node";
  }
}

std::string gvto(const SH::StructuralNode* p)
{
  if (p->type == SH::StructuralNode::UNREDUCED) {
    return gvname(p);
  } else {
    return gvname(p) + "_entry_node";
  }
}

// Return true if b is a descendent of a
bool descendent(SH::StructuralNode* a,
                SH::StructuralNode* b)
{
  SH::StructuralNode* n = b;

  while (n && n != a) {
    n = n->parent;
  }
  
  return n != 0;
}

template<typename Container, typename T>
bool contains(const Container& c,
              const T& v)
{
  return find(c.begin(), c.end(), v) != c.end();
}

template<typename Container>
void reach_under_traverse(SH::StructuralNode* head,
                          SH::StructuralNode* node,
                          Container& r)
{
  if (contains(r, node)) return;
  if (node == head) return;

  r.push_front(node);
  for (SH::StructuralNode::PredecessorList::iterator I = node->preds.begin();
       I != node->preds.end(); ++I) {
    reach_under_traverse(head, I->second, r);
  }
}

template<typename T>
void reach_under(SH::StructuralNode* a,
                 T& container)
{
  using namespace SH;
  bool backedge = false;
  for (StructuralNode::PredecessorList::iterator I = a->preds.begin();
       I != a->preds.end(); ++I) {
    StructuralNode* p = I->second;
    // Check that p->a is a backedge

    if (!descendent(a, p)) continue;

    reach_under_traverse(a, p, container);

    backedge = true;
  }

  if (backedge) container.push_front(a);
}

}

namespace SH {

StructuralNode::CfgMatch::CfgMatch() {}

StructuralNode::CfgMatch::CfgMatch(const CtrlGraphNodePtr& from)
  : from(from), to(from->follower), S(from->successors.end())
{
  SH_DEBUG_ASSERT(to);
}

StructuralNode::CfgMatch::CfgMatch(const CtrlGraphNodePtr& from, 
    CtrlGraphNode::SuccessorList::iterator S)
  : from(from), to(S->node), S(S)
{
}

bool StructuralNode::CfgMatch::isFollower()
{
  return S == from->successors.end();
}

StructuralNode::StructuralNode(const CtrlGraphNodePtr& node)
  : type(UNREDUCED),
    container(0),
    cfg_node(node),
    secStart(0),
    secEnd(0), 
    parent(0)
{
  if(node->block && !node->block->empty()) {
    if(node->block->begin()->op == OP_STARTSEC) {
      secStart = &*node->block->begin();
    }
    if(node->block->rbegin()->op == OP_ENDSEC) {
      secEnd = &*node->block->rbegin();
    }
  }
}

StructuralNode::StructuralNode(NodeType type)
  : type(type),
    container(0),
    cfg_node(0),
    secStart(0),
    secEnd(0),
    parent(0)
{
}

bool StructuralNode::contains(const CtrlGraphNodePtr& node) const
{
  if(cfg_node == node) return true;
  for(StructNodeList::const_iterator S = structnodes.begin();
      S != structnodes.end(); ++S) {
    if((*S)->contains(node)) return true;
  }
  return false;
}


std::ostream& StructuralNode::dump(std::ostream& out, int noedges) const
{
  if (noedges != 0) {
    if (type == UNREDUCED) {
      out << gvname(this) << " ";
      if (cfg_node->block) {
        out << "[label=\"";
        cfg_node->block->graphvizDump(out);
        out << "\", shape=box]";
      } else {
        out << " [label=\"\", shape=circle, height=0.25]";
      }
      out << ";" << std::endl;
    } else {
      out << "subgraph " << gvname(this) << " {" << std::endl;
      out << "subgraph " << gvname(this) << "_entry { ";
      out << "  " << gvname(this) << "_entry_node [label=\"\",shape=box,fillcolor=green,style=filled];";
      out << "  label=\"\";" << std::endl;
      out << " }" << std::endl;
      for (StructNodeList::const_iterator I = structnodes.begin(); I != structnodes.end(); ++I) {
        (*I)->dump(out, 1);
      }
      out << "  label=\"" << nodetypename[type];
      // out << " [" << gvname(this) << "]";
      out << "\";" << std::endl;
      out << "subgraph " << gvname(this) << "_exit { ";
      out << "  " << gvname(this) << "_exit_node [label=\"\",shape=box,fillcolor=red,style=filled];";
      out << "  label=\"\";" << std::endl;
      out << " }" << std::endl;
      out << "}" << std::endl;
    }
  }

  if (noedges <= 0) {
    for (SuccessorList::const_iterator I = succs.begin(); I != succs.end(); ++I) {
      //if (find(children.begin(), children.end(), I->second) != children.end()) continue;
      out << gvfrom(this) << " -> " << gvto(I->second);
      if (I->first.node()) {
        out << " [style=dashed,label=\"" << I->first.name() << "\"]";
      }
      out << ";" << std::endl;
    }
    for (StructNodeList::const_iterator I = structnodes.begin(); I != structnodes.end(); ++I) {
      (*I)->dump(out, 0);
    }

  }

#if 0
  out << gvname(this) << " ";
  out << " [shape=circle]";
  out << ";" << std::endl;
  
  for (ChildList::const_iterator I = children.begin(); I != children.end(); ++I) {
    (*I)->dump(out);
    out << gvname(this) << " -> " << gvname(I->object()) << ";" << std::endl;
  }
#endif
  return out;
}

// Predicates have two operators - one that returns true if a ctrl graph edge
// (from, to, var) where var may be null for a follower matches the predicate
//
// And an operator that returns true if a given branch conditoin var matches the predicate.
struct SuccEdgePred {
  const StructuralNode::SuccessorEdge &edge;
  SuccEdgePred(const StructuralNode::SuccessorEdge &edge): edge(edge) {}
  SuccEdgePred& operator=(SuccEdgePred const&);
  bool operator()(const CtrlGraphNodePtr& from, const CtrlGraphNodePtr& to, const Variable& var) const 
  {
    return (edge.first == var && edge.second->contains(to));
  }
  bool operator()(const Variable &var) const 
  {
    return var == edge.first;
  }
};

// returns true if to node->contains(to) == in 
struct SuccNodePred {
  StructuralNode* node;
  bool in;
  SuccNodePred(StructuralNode* node, bool in): node(node), in(in) {}
  bool operator()(const CtrlGraphNodePtr& from, const CtrlGraphNodePtr& to, const Variable& var) const 
  {
    return node->contains(to) == in; 
  }
  bool operator()(const Variable &var) const 
  {
    return true; 
  }
};


template<typename P>
void getStructuralSuccs(StructuralNode::CfgMatchList &result, StructuralNode* node, const P &predicate) 
{
  CtrlGraphNodePtr cfg = node->cfg_node;
  if(cfg) {
    if(cfg->follower && predicate(cfg, cfg->follower, Variable())) {
      result.push_back(StructuralNode::CfgMatch(cfg));
    }
    for(CtrlGraphNode::SuccessorList::iterator S = cfg->successors.begin();
        S != cfg->successors.end(); ++S) {
      if(predicate(cfg, S->node, S->cond)) {
        result.push_back(StructuralNode::CfgMatch(cfg, S));
      }
    }
  }
  StructuralNode::StructNodeList::iterator I = node->structnodes.begin(); 
  for(;I != node->structnodes.end(); ++I) {
    getStructuralSuccs(result, *I, predicate);
  }
}

void StructuralNode::getSuccs(CfgMatchList &result, const SuccessorEdge &edge) 
{
  getStructuralSuccs(result, this, SuccEdgePred(edge));
}

void StructuralNode::getExits(CfgMatchList &result, StructuralNode* node) 
{
  if(!node) node = this;
  getStructuralSuccs(result, this, SuccNodePred(node, false));
}

template<typename P>
void getStructuralPreds(StructuralNode::CfgMatchList &result, StructuralNode* node, const P &predicate) 
{
  StructuralNode::PredecessorList::iterator I = node->preds.begin();
  for(; I != node->preds.end(); ++I) {
    if(predicate(I->first)) {
      getStructuralSuccs(result, I->second, predicate);
    }
  }
}

void StructuralNode::getPreds(CfgMatchList &result, const PredecessorEdge &edge) 
{
  SuccEdgePred predicate(SuccessorEdge(edge.first, this));
  getStructuralPreds(result, this, predicate);
}

void StructuralNode::getEntries(CfgMatchList &result, StructuralNode* node) 
{
  if(!node) node = this;
  SuccNodePred predicate(node, true);
  getStructuralPreds(result, this, predicate);
}

Structural::Structural(const CtrlGraphPtr& graph)
  : m_graph(graph),
    m_head(0)
{
  // Build DFS spanning tree, and postorder traversal stack

  std::map<CtrlGraphNodePtr, StructuralNode*> nodemap;
  graph->entry()->clearMarked();
  m_head = build_tree(graph->entry(), nodemap);
  graph->entry()->clearMarked();

  bool changed;
  do {
    changed = false;
    build_postorder(m_head);

    while (!m_postorder.empty()) {
      SH_STR_DEBUG_PRINT("Considering a node");
      
      StructuralNode* node = m_postorder.front();
      m_postorder.pop_front();
      
      typedef StructuralNode::StructNodeList NodeSet;
      NodeSet nodeset;
      StructuralNode* newnode = 0;
   
      if (!newnode) {
        // Check for blocks

        StructuralNode* n = node;
        bool p = true;
        bool s = n->succs.size() == 1;
        
        while (p && s) {
          nodeset.push_back(n);
          n = n->succs.begin()->second;
          p = n->preds.size() == 1;
          s = n->succs.size() == 1;
        }

        if (p && n != node) nodeset.push_back(n);
        n = node; p = n->preds.size() == 1; s = true;
        while (p && s) {
          if (n != node) nodeset.push_front(n);
          n = n->preds.begin()->second;
          p = n->preds.size() == 1;
          s = n->succs.size() == 1;
        }
        if (s && n != node) nodeset.push_front(n);

        // search for a section
        // find the last START (if there is one)
        // the next END after that must represent a SECTION
        NodeSet::iterator S, E;
        for(S = nodeset.end(); !newnode && S != nodeset.begin();) {
          --S;
          if((*S)->secStart) {
            for(E = S;; ++E) {
              if(E == nodeset.end()) {
                SH_STR_DEBUG_PRINT("Found STARTSEC with no ENDSEC!");
                SH_DEBUG_ASSERT(0);
                break;
              }

              // great...have a section, erase other nodes, and make a newnode
              if((*E)->secEnd) { 
                newnode = create_structural_node(StructuralNode::SECTION);
                // @todo range - do we need to check if these are empty
                // ranges?
                ++E;
                nodeset.erase(E, nodeset.end());
                nodeset.erase(nodeset.begin(), S);
                SH_STR_DEBUG_PRINT("FOUND SECTION of size " << nodeset.size());
                for (NodeSet::iterator I = nodeset.begin(); I != nodeset.end(); ++I) {
                  SH_STR_DEBUG_PRINT("  node " << I->object());
                }
                break;
              }
            }
            break;
          }
        }
        
        // otherwise may be block
        if(!newnode) {
          if (nodeset.size() >= 2) {
            SH_STR_DEBUG_PRINT("FOUND BLOCK of size " << nodeset.size());
            for (NodeSet::iterator I = nodeset.begin(); I != nodeset.end(); ++I) {
              SH_STR_DEBUG_PRINT("  node " << I->object());
            }
            //node = n;
            newnode = create_structural_node(StructuralNode::BLOCK);
            newnode->secStart = nodeset.front()->secStart;
            newnode->secEnd = nodeset.back()->secEnd;
            SH_DEBUG_ASSERT(!(newnode->secStart && newnode->secEnd));
          } else {
            nodeset.clear();
          }
        }
      } else {
        SH_STR_DEBUG_PRINT("Not a block. |succs| = " << node->succs.size());
        if (!node->succs.empty()) {
          StructuralNode* next = (node->succs.begin()->second);
          SH_STR_DEBUG_PRINT("|next->preds| = " << next->preds.size());
          for (StructuralNode::PredecessorList::iterator P = next->preds.begin();
               P != next->preds.end(); ++P) {
            SH_STR_DEBUG_PRINT("next pred = " << P->second);
          }
        }
      }
      if (!newnode
          && node->succs.size() == 2) {
        // Check for if-else
        StructuralNode::SuccessorList::iterator S = node->succs.begin();
        StructuralNode* m = S->second;
        StructuralNode* n = (++S)->second;
        if (m->succs == n->succs && m->succs.size() == 1) {
          SH_STR_DEBUG_PRINT("FOUND IF-ELSE");
          nodeset.push_back(node);
          nodeset.push_back(m);
          nodeset.push_back(n);
          newnode = create_structural_node(StructuralNode::IFELSE);
          newnode->secStart = node->secStart;
        } else if (m->succs.size() == 1 && m->succs.front().second == n) {
          SH_STR_DEBUG_PRINT("FOUND IF");
          nodeset.push_back(node);
          nodeset.push_back(m);
          newnode = create_structural_node(StructuralNode::IF);
          newnode->secStart = node->secStart;
        }
      }

      // Find cyclic regions
      if (!newnode) {
        typedef std::list<StructuralNode*> ReachUnder;
        //        typedef NodeSet ReachUnder;
        ReachUnder ru;
        reach_under(node, ru);
        SH_STR_DEBUG_PRINT("Reach under set for " << node);
        for (ReachUnder::iterator I = ru.begin(); I != ru.end(); ++I) {
          SH_STR_DEBUG_PRINT("  " << *I);
        }

        if (ru.size() == 1) {
          SH_STR_DEBUG_PRINT("FOUND SELFLOOP");
          SH_DEBUG_ASSERT(ru.front() == node);
          newnode = create_structural_node(StructuralNode::SELFLOOP);
        } else if (ru.size() == 2 && ru.back()->succs.size() == 1) {
          SH_STR_DEBUG_PRINT("FOUND WHILELOOP");
          SH_DEBUG_ASSERT(ru.front() == node);
          SH_DEBUG_ASSERT(ru.back() != node);
          newnode = create_structural_node(StructuralNode::WHILELOOP);
        } else if (ru.size() > 0) {
          SH_STR_DEBUG_PRINT("FOUND PROPINT");
          newnode = create_structural_node(StructuralNode::PROPINT);
        }

        if (newnode) {
          for (ReachUnder::iterator I = ru.begin(); I != ru.end(); ++I) {
            nodeset.push_back(*I);
          }
        }
      }
      
      // Fix up the graph.
      if (newnode) {
        changed = true;
        newnode->structnodes = nodeset;

        for (NodeSet::iterator N = nodeset.begin(); N != nodeset.end(); ++N) {
          (*N)->container = newnode;
        }

        SH_STR_DEBUG_PRINT("Parent, preds and succs");
        
        newnode->parent = nodeset.front()->parent;

        
        for (NodeSet::iterator N = nodeset.begin(); N != nodeset.end(); ++N) {
          StructuralNode* n = *N;
          
          for (StructuralNode::PredecessorList::iterator P = n->preds.begin();
               P != n->preds.end(); ++P) {
            if (N == nodeset.begin() && newnode->type == StructuralNode::BLOCK
                && P->second == nodeset.back()) {
              StructuralNode::PredecessorEdge e = *P;
              e.second = newnode;
              newnode->preds.push_back(e);
              continue;
            }
            if (contains(newnode->preds, *P)) continue;
            if (contains(nodeset, P->second)) continue;

            newnode->preds.push_back(*P);
          }
          for (StructuralNode::SuccessorList::iterator S = n->succs.begin();
               S != n->succs.end(); ++S) {
            if (contains(newnode->succs, *S)) continue;
            if (n == nodeset.back() && newnode->type == StructuralNode::BLOCK
                && S->second == nodeset.front()) {
              StructuralNode::SuccessorEdge e = *S;
              e.second = newnode;
              newnode->succs.push_back(e);
              continue;
            }
            if (contains(nodeset, S->second)) continue;
            newnode->succs.push_back(*S);
          }
          for (StructuralNode::ChildList::iterator C = n->children.begin();
               C != n->children.end(); ++C) {
            if (contains(nodeset, *C)) continue;
            newnode->children.push_back(*C);
          }

          // Replace children of parent
          if (n->parent && !contains(nodeset, n->parent)) {
            SH_STR_DEBUG_PRINT("Replace children of parent");
            for (StructuralNode::ChildList::iterator I = n->parent->children.begin();
                 I != n->parent->children.end(); ++I) {
              if (*I == n) *I = newnode;
            }
          }
          m_postorder.remove(n);
        }

        // Remove duplicate successors from newnode
        for (StructuralNode::SuccessorList::iterator J = newnode->succs.begin();
             J != newnode->succs.end(); ++J) {
          StructuralNode::SuccessorList::iterator K = J;
          for (K++; K != newnode->succs.end(); ++K) {
            if (K->second == J->second) {
              // Favour conditional edges for deletion in order to
              // keep the non-conditional edges if possible
              if (J->first.node()) {
                J = newnode->succs.erase(J);
                if (J != newnode->succs.begin()) --J;
              } else {
                K = newnode->succs.erase(K);
                J = newnode->succs.begin(); // The previous erase has invalidated J
              }
              break;
            }
          }
        }

        SH_STR_DEBUG_PRINT("Replace parent of children");
        // Replace parent of children
        for (StructuralNode::ChildList::iterator I = newnode->children.begin();
             I != newnode->children.end(); ++I) {
          (*I)->parent = newnode;
        }

        SH_STR_DEBUG_PRINT("Replace preds of succs");
        // Replace preds of succs
        for (StructuralNode::SuccessorList::iterator I = newnode->succs.begin();
             I != newnode->succs.end(); ++I) {
          StructuralNode* s = I->second;
          for (StructuralNode::PredecessorList::iterator J = s->preds.begin();
               J != s->preds.end(); ++J) {
            if (contains(nodeset, J->second)) J->second = newnode;
          }
          // Make preds unique.
          for (StructuralNode::PredecessorList::iterator J = s->preds.begin();
               J != s->preds.end(); ++J) {
            StructuralNode::PredecessorList::iterator K = J;
            for (++K; K != s->preds.end(); ++K) {
              if (K->second == J->second) {
                J = s->preds.erase(J);
                if (J != s->preds.begin()) --J;
                break;
              }
            }
          }
        }

        SH_STR_DEBUG_PRINT("Replace succs of preds");
        // Replace succs of preds
        for (StructuralNode::PredecessorList::iterator I = newnode->preds.begin();
             I != newnode->preds.end(); ++I) {
          StructuralNode* p = I->second;
          for (StructuralNode::SuccessorList::iterator J = p->succs.begin();
             J != p->succs.end(); ++J) {
            if (contains(nodeset, J->second)) J->second = newnode;
          }
          // Make succs unique.
          for (StructuralNode::SuccessorList::iterator J = p->succs.begin();
               J != p->succs.end(); ++J) {
            StructuralNode::SuccessorList::iterator K = J;
            for (K++; K != p->succs.end(); ++K) {
              if (K->second == J->second) {
                J = p->succs.erase(J);
                if (J != p->succs.begin()) --J;
                break;
              }
            }
          }
        }
        
        SH_STR_DEBUG_PRINT("Add to postorder");
        m_postorder.push_back(newnode);

        if (nodeset.front() == m_head) {
          SH_STR_DEBUG_PRINT("Replace head");
          m_head = newnode;
        }
      }
    }
  } while (changed);
}

std::ostream& Structural::dump(std::ostream& out) const
{
  out << "digraph structural {" << std::endl;
  m_head->dump(out);

  int counter = 1;
  for (PostorderList::const_iterator I = m_postorder.begin(); I != m_postorder.end(); ++I) {
    out << "\"" << *I << "\" [label=\"" << counter << "\"];" << std::endl;
    counter++;
  }
  out << "}" << std::endl;
  return out;
}

StructuralNode* Structural::build_tree(const CtrlGraphNodePtr& node, std::map<CtrlGraphNodePtr, StructuralNode*>& nodemap)
{
  using std::make_pair;
  
  if (!node) return 0;
  if (node->marked()) return 0;
  node->mark();
  
  StructuralNode* snode = create_structural_node(node);

  nodemap[node] = snode;
  
  for (CtrlGraphNode::SuccessorList::iterator I = node->successors.begin();
       I != node->successors.end(); ++I) {
    if (!I->node) continue; // should never happen
    StructuralNode* child = build_tree(I->node, nodemap);
    if (child) {
      child->parent = snode;
      snode->children.push_back(child);
    }

    snode->succs.push_back(make_pair(I->cond, nodemap[I->node]));
    nodemap[I->node]->preds.push_back(make_pair(I->cond, snode));
  }

  if (node->follower) {
    StructuralNode* fchild = build_tree(node->follower, nodemap);
    if (fchild) {
      fchild->parent = snode;
      snode->children.push_back(fchild);
    }
    snode->succs.push_back(make_pair(Variable(), nodemap[node->follower]));
    nodemap[node->follower]->preds.push_back(make_pair(Variable(), snode));
  }

  return snode;
}

void Structural::build_postorder(StructuralNode* node)
{
  if (node == m_head) {
    m_postorder.clear();
  }

  for (StructuralNode::ChildList::iterator I = node->children.begin();
       I != node->children.end(); ++I) {
    build_postorder(*I);
  }
  
  m_postorder.push_back(node);
}

StructuralNode* Structural::head()
{
  return m_head;
}

}
