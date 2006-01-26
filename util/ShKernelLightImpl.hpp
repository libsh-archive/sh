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
#ifndef SHUTIL_KERNELLIGHTIMPL_HPP 
#define SHUTIL_KERNELLIGHTIMPL_HPP 

#include <sstream>
#include "sh/ShSyntax.hpp"
#include "sh/ShPosition.hpp"
#include "sh/ShManipulator.hpp"
#include "sh/ShAlgebra.hpp"
#include "sh/ShProgram.hpp"
#include "sh/ShNibbles.hpp"
#include "ShKernelLight.hpp"

/** \file ShKernelLightImpl.hpp
 * This is an implementation of useful kernels and nibbles (simple kernels).
 */

namespace ShUtil {

using namespace SH;

template <typename T>
ShProgram ShKernelLight::pointLight() {
  ShProgram kernel =  SH_BEGIN_PROGRAM() {
    typename T::InputType SH_DECL(lightColor);
    typename T::OutputType SH_DECL(irrad) = lightColor;
  } SH_END;
  return kernel;
}

template<typename T>
ShProgram ShKernelLight::spotLight() {
  ShProgram kernel =  SH_BEGIN_PROGRAM() {
    typename T::InputType SH_DECL(lightColor);
    ShInputAttrib1f SH_DECL(falloff);
    ShInputAttrib1f SH_DECL(lightAngle);
    ShInputVector3f SH_DECL(lightDir);
    ShInputVector3f SH_DECL(lightVec);

    typename T::OutputType SH_DECL(irrad); 

    lightDir = normalize(lightDir);
    lightVec = normalize(lightVec);
    ShAttrib1f t = -lightDir | lightVec;
    ShAttrib1f cosf = cos(falloff);
    ShAttrib1f cosang = cos(lightAngle);

    irrad = lightColor;
    irrad *= t > cosang; // if outside light angle, always 0 
    irrad *= (t < cosf) * (t - cosang) / (cosf - cosang) + (t >= cosf); // linear blend between start of falloff and 0 
  } SH_END;
  return kernel;
}

template<typename T>
ShProgram ShKernelLight::texLight2D(const ShBaseTexture2D<T> &tex) {
  ShProgram kernel =  SH_BEGIN_PROGRAM() {
    ShInputAttrib1f SH_DECL(scaling);
    ShInputAttrib1f SH_DECL(lightAngle);
    ShInputVector3f SH_DECL(lightDir);
    ShInputVector3f SH_DECL(lightUp);
    ShInputVector3f SH_DECL(lightVec);

    typename T::OutputType SH_DECL(irrad); 

    lightDir = normalize(lightDir);
    lightUp = normalize(lightUp);
    lightVec = normalize(lightVec);
    ShVector3f lightHoriz = cross(lightDir, lightUp);

    ShAttrib2f texcoord;
    texcoord(0) = frac(((-lightVec | lightHoriz) + ShConstAttrib1f(0.5f)) * scaling);
    texcoord(1) = frac(((-lightVec | lightUp) + ShConstAttrib1f(0.5f)) * scaling);

    irrad = tex(texcoord); 
  } SH_END;
  return kernel;
}

}

#endif
