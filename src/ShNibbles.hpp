// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#ifndef SHNIBBLES_HPP
#define SHNIBBLES_HPP

/** \file ShNibbles.hpp
 * A set of nibbles that are functions that return ShPrograms for
 * basic operations.
 */

#include "ShBaseTexture.hpp"
#include "ShProgram.hpp"
#include "ShMatrix.hpp"

namespace SH {

/** \brief Creates a keep nibble.
 * The keep nibble passes through one input of type T named with
 * the given name.
 */
template<typename T>
ShProgram keep(const std::string & name = ""); 

/** \brief Creates a dup nibble.
 * The dup nibble makes an extra copy of on input of type T named with
 * the given name.
 */
template<typename T>
ShProgram dup(const std::string & name = "var"); 

/** \brief Creates a lose nibble.
 * The lose nibble discards through one input of type T named with
 * the given name.
 */
template<typename T>
ShProgram lose(const std::string & name = "");

/** \brief 2D texture lookup nibble 
 * Inputs: IN(0) ShTexCoord1f texcoord
 * Outputs: OUT(0) name = tex(tc) */
template<typename T> 
ShProgram shAccess(const ShBaseTexture1D<T> &tex, const std::string & tcname = "texcoord", const std::string & name = "result");

/** \brief 2D texture lookup nibble 
 * Inputs: IN(0) ShTexCoord2f texcoord
 * Outputs: OUT(0) name = tex(tc) */
template<typename T> 
ShProgram shAccess(const ShBaseTexture2D<T> &tex, const std::string & tcname = "texcoord", const std::string & name = "result");

/** \brief Rect texture lookup nibble 
 * Inputs: IN(0) ShTexCoord2f texcoord
 * Outputs: OUT(0) name = tex(tc) */
template<typename T> 
ShProgram shAccess(const ShBaseTextureRect<T> &tex, const std::string & tcname = "texcoord", const std::string & name = "result");

/** \brief 3D texture lookup nibble 
 * Inputs: IN(0) ShTexCoord3f texcoord
 * Outputs: OUT(0) name = tex(tc) */
template<typename T> 
ShProgram shAccess(const ShBaseTexture3D<T> &tex, const std::string & tcname = "texcoord", const std::string & name = "result");

/** \brief Cube texture lookup nibble 
 * Inputs: IN(0) ShTexCoord3f texcoord
 * Outputs: OUT(0) name = tex(tc) */
template<typename T> 
ShProgram shAccess(const ShBaseTextureCube<T> &tex, const std::string & tcname = "texcoord", const std::string & name = "result");

/** \brief transformation nibble 
 * Creates a nibble that transforms a variable of type T by 
 * a matrix of type ShMatrix<Rows, Cols, Binding, V>
 * Inputs: IN(0) attrib
 * Outputs: OUT(0) T name = m | attrib
 */
template<typename T, int Rows, int Cols, ShBindingType Binding, typename T2>
ShProgram shTransform(const ShMatrix<Rows, Cols, Binding, T2> &m, const std::string & name = "result");

/** \brief Casting nibble 
 * Casts from tuple type T to type T2
 * If T::typesize < T2::typesize, pads with 0 components at end.
 * If T::typesize > T2::typesize, truncates components at end. 
 * Inputs: IN(0) T in
 * Outputs: OUT(0) T2 name 
 *
 */
template<typename T, typename T2>
ShProgram shCast(const std::string & name = "result");

/** \brief Fill-Casting nibble 
 * Casts from tuple type T to type T2
 * If T::typesize < T2::typesize, pads with repeated last component at end. 
 * If T::typesize > T2::typesize, truncates components at end. 
 * Inputs: IN(0) T in
 * Outputs: OUT(0) T2 name 
 *
 */
template<typename T, typename T2>
ShProgram shFillcast(const std::string & name = "result");

/**@{ \brief Nibbles for unary operators
 * Inputs: IN(0) T name 
 * Outputs: OUT(0) T name 
 */
template<typename T> ShProgram shAbs(const std::string & name = "result");
template<typename T> ShProgram shAcos(const std::string & name = "result");
template<typename T> ShProgram shAcosh(const std::string & name = "result");
template<typename T> ShProgram shAsin(const std::string & name = "result");
template<typename T> ShProgram shAsinh(const std::string & name = "result");
template<typename T> ShProgram shAtan(const std::string & name = "result");
template<typename T> ShProgram shAtanh(const std::string & name = "result");
template<typename T> ShProgram shCbrt(const std::string & name = "result");
template<typename T> ShProgram shCos(const std::string & name = "result");
template<typename T> ShProgram shCosh(const std::string & name = "result");
template<typename T> ShProgram shDx(const std::string & name = "result");
template<typename T> ShProgram shDy(const std::string & name = "result");
template<typename T> ShProgram shExp(const std::string & name = "result");
template<typename T> ShProgram shExpm1(const std::string & name = "result");
template<typename T> ShProgram shExp2(const std::string & name = "result");
template<typename T> ShProgram shExp10(const std::string & name = "result");
template<typename T> ShProgram shFrac(const std::string & name = "result");
template<typename T> ShProgram shFwidth(const std::string & name = "result");
template<typename T> ShProgram shLog(const std::string & name = "result");
template<typename T> ShProgram shLogp1(const std::string & name = "result");
template<typename T> ShProgram shLog2(const std::string & name = "result");
template<typename T> ShProgram shLog10(const std::string & name = "result");
template<typename T> ShProgram shNormalize(const std::string & name = "result");
template<typename T> ShProgram shPos(const std::string & name = "result");
template<typename T> ShProgram shProd(const std::string & name = "result");
template<typename T> ShProgram shRcp(const std::string & name = "result");
template<typename T> ShProgram shRsqrt(const std::string & name = "result");
template<typename T> ShProgram shSin(const std::string & name = "result");
template<typename T> ShProgram shSinh(const std::string & name = "result");
template<typename T> ShProgram shSqrt(const std::string & name = "result");
template<typename T> ShProgram shSum(const std::string & name = "result");
template<typename T> ShProgram shTan(const std::string & name = "result");
template<typename T> ShProgram shTanh(const std::string & name = "result");
//@}

/**@{ \brief Nibbles for binary operators
 * Inputs: IN(0) T1 a
 *         IN(1) T2 b (by default T2 = T1)
 * Outputs: OUT(0) name  (type is T1 or T2, whichever has more components)
 */
#define SHNIBBLE_BINARY_OP_DECL(opname) \
  template<typename T1, typename T2> ShProgram opname(const std::string & output_name = "result",\
      const std::string & input_name0 = "x", const std::string & input_name1 = "y"); \
  template<typename T1> ShProgram opname(const std::string & output_name = "result",\
      const std::string & input_name0 = "x", const std::string & input_name1 = "y"); 
SHNIBBLE_BINARY_OP_DECL(shAdd);
SHNIBBLE_BINARY_OP_DECL(shAtan2);
SHNIBBLE_BINARY_OP_DECL(shDiv);
SHNIBBLE_BINARY_OP_DECL(shMax);
SHNIBBLE_BINARY_OP_DECL(shMin);
SHNIBBLE_BINARY_OP_DECL(shMod);
SHNIBBLE_BINARY_OP_DECL(shMul);
SHNIBBLE_BINARY_OP_DECL(shPow);
SHNIBBLE_BINARY_OP_DECL(shSeq);
SHNIBBLE_BINARY_OP_DECL(shSge);
SHNIBBLE_BINARY_OP_DECL(shSgt);
SHNIBBLE_BINARY_OP_DECL(shSle);
SHNIBBLE_BINARY_OP_DECL(shSlt);
SHNIBBLE_BINARY_OP_DECL(shSne);
SHNIBBLE_BINARY_OP_DECL(shSub);
//@}

/**@{ \brief Nibbles for ternary operators
 * Inputs: IN(0) T1 a
 *         IN(1) T2 b (by default T2 = T1)
 *         IN(2) T3 c (by default T3 = T2)
 * Outputs: OUT(0) name  (type is T1, T2 or T3, whichever has more components)
 */
#define SHNIBBLE_TERNARY_OP_DECL(opname) \
  template<typename T1, typename T2, typename T3> ShProgram opname(const std::string & output_name = "result",\
      const std::string & input_name0 = "x", const std::string & input_name1 = "y", \
      const std::string & input_name2 = "z"); \
  template<typename T1, typename T2> ShProgram opname(const std::string & output_name = "result",\
      const std::string & input_name0 = "x", const std::string & input_name1 = "y", \
      const std::string & input_name2 = "z"); \
  template<typename T1> ShProgram opname(const std::string & output_name = "result",\
      const std::string & input_name0 = "x", const std::string & input_name1 = "y", \
      const std::string & input_name2 = "z"); \

SHNIBBLE_TERNARY_OP_DECL(shMad);
//@}

/**@{ \brief Nibbles for hash functions
 * Inputs: IN(0) T1 a
 * Outputs: OUT(0) name  (type is T2)
 */
#define SHNIBBLE_HASH_FUNC_DECL(opname) \
  template<typename T1, typename T2> ShProgram opname(const std::string & output_name = "result",\
      const std::string & input_name0 = "x");

SHNIBBLE_HASH_FUNC_DECL(shCellnoise);
SHNIBBLE_HASH_FUNC_DECL(shScellnoise);
SHNIBBLE_HASH_FUNC_DECL(shHash);
SHNIBBLE_HASH_FUNC_DECL(shTexhash);
//@}

/**@{ \brief Nibbles for noise functions
 * Inputs: IN(0) T1 a
 *         IN(1) T2 b (optional)
 * Outputs: OUT(0) name  (type is the last of T2 or T3)
 */
#define SHNIBBLE_NOISE_FUNC_DECL(opname) \
  template<typename T1, typename T2> ShProgram opname(const std::string & output_name = "result",\
      const std::string & input_name0 = "x"); \
  template<typename T1, typename T2, typename T3> ShProgram opname(const std::string & output_name = "result",\
      const std::string & input_name0 = "x", const std::string & input_name1 = "y");

SHNIBBLE_NOISE_FUNC_DECL(shLinnoise);
SHNIBBLE_NOISE_FUNC_DECL(shNoise);
SHNIBBLE_NOISE_FUNC_DECL(shPerlin);
SHNIBBLE_NOISE_FUNC_DECL(shTurbulence);
SHNIBBLE_NOISE_FUNC_DECL(shSlinnoise);
SHNIBBLE_NOISE_FUNC_DECL(shSnoise);
SHNIBBLE_NOISE_FUNC_DECL(shSperlin);
SHNIBBLE_NOISE_FUNC_DECL(shSturbulence);

//@}

/**@{ \brief Nibbles for distance functions
 * Inputs: IN(0) T1 a
 *         IN(1) T2 b
 * Outputs: OUT(0) ShAttrib1f name
 */
#define SHNIBBLE_DISTANCE_FUNC_DECL(opname) \
  template<typename T1, typename T2> ShProgram opname(const std::string & output_name = "result",\
      const std::string & input_name0 = "x", const std::string & input_name1 = "y"); \
  template<typename T> ShProgram opname(const std::string & output_name = "result",\
      const std::string & input_name0 = "x", const std::string & input_name1 = "y");

SHNIBBLE_DISTANCE_FUNC_DECL(shDistance);
SHNIBBLE_DISTANCE_FUNC_DECL(shDistance_1);
SHNIBBLE_DISTANCE_FUNC_DECL(shDistance_inf);
//@}

/**@{ \brief Nibbles for length functions
 * Inputs: IN(0) T1 a
 * Outputs: OUT(0) ShAttrib1f name
 */
#define SHNIBBLE_LENGTH_FUNC_DECL(opname) \
  template<typename T> ShProgram opname(const std::string & output_name = "result",\
      const std::string & input_name0 = "x");

SHNIBBLE_LENGTH_FUNC_DECL(shLength);
SHNIBBLE_LENGTH_FUNC_DECL(shLength_1);
SHNIBBLE_LENGTH_FUNC_DECL(shLength_inf);
//@}

/** \brief Nibble for cross product operator
 * Inputs: IN(0) T a
 *         IN(1) T b
 * Outputs: OUT(0) T name  
 */
template<typename T> ShProgram shCross(const std::string & name = "result",
                                       const std::string & input_name0 = "x", 
                                       const std::string & input_name1 = "y");

/** \brief Nibble for dot product operator
 * Inputs: IN(0) T a
 *         IN(1) T b
 * Outputs: OUT(0) ShAttrib1f name  
 */
template<typename T> ShProgram shDot(const std::string & name = "result");

/** \brief Nibble for the gradient function
 * Inputs: IN(0) T a
 * Outputs: OUT(0) ShAttrib1f name
 */
template<typename T> ShProgram shGradient(const std::string & name = "result");

/** @{ \brief Nibble for lerp operator
 * Inputs: IN(0) T1 a
 *         IN(1) T1 b
 *         IN(2) T2 alpha (T2 = T1 by default)
 * Outputs: OUT(0) T1 name = a * alpha + b * (1 - alpha)  
 */
template<typename T1, typename T2>
ShProgram shLerp(const std::string & name = "result");

template<typename T1>
ShProgram shLerp(const std::string & name = "result");
//@}

/** @{ \brief Nibble for poly function
 * Inputs: IN(0) T1 a
 *         IN(1) T1 b
 *         IN(2) T2 alpha (T2 = T1 by default)
 * Outputs: OUT(0) T1 name
 */
template<typename T1, typename T2>
ShProgram shPoly(const std::string & name = "result",
                 const std::string & input_name0 = "x", 
                 const std::string & input_name1 = "y");

template<typename T1>
ShProgram shPoly(const std::string & name = "result",
                 const std::string & input_name0 = "x", 
                 const std::string & input_name1 = "y");
//@}

}

#include "ShNibblesImpl.hpp"

#endif
