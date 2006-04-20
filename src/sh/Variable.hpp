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

#include "DllExport.hpp"
#include "RefCount.hpp"
#include "Swizzle.hpp"
#include "Utility.hpp"
#include "MetaForwarder.hpp"
#include "Variant.hpp"
#include "VariableNode.hpp"

namespace SH {

class Program;

/** A reference and interface to a generic n-tuple variable.
 * Note: subclasses should not keep any additional data. All data
 * relevant to the node should be stored in m_node. This is due to
 * instances of subclasses of Variable being sliced when they get
 * placed in Statements.
*/
class 
SH_DLLEXPORT Variable : public MetaForwarder {
public:
  Variable();
  Variable(const VariableNodePtr& node);
  Variable(const VariableNodePtr& node, const Swizzle& swizzle, bool neg);

  ~Variable() {}

  /* Executes a program and assigns the result to this variable.
   *
   * In retained mode, the prg gets inserted into the Program currently
   * being defined.
   */
  Variable& operator=(const Program& prg);
 
  /* Attaches a Program to this variable, making it a dependent uniform. */
  void attach(const Program& prg);
  
  bool null() const; ///< true iff node is a null pointer.
  
  bool uniform() const; ///< Is this a uniform (non-shader specific) variable?
  bool hasValues() const; ///< Does this variable have constant
                          ///(host-local) values?
  
  int size() const; ///< Get the number of elements in this variable,
                    /// after swizzling.
                    
  ValueType valueType() const; ///< Returns index of the data type held in this node, or 0 if no node. 

  /**@name Metadata
   * This data is useful for various things, including asset
   * management.
   */
  //@{

  
  /// Set a range of values for this variable
  // TODO check if this works when swizzle contains one index more than once
  void rangeVariant(const Variant* low, const Variant* high);

  /// Obtain a lower bounds on this variable (tuple of same size as this)
  // @{
  VariantPtr lowBoundVariant() const;

  /// Obtain an upper bounds on this variable (tuple of same size as this)
  VariantPtr highBoundVariant() const;

  //@}
  
  /// Obtain the swizzling (if any) applied to this variable.
  const Swizzle& swizzle() const;

  /// Obtain the actual node this variable refers to.
  const VariableNodePtr& node() const;

  /// Return true if this variable is negated
  bool neg() const;

  bool& neg();

  ///
  
  /// Returns a copy of the variant (with swizzling & proper negation)
  VariantPtr getVariant() const;
  VariantPtr getVariant(int index) const;

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
  bool loadVariant(Variant *&result) const;
  void updateVariant();
  // @}

  
  /** Sets the elements of this variant from other accounting for 
   * this' writemask and negations
   * @{
   */
  void setVariant(const Variant* other, bool neg, const Swizzle &writemask); 
  void setVariant(const VariantCPtr& other, bool neg, const Swizzle &writemask); 
  // @}
 
  /** Sets the indicated element of this' variant from other 
   * @{
   */
  void setVariant(const Variant* other, int index); 
  void setVariant(const VariantCPtr& other, int index); 
  // @}

  /** Sets this' variant from the contents of other
   * @{
   */
  void setVariant(const Variant* other);
  void setVariant(const VariantCPtr& other);
  // @}


  Variable operator()() const; ///< Identity swizzle
  Variable operator()(int) const;
  Variable operator()(int, int) const;
  Variable operator()(int, int, int) const;
  Variable operator()(int, int, int, int) const;
  Variable operator()(int size, int indices[]) const;
  Variable operator()(const Swizzle &swizzle) const;

  
  Variable operator-() const;

  bool operator==(const Variable& other) const;
  bool operator!=(const Variable& other) const { return !((*this) == other); }

  /// @internal used by Matrix to set up its rows when swizzled.
  /// Sets this variable's node, swizzle, and negation bit to be
  /// identical to the given variable.
  void clone(const Variable& other);
  
protected:
  
  VariableNodePtr m_node; ///< The actual variable node we refer to.
  Swizzle m_swizzle; ///< Swizzling applied to this variable.
  bool m_neg; ///< True iff this variable is negated

  friend SH_DLLEXPORT std::ostream& operator<<(std::ostream& out, const Variable& variableToPrint);
};

}

#endif
