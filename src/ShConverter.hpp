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
#ifndef SHCONVERTER_HPP
#define SHCONVERTER_HPP

#include <map>
#include <vector>
#include "ShProgram.hpp"

namespace SH {
 
  enum UsageType {
    SH_USAGE_R, 
    SH_USAGE_W, 
    SH_USAGE_RW
  };

  struct VarUsage {

    /// what blocks is this variable used
    ShBasicBlockPtr block;

    /// usage type
    UsageType usage;
  };


  /// it shows how each variable is used
  typedef std::map<ShVariableNodePtr, std::vector<VarUsage> > VarUsageMap;


  /// var reassignment map
  typedef std::map<ShVariableNodePtr, ShVariableNodePtr> VarRelocation;

  /// block to program map
  typedef std::map<ShVariableNodePtr, ShProgram> Block2ProgramMap;

/* **********************
 * Class ShConverter is responsible to build the high level program graph from the low level ctrl graph
 * Steps:
 * 	1 - Run BuildVarList: this creates a map (var_usage_map) that shows what variables 
 * 	    are used in what block. This functionality will be replaced by the use def chains
 * 	2 - 
 * **********************/
  class ShConverter {
  public:
    ShConverter(ShProgram& prgm);
    
    // build the variable dependency list
    void BuildVarList();

    /// generate one program out of a basic block
    ShProgram generateProgram(ShBasicBlockPtr block);


    void operator()(ShCtrlGraphNodePtr);

  private:
    
    /// the program that we are splitting
    ShProgram program;

    /// this map shows where the variables are used and in what way
    VarUsageMap var_usage_map;

    /// we need a texture allocator
    

    /// this map points from the bssic blocks to the actual ShPrograms
    Block2ProgramMap program_map;
    
  private:
    /// private helper functions
    

    /// build the var list for one particular block
    void BuildVarList(ShBasicBlockPtr);
    
    /// build the programs and pu them into the program_map 
    void BuildProgramMap();
    void BuildProgramMap(ShBasicBlockPtr);

    /// adds the basic block pointer to the variable map
    void MapVar(const ShVariable& var, UsageType, ShBasicBlockPtr );

    /// duplicate a variable
    ShVariableNodePtr ShConverter::ReplaceVar(ShVariableNodePtr var, VarUsage usage );

    /// select veriable for the () operator
    /// if select == 0 do nothing, 1 == for variable list, 2 - for program generation
    int select;

  };// class ShConverter


}

#endif
