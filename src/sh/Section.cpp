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
#include "Section.hpp"

namespace {
using namespace SH;

struct DefaultDumper {
  void operator()(std::ostream& out, CtrlGraphNode* node)
  {
    out << "[label=\"";
    if(node->block) {
      node->block->graphviz_dump(out);
      out << "\", shape=box]";
    } else {
      out << "\", shape=circle, height=0.25]";
    }
}

  void operator()(std::ostream& out, const SectionNodePtr& node)
  {
    out << "label=\"" << node->name() << "\"" << std::endl;
  }
};

}

namespace SH {
void SectionNode::addChild(Pointer<SectionNode> child)
{
  children.insert(child);
  child->parent = this;
}

SectionNode::SectionNode()
  : parent(0)
{
}


Statement* SectionNode::getStart()
{
  if(cfgNodes.empty() || !cfgNodes.front()->block 
      || cfgNodes.front()->block->empty()) {
    SH_DEBUG_PRINT("Could not find STARTSEC");
    return 0;
  }

  Statement* result = &*cfgNodes.front()->block->begin();
  if(result->op != OP_STARTSEC) {
    SH_DEBUG_PRINT("First stmt is not a STARTSEC");
    return 0;
  }
  return result;
}

Statement* SectionNode::getEnd()
{
  if(cfgNodes.empty() || !cfgNodes.back()->block 
      || cfgNodes.back()->block->empty()) {
    SH_DEBUG_PRINT("Could not find ENDSEC");
    return 0;
  }

  Statement* result = &*cfgNodes.back()->block->rbegin();
  if(result->op != OP_ENDSEC) {
    SH_DEBUG_PRINT("Last stmt is not a ENDSEC");
    return 0;
  }
  return result;
}

std::string SectionNode::name() 
{
  if(!parent) return "Main Program";                                                 

  Statement* start = getStart();
  if(!start) return "[null - error]";
  InfoComment* comment = start->get_info<InfoComment>();                          
  SH_DEBUG_ASSERT(comment);                                                           
  return comment->comment;   
}

int SectionNode::depth()
{
  if(!parent) return 0;
  return parent->depth() + 1;
}

SectionTree::SectionTree(Structural& structural) 
  : root(new SectionNode())
{
  makeSection(structural.head(), root);
}

SectionNodePtr SectionTree::operator[](CtrlGraphNode* cfgNode)
{
  if(cfgSection.empty()) {
    gatherCfgSection(root);
  }
  return cfgSection[cfgNode];
}

bool SectionTree::contains(const SectionNodePtr& section, CtrlGraphNode* cfgNode)
{
  for(SectionNodePtr s = cfgSection[cfgNode]; s; s = s->parent) {
    if(s == section) return true;
  }
  return false;
}

/* Graphviz dump */
std::ostream& SectionTree::dump(std::ostream& out)
{
  DefaultDumper dd;
  dump(out, dd);
  return out;
}

void SectionTree::makeSection(const StructuralNodePtr& structNode, SectionNodePtr section) 
{
  if(structNode->type == StructuralNode::SECTION) {
    SectionNodePtr newSection = new SectionNode(); 
    section->addChild(newSection);
    section = newSection;
  }

  if(structNode->cfg_node) section->addCfg(structNode->cfg_node);

  for(StructuralNode::StructNodeList::iterator I = structNode->structnodes.begin();
      I != structNode->structnodes.end(); ++I) {
    makeSection(*I, section);
  }
}

void SectionTree::gatherCfgSection(const SectionNodePtr& section) {
  for(SectionNode::cfg_iterator I = section->cfgBegin(); I != section->cfgEnd(); ++I) {
    cfgSection[*I] = section;
  }
  for(SectionNode::iterator J = section->begin(); J != section->end(); ++J) {
    gatherCfgSection(*J);
  }
}

}
