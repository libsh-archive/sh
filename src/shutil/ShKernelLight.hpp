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
#ifndef SHUTIL_KERNELLIGHT_HPP 
#define SHUTIL_KERNELLIGHT_HPP 

#include "sh/ShMatrix.hpp"
#include "sh/ShTexture.hpp"
#include "sh/ShProgram.hpp"

/** \file ShKernelLight.hpp
 * A set of light shaders
 * Light shaders can use any of the vertex shader outputs from ShKerneLib::vsh 
 * and must output one irrad representing the irradiance at a surface of type T (probably ShColor3f).
 */

namespace ShUtil {

using namespace SH;

class ShKernelLight {
  public:
    /** Omnidirectional light program
     * IN(0) T lightColor - color;
     *
     * OUT(0) T irrad - irradiance
     */
    template<typename T>
    static ShProgram pointLight();

    /** Spotlight program 
     * linear falloff from (lightVec | lightDir) == -1 to -cos(fallofAngle)
     *
     * Takes a gradient direction and applies 
     * IN(0) T lightColor - color;
     * IN(1) ShAttrib1f falloff  - angle in radians where spotlight intensity begins to go to 0 
     * IN(0) ShAttrib1f lightAngle - angle in radians where spotlight intensity = 0
     * IN(2) ShVector3f lightDir - light direction (VCS) 
     *
     * The following usually comes from shVsh
     * IN(3) ShPoint3f lightVec - light vector at surface point (VCS) 
     *
     * OUT(0) T irrad - irradiance
     */
    template<typename T>
    static ShProgram spotLight();

    /** 2D-Textured light program  
     *
     * Takes as input
     * IN(0) ShAttrib1f scaling - scaling on the texture (tiles texture)
     * IN(1) ShAttrib1f lightAngle - angle in radians for fov of light 
     * IN(2) ShVector3f lightDir - direction light faces (VCS)
     * IN(3) ShVector3f lightUp - up direction of light, must be orthogonal to lightDir (VCS)
     *
     * The following typically come from shVsh 
     * IN(3) ShVector3f lightVec - light vector at surface point (VCS) 
     *
     * OUT(0) T irrad - irradiance
     */
    template<typename T>
    static ShProgram texLight2D(const ShBaseTexture2D<T> &tex);
};

}

#include "ShKernelLightImpl.hpp"

#endif
