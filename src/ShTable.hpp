#ifndef SHTABLE_HPP
#define SHTABLE_HPP

#include "ShBaseTexture.hpp"

namespace SH {

const unsigned int SH_TABLE_TRAITS =
  SH_LOOKUP_LINEAR | SH_FILTER_NONE | SH_WRAP_CLAMP;

template<typename T>
class ShTable1D
  : public ShBaseTexture1D<T, SH_TABLE_TRAITS> {
public:
  ShTable1D(int width)
    : ShBaseTexture1D<T, SH_TABLE_TRAITS>(width)
  {}
};

template<typename T>
class ShTable2D
  : public ShBaseTexture2D<T, SH_TABLE_TRAITS> {
public:
  ShTable2D(int width, int height)
    : ShBaseTexture2D<T, SH_TABLE_TRAITS>(width, height)
  {}
};

template<typename T>
class ShTableRect
  : public ShBaseTextureRect<T, SH_TABLE_TRAITS> {
public:
  ShTableRect(int width, int height)
    : ShBaseTextureRect<T, SH_TABLE_TRAITS>(width, height)
  {}
};

template<typename T>
class ShTable3D
  : public ShBaseTexture3D<T, SH_TABLE_TRAITS> {
public:
  ShTable3D(int width, int height, int depth)
    : ShBaseTexture3D<T, SH_TABLE_TRAITS>(width, height, depth)
  {}
};

template<typename T>
class ShTableCube
  : public ShBaseTextureCube<T, SH_TABLE_TRAITS> {
public:
  ShTableCube(int width, int height)
    : ShBaseTextureCube<T, SH_TABLE_TRAITS>(width, height)
  {}
};

}

#endif
