// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Bryan Chan, Michael D. McCool
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
#include "ShKernelLib.hpp"
#include "ShUtil.hpp"

namespace ShUtil {

std::string ShKernelLib::makeName(std::string prefix, int index) {
  std::ostringstream name;
  if( index == 0 ) return prefix;
  name << prefix << index;
  return name.str();
}

ShProgram ShKernelLib::outputPass( const ShProgram &p ) {
  ShProgram passer = SH_BEGIN_PROGRAM() {
    for( ShProgramNode::VarList::const_iterator it = p->outputs.begin();
        it != p->outputs.end(); ++it ) {
      ShVariableNodePtr var = *it;
      ShVariable inout(new ShVariableNode(SH_INOUT, var->size(), var->specialType()));
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

ShProgram ShKernelLib::bump() {
  ShProgram kernel = SH_BEGIN_PROGRAM() {
    ShInputAttrib2f SH_DECL(gradient);
    ShInOutNormal3f SH_DECL(normal);
    normal(1,2) += gradient;
    normal = normalize( normal );
  } SH_END;
  return kernel;
}

ShProgram ShKernelLib::vcsBump() {
  ShProgram kernel = SH_BEGIN_PROGRAM() {
    ShInputAttrib2f SH_DECL(gradient);
    ShInOutNormal3f SH_DECL(normal);
    ShInputVector3f SH_DECL(tangent);
    ShInputVector3f SH_DECL(tangent2);

    normal += gradient(0) * tangent;
    normal += gradient(1) * tangent2;
    normal = normalize( normal );
  } SH_END;
  return kernel;
}

};
