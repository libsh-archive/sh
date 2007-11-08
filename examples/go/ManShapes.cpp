#include "ManOps.hpp"
#include "ManShapes.hpp"

using namespace SH;

/* Higher level shapes and shape operations */


/* A circle with n-dimensional output, t = angle in radians*/

Curve2D m_circle(const Man& t) {
  return m_combine(cos(t), sin(t))
         .name("m_circle(" + t.name()+")"); 
}

Surface3D m_sphere(const Man& u, const Man& v) {
  return m_combine(cos(u) * sin(v), sin(u) * sin(v), cos(v))
         .name("m_sphere(" + u.name() + "," + v.name() + ")"); 
}

/* Normalized 2D normal to a curve */ 
Man m_normal2d(const Man& curve) {
  Man d = normalize(m_differentiate(curve, 0));
  return m_combine(-d(1), d(0))
         .name("m_normal2d(" + curve.name() + ")"); 
}

/* Offset curve at a given radius to another curve */
Man m_offset2d(const Man& curve, const Man& radius) {
  Man normal = m_normal2d(curve);
  return (curve + radius * normal).name("m_offset2d(" + curve.name() + "," + radius.name());
}

/* Normalized 3D normal to a surface */
Man m_normal3d(const Man& surface) {
  Man d0 = m_differentiate(surface, 0);
  Man d1 = m_differentiate(surface, 1);
  Man result = normalize(cross(d0, d1));

  return result; 
}

/* A cubic bezier specified by four control points */ 
Curve2D m_cubic_bezier(const Man& t, const Man p[4]) {
  Man bt = bernstein<4, Attrib1f>() << t;
  return (bt(0) * p[0] + 
         bt(1) * p[1] + 
         bt(2) * p[2] + 
         bt(3) * p[3]).name("m_cubic_bezier(" + t.name()  + ", ...)");
}

/* A cubic bezier patch specified by 4x4 control points */

Surface3D m_cubic_bezier_patch(const Man& u,  const Man& v, const Man p[4][4]) {
  Man bu = bernstein<4, Attrib1f>() << u;
  Man bv = bernstein<4, Attrib1f>() << v;
  Man result;
  
  int i, j;
  for(i = 0; i < 4; ++i) for(j = 0; j < 4; ++j) {
    if(i == 0 && j == 0) result = bu(i) * bv(j) * p[i][j];
    else result = result + bu(i) * bv(j) * p[i][j];
  }
  return result.name("m_cubic_bezier_patch(" + u.name() + "," + v.name() + ",...)");
}


/* profile product of a cross section and 2D profile curve */

Surface3D m_profile(const Curve2D& cross_section, const Curve2D& profile) {
  Curve2D profilev = profile(m_u(1));
  return m_combine(cross_section * profilev(0), profilev(1))
         .name("m_profile(" + cross_section.name() + "," + profile.name() + ")");
}

/* offset product */ 

Surface3D m_wire(const Curve2D& cross_section, const Curve2D& wire) {
  Man n = m_normal2d(wire)(m_u(1)); 
  n.node()->dump("normal2d");
  Man xy = n * cross_section(0) + wire(m_u(1)); 
  return m_combine(xy, cross_section(1))
         .name("m_wire(" + cross_section.name() + "," + wire.name() + ")");
}

/* Concatenates manifolds in ma by uniformly splitting in the given dimension */ 

Man m_uniform_concat(std::vector<Man>& ms, int dim) { 
  int n = ms.size();
  float step = 1.0f / n; 
  Program f = SH_BEGIN_PROGRAM() {
    Variable in = ms[0].insize_var(SH_INPUT);
    Variable out = ms[0].outsize_var(SH_OUTPUT);

    Variable temp(in.node()->clone(SH_TEMP));
    for(int i = 0; i < n; ++i) {
      float start = step * i;
      float end = step * (i + 1);
      Attrib1f in_dim;
      shASN(in_dim, in(dim));
      SH_IF(in_dim >= start && 
            in_dim <= end) {
        Variable temp_dim = temp(dim);
        shASN(temp_dim, (in_dim - start) * n);
        out = ms[i](temp); 
      } SH_ENDIF;
    }
  } SH_END;
  Man result(f);
  return result.name("m_uniform_concat(...)"); 
}

Man m_add_noise(const Man& base, const Man& scale) {
  Man both = m_combine(base, scale);

  size_t bsize = base.output_size();
  size_t ssize = scale.output_size();
  int* base_swiz = new int[bsize];
  int* scale_swiz = new int[ssize];
  for(size_t i = 0; i < bsize; ++i) {
    base_swiz[i] = i;
  }

  for(size_t i = 0; i < ssize; ++i) {
    scale_swiz[i] = i + bsize; 
  }

  Program f = SH_BEGIN_PROGRAM() {
    Variable in = both.outsize_var(SH_INPUT); 
    Variable out = base.outsize_var(SH_OUTPUT);

    Variable in_base = in(Swizzle(bsize + ssize, bsize, base_swiz)); 
    Variable in_scale = in(Swizzle(bsize + ssize, ssize, scale_swiz)); 

    switch(base.output_size()) {
      case 1: { Attrib1f p; shASN(p, in_base); shMAD(p, in_scale, snoise<1>(p, false), p); shASN(out, p); break; } 
      case 2: { Attrib2f p; shASN(p, in_base); shMAD(p, in_scale, snoise<2>(p, false), p); shASN(out, p); break; } 
      case 3: { Attrib3f p; shASN(p, in_base); shMAD(p, in_scale, snoise<3>(p, false), p); shASN(out, p); break; } 
      default:
        assert(false);
    }
  } SH_END;
  Man result(f << both);

  delete[] base_swiz; 
  delete[] scale_swiz;
  return result.name("m_add_noise(" + base.name() + "," + scale.name() + ")"); 
}

Man m_rotatex(const Man& m, const Man& angle) {
  Man mf = m_resize_inputs(m, angle).fill(3); 
  Man anglef  = m_resize_inputs(angle, m); 
  Program p = SH_BEGIN_PROGRAM() {
    Variable in_m = mf.outsize_var(SH_INPUT);
    Variable in_angle = angle.outsize_var(SH_INPUT);
    Variable out = mf.outsize_var(SH_OUTPUT); 
    Attrib1f c, s; 
    shCOS(c, in_angle);
    shSIN(s, in_angle);
    shASN(out, in_m);
    Variable out1(out(1));
    Variable out2(out(2));
    shMUL(out1, c, in_m(1));
    shMAD(out1, s, in_m(2), out1);
    shMUL(out2, -s, in_m(1));
    shMAD(out2, c, in_m(2), out2);
  } SH_END;
  Man result = p << (mf & anglef) << SH::dup(); 
  return result.name("m_rotatex(" + m.name() + "," + angle.name() + ")");
}

Man m_rotatey(const Man& m, const Man& angle) {
  Man mf = m_resize_inputs(m, angle).fill(3); 
  Man anglef  = m_resize_inputs(angle, m); 
  Program p = SH_BEGIN_PROGRAM() {
    Variable in_m = mf.outsize_var(SH_INPUT);
    Variable in_angle = angle.outsize_var(SH_INPUT);
    Variable out = mf.outsize_var(SH_OUTPUT); 
    Attrib1f c, s; 
    shCOS(c, in_angle);
    shSIN(s, in_angle);
    shASN(out, in_m);
    Variable out0(out(0));
    Variable out2(out(2));
    shMUL(out0, c, in_m(0));
    shMAD(out0, -s, in_m(2), out0);
    shMUL(out2, s, in_m(0));
    shMAD(out2, c, in_m(2), out2);
  } SH_END;
  Man result = p << (mf & anglef) << SH::dup(); 
  return result.name("m_rotatey(" + m.name() + "," + angle.name() + ")");
}

Man m_rotatez(const Man& m, const Man& angle) {
  Man mf = m_resize_inputs(m, angle);
  if(mf.output_size() < 2) mf = mf.fill(2);
  Man anglef  = m_resize_inputs(angle, m); 
  Program p = SH_BEGIN_PROGRAM() {
    Variable in_m = mf.outsize_var(SH_INPUT);
    Variable in_angle = angle.outsize_var(SH_INPUT);
    Variable out = mf.outsize_var(SH_OUTPUT); 
    Attrib1f c, s; 
    shCOS(c, in_angle);
    shSIN(s, in_angle);
    shASN(out, in_m);
    Variable out0(out(0));
    Variable out1(out(1));
    shMUL(out0, c, in_m(0));
    shMAD(out0, s, in_m(1), out0);
    shMUL(out1, -s, in_m(0));
    shMAD(out1, c, in_m(1), out1);
  } SH_END;
  Man result = p << (mf & anglef) << SH::dup(); 
  return result.name("m_rotatez(" + m.name() + "," + angle.name() + ")");
}

Man m_translatex(const Man& m, const Man& t) {
  Man mf = m_resize_inputs(m, t);
  Man tf = m_resize_inputs(t, m);
  Program p = SH_BEGIN_PROGRAM() {
    Variable mval = mf.outsize_var(SH_INOUT);
    Variable in_t = tf.outsize_var(SH_INPUT);
    Variable mval0 = mval(0);
    shADD(mval0, mval(0), in_t);
  } SH_END;
  Man result = p << (mf & tf) << SH::dup(); 
  return result.name("m_translatex(" + m.name() + "," + t.name() + ")");
}

Man m_translatey(const Man& m, const Man& t) {
  Man mf = m_resize_inputs(m, t);
  if(mf.output_size() < 2) mf = mf.fill(2);
  Man tf = m_resize_inputs(t, m);
  Program p = SH_BEGIN_PROGRAM() {
    Variable mval = mf.outsize_var(SH_INOUT);
    Variable in_t = tf.outsize_var(SH_INPUT);
    Variable mval1 = mval(1);
    shADD(mval1, mval(1), in_t);
  } SH_END;
  Man result = p << (mf & tf) << SH::dup(); 
  return result.name("m_translatey(" + m.name() + "," + t.name() + ")");
}

Man m_translatez(const Man& m, const Man& t) {
  Man mf = m_resize_inputs(m, t);
  if(mf.output_size() < 3) mf = mf.fill(3);
  Man tf = m_resize_inputs(t, m);
  Program p = SH_BEGIN_PROGRAM() {
    Variable mval = mf.outsize_var(SH_INOUT);
    Variable in_t = tf.outsize_var(SH_INPUT);
    Variable mval2 = mval(2);
    shADD(mval2, mval(2), in_t);
  } SH_END;
  Man result = p << (mf & tf) << SH::dup(); 
  return result.name("m_translatez(" + m.name() + "," + t.name() + ")");
}
