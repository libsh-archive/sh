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
#ifndef SHCTRLGRAPHWRANGLERS_HPP
#define SHCTRLGRAPHWRANGLERS_HPP

#include "ShCtrlGraph.hpp"
#include "ShStructural.hpp"
#include "ShBasicBlock.hpp"
#include "ShProgram.hpp"

/** @file ShCtrlGraphWranglers.hpp
 * A number of utility functions for altering control graph structures.
 */
namespace SH {

/** Splices a program into a ctrl graph node after stmt. 
 *
 * This splits the node's block in two.  The original node keeps all stmts up to and including stmt and the second node
 * takes over all stmts after stmt. Then the ctrl graph of program gets planted right between the two nodes.
 *
 * All stmtlist iterators remain valid, but any iterators from node's stmt
 * list after stmt now point into a list in the newly split off node. 
 *
 * The splice uses program's ctrl graph directly (no copy involved), so
 * the marked state of nodes does not change (This may be important to know for those
 * of you that love doing a dfs through the ctrl graph while splicing new bits 
 * in front of the dfs...)
 */
SH_DLLEXPORT
void spliceProgram(ShCtrlGraphNodePtr node, ShBasicBlock::ShStmtList::iterator stmt, ShProgram &program); 

/** Replaces a statement with a program, mapping the stmt's srcs and dest to the program inputs and first output.
 *
 * This is just some extra sugar on top of the spliceProgram op above.
 *
 * The difference is that since stmt gets removed, the iterator is not valid.
 * Everything else said above about other valid iterators still applies. 
 *
 * Afterwards, there are no more statements left to iterate over in the original
 * node.
 */
SH_DLLEXPORT
void replaceStmt(ShCtrlGraphNodePtr node, ShBasicBlock::ShStmtList::iterator stmt, ShProgram &program); 

/* Removes a statement from a block */ 
SH_DLLEXPORT
void removeStmt(ShCtrlGraphNodePtr node, ShBasicBlock::ShStmtList::iterator stmt);


/** Takes any edges in the structural graph between from and to, and
 * replaces corresponding edges in the cfg by unconditional edges between from and fromCfg
 * and toCfg and to 
 */
SH_DLLEXPORT
void structSplit(ShStructuralNodePtr from, ShStructuralNodePtr to, ShCtrlGraphNodePtr fromCfg,
    ShCtrlGraphNodePtr toCfg); 

// replace all edges (from,to) that leave node with unconditional edges
// (from, head) and (tail, to)
//
// @todo range - this currently only works if all node exits go to the same
// place 
//
SH_DLLEXPORT
void structReplaceExits(ShStructuralNodePtr node, 
                        ShCtrlGraphNodePtr head, ShCtrlGraphNodePtr tail); 

// replace all edges (from, to) that enter node with edges (from, head) that
// keep their original cond var (or lack of cond var),
// and unconditional edge (tail, to)
//
// @todo range - this currently only works if there is a single entry into
// node in all matching pred edges
SH_DLLEXPORT
void structReplaceEntries(ShStructuralNodePtr node, 
                          ShCtrlGraphNodePtr head, ShCtrlGraphNodePtr tail); 

/* @todo range assumes single exit destination cfg node.
 *
 * Removes the node from the graph, redirecting pred edges to the exit
 * destination. 
 */
SH_DLLEXPORT
void structRemove(ShStructuralNodePtr node);

}

#endif
