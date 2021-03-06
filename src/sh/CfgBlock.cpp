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
#include <iostream>
#include "Program.hpp"
#include "Utility.hpp"
#include "CfgBlock.hpp"

namespace SH {

CfgBlock::CfgBlock(const Program &program, bool copy)
{
  init(program.node()->ctrlGraph, copy);
}

CfgBlock::CfgBlock(const CtrlGraphPtr& cfg, bool copy)
{
  init(cfg, copy);
}

CfgBlock::CfgBlock(CtrlGraphNode* node, bool copy)
{
  CtrlGraphPtr cfg = new CtrlGraph(node, node); 
  init(cfg, copy);
}

void CfgBlock::init(const CtrlGraphPtr& cfg, bool copy)
{
  if(copy) {
    m_cfg = cfg->clone();
  } else {
    m_cfg = cfg;
  }
}

CfgBlock::~CfgBlock()
{
}

void CfgBlock::print(std::ostream& out, int indent) const
{
  using std::endl;
  
  printIndent(out, indent);
  out << "CfgBlock {" << endl;
    graph()->entry()->print(out, indent + 2);
  printIndent(out, indent);
  out << "}" << endl;
}

void CfgBlock::graphviz_dump(std::ostream& out) const
{
  out << "subgraph cfgblock {";
    graph()->entry()->graphviz_dump(out);
  out << "}";
}

}

