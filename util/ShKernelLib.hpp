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
#ifndef SHUTIL_KERNELLIB_HPP 
#define SHUTIL_KERNELLIB_HPP 

#include "ShLib.hpp"
#include "ShMatrix.hpp"
#include "ShTexture.hpp"
#include "ShProgram.hpp"

/** \file ShKernelLib.hpp
 * This is an implementation of useful kernels and nibbles (simple kernels).
 *
 * Much of this is bits and pieces gathered from smashtest & shdemo
 */

namespace ShUtil {

using namespace SH;

/* Shaders
 * Mix - color/scalar inputs, add colors
 * Bump - input - normal, 2 tangents, perturb, output - normal
 *
 */
class ShKernelLib {
  public:
/// Nibblets - very simple programs encapsulating single operations/types
    /* transformation nibble (transforms ShAttrib of type T using matrix
     * of type ShMatrix<Rows, Cols, Kind, MT> 
     * Inputs: IN(0) attrib
     * Outputs: OUT(0) T transformedAttrib = m | attrib
     */
    template<int Rows, int Cols, int Kind, typename T>
    static ShProgram transformNibble( const ShMatrix<Rows, Cols, Kind, T> &m ); 

    /* 2D texture lookup nibble 
     * Inputs: IN(0) ShTexCoord2f tc
     * Outputs: OUT(0) T texLookupResult = tex(tc) */ 
    template<typename T> 
    static ShProgram textureNibble( const ShTexture2D<T> &tex );

    /* lerping nibble 
     * Inputs: IN(0) ShAttrib1f alpha
     *         IN(1) T a
     *         IN(2) T b
     * Outputs: OUT(0) T result = lerp( alpha, a, b )*/
    template<typename T>
    static ShProgram lerpNibble(); 

    template<typename T>
    static ShProgram dotNibble(); 

    template<typename T>
    static ShProgram addNibble(); 

    /* casts to the given type either by padding with 0 channels
     * or truncating channels
     */
    template<typename T, typename T2>
    static ShProgram castNibble();


/// Kernels - some commonly used programs
    /** Bump program
     * Takes a gradient direction and applies 
     * IN(0) ShAttrib2f       - gradient
     * IN(1) ShVector3f       - normalized normal vector (tangent space) 
     */
    static ShProgram bump();

    /** Diffuse fragment program 
     * IN(0) T                - diffuse coefficient (kd) could be ShColor?f
     * IN(1) ShNormal3f       - normal (xCS)
     * IN(2) ShVector3f       - light vector (xCS)
     * IN(3) ShPosition4f     - positino (HDCS)
     *
     * OUT(0) T               - output colour
     *
     * xCS is usually either VCS or tangent space 
     */
    template<typename T>
    static ShProgram diffuse();

    /** Specular fragment program 
     * IN(0) T                - specular coefficient (ks) could be ShColor?f
     * IN(1) ShAttrib1f       - specular exponent 
     * IN(2) ShNormal3f       - normal (xCS)
     * IN(3) ShVector3f       - view vector (xCS)
     * IN(4) ShVector3f       - half vector (xCS)
     * IN(5) ShVector3f       - light vector (xCS)
     * IN(6) ShPosition4f     - positino (HDCS)
     *
     * OUT(0) T               - output colour 
     *
     * xCS is usually either VCS or tangent space 
     */
    template<typename T>
    static ShProgram specular();

    /** Phong fragment program 
     * IN(0) T                - diffuse coefficient (kd) could be ShColor?f
     * IN(1) T                - specular coefficient (ks) could be ShColor?f
     * IN(2) ShAttrib1f       - specular exponent
     * IN(3) ShNormal3f       - normal (xCS)
     * IN(4) ShVector3f       - view vector (xCS)
     * IN(5) ShVector3f       - half vector (xCS)
     * IN(6) ShVector3f       - light vector (xCS)
     * IN(7) ShPosition4f     - positino (HDCS)
     *
     * OUT(0) T               - output colour 
     *
     * xCS is usually either VCS or tangent space 
     */
    template<typename T>
    static ShProgram phong();


/// Cobs - Massive, general programs designed to be specialized  
    /** Generalized vertex program that computes lots of different outputs
     * Inputs:
     *  IN(0) ShTexCoord2f    - texture coordinate
     *  IN(1) ShNormal3f      - normal vector (MCS)
     *  IN(2) ShVector3f      - tangent (MCS)
     *  IN(3) ShVector3f      - secondary tangent (MCS)
     *  IN(4) ShPoint3f       - light position (VCS)
     *  IN(5) ShPosition4f    - position (MCS)
     *
     *  OUT(0) ShTexCoord2f   - texture coordinate
     *  OUT(1) ShPoint3f      - output point (VCS)
     *  OUT(2) ShNormal3f     - normal vector (VCS) 
     *  OUT(3) ShVector3f     - view vector (VCS)
     *  OUT(4) ShVector3f     - half vector (VCS)
     *  OUT(5) ShVector3f     - light vector (VCS)
     *  OUT(6) ShVector3f     - view vector (tangent coordinate space)
     *  OUT(7) ShVector3f     - half vector (tangent coordinate space)
     *  OUT(8) ShVector3f     - light vector (tangent coordinate space)
     *  OUT(9) ShPosition4f  - position (HDCS)
     *
     *  Input specializations
     *  - remove inputs (1,2) if outputs (5-7) not needed
     *  - remove texcoord (it's just here for convenience) 
     */
    template<int N, int Kind, typename T>
    static ShProgram vsh(const ShMatrix<N, N, Kind, T> &mv, const ShMatrix<N, N, Kind, T> &mvp ); 
};

}

#include "ShKernelLibImpl.hpp"

#endif
