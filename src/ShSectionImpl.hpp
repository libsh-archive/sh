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
#ifndef SHSECTIONIMPL_HPP
#define SHSECTIONIMPL_HPP

#include <sstream>
#include "ShSection.hpp"

/** @file ShSectionImpl.hpp
 */

namespace SH {

template<class F>
void ShSectionTree::dfs(F& functor) {
  realDfs(root, functor);
}

template<class F>
void ShSectionTree::realDfs(ShSectionNodePtr node, F& functor)
{
  for(ShSectionNode::iterator I = node->begin(); I != node->end(); ++I) {
    realDfs(*I, functor);
  }
  functor(node);
}

template<class F>
std::ostream& ShSectionTree::dump(std::ostream& out, F& functor)
{
  std::ostringstream cfgout;
  out << "digraph sectiontree {" << std::endl;
    realDump(out, cfgout, root, functor);
    out << cfgout.str() << std::endl;
  out << "}";
  return out;
}

template<class F>
void ShSectionTree::realDump(std::ostream& out, std::ostream& cfgout, 
                             ShSectionNodePtr node, F& functor)
{
  out << "subgraph " << "cluster_" << node.object() << " {" << std::endl; 
    functor(out, node);
    for(ShSectionNode::cfg_iterator C = node->cfgBegin(); C != node->cfgEnd(); ++C) {
      ShCtrlGraphNodePtr c = (*C);
      
      out << "\"cfg_" << c.object() << "\""; 
      functor(out, c);
#if 0
      if(c->block) {
        for(ShBasicBlock::iterator I = c->block->begin(); I != c->block->end(); ++I) {
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
      for (ShCtrlGraphNode::SuccessorList::const_iterator I = c->successors.begin();
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

    for(ShSectionNode::iterator S = node->begin(); S != node->end(); ++S) {
      realDump(out, cfgout, *S, functor);
    }
  out << "}" << std::endl; 
}

}



#endif
