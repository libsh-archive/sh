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
#ifndef SHCFGBLOCK_HPP
#define SHCFGBLOCK_HPP

#include <list>
#include "Block.hpp"
#include "CtrlGraph.hpp"

namespace SH {

/** A control-flow graph block 
 * A cfg block is a block composed a previously defined control graph, 
 * Normally the block stores a copy, but for internal use you may set copy =
 * false to use the original graph.  
 */
class 
SH_DLLEXPORT
CfgBlock : public Block {
public:
  CfgBlock(const Program &program, bool copy=true);
  CfgBlock(const CtrlGraphPtr& cfg, bool copy=true);
  CfgBlock(CtrlGraphNode* node, bool copy=true); 

  ~CfgBlock();

  CtrlGraphPtr graph() const { return m_cfg; }

  void print(std::ostream& out, int indent) const;
  void graphviz_dump(std::ostream& out) const;

  
private:
  void init(const CtrlGraphPtr& cfg, bool copy);

  CtrlGraphPtr m_cfg;
};

typedef Pointer<CfgBlock> CfgBlockPtr;
typedef Pointer<const CfgBlock> CfgBlockCPtr;

}

#endif
