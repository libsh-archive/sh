#ifndef MAN_SHAPES_HPP
#define MAN_SHAPES_HPP

/* Higher level shapes and shape operations */


/* A circle with n-dimensional output, t = angle in radians*/
template<int I1>
Man<I1, 2> m_circle(const Man<I1, 1>& t) {
  return m_combine(cos(t), sin(t)); 
}

template<int I1, int I2>
Man<IntOp<I1, I2>::max, 3> m_sphere(const Man<I1, 1>& u, const Man<I2, 1>& v) {
  return m_combine(cos(u) * sin(v), sin(u) * sin(v), cos(v)); 
}

/* Normalized 2D normal to a curve */ 
inline Man<1, 2> m_normal2d(const Man<1, 2>& curve) {
  Man<1, 2> d = normalize(m_differentiate(curve, 0));
  return m_combine(-d(1), d(0)); 
}

/* Normalized 3D normal to a surface */
inline Man<2, 3> m_normal3d(const Man<2, 3>& surface) {
  return normalize(cross(m_differentiate(surface, 0), m_differentiate(surface, 1)));
}

/* A line between two points */
template<int I1, int I2, int I3, int O>
Man<IntOp<I1, I2, I3>::max, O> m_line(const Man<I1, 1>& t, const Man<I2, O>& start, const Man<I3, O>& end) {
  return m_lerp(t, end, start); 
}

/* A cubic bezier specified by four control points */ 
template<int I1, int I2, int O>
Man<IntOp<I1, I2>::max, O> m_cubic_bezier(const Man<I1, 1>& t, const Man<I2, O> p[4]) {
  Man<I1, 4> bt = SH::bernstein<4, SH::Attrib1f>() << t;
  return bt(0) * p[0] + 
         bt(1) * p[1] + 
         bt(2) * p[2] + 
         bt(3) * p[3];
}

/* A cubic bezier patch specified by 4x4 control points */
template<int I1, int I2, int I3, int O>
Man<IntOp<I1, I2, I3>::max, O> m_cubic_bezier_patch(const Man<I1, 1>& u,  const Man<I2, 1>& v, const Man<I3, O> p[4][4]) {
  Man<I1, 4> bu = SH::bernstein<4, SH::Attrib1f>() << u;
  Man<I1, 4> bv = SH::bernstein<4, SH::Attrib1f>() << v;
  Man<IntOp<I1, I2, I3>::max, O> result;
  
  int i, j;
  for(i = 0; i < 4; ++i) for(j = 0; j < 4; ++j) {
    if(i == 0 && j == 0) result = bu(i) * bv(j) * p[i][j];
    else result = result + bu(i) * bv(j) * p[i][j];
  }
  return result;
}

/* combining shapes */

/* lerp between two values (called m_interp in GENMOD) */    
template<int I1, int I2, int I3, int O1, int O2, int O3>
Man<IntOp<I1, I2, I3>::max, IntOp<O1, O2, O3>::max> 
m_lerp(const Man<I1, O1>& t, const Man<I2, O2>& a, const Man<I3, O3>& b) {
  return t * a + (1.0f - t) * b; 
}


/* profile product of a cross section and 2D profile curve */
template<int I1, int I2, int O1>
Man<IntOp<I1, I2>::max, O1 + 1>
m_profile(const Man<I1, O1>& cross_section, const Man<I2, 2>& profile) {
  std::cout << "m_profile "
            << " I1=" << I1 
            << " I2=" << I2 
            << " O1=" << O1 << std::endl;
  return m_combine(cross_section * profile(0), profile(1));
}

/* offset product */ 
template<int I>
Man<I, 3> 
m_offset_prod(const Man<1, 2>& cross_section, const Man<I, 2>& profile) {
  Man<1, 2> n = m_normal2d(cross_section); 
  Man<I, 2> xy = cross_section + profile(0) * n;
  return m_combine(xy, profile(1));
}

/* Concatenates manifolds in ma by uniformly splitting in the given dimension */ 
template<int I, int O>
Man<I, O> m_uniform_concat(std::vector<Man<I, O> >& ms, int dim) { 
  int n = ms.size();
  float step = 1.0f / n; 
  SH::Program f = SH_BEGIN_PROGRAM() {
    typename Man<I, O>::InType in;
    typename Man<I, O>::OutType out;

    SH::Attrib<I, SH::SH_TEMP, float, SH::SH_ATTRIB> temp = in;
    for(int i = 0; i < n; ++i) {
      float start = step * i;
      float end = step * (i + 1);
      SH_IF(in[dim] >= start && 
            in[dim] <= end) {
        temp[dim] = (in[dim] - start) * n;
        out = ms[i](temp); 
      } SH_ENDIF;
    }
  } SH_END;
  return f;
}

#endif
