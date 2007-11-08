#ifndef MAN_SHAPES_HPP
#define MAN_SHAPES_HPP

/* Higher level shapes and shape operations */
#include "Man.hpp"

/* 2D circle curve*/
Curve2D m_circle(const Man& t); 

/* 3D sphere surface */
Surface3D m_sphere(const Man& u, const Man& v); 

/* Normalized 2D normal to a curve */ 
Man m_normal2d(const Man& curve); 

/* Normalized 2D normal to a curve */ 
Man m_offset2d(const Man& curve, const Man& radius); 

/* Normalized 3D normal to a surface */
Man m_normal3d(const Man& surface); 

/* A cubic bezier specified by four control points */ 
Curve2D m_cubic_bezier(const Man& t, const Man p[4]); 

/* A cubic bezier patch specified by 4x4 control points */
Surface3D m_cubic_bezier_patch(const Man& u,  const Man& v, const Man p[4][4]); 

/* profile product of a cross section and 2D profile curve */
Surface3D m_profile(const Curve2D& cross_section, const Curve2D& profile); 

/* wire product */ 
Surface3D m_wire(const Curve2D& cross_section, const Curve2D& wire); 

/* Concatenates manifolds in ma by uniformly splitting in the given dimension */ 
Man m_uniform_concat(std::vector<Man>& ms, int dim); 

/* Adds noise
 * p = p + scale * snoise(p), where snoise is signed perlin noise */ 
Man m_add_noise(const Man& base, const Man& scale);

/* translation, rotation ops that don't require using ManMatrix */ 
Man m_rotatex(const Man& m, const Man& angle);
Man m_rotatey(const Man& m, const Man& angle);
Man m_rotatez(const Man& m, const Man& angle);
Man m_translatex(const Man& m, const Man& t);
Man m_translatey(const Man& m, const Man& t);
Man m_translatez(const Man& m, const Man& t);

#endif
