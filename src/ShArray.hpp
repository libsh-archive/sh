#ifndef SHARRAY_HPP
#define SHARRAY_HPP

#include "ShBaseTexture.hpp"

namespace SH {

/** Default traits for ShArray.
 * An array is a texture that does not support filtering or interpolation.
 */
struct ShArrayTraits : public ShTextureTraits {
  ShArrayTraits()
    : ShTextureTraits(0, SH_FILTER_NONE, SH_WRAP_CLAMP_TO_EDGE, SH_CLAMPED)
  {
  }
};

/** One-dimensional array.
 */
template<typename T>
class ShArray1D
  : public ShBaseTexture1D<T> {
public:
  ShArray1D(int width)
    : ShBaseTexture1D<T>(width, ShArrayTraits())
  {}
};

/** Two-dimensional square power-of-two array.
 */
template<typename T>
class ShArray2D
  : public ShBaseTexture2D<T> {
public:
  ShArray2D(int width, int height)
    : ShBaseTexture2D<T>(width, height, ShArrayTraits())
  {}
};

/** Two-dimensional non-square array.
 */
template<typename T>
class ShArrayRect
  : public ShBaseTextureRect<T> {
public:
  ShArrayRect(int width, int height)
    : ShBaseTextureRect<T>(width, height, ShArrayTraits())
  {}
};

/** Three-dimensional array.
 */
template<typename T>
class ShArray3D
  : public ShBaseTexture3D<T> {
public:
  ShArray3D(int width, int height, int depth)
    : ShBaseTexture3D<T>(width, height, depth, ShArrayTraits())
  {}
};

/** Cube array.
 * A cube array is indexed by a 3D vector, and has six square power-of-two
 * faces.   The texel indexed depends only on the direction of the vector.
 */
template<typename T>
class ShArrayCube
  : public ShBaseTextureCube<T> {
public:
  ShArrayCube(int width, int height)
    : ShBaseTextureCube<T>(width, height, ShArrayTraits())
  {}
};

}

#endif
