// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
//////////////////////////////////////////////////////////////////////////////
#ifndef SHVARIABLE_HPP
#define SHVARIABLE_HPP

#include "ShDllExport.hpp"
#include "ShRefCount.hpp"
#include "ShSwizzle.hpp"
#include "ShUtility.hpp"
#include "ShMetaForwarder.hpp"
#include "ShVariant.hpp"
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
  ShVariable() 
    : ShMetaForwarder(0), m_node(0), m_swizzle(0), m_neg(false) {}
  ShVariable(const ShVariableNodePtr& node) 
    : ShMetaForwarder(node.object()), m_node(node)
    , m_swizzle(node ? node->size() : 0), m_neg(false) {}
  ShVariable::ShVariable(const ShVariableNodePtr& node,
                         const ShSwizzle& swizzle, bool neg)
    : ShMetaForwarder(node.object()), m_node(node)
    , m_swizzle(swizzle), m_neg(neg) {}

  ~ShVariable() {}

  /* Executes a program and assigns the result to this variable.
   *
   * In retained mode, the prg gets inserted into the ShProgram currently
   * being defined.
   */
  ShVariable& operator=(const ShProgram& prg);
 
  /* Attaches a ShProgram to this variable, making it a dependent uniform. */
  void attach(const ShProgram& prg);
  
  ///< true iff node is a null pointer.
  bool null() const { return !m_node; }
  
  ///< Is this a uniform (non-shader specific) variable?
  bool uniform() const { return m_node->uniform(); }

  ///< Does this variable have constant (host-local) values?
  bool hasValues() const { return m_node->hasValues(); }

  
  ///< Get the number of elements in this variable after swizzling.
  int size() const { return m_swizzle.size(); }
                    
  ///< Returns index of the data type held in this node, or 0 if no node. 
  ShValueType valueType() const 
  {
    if(!m_node) return SH_VALUETYPE_END;
    return m_node->valueType();
  }

  /**@name Metadata
   * This data is useful for various things, including asset
   * management.
   */
  //@{

  
  /// Set a range of values for this variable
  // TODO check if this works when swizzle contains one index more than once
  void rangeVariant(const ShVariant* low, const ShVariant* high);

  /// Obtain a lower bounds on this variable (tuple of same size as this)
  // @{
  ShVariantPtr lowBoundVariant() const;

  /// Obtain an upper bounds on this variable (tuple of same size as this)
  ShVariantPtr highBoundVariant() const;

  //@}
  
  /// Obtain the swizzling (if any) applied to this variable.
  const ShSwizzle& swizzle() const {  return m_swizzle; }


  /// Obtain the actual node this variable refers to.
  const ShVariableNodePtr& node() const {  return m_node; }

  /// Return true if this variable is negated
  bool neg() const {  return m_neg; }
  bool& neg() {  return m_neg; }

  ///
  
  /// Returns a copy of the variant (with swizzling & proper negation)
  ShVariantPtr getVariant() const
  {
    return m_node->getVariant()->get(m_neg, m_swizzle);
  }

  ShVariantPtr getVariant(int index) const
  {
    return m_node->getVariant()->get(m_neg, m_swizzle * ShSwizzle(size(), index)); 
  }

  /** Sets result to this' variant if possible.
   * Otherwise, if swizzling or negation are required, then 
   * makes a copy into result.
   * @returns whether a copy was allocated
   *
   * (This function should only be used internally. the ref count
   * on result will be 1 if it's allocated as a copy.  You may
   * assign this to a refcounted pointer, and then manually release a ref.
   * @todo type figure out a cleaner way) 
   *
   * Since this allows you to possibly change the variant values without
   * triggering a uniform update, if loadVariant returns false, you must
   * call updateVariant() afterwards if you change any values in result. 
   * @{
   */
  bool loadVariant(ShVariant *&result) const;
  void updateVariant() { m_node->update_all(); }

  // @}

  
  /** Sets the elements of this variant from other accounting for 
   * this' writemask and negations
   * @{
   */
  void setVariant(const ShVariant* other, bool neg, const ShSwizzle &writemask); 
  void setVariant(const ShVariantCPtr& other, bool neg, const ShSwizzle &writemask); 
  // @}
 
  /** Sets the indicated element of this' variant from other 
   * @{
   */
  void setVariant(const ShVariant* other, int index); 
  void setVariant(const ShVariantCPtr& other, int index); 
  // @}

  /** Sets this' variant from the contents of other
   * @{
   */
  void setVariant(const ShVariant* other);
  void setVariant(const ShVariantCPtr& other);
  // @}


  ShVariable operator()() const; ///< Identity swizzle
  ShVariable operator()(int) const;
  ShVariable operator()(int, int) const;
  ShVariable operator()(int, int, int) const;
  ShVariable operator()(int, int, int, int) const;
  ShVariable operator()(int size, int indices[]) const;
  ShVariable operator()(const ShSwizzle &swizzle) const;

  
  ShVariable operator-() const;

  bool operator==(const ShVariable& other) const;
  bool operator!=(const ShVariable& other) const { return !((*this) == other); }

  /// @internal used by ShMatrix to set up its rows when swizzled.
  /// Sets this variable's node, swizzle, and negation bit to be
  /// identical to the given variable.
  void clone(const ShVariable& other);
  
protected:
  
  ShVariableNodePtr m_node; ///< The actual variable node we refer to.
  ShSwizzle m_swizzle; ///< Swizzling applied to this variable.
  bool m_neg; ///< True iff this variable is negated

  friend SH_DLLEXPORT std::ostream& operator<<(std::ostream& out, const ShVariable& shVariableToPrint);
};

}

#endif
