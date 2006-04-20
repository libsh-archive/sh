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
#ifndef SHSECTIONIMPL_HPP
#define SHSECTIONIMPL_HPP

#include <sstream>
#include "Section.hpp"

/** @file SectionImpl.hpp
 */

namespace SH {

template<class F>
void SectionTree::dfs(F& functor) {
  realDfs(root, functor);
}

template<class F>
void SectionTree::realDfs(const SectionNodePtr& node, F& functor)
{
  for(SectionNode::iterator I = node->begin(); I != node->end(); ++I) {
    realDfs(*I, functor);
  }
  functor(node);
}

template<class F>
std::ostream& SectionTree::dump(std::ostream& out, F& functor)
{
  std::ostringstream cfgout;
  out << "digraph sectiontree {" << std::endl;
    realDump(out, cfgout, root, functor);
    out << cfgout.str() << std::endl;
  out << "}";
  return out;
}

template<class F>
void SectionTree::realDump(std::ostream& out, std::ostream& cfgout, 
                             const SectionNodePtr& node, F& functor)
{
  out << "subgraph " << "cluster_" << node.object() << " {" << std::endl; 
    functor(out, node);
    for(SectionNode::cfg_iterator C = node->cfgBegin(); C != node->cfgEnd(); ++C) {
      CtrlGraphNodePtr c = (*C);
      
      out << "\"cfg_" << c.object() << "\""; 
      functor(out, c);
#if 0
      if(c->block) {
        for(BasicBlock::iterator I = c->block->begin(); I != c->block->end(); ++I) {
          functor(out, *I); 
          out << "\n";
        }
        out << ">, shape=box]";
      } else {
        out << ">, shape=circle, height=0.25]";
      }
#endif
      out << ";" << std::endl;

      // @todo this is copied from CtrlGraph.cpp...
      for (CtrlGraphNode::SuccessorList::const_iterator I = c->successors.begin();
           I != c->successors.end(); ++I) {
        std::ostream& curout = contains(node, I->node) ? out : cfgout; 
        curout << "\"cfg_" << c.object() << "\" ";
        curout << "-> ";
        curout << "\"cfg_" << I->node.object() << "\" ";

        curout << "[style=dashed, label=\"" << I->cond.name() << "\"]";
        curout << ";" << std::endl;
      }

      if (c->follower) {
        std::ostream& curout = contains(node, c->follower) ? out : cfgout; 
        curout << "\"cfg_" << c.object() << "\" ";
        curout << "-> ";
        curout << "\"cfg_" << c->follower.object() << "\";" << std::endl;
      }
    }

    for(SectionNode::iterator S = node->begin(); S != node->end(); ++S) {
      realDump(out, cfgout, *S, functor);
    }
  out << "}" << std::endl; 
}

}



#endif
