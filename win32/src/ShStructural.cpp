#include "ShStructural.hpp"
#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <algorithm>
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
  "IF",
  "IFELSE",
  "SELFLOOP",
  "WHILELOOP"
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
bool descendent(const SH::ShStructuralNodePtr& a,
                const SH::ShStructuralNodePtr& b)
{
  SH::ShStructuralNode* n = b.object();

  while (n && n != a.object()) {
    n = n->parent;
  }
  
  return n;
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
void reach_under(const SH::ShStructuralNodePtr& a,
                 T& container)
{
  using namespace SH;
  bool backedge = false;
  for (ShStructuralNode::PredecessorList::iterator I = a->preds.begin();
       I != a->preds.end(); ++I) {
    ShStructuralNode* p = I->second;
    // Check that p->a is a backedge

    if (!descendent(a, p)) continue;

    reach_under_traverse(a.object(), p, container);

    backedge = true;
  }

  if (backedge) container.push_front(a.object());
}

}

namespace SH {

ShStructuralNode::ShStructuralNode(const ShCtrlGraphNodePtr& node)
  : type(UNREDUCED),
    container(0),
    cfg_node(node),
    parent(0)
{
}

ShStructuralNode::ShStructuralNode(NodeType type)
  : type(type),
    container(0),
    cfg_node(0),
    parent(0)
{
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
      out << gvfrom(this) << " -> " << gvto(I->second.object());
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

ShStructural::ShStructural(const ShCtrlGraphPtr& graph)
  : m_graph(graph),
    m_head(0)
{
  // Build DFS spanning tree, and postorder traversal stack

  graph->entry()->clearMarked();
  m_head = build_tree(graph->entry());
  graph->entry()->clearMarked();

  bool changed;
  do {
    changed = false;
    build_postorder(m_head);

    while (!m_postorder.empty()) {
      SH_STR_DEBUG_PRINT("Considering a node");
      
      ShStructuralNode* node = m_postorder.front();
      m_postorder.pop_front();
      
      typedef std::list<ShStructuralNodePtr> NodeSet;
      NodeSet nodeset;
      ShStructuralNodePtr newnode = 0;
      
      if (!newnode) {
        // Check for blocks

        ShStructuralNodePtr n = node;
        
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

        if (nodeset.size() >= 2) {
          SH_STR_DEBUG_PRINT("FOUND BLOCK of size " << nodeset.size());
          for (NodeSet::iterator I = nodeset.begin(); I != nodeset.end(); ++I) {
            SH_STR_DEBUG_PRINT("  node " << I->object());
          }
          //node = n.object();
          newnode = new ShStructuralNode(ShStructuralNode::BLOCK);
        } else {
          nodeset.clear();
        }
      } else {
        SH_STR_DEBUG_PRINT("Not a block. |succs| = " << node->succs.size());
        if (!node->succs.empty()) {
          ShStructuralNodePtr next = (node->succs.begin()->second);
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
        ShStructuralNodePtr m = S->second;
        ShStructuralNodePtr n = (++S)->second;
        if (m->succs == n->succs && m->succs.size() == 1) {
          SH_STR_DEBUG_PRINT("FOUND IF-ELSE");
          nodeset.push_back(node);
          nodeset.push_back(m);
          nodeset.push_back(n);
          newnode = new ShStructuralNode(ShStructuralNode::IFELSE);
        }
      }

      // Find cyclic regions
      if (!newnode) {
        /*
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
          newnode = new ShStructuralNode(ShStructuralNode::SELFLOOP);
        } else
        if (ru.size() == 2 && ru.back()->succs.size() == 1) {
          SH_STR_DEBUG_PRINT("FOUND WHILELOOP");
          SH_DEBUG_ASSERT(ru.front() == node);
          SH_DEBUG_ASSERT(ru.back() != node);
          newnode = new ShStructuralNode(ShStructuralNode::WHILELOOP);
        } 
        if (newnode) {
          for (ReachUnder::iterator I = ru.begin(); I != ru.end(); ++I) {
            nodeset.push_back(*I);
          }
        }
        */

        for (ShStructuralNode::SuccessorList::iterator S = node->succs.begin();
             S != node->succs.end(); ++S) {
          if (S->second->preds.size() == 1
              && S->second->succs.size() == 1
              && S->second->succs.begin()->second == node) {
            // found while loop
            nodeset.push_back(node);
            nodeset.push_back(S->second);
            newnode = new ShStructuralNode(ShStructuralNode::WHILELOOP);
            break;
          }
          // self loop?
          if (S->second == node) {
            nodeset.push_back(node);
            newnode = new ShStructuralNode(ShStructuralNode::SELFLOOP);
            break;
          }
        }
      }
      
      // Fix up the graph.
      if (newnode) {
        changed = true;
        newnode->structnodes = nodeset;

        for (NodeSet::iterator N = nodeset.begin(); N != nodeset.end(); ++N) {
          (*N)->container = newnode.object();
        }

        SH_STR_DEBUG_PRINT("Parent, preds and succs");
        
        newnode->parent = nodeset.front()->parent;

        
        for (NodeSet::iterator N = nodeset.begin(); N != nodeset.end(); ++N) {
          ShStructuralNodePtr n = *N;
          
          for (ShStructuralNode::PredecessorList::iterator P = n->preds.begin();
               P != n->preds.end(); ++P) {
            if (N == nodeset.begin() && newnode->type == ShStructuralNode::BLOCK
                && P->second == nodeset.back().object()) {
              ShStructuralNode::PredecessorEdge e = *P;
              e.second = newnode.object();
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
          m_postorder.remove(n.object());
        }

        SH_STR_DEBUG_PRINT("Replace parent of children");
        // Replace parent of children
        for (ShStructuralNode::ChildList::iterator I = newnode->children.begin();
             I != newnode->children.end(); ++I) {
          (*I)->parent = newnode.object();
        }

        SH_STR_DEBUG_PRINT("Replace preds of succs");
        // Replace preds of succs
        for (ShStructuralNode::SuccessorList::iterator I = newnode->succs.begin();
             I != newnode->succs.end(); ++I) {
          ShStructuralNodePtr s = I->second;
          for (ShStructuralNode::PredecessorList::iterator J = s->preds.begin();
               J != s->preds.end(); ++J) {
            if (contains(nodeset, J->second)) J->second = newnode.object();
          }
          // Make preds unique.
          for (ShStructuralNode::PredecessorList::iterator J = s->preds.begin();
               J != s->preds.end(); ++J) {
            ShStructuralNode::PredecessorList::iterator K = J;
            ++K;
            if (std::find(K, s->preds.end(), *J) != s->preds.end()) {
              J = s->preds.erase(J);
              J--;
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
            ++K;
            if (std::find(K, p->succs.end(), *J) != p->succs.end()) {
              J = p->succs.erase(J);
              J--;
            }
          }
        }
        
        SH_STR_DEBUG_PRINT("Add to postorder");
        m_postorder.push_back(newnode.object());

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

ShStructuralNodePtr ShStructural::build_tree(const ShCtrlGraphNodePtr& node)
{
  static std::map<ShCtrlGraphNodePtr, ShStructuralNodePtr> nodemap;
  using std::make_pair;
  
  if (!node) return 0;
  if (node->marked()) return 0;
  node->mark();
  
  ShStructuralNodePtr snode = new ShStructuralNode(node);

  nodemap[node] = snode;
  
  for (ShCtrlGraphNode::SuccessorList::iterator I = node->successors.begin();
       I != node->successors.end(); ++I) {
    if (!I->node) continue; // should never happen
    ShStructuralNodePtr child = build_tree(I->node);
    if (child) {
      child->parent = snode.object();
      snode->children.push_back(child);
    }

    snode->succs.push_back(make_pair(I->cond, nodemap[I->node]));
    nodemap[I->node]->preds.push_back(make_pair(I->cond, snode.object()));
  }

  if (node->follower) {
    ShStructuralNodePtr fchild = build_tree(node->follower);
    if (fchild) {
      fchild->parent = snode.object();
      snode->children.push_back(fchild);
    }
    snode->succs.push_back(make_pair(ShVariable(), nodemap[node->follower]));
    nodemap[node->follower]->preds.push_back(make_pair(ShVariable(), snode.object()));
  }

  return snode;
}

void ShStructural::build_postorder(const ShStructuralNodePtr& node)
{
  if (node == m_head) {
    m_postorder.clear();
  }

  for (ShStructuralNode::ChildList::iterator I = node->children.begin();
       I != node->children.end(); ++I) {
    build_postorder(*I);
  }
  
  m_postorder.push_back(node.object());
}

const ShStructuralNodePtr& ShStructural::head()
{
  return m_head;
}

}
