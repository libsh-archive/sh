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
#include "Block.hpp"
#include "Statement.hpp"
#include "BasicBlock.hpp"

namespace SH {

Block::~Block()
{
}

// BlockList stuff

BlockList::BlockList(bool isArgument)
  : m_isArgument(isArgument)
{
}

bool BlockList::isArgument()
{
  return m_isArgument;
}

void BlockList::addStatement(const Statement& statement)
{
  BasicBlockPtr basicBlock;
  if (!m_blocks.empty()) {
    basicBlock = shref_dynamic_cast<BasicBlock>(m_blocks.back());
    if (!basicBlock) {
      basicBlock = new BasicBlock();
      m_blocks.push_back(BlockPtr(basicBlock));
    }
  } else {
    basicBlock = new BasicBlock();
    m_blocks.push_back(BlockPtr(basicBlock));
  }
  basicBlock->addStatement(statement);
}

void BlockList::addBlock(const BlockPtr& statement)
{
  m_blocks.push_back(statement);
}

BlockPtr BlockList::getFront() const
{
  if (m_blocks.empty()) return 0;
  
  return m_blocks.front();
}

BlockPtr BlockList::removeFront()
{
  if (m_blocks.empty()) return 0;
  
  BlockPtr front = m_blocks.front();
  m_blocks.erase(m_blocks.begin());
  return front;
}

bool BlockList::empty() const
{
  return m_blocks.empty();
}

std::ostream& BlockList::print(std::ostream& out, int indent) const
{
  for (std::vector<BlockPtr>::const_iterator I = m_blocks.begin();
       I != m_blocks.end(); ++I) {
    (*I)->print(out, indent);
  }
  return out;
}

std::ostream& operator<<(std::ostream& out, const BlockList& l)
{
  l.print(out, 0);
  return out;
  
}


}


