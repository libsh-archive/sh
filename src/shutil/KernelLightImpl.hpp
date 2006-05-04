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
#ifndef SHUTIL_KERNELLIGHTIMPL_HPP 
#define SHUTIL_KERNELLIGHTIMPL_HPP 

#include <sstream>
#include "sh/Syntax.hpp"
#include "sh/Position.hpp"
#include "sh/Manipulator.hpp"
#include "sh/Algebra.hpp"
#include "sh/Program.hpp"
#include "sh/Nibbles.hpp"
#include "KernelLight.hpp"

/** \file KernelLightImpl.hpp
 * This is an implementation of useful kernels and nibbles (simple kernels).
 */

namespace ShUtil {

using namespace SH;

template <typename T>
Program KernelLight::pointLight() {
  Program kernel =  SH_BEGIN_PROGRAM() {
    typename T::InputType SH_DECL(lightColor);
    typename T::OutputType SH_DECL(irrad) = lightColor;
  } SH_END;
  return kernel;
}

template<typename T>
Program KernelLight::spotLight() {
  Program kernel =  SH_BEGIN_PROGRAM() {
    typename T::InputType SH_DECL(lightColor);
    InputAttrib1f SH_DECL(falloff);
    InputAttrib1f SH_DECL(lightAngle);
    InputVector3f SH_DECL(lightDir);
    InputVector3f SH_DECL(lightVec);

    typename T::OutputType SH_DECL(irrad); 

    lightDir = normalize(lightDir);
    lightVec = normalize(lightVec);
    Attrib1f t = -lightDir | lightVec;
    Attrib1f cosf = cos(falloff);
    Attrib1f cosang = cos(lightAngle);

    irrad = lightColor;
    irrad *= t > cosang; // if outside light angle, always 0 
    irrad *= (t < cosf) * (t - cosang) / (cosf - cosang) + (t >= cosf); // linear blend between start of falloff and 0 
  } SH_END;
  return kernel;
}

template<typename T>
Program KernelLight::texLight2D(const BaseTexture2D<T> &tex) {
  Program kernel =  SH_BEGIN_PROGRAM() {
    InputAttrib1f SH_DECL(scaling);
    InputAttrib1f SH_DECL(lightAngle);
    InputVector3f SH_DECL(lightDir);
    InputVector3f SH_DECL(lightUp);
    InputVector3f SH_DECL(lightVec);

    typename T::OutputType SH_DECL(irrad); 

    lightDir = normalize(lightDir);
    lightUp = normalize(lightUp);
    lightVec = normalize(lightVec);
    Vector3f lightHoriz = cross(lightDir, lightUp);

    Attrib2f texcoord;
    texcoord(0) = frac(((-lightVec | lightHoriz) + ConstAttrib1f(0.5f)) * scaling);
    texcoord(1) = frac(((-lightVec | lightUp) + ConstAttrib1f(0.5f)) * scaling);

    irrad = tex(texcoord); 
  } SH_END;
  return kernel;
}

}

#endif
