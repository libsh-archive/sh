#ifdef WIN32
#include <windows.h>
#endif /* WIN32 */

#include <GL/gl.h>

#include "Camera.hpp"
#include "ShTrackball.hpp"

using namespace SH;

Camera::Camera()
{
  proj = perspective(45, 1, .1, 100);
}

void printMatrix(std::ostream& out, const ShMatrix4x4f& mat)
{
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      float v;
      mat[i](j).getValues(&v);
      out << v << (j == 3 ? '\n' : ' ');
    }
  }
}

std::ostream &operator<<(std::ostream &out, Camera &camera)
{
  printMatrix(out, camera.rots);
  printMatrix(out, camera.trans);
  return out;
}

std::istream &operator>>(std::istream &in, Camera &camera)
{
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      // TODO
      //      in >> camera.rots[i](j);
      //      in >> camera.trans[i](j);
    }
  }
  return in;
}

void Camera::glModelView()
{
  float values[16];
  for (int i = 0; i < 16; i++) trans[i%4](i/4).getValues(&values[i]);
  glMultMatrixf(values);
  for (int i = 0; i < 16; i++) rots[i%4](i/4).getValues(&values[i]);
  glMultMatrixf(values);
}

void Camera::glProjection(float aspect)
{
  proj = perspective(45, aspect, .1, 100);
  float values[16];
  for (int i = 0; i < 16; i++) proj[i%4](i/4).getValues(&values[i]);
  glMultMatrixf(values);
}

ShMatrix4x4f Camera::shModelView()
{
  return (trans | rots);
}

ShMatrix4x4f Camera::shModelViewProjection(ShMatrix4x4f viewport)
{
  return (viewport | (proj | (trans | rots)));
}

void Camera::reset()
{
  rots = ShMatrix4x4f();
  trans = ShMatrix4x4f();
}

void Camera::move(float x, float y, float z)
{
  ShMatrix4x4f m;
  m[0](3) = x;
  m[1](3) = y;
  m[2](3) = z;

  trans = (m | trans);
}

void Camera::rotate(float a, float x, float y, float z)
{
  float cosa = cos((M_PI/180)*a);
  float sina = sin((M_PI/180)*a);
  ShMatrix4x4f m;
    
  m[0](0) = x*x*(1-cosa) +   cosa;
  m[0](1) = x*y*(1-cosa) - z*sina;
  m[0](2) = x*z*(1-cosa) + y*sina;
      
  m[1](0) = y*x*(1-cosa) + z*sina;
  m[1](1) = y*y*(1-cosa) +   cosa;
  m[1](2) = y*z*(1-cosa) - x*sina;
      
  m[2](0) = z*x*(1-cosa) - y*sina;
  m[2](1) = z*y*(1-cosa) + x*sina;
  m[2](2) = z*z*(1-cosa) +   cosa;

  rots = (m | rots);
}

void Camera::orbit(int sx, int sy, int x, int y, int w, int h)
{
  ShTrackball t;
  t.resize(w, h);
  rots = (t.rotate(sx, sy, x, y) | rots);
}

ShMatrix4x4f Camera::perspective(float fov, float aspect, float znear, float zfar)
{
  float zmin = znear;
  float zmax = zfar;
  float ymax = zmin*tan(fov*(M_PI/360));
  float ymin = -ymax;
  float xmin = ymin*aspect;
  float xmax = ymax*aspect;

  ShMatrix4x4f ret;

  ret[0](0) = 2.0*zmin/(xmax-xmin);
  ret[0](1) = 0.0;
  ret[0](2) = 0.0;
  ret[0](3) = 0.0;

  ret[1](0) = 0.0;
  ret[1](1) = 2.0*zmin/(ymax-ymin);
  ret[1](2) = 0.0;
  ret[1](3) = 0.0;

  ret[2](0) = 0.0;
  ret[2](1) = 0.0;
  ret[2](2) = -(zmax+zmin)/(zmax-zmin);
  ret[2](3) = -2.0*zmax*zmin/(zmax-zmin);

  ret[3](0) = 0.0;
  ret[3](1) = 0.0;
  ret[3](2) = -1.0;
  ret[3](3) = 0.0;

  return ret;
}
