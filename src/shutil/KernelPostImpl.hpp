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
#ifndef SHUTIL_KERNELPOSTIMPL_HPP 
#define SHUTIL_KERNELPOSTIMPL_HPP 

#include <sstream>
#include "sh/Syntax.hpp"
#include "sh/Position.hpp"
#include "sh/Manipulator.hpp"
#include "sh/Algebra.hpp"
#include "sh/Program.hpp"
#include "sh/Nibbles.hpp"
#include "KernelPost.hpp"
#include "Func.hpp"

/** \file KernelPostImpl.hpp
 * This is an implementation of useful postprocessing kernels 
 */

namespace ShUtil {

using namespace SH;

template<typename T>
Program halftone(const BaseTexture2D<T> &tex) {
  Program kernel = SH_BEGIN_FRAGMENT_PROGRAM {
    typename T::InputType SH_NAMEDECL(in, "result");
    InputTexCoord2f SH_NAMEDECL(tc, "texcoord");

    typename T::OutputType SH_NAMEDECL(out, "result");

    // TODO rotate color components...
    // TODO decide whether this frac should stay
    out = in > tex(frac(tc));
  } SH_END;
  return kernel;
}

template<int N, typename T>
Program noisify(bool useTexture) {
  Program kernel = SH_BEGIN_FRAGMENT_PROGRAM {
    InputAttrib1f SH_DECL(noise_scale);
    typename T::InputType SH_NAMEDECL(in, "result");
    Attrib<N, SH_INPUT, typename T::storage_type> SH_NAMEDECL(tc, "texcoord");

    typename T::OutputType SH_NAMEDECL(out, "result");

    out = in + cellnoise<T::typesize>(tc, useTexture)*noise_scale; 
  } SH_END;
  return kernel;
}

}

#endif
