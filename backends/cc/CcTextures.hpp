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

/** @file CcTextures.hpp
 *
 * This implements 1D, 2D, and 3D texture lookup functions, 
 * clamps texture coordinates, but  
 *
 * Assume interpolation/mipmapping afterwards 
 *
 * src indices are assumed to be integers, even if the int 
 * allows floating point values.
 *
 * All of the parameters such as tuple-size, etc.
 * will be known statically at time of code emission, so they
 * are all template parameters.
 */

// @todo type
// add casting code for when MemoryType is not equivalent to the type used in computation

// floor and clamp to [0, max) for aninteger lookup
template<typename T>
inline int sh_cc_backend_nearest(T value)
{
  return (int)(floor(static_cast<double>(value))); 
}

struct sh_gcc_backend_wrap_clamp
{
  static inline int wrap(int src, int Max) 
  {
    return src >= Max ? Max - 1 : (src < 0 ? 0 : src);
  }
};

struct sh_gcc_backend_wrap_repeat
{
  static int wrap(int src, int Max) 
  {
    src %= Max;
    if(src < 0) src += Max;
    return src; 
  }
};

template<int TexDims, int TexSize, int TexWidth, int TexHeight, int TexDepth, typename TexType,
  typename SrcWrap, typename IndexType, typename MemoryType> 
void sh_cc_backend_lookupi(const void *texture, IndexType *src, MemoryType *dest)
{
  const TexType* data = reinterpret_cast<const TexType*>(texture);
  int index = 0;
  if(TexDims == 3) index = SrcWrap::wrap(sh_cc_backend_nearest(src[2]), TexDepth);
  if(TexDims >= 2) index = SrcWrap::wrap(sh_cc_backend_nearest(src[1]), TexHeight) 
      + TexHeight * index;
  index = SrcWrap::wrap(sh_cc_backend_nearest(src[0]), TexWidth)
          + TexWidth * index;

  int start = index * TexSize; 
  for(int i = 0; i < TexSize; ++i) {
    dest[i] = static_cast<MemoryType>(data[start + i]); 
  }
}

template<int TexDims, int TexSize, int TexWidth, int TexHeight, int TexDepth, typename TexType,
  typename SrcWrap, typename IndexType, typename MemoryType> 
void sh_cc_backend_lookup(const void *texture, IndexType *src, MemoryType *dest)
{
  IndexType scaled_src[TexDims];
  scaled_src[0] = TexWidth * src[0];
  if(TexDims > 1) scaled_src[1] = TexHeight * src[1];
  if(TexDims > 2) scaled_src[2] = TexDepth * src[2];

  sh_cc_backend_lookupi<TexDims, TexSize, TexWidth, TexHeight, TexDepth, TexType, 
    SrcWrap>(texture, scaled_src, dest);
}

