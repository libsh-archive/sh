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

#include <sstream>
#include "ShDebug.hpp"
#include "ShVariableNode.hpp"
#include "ShError.hpp"
#include "ShAlgebra.hpp"
#include "ShFixedManipulator.hpp"

namespace SH {

ShFixedManipulator::ShFixedManipulator() {
}

ShFixedManipulator::ShFixedManipulator(int channels) {
  m_channels.push_back(channels);
}


ShFixedManipulator ShFixedManipulator::operator&(const ShFixedManipulator &b) const {
  ShFixedManipulator result = *this;
  result.m_channels.insert(result.m_channels.end(), b.m_channels.begin(), b.m_channels.end());
  result.m_programs.insert(result.m_programs.end(), b.m_programs.begin(), b.m_programs.end());
  return result;
}

ShFixedManipulator ShFixedManipulator::operator&(const ShProgram &p) const {
  ShFixedManipulator result = *this;
  result.m_channels.push_back(0);
  result.m_programs.push_back(p);
  return result;
}

ShFixedManipulator shKeep(int n) {
  if( n == 0 ) ShError( ShAlgebraException( "Cannot shKeep(0)" )); 
  return ShFixedManipulator(n); 
}

ShFixedManipulator shLose(int n) {
  if( n == 0 ) ShError( ShAlgebraException( "Cannot shLose(0)" )); 
  return ShFixedManipulator(-n); 
}

ShFixedManipulator operator&( const ShProgram &p, const ShFixedManipulator &m ) {
  ShFixedManipulator result = m; 
  result.m_channels.push_front(0);
  result.m_programs.push_front(p);
  return result;
}

ShProgram operator<<(const ShProgram &p, const ShFixedManipulator &m) {
  ShProgram manip, nextKernel; 

  std::list<int>::const_iterator chanIt = m.m_channels.begin(); 
  std::list<ShProgram>::const_iterator progIt = m.m_programs.begin();
  ShProgramNode::VarList::const_iterator inputIt = p->inputs.begin(); 

  for(; chanIt != m.m_channels.end(); ++chanIt) {
    if( *chanIt == 0 ) {
      SH_DEBUG_ASSERT( progIt != m.m_programs.end() );
      nextKernel=*progIt;
      for( int i = 0; i < nextKernel->outputs.size(); ++i, ++inputIt ) {
        if( inputIt == p->inputs.end() ) { 
          ShError(ShAlgebraException( "Not enough ShProgram inputs for input manipulator" ));
        }
      }
      ++progIt;
    } else {
      int numOutputs = *chanIt;
      bool keep = numOutputs > 0;
      if( numOutputs < 0 ) numOutputs = -numOutputs;

      nextKernel = SH_BEGIN_PROGRAM() {
        for( int i = 0; i < numOutputs; ++i, ++inputIt ) {
          ShVariableNodePtr var = *inputIt;
          if( inputIt == p->inputs.end() ) {
            ShError(ShAlgebraException( "Not enough ShProgram inputs for input manipulator" ));
          }
          ShVariable output(new ShVariableNode(SH_VAR_OUTPUT,
            var->size(), var->specialType()));
          output.name(var->name());

          if( keep ) {
            ShVariable input(new ShVariableNode(SH_VAR_INPUT,
              var->size(), var->specialType()));
            input.name(var->name());
            ShStatement stmt(output, SH_OP_ASN, input);
            ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
          } 
          // otherwise, default output is zero, input is lost
        }
      } SH_END;
    }
    if( manip ) manip = manip & nextKernel;
    else manip = nextKernel;
  }

  /* There may be some other inputs remaining.  The behaviour for these
   * depends on what connect does. */
  if( manip ) return p << manip;
  return p;
}
ShProgram operator>>(const ShFixedManipulator &m, const ShProgram &p) {
  return p << m;
}
ShProgram operator<<(const ShFixedManipulator &m, const ShProgram &p) {
  ShProgram manip, nextKernel; 

  std::list<int>::const_iterator chanIt = m.m_channels.begin(); 
  std::list<ShProgram>::const_iterator progIt = m.m_programs.begin();
  ShProgramNode::VarList::const_iterator outputIt = p->outputs.begin(); 

  for(; chanIt != m.m_channels.end(); ++chanIt) {
    if( *chanIt == 0 ) {
      SH_DEBUG_ASSERT( progIt != m.m_programs.end() );
      nextKernel= *progIt;
      for( int i = 0; i < nextKernel->inputs.size(); ++i, ++outputIt ) {
        if( outputIt == p->outputs.end() ) { 
          ShError(ShAlgebraException( "Not enough ShProgram outputs for output manipulator" ));
        }
      }
      ++progIt;
    } else {
      int numInputs = *chanIt;
      bool keep = numInputs > 0;
      if( numInputs < 0 ) numInputs = -numInputs;

      nextKernel = SH_BEGIN_PROGRAM() {
        for( int i = 0; i < numInputs; ++i, ++outputIt) {
          ShVariableNodePtr var = *outputIt;
          if( outputIt == p->outputs.end() ) {
            ShError(ShAlgebraException( "Not enough ShProgram outputs for output manipulator" ));
          }
          ShVariable input(new ShVariableNode(SH_VAR_INPUT,
            var->size(), var->specialType()));
          input.name(var->name());

          if( keep ) {
            ShVariable output(new ShVariableNode(SH_VAR_OUTPUT,
              var->size(), var->specialType()));
            output.name(var->name());
            ShStatement stmt(output, SH_OP_ASN, input);
            ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
          } 
        }
      } SH_END;
    }
    if( manip ) manip = manip & nextKernel;
    else manip = nextKernel;
  }

  /* There may be some other outputs remaining.  The behaviour for these
   * depends on what connect does. */

  if( manip ) return p >> manip;
  return p;
}

ShProgram operator>>(const ShProgram &p, const ShFixedManipulator &m) {
  return m << p;
}



}

