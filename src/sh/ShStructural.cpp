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
#include "ShStructural.hpp"
#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <algorithm>
#include "ShCtrlGraph.hpp"
#include "ShDebug.hpp"

// #define SH_STRUCTURAL_DEBUG

#ifdef SH_STRUCTURAL_DEBUG
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

std::string gvname(const SH::ShStructuralNode* p)
{
  std::ostringstream s;
  if (p->type == SH::ShStructuralNode::UNREDUCED) {
    s << '\"';
    s << p;
    s << '\"';
  } else {
    s << "cluster_";
    s << p;
  }
  return s.str();
}

std::string gvfrom(const SH::ShStructuralNode* p)
{
  if (p->type == SH::ShStructuralNode::UNREDUCED) {
    return gvname(p);
  } else {
    return gvname(p) + "_exit_node";
  }
}

std::string gvto(const SH::ShStructuralNode* p)
{
  if (p->type == SH::ShStructuralNode::UNREDUCED) {
    return gvname(p);
  } else {
    return gvname(p) + "_entry_node";
  }
}

// Return true if b is a descendent of a
bool descendent(SH::ShStructuralNode* a,
                SH::ShStructuralNode* b)
{
  SH::ShStructuralNode* n = b;

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
void reach_under_traverse(SH::ShStructuralNode* head,
                          SH::ShStructuralNode* node,
                          Container& r)
{
  if (contains(r, node)) return;
  if (node == head) return;

  r.push_front(node);
  for (SH::ShStructuralNode::PredecessorList::iterator I = node->preds.begin();
       I != node->preds.end(); ++I) {
    reach_under_traverse(head, I->second, r);
  }
}

template<typename T>
void reach_under(SH::ShStructuralNode* a,
                 T& container)
{
  using namespace SH;
  bool backedge = false;
  for (ShStructuralNode::PredecessorList::iterator I = a->preds.begin();
       I != a->preds.end(); ++I) {
    ShStructuralNode* p = I->second;
    // Check that p->a is a backedge

    if (!descendent(a, p)) continue;

    reach_under_traverse(a, p, container);

    backedge = true;
  }

  if (backedge) container.push_front(a);
}

}

namespace SH {

ShStructuralNode::CfgMatch::CfgMatch() {}

ShStructuralNode::CfgMatch::CfgMatch(const ShCtrlGraphNodePtr& from)
  : from(from), to(from->follower), S(from->successors.end())
{
  SH_DEBUG_ASSERT(to);
}

ShStructuralNode::CfgMatch::CfgMatch(const ShCtrlGraphNodePtr& from, 
    ShCtrlGraphNode::SuccessorList::iterator S)
  : from(from), to(S->node), S(S)
{
}

bool ShStructuralNode::CfgMatch::isFollower()
{
  return S == from->successors.end();
}

ShStructuralNode::ShStructuralNode(const ShCtrlGraphNodePtr& node)
  : type(UNREDUCED),
    container(0),
    cfg_node(node),
    secStart(0),
    secEnd(0), 
    parent(0)
{
  if(node->block && !node->block->empty()) {
    if(node->block->begin()->op == SH_OP_STARTSEC) {
      secStart = &*node->block->begin();
    }
    if(node->block->rbegin()->op == SH_OP_ENDSEC) {
      secEnd = &*node->block->rbegin();
    }
  }
}

ShStructuralNode::ShStructuralNode(NodeType type)
  : type(type),
    container(0),
    cfg_node(0),
    secStart(0),
    secEnd(0),
    parent(0)
{
}

bool ShStructuralNode::contains(const ShCtrlGraphNodePtr& node) const
{
  if(cfg_node == node) return true;
  for(StructNodeList::const_iterator S = structnodes.begin();
      S != structnodes.end(); ++S) {
    if((*S)->contains(node)) return true;
  }
  return false;
}


std::ostream& ShStructuralNode::dump(std::ostream& out, int noedges) const
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
  const ShStructuralNode::SuccessorEdge &edge;
  SuccEdgePred(const ShStructuralNode::SuccessorEdge &edge): edge(edge) {}
  SuccEdgePred& operator=(SuccEdgePred const&);
  bool operator()(const ShCtrlGraphNodePtr& from, const ShCtrlGraphNodePtr& to, const ShVariable& var) const 
  {
    return (edge.first == var && edge.second->contains(to));
  }
  bool operator()(const ShVariable &var) const 
  {
    return var == edge.first;
  }
};

// returns true if to node->contains(to) == in 
struct SuccNodePred {
  ShStructuralNode* node;
  bool in;
  SuccNodePred(ShStructuralNode* node, bool in): node(node), in(in) {}
  bool operator()(const ShCtrlGraphNodePtr& from, const ShCtrlGraphNodePtr& to, const ShVariable& var) const 
  {
    return node->contains(to) == in; 
  }
  bool operator()(const ShVariable &var) const 
  {
    return true; 
  }
};


template<typename P>
void getStructuralSuccs(ShStructuralNode::CfgMatchList &result, ShStructuralNode* node, const P &predicate) 
{
  ShCtrlGraphNodePtr cfg = node->cfg_node;
  if(cfg) {
    if(cfg->follower && predicate(cfg, cfg->follower, ShVariable())) {
      result.push_back(ShStructuralNode::CfgMatch(cfg));
    }
    for(ShCtrlGraphNode::SuccessorList::iterator S = cfg->successors.begin();
        S != cfg->successors.end(); ++S) {
      if(predicate(cfg, S->node, S->cond)) {
        result.push_back(ShStructuralNode::CfgMatch(cfg, S));
      }
    }
  }
  ShStructuralNode::StructNodeList::iterator I = node->structnodes.begin(); 
  for(;I != node->structnodes.end(); ++I) {
    getStructuralSuccs(result, *I, predicate);
  }
}

void ShStructuralNode::getSuccs(CfgMatchList &result, const SuccessorEdge &edge) 
{
  getStructuralSuccs(result, this, SuccEdgePred(edge));
}

void ShStructuralNode::getExits(CfgMatchList &result, ShStructuralNode* node) 
{
  if(!node) node = this;
  getStructuralSuccs(result, this, SuccNodePred(node, false));
}

template<typename P>
void getStructuralPreds(ShStructuralNode::CfgMatchList &result, ShStructuralNode* node, const P &predicate) 
{
  ShStructuralNode::PredecessorList::iterator I = node->preds.begin();
  for(; I != node->preds.end(); ++I) {
    if(predicate(I->first)) {
      getStructuralSuccs(result, I->second, predicate);
    }
  }
}

void ShStructuralNode::getPreds(CfgMatchList &result, const PredecessorEdge &edge) 
{
  SuccEdgePred predicate(SuccessorEdge(edge.first, this));
  getStructuralPreds(result, this, predicate);
}

void ShStructuralNode::getEntries(CfgMatchList &result, ShStructuralNode* node) 
{
  if(!node) node = this;
  SuccNodePred predicate(node, true);
  getStructuralPreds(result, this, predicate);
}

ShStructural::ShStructural(const ShCtrlGraphPtr& graph)
  : m_graph(graph),
    m_head(0)
{
  // Build DFS spanning tree, and postorder traversal stack

  std::map<ShCtrlGraphNodePtr, ShStructuralNode*> nodemap;
  graph->entry()->clearMarked();
  m_head = build_tree(graph->entry(), nodemap);
  graph->entry()->clearMarked();

  bool changed;
  do {
    changed = false;
    build_postorder(m_head);

    while (!m_postorder.empty()) {
      SH_STR_DEBUG_PRINT("Considering a node");
      
      ShStructuralNode* node = m_postorder.front();
      m_postorder.pop_front();
      
      typedef ShStructuralNode::StructNodeList NodeSet;
      NodeSet nodeset;
      ShStructuralNode* newnode = 0;
   
      if (!newnode) {
        // Check for blocks

        ShStructuralNode* n = node;
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
                newnode = create_structural_node(ShStructuralNode::SECTION);
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
            newnode = create_structural_node(ShStructuralNode::BLOCK);
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
          ShStructuralNode* next = (node->succs.begin()->second);
          SH_STR_DEBUG_PRINT("|next->preds| = " << next->preds.size());
          for (ShStructuralNode::PredecessorList::iterator P = next->preds.begin();
               P != next->preds.end(); ++P) {
            SH_STR_DEBUG_PRINT("next pred = " << P->second);
          }
        }
      }
      if (!newnode
          && node->succs.size() == 2) {
        // Check for if-else
        ShStructuralNode::SuccessorList::iterator S = node->succs.begin();
        ShStructuralNode* m = S->second;
        ShStructuralNode* n = (++S)->second;
        if (m->succs == n->succs && m->succs.size() == 1) {
          SH_STR_DEBUG_PRINT("FOUND IF-ELSE");
          nodeset.push_back(node);
          nodeset.push_back(m);
          nodeset.push_back(n);
          newnode = create_structural_node(ShStructuralNode::IFELSE);
          newnode->secStart = node->secStart;
        } else if (m->succs.size() == 1 && m->succs.front().second == n) {
          SH_STR_DEBUG_PRINT("FOUND IF");
          nodeset.push_back(node);
          nodeset.push_back(m);
          newnode = create_structural_node(ShStructuralNode::IF);
          newnode->secStart = node->secStart;
        }
      }

      // Find cyclic regions
      if (!newnode) {
        typedef std::list<ShStructuralNode*> ReachUnder;
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
          newnode = create_structural_node(ShStructuralNode::SELFLOOP);
        } else if (ru.size() == 2 && ru.back()->succs.size() == 1) {
          SH_STR_DEBUG_PRINT("FOUND WHILELOOP");
          SH_DEBUG_ASSERT(ru.front() == node);
          SH_DEBUG_ASSERT(ru.back() != node);
          newnode = create_structural_node(ShStructuralNode::WHILELOOP);
        } else if (ru.size() > 0) {
          SH_STR_DEBUG_PRINT("FOUND PROPINT");
          newnode = create_structural_node(ShStructuralNode::PROPINT);
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
          ShStructuralNode* n = *N;
          
          for (ShStructuralNode::PredecessorList::iterator P = n->preds.begin();
               P != n->preds.end(); ++P) {
            if (N == nodeset.begin() && newnode->type == ShStructuralNode::BLOCK
                && P->second == nodeset.back()) {
              ShStructuralNode::PredecessorEdge e = *P;
              e.second = newnode;
              newnode->preds.push_back(e);
              continue;
            }
            if (contains(newnode->preds, *P)) continue;
            if (contains(nodeset, P->second)) continue;

            newnode->preds.push_back(*P);
          }
          for (ShStructuralNode::SuccessorList::iterator S = n->succs.begin();
               S != n->succs.end(); ++S) {
            if (contains(newnode->succs, *S)) continue;
            if (n == nodeset.back() && newnode->type == ShStructuralNode::BLOCK
                && S->second == nodeset.front()) {
              ShStructuralNode::SuccessorEdge e = *S;
              e.second = newnode;
              newnode->succs.push_back(e);
              continue;
            }
            if (contains(nodeset, S->second)) continue;
            newnode->succs.push_back(*S);
          }
          for (ShStructuralNode::ChildList::iterator C = n->children.begin();
               C != n->children.end(); ++C) {
            if (contains(nodeset, *C)) continue;
            newnode->children.push_back(*C);
          }

          // Replace children of parent
          if (n->parent && !contains(nodeset, n->parent)) {
            SH_STR_DEBUG_PRINT("Replace children of parent");
            for (ShStructuralNode::ChildList::iterator I = n->parent->children.begin();
                 I != n->parent->children.end(); ++I) {
              if (*I == n) *I = newnode;
            }
          }
          m_postorder.remove(n);
        }

        // Remove duplicate successors from newnode
        for (ShStructuralNode::SuccessorList::iterator J = newnode->succs.begin();
             J != newnode->succs.end(); ++J) {
          ShStructuralNode::SuccessorList::iterator K = J;
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
        for (ShStructuralNode::ChildList::iterator I = newnode->children.begin();
             I != newnode->children.end(); ++I) {
          (*I)->parent = newnode;
        }

        SH_STR_DEBUG_PRINT("Replace preds of succs");
        // Replace preds of succs
        for (ShStructuralNode::SuccessorList::iterator I = newnode->succs.begin();
             I != newnode->succs.end(); ++I) {
          ShStructuralNode* s = I->second;
          for (ShStructuralNode::PredecessorList::iterator J = s->preds.begin();
               J != s->preds.end(); ++J) {
            if (contains(nodeset, J->second)) J->second = newnode;
          }
          // Make preds unique.
          for (ShStructuralNode::PredecessorList::iterator J = s->preds.begin();
               J != s->preds.end(); ++J) {
            ShStructuralNode::PredecessorList::iterator K = J;
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
        for (ShStructuralNode::PredecessorList::iterator I = newnode->preds.begin();
             I != newnode->preds.end(); ++I) {
          ShStructuralNode* p = I->second;
          for (ShStructuralNode::SuccessorList::iterator J = p->succs.begin();
             J != p->succs.end(); ++J) {
            if (contains(nodeset, J->second)) J->second = newnode;
          }
          // Make succs unique.
          for (ShStructuralNode::SuccessorList::iterator J = p->succs.begin();
               J != p->succs.end(); ++J) {
            ShStructuralNode::SuccessorList::iterator K = J;
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

std::ostream& ShStructural::dump(std::ostream& out) const
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

ShStructuralNode* ShStructural::build_tree(const ShCtrlGraphNodePtr& node, std::map<ShCtrlGraphNodePtr, ShStructuralNode*>& nodemap)
{
  using std::make_pair;
  
  if (!node) return 0;
  if (node->marked()) return 0;
  node->mark();
  
  ShStructuralNode* snode = create_structural_node(node);

  nodemap[node] = snode;
  
  for (ShCtrlGraphNode::SuccessorList::iterator I = node->successors.begin();
       I != node->successors.end(); ++I) {
    if (!I->node) continue; // should never happen
    ShStructuralNode* child = build_tree(I->node, nodemap);
    if (child) {
      child->parent = snode;
      snode->children.push_back(child);
    }

    snode->succs.push_back(make_pair(I->cond, nodemap[I->node]));
    nodemap[I->node]->preds.push_back(make_pair(I->cond, snode));
  }

  if (node->follower) {
    ShStructuralNode* fchild = build_tree(node->follower, nodemap);
    if (fchild) {
      fchild->parent = snode;
      snode->children.push_back(fchild);
    }
    snode->succs.push_back(make_pair(ShVariable(), nodemap[node->follower]));
    nodemap[node->follower]->preds.push_back(make_pair(ShVariable(), snode));
  }

  return snode;
}

void ShStructural::build_postorder(ShStructuralNode* node)
{
  if (node == m_head) {
    m_postorder.clear();
  }

  for (ShStructuralNode::ChildList::iterator I = node->children.begin();
       I != node->children.end(); ++I) {
    build_postorder(*I);
  }
  
  m_postorder.push_back(node);
}

ShStructuralNode* ShStructural::head()
{
  return m_head;
}

}
