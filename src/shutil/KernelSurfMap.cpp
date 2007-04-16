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
#include "sh/Syntax.hpp"
#include "sh/Normal.hpp"
#include "sh/Lib.hpp"
#include "KernelSurfMap.hpp"
#include "Func.hpp"

namespace ShUtil {

Program KernelSurfMap::bump() {
  Program kernel = SH_BEGIN_PROGRAM() {
    InputAttrib2f SH_DECL(gradient);
    InOutNormal3f SH_DECL(normalt);

    normalt(1,2) += gradient;
  } SH_END;
  return kernel;
}

Program KernelSurfMap::vcsBump() {
  Program kernel = SH_BEGIN_PROGRAM() {
    InputAttrib2f SH_DECL(gradient);
    InOutNormal3f SH_DECL(normal);
    InputVector3f SH_DECL(tangent);
    InputVector3f SH_DECL(tangent2);

    normal = mad(gradient(0), tangent, normal);
    normal = mad(gradient(1), tangent2, normal);
  } SH_END;
  return kernel;
}

};
