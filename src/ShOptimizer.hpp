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
#ifndef SHOPTIMIZER_HPP
#define SHOPTIMIZER_HPP

#include <list>
#include <utility>
#include <set>
#include <map>
#include <vector>
#include "ShCtrlGraph.hpp"
#include "ShBasicBlock.hpp"
#include "ShBitSet.hpp"

namespace SH {

/** Program optimizer
 * Used internally after programs are created to (hopefully) make them
 * more efficient.
 */
class ShOptimizer {
public:
  ShOptimizer(ShCtrlGraphPtr graph);

  /// Optimize with the given agressiveness
  void optimize(int level);

private:
  ShCtrlGraphPtr m_graph;

  /**@name Copy propagation
   */
  //@{
  friend class CopyPropagator;
  void copyPropagation(bool& changed);
  //@}
  
  /**@name Move elimination
   * A local optimization, replaces occurences of
   *  t <- expr
   *  x <- t
   * with
   *  t <- expr
   *  x <- expr
   * Which may cause t to be removed.
   */
  //@{
  friend class MoveEliminator;
  void moveElimination(bool& changed);
  //@}

  /**@name Basic block straightening
   * Combines basic blocks which can be connected with another.
   */
  //@{
  friend class Straightener;
  void straighten(bool& changed);
  //@}
  
  /**@name Reaching definitions
   * Used to build the ud/du chains.
   */
  //@{
  friend class DefFinder;
  friend class InitRch;
  friend class IterateRch;
  friend class DumpRch;
  typedef std::vector<ShStatement*> DefList;
  DefList m_defs; ///< All definition statements
  typedef std::vector<ShCtrlGraphNodePtr> DefNodeList;
  DefNodeList m_defNodes; ///< Corresponding nodes to m_defs
  typedef std::map<ShCtrlGraphNodePtr, ShBitSet> ReachingMap;
  ReachingMap m_reachIn;
  void solveReachDefs();
  //@}

  /**@name ud/du chain building
   */
  //@{
  friend class UdDuBuilder;
  void buildUdDuChains();
  //@}
  
  /**@name Dead code removal
   */
  //@{
  friend class InitLiveCode;
  friend class DeadCodeRemover;
  void removeDeadCode(bool& changed);
  //@}
  
  /**@name SH_OP_OPTBRA insertion/removal
   * These instructions are inserted at each conditional branch. This
   * is to avoid the dead code remover removing code that a branch
   * depends on.
   */
  //@{
  friend class BraInstInserter;
  friend class BraInstRemover;
  void insertBraInsts();
  void removeBraInsts();
  //@}
  
  /// NOT IMPLEMENTED
  ShOptimizer(const ShOptimizer& other);
  /// NOT IMPLEMENTED
  ShOptimizer& operator=(const ShOptimizer& other);
};

}

#endif
