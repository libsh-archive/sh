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

#include "ShBackend.hpp"
#include "ShEnvironment.hpp"
#include "ShDebug.hpp"
#include "ShTextureNode.hpp"
#include "ShCtrlGraph.hpp"
#include "ShError.hpp"

#include "ShStatement.hpp"
#include "ShVariable.hpp"

#include "ShConverter.hpp"

#include <iostream>


namespace SH {

/// API functions ////////////////////////

  ShConverter::ShConverter(ShProgram& prgm)
    :program(prgm), select(0){}
    
  

  void ShConverter::BuildVarList(){	
    select = 1;
    program->ctrlGraph->dfs(*this);
    select = 0;
  }
  
  void ShConverter::BuildProgramMap(){	
    select = 2;
    program->ctrlGraph->dfs(*this);
    select = 0;
  }

  void ShConverter::BuildProgramMap(ShBasicBlockPtr block){
    program_map[block] = generateProgram(block);
  }
  
  ShProgram ShConverter::generateProgram(ShBasicBlockPtr block){
    
    /// relocation table
    VarRelocation var_relocation;
    ShBasicBlockPtr pblock = new ShBasicBlock();

    // traverse all and build the var_relocation
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      

      // process first the destination node
      ShVariableNodePtr varptr = I->dest.node();  

      /// check if we already allocated it
      VarRelocation::iterator J = var_relocation.find(varptr);
      if(J==var_relocation.end()){	

	// look for it in the var map (we must find it!!!)
	std::vector<VarUsage> vusage = var_usage_map[varptr];

	switch(vusage.size()){	
	case 0:
	  std::cout<<"Error: we have a zero length vector in BuildVarList"<<std::endl;
	  break;
	case 1:
	  // the variable is only used here!!!! so it is the simple case, just a plain temporary variable
	  var_relocation[varptr] = new ShVariableNode(SH_VAR_TEMP, 4 /* hard-code 4 for the moment */);
	break;
	default:
	  // the variable is used in multiple places, ... we will have to change it later
	   var_relocation[varptr] = new ShVariableNode(SH_VAR_TEMP, 4 /* hard-code 4 for the moment */);
	}// switch

	for (int i = 0; i < 3; i++) 
	  if(!I->src[i].null()) {
	    varptr = I->src[i].node();

	    // look for it in the var map (we must find it!!!)
	    std::vector<VarUsage> vusage = var_usage_map[varptr];

	    switch(vusage.size()){	
	    case 0:
	      std::cout<<"Error: we have a zero length vector in BuildVarList"<<std::endl;
	      break;
	    case 1:
	      // the variable is only used here!!!! so it is the simple case, just a plain temporary variable
	      var_relocation[varptr] = new ShVariableNode(SH_VAR_TEMP, 4 /* hard-code 4 for the moment */);
	      break;
	    default:
	      // the variable is used in multiple places, ... we will have to change it later
	      var_relocation[varptr] = new ShVariableNode(SH_VAR_TEMP, 4 /* hard-code 4 for the moment */);
	    }// switch
	  }// if
      }// if
      
    }// for 

    
      // traverse all and replace the variables
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      switch(opInfo[I->op].arity){
      case 0:
      	pblock->addStatement(ShStatement(
		   ShVariable(var_relocation[I->dest.node()], I->dest.swizzle(), I->dest.neg()), I->op));
	break;
      case 1:
	pblock->addStatement(ShStatement(
		   ShVariable(var_relocation[I->dest.node()], I->dest.swizzle(), I->dest.neg()), 
		   I->op, 
		   ShVariable(var_relocation[I->src[0].node()], I->dest.swizzle(), I->dest.neg())));
	break;
      case 2:
	pblock->addStatement(ShStatement(
		   ShVariable(var_relocation[I->dest.node()], I->dest.swizzle(), I->dest.neg()), 
		   ShVariable(var_relocation[I->src[0].node()], I->dest.swizzle(), I->dest.neg()), 
		   I->op, 
		   ShVariable(var_relocation[I->src[1].node()], I->dest.swizzle(), I->dest.neg())
		   ));
	break;
      case 3:
	pblock->addStatement(ShStatement(
		   ShVariable(var_relocation[I->dest.node()], I->dest.swizzle(), I->dest.neg()), 
		   I->op, 	
		   ShVariable(var_relocation[I->src[0].node()], I->dest.swizzle(), I->dest.neg()), 
		   ShVariable(var_relocation[I->src[1].node()], I->dest.swizzle(), I->dest.neg()),
		   ShVariable(var_relocation[I->src[2].node()], I->dest.swizzle(), I->dest.neg())
		   ));
	break;
    }// for 

   
    }
    // we have created the basic block, now create the actual program
    ShProgram ret = new ShProgramNode(program->kind());
    ShBlockListPtr block_list = new ShBlockList();
    block_list->addBlock(pblock);
    ShRefCount<ShCtrlGraph> gptr = new ShCtrlGraph(block_list);
    ret->ctrlGraph = gptr;

    /// not quite correct; input variables are lost
    ret->collectVariables();

    return ret;
  }


  void ShConverter::operator()(ShCtrlGraphNodePtr node){
    switch(select){
    case 0:
      std::cout<<"Select is 0. Do nothing"<<std::endl;
      break;
    case 1:
      BuildVarList(node->block);
      break;
    case 2:
      BuildProgramMap(node->block);
      break;
    default:
       std::cout<<"Unsupported select. Do nothing"<<std::endl;
    }// switch
  }


/// Helper functions /////////////////


  void ShConverter::BuildVarList(ShBasicBlockPtr block){
    
    /// sanity check
    if (!block) return;

    /// iterate through the statements
    for( ShBasicBlock::ShStmtList::const_iterator I = block->begin();I!=block->end();++I){
      
      /// map the destination variable
      MapVar(I->dest, SH_USAGE_W, block);

      /// map the source veriable
      for (int i = 0; i < 3; i++)	
        if( !I->src[i].null() ) MapVar(I->src[i], SH_USAGE_R, block);

    }// for

  }


  void ShConverter::MapVar(const ShVariable& var, UsageType usage,  ShBasicBlockPtr block){

    /// get the node ptr
    ShVariableNodePtr varptr = var.node();

    // try to find the basic block in the usage map
    VarUsageMap::iterator I = var_usage_map.find(varptr);

    /// if unfound
    if(I==var_usage_map.end()){
      VarUsage vusage;
      vusage.block = block;
      vusage.usage = usage;
      std::vector<VarUsage> usage_array;
      usage_array.push_back(vusage);

      // add it to the map
      var_usage_map[varptr] = usage_array;

    } else {
           
      // find this basic block
      int bFound = 0;
      int i;
      for(i=0;i<I->second.size();i+=1)
	if(I->second[i].block == block){
	  bFound = 1;
	  break;
	}

      // if not found
      if(!bFound){
	VarUsage vusage;
	vusage.block = block;
	vusage.usage = usage;
	
	/// add the new entry
	I->second.push_back(vusage);

      } else {
	if(I->second[i].usage !=usage)
	  I->second[i].usage = SH_USAGE_RW;
      }

    }// if ... else ...

  }/// MapVar


  ShVariableNodePtr ShConverter::ReplaceVar(ShVariableNodePtr var, VarUsage usage ){
#if 0
    switch(usage){
    case SH_USAGE_R:
      break;
    case SH_USAGE_W:
      break;
    case SH_USAGE_RW:
      break;
    default:

    }
    
    ShVariableNodePtr ret = new ShVariableNode(kind, var->size(),var->specialType());

    // other possible initializations, like set var or setname
      

    return ret;
#endif 
    return 0;
  }// replace var
    
  

} // namespace sh



