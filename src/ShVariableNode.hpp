#ifndef SHVARIABLENODE_HPP
#define SHVARIABLENODE_HPP

#include <string>
#include "ShRefCount.hpp"

namespace SH {

enum ShVariableKind {
  SH_VAR_INPUT = 0,
  SH_VAR_OUTPUT = 1,
  SH_VAR_TEMP = 2,
  SH_VAR_CONST = 3,
  SH_VAR_TEXTURE = 4
};


/** A generic n-tuple variable.
 */
class ShVariableNode : public ShRefCountable {
public:
  ShVariableNode(ShVariableKind kind, int size);
  virtual ~ShVariableNode();

  bool uniform() const; ///< Is this a uniform (non-shader specific) variable?
  bool hasValues() const; ///< Does this variable have values in the
                          ///host, e.g. for constants and uniforms.
  int size() const; ///< Get the number of elements in this variable
  std::string name() const; ///< Get this variable's name

  /// Set this variable's name. If set to the empty string, defaults
  /// to the type and id of the variable.
  void name(const std::string& name);

  ShVariableKind kind() const;

  typedef double ValueType;
  
  /// For variables with values only. Sets the value of the i'th entry.
  /// If i is outside [0, size - 1] this is a no-op.
  ///
  ///@see hasValues()
  void setValue(int i, ValueType value);

  /// Retrieve a particular value
  ValueType getValue(int i) const;
  
protected:

  bool m_uniform;
  
  ShVariableKind m_kind;
  int m_size;
  int m_id;
  std::string m_name;

  ValueType* m_values;
  
  static int m_maxID;
};

typedef ShRefCount<ShVariableNode> ShVariableNodePtr;

}

#endif
