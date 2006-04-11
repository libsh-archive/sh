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
#include <sstream>
#include "KernelLib.hpp"
#include "Func.hpp"

namespace ShUtil {

std::string KernelLib::makeName(std::string prefix, int index) {
  std::ostringstream name;
  if( index == 0 ) return prefix;
  name << prefix << index;
  return name.str();
}

Program KernelLib::outputPass( const Program &p ) {
  Program passer = SH_BEGIN_PROGRAM() {
    for( ProgramNode::VarList::const_iterator it = p.node()->outputs.begin();
        it != p.node()->outputs.end(); ++it ) {
      VariableNodePtr var = *it;
      Variable inout(new VariableNode(INOUT, var->size(), var->valueType(), var->specialType()));
      inout.name( var->name() ); 
    }
  } SH_END;
  return passer;
}

Program KernelLib::inputPass( const Program &p ) {
  Program passer = SH_BEGIN_PROGRAM() {
    for( ProgramNode::VarList::const_iterator it = p.node()->inputs.begin();
        it != p.node()->inputs.end(); ++it ) {
      VariableNodePtr var = *it;
      Variable inout(new VariableNode(INOUT, var->size(), var->valueType(), var->specialType()));
      inout.name( var->name() ); 
    }
  } SH_END;
  return passer;
}

Program KernelLib::change_basis(std::string name, 
    std::string b0Name, std::string b1Name, std::string b2Name) {
  Program kernel = SH_BEGIN_PROGRAM() {
    InputVector3f NAMEDECL( b0, b0Name );
    InputVector3f NAMEDECL( b1, b1Name );
    InputVector3f NAMEDECL( b2, b2Name );
    InOutVector3f NAMEDECL( vec, name ) = changeBasis(b0, b1, b2, vec);
  } SH_END;
  return kernel;
}

};
