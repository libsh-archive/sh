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

  SH_OP_SLT, ///< Set less than
  SH_OP_SLE, ///< Set less than or equal
  SH_OP_SGT, ///< Set greater than
  SH_OP_SGE, ///< Set greater than or equal
  SH_OP_SEQ, ///< Set equal
  SH_OP_SNE, ///< Set not equal
  
  SH_OP_ABS, ///< Unary absolute value
  SH_OP_ACOS, ///< Unary arccosine (result between -pi/2 and pi/2)
  SH_OP_ASIN, ///< Unary arcsine (result between 0 and pi)
  SH_OP_ATAN, ///< Unary arctan (result between -pi/2 and pi/2)
  SH_OP_ATAN2, ///< Binary arctan of src2/src1 (result between -pi and pi)
  SH_OP_CEIL, ///< Unary smallest integer not less than src1
  SH_OP_COS, ///< Unary cosine function
  SH_OP_DOT, ///< Binary dot product
  SH_OP_FRAC, ///< Unary fractional part
  SH_OP_MAX, ///< Binary componentwise maximum
  SH_OP_MIN, ///< Binary componentwise minimum
  SH_OP_POW, ///< Binary power
  SH_OP_SIN, ///< Unary sine function
  SH_OP_SQRT, ///< Unary square root

  SH_OP_NORM, ///< Normalize vector

  SH_OP_TEX, ///< Texture lookup

  SH_OP_COND, ///< Conditional assignment: dst[i] = (src1[i] > 0.0 ? src2[i] : src3[i])

  SH_OP_KIL, ///< Conditionally kill fragment (if for any i, dst[i] > 0)
  // TODO: finish these
};

/** Information related to a specific operation */
struct ShOperationInfo {
  const char* name; ///< The operation's name, e.g. "ASN"
  int arity; ///< The arity of the operation. 1, 2 or 3.
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
  ShStatement(ShVariable dest, ShOperation op);
  ShStatement(ShVariable dest, ShOperation op, ShVariable src);
  ShStatement(ShVariable dest, ShVariable src1, ShOperation op, ShVariable src2);
  ShStatement(ShVariable dest, ShOperation op, ShVariable src1, ShVariable src2, ShVariable src3);
  
  ShVariable dest;
  ShVariable src1;
  ShVariable src2;
  ShVariable src3;
  
  ShOperation op;
};

std::ostream& operator<<(std::ostream& out, const SH::ShStatement& stmt);

} // namespace SH

#endif
