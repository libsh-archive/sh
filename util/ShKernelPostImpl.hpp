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
#ifndef SHUTIL_KERNELPOSTIMPL_HPP 
#define SHUTIL_KERNELPOSTIMPL_HPP 

#include <sstream>
#include "ShSyntax.hpp"
#include "ShPosition.hpp"
#include "ShManipulator.hpp"
#include "ShAlgebra.hpp"
#include "ShProgram.hpp"
#include "ShNibbles.hpp"
#include "ShKernelPost.hpp"
#include "ShUtil.hpp"

/** \file ShKernelPostImpl.hpp
 * This is an implementation of useful postprocessing kernels 
 */

namespace ShUtil {

using namespace SH;

template<typename T>
ShProgram ShKernelPost::halftone(const ShBaseTexture2D<T> &tex) {
  ShProgram kernel = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputAttrib1f SH_NAMEDECL(scale, "halftoneScale");
    typename T::InputType SH_NAMEDECL(in, "result");
    ShInputPosition4f SH_DECL(posh);

    typename T::OutputType SH_NAMEDECL(out, "result");
    //out = in > tex(posh(0,1) * scale);
    out(0,1) = posh(0,1) * ShConstant1f(1.0f/400.0f); 
  } SH_END;
  return kernel;
}

}

#endif
