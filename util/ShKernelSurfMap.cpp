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
#include "ShKernelSurfMap.hpp"
#include "ShFunc.hpp"
#include "ShSyntax.hpp"
#include "ShNormal.hpp"
#include "ShLib.hpp"

namespace ShUtil {

ShProgram ShKernelSurfMap::bump() {
  ShProgram kernel = SH_BEGIN_PROGRAM() {
    ShInputAttrib2f SH_DECL(gradient);
    ShInOutNormal3f SH_DECL(normalt);

    normalt(1,2) += gradient;
  } SH_END;
  return kernel;
}

ShProgram ShKernelSurfMap::vcsBump() {
  ShProgram kernel = SH_BEGIN_PROGRAM() {
    ShInputAttrib2f SH_DECL(gradient);
    ShInOutNormal3f SH_DECL(normal);
    ShInputVector3f SH_DECL(tangent);
    ShInputVector3f SH_DECL(tangent2);

    normal = mad(gradient(0), tangent, normal);
    normal = mad(gradient(1), tangent2, normal);
  } SH_END;
  return kernel;
}

};
