#ifndef SHBASETEXTURE_HPP
#define SHBASETEXTURE_HPP

#include <string>
#include "ShTextureNode.hpp"
#include "ShMemory.hpp"
#include "ShVariable.hpp"
#include "ShAttrib.hpp"
#include "ShMetaForwarder.hpp"

namespace SH {

class ShBaseTexture : public ShMetaForwarder {
public:
  ShBaseTexture(const ShTextureNodePtr& node);

protected:
  ShTextureNodePtr m_node;
};

/** Base type for 1D Textures.
 */
template<typename T>
class ShBaseTexture1D : public ShBaseTexture {
public:
  ShBaseTexture1D(const ShTextureTraits& traits);
  ShBaseTexture1D(int width, const ShTextureTraits& traits);

  template<typename T2>
  T operator()(const ShGeneric<1, T2>& coords) const;

  template<typename T2>
  T operator[](const ShGeneric<1, T2>& coords) const;

  ShMemoryPtr memory();
  void memory(ShMemoryPtr memory);
  void size(int width);

  ShAttrib1f size() const;

  typedef T return_type;
};

/** Base type for 2D Textures.
 */
template<typename T>
class ShBaseTexture2D : public ShBaseTexture  {
public:
  ShBaseTexture2D(const ShTextureTraits& traits);
  ShBaseTexture2D(int width, int height, const ShTextureTraits& traits);

  template<typename T2>
  T operator()(const ShGeneric<2, T2>& coords) const;

  template<typename T2>
  T operator[](const ShGeneric<2, T2>& coords) const;

  ShMemoryPtr memory();
  void memory(ShMemoryPtr memory);
  void size(int width, int height);

  ShAttrib2f size() const;

  typedef T return_type;
};

/** Base type for Rectangular Textures.
 */
template<typename T>
class ShBaseTextureRect : public ShBaseTexture  {
public:
  ShBaseTextureRect(const ShTextureTraits& traits);
  ShBaseTextureRect(int width, int height, const ShTextureTraits& traits);

  template<typename T2>
  T operator()(const ShGeneric<2, T2>& coords) const;

  template<typename T2>
  T operator[](const ShGeneric<2, T2>& coords) const;

  ShMemoryPtr memory();
  void memory(ShMemoryPtr memory);
  void size(int width, int height);

  ShAttrib2f size() const;

  typedef T return_type;
};

/** Base type for 3D Textures.
 */
template<typename T>
class ShBaseTexture3D : public ShBaseTexture  {
public:
  ShBaseTexture3D(const ShTextureTraits& traits);
  ShBaseTexture3D(int width, int height, int depth, const ShTextureTraits& traits);

  template<typename T2>
  T operator()(const ShGeneric<3, T2>& coords) const;

  template<typename T2>
  T operator[](const ShGeneric<3, T2>& coords) const;

  ShMemoryPtr memory();
  void memory(ShMemoryPtr memory);
  void size(int width, int height, int depth);

  ShAttrib3f size() const;

  typedef T return_type;
};

/** Base type for Cube Textures.
 */
template<typename T>
class ShBaseTextureCube : public ShBaseTexture {
public:
  ShBaseTextureCube(const ShTextureTraits& traits);
  ShBaseTextureCube(int width, int height, const ShTextureTraits& traits);

  template<typename T2>
  T operator()(const ShGeneric<3, T2>& coords) const;

  ShMemoryPtr memory(ShCubeDirection face);
  void memory(ShMemoryPtr memory, ShCubeDirection face);
  void size(int width, int height);

  ShAttrib2f size() const;

  typedef T return_type;
};

}

#include "ShBaseTextureImpl.hpp"

#endif
