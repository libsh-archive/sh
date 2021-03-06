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
#ifndef SHUTIL_KERNELLIB_HPP 
#define SHUTIL_KERNELLIB_HPP 

#include <string>
#include "sh/Lib.hpp"
#include "sh/Matrix.hpp"
#include "sh/Texture.hpp"
#include "sh/Program.hpp"

/** \file KernelLib.hpp
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
 *
 * Note that in the comments for each kernel, there's a list of input/output attributes
 * with their string names and positional ordering. Negative positions denote
 * position from the end (-1 means last attribute, -2 means second last, etc.)
 */

namespace ShUtil {

using namespace SH;

class KernelLib {
  private:
    // returns the string prefix concatenated with index
    static std::string makeName(std::string prefix, int index); 

  public:
/// Kernels - some commonly used programs
    /** Generates a passthrough program using the outputs of a given Program
     * and keeps all the names.
     *
     * Useful in a cases where vsh outputs need to be duplicated before
     * being passed to fsh 
     */
    static Program outputPass( const Program &p );

    /** Generates a passthrough program using the outputs of a given Program
     * and keeps all the names.
     *
     * Useful in a cases where vsh outputs need to be duplicated before
     * being passed to fsh 
     */
    static Program inputPass( const Program &p );

    /** Basis Conversion program 
     * Takes 3 vectors for an orthonormal basis and converts the fourth 
     * vector.
     *
     * Hardcoded for 3f right now
     * IN(0,1,2) Vector3f b0Name, b1Name, b2Name 
     * IN(3) Vector3f name;
     *
     * OUT(0) Vector3f name;
     */
    static Program change_basis(std::string name="vec", 
        std::string b0Name="b0", std::string b1Name="b1", std::string b2Name="b2"); 

/// Cobs - Massive, general programs designed to be specialized  
    // TODO make a version of this with names like RenderMan globals
    // make a version with names like Houdini globals

    /** Generalized vertex program that computes *lots* of different outputs 
     * If numTangents = 0, then tangent, tangent 2 are not included in the inputs and any TCS outputs are invalid
     * If numTangents = 1, then only tangent is an input, tangent2 is computed from normal and tangent.  All TCS outputs are valid
     * If numTangent > 2, then both tangent and tangent2 are inputs.  
     *
     * (using new orthonormal bases {normal, tangent, tangent2} at each given point
     *  IN(0) TexCoord2f texcoord   - texture coordinate
     *  IN(1) Normal3f normal       - normal vector (MCS)
     *  Vector3f tangent            - primary tangent (MCS) (only included if numTangents > 0)
     *  Vector3f tangent2           - secondary tangent (MCS) (only included if numTangents > 1) 
     *  Point3f lightPosi           - light position (VCS), for i = 0..numLights - 1 
     *  IN(-1) Position4f posm       - position (MCS)
     *
     *  OUT(0) TexCoord2f texcoord   - texture coordinate
     *  OUT(1) Point3f posv          - output point (VCS)
     *  OUT(2) Point4f posm          - position (MCS)
     *
     *  Normal3f normal       - normal vector (VCS) 
     *  Vector3f tangent      - primary tangent (VCS) (only valid if numTangents > 0)
     *  Vector3f tangent2     - secondary tangent (VCS) (only valid if numTangents > 0) 
     *  Vector3f viewVec      - view vector (VCS)
     *  Vector3f halfVec       - half Vector (VCS) = halfVec0
     *  Vector3f halfVeci      - half Vector (VCS), for i = 0..numLights - 1 
     *  Vector3f lightVec      - light Vector (VCS) = lightVec0 
     *  Vector3f lightVeci     - light Vecor (VCS), for i = 0..numLights - 1 
     *  Vector3f lightPos      - light position (VCS) = lightPos0 
     *  Point3f lightPosi      - light position (VCS), for i = 0..numLights -1 
     *
     *  Normal3f normalt       - normal vector (TCS) 
     *  Vector3f viewVec      - view vector (TCS)
     *  Vector3f halfVect      - half Vector (TCS) = halfVect0
     *  Vector3f halfVecti      - half Vector (TCS), for i = 0..numLights - 1 
     *  Vector3f lightVect      - light Vector (TCS) = lightVect0 
     *  Vector3f lightVecti     - light Vector (TCS), for i  0..numLights - 1 
     *
     *  OUT(-1) Position4f posh       - position (HDCS)
     */
    template<int N, BindingType Binding, typename T>
    static Program vsh(const Matrix<N, N, Binding, T> &mv,
                           const Matrix<N, N, Binding, T> &mvp,
                           int numTangents = 0, int numLights = 1); 
};

}

#include "KernelLibImpl.hpp"

#endif
