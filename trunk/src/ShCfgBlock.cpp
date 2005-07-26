// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#include "ShProgram.hpp"
#include "ShUtility.hpp"
#include "ShCfgBlock.hpp"

namespace SH {

ShCfgBlock::ShCfgBlock(const ShProgram &program, bool copy)
{
  init(program.node()->ctrlGraph, copy);
}

ShCfgBlock::ShCfgBlock(ShCtrlGraphPtr cfg, bool copy)
{
  init(cfg, copy);
}

ShCfgBlock::ShCfgBlock(ShCtrlGraphNodePtr node, bool copy)
{
  ShCtrlGraphPtr cfg = new ShCtrlGraph(node, node); 
  init(cfg, copy);
}

void ShCfgBlock::init(ShCtrlGraphPtr cfg, bool copy)
{
  if(copy) {
    cfg->copy(m_entry, m_exit);
  } else {
    m_entry = cfg->entry();
    m_exit = cfg->exit();
  }
}

ShCfgBlock::~ShCfgBlock()
{
}

ShCtrlGraphNodePtr ShCfgBlock::entry() const 
{
  return m_entry;
}

ShCtrlGraphNodePtr ShCfgBlock::exit() const 
{
  return m_exit;
}

void ShCfgBlock::print(std::ostream& out, int indent) const
{
  using std::endl;
  
  shPrintIndent(out, indent);
  out << "CfgBlock {" << endl;
    m_entry->print(out, indent + 2);
  shPrintIndent(out, indent);
  out << "}" << endl;
}

void ShCfgBlock::graphvizDump(std::ostream& out) const
{
  out << "subgraph cfgblock {";
    m_entry->graphvizDump(out);
  out << "}";
}

}

