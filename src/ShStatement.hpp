#ifndef SHSTATEMENT_HPP
#define SHSTATEMENT_HPP

#include <iosfwd>
#include "ShVariable.hpp"

namespace SH {

/** Kinds of operations.
 *  If you change this, be sure to change the opInfo array in
 *  ShStatement.cpp
 * @see opInfo
 */
enum ShOperation {
  SH_OP_ASN, ///< Assignment
  SH_OP_NEG, ///< Unary negation
  SH_OP_ADD, ///< Binary addition
  SH_OP_MUL, ///< Binary multiplication
  SH_OP_DIV, ///< Binary division
  SH_OP_ABS, ///< Unary absolute value
  SH_OP_ACOS, ///< Unary arccosine (result between -pi/2 and pi/2)
  SH_OP_ASIN, ///< Unary arcsine (result between 0 and pi)
  SH_OP_ATAN, ///< Unary arctan (result between -pi/2 and pi/2)
  SH_OP_ATAN2, ///< Binary arctan of src2/src1 (result between -pi and pi)
  SH_OP_CEIL, ///< Unary smallest integer not less than src1
  // TODO: finish these
};

/** Information related to a specific operation */
struct ShOperationInfo {
  const char* name; ///< The operation's name, e.g. "ASN"
  int arity; ///< The arity of the operation. Either 1 or 2.
};

extern const ShOperationInfo opInfo[];

/** A single statement.
 * Represent a statement of the form 
 * <pre>dest := src1 op src2</pre>
 * or, for unary operators: 
 * <pre>dest := op src1</pre>
 * or, for op == SH_OP_ASN:
 * <pre>dest := src1</pre>
 */
class ShStatement {
public:
  ShStatement(ShVariable dest, ShOperation op, ShVariable src);
  ShStatement(ShVariable dest, ShVariable src1, ShOperation op, ShVariable src2);
              
  ShVariable dest;
  ShVariable src1;
  ShVariable src2;
  ShOperation op;
};

std::ostream& operator<<(std::ostream& out, const SH::ShStatement& stmt);

} // namespace SH

#endif
