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
#ifndef SHWRAP_HPP
#define SHWRAP_HPP

namespace SH {

/** Set WrapClamp trait in Texture type.
 * Use this template to indicate that a texture should be set up with 
 * the wrap/clamp boundary treatment enabled.   This version can be
 * used with any dimension of texture; however, a resolution must
 * be provided during construction.
 */
template<typename T>
class ShWrapClamp : public T {
public:
  ShWrapClamp(int width)
    : T(width)
  {
    m_node->traits().wrapping(ShTextureTraits::SH_WRAP_CLAMP);
  }
  ShWrapClamp(int width, int height)
    : T(width, height)
  {
    m_node->traits().wrapping(ShTextureTraits::SH_WRAP_CLAMP);
  }
  ShWrapClamp(int width, int height, int depth)
    : T(width, height, depth)
  {
    m_node->traits().wrapping(ShTextureTraits::SH_WRAP_CLAMP);
  }
};

/** Set WrapClampToEdge trait in Texture type.
 * Use this template to indicate that a texture should be set up with 
 * the wrap/clamp-to-boundary trait enabled.   This version can be
 * used with any dimension of texture; however, a resolution must
 * be provided during construction.
 */
template<typename T>
class ShWrapClampToEdge : public T {
public:
  ShWrapClampToEdge(int width)
    : T(width)
  {
    m_node->traits().wrapping(ShTextureTraits::SH_WRAP_CLAMP_TO_EDGE);
  }
  ShWrapClampToEdge(int width, int height)
    : T(width, height)
  {
    m_node->traits().wrapping(ShTextureTraits::SH_WRAP_CLAMP_TO_EDGE);
  }
  ShWrapClampToEdge(int width, int height, int depth)
    : T(width, height, depth)
  {
    m_node->traits().wrapping(ShTextureTraits::SH_WRAP_CLAMP_TO_EDGE);
  }
};

/** Set WrapRepeat trait in Texture type.
 * Use this template to indicate that a texture should be set up with 
 * the wrap-repeat boundary trait enabled.   This version can be
 * used with any dimension of texture; however, a resolution must
 * be provided during construction.
 */
template<typename T>
class ShWrapRepeat : public T {
public:
  ShWrapRepeat(int width)
    : T(width)
  {
    m_node->traits().wrapping(ShTextureTraits::SH_WRAP_REPEAT);
  }
  ShWrapRepeat(int width, int height)
    : T(width, height)
  {
    m_node->traits().wrapping(ShTextureTraits::SH_WRAP_REPEAT);
  }
  ShWrapRepeat(int width, int height, int depth)
    : T(width, height, depth)
  {
    m_node->traits().wrapping(ShTextureTraits::SH_WRAP_REPEAT);
  }
};

/** Set WrapClamp trait for a 1D Texture type.
 * Use this template to indicate that a 1D texture should be set up with 
 * the wrap-clamp boundary trait enabled.   This version can only
 * be used with textures that have one spatial dimension, such
 * as ShTexture1D.   
 * It provides a default size of 1, so that 
 * a resolution does not need to be provided.
 */
template<typename T>
class ShWrapClamp1D : public T {
public:
  ShWrapClamp1D(int width = 1)
    : T(width)
  {
    m_node->traits().wrapping(ShTextureTraits::SH_WRAP_CLAMP);
  }
};

/** Set WrapClamp trait for a 2D Texture type.
 * Use this template to indicate that a 2D texture should be set up with 
 * the wrap-clamp boundary trait enabled.   This version can only
 * be used with textures that have two spatial dimensions, such
 * as ShTexture2D, ShTextureRect, or ShTextureCube.   
 * It provides a default size of 1x1, so that 
 * a resolution does not need to be provided.
 */
template<typename T>
class ShWrapClamp2D : public T {
public:
  ShWrapClamp2D(int width = 1, int height = 1)
    : T(width, height)
  {
    m_node->traits().wrapping(ShTextureTraits::SH_WRAP_CLAMP);
  }
};

/** Set WrapClamp trait for a 3D Texture type.
 * Use this template to indicate that a 3D texture should be set up with 
 * the wrap-clamp boundary trait enabled.   This version can only
 * be used with textures that have three spatial dimensions, such
 * as ShTexture3D.
 * It provides a default size of 1x1x1, so that 
 * a resolution does not need to be provided.
 */
template<typename T>
class ShWrapClamp3D : public T {
public:
  ShWrapClamp3D(int width = 1, int height = 1, int depth = 1)
    : T(width, height, depth)
  {
    m_node->traits().wrapping(ShTextureTraits::SH_WRAP_CLAMP);
  }
};

/** Set WrapClampToEdge trait for a 1D Texture type.
 * Use this template to indicate that a 1D texture should be set up with 
 * the wrap-clamp-to-edge boundary trait enabled.   This version can only
 * be used with textures that have one spatial dimension, such
 * as ShTexture1D.   
 * It provides a default size of 1, so that 
 * a resolution does not need to be provided.
 */
template<typename T>
class ShWrapClampToEdge1D : public T {
public:
  ShWrapClampToEdge1D(int width = 1)
    : T(width)
  {
    m_node->traits().wrapping(ShTextureTraits::SH_WRAP_CLAMP_TO_EDGE);
  }
};

/** Set WrapClampToEdge trait for a 2D Texture type.
 * Use this template to indicate that a 2D texture should be set up with 
 * the wrap-clamp-to-edge boundary trait enabled.   This version can only
 * be used with textures that have two spatial dimensions, such
 * as ShTexture2D, ShTextureRect, or ShTextureCube.   
 * It provides a default size of 1x1, so that 
 * a resolution does not need to be provided.
 */
template<typename T>
class ShWrapClampToEdge2D : public T {
public:
  ShWrapClampToEdge2D(int width = 1, int height = 1)
    : T(width, height)
  {
    m_node->traits().wrapping(ShTextureTraits::SH_WRAP_CLAMP_TO_EDGE);
  }
};

/** Set WrapClampToEdge trait for a 3D Texture type.
 * Use this template to indicate that a 3D texture should be set up with 
 * the wrap-clamp-to-edge boundary trait enabled.   This version can only
 * be used with textures that have three spatial dimensions, such
 * as ShTexture3D.
 * It provides a default size of 1x1x1, so that 
 * a resolution does not need to be provided.
 */
template<typename T>
class ShWrapClampToEdge3D : public T {
public:
  ShWrapClampToEdge3D(int width = 1, int height = 1, int depth = 1)
    : T(width, height, depth)
  {
    m_node->traits().wrapping(ShTextureTraits::SH_WRAP_CLAMP_TO_EDGE);
  }
};

/** Set WrapRepeat trait for a 1D Texture type.
 * Use this template to indicate that a 1D texture should be set up with 
 * the wrap-repeat boundary trait enabled.   This version can only
 * be used with textures that have one spatial dimension, such
 * as ShTexture1D.   
 * It provides a default size of 1, so that 
 * a resolution does not need to be provided.
 */
template<typename T>
class ShWrapRepeat1D : public T {
public:
  ShWrapRepeat1D(int width = 1)
    : T(width)
  {
    m_node->traits().wrapping(ShTextureTraits::SH_WRAP_REPEAT);
  }
};

/** Set WrapRepeat trait for a 2D Texture type.
 * Use this template to indicate that a 2D texture should be set up with 
 * the wrap-repeat boundary trait enabled.   This version can only
 * be used with textures that have two spatial dimensions, such
 * as ShTexture2D, ShTextureRect, or ShTextureCube.   
 * It provides a default size of 1x1, so that 
 * a resolution does not need to be provided.
 */
template<typename T>
class ShWrapRepeat2D : public T {
public:
  ShWrapRepeat2D(int width = 1, int height = 1)
    : T(width, height)
  {
    m_node->traits().wrapping(ShTextureTraits::SH_WRAP_REPEAT);
  }
};

/** Set WrapRepeat trait for a 3D Texture type.
 * Use this template to indicate that a 3D texture should be set up with 
 * the wrap-repeat boundary trait enabled.   This version can only
 * be used with textures that have three spatial dimensions, such
 * as ShTexture3D.   It provides a default size of 1x1x1, so that 
 * a resolution does not need to be provided.
 */
template<typename T>
class ShWrapRepeat3D : public T {
public:
  ShWrapRepeat3D(int width = 1, int height = 1, int depth = 1)
    : T(width, height, depth)
  {
    m_node->traits().wrapping(ShTextureTraits::SH_WRAP_REPEAT);
  }
};

}

#endif
