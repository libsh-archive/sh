#ifndef SHTEXTURE_HPP
#define SHTEXTURE_HPP

#include "ShBaseTexture.hpp"

namespace SH {

struct ShFilteredTextureTraits : public ShTextureTraits {
  ShFilteredTextureTraits()
    : ShTextureTraits(1, SH_FILTER_MIPMAP, SH_WRAP_CLAMP, SH_CLAMPED)
  {
  }
};

template<typename T>
class ShTexture1D
  : public ShBaseTexture1D<T> {
public:
  ShTexture1D(int width)
    : ShBaseTexture1D<T>(width, ShFilteredTextureTraits())
  {}
};

template<typename T>
class ShTexture2D
  : public ShBaseTexture2D<T> {
public:
  ShTexture2D(int width, int height)
    : ShBaseTexture2D<T>(width, height, ShFilteredTextureTraits())
  {}
   ShTexture2D(int width, int height, ShTextureTraits& traits)
    : ShBaseTexture2D<T>(width, height, traits)
  {}
};

template<typename T>
class ShTextureRect
  : public ShBaseTextureRect<T> {
public:
  ShTextureRect(int width, int height)
    : ShBaseTextureRect<T>(width, height, ShFilteredTextureTraits())
  {}
   ShTextureRect(int width, int height, ShTextureTraits& traits)
    : ShBaseTextureRect<T>(width, height, traits)
  {}
};

template<typename T>
class ShTexture3D
  : public ShBaseTexture3D<T> {
public:
  ShTexture3D(int width, int height, int depth)
    : ShBaseTexture3D<T>(width, height, depth, ShFilteredTextureTraits())
  {}
};

template<typename T>
class ShTextureCube
  : public ShBaseTextureCube<T> {
public:
  ShTextureCube(int width, int height)
    : ShBaseTextureCube<T>(width, height, ShFilteredTextureTraits())
  {}
};

}

#endif
