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

