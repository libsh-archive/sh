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
#include "ShProgram.hpp"
#include <cassert>
#include <algorithm>
#include "ShBackend.hpp"
#include "ShEnvironment.hpp"
#include "ShDebug.hpp"
#include "ShTextureNode.hpp"
#include "ShCtrlGraph.hpp"
#include "ShError.hpp"

namespace SH {

ShProgramNode::ShProgramNode(const std::string& target)
  : m_target(target)
{
}

ShProgramNode::~ShProgramNode()
{
  // TODO: maybe need some cleanup here, although refcounting ought to
  // take care of it.
}

void ShProgramNode::compile(ShRefCount<ShBackend>& backend)
{
  if (m_target.empty()) ShError( ShException( "Invalid ShProgram target" ) );
  compile(m_target, backend);
}

void ShProgramNode::compile(const std::string& target, ShRefCount<ShBackend>& backend)
{
  if (!backend) return;

  ShEnvironment::shader = this;
  ShEnvironment::insideShader = true;
  ShBackendCodePtr code = backend->generateCode(target, this);
#ifdef SH_DEBUG
  code->print(std::cerr);
#endif
  ShEnvironment::insideShader = false;
  m_code[std::make_pair(target, backend)] = code;
}

ShRefCount<ShBackendCode> ShProgramNode::code(ShRefCount<ShBackend>& backend) {
  if (m_target.empty()) ShError( ShException( "Invalid ShProgram target" ) );

  return code(m_target, backend);
}

ShRefCount<ShBackendCode> ShProgramNode::code(const std::string& target, ShRefCount<ShBackend>& backend) {
  if (!backend) return 0;
  assert(!ShEnvironment::insideShader);

  if (m_code.find(std::make_pair(target, backend)) == m_code.end()) compile(target, backend);

  return m_code[std::make_pair(target, backend)];
}

void ShProgramNode::updateUniform(const ShVariableNodePtr& uniform)
{
  // Look to see if this program is bound anywhere, and possibly
  // update some uniforms through the corresponding backend code.
  for (ShEnvironment::BoundShaderMap::const_iterator I = ShEnvironment::boundShaders().begin();
       I != ShEnvironment::boundShaders().end(); ++I) {
    if (I->second == this) code(I->first, ShEnvironment::backend)->updateUniform(uniform);
  }
}

void ShProgramNode::collectVariables()
{
  temps.clear();
  uniforms.clear();
  constants.clear();
  textures.clear();
  if (ctrlGraph->entry()) {
    ctrlGraph->entry()->clearMarked();
    collectNodeVars(ctrlGraph->entry());
    ctrlGraph->entry()->clearMarked();
  }
}

void ShProgramNode::collectNodeVars(const ShCtrlGraphNodePtr& node)
{
  if (node->marked()) return;
  node->mark();

  if (node->block) {
    for (ShBasicBlock::ShStmtList::const_iterator I = node->block->begin();
         I != node->block->end(); ++I) {
      
      collectVar(I->dest.node());
      collectVar(I->src[0].node());
      collectVar(I->src[1].node());
      collectVar(I->src[2].node());
    }
  }

  for (std::vector<ShCtrlGraphBranch>::const_iterator J = node->successors.begin();
       J != node->successors.end(); ++J) {
    collectNodeVars(J->node);
  }
  
  if (node->follower) collectNodeVars(node->follower);
}

void ShProgramNode::collectVar(const ShVariableNodePtr& var) {
  if (!var) return;
  if (var->uniform() && var->kind() != SH_VAR_TEXTURE) {
    if (std::find(uniforms.begin(), uniforms.end(), var) == uniforms.end()) uniforms.push_back(var);
  } else switch (var->kind()) {
  case SH_VAR_INPUT:
  case SH_VAR_OUTPUT:
    // Taken care of by ShVariableNode constructor
    break;
  case SH_VAR_TEMP:
    if (std::find(temps.begin(), temps.end(), var) == temps.end()) temps.push_back(var);
    break;
  case SH_VAR_CONST:
    if (std::find(constants.begin(), constants.end(), var) == constants.end()) constants.push_back(var);
    break;
  case SH_VAR_TEXTURE:
    if (std::find(textures.begin(), textures.end(), var) == textures.end()) {
      textures.push_back(var);
      ShCubeTextureNodePtr cubetex = var;
      if (cubetex) {
        for (int i = 0; i < 6; i++) {
          ShVariableNodePtr face = cubetex->face(static_cast<ShCubeDirection>(i));
          if (!face) continue;
          if (std::find(textures.begin(), textures.end(), face) == textures.end()) textures.push_back(face);
        }
      }
    }    
    break;
  }
}

std::ostream& operator<<( std::ostream& out, const ShProgramNode::VarList &varList ) {
  out << "(";
  for( ShProgramNode::VarList::const_iterator it = varList.begin(); 
      it != varList.end(); ++it ) {
    if( it != varList.begin() ) out << ", ";
    out << (*it)->nameOfType() << " " << (*it)->name(); 
  }
  out << ")";
  return out;
}

}
