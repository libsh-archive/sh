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
#ifndef SHCLAMPING_HPP
#define SHCLAMPING_HPP

namespace SH {

/** Set Clamp trait in Texture type.
 * Use this template to indicate that a texture should be set up with
 * the clamp trait enabled.   This version can be
 * used with any dimension of texture; however, a resolution must
 * be provided during construction.
 */
template<typename T>
class ShClamped : public T {
public:
  ShClamped(int width)
    : T(width)
  {
    m_node->traits().clamping(ShTextureTraits::SH_CLAMPED);
  }
  ShClamped(int width, int height)
    : T(width, height)
  {
    m_node->traits().clamping(ShTextureTraits::SH_CLAMPED);
  }
  ShClamped(int width, int height, int depth)
    : T(width, height, depth)
  {
    m_node->traits().clamping(ShTextureTraits::SH_CLAMPED);
  }
};

/** Set Clamp trait in 1D Texture type.
 */
template<typename T>
class ShClamped1D : public T {
public:
  ShClamped1D(int width = 1)
    : T(width)
  {
    m_node->traits().clamping(ShTextureTraits::SH_CLAMPED);
  }
};

/** Set Clamp trait in 2D Texture type.
 */
template<typename T>
class ShClamped2D : public T {
public:
  ShClamped2D(int width = 1, int height = 1)
    : T(width, height)
  {
    m_node->traits().clamping(ShTextureTraits::SH_CLAMPED);
  }
};

/** Set Clamp trait in 3D Texture type.
 */
template<typename T>
class ShClamped3D : public T {
public:
  ShClamped3D(int width = 1, int height = 1, int depth = 1)
    : T(width, height, depth)
  {
    m_node->traits().clamping(ShTextureTraits::SH_CLAMPED);
  }
};

/** Reset Clamp trait in Texture type.
 * Use this template to indicate that a texture should be set up without
 * the clamp trait enabled.   This version can be
 * used with any dimension of texture; however, a resolution must
 * be provided during construction.
 */
template<typename T>
class ShUnclamped : public T {
public:
  ShUnclamped(int width)
    : T(width)
  {
    m_node->traits().clamping(ShTextureTraits::SH_UNCLAMPED);
  }
  ShUnclamped(int width, int height)
    : T(width, height)
  {
    m_node->traits().clamping(ShTextureTraits::SH_UNCLAMPED);
  }
  ShUnclamped(int width, int height, int depth)
    : T(width, height, depth)
  {
    m_node->traits().clamping(ShTextureTraits::SH_UNCLAMPED);
  }
};

/** Reset Clamp trait in 1D Texture type.
 */
template<typename T>
class ShUnclamped1D : public T {
public:
  ShUnclamped1D(int width = 1)
    : T(width)
  {
    m_node->traits().clamping(ShTextureTraits::SH_UNCLAMPED);
  }
};
/** Reset Clamp trait in 2D Texture type.
 */
template<typename T>
class ShUnclamped2D : public T {
public:
  ShUnclamped2D(int width = 1, int height = 1)
    : T(width, height)
  {
    m_node->traits().clamping(ShTextureTraits::SH_UNCLAMPED);
  }
};
/** Reset Clamp trait in 3D Texture type.
 */
template<typename T>
class ShUnclamped3D : public T {
public:
  ShUnclamped3D(int width = 1, int height = 1, int depth = 1)
    : T(width, height, depth)
  {
    m_node->traits().clamping(ShTextureTraits::SH_UNCLAMPED);
  }
};

}

#endif
