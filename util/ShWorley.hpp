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
#ifndef SMASHTEST_WORLEY_HPP 
#define SMASHTEST_WORLEY_HPP

#include "ShAttrib.hpp"
#include "ShColor.hpp"
#include "ShTexture.hpp"

namespace ShUtil {

using namespace SH;

/** \file ShWorley.hpp
 * This is an implementation of 2D ShWorley texture.
 */

/** Kinds of metrics used for finding nearest neighbours.
 */
enum ShWorleyMetric {
  L1, ///< Sum of absolute componentwise differences
  L2, ///< Euclidean metric
  L2_SQ,  ///< Euclidean metric squared
  LINF, ///< Maximum absolute difference
};

/** A 2D Worley texture class.
 */
class ShWorley {
  public:
    /** \brief Constructor for ShWorley.
     * Creates a Worley texture with the given frequency.
     * (frequency x frequency cells are generated in the [0,1]x[0,1] square) 
     *
     * The texture should tile properly (perhaps...). 
     */  
    ShWorley( int frequency, bool useTexture ); 

    /** \brief Destructor for ShWorley.
     * Destroys a ShWorley.
     */
    ~ShWorley();

    /** \brief Toggles texture lookup for cell positions. 
     * The Worley texture function can use either a procedural hash
     * to generate positions in adjacent cells, or use several
     * texture lookups.
     *
     * This toggles whether to use the texture lookup method, which is 
     * currently much faster than the procedural method.
     */
    void useNoiseTexture( bool useNoiseTex );

    /** \brief Worley texture generator.
     * This finds the 4 nearest neighbours to the point p using metric m 
     * and returns the sum of the distances, weighted by c.
     */
    // TODO implement gradients
    // should be real easy, but need to makes odd-even transposition sort 
    // much messier (maybe try bubble sort?)
    ShAttrib3f worley( ShAttrib2f p, 
        ShAttrib4f c = ShAttrib4f( 1.0, 0.0, 0.0, 0.0 ), ShWorleyMetric m = L2_SQ ); 
    
    /** Makes a shader that takes 
     *  IN(0) ShAttrib4f coefficients; // worley coefficients
     *  IN(1) ShTexCoord2f texcoord; // texture lookup coordinates
     *
     *  OUT(0) ShAttrib1f scalar 
     *  OUT(1) ShAttrib2f gradient;
     */
    ShProgram worleyProgram( ShWorleyMetric m = L2_SQ );

  private:
    bool useTexture; ///< toggles whether to use texture lookup for points 
    int freq;  ///< frequency of the texture (number of cells in [0,1])
    double dfreq; ///< frequency of the texture

    /** Lookup for the point in the given cell
     */
    ShTexture2D<ShColor4f> cellPosTex; 

    /** Lookup for 8 points in cells adjacent to the given cell
     */
    ShTexture2D<ShColor4f>* adjTex[4]; 

    void doWorley( ShAttrib2f p, ShAttrib4f c, ShWorleyMetric m, ShAttrib1f &result, ShAttrib2f &gradientResult ); 
};

} // namespace ShUtil

#endif
