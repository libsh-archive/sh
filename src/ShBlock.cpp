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
    basicBlock = m_blocks.back();
    if (!basicBlock.object()) {
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


