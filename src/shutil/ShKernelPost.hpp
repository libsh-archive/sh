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
#ifndef SHUTIL_KERNELPOST_HPP 
#define SHUTIL_KERNELPOST_HPP 

#include <string>
#include "sh/ShMatrix.hpp"
#include "sh/ShTexture.hpp"
#include "sh/ShProgram.hpp"

/** \file ShKernelPost.hpp
 * These are postprocessing kernels.  Several postprocessing kernels can be
 * connected together to create a more complicated postprocessing kernel.
 *
 * They must take an input result of type T and return one output of type T named result.
 */

namespace ShUtil {

using namespace SH;

/** screen space Halftoning/Hatching in each color channel using tex as a threshold image
 * IN(0) T result 
 * IN(1) ShTexcoord2f texcoord
 *
 * OUT(0) T result            - output result 
 */
template<typename T>
static ShProgram shHalftone(const ShBaseTexture2D<T> &tex);

/** screen space noise 
 * IN(0) ShAttrib1f noise_scale - scaling on cellnoise
 * IN(1) T result 
 * IN(2) ShAttrib<N> texcoord  
 *
 * OUT(0) T result            - output result 
 */
template<int N, typename T>
static ShProgram shNoisify(bool useTexture = false);

}

#include "ShKernelPostImpl.hpp"

#endif
