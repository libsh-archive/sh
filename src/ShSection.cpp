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

ShSectionNode::ShSectionNode()
  : parent(0)
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

ShSectionTree::ShSectionTree(ShStructural& structural) 
  : root(new ShSectionNode())
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
  for(ShSectionNodePtr s = cfgSection[cfgNode]; s; s = s->parent) {
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
    ShSectionNodePtr newSection = new ShSectionNode(); 
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
