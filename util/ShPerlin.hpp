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
#ifndef SHDEMO_PERLIN_HPP 
#define SHDEMO_PERLIN_HPP 

#include "ShAttrib.hpp"
#include "ShTexture.hpp"

namespace ShUtil {

using namespace SH;

/** \file ShPerlin.hpp
 * This is an implementation of Perlin noise.
 */

/** \brief A Perlin noise/turbulence generator.
 */
class ShPerlin
{
  public:
    /** \brief Constructor for ShPerlin.
     * Constructs a Perlin noise generator that uses the given 
     * persistence and number of octaves for generating turbulence
     */
    ShPerlin(double persistence, int octaves);

    /** \brief Destructor for ShPerlin.
     * Destroys a Perlin noise generator.
     */
    ~ShPerlin();

    /** \brief Generates a single octave Perlin noise.
     */
    ShAttrib1f noise(ShAttrib3f p);

    /** \brief Turbulence functions sum several octaves of Perlin noise. 
     * The number of octaves and persistence can be controlled using 
     * setTurbulenceParams.
     * \sa void setTurbulenceParams(double persist, int numOctaves);
     */
    ShAttrib1f turbulence(ShAttrib3f p);
    ShAttrib1f sturbulence(ShAttrib3f p);

    /** \brief Toggles texture lookup Perlin method
     * The Perlin noise function can use either a procedural hash
     * function to generate gradients on the integer lattice or use
     * 3D texture lookup.
     *
     * This toggles whether to use the texture lookup method, which is 
     * currently much faster than the procedural method.
     */
    void useNoiseTexture(bool useNoiseTex);

    /** \brief Sets parameters used for turbulence.
     * Sets the persistence and number of octaves to use when
     * summing Perlin noise functions together for turbulence.
     *
     * The ith octave used in the sum has period
     * 2^i and is scaled by persist^i 
     */
    void setTurbulenceParams(double persist, int numOctaves);


  private:
    double persistence; ///< persistence to use for turbulence
    int octaves; ///< number of octaves to use for turbulence
    bool useTexture; ///< toggles whether to use texture lookup for gradients

    ShTexture3D<ShColor4f> noiseTex; ///< pseudorandom 3D perlin noise texture 

    ShAttrib4f grad4(ShAttrib3f p); 
    ShAttrib1f grad(ShAttrib3f p); 

};

}

#endif
