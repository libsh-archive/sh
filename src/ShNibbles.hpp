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
#ifndef SHNIBBLES_HPP
#define SHNIBBLES_HPP

/** \file ShNibbles.hpp
 * A set of nibbles that are functions that return ShPrograms for
 * basic operations.
 */

namespace SH {

/** \brief Creates a keep nibble.
 * The keep nibble passes through one input of type T named with
 * the given name.
 */
template<typename T>
ShProgram keep(std::string name = ""); 

/** \brief Creates a lose nibble.
 * The lose nibble discards through one input of type T named with
 * the given name.
 */
template<typename T>
ShProgram lose(std::string name = "");

/** \brief 2D texture lookup nibble 
 * Inputs: IN(0) ShTexCoord1f tc
 * Outputs: OUT(0) name = tex(tc) */ 
template<typename T> 
ShProgram access(const ShTexture1D<T> &tex, std::string name = "result");

/** \brief 2D texture lookup nibble 
 * Inputs: IN(0) ShTexCoord2f tc
 * Outputs: OUT(0) name = tex(tc) */ 
template<typename T> 
ShProgram access(const ShTexture2D<T> &tex, std::string name = "result");

/** \brief 2D texture lookup nibble 
 * Inputs: IN(0) ShTexCoord3f tc
 * Outputs: OUT(0) name = tex(tc) */ 
template<typename T> 
ShProgram access(const ShTexture3D<T> &tex, std::string name = "result");

/** \brief transformation nibble 
 * Creates a nibble that transforms a variable of type T2 by 
 * a matrix of type ShMatrix<Rows, Cols, Kind, T>
 * Inputs: IN(0) attrib
 * Outputs: OUT(0) T2 name = m | attrib
 */
template<typename T2, int Rows, int Cols, int Kind, typename T>
ShProgram transform(const ShMatrix<Rows, Cols, Kind, T> &m, std::string name = "result");

/** \brief Casting nibble 
 * Casts from tuple type T to type T2
 * If T::typesize < T2::typesize, pads with 0 components at end.
 * If T::typesize > T2::typesize, truncates components at end. 
 * Inputs: IN(0) T in
 * Outputs: OUT(0) T2 name 
 *
 */
template<typename T, typename T2>
ShProgram cast(std::string name = "result");

/**@{ \brief Nibbles for unary operators
 * Inputs: IN(0) T name 
 * Outputs: OUT(0) T name 
 */
template<typename T> ShProgram abs(std::string name = "result");
template<typename T> ShProgram acos(std::string name = "result");
template<typename T> ShProgram asin(std::string name = "result");
template<typename T> ShProgram cos(std::string name = "result");
template<typename T> ShProgram frac(std::string name = "result");
template<typename T> ShProgram sin(std::string name = "result");
template<typename T> ShProgram sqrt(std::string name = "result");
template<typename T> ShProgram normalize(std::string name = "result");
template<typename T> ShProgram pos(std::string name = "result");
//@}

/**@{ \brief Nibbles for binary operators
 * Inputs: IN(0) T a
 *         IN(1) T b
 * Outputs: OUT(0) T name 
 */
template<typename T> ShProgram add(std::string name = "result");
template<typename T> ShProgram mul(std::string name = "result");
template<typename T> ShProgram div(std::string name = "result");
template<typename T> ShProgram pow(std::string name = "result");
template<typename T> ShProgram slt(std::string name = "result");
template<typename T> ShProgram sle(std::string name = "result");
template<typename T> ShProgram sgt(std::string name = "result");
template<typename T> ShProgram sge(std::string name = "result");
template<typename T> ShProgram seq(std::string name = "result");
template<typename T> ShProgram sne(std::string name = "result");
template<typename T> ShProgram fmod(std::string name = "result");
template<typename T> ShProgram min(std::string name = "result");
template<typename T> ShProgram max(std::string name = "result");

/** @{ \brief Nibble for dot product operator
 * Inputs: IN(0) T a
 *         IN(1) T b
 * Outputs: OUT(0) ShAttrib1f name  
 */
template<typename T> ShProgram dot(std::string name = "result");

/** @{ \brief Nibble for lerp operator
 * Inputs: IN(0) T a
 *         IN(1) T b
 *         IN(2) T alpha
 * Outputs: OUT(0) T name = a * alpha + b * (1 - alpha) 
 */
template<typename T> ShProgram lerp(std::string name = "result");





}


#include "ShNibblesImpl.hpp"

#endif
