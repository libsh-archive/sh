#ifndef SHTEXTURE_HPP
#define SHTEXTURE_HPP

#include "ShBaseTexture.hpp"

namespace SH {

/** Trait class to represent texture filtering modes.
 */
struct ShFilteredTextureTraits : public ShTextureTraits {
  ShFilteredTextureTraits()
    : ShTextureTraits(1, SH_FILTER_MIPMAP, SH_WRAP_CLAMP_TO_EDGE, SH_CLAMPED)
  {
  }
};

/** One-dimensional power-of-two texture type.
 */
template<typename T>
class ShTexture1D
  : public ShBaseTexture1D<T> {
public:
  ShTexture1D(int width = 1)
    : ShBaseTexture1D<T>(width, ShFilteredTextureTraits())
  {}
};

/** Two-dimensional square power-of-two texture type.
 */
template<typename T>
class ShTexture2D
  : public ShBaseTexture2D<T> {
public:
  ShTexture2D(int width = 1, int height = 1)
    : ShBaseTexture2D<T>(width, height, ShFilteredTextureTraits())
  {}
   ShTexture2D(int width, int height, ShTextureTraits& traits)
    : ShBaseTexture2D<T>(width, height, traits)
  {}
};

/** Two-dimensional rectangular texture type.
 */
template<typename T>
class ShTextureRect
  : public ShBaseTextureRect<T> {
public:
  ShTextureRect(int width = 1, int height = 1)
    : ShBaseTextureRect<T>(width, height, ShFilteredTextureTraits())
  {}
   ShTextureRect(int width, int height, ShTextureTraits& traits)
    : ShBaseTextureRect<T>(width, height, traits)
  {}
};

/** Three-dimensional texture type.
 */
template<typename T>
class ShTexture3D
  : public ShBaseTexture3D<T> {
public:
  ShTexture3D(int width = 1, int height = 1, int depth = 1)
    : ShBaseTexture3D<T>(width, height, depth, ShFilteredTextureTraits())
  {}
};

/** Cubic texture type.
 * Cubic textures have six faces and are indexed by a three-dimensional
 * vector.   This vector does NOT have to be unit length.   The texel
 * indexed will depend only on the direction of the vector.
 */
template<typename T>
class ShTextureCube
  : public ShBaseTextureCube<T> {
public:
  ShTextureCube(int width = 1, int height = 1)
    : ShBaseTextureCube<T>(width, height, ShFilteredTextureTraits())
  {}
};

}

#endif
