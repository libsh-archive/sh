#ifndef SHWRAP_HPP
#define SHWRAP_HPP

namespace SH {

template<typename T>
class ShWrapClamp : public T {
public:
  ShWrapClamp(int width)
    : T(width)
  {
    m_node->traits((m_node->traits() & ~SH_WRAP_MASK) | SH_WRAP_CLAMP);
  }
  ShWrapClamp(int width, int height)
    : T(width, height)
  {
    m_node->traits((m_node->traits() & ~SH_WRAP_MASK) | SH_WRAP_CLAMP);
  }
  ShWrapClamp(int width, int height, int depth)
    : T(width, height, depth)
  {
    m_node->traits((m_node->traits() & ~SH_WRAP_MASK) | SH_WRAP_CLAMP);
  }
};

template<typename T>
class ShWrapRepeat : public T {
public:
  ShWrapRepeat(int width)
    : T(width)
  {
    m_node->traits((m_node->traits() & ~SH_WRAP_MASK) | SH_WRAP_REPEAT);
  }
  ShWrapRepeat(int width, int height)
    : T(width, height)
  {
    m_node->traits((m_node->traits() & ~SH_WRAP_MASK) | SH_WRAP_REPEAT);
  }
  ShWrapRepeat(int width, int height, int depth)
    : T(width, height, depth)
  {
    m_node->traits((m_node->traits() & ~SH_WRAP_MASK) | SH_WRAP_REPEAT);
  }
};

}

#endif
