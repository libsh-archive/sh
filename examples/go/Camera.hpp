// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
//////////////////////////////////////////////////////////////////////////////
#ifndef SHCAMERA_HPP
#define SHCAMERA_HPP

#include <iostream>

#include <sh/sh.hpp>

class Camera {
public:
  Camera();

  void reset();
  void move(float x, float y, float z);
  void rotate(float a, float x, float y, float z);
  void orbit(int sx, int sy, int x, int y, int w, int h);
  
  void glModelView();
  void glProjection(float aspect);
  void glOrtho(float width, float height);

  SH::Matrix4x4f modelView();
  SH::Matrix4x4f modelViewProjection(SH::Matrix4x4f viewport);

private:
  SH::Matrix4x4f perspective(float fov, float aspect, float znear, float zfar);
  SH::Matrix4x4f ortho(float width, float height, float znear, float zfar);

  SH::Matrix4x4f proj;
  SH::Matrix4x4f rots;
  SH::Matrix4x4f trans;

  friend std::ostream &operator<<(std::ostream &output, Camera &camera);
  friend std::istream &operator>>(std::istream &input, Camera &camera);
};

#endif
