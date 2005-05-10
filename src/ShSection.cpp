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
#include <iostream>
#include "ShSection.hpp"

namespace {
using namespace SH;

struct DefaultDumper {
  void operator()(std::ostream& out, ShCtrlGraphNodePtr node)
  {
    out << "[label=\"";
    if(node->block) {
      node->block->graphvizDump(out);
      out << "\", shape=box]";
    } else {
      out << "\", shape=circle, height=0.25]";
    }
}

  void operator()(std::ostream& out, ShSectionNodePtr node)
  {
    out << "label=\"" << node->name() << "\"" << std::endl;
  }
};

}

namespace SH {
void ShSectionNode::addChild(ShPointer<ShSectionNode> child)
{
  children.insert(child);
  child->parent = this;
}

ShSectionNode::ShSectionNode(ShStructuralNodePtr structnode)
  : structnode(structnode), 
    parent(0)
{
}


ShStatement* ShSectionNode::getStart()
{
  if(cfgNodes.empty() || !cfgNodes.front()->block 
      || cfgNodes.front()->block->empty()) {
    SH_DEBUG_PRINT("Could not find STARTSEC");
    return 0;
  }

  ShStatement* result = &*cfgNodes.front()->block->begin();
  if(result->op != SH_OP_STARTSEC) {
    SH_DEBUG_PRINT("First stmt is not a STARTSEC");
    return 0;
  }
  return result;
}

ShStatement* ShSectionNode::getEnd()
{
  if(cfgNodes.empty() || !cfgNodes.back()->block 
      || cfgNodes.back()->block->empty()) {
    SH_DEBUG_PRINT("Could not find ENDSEC");
    return 0;
  }

  ShStatement* result = &*cfgNodes.back()->block->rbegin();
  if(result->op != SH_OP_ENDSEC) {
    SH_DEBUG_PRINT("Last stmt is not a ENDSEC");
    return 0;
  }
  return result;
}

std::string ShSectionNode::name() 
{
  if(!parent) return "Main Program";                                                 

  ShStatement* start = getStart();
  if(!start) return "[null - error]";
  ShInfoComment* comment = start->get_info<ShInfoComment>();                          
  SH_DEBUG_ASSERT(comment);                                                           
  return comment->comment;   
}

int ShSectionNode::depth()
{
  if(!parent) return 0;
  return parent->depth() + 1;
}

ShSectionTree::ShSectionTree()
{
}

ShSectionTree::ShSectionTree(ShStructural& structural) 
  : root(new ShSectionNode(structural.head()))
{
  makeSection(structural.head(), root);
}

ShSectionNodePtr ShSectionTree::operator[](ShCtrlGraphNodePtr cfgNode)
{
  if(cfgSection.empty()) {
    gatherCfgSection(root);
  }
  return cfgSection[cfgNode];
}

bool ShSectionTree::contains(ShSectionNodePtr section, ShCtrlGraphNodePtr cfgNode)
{
  for(ShSectionNodePtr s = operator[](cfgNode); s; s = s->parent) {
    if(s == section) return true;
  }
  return false;
}

/* Graphviz dump */
std::ostream& ShSectionTree::dump(std::ostream& out)
{
  DefaultDumper dd;
  dump(out, dd);
  return out;
}

void ShSectionTree::makeSection(ShStructuralNodePtr structNode, ShSectionNodePtr section) 
{
  if(structNode->type == ShStructuralNode::SECTION) {
    ShSectionNodePtr newSection = new ShSectionNode(structNode); 
    section->addChild(newSection);
    section = newSection;
  }

  if(structNode->cfg_node) section->addCfg(structNode->cfg_node);

  for(ShStructuralNode::StructNodeList::iterator I = structNode->structnodes.begin();
      I != structNode->structnodes.end(); ++I) {
    makeSection(*I, section);
  }
}

void ShSectionTree::gatherCfgSection(ShSectionNodePtr section) {
  for(ShSectionNode::cfg_iterator I = section->cfgBegin(); I != section->cfgEnd(); ++I) {
    cfgSection[*I] = section;
  }
  for(ShSectionNode::iterator J = section->begin(); J != section->end(); ++J) {
    gatherCfgSection(*J);
  }
}

}
