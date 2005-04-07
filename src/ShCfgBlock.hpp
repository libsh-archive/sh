// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#ifndef SHCFGBLOCK_HPP
#define SHCFGBLOCK_HPP

#include <list>
#include "ShBlock.hpp"
#include "ShCtrlGraph.hpp"

namespace SH {

/** A control-flow graph block 
 * A cfg block is a block composed a previously defined control graph, 
 * Normally the block stores a copy, but for internal use you may set copy =
 * false to use the original graph.  
 */
class 
SH_DLLEXPORT
ShCfgBlock : public ShBlock {
public:
  ShCfgBlock(const ShProgram &program, bool copy=true);
  ShCfgBlock(ShCtrlGraphPtr cfg, bool copy=true);
  ShCfgBlock(ShCtrlGraphNodePtr node, bool copy=true); 

  ~ShCfgBlock();

  ShCtrlGraphNodePtr entry() const;
  ShCtrlGraphNodePtr exit() const;

  void print(std::ostream& out, int indent) const;
  void graphvizDump(std::ostream& out) const;

  
private:
  void init(ShCtrlGraphPtr cfg, bool copy);
  ShCtrlGraphNodePtr m_entry, m_exit;
};

typedef ShPointer<ShCfgBlock> ShCfgBlockPtr;
typedef ShPointer<const ShCfgBlock> ShCfgBlockCPtr;

}

#endif
