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
#include "ShLib.hpp" // ShLibWorley needs to be included from within ShLib

#ifndef SHLIBWORLEY_HPP 
#define SHLIBWORLEY_HPP

#include <vector>
#include "ShAttrib.hpp"
#include "ShColor.hpp"

namespace SH {

/** \file ShLibWorley.hpp
 * This is an implementation of 2D ShWorley texture.  A Worley
 * texture is made by distributing a set of points in a space and
 * taking a weighted sum of basis functions for the k-nearest 
 * neighbours of a lookup point p.  These basis functions can 
 * be the distance itself from p to the adjacent generators (the
 * typical Worley function), or any other information.  For example,
 * we can attach colour information or refraction indicies to the generators.
 *
 * This Worley implementation is split into two parts.
 * The first part is a Generator Factory that produces
 * a fixed number of candidate generators around a lookup point p.
 *
 * A Property Factory is attaches properties to each Generator.
 * Currently, there is no internal Sh representation for a struct of
 * Sh types, so properties are generated into a long N-tuple.
 * The generators are sorted by the first component in the N-tuple, 
 * so this should typically be a distance.
 *
 * In the future, when it's possible to "call" an ShProgram object
 * directly, Property Factories should actually be ShProgram objects
 * that take a integer cell position as an input and output 
 * the desired properties. 
 *
 * The worley function itself sorts the generators based on
 * the first component in its properties and returns a N-tuple
 * containing the weighted sum of all properties.
 */

// A Generator point represents the position of a generator relative
// to the cell origin of a lookup position. 
//
template<int D, typename T>
struct Generator {
  Generator() {}
  ShAttrib<D, SH_TEMP, T> pos; // position 
  ShAttrib<D, SH_TEMP, T> offset; // offset of the cell relative to lookup point's cell
  ShAttrib<D, SH_TEMP, T> cell; // integer cell (this should actually be typed T = int, but not possible yet) 
};

// Generator Factory 
// A Point Generator must implement a function that 
// sets a Generator array with P candidate generator points. 
template<int P, int D, typename T>
struct GeneratorFactory {
  static const int NUM_POINTS = P;
  virtual ~GeneratorFactory() {}
  virtual void operator()(const ShGeneric<D, T> &p, Generator<D, T> result[]) const = 0;
};

// Property Factory
// This takes a Generator and attaches N properties to it.
// The first property in the result is used to sort the generator points 
// (and so should probably be a distance metric).
template<int N, int D, typename T>
struct PropertyFactory {
  static const int NUM_PROPS = N;
  static const int DIM = D;
  typedef T PropType; 
  
  virtual ~PropertyFactory() {} 
  virtual ShGeneric<N, T> operator()(const ShGeneric<D, T> &p, const Generator<D, T> &g) const = 0; 
};

#ifndef WIN32

/** \brief Worley texture generator.
 * This uses the DefaultGeneratorFactory and DistSqPropFactory 
 * TODO allow arbitrary distance function
 * @{
 */
template<int K, int D, typename T>
ShGeneric<K, T> worley(const ShGeneric<D, T> &p, bool useTexture = true); 
//@}

/** \brief Worley texture generator.
 * This uses a GeneratorFactory and PropertyFactory of your choice.
 */
template<int K, int L, int P, int D, typename T>
void worley(ShGeneric<K, T> result[], const ShGeneric<D, T> &p, 
            const GeneratorFactory<P, D, T> *genFactory,
            const PropertyFactory<L, D, T> *propFactory);

/** Makes a shader that takes 
 *  IN(1) ShTexCoord<D,T> texcoord; // texture lookup coordinates
 *
 * TODO make Output a struct of some kind when Sh supports structs
 *  OUT(0) ShAttrib<K, T> result[N]; // properties of k-nearest neighbours 
 * @{
 */
template<int K, int D, typename T>
ShProgram shWorley(bool useTexture);

template<int K, int N, int P, int D, typename T>
ShProgram shWorley(const GeneratorFactory<P, D, T> *genFactory,
                   const PropertyFactory<N, D, T> *propFactory);
//@}

#endif // WIN32

} // namespace SH

#include "ShLibWorleyImpl.hpp"

#endif // SHLIBWORLEY_HPP
