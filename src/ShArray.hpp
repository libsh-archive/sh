#ifndef SHARRAY_HPP
#define SHARRAY_HPP

#include "ShBaseTexture.hpp"

namespace SH {

const unsigned int SH_ARRAY_TRAITS =
  SH_LOOKUP_NEAREST | SH_FILTER_NONE | SH_WRAP_CLAMP;

template<typename T>
class ShArray1D
  : public ShBaseTexture1D<T, SH_ARRAY_TRAITS> {
public:
  ShArray1D(int width)
    : ShBaseTexture1D<T, SH_ARRAY_TRAITS>(width)
  {}
};

template<typename T>
class ShArray2D
  : public ShBaseTexture2D<T, SH_ARRAY_TRAITS> {
public:
  ShArray2D(int width, int height)
    : ShBaseTexture2D<T, SH_ARRAY_TRAITS>(width, height)
  {}
};

template<typename T>
class ShArrayRect
  : public ShBaseTextureRect<T, SH_ARRAY_TRAITS> {
public:
  ShArrayRect(int width, int height)
    : ShBaseTextureRect<T, SH_ARRAY_TRAITS>(width, height)
  {}
};

template<typename T>
class ShArray3D
  : public ShBaseTexture3D<T, SH_ARRAY_TRAITS> {
public:
  ShArray3D(int width, int height, int depth)
    : ShBaseTexture3D<T, SH_ARRAY_TRAITS>(width, height, depth)
  {}
};

template<typename T>
class ShArrayCube
  : public ShBaseTextureCube<T, SH_ARRAY_TRAITS> {
public:
  ShArrayCube(int width, int height)
    : ShBaseTextureCube<T, SH_ARRAY_TRAITS>(width, height)
  {}
};

}

#endif
