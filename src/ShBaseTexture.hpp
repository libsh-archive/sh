#ifndef SHBASETEXTURE_HPP
#define SHBASETEXTURE_HPP

#include "ShTextureNode.hpp"
#include "ShMemory.hpp"
#include "ShVariable.hpp"

namespace SH {

template<typename T, unsigned int Traits>
class ShBaseTexture1D : public ShRefCountable {
public:
  ShBaseTexture1D(int width);

  T operator()(const ShVariableN<1, float>& coords) const;
  T operator[](const ShVariableN<1, float>& coords) const;

  ShMemoryPtr memory();
  void memory(ShMemoryPtr memory);
  
protected:
  ShTextureNodePtr m_node;
};

template<typename T, unsigned int Traits>
class ShBaseTexture2D : public ShRefCountable {
public:
  ShBaseTexture2D(int width, int height);

  T operator()(const ShVariableN<2, float>& coords) const;
  T operator[](const ShVariableN<2, float>& coords) const;

  ShMemoryPtr memory();
  void memory(ShMemoryPtr memory);
  
protected:
  ShTextureNodePtr m_node;
};

template<typename T, unsigned int Traits>
class ShBaseTextureRect : public ShRefCountable {
public:
  ShBaseTextureRect(int width, int height);

  T operator()(const ShVariableN<2, float>& coords) const;
  T operator[](const ShVariableN<2, float>& coords) const;

  ShMemoryPtr memory();
  void memory(ShMemoryPtr memory);
  
protected:
  ShTextureNodePtr m_node;
};

template<typename T, unsigned int Traits>
class ShBaseTexture3D : public ShRefCountable {
public:
  ShBaseTexture3D(int width, int height, int depth);

  T operator()(const ShVariableN<3, float>& coords) const;
  T operator[](const ShVariableN<3, float>& coords) const;

  ShMemoryPtr memory();
  void memory(ShMemoryPtr memory);
  
protected:
  ShTextureNodePtr m_node;
};

template<typename T, unsigned int Traits>
class ShBaseTextureCube : public ShRefCountable {
public:
  ShBaseTextureCube(int width, int height);

  T operator()(const ShVariableN<3, float>& coords) const;
  T operator[](const ShVariableN<3, float>& coords) const;

  ShMemoryPtr memory(ShCubeDirection face);
  void memory(ShMemoryPtr memory, ShCubeDirection face);
  
protected:
  ShTextureNodePtr m_node;
};

}

#include "ShBaseTextureImpl.hpp"

#endif
