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
#include "ShKernelLib.hpp"

namespace ShUtil {

ShProgram ShKernelLib::outputPass( const ShProgram &p ) {
  ShProgram passer = SH_BEGIN_PROGRAM() {
    for( ShProgramNode::VarList::const_iterator it = p->outputs.begin();
        it != p->outputs.end(); ++it ) {
      ShVariableNodePtr var = *it;
      ShVariable input(new ShVariableNode(SH_VAR_INPUT, var->size(), var->specialType()));
      input.name( var->name() ); 

      ShVariable output(new ShVariableNode(SH_VAR_OUTPUT, var->size(), var->specialType()));
      output.name( var->name() ); 

      ShStatement stmt(output, SH_OP_ASN, input); 
      ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    }
  } SH_END;
  return passer;
}

ShProgram ShKernelLib::shConvertBasis(std::string name, 
    std::string b0Name, std::string b1Name, std::string b2Name) {
  ShProgram kernel = SH_BEGIN_PROGRAM() {
    ShInputVector3f SH_NAMEDECL( b0, b0Name );
    ShInputVector3f SH_NAMEDECL( b1, b1Name );
    ShInputVector3f SH_NAMEDECL( b2, b2Name );
    ShInputVector3f SH_NAMEDECL( vec, name );
    ShOutputVector3f SH_NAMEDECL( veco, name );
    veco(0) = vec | b0;
    veco(1) = vec | b1;
    veco(2) = vec | b2;
  } SH_END;
  return kernel;
}

ShProgram ShKernelLib::bump() {
  ShProgram kernel = SH_BEGIN_PROGRAM() {
    ShInputAttrib2f SH_DECL(gradient);
    ShInputNormal3f SH_NAMEDECL(inNormal,"normal");

    ShOutputNormal3f SH_DECL(normal);
    ShNormal3f tempNormal = inNormal;
    tempNormal(1,2) += gradient;
    normal = normalize( tempNormal );
  } SH_END;
  return kernel;
}

};
