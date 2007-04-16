#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <iostream>

#include <sh/sh.hpp>

class Camera {
public:
  Camera();

  void move(float x, float y, float z);
  void rotate(float a, float x, float y, float z);
  void orbit(int sx, int sy, int x, int y, int w, int h);
  
  void glModelView();
  void glProjection(float aspect);

  SH::Matrix4x4f modelView();
  SH::Matrix4x4f modelViewProjection(SH::Matrix4x4f viewport);

private:
  SH::Matrix4x4f perspective(float fov, float aspect, float znear, float zfar);

  SH::Matrix4x4f proj;
  SH::Matrix4x4f rots;
  SH::Matrix4x4f trans;

  friend std::ostream &operator<<(std::ostream &output, Camera &camera);
  friend std::istream &operator>>(std::istream &input, Camera &camera);
};

#endif
