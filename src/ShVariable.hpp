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
#ifndef SHVARIABLE_HPP
#define SHVARIABLE_HPP

#include "ShDllExport.hpp"
#include "ShRefCount.hpp"
#include "ShSwizzle.hpp"
#include "ShUtility.hpp"
#include "ShMetaForwarder.hpp"
#include "ShVariableNode.hpp"

namespace SH {

class ShProgram;

/** A reference and interface to a generic n-tuple variable.
 * Note: subclasses should not keep any additional data. All data
 * relevant to the node should be stored in m_node. This is due to
 * instances of subclasses of ShVariable being sliced when they get
 * placed in ShStatements.
*/
class 
SH_DLLEXPORT ShVariable : public ShMetaForwarder {
public:
  ShVariable();
  ShVariable(const ShVariableNodePtr& node);
  ShVariable(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);

  ~ShVariable() {}

  ShVariable& operator=(const ShProgram& prg);
  
  bool null() const; ///< true iff node is a null pointer.
  
  bool uniform() const; ///< Is this a uniform (non-shader specific) variable?
  bool hasValues() const; ///< Does this variable have constant
                          ///(host-local) values?
  
  int size() const; ///< Get the number of elements in this variable,
                    /// after swizzling.
                    
  int typeIndex() const; ///< Returns index of the data type held in this node, or 0 if no node. 

  /**@name Metadata
   * This data is useful for various things, including asset
   * management.
   */
  //@{

  
  /// Set a range of values for this variable
  // TODO check if this works when swizzle contains one index more than once
  void rangeVariant(ShPointer<const ShVariant> low, ShPointer<const ShVariant> high);

  /// Obtain a lower bounds on this variable (tuple of same size as this)
  ShPointer<ShVariant> lowBoundVariant() const;

  /// Obtain an upper bounds on this variable (tuple of same size as this)
  ShPointer<ShVariant> highBoundVariant() const;

  //@}
  
  /// Obtain the swizzling (if any) applied to this variable.
  const ShSwizzle& swizzle() const;

  /// Obtain the actual node this variable refers to.
  const ShVariableNodePtr& node() const;

  /// Return true if this variable is negated
  bool neg() const;

  bool& neg();

  ///
  
  /// Gets a copy of the variant (with swizzling & proper negation)
  ShPointer<ShVariant> getVariant() const;
  ShPointer<ShVariant> getVariant(int index) const;
  
  /// sets up to num elements starting at index in this variant 
  /// from the other variant, accounting for swizzles and negation
  void setVariant(ShPointer<const ShVariant> other, bool neg, const ShSwizzle &writemask);
  void setVariant(ShPointer<const ShVariant> other, int index);
  void setVariant(ShPointer<const ShVariant> other);

  ShVariable operator()() const; ///< Identity swizzle
  ShVariable operator()(int) const;
  ShVariable operator()(int, int) const;
  ShVariable operator()(int, int, int) const;
  ShVariable operator()(int, int, int, int) const;
  ShVariable operator()(int size, int indices[]) const;
  
  ShVariable operator-() const;

  bool operator==(const ShVariable& other) const;
  bool operator!=(const ShVariable& other) const { return !((*this) == other); }

protected:
  
  ShVariableNodePtr m_node; ///< The actual variable node we refer to.
  ShSwizzle m_swizzle; ///< Swizzling applied to this variable.
  bool m_neg; ///< True iff this variable is negated

  friend SH_DLLEXPORT std::ostream& operator<<(std::ostream& out, const ShVariable& shVariableToPrint);
};

}

#endif
