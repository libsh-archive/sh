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
#include <sstream>
#include "ShKernelLib.hpp"
#include "ShFunc.hpp"

namespace ShUtil {

std::string ShKernelLib::makeName(std::string prefix, int index) {
  std::ostringstream name;
  if( index == 0 ) return prefix;
  name << prefix << index;
  return name.str();
}

ShProgram ShKernelLib::outputPass( const ShProgram &p ) {
  ShProgram passer = SH_BEGIN_PROGRAM() {
    for( ShProgramNode::VarList::const_iterator it = p.node()->outputs.begin();
        it != p.node()->outputs.end(); ++it ) {
      ShVariableNodePtr var = *it;
      ShVariable inout(new ShVariableNode(SH_INOUT, var->size(), var->valueType(), var->specialType()));
      inout.name( var->name() ); 
    }
  } SH_END;
  return passer;
}

ShProgram ShKernelLib::inputPass( const ShProgram &p ) {
  ShProgram passer = SH_BEGIN_PROGRAM() {
    for( ShProgramNode::VarList::const_iterator it = p.node()->inputs.begin();
        it != p.node()->inputs.end(); ++it ) {
      ShVariableNodePtr var = *it;
      ShVariable inout(new ShVariableNode(SH_INOUT, var->size(), var->valueType(), var->specialType()));
      inout.name( var->name() ); 
    }
  } SH_END;
  return passer;
}

ShProgram ShKernelLib::shChangeBasis(std::string name, 
    std::string b0Name, std::string b1Name, std::string b2Name) {
  ShProgram kernel = SH_BEGIN_PROGRAM() {
    ShInputVector3f SH_NAMEDECL( b0, b0Name );
    ShInputVector3f SH_NAMEDECL( b1, b1Name );
    ShInputVector3f SH_NAMEDECL( b2, b2Name );
    ShInOutVector3f SH_NAMEDECL( vec, name ) = changeBasis(b0, b1, b2, vec);
  } SH_END;
  return kernel;
}

};
