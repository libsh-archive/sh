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

#include "ShRefCount.hpp"
#include "ShVariableNode.hpp"
#include "ShSwizzle.hpp"
#include "ShUtility.hpp"


namespace SH {

/** A reference and interface to a generic n-tuple variable.
 * Note: subclasses should not keep any additional data. All data
 * relevant to the node should be stored in m_node. This is due to
 * instances of subclasses of ShVariable being sliced when they get
 * placed in ShStatements.
*/
class ShVariable {
public:
  ShVariable();
  ShVariable(const ShVariableNodePtr& node);

  ~ShVariable() {}

  bool null() const; ///< true iff node is a null pointer.
  
  bool uniform() const; ///< Is this a uniform (non-shader specific) variable?
  bool hasValues() const; ///< Does this variable have constant
                          ///(host-local) values?
  
  int size() const; ///< Get the number of elements in this variable
  std::string name() const; ///< Get this variable's name

  /// Set this variable's name. If set to the empty string, defaults
  /// to the type and id of the variable.
  void name(const std::string& name);

  /// Obtain the swizzling (if any) applied to this variable.
  const ShSwizzle& swizzle() const;

  /// Obtain the actual node this variable refers to.
  const ShVariableNodePtr& node() const;

  /// Obtain the actual node this variable refers to.
  ShVariableNodePtr& node();
  
  /// Return true if this variable is negated
  bool neg() const;

  bool& neg();
  
  /// Get the values of this variable, with swizzling taken into account
  void getValues(ShVariableNode::ValueType dest[]) const;
  /// Set the values of this variable, using the swizzle as a
  /// writemask.
  void setValues(ShVariableNode::ValueType values[]);

  ShVariable operator()() const; ///< Identity swizzle
  ShVariable operator()(int) const;
  ShVariable operator()(int, int) const;
  ShVariable operator()(int, int, int) const;
  ShVariable operator()(int, int, int, int) const;
  ShVariable operator()(int size, int indices[]) const;
  
  
  ShVariable operator-() const;
  
  ShVariable(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg)
    : m_node(node), m_swizzle(swizzle), m_neg(neg)
  {
  }

protected:  

  ShVariableNodePtr m_node; ///< The actual variable node we refer to.
  ShSwizzle m_swizzle; ///< Swizzling applied to this variable.
  bool m_neg; ///< True iff this variable is negated

  friend std::ostream& operator<<(std::ostream& out, const ShVariable& shVariableToPrint);
};


/** A variable of length N.
 * This is only provided so that subclasses can derive from it and
 * indicate that certain operations only work on variables of certain
 * lengths.
 *
 */
template<int N, typename T>
class ShVariableN : public ShVariable 
{
public:
  /*
  ShVariableN(T v)
    : ShVariable(new ShVariableNode(SH_VAR_CONST, N))
  {
    ShIntEqual<N, 1>();
    m_node->setValue(0, v);
  }
  */
  
  ShVariableN(const ShVariableNodePtr& node)
    : ShVariable(node)
  {
  }
  ~ShVariableN() {}

  ShVariableN<N, T> operator-() const
  {
    return ShVariableN<N, T>(m_node, m_swizzle, !m_neg);
  }

  ShVariableN<N, T> operator()() const; ///< Identity swizzle
  ShVariableN<1, T> operator()(int) const;
  ShVariableN<2, T> operator()(int, int) const;
  ShVariableN<3, T> operator()(int, int, int) const;
  ShVariableN<4, T> operator()(int, int, int, int) const;

  ShVariableN(const ShVariableNodePtr& node,
              ShSwizzle swizzle,
              bool neg)
    : ShVariable(node)
  {
    m_swizzle = swizzle;
    m_neg = neg;
  }
};

}

namespace SH {

template<int N, typename T>
ShVariableN<N, T> ShVariableN<N, T>::operator()() const
{
  return ShVariableN<N, T>(m_node, m_swizzle, m_neg);
}

template<int N, typename T>
ShVariableN<1, T> ShVariableN<N, T>::operator()(int i1) const
{
  return ShVariableN<1, T>(m_node, m_swizzle * ShSwizzle(size(), i1), m_neg);
}

template<int N, typename T>
ShVariableN<2, T> ShVariableN<N, T>::operator()(int i1, int i2) const
{
  return ShVariableN<2, T>(m_node, m_swizzle * ShSwizzle(size(), i1, i2), m_neg);
}

template<int N, typename T>
ShVariableN<3, T> ShVariableN<N, T>::operator()(int i1, int i2, int i3) const
{
  return ShVariableN<3, T>(m_node, m_swizzle * ShSwizzle(size(), i1, i2, i3), m_neg);
}

template<int N, typename T>
ShVariableN<4, T> ShVariableN<N, T>::operator()(int i1, int i2, int i3, int i4) const
{
  return ShVariableN<4, T>(m_node, m_swizzle * ShSwizzle(size(), i1, i2, i3, i4), m_neg);
}


}

#endif
