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
#ifndef SHAASCANCORE_HPP
#define SHAASCANCORE_HPP

#include <map>
#include "DllExport.hpp"
#include "Program.hpp"
#include "Record.hpp"
#include "Inclusion.hpp"
#include "Statement.hpp"
#include "AaSyms.hpp"
#include "AaSymPlacer.hpp"
#include "AaVariable.hpp"

/** @file AaScanCore.hpp 
 *
 * Given a regular program (at this point it should not contain any affine arithmetic), 
 * this can run several affine arithmetic analyses on the program
 */

namespace SH {

class
SH_DLLEXPORT AaScanCore {
  public:
    // Given a regular program, this performs the AA inclusion transformation
    // and static symbol assignment to prepare for running affine arithmetic
    // analyses
    AaScanCore(Program program);
    
    typedef std::vector<AaVariable> AaVarVec;

    // domain splitting analysis, currently you can only analyze
    // one input variable at a time by pa
    //
    // @param input The variable node to analyze (use input.null() to indicate
    // using all default ranges)
    // @param inputStream stream of tuple pairs (center, error) to use for analyzing input  
    // @param Returns the pattern of the result variables in here as temp
    //        variables. 
    // @param Returns a record using the variables in pattern 
    //
    // pattern and record are enough to easily retrieve single
    // elements from the stream and understand their contents.
    Stream doit(VariableNodePtr input, const Stream& inputStream, 
                  AaVarVec& pattern, Record& record);
     
    /** should really move this over to Inclusion as a utility func */
    typedef std::map<StmtIndex, Statement*> IndexStmtMap;

    typedef std::pair<VariableNodePtr, int> VarIntPair; 
    typedef std::pair<Statement*, int> StmtIntPair;

    // functions to access error symbols
    bool isInputSym(int idx);
    bool isStmtSym(int idx);

    // returns an input in the original program corresponding to a sym
    VarIntPair getInput(int idx);

    // returns a statement in the original program corresponding to a sym
    StmtIntPair  getStmt(int idx);


  private:
    // Returns 

    Program m_program;
    Program m_aa_program;
    Program m_aa_opped;
    AaProgramSyms *m_psyms;

    typedef std::map<VariableNodePtr, VariableNodePtr> VarVarMap;
    VarVarMap m_inputMap; ///< Maps inputs in m_program to m_aa_program
    VarVarMap m_outputMap; ///< Maps outputs in m_aa_program to m_program

    IndexStmtMap m_idxStmt; ///< Maps from statement index to statement in m_program

    typedef std::map<int, VarIntPair>  IndexVarMap;
    IndexVarMap m_symInput; ///< Maps from noise symbol idx to input variable tuple elements

    typedef std::map<int, StmtIntPair> IndexStmtElmMap;
    IndexStmtElmMap m_symStmt; ///< Maps from noise symbol idx to stmt dest tuple element  
};

}

#endif
