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
#ifndef SHUTIL_KERNELSURFACEIMPL_HPP 
#define SHUTIL_KERNELSURFACEIMPL_HPP 

#include <sstream>
#include "sh/Syntax.hpp"
#include "sh/Position.hpp"
#include "sh/Manipulator.hpp"
#include "sh/Algebra.hpp"
#include "sh/Program.hpp"
#include "sh/Nibbles.hpp"
#include "KernelSurface.hpp"
#include "Func.hpp"

/** \file KernelSurfaceImpl.hpp
 * This is an implementation of useful surface kernels 
 */

namespace ShUtil {

using namespace SH;

template<typename T>
Program KernelSurface::diffuse() {
  Program kernel = SH_BEGIN_FRAGMENT_PROGRAM {
    typename T::InputType SH_DECL(kd);
    typename T::InputType SH_DECL(irrad);
    InputNormal3f SH_DECL(normal);
    InputVector3f SH_DECL(lightVec);
    InputPosition4f SH_DECL(posh);

    irrad *= pos(dot(normalize(normal), normalize(lightVec)));
    typename T::OutputType SH_DECL(result);
    result = irrad * kd; 
  } SH_END;
  return kernel;
}

template<typename T>
Program KernelSurface::specular() {
  Program kernel = SH_BEGIN_FRAGMENT_PROGRAM {
    typename T::InputType SH_DECL(ks);
    InputAttrib1f SH_DECL(specExp);
    typename T::InputType SH_DECL(irrad);

    InputNormal3f SH_DECL(normal);
    InputVector3f SH_DECL(halfVec);
    InputVector3f SH_DECL(lightVec);
    InputPosition4f SH_DECL(posh);

    normal = normalize(normal);
    halfVec = normalize(halfVec);
    lightVec = normalize(lightVec);
    irrad *= pos(normal | lightVec);

    typename T::OutputType SH_DECL(result);
    result = irrad * ks * pow(pos(normal | halfVec),specExp); 
  } SH_END;
  return kernel;
}


template<typename T>
Program KernelSurface::phong() {
  Program kernel = SH_BEGIN_PROGRAM("gpu:fragment") {
    typename T::InputType SH_DECL(kd);
    typename T::InputType SH_DECL(ks);
    InputAttrib1f SH_DECL(specExp);
    typename T::InputType SH_DECL(irrad);

    InputNormal3f SH_DECL(normal);
    InputVector3f SH_DECL(halfVec);
    InputVector3f SH_DECL(lightVec);
    InputPosition4f SH_DECL(posh);

    typename T::OutputType SH_DECL(result);

    normal = normalize(normal);
    halfVec = normalize(halfVec);
    lightVec = normalize(lightVec);
    irrad *= pos(normal | lightVec);
    result = irrad * (kd + ks * pow(pos(normal | halfVec), specExp)); 
  } SH_END;
  return kernel;
}

template<typename T>
Program KernelSurface::gooch() {
  Program kernel = SH_BEGIN_PROGRAM("gpu:fragment") {
    typename T::InputType SH_DECL(kd);
    typename T::InputType SH_DECL(cool);
    typename T::InputType SH_DECL(warm);
    typename T::InputType SH_DECL(irrad);

    InputNormal3f SH_DECL(normal);
    InputVector3f SH_DECL(lightVec);
    InputPosition4f SH_DECL(posh);

    typename T::OutputType SH_DECL(result);

    normal = normalize(normal);
    lightVec = normalize(lightVec);
    result = lerp(mad((normal | lightVec), ConstAttrib1f(0.5f), ConstAttrib1f(0.5f)) * irrad, warm, cool) * kd;
  } SH_END;
  return kernel;
}

template<typename T>
Program KernelSurface::null() {
  Program kernel = SH_BEGIN_PROGRAM("gpu:fragment") {
    typename T::InputType SH_DECL(irrad);
    InputPosition4f SH_DECL(posh);

    typename T::OutputType SH_DECL(result) = irrad;
  } SH_END;
  return kernel;
}

}

#endif
