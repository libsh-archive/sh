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
#include "sh/ShSyntax.hpp"
#include "sh/ShNormal.hpp"
#include "sh/ShLib.hpp"
#include "ShKernelSurfMap.hpp"
#include "ShFunc.hpp"

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
