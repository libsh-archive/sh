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

/** @file CcTextures.hpp
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

// @todo type
// add casting code for when MemoryType is not equivalent to the type used in computation

// floor and clamp to [0, max) for aninteger lookup
template<typename T>
inline int sh_cc_backend_nearest(T value)
{
  return (int)(floor(value)); 
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
  static inline int wrap(int src, int Max) 
  {
    src %= Max;
    if(src < 0) src += Max;
    return src; 
  }
};

struct sh_gcc_backend_clamped
{
  template<typename T>
  static inline T clamp(T dest) 
  {
    return dest < 0 ? 0 : (dest > 1 ? 1 : dest);
  }
};

struct sh_gcc_backend_unclamped
{
  template<typename T>
  static inline T clamp(T dest) 
  {
    return dest; 
  }
};

template<int TexDims, int TexSize, int TexWidth, int TexHeight, int TexDepth, 
  typename SrcWrap, typename DestClamp,
  typename IndexType, typename MemoryType> 
void sh_cc_backend_lookupi(const void *texture, IndexType *src, MemoryType *dest)
{
  const MemoryType* data = reinterpret_cast<const MemoryType *>(texture);
  int index = 0;
  if(TexDims == 3) index = SrcWrap::wrap(sh_cc_backend_nearest(src[2]), TexDepth);
  if(TexDims >= 2) index = SrcWrap::wrap(sh_cc_backend_nearest(src[1]), TexHeight) 
      + TexHeight * index;
  index = SrcWrap::wrap(sh_cc_backend_nearest(src[0]), TexWidth)
          + TexWidth * index;

  int start = index * TexSize; 
  for(int i = 0; i < TexSize; ++i) {
    dest[i] = DestClamp::clamp(data[start + i]); 
  }
}

template<int TexDims, int TexSize, int TexWidth, int TexHeight, int TexDepth, 
  typename SrcWrap, typename DestClamp,
  typename IndexType, typename MemoryType> 
void sh_cc_backend_lookup(const void *texture, IndexType *src, MemoryType *dest)
{
  IndexType scaled_src[TexDims];
  scaled_src[0] = TexWidth * src[0];
  if(TexDims > 1) scaled_src[1] = TexHeight * src[1];
  if(TexDims > 2) scaled_src[2] = TexDepth * src[2];

  sh_cc_backend_lookupi<TexDims, TexSize, TexWidth, TexHeight, TexDepth, 
    SrcWrap, DestClamp>(texture, scaled_src, dest);
}

