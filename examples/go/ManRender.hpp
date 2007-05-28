#ifndef MAN_RENDER_HPP
#define MAN_RENDER_HPP

#include <vector>
#include "Man.hpp"

/* Functions useful for rendering */

/* tesselation - should use streams but this is okay for now */ 

/* tesselate a N-d curve m, splitting parameter in n uniform parts */
typedef std::vector<SH::Generic<2, float> > TessCurve2D; 
typedef std::vector<SH::Generic<3, float> > TessCurve3D; 

template<int N>
std::vector<SH::Generic<N, float> > m_uniform_tess_crv(const Man& m, int n) {
  assert(m.size_matches(1, N));
  Man mf = m.resize_fill(1, N);
  std::vector<SH::Generic<N, float> > result;
  float step = 1.0f / n;
  for(int i = 0; i <= n; ++i) {
    SH::Attrib<N, SH::SH_TEMP, float, SH::SH_ATTRIB> resulti;
    resulti = mf(SH::ConstAttrib1f(i * step));
    result.push_back(resulti);
  }
  return result;
}

/* tesselates a 1D function y = m(u), x = u */
template<int N>
std::vector<SH::Generic<N, float> > m_uniform_plot_fun(const Man& m, int n) {
  assert(m.size_matches(1, 1) && N >= 2);
  Man mf = m.resize_fill(1, 1);
  std::vector<SH::Generic<N, float> > result;
  float step = 1.0f / n;
  for(int i = 0; i <= n; ++i) {
    SH::Attrib<N, SH::SH_TEMP, float, SH::SH_ATTRIB> resulti;
    resulti *= 0.0f; 
    resulti(0) = i * step;
    resulti(1) = mf(resulti(0));
    result.push_back(resulti);
  }
  return result;
}

/* tesselate a surface m, splitting parameters in n uniform parts per dimension */
typedef std::vector<TessCurve3D> TessSurface3D;
template<int N>
std::vector<std::vector<SH::Generic<N, float> > > m_uniform_tess_surface(const Man& m, int n) {
  assert(m.size_matches(2, N));
  Man mf = m.resize_fill(2, N);

  std::vector<std::vector<SH::Generic<N, float> > > result;
  float step = 1.0f / n;
  for(int i = 0; i <= n; ++i) {
    result.push_back(std::vector<SH::Generic<N, float> >());
    for(int j = 0; j <= n; ++j) {
      SH::Attrib<N, SH::SH_TEMP, float, SH::SH_ATTRIB> resultij;
      resultij = mf(SH::ConstAttrib2f(i * step, j * step));
      result.back().push_back(resultij);
    }
  }
  return result;
}

template<int N>
std::vector<std::vector<SH::Generic<N, float> > > m_uniform_plot_fun2d(const Man& m, int n) {
  assert(m.size_matches(2, 1));
  Man mf = m.resize_fill(2, 1);

  std::vector<std::vector<SH::Generic<N, float> > > result;
  float step = 1.0f / n;
  for(int i = 0; i <= n; ++i) {
    result.push_back(std::vector<SH::Generic<N, float> >());
    for(int j = 0; j <= n; ++j) {
      SH::Attrib<N, SH::SH_TEMP, float, SH::SH_ATTRIB> resultij;
      resultij *= 0.0f; 
      resultij(0) = i * step;
      resultij(1) = j * step;
      resultij(2) = mf(resultij(0,1));
      result.back().push_back(resultij);
    }
  }
  return result;
}

#endif
