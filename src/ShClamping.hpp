#ifndef SHCLAMPING_HPP
#define SHCLAMPING_HPP

namespace SH {

template<typename T>
class ShClamped : public T {
public:
  ShClamped(int width)
    : T(width)
  {
    m_node->traits().wrapping(ShTextureTraits::SH_CLAMPED);
  }
  ShClamped(int width, int height)
    : T(width, height)
  {
    m_node->traits().wrapping(ShTextureTraits::SH_CLAMPED);
  }
  ShClamped(int width, int height, int depth)
    : T(width, height, depth)
  {
    m_node->traits().wrapping(ShTextureTraits::SH_CLAMPED);
  }
};

template<typename T>
class ShUnclamped : public T {
public:
  ShUnclamped(int width)
    : T(width)
  {
    m_node->traits().wrapping(ShTextureTraits::SH_UNCLAMPED);
  }
  ShUnclamped(int width, int height)
    : T(width, height)
  {
    m_node->traits().wrapping(ShTextureTraits::SH_UNCLAMPED);
  }
  ShUnclamped(int width, int height, int depth)
    : T(width, height, depth)
  {
    m_node->traits().wrapping(ShTextureTraits::SH_UNCLAMPED);
  }
};

}

#endif
