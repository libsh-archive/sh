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
#ifndef SHUTIL_KERNELSURFACE_HPP 
#define SHUTIL_KERNELSURFACE_HPP 

#include <string>
#include "sh/Matrix.hpp"
#include "sh/Texture.hpp"
#include "sh/Program.hpp"

/** \file KernelSurface.hpp
 * These are the surface shader kernels.
 * They must take as input an irrad of type T, which is the irradiance from a single light source
 * and should use only the globals "halfVec", "lightVec", "lightPos", "halfVect", or "lightVect". 
 *
 * Output must be a single variable of type T named result (Most likely a Color3f or 4f)
 */

namespace ShUtil {

using namespace SH;

class KernelSurface {
  private:
    // returns the string prefix concatenated with index
    static std::string makeName(std::string prefix, int index); 

  public:
    /** Diffuse fragment program 
     * IN(0) T kd                 - diffuse coefficient (kd) could be Color?f
     * IN(1) T irrad              - irradiance from the light source
     *
     * IN(1) Normal3f normal    - normal (xCS)
     * IN(2) Vector3f lightVec  - light vector (xCS)
     * IN(3) Position4f posh    - positino (HDCS)
     *
     * OUT(0) T result            - output result 
     *
     * xCS is usually either VCS or tangent space 
     */
    template<typename T>
    static Program diffuse();

    /** Specular fragment program 
     * IN(0) T ks                 - specular coefficient (ks) could be Color?f
     * IN(1) Attrib1f specExp   - specular exponent 
     * IN(1) T irrad              - irradiance from the light source
     *
     * IN(2) Normal3f normal    - normal (xCS)
     * IN(3) Vector3f halfVec   - half vector (xCS)
     * IN(4) Vector3f lightVec  - light vector (xCS)
     * IN(5) Position4f posh    - positino (HDCS)
     *
     * OUT(0) T result            - output result 
     *
     * xCS is usually either VCS or tangent space 
     */
    template<typename T>
    static Program specular();

    /** Phong fragment program 
     * IN(0) T kd                 - diffuse coefficient (kd) could be Color?f
     * IN(1) T ks                 - specular coefficient (ks) could be Color?f
     * IN(2) Attrib1f specExp   - specular exponent
     * IN(1) T irrad              - irradiance from the light source
     *
     * IN(3) Normal3f normal    - normal (xCS)
     * IN(4) Vector3f halfVec   - half vector (xCS)
     * IN(5) Vector3f lightVec  - light vector (xCS)
     * IN(6) Position4f posh    - position (HDCS)
     *
     * OUT(0) T result            - output colour 
     *
     * xCS is usually either VCS or tangent space 
     */
    template<typename T>
    static Program phong();

    /** Gooch illustrative shading 
     * IN(0) T kd                 - diffuse coefficient (kd) could be Color?f
     * IN(1) T cool               - cool multiplier (when light | normal == -1) 
     * IN(2) T warm               - warm multiplier (when light | normal == 1)
     *
     * IN(1) T irrad              - irradiance from the light source
     * IN(3) Normal3f normal    - normal (xCS)
     * IN(5) Vector3f lightVec  - light vector (xCS)
     * IN(6) Position4f posh    - position (HDCS)
     *
     * OUT(0) T result            - output colour 
     */
    template<typename T>
    static Program gooch();

    /** null surface
     * IN(0) T irrad
     * IN(1) Position4f posh
     *
     * OUT(0) T result = irrad
     */
    template<typename T>
    static Program null();
};

}

#include "KernelSurfaceImpl.hpp"

#endif
