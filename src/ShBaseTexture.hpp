#ifndef SHBASETEXTURE_HPP
#define SHBASETEXTURE_HPP

#include <string>
#include "ShTextureNode.hpp"
#include "ShMemory.hpp"
#include "ShVariable.hpp"
#include "ShAttrib.hpp"

namespace SH {

/** Base type for 1D Textures.
 */
template<typename T>
class ShBaseTexture1D : public ShRefCountable {
public:
  ShBaseTexture1D(const ShTextureTraits& traits);
  ShBaseTexture1D(int width, const ShTextureTraits& traits);

  T operator()(const ShGeneric<1, float>& coords) const;
  T operator[](const ShGeneric<1, float>& coords) const;

  ShMemoryPtr memory();
  void memory(ShMemoryPtr memory);
  void size(int width);

  void name(const std::string&);
  std::string name() const;

  ShAttrib1f size() const;

protected:
  ShTextureNodePtr m_node;
};

/** Base type for 2D Textures.
 */
template<typename T>
class ShBaseTexture2D : public ShRefCountable {
public:
  ShBaseTexture2D(const ShTextureTraits& traits);
  ShBaseTexture2D(int width, int height, const ShTextureTraits& traits);

  T operator()(const ShGeneric<2, float>& coords) const;
  T operator[](const ShGeneric<2, float>& coords) const;

  ShMemoryPtr memory();
  void memory(ShMemoryPtr memory);
  void size(int width, int height);

  void name(const std::string&);
  std::string name() const;

  ShAttrib2f size() const;
  
protected:
  ShTextureNodePtr m_node;
};

/** Base type for Rectangular Textures.
 */
template<typename T>
class ShBaseTextureRect : public ShRefCountable {
public:
  ShBaseTextureRect(const ShTextureTraits& traits);
  ShBaseTextureRect(int width, int height, const ShTextureTraits& traits);

  T operator()(const ShGeneric<2, float>& coords) const;
  T operator[](const ShGeneric<2, float>& coords) const;

  ShMemoryPtr memory();
  void memory(ShMemoryPtr memory);
  void size(int width, int height);
  
  void name(const std::string&);
  std::string name() const;

  ShAttrib2f size() const;

protected:
  ShTextureNodePtr m_node;
};

/** Base type for 3D Textures.
 */
template<typename T>
class ShBaseTexture3D : public ShRefCountable {
public:
  ShBaseTexture3D(const ShTextureTraits& traits);
  ShBaseTexture3D(int width, int height, int depth, const ShTextureTraits& traits);

  T operator()(const ShGeneric<3, float>& coords) const;
  T operator[](const ShGeneric<3, float>& coords) const;

  ShMemoryPtr memory();
  void memory(ShMemoryPtr memory);
  void size(int width, int height, int depth);

  void name(const std::string&);
  std::string name() const;

  ShAttrib3f size() const;

protected:
  ShTextureNodePtr m_node;
};

/** Base type for Cube Textures.
 */
template<typename T>
class ShBaseTextureCube : public ShRefCountable {
public:
  ShBaseTextureCube(const ShTextureTraits& traits);
  ShBaseTextureCube(int width, int height, const ShTextureTraits& traits);

  T operator()(const ShGeneric<3, float>& coords) const;

  ShMemoryPtr memory(ShCubeDirection face);
  void memory(ShMemoryPtr memory, ShCubeDirection face);
  void size(int width, int height);
  
  void name(const std::string&);
  std::string name() const;

  ShAttrib2f size() const;
  
protected:
  ShTextureNodePtr m_node;
};

}

#include "ShBaseTextureImpl.hpp"

#endif
