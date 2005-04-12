// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
const char* cc_texture_string[] = {
"// Sh: A GPU metaprogramming language.\n",
"//\n",
"// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory\n",
"// Project administrator: Michael D. McCool\n",
"// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,\n",
"//          Michael D. McCool\n",
"// \n",
"// This software is provided 'as-is', without any express or implied\n",
"// warranty. In no event will the authors be held liable for any damages\n",
"// arising from the use of this software.\n",
"// \n",
"// Permission is granted to anyone to use this software for any purpose,\n",
"// including commercial applications, and to alter it and redistribute it\n",
"// freely, subject to the following restrictions:\n",
"// \n",
"// 1. The origin of this software must not be misrepresented; you must\n",
"// not claim that you wrote the original software. If you use this\n",
"// software in a product, an acknowledgment in the product documentation\n",
"// would be appreciated but is not required.\n",
"// \n",
"// 2. Altered source versions must be plainly marked as such, and must\n",
"// not be misrepresented as being the original software.\n",
"// \n",
"// 3. This notice may not be removed or altered from any source\n",
"// distribution.\n",
"//////////////////////////////////////////////////////////////////////////////\n",
"\n",
"/** @file CcTextures.hpp\n",
" *\n",
" * This implements 1D, 2D, and 3D texture lookup functions, \n",
" * clamps texture coordinates, but  \n",
" *\n",
" * Assume interpolation/mipmapping and clamping afterwards \n",
" *\n",
" * src indices are assumed to be integers, even if the int \n",
" * allows floating point values.\n",
" *\n",
" * All of the parameters such as tuple-size, clamp mode, etc.\n",
" * will be known statically at time of code emission, so they\n",
" * are all template parameters.\n",
" */\n",
"\n",
"// @todo type\n",
"// add casting code for when MemoryType is not equivalent to the type used in computation\n",
"\n",
"// floor and clamp to [0, max) for aninteger lookup\n",
"template<typename T>\n",
"inline int sh_cc_backend_nearest(T value)\n",
"{\n",
"  return (int)(floor(static_cast<double>(value))); \n",
"}\n",
"\n",
"struct sh_gcc_backend_wrap_clamp\n",
"{\n",
"  static inline int wrap(int src, int Max) \n",
"  {\n",
"    return src >= Max ? Max - 1 : (src < 0 ? 0 : src);\n",
"  }\n",
"};\n",
"\n",
"struct sh_gcc_backend_wrap_repeat\n",
"{\n",
"  static inline int wrap(int src, int Max) \n",
"  {\n",
"    src %= Max;\n",
"    if(src < 0) src += Max;\n",
"    return src; \n",
"  }\n",
"};\n",
"\n",
"struct sh_gcc_backend_clamped\n",
"{\n",
"  template<typename T>\n",
"  static inline T clamp(T dest) \n",
"  {\n",
"    return dest < 0 ? 0 : (dest > 1 ? 1 : dest);\n",
"  }\n",
"};\n",
"\n",
"struct sh_gcc_backend_unclamped\n",
"{\n",
"  template<typename T>\n",
"  static inline T clamp(T dest) \n",
"  {\n",
"    return dest; \n",
"  }\n",
"};\n",
"\n",
"template<int TexDims, int TexSize, int TexWidth, int TexHeight, int TexDepth, typename TexType,\n",
"  typename SrcWrap, typename DestClamp,\n",
"  typename IndexType, typename MemoryType> \n",
"void sh_cc_backend_lookupi(const void *texture, IndexType *src, MemoryType *dest)\n",
"{\n",
"  const TexType* data = reinterpret_cast<const TexType*>(texture);\n",
"  int index = 0;\n",
"  if(TexDims == 3) index = SrcWrap::wrap(sh_cc_backend_nearest(src[2]), TexDepth);\n",
"  if(TexDims >= 2) index = SrcWrap::wrap(sh_cc_backend_nearest(src[1]), TexHeight) \n",
"      + TexHeight * index;\n",
"  index = SrcWrap::wrap(sh_cc_backend_nearest(src[0]), TexWidth)\n",
"          + TexWidth * index;\n",
"\n",
"  int start = index * TexSize; \n",
"  for(int i = 0; i < TexSize; ++i) {\n",
"    dest[i] = static_cast<MemoryType>(DestClamp::clamp(data[start + i])); \n",
"  }\n",
"}\n",
"\n",
"template<int TexDims, int TexSize, int TexWidth, int TexHeight, int TexDepth, typename TexType,\n",
"  typename SrcWrap, typename DestClamp,\n",
"  typename IndexType, typename MemoryType> \n",
"void sh_cc_backend_lookup(const void *texture, IndexType *src, MemoryType *dest)\n",
"{\n",
"  IndexType scaled_src[TexDims];\n",
"  scaled_src[0] = TexWidth * src[0];\n",
"  if(TexDims > 1) scaled_src[1] = TexHeight * src[1];\n",
"  if(TexDims > 2) scaled_src[2] = TexDepth * src[2];\n",
"\n",
"  sh_cc_backend_lookupi<TexDims, TexSize, TexWidth, TexHeight, TexDepth, TexType, \n",
"    SrcWrap, DestClamp>(texture, scaled_src, dest);\n",
"}\n",
"\n",
""};