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
#ifndef SHNIBBLES_HPP
#define SHNIBBLES_HPP

/** \file Nibbles.hpp
 * A set of nibbles that are functions that return Programs for
 * basic operations.
 */

#include "BaseTexture.hpp"
#include "Program.hpp"
#include "Matrix.hpp"

namespace SH {

/** \brief Creates a keep nibble.
 * The keep nibble passes through one input of type T named with
 * the given name.
 */
template<typename T>
Program keep(const std::string & name = ""); 

/** \brief Creates a dup nibble.
 * The dup nibble makes an extra copy of on input of type T named with
 * the given name.
 */
template<typename T>
Program dup(const std::string & name = "var"); 

/** \brief Creates a lose nibble.
 * The lose nibble discards through one input of type T named with
 * the given name.
 */
template<typename T>
Program lose(const std::string & name = "");

/** \brief 2D texture lookup nibble 
 * Inputs: IN(0) TexCoord1f texcoord
 * Outputs: OUT(0) name = tex(tc) */
template<typename T> 
Program access(const BaseTexture1D<T> &tex, const std::string & tcname = "texcoord", const std::string & name = "result");

/** \brief 2D texture lookup nibble 
 * Inputs: IN(0) TexCoord2f texcoord
 * Outputs: OUT(0) name = tex(tc) */
template<typename T> 
Program access(const BaseTexture2D<T> &tex, const std::string & tcname = "texcoord", const std::string & name = "result");

/** \brief Rect texture lookup nibble 
 * Inputs: IN(0) TexCoord2f texcoord
 * Outputs: OUT(0) name = tex(tc) */
template<typename T> 
Program access(const BaseTextureRect<T> &tex, const std::string & tcname = "texcoord", const std::string & name = "result");

/** \brief 3D texture lookup nibble 
 * Inputs: IN(0) TexCoord3f texcoord
 * Outputs: OUT(0) name = tex(tc) */
template<typename T> 
Program access(const BaseTexture3D<T> &tex, const std::string & tcname = "texcoord", const std::string & name = "result");

/** \brief Cube texture lookup nibble 
 * Inputs: IN(0) TexCoord3f texcoord
 * Outputs: OUT(0) name = tex(tc) */
template<typename T> 
Program access(const BaseTextureCube<T> &tex, const std::string & tcname = "texcoord", const std::string & name = "result");

/** \brief transformation nibble 
 * Creates a nibble that transforms a variable of type T by 
 * a matrix of type Matrix<Rows, Cols, Binding, V>
 * Inputs: IN(0) attrib
 * Outputs: OUT(0) T name = m | attrib
 */
template<typename T, int Rows, int Cols, BindingType Binding, typename T2>
Program transform(const Matrix<Rows, Cols, Binding, T2> &m, const std::string & name = "result");

/** \brief Casting nibble 
 * Casts from tuple type T to type T2
 * If T::typesize < T2::typesize, pads with 0 components at end.
 * If T::typesize > T2::typesize, truncates components at end. 
 * Inputs: IN(0) T in
 * Outputs: OUT(0) T2 name 
 *
 */
template<typename T, typename T2>
Program cast(const std::string & name = "result");

/** \brief Fill-Casting nibble 
 * Casts from tuple type T to type T2
 * If T::typesize < T2::typesize, pads with repeated last component at end. 
 * If T::typesize > T2::typesize, truncates components at end. 
 * Inputs: IN(0) T in
 * Outputs: OUT(0) T2 name 
 *
 */
template<typename T, typename T2>
Program fillcast(const std::string & name = "result");

/**@{ \brief Nibbles for unary operators
 * Inputs: IN(0) T name 
 * Outputs: OUT(0) T name 
 */
template<typename T> Program abs(const std::string & name = "result");
template<typename T> Program acos(const std::string & name = "result");
template<typename T> Program acosh(const std::string & name = "result");
template<typename T> Program asin(const std::string & name = "result");
template<typename T> Program asinh(const std::string & name = "result");
template<typename T> Program atan(const std::string & name = "result");
template<typename T> Program atanh(const std::string & name = "result");
template<typename T> Program cbrt(const std::string & name = "result");
template<typename T> Program ceil(const std::string & name = "result");
template<typename T> Program cos(const std::string & name = "result");
template<typename T> Program cosh(const std::string & name = "result");
template<typename T> Program dx(const std::string & name = "result");
template<typename T> Program dy(const std::string & name = "result");
template<typename T> Program exp(const std::string & name = "result");
template<typename T> Program expm1(const std::string & name = "result");
template<typename T> Program exp2(const std::string & name = "result");
template<typename T> Program exp10(const std::string & name = "result");
template<typename T> Program floor(const std::string & name = "result");
template<typename T> Program frac(const std::string & name = "result");
template<typename T> Program fwidth(const std::string & name = "result");
template<typename T> Program log(const std::string & name = "result");
template<typename T> Program logp1(const std::string & name = "result");
template<typename T> Program log2(const std::string & name = "result");
template<typename T> Program log10(const std::string & name = "result");
template<typename T> Program normalize(const std::string & name = "result");
template<typename T> Program logical_not(const std::string & name = "result");
template<typename T> Program pos(const std::string & name = "result");
template<typename T> Program rcp(const std::string & name = "result");
template<typename T> Program round(const std::string & name = "result");
template<typename T> Program rsqrt(const std::string & name = "result");
template<typename T> Program sat(const std::string & name = "result");
template<typename T> Program sign(const std::string & name = "result");
template<typename T> Program sin(const std::string & name = "result");
template<typename T> Program sinh(const std::string & name = "result");
template<typename T> Program sort(const std::string & name = "result");
template<typename T> Program sqrt(const std::string & name = "result");
template<typename T> Program tan(const std::string & name = "result");
template<typename T> Program tanh(const std::string & name = "result");
//@}

/**@{ \brief Nibbles for binary operators
 * Inputs: IN(0) T1 a
 *         IN(1) T2 b (by default T2 = T1)
 * Outputs: OUT(0) name  (type is T1 or T2, whichever has more components)
 */
#define NIBBLE_BINARY_OP_DECL(opname) \
  template<typename T1, typename T2> Program opname(const std::string & output_name = "result",\
      const std::string & input_name0 = "x", const std::string & input_name1 = "y"); \
  template<typename T1> Program opname(const std::string & output_name = "result",\
      const std::string & input_name0 = "x", const std::string & input_name1 = "y")

NIBBLE_BINARY_OP_DECL(add);
NIBBLE_BINARY_OP_DECL(logical_and);
NIBBLE_BINARY_OP_DECL(atan2);
NIBBLE_BINARY_OP_DECL(div);
NIBBLE_BINARY_OP_DECL(faceforward);
NIBBLE_BINARY_OP_DECL(max);
NIBBLE_BINARY_OP_DECL(min);
NIBBLE_BINARY_OP_DECL(mod);
NIBBLE_BINARY_OP_DECL(mul);
NIBBLE_BINARY_OP_DECL(logical_or);
NIBBLE_BINARY_OP_DECL(pow);
NIBBLE_BINARY_OP_DECL(reflect);
NIBBLE_BINARY_OP_DECL(seq);
NIBBLE_BINARY_OP_DECL(sge);
NIBBLE_BINARY_OP_DECL(sgt);
NIBBLE_BINARY_OP_DECL(sle);
NIBBLE_BINARY_OP_DECL(slt);
NIBBLE_BINARY_OP_DECL(sne);
NIBBLE_BINARY_OP_DECL(sub);
//@}

/**@{ \brief Nibbles for ternary operators
 * Inputs: IN(0) T1 a
 *         IN(1) T2 b (by default T2 = T1)
 *         IN(2) T3 c (by default T3 = T2)
 * Outputs: OUT(0) name  (type is T1, T2 or T3, whichever has more components)
 */
#define NIBBLE_TERNARY_OP_DECL(opname) \
  template<typename T1, typename T2, typename T3> Program opname(const std::string & output_name = "result",\
      const std::string & input_name0 = "x", const std::string & input_name1 = "y", \
      const std::string & input_name2 = "z"); \
  template<typename T1, typename T2> Program opname(const std::string & output_name = "result",\
      const std::string & input_name0 = "x", const std::string & input_name1 = "y", \
      const std::string & input_name2 = "z"); \
  template<typename T1> Program opname(const std::string & output_name = "result",\
      const std::string & input_name0 = "x", const std::string & input_name1 = "y", \
      const std::string & input_name2 = "z")

NIBBLE_TERNARY_OP_DECL(clamp);
NIBBLE_TERNARY_OP_DECL(cond);
NIBBLE_TERNARY_OP_DECL(mad);
NIBBLE_TERNARY_OP_DECL(smoothstep);
//@}

/**@{ \brief Nibbles for hash functions
 * Inputs: IN(0) T1 a
 * Outputs: OUT(0) name  (type is T2)
 */
#define NIBBLE_HASH_FUNC_DECL(opname) \
  template<typename T1, typename T2> Program opname(const std::string & output_name = "result",\
      const std::string & input_name0 = "x")

NIBBLE_HASH_FUNC_DECL(cellnoise);
NIBBLE_HASH_FUNC_DECL(scellnoise);
NIBBLE_HASH_FUNC_DECL(hash);
NIBBLE_HASH_FUNC_DECL(texhash);
//@}

/**@{ \brief Nibbles for noise functions
 * Inputs: IN(0) T1 a
 *         IN(1) T2 b (optional)
 * Outputs: OUT(0) name  (type is the last of T2 or T3)
 */
#define NIBBLE_NOISE_FUNC_DECL(opname) \
  template<typename T1, typename T2> Program opname(const std::string & output_name = "result",\
      const std::string & input_name0 = "x"); \
  template<typename T1, typename T2, typename T3> Program opname(const std::string & output_name = "result",\
      const std::string & input_name0 = "x", const std::string & input_name1 = "y")

NIBBLE_NOISE_FUNC_DECL(linnoise);
NIBBLE_NOISE_FUNC_DECL(noise);
NIBBLE_NOISE_FUNC_DECL(perlin);
NIBBLE_NOISE_FUNC_DECL(turbulence);
NIBBLE_NOISE_FUNC_DECL(slinnoise);
NIBBLE_NOISE_FUNC_DECL(snoise);
NIBBLE_NOISE_FUNC_DECL(sperlin);
NIBBLE_NOISE_FUNC_DECL(sturbulence);
//@}

/**@{ \brief Nibbles for distance functions
 * Inputs: IN(0) T1 a
 *         IN(1) T2 b
 * Outputs: OUT(0) Attrib1f name
 */
#define NIBBLE_DISTANCE_FUNC_DECL(opname) \
  template<typename T1, typename T2> Program opname(const std::string & output_name = "result",\
      const std::string & input_name0 = "x", const std::string & input_name1 = "y"); \
  template<typename T> Program opname(const std::string & output_name = "result",\
      const std::string & input_name0 = "x", const std::string & input_name1 = "y")

NIBBLE_DISTANCE_FUNC_DECL(distance);
NIBBLE_DISTANCE_FUNC_DECL(distance_1);
NIBBLE_DISTANCE_FUNC_DECL(distance_inf);
//@}

/**@{ \brief Nibbles for unary non-componentwise tuple operators
 * Inputs: IN(0) T1 a
 * Outputs: OUT(0) Attrib1f name
 */
#define NIBBLE_UNARY_TUPLEOP_DECL(opname) \
  template<typename T> Program opname(const std::string & output_name = "result",\
      const std::string & input_name0 = "x")

NIBBLE_UNARY_TUPLEOP_DECL(all);
NIBBLE_UNARY_TUPLEOP_DECL(any);
NIBBLE_UNARY_TUPLEOP_DECL(length);
NIBBLE_UNARY_TUPLEOP_DECL(length_1);
NIBBLE_UNARY_TUPLEOP_DECL(length_inf);
NIBBLE_UNARY_TUPLEOP_DECL(max1);
NIBBLE_UNARY_TUPLEOP_DECL(min1);
NIBBLE_UNARY_TUPLEOP_DECL(prod);
NIBBLE_UNARY_TUPLEOP_DECL(sum);
//@}

/** \brief Nibble for the bernstein<N> function
 * Inputs: IN(0) T a
 * Outputs: OUT(0) Generic<N, T>
 */
template<int N, typename T>
Program bernstein(const std::string& name = "result",
                      const std::string& input_name0 = "x");

/** \brief Nibble for the bezier function
 * Inputs: IN(0) T1 a
 *         IN(1) T2 b
 * Outputs: OUT(0) T2
 */
template<typename T1, typename T2>
Program bezier(const std::string& name = "result",
                   const std::string& input_name0 = "x",
                   const std::string& input_name1 = "y");

/** \brief Nibble for cross product operator
 * Inputs: IN(0) T a
 *         IN(1) T b
 * Outputs: OUT(0) T name  
 */
template<typename T> Program cross(const std::string & name = "result",
                                       const std::string & input_name0 = "x", 
                                       const std::string & input_name1 = "y");

/** \brief Nibble for discard
 * Inputs: IN(0) T a
 * Outputs: (none)
 */
template<typename T> Program discard(const std::string & name = "result",
                                         const std::string & input_name0 = "x");

/** \brief Nibble for dot product operator
 * Inputs: IN(0) T a
 *         IN(1) T b
 * Outputs: OUT(0) Attrib1f name  
 */
template<typename T> Program dot(const std::string & name = "result",
                                     const std::string & input_name0 = "x", 
                                     const std::string & input_name1 = "y");

/** \brief Nibble for the gradient function
 * Inputs: IN(0) T a
 * Outputs: OUT(0) Attrib1f name
 */
template<typename T> Program gradient(const std::string & name = "result",
                                          const std::string & input_name0 = "x");

/** \brief Nibble for the groupsort<S> function
 * Inputs: IN(0) T a
 * Outputs: (none)
 */
template<int S, typename T>
Program groupsort(const std::string & name = "result",
                      const std::string & input_name0 = "x");

/** \brief Nibble for the hermite function
 * Inputs: IN(0) T1 a
 *         IN(1) T2 b
 *         IN(2) T2 c
 *         IN(3) T2 d
 *         IN(4) T2 e
 * Outputs: OUT(0) T2
 */
template<typename T1, typename T2>
Program hermite(const std::string& name = "result",
                    const std::string& input_name0 = "x",
                    const std::string& input_name1 = "y",
                    const std::string& input_name2 = "z",
                    const std::string& input_name3 = "w",
                    const std::string& input_name4 = "v");

/** \brief Nibble for the join function
 * Inputs: IN(0) T1 a
 *         IN(1) T2 b
 * Outputs: OUT(0) T1+T2 name
 */
template<typename T1, typename T2> 
Program join(const std::string & name = "result",
                 const std::string & input_name0 = "x", 
                 const std::string & input_name1 = "y");

/** @{ \brief Nibble for lerp operator
 * Inputs: IN(0) T1 a
 *         IN(1) T1 b
 *         IN(2) T2 alpha (T2 = T1 by default)
 * Outputs: OUT(0) T1 name = a * alpha + b * (1 - alpha)  
 */
template<typename T1, typename T2>
Program lerp(const std::string & name = "result",
                 const std::string & input_name0 = "x",
                 const std::string & input_name1 = "y",
                 const std::string & input_name2 = "z");

template<typename T1>
Program lerp(const std::string & name = "result",
                 const std::string & input_name0 = "x",
                 const std::string & input_name1 = "y",
                 const std::string & input_name2 = "z");
//@}

/** \brief Nibble for the lit function
 * Inputs: IN(0) T a
 *         IN(1) T b
 *         IN(2) T c
 * Outputs: OUT(0) Attrib4f name  
 */
template<typename T> 
Program lit(const std::string& name = "result",
                const std::string& input_name0 = "x", 
                const std::string& input_name1 = "y",
                const std::string& input_name2 = "z");

/** @{ \brief Nibble for poly function
 * Inputs: IN(0) T1 a
 *         IN(1) T1 b
 *         IN(2) T2 alpha (T2 = T1 by default)
 * Outputs: OUT(0) T1 name
 */
template<typename T1, typename T2>
Program poly(const std::string & name = "result",
                 const std::string & input_name0 = "x", 
                 const std::string & input_name1 = "y");

template<typename T1>
Program poly(const std::string & name = "result",
                 const std::string & input_name0 = "x", 
                 const std::string & input_name1 = "y");
//@}

/** @{ \brief Nibble for the refract function
 * Inputs: IN(0) T1 a
 *         IN(1) T1 b
 *         IN(2) T2 c
 * Outputs: OUT(0) T1 name
 */
template<typename T1, typename T2>
Program refract(const std::string& name = "result",
                    const std::string& input_name0 = "x", 
                    const std::string& input_name1 = "y",
                    const std::string& input_name2 = "z");

template<typename T>
Program refract(const std::string& name = "result",
                    const std::string& input_name0 = "x", 
                    const std::string& input_name1 = "y",
                    const std::string& input_name2 = "z");
//@}

/** \brief Nibble for the smoothpulse function
 * Inputs: IN(0) T1 a
 *         IN(1) T1 b
 *         IN(2) T2 c
 * Outputs: OUT(0) T1 name
 */
template<typename T>
Program smoothpulse(const std::string& name = "result",
                        const std::string& input_name0 = "x", 
                        const std::string& input_name1 = "y",
                        const std::string& input_name2 = "z",
                        const std::string& input_name3 = "w");

} // namespace

#include "NibblesImpl.hpp"

#endif // SHNIBBLES_HPP
