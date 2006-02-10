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
#include "ShBlock.hpp"
#include "ShStatement.hpp"
#include "ShBasicBlock.hpp"

namespace SH {

ShBlock::~ShBlock()
{
}

// ShBlockList stuff

ShBlockList::ShBlockList(bool isArgument)
  : m_isArgument(isArgument)
{
}

bool ShBlockList::isArgument()
{
  return m_isArgument;
}

void ShBlockList::addStatement(const ShStatement& statement)
{
  ShBasicBlockPtr basicBlock;
  if (!m_blocks.empty()) {
    basicBlock = shref_dynamic_cast<ShBasicBlock>(m_blocks.back());
    if (!basicBlock) {
      basicBlock = new ShBasicBlock();
      m_blocks.push_back(ShBlockPtr(basicBlock));
    }
  } else {
    basicBlock = new ShBasicBlock();
    m_blocks.push_back(ShBlockPtr(basicBlock));
  }
  basicBlock->addStatement(statement);
}

void ShBlockList::addBlock(const ShBlockPtr& statement)
{
  m_blocks.push_back(statement);
}

ShBlockPtr ShBlockList::getFront() const
{
  if (m_blocks.empty()) return 0;
  
  return m_blocks.front();
}

ShBlockPtr ShBlockList::removeFront()
{
  if (m_blocks.empty()) return 0;
  
  ShBlockPtr front = m_blocks.front();
  m_blocks.erase(m_blocks.begin());
  return front;
}

bool ShBlockList::empty() const
{
  return m_blocks.empty();
}

std::ostream& ShBlockList::print(std::ostream& out, int indent) const
{
  for (std::vector<ShBlockPtr>::const_iterator I = m_blocks.begin();
       I != m_blocks.end(); ++I) {
    (*I)->print(out, indent);
  }
  return out;
}

std::ostream& operator<<(std::ostream& out, const ShBlockList& l)
{
  l.print(out, 0);
  return out;
  
}


}


