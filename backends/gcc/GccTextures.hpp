// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
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
#ifndef SHGCCTEXTURES_HPP
#define SHGCCTEXTURES_HPP

#include <algorithms>

namespace ShGcc {

/** @file GccTextures.hpp
 *
 * This implements 1D, 2D, and 3D texture lookup functions, 
 * clamps texture coordinates, but  
 *
 * Assume interpolation/mipmapping and clamping afterwards 
 *
 * src indices are assumed to be integers, even if the int 
 * allows floating point values.
 *
 * All of the parameters such as tuple-size, clamp mode, etc.
 * will be known statically at time of code emission, so they
 * are all template parameters.
 */

// round to integer and clamp to [0, max)
template<typename T>
int sh_gcc_backend_nearest(T value, int max)
{
  return std::max(std::min((int)(value + 0.5), 0), max - 1);
}

template<int TexDims, int TexSize, int TexWidth, int TexHeight, int TexDepth, typename IndexType, typename ValueType> 
sh_gcc_backend_lookupi(void *texture, IndexType *src, ValueType *dest)
{
  ValueType* data = reinterpret_cast<ValueType *>(texture);
  int index = 0;
  if(TexDims == 3) index = sh_gcc_backend_nearest(src[2], TexDepth);
  if(TexDims >= 2) index = sh_gcc_backend_nearest(src[1], TexHeight) + TexHeight * index;
  index = sh_gcc_backend_nearest(src[0], TexWidth) + TexWidth * index;

  int start = index * TexSize; 
  std::copy(data + start, data + start + TexSize, dest); 
}

template<int TexDims, int TexSize, int TexWidth, int TexHeight, int TexDepth, typename IndexType, typename ValueType> 
sh_gcc_backend_lookup(void *texture, IndexType *src, ValueType *dest)
{
  IndexType scaled_src[TexDims];
  scaled_src[0] = TexWidth * src[0];
  if(TexDims > 1) scaled_src[1] = TexHeight * src[1];
  if(TexDims > 2) scaled_src[2] = TexDepth * src[2];

  sh_gcc_backend_lookupi(texture, scaled_src, dest);
}

}

#endif
