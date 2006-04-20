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
#ifndef SHUTIL_KERNELSURFMAP_HPP 
#define SHUTIL_KERNELSURFMAP_HPP 

#include "sh/Program.hpp"

/** \file KernelSurfMap.hpp
 * 
 */

namespace ShUtil {

using namespace SH;

class KernelSurfMap {
  public:
    /** Bump program
     * Takes a gradient direction and applies 
     * IN(0) Attrib2f gradient  - gradient
     * IN(1) Normal3f normalt    - normalized normal vector (tangent space) 
     *
     * OUT(0) Normal3f normalt   - perturbed normal (tangent space)
     */
    static Program bump();

    /** VCS Bump program
     * Takes a gradient direction and applies 
     * IN(0) Attrib2f gradient  - gradient
     * IN(1) Normal3f normal    - normalized normal vector (VCS)
     * IN(2) Vector3f tangent   - normalized tangent vector (VCS)
     * IN(3) Vector3f tangent2  - normalized secondary tangent (VCS)
     *
     * OUT(0) Normal3f normal   - perturbed normal (VCS)
     */
    static Program vcsBump();
};

}

#endif
