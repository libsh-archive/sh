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
#include "ShSectionUtil.hpp"

namespace {
using namespace SH;

void setNestingLevel(NestingLevelMap& nlm, ShStructuralNodePtr node, unsigned int level)
{
  if(node->type == ShStructuralNode::SECTION) ++level;
  if(node->cfg_node) nlm[node->cfg_node] = level;
  for(ShStructuralNode::StructNodeList::iterator I = node->structnodes.begin();
      I != node->structnodes.end(); ++I) {
    setNestingLevel(nlm, *I, level);
  }
}

/* Fills in srm using the given index as the starting point (i.e. first index) for 
 * the subtree rooted at node
 *
 * @return next unused index
 */
int setPostorderRange(StructRangeMap& srm, ShStructuralNodePtr node, unsigned int index)
{
  std::pair<int, int> range;
  range.first = index;

  for(ShStructuralNode::StructNodeList::iterator I = node->structnodes.begin();
      I != node->structnodes.end(); ++I) {
    index = setPostorderRange(srm, *I, index);
  }
  range.second = index++; // add index for this node
  srm[node] = range;
  return index;
}

void setCfgSections(CfgSectionMap& csm, ShStructuralNodePtr node, ShStructuralNodePtr section)
{
  if(node->type == ShStructuralNode::SECTION) {
    section = node;
  }
  if(node->cfg_node) csm[node->cfg_node] = section;

  for(ShStructuralNode::StructNodeList::iterator I = node->structnodes.begin();
      I != node->structnodes.end(); ++I) {
    setCfgSections(csm, *I, section);
  }
}

void setParent(SectionParentMap& spm, ShStructuralNodePtr node, ShStructuralNodePtr parent)
{
  if(node->type == ShStructuralNode::SECTION) {
    spm[node] = parent;
    parent = node;
  }

  for(ShStructuralNode::StructNodeList::iterator I = node->structnodes.begin();
      I != node->structnodes.end(); ++I) {
    setParent(spm, *I, parent);
  }
}

}
namespace SH {
NestingLevelMap sectionNestingLevel(ShStructural &s)
{
  NestingLevelMap result;
  setNestingLevel(result, s.head(), 0); 
  return result;
}

StructRangeMap postorderRange(ShStructural &s)
{
  StructRangeMap result;
  setPostorderRange(result, s.head(), 0);
  return result;
}

CfgSectionMap gatherCfgSection(ShStructural &s)
{
  CfgSectionMap result;
  setCfgSections(result, s.head(), 0);
  return result;
}

SectionParentMap findParents(ShStructural &s)
{
  SectionParentMap result;
  setParent(result, s.head(), 0);
  return result;
}


}
