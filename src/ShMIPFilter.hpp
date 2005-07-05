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
#ifndef SHMIPFILTER_HPP
#define SHMIPFILTER_HPP

namespace SH {

/** Enable mipmapping in Texture type.
 * Use this template to indicate that a texture should be mipmapped
 * and whether it should choose the nearest mipmap or interpolate
 * between the two closest one.
 */
template<typename T, bool interpolated=true>
class ShMIPFilter : public T {
public:
  ShMIPFilter()
    : T()
  {
    if (interpolated) {
      this->m_node->traits().filtering(ShTextureTraits::SH_FILTER_MIPMAP_LINEAR);
    } else {
      this->m_node->traits().filtering(ShTextureTraits::SH_FILTER_MIPMAP_NEAREST);
    }
  }
  ShMIPFilter(int width)
    : T(width)
  {
    if (interpolated) {
      this->m_node->traits().filtering(ShTextureTraits::SH_FILTER_MIPMAP_LINEAR);
    } else {
      this->m_node->traits().filtering(ShTextureTraits::SH_FILTER_MIPMAP_NEAREST);
    }
  }
  ShMIPFilter(int width, int height)
    : T(width, height)
  {
    if (interpolated) {
      this->m_node->traits().filtering(ShTextureTraits::SH_FILTER_MIPMAP_LINEAR);
    } else {
      this->m_node->traits().filtering(ShTextureTraits::SH_FILTER_MIPMAP_NEAREST);
    }
  }
  ShMIPFilter(int width, int height, int depth)
    : T(width, height, depth)
  {
    if (interpolated) {
      this->m_node->traits().filtering(ShTextureTraits::SH_FILTER_MIPMAP_LINEAR);
    } else {
      this->m_node->traits().filtering(ShTextureTraits::SH_FILTER_MIPMAP_NEAREST);
    }
  }

  typedef typename T::return_type return_type;
  
};

/** Disable mipmapping in Texture type.
 * Use this template to indicate that a texture should not be mipmapped.
 */
template<typename T>
class ShNoMIPFilter : public T {
public:
  ShNoMIPFilter()
    : T()
  {
    this->m_node->traits().filtering(ShTextureTraits::SH_FILTER_NONE);
  }
  ShNoMIPFilter(int width)
    : T(width)
  {
    this->m_node->traits().filtering(ShTextureTraits::SH_FILTER_NONE);
  }
  ShNoMIPFilter(int width, int height)
    : T(width, height)
  {
    this->m_node->traits().filtering(ShTextureTraits::SH_FILTER_NONE);
  }
  ShNoMIPFilter(int width, int height, int depth)
    : T(width, height, depth)
  {
    this->m_node->traits().filtering(ShTextureTraits::SH_FILTER_NONE);
  }

  typedef typename T::return_type return_type;
  
};

}

#endif
