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
#ifndef SHTRANSFORMER_HPP
#define SHTRANSFORMER_HPP

#include "ShBackend.hpp"
#include "ShCtrlGraph.hpp"

namespace SH {

/** Program transformer 
 * Final pre-backend stage that applies some simple transforms to the
 * code so it is compatible with a specific backend.
 */
class ShTransformer {
public:
  typedef std::vector<ShVariableNodePtr> VarNodeVec;
  typedef std::map<ShVariableNodePtr, VarNodeVec> VarSplitMap;

  ShTransformer(ShProgram program, ShBackendPtr backend);

  ShTransformer::~ShTransformer();
  
  /// Apply transformations to the IR specific to this backend
  // Returns true if anything was changed
  bool transform();

  /// After running transform, this returns any split variables 
  const VarSplitMap& splits() const;

private:
  ShProgram m_program;
  ShCtrlGraphPtr m_graph;
  ShBackendPtr m_backend;
  VarSplitMap m_splits;

  /**@name Tuple splitting when backend canot support arbitrary 
   * length tuples.
   *
   * If any tuples are split, this adds entries to the splits map
   * to map from original long tuple ShVariableNode
   * to a vector of ShVariableNodes all <= max tuple size of the backend.
   * All long tuples in the intermediate representation are split up into
   * shorter tuples.
   *
   * This is done only if SH_BACKEND_MAX_TUPLE != 0 (default unlimited length tuples)
   * and it sets the VarSplitMap for use by the backend.
   */
  //@{
  friend struct VariableSplitter; // makes a map of large tuples to split tuples
  friend struct StatementSplitter; // updates statements involving large tuples
  void tupleSplitting(bool& changed);
  //@}
  
  /**@name Input and Output variable to temp convertor
   * In most GPU shading languages/assembly, outputs cannot be used as src
   * variable in computation and inputs cannot be used as a dest.  
   * This converts outputs and inputs used in computation 
   * into a temporaries.
   *
   * This is done only if SH_BACKEND_USE_INPUT_DEST or 
   * SH_BACKEND_USE_OUTPUT_SRC are 0.
   */
  friend struct InputOutputConvertor;
  void inputOutputConversion(bool& changed);

  /// NOT IMPLEMENTED
  ShTransformer(const ShTransformer& other);
  /// NOT IMPLEMENTED
  ShTransformer& operator=(const ShTransformer& other);
};

}

#endif
