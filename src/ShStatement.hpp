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
#include <set>
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
  SH_OP_MUL, ///< Binary multiplication. Can be scalar on left or
             ///  right side.
  SH_OP_DIV, ///< Binary division. Can be scalar on right side.

  // All of the following set dst to either 1 or 0.
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
  SH_OP_ATAN2, ///< Binary arctan of src[1]/src[0] (result between -pi and pi)
  SH_OP_CEIL, ///< Unary smallest integer not less than src[0]
  SH_OP_COS, ///< Unary cosine function
  SH_OP_DOT, ///< Binary dot product
  SH_OP_FLR, ///< Unary smallest integer not greater than src[0]
  SH_OP_FMOD, ///< Binary float modulus, result has same sign as src[0] 
  SH_OP_FRAC, ///< Unary fractional part
  SH_OP_LRP, ///< Trinary linear interpolation src[0] * src[1]  +  (1-src[0]) * src[2]
  SH_OP_MAD, ///< Multiply and add src[0] * src[1] + src[2]
  SH_OP_MAX, ///< Binary componentwise maximum
  SH_OP_MIN, ///< Binary componentwise minimum
  SH_OP_POW, ///< Binary power
  SH_OP_SIN, ///< Unary sine function
  SH_OP_SQRT, ///< Unary square root

  SH_OP_NORM, ///< Normalize vector
  SH_OP_XPD, ///< Cross product

  SH_OP_TEX, ///< Texture lookup (0..1)
  SH_OP_TEXI, ///< Texture lookup, indexed mode (0..width - 1)

  SH_OP_COND, ///< Conditional assignment: dst[i] = (src[0][i] > 0.0 ? src[1][i] : src[2][i])

  SH_OP_KIL, ///< Conditionally kill fragment (if for any i, dst[i] > 0)

  SH_OP_OPTBRA, ///< Used in the optimizer to indicate a conditional
                ///  branch dependency. This should never show up in
                ///  code passed to the backend.

  SH_OP_FETCH, ///< Unary (takes a stream). Fetch an element from a stream. Similar to TEX
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
 * <pre>dest := src[0] op src[1]</pre>
 * or, for unary operators: 
 * <pre>dest := op src[0]</pre>
 * or, for op == SH_OP_ASN:
 * <pre>dest := src[0]</pre>
 */
class ShStatement {
public:
  ShStatement(ShVariable dest, ShOperation op);
  ShStatement(ShVariable dest, ShOperation op, ShVariable src);
  ShStatement(ShVariable dest, ShVariable src0, ShOperation op, ShVariable src1);
  ShStatement(ShVariable dest, ShOperation op, ShVariable src0, ShVariable src1, ShVariable src2);
  
  ShVariable dest;
  ShVariable src[3];
  
  ShOperation op;

  // The following are used for the optimizer.
  
  std::set<ShStatement*> ud[3];
  std::set<ShStatement*> du;

  bool marked;
};

std::ostream& operator<<(std::ostream& out, const SH::ShStatement& stmt);

} // namespace SH

#endif
