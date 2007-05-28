#ifndef MAN_RENDER_HPP
#define MAN_RENDER_HPP

#include <vector>

/* Functions useful for rendering */

/* tesselation - should use streams but this is okay for now */ 

/* tesselate a curve m, splitting parameter in n uniform parts */
template<int O>
std::vector<SH::Generic<O, float> > m_uniform_tess(const Man<1, O>& m, int n) {
  std::vector<SH::Generic<O, float> > result;
  float step = 1.0f / n;
  for(int i = 0; i <= n; ++i) {
    SH::Attrib<O, SH::SH_TEMP, float, SH::SH_ATTRIB> resulti;
    resulti = m(SH::ConstAttrib1f(i * step));
    result.push_back(resulti);
  }
  return result;
}
typedef std::vector<SH::Generic<2, float> > TessCurve2D; 
typedef std::vector<SH::Generic<3, float> > TessCurve3D; 

/* tesselate a surface m, splitting parameters in n uniform parts per dimension */
template<int O>
std::vector<std::vector<SH::Generic<O, float> > > m_uniform_tess(const Man<2, O>& m, int n) {
  std::vector<std::vector<SH::Generic<O, float> > > result;
  float step = 1.0f / n;
  for(int i = 0; i <= n; ++i) {
    result.push_back(std::vector<SH::Generic<O, float> >());
    for(int j = 0; j <= n; ++j) {
      SH::Attrib<O, SH::SH_TEMP, float, SH::SH_ATTRIB> resultij;
      resultij = m(SH::ConstAttrib2f(i * step, j * step));
      result.back().push_back(resultij);
    }
  }
  return result;
}
typedef std::vector<TessCurve3D> TessSurface3D;

#endif
