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
 *
 * Fundamental design decisions that makes these easy to use:
 *  -commonly curried inputs are always at the begining to reduce need for extract
 *  -all inputs/outputs are named parameters (allows manipulate by name)
 *  -names for inputs/outputs that should be joined together on connect have the same
 *  name and are usually in the same order (allows either connect by position or by 
 *  name without too many manipulators)
 */

namespace ShUtil {

using namespace SH;

/// Nibbles - very simple programs encapsulating single operations/types
/* transformation nibble (transforms ShAttrib of type T using matrix
 * of type ShMatrix<Rows, Cols, Kind, MT> 
 * Inputs: IN(0) attrib 
 * Outputs: OUT(0) T transAttrib = m | attrib
 */
template<int Rows, int Cols, int Kind, typename T>
static ShProgram shTransform( const ShMatrix<Rows, Cols, Kind, T> &m ); 

/** Scales given type by a constant factor
 * IN(0) ShAttrib1f scale
 * IN(1) T attrib
 * OUT(1) T attrib
 */
template<typename T>
static ShProgram shScale(); 

/* 2D texture lookup nibble 
 * Inputs: IN(0) ShTexCoord1f tc
 * Outputs: OUT(0) result = tex(tc) */ 
template<typename T> 
static ShProgram access( const ShTexture1D<T> &tex );

/* 2D texture lookup nibble 
 * Inputs: IN(0) ShTexCoord2f tc
 * Outputs: OUT(0) result = tex(tc) */ 
template<typename T> 
static ShProgram access( const ShTexture2D<T> &tex );

/* 2D texture lookup nibble 
 * Inputs: IN(0) ShTexCoord3f tc
 * Outputs: OUT(0) result = tex(tc) */ 
template<typename T> 
static ShProgram access( const ShTexture3D<T> &tex );

/* lerping nibble 
 * Inputs: IN(0) ShAttrib1f alpha
 *         IN(1) T a
 *         IN(2) T b
 * Outputs: OUT(0) T result = lerp( alpha, a, b )*/
template<typename T>
static ShProgram shLerp(); 

/* dot product nibble 
 * Inputs: IN(0) T a
 *         IN(1) T b
 * Outputs: OUT(0) T result = dot( a, b ) */ 
template<typename T>
static ShProgram shDot(); 

/* sum nibble 
 * Inputs: IN(0) T a
 *         IN(1) T b
 * Outputs: OUT(0) T result = a + b */ 
template<typename T>
static ShProgram shAdd(); 

/* casts to the given type either by padding with 0 channels
 * or truncating channels
 */
template<typename T, typename T2>
static ShProgram shCast();

class ShKernelLib {
  public:
/// Kernels - some commonly used programs
    /** Generates a passthrough program using the outputs of a given ShProgram
     * and keeps all the names.
     *
     * Useful in a cases where vsh outputs need to be duplicated before
     * being passed to fsh 
     */
    static ShProgram outputPass( const ShProgram &p );

    /** Bump program
     * Takes a gradient direction and applies 
     * IN(0) ShAttrib2f gradient  - gradient
     * IN(1) ShNormal3f normal    - normalized normal vector (tangent space) 
     *
     * OUT(0) ShNormal3f normal   - perturbed normal (tangent space)
     */
    static ShProgram bump();

    /** Diffuse fragment program 
     * IN(0) T kd                 - diffuse coefficient (kd) could be ShColor?f
     * IN(1) ShNormal3f normal    - normal (xCS)
     * IN(2) ShVector3f lightVec  - light vector (xCS)
     * IN(3) ShPosition4f posh    - positino (HDCS)
     *
     * OUT(0) T result            - output result 
     *
     * xCS is usually either VCS or tangent space 
     */
    template<typename T>
    static ShProgram shDiffuse();

    /** Specular fragment program 
     * IN(0) T ks                 - specular coefficient (ks) could be ShColor?f
     * IN(1) ShAttrib1f specExp   - specular exponent 
     * IN(2) ShNormal3f normal    - normal (xCS)
     * IN(3) ShVector3f halfVec   - half vector (xCS)
     * IN(4) ShVector3f lightVec  - light vector (xCS)
     * IN(5) ShPosition4f posh    - positino (HDCS)
     *
     * OUT(0) T result            - output result 
     *
     * xCS is usually either VCS or tangent space 
     */
    template<typename T>
    static ShProgram shSpecular();

    /** Phong fragment program 
     * IN(0) T kd                 - diffuse coefficient (kd) could be ShColor?f
     * IN(1) T ks                 - specular coefficient (ks) could be ShColor?f
     * IN(2) ShAttrib1f specExp   - specular exponent
     * IN(3) ShNormal3f normal    - normal (xCS)
     * IN(4) ShVector3f halfVec   - half vector (xCS)
     * IN(5) ShVector3f lightVec  - light vector (xCS)
     * IN(6) ShPosition4f posh    - positino (HDCS)
     *
     * OUT(0) T result            - output colour 
     *
     * xCS is usually either VCS or tangent space 
     */
    template<typename T>
    static ShProgram shPhong();

/// Cobs - Massive, general programs designed to be specialized  
    /** Generalized vertex program that computes lots of different outputs
     * Inputs:
     *  IN(0) ShTexCoord2f texcoord   - texture coordinate
     *  IN(1) ShNormal3f normal       - normal vector (MCS)
     *  IN(2) ShPoint3f lightPos      - light position (VCS)
     *  IN(3) ShPosition4f posm       - position (MCS)
     *
     *  OUT(0) ShTexCoord2f texcoord   - texture coordinate
     *  OUT(1) ShPoint3f posv          - output point (VCS)
     *  OUT(2) ShNormal3f normal       - normal vector (VCS) 
     *  OUT(4) ShVector3f halfVec      - half vector (VCS)
     *  OUT(5) ShVector3f lightVec     - light vector (VCS)
     *  OUT(3) ShVector3f viewVec      - view vector (VCS)
     *  OUT(6) ShPosition4f posh       - position (HDCS)
     *
     *  Input specializations
     *  - remove inputs (1,2) if outputs (5-7) not needed
     *  - remove texcoord (it's just here for convenience) 
     */
    template<int N, int Kind, typename T>
    static ShProgram shVsh(const ShMatrix<N, N, Kind, T> &mv, const ShMatrix<N, N, Kind, T> &mvp ); 

    /** Uses shVsh to build a more advanced vertex program 
     * that transform normal, view, half, and lightVec to tangent coordinate space
     * (using new orthonormal bases {normal, tangent, tangent2} at each given point
     *  IN(0) ShTexCoord2f texcoord   - texture coordinate
     *  IN(1) ShNormal3f normal       - normal vector (MCS)
     *  IN(2) ShVector3f tangent      - primary tangent (MCS)
     *  IN(3) ShVector3f tangent2     - secondary tangent (MCS)
     *  IN(4) ShPoint3f lightPos      - light position (VCS)
     *  IN(5) ShPosition4f posm       - position (MCS)
     *
     *  OUT(0) ShTexCoord2f texcoord   - texture coordinate
     *  OUT(1) ShPoint3f posv          - output point (VCS)
     *  OUT(2) ShNormal3f normal       - normal vector (TCS) 
     *  OUT(3) ShVector3f viewVec      - view vector (TCS)
     *  OUT(4) ShVector3f halfVec      - half vector (TCS)
     *  OUT(5) ShVector3f lightVec     - light vector (TCS)
     *  OUT(6) ShPosition4f posh       - position (HDCS)
     */
    template<int N, int Kind, typename T>
    static ShProgram shVshTangentSpace( const ShMatrix<N, N, Kind, T> &mv, const ShMatrix<N, N, Kind, T> &mvp ); 
};

}

#include "ShKernelLibImpl.hpp"

#endif
