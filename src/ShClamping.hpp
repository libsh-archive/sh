#ifndef SHCLAMPING_HPP
#define SHCLAMPING_HPP

namespace SH {

template<typename T>
class ShClamped : public T {
public:
  ShClamped(int width)
    : T(width)
  {
    m_node->traits().clamping(ShTextureTraits::SH_CLAMPED);
  }
  ShClamped(int width, int height)
    : T(width, height)
  {
    m_node->traits().clamping(ShTextureTraits::SH_CLAMPED);
  }
  ShClamped(int width, int height, int depth)
    : T(width, height, depth)
  {
    m_node->traits().clamping(ShTextureTraits::SH_CLAMPED);
  }
};
template<typename T>
class ShClamped1D : public T {
public:
  ShClamped1D(int width = 1)
    : T(width)
  {
    m_node->traits().clamping(ShTextureTraits::SH_CLAMPED);
  }
};
template<typename T>
class ShClamped2D : public T {
public:
  ShClamped2D(int width = 1, int height = 1)
    : T(width, height)
  {
    m_node->traits().clamping(ShTextureTraits::SH_CLAMPED);
  }
};
template<typename T>
class ShClamped3D : public T {
public:
  ShClamped3D(int width = 1, int height = 1, int depth = 1)
    : T(width, height, depth)
  {
    m_node->traits().clamping(ShTextureTraits::SH_CLAMPED);
  }
};

template<typename T>
class ShUnclamped : public T {
public:
  ShUnclamped(int width)
    : T(width)
  {
    m_node->traits().clamping(ShTextureTraits::SH_UNCLAMPED);
  }
  ShUnclamped(int width, int height)
    : T(width, height)
  {
    m_node->traits().clamping(ShTextureTraits::SH_UNCLAMPED);
  }
  ShUnclamped(int width, int height, int depth)
    : T(width, height, depth)
  {
    m_node->traits().clamping(ShTextureTraits::SH_UNCLAMPED);
  }
};
template<typename T>
class ShUnclamped1D : public T {
public:
  ShUnclamped1D(int width = 1)
    : T(width)
  {
    m_node->traits().clamping(ShTextureTraits::SH_UNCLAMPED);
  }
};
template<typename T>
class ShUnclamped2D : public T {
public:
  ShUnclamped2D(int width = 1, int height = 1)
    : T(width, height)
  {
    m_node->traits().clamping(ShTextureTraits::SH_UNCLAMPED);
  }
};
template<typename T>
class ShUnclamped3D : public T {
public:
  ShUnclamped3D(int width = 1, int height = 1, int depth = 1)
    : T(width, height, depth)
  {
    m_node->traits().clamping(ShTextureTraits::SH_UNCLAMPED);
  }
};

}

#endif
