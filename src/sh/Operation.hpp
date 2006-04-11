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
#ifndef SHOPERATION_HPP
#define SHOPERATION_HPP

#include "DllExport.hpp"

namespace SH {

/** Kinds of operations.
 * If you change this, be sure to change the opInfo array in
 * Operation.cpp
 * @see opInfo
 */
enum Operation {
  OP_ASN, ///< Assignment

  // Simple arithmetic
  OP_NEG, ///< Unary negation
  OP_ADD, ///< Binary addition
  OP_MUL, ///< Binary multiplication. Can be scalar on left or
             ///  right side.
  OP_DIV, ///< Binary division. Can be scalar on right side.

  // Comparisons
  // All of the following set dst to either 1 or 0.
  OP_SLT, ///< Set less than
  OP_SLE, ///< Set less than or equal
  OP_SGT, ///< Set greater than
  OP_SGE, ///< Set greater than or equal
  OP_SEQ, ///< Set equal
  OP_SNE, ///< Set not equal

  // The bulk
  OP_ABS, ///< Unary absolute value
  OP_ACOS, ///< Unary arccosine (result between -pi/2 and pi/2)
  OP_ACOSH, ///< Unary hyperbolic arccosine (result between -pi/2 and pi/2)
  OP_ASIN, ///< Unary arcsine (result between 0 and pi)
  OP_ASINH, ///< Unary hyperbolic arcsine (result between 0 and pi)
  OP_ATAN, ///< Unary arctan (result between -pi/2 and pi/2)
  OP_ATAN2, ///< Binary arctan of src[1]/src[0] (result between -pi and pi)
  OP_ATANH, ///< Unary hyperbolic arctan (result between -pi/2 and pi/2)

  OP_CBRT, ///< Unary componentwise cube root
  OP_CEIL, ///< Unary smallest integer not less than src[0]
  OP_COS, ///< Unary cosine function
  OP_COSH, ///< Unary hyperbolic cosine function
  OP_CMUL, ///< Unary product of components
  OP_CSUM, ///< Unary sum of components
  OP_DOT, ///< Binary dot product
  OP_DX, ///< Unary derivative in x
  OP_DY, ///< Unary derivative in y

  OP_EXP, ///< Unary natural exponent
  OP_EXP2, ///< Unary base-2 exponent
  OP_EXP10, ///< Unary base-10 exponent
  OP_FLR, ///< Unary smallest integer not greater than src[0]
  OP_FRAC, ///< Unary fractional part

  OP_HASH, ///< Procedural hash function

  OP_LIT, ///< Compute lighting coefficients
  OP_LOG, ///< Unary natural log
  OP_LOG2, ///< Unary base-2 log
  OP_LOG10, ///< Unary base-10 log
  OP_LRP, ///< Trinary linear interpolation src[0] * src[1]  +  (1-src[0]) * src[2]

  OP_MAD, ///< Multiply and add src[0] * src[1] + src[2]
  OP_MAX, ///< Binary componentwise maximum
  OP_MIN, ///< Binary componentwise minimum
  OP_MOD, ///< Binary float modulus, result has same sign as src[0] 

  OP_NOISE, ///< Noise function

  OP_POW, ///< Binary power
  OP_RCP, ///< Unary reciprocal
  OP_RND, ///< Unary round to neareset integer
  OP_RSQ, ///< Unary reciprocal square root

  OP_SIN, ///< Unary sine function
  OP_SINH, ///< Unary hyperbolic sine function
  OP_SGN, ///< Unary componentwise sign
  OP_SQRT, ///< Unary square root
  OP_TAN, ///< Unary tangent
  OP_TANH, ///< Unary hyperbolic tangent

  // "Vector" operations
  OP_NORM, ///< Normalize vector
  OP_XPD, ///< Cross product

  // Textures
  OP_TEX, ///< Texture lookup (0..1)
  OP_TEXI, ///< Texture lookup, indexed mode (0..width - 1)
  OP_TEXD, ///< Texture lookup with derivatives [0..1]
  OP_TEXLOD, ///< Texture lookup with LOD [0..1]
  OP_TEXBIAS, ///< Texture lookup with LOD Bias [0..1]

  // Conditionals
  OP_COND, ///< Conditional assignment: dst[i] = (src[0][i] > 0.0 ? src[1][i] : src[2][i])

  // Fragment kill
  OP_KIL, ///< Conditionally kill fragment (if for any i, src[0][i] > 0)

  // Special
  OP_OPTBRA, ///< Used in the optimizer to indicate a conditional
                ///  branch dependency. This should never show up in
                ///  code passed to the backend.
  OP_DECL,   ///< Used during Program definition to indicate temp declaration points.
                ///  These are transferred during parsing into a map in each cfg
                ///  node, and do not appear in actual cfg block statement lists.

  OP_STARTSEC, ///< Indicates the starting point of a hierarchical section
  OP_ENDSEC,  ///< Indicates the ending point of a hierarchical section

  // Streams
  OP_FETCH, ///< Unary (takes a stream). Fetch an element from a stream. Similar to TEX
  OP_LOOKUP, ///< Binary (first argument is a stream). Fetch an
                /// element from a stream with a given index. Similar to TEX

  // Palettes
  OP_PAL, ///< Binary. First argument is a palette. Palette array lookup.

  OP_COMMENT, ///< For internal use.  Stores a comment at this point in the IR.
                 ///< Statements of this kind MUST have a InfoComment attached

  // Control flow
  OP_RET,

  OPERATION_END ///< End of List marker.  Not an actual op
};

#ifdef IGNORE
#undef IGNORE
#endif

/** Information related to a specific operation */
struct
DLLEXPORT
OperationInfo {
  const char* name; ///< The operation's name, e.g. "ASN"
  int arity; ///< The arity of the operation. 1, 2 or 3.

  enum ResultSource {
    LINEAR,   // dest[i] depends only on src_j[i] for all 0 <= j < arity
    ALL,      // dest[i] depends on all elements of src_j for all 0 <= j < arity
    EXTERNAL, // Statement yields its results from an external source
              // (e.g. TEX)
    IGNORE   // Does not yield a result
  } result_source;

  bool commutative; ///< True if order of sources does not matter.
};


DLLEXPORT
extern const OperationInfo opInfo[];

} // namespace SH

#endif
