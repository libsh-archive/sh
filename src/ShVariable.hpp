#ifndef SHVARIABLE_HPP
#define SHVARIABLE_HPP

#include "ShRefCount.hpp"
#include "ShVariableNode.hpp"
#include "ShSwizzle.hpp"

namespace SH {

/** A reference and interface to a generic n-tuple variable.
 * Note: subclasses should not keep any additional data. All data
 * relevant to the node should be stored in m_node. This is due to
 * instances of subclasses of ShVariable being sliced when they get
 * placed in ShStatements.
*/
class ShVariable {
public:
  ShVariable(const ShVariableNodePtr& node);

  bool null() const; ///< true iff node is a null pointer.
  
  bool uniform() const; ///< Is this a uniform (non-shader specific) variable?

  int size() const; ///< Get the number of elements in this variable
  std::string name() const; ///< Get this variable's name

  /// Set this variable's name. If set to the empty string, defaults
  /// to the type and id of the variable.
  void name(const std::string& name);

  /// Obtain the swizzling (if any) applied to this variable.
  const ShSwizzle& swizzle() const;
  
protected:

  ShVariableNodePtr m_node; ///< The actual variable node we refer to.
  ShSwizzle m_swizzle; ///< Swizzling applied to this variable.
};

/** A variable of length N.
 * This is only provided so that subclasses can derive from it and
 * indicate that certain operations only work on variables of certain
 * lengths.
 */
template<int N, typename T>
class ShVariableN : public ShVariable 
{
public:
  ShVariableN(const ShVariableNodePtr& node)
    : ShVariable(node)
  {
  }
  ~ShVariableN() {}
};

}

#endif
