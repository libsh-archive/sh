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
#include "ShProgramGraph.hpp"
#include <iostream>
#include <cassert>

#include "ShBackend.hpp"

namespace SH {

    //ShRefCount<ShProgramNode> shp;

ShPrgmGraphNode::ShPrgmGraphNode()
  :m_marked(false), program(0), next(0), prev(0)
{
}


  ShPrgmGraphNode::ShPrgmGraphNode(ShProgram prg)
    :m_marked(false), next(0), prev(0){
    program = prg;
  }

  std::ostream& ShPrgmGraphNode::print(std::ostream& out, int indent) const
  {
    if (marked()) return out;
    mark();
    
    return out;
  }

  std::ostream& ShPrgmGraphNode::graphvizDump(std::ostream& out) const
  {
#if 0
  if (marked()) return out;
  mark();
  out << "\"" << this << "\" ";
  if (block) {
    out << " [label=\"";
    block->graphvizDump(out);
    out << "\", shape=box]";
  } else {
    out << " [label=\"\", shape=circle, height=0.25]";
  }
  out << ";" << std::endl;

  for (std::vector<ShCtrlGraphBranch>::const_iterator I = successors.begin();
       I != successors.end(); ++I) {
    I->node->graphvizDump(out);
    out << "\"" << this << "\" ";
    out << "-> ";
    out << "\"" << I->node.object() << "\" ";

    out << "[style=dashed, label=\"" << I->cond.name() << "\"]";
    out << ";" << std::endl;
  }

  if (follower) {
    follower->graphvizDump(out);
    out << "\"" << this << "\" ";
    out << "-> ";
    out << "\"" << follower.object() << "\";" << std::endl;
  }
  
#endif
  return out;
  }

bool ShPrgmGraphNode::marked() const
{
  return m_marked;
}

void ShPrgmGraphNode::mark() const
{
  m_marked = true;
}

void ShPrgmGraphNode::clearMarked() const
{
  if (!marked()) return ;
  m_marked = false;

}


void ShPrgmGraphNode::append(const ShRefCount<ShPrgmGraphNode>& node){
  next = node;

  // not sure if circular reference it is a good thing!!!
//  node->prev = *this;
}
    
void ShPrgmGraphNode::append(ShProgram prgm){
  ShRefCount<ShPrgmGraphNode> node = new ShPrgmGraphNode(prgm);
  append(node);
}


ShPrgmGraph::ShPrgmGraph(){

}

int ShPrgmGraph::getSize(){
  int ret;
  if(!root){
    ret = 0;
  }else {
    ret = 1;
    ShPrgmGraphNodePtr traverse = root;
    while(traverse->next){
      traverse = traverse->next;
      ret+=1;
    }
  }// else

  return ret;
}

ShPrgmGraph::~ShPrgmGraph()
{

}

void ShPrgmGraph::append(ShPrgmGraphNodePtr node) {
   if(!root){
    root = node;
  }else {
    ShPrgmGraphNodePtr traverse = root;
    while(traverse->next)
      traverse = traverse->next;

    traverse->next = node;
  }// else
}

void ShPrgmGraph::append(ShProgram node){
  ShPrgmGraphNodePtr pnode = new ShPrgmGraphNode(node);
  append(pnode);
}


int ShPrgmGraph::numPrograms(){

  ShPrgmGraphNodePtr traverse = root;
  int ret = 0;
  for(int k=0;;k+=1){   
    if(!traverse)
      break;
    ret+=1;
    traverse = traverse->next;
  }

  return ret;
}

ShProgram& ShPrgmGraph::getProgram(int i){

  ShPrgmGraphNodePtr traverse = root;
  
  for(int k=0;k<i;k+=1){
    
    /// I need to fic this: make sure that the return value is not null
//if(!traverse)
    //return ShProgram();
    traverse = traverse->next;
  }
  return traverse->program;
}


std::ostream& ShPrgmGraph::print(std::ostream& out, int indent) const
{
 
  
  return out;
}

std::ostream& ShPrgmGraph::graphvizDump(std::ostream& out) const
{
#if 0
  out << "digraph control {" << std::endl;
  
  if (m_entry) {
    m_entry->clearMarked();
    m_entry->graphvizDump(out);
  }
  
  out << "}" << std::endl;
#endif
  return out;
}


}// namespace sh

