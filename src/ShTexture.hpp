#ifndef SHTEXTURE_HPP
#define SHTEXTURE_HPP

#include "ShBaseTexture.hpp"

namespace SH {

const unsigned int SH_TEXTURE_TRAITS =
  SH_LOOKUP_LINEAR | SH_FILTER_MIPMAP | SH_WRAP_CLAMP;

template<typename T>
class ShTexture1D
  : public ShBaseTexture1D<T, SH_TEXTURE_TRAITS> {
public:
  ShTexture1D(int width)
    : ShBaseTexture1D<T, SH_TEXTURE_TRAITS>(width)
  {}
};

template<typename T>
class ShTexture2D
  : public ShBaseTexture2D<T, SH_TEXTURE_TRAITS> {
public:
  ShTexture2D(int width, int height)
    : ShBaseTexture2D<T, SH_TEXTURE_TRAITS>(width, height)
  {}
};

template<typename T>
class ShTextureRect
  : public ShBaseTextureRect<T, SH_TEXTURE_TRAITS> {
public:
  ShTextureRect(int width, int height)
    : ShBaseTextureRect<T, SH_TEXTURE_TRAITS>(width, height)
  {}
};

template<typename T>
class ShTexture3D
  : public ShBaseTexture3D<T, SH_TEXTURE_TRAITS> {
public:
  ShTexture3D(int width, int height, int depth)
    : ShBaseTexture3D<T, SH_TEXTURE_TRAITS>(width, height, depth)
  {}
};

template<typename T>
class ShTextureCube
  : public ShBaseTextureCube<T, SH_TEXTURE_TRAITS> {
public:
  ShTextureCube(int width, int height)
    : ShBaseTextureCube<T, SH_TEXTURE_TRAITS>(width, height)
  {}
};

}

#endif
