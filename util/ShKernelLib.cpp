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

ShProgram ShKernelLib::shConvertBasis() {
  ShProgram kernel = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputVector3f SH_DECL( v0 );
    ShInputVector3f SH_DECL( v1 );
    ShInputVector3f SH_DECL( v2 );
    ShInputVector3f SH_DECL( vec );
    ShOutputVector3f SH_NAMEDECL( veco, "vec" );
    veco(0) = vec | v0;
    veco(1) = vec | v1;
    veco(2) = vec | v2;
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
