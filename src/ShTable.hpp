#ifndef SHTABLE_HPP
#define SHTABLE_HPP

#include "ShBaseTexture.hpp"

namespace SH {

struct ShTableTraits : public ShTextureTraits {
  ShTableTraits()
    : ShTextureTraits(1, SH_FILTER_NONE, SH_WRAP_CLAMP, SH_CLAMPED)
  {
  }
};

template<typename T>
class ShTable1D
  : public ShBaseTexture1D<T> {
public:
  ShTable1D(int width)
    : ShBaseTexture1D<T>(width, ShTableTraits())
  {}
};

template<typename T>
class ShTable2D
  : public ShBaseTexture2D<T> {
public:
  ShTable2D(int width, int height)
    : ShBaseTexture2D<T>(width, height, ShTableTraits())
  {}
};

template<typename T>
class ShTableRect
  : public ShBaseTextureRect<T> {
public:
  ShTableRect(int width, int height)
    : ShBaseTextureRect<T>(width, height, ShTableTraits())
  {}
};

template<typename T>
class ShTable3D
  : public ShBaseTexture3D<T> {
public:
  ShTable3D(int width, int height, int depth)
    : ShBaseTexture3D<T>(width, height, depth, ShTableTraits())
  {}
};

template<typename T>
class ShTableCube
  : public ShBaseTextureCube<T> {
public:
  ShTableCube(int width, int height)
    : ShBaseTextureCube<T>(width, height, ShTableTraits())
  {}
};

}

#endif
