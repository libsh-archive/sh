// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Bryan Chan, Michael D. McCool
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
#ifndef SHUTIL_KERNELPOST_HPP 
#define SHUTIL_KERNELPOST_HPP 

#include <string>
#include "ShMatrix.hpp"
#include "ShTexture.hpp"
#include "ShProgram.hpp"

/** \file ShKernelPost.hpp
 * These are postprocessing kernels.
 *
 * They must take one input result of type T and return one output of type T named result.
 *
 * They may use any of the attributes from the general vertex shader EXCEPT the lighting ones
 * (you can use lighting attributes (halfVec, lightVec, lightPos), but only if you truly 
 * understand what you're getting into)
 */

namespace ShUtil {

using namespace SH;

class ShKernelPost {
  public:
    /** screen space Halftoning/Hatching in each color channel using tex as a threshold image
     * IN(0) ShAttrib1f scaling   - scaling on posh(0,1) before doing texture lookup
     * IN(1) T result 
     * IN(2) ShPosition4f posh    - homogeneous position (HDCS)
     *
     * OUT(0) T result            - output result 
     */
    template<typename T>
    static ShProgram halftone(const ShBaseTexture2D<T> &tex);
};

}

#include "ShKernelPostImpl.hpp"

#endif
