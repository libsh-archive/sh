#ifndef SHINTERP_HPP
#define SHINTERP_HPP

namespace SH {

/** Set Interpolation level in Texture type.
 * Use this template to indicate that a texture should be interpolated
 * to a particular level L.
 * For example, ShInterp<0, T> implies a nearest-neighbour lookup,
 * whereas ShInterp<1, T> implies linear interpolation.
 */
template<int L, typename T>
class ShInterp : public T {
public:
  static int level() {
    if (L >= 2) return 3; else return L;
  }
  
  ShInterp()
    : T()
  {
    m_node->traits().interpolation(level());
  }
  ShInterp(int width)
    : T(width)
  {
    m_node->traits().interpolation(level());
  }
  ShInterp(int width, int height)
    : T(width, height)
  {
    m_node->traits().interpolation(level());
  }
  ShInterp(int width, int height, int depth)
    : T(width, height, depth)
  {
    m_node->traits().interpolation(level());
  }

  typedef typename T::return_type return_type;
  
};

}

#endif
