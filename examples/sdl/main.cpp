// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#include <sh/sh.hpp>
#include <sh/shutil.hpp>
#ifdef __APPLE__
#else
#include <GL/gl.h>
#include <GL/glut.h>
#include <SDL.h>
#endif
#include "Camera.hpp"
#include <iostream>

using namespace SH;
using namespace std;

ShMatrix4x4f mv, mvd;
ShPoint3f lightPos;
Camera camera;
ShProgram vsh, fsh;
ShProgramSet* shaders;
ShColor3f diffusecolor;

ShColor3f color1 = ShColor3f(0.5, 0.7, 0.9);
ShColor3f color2 = ShColor3f(0.0, 0.9, 0.3);
ShPoint3f light1 = ShPoint3f(5.0, 5.0, 5.0);
ShPoint3f light2 = ShPoint3f(-5.0, 5.0, 5.0);

void initShaders()
{
  vsh = SH_BEGIN_VERTEX_PROGRAM {
    ShInOutPosition4f pos;
    ShInOutNormal3f normal;
    ShOutputVector3f lightv;

    ShPoint3f posv = (mv | pos)(0,1,2); // Compute viewspace position
    lightv = lightPos - posv; // Compute light direction
    
    pos = mvd | pos; // Project position
    normal = mv | normal; // Project normal
  } SH_END;

  fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputNormal3f normal;
    ShInputVector3f lightv;

    ShOutputColor3f color;

    normal = normalize(normal);
    lightv = normalize(lightv);
    
    color = (normal | lightv) * diffusecolor;
  } SH_END;

  shaders = new ShProgramSet(vsh, fsh);
}

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shBind(*shaders);

  glFrontFace(GL_CW);
  glutSolidTeapot(2.5);
  glFrontFace(GL_CCW);

  shUnbind(*shaders);
  
  SDL_GL_SwapBuffers();
}

void setupView()
{
  mv = camera.shModelView();
  mvd = camera.shModelViewProjection(ShMatrix4x4f());
}

void reshape(int w, int h)
{
  const SDL_VideoInfo *video = SDL_GetVideoInfo();
  SDL_SetVideoMode(w, h, video->vfmt->BitsPerPixel, SDL_OPENGL|SDL_RESIZABLE);
  glViewport(0, 0, w, h);
  setupView();
}

void motion(const SDL_MouseMotionEvent &event)
{
  const double factor = 20.0;
  bool changed = false;
  
  if (event.state & SDL_BUTTON(1)) {
    camera.orbit(event.x - event.xrel,
                 event.y - event.yrel,
                 event.x, event.y,
                 SDL_GetVideoSurface()->w,
                 SDL_GetVideoSurface()->h);
    changed = true;
  }
  if (event.state & SDL_BUTTON(2)) {
    camera.move(0, 0, event.yrel/factor);
    changed = true;
  }
  if (event.state & SDL_BUTTON(3)) {
    camera.move(event.xrel/factor, -event.yrel/factor, 0);
    changed = true;
  }

  if (changed) {
    setupView();
  }
}

void keyboard(const SDL_KeyboardEvent &event)
{
  switch(event.keysym.sym) {
  case SDLK_1:
    diffusecolor = color1;
    break;
  case SDLK_2:
    diffusecolor = color2;
    break;
  case SDLK_3:
    lightPos = light1;
    break;
  case SDLK_4:
    lightPos = light2;
    break;
  default:
    break;
  }
}

int main(int argc, char* argv[])
{
  glutInit(&argc, argv);
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    cerr << "Couldn't initialize SDL video: " << SDL_GetError() << endl;
    return -1;  
  }
  
  SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
  SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
  SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

  reshape(512, 512);

  SDL_WM_SetCaption("Sh SDL Example", NULL);
  
  if (argc > 1) {
    shUseBackend(argv[1]);
  }

  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0, 0.0, 0.0, 1.0);

  // Initial values for the uniforms
  camera.move(0.0, 0.0, -15.0);
  lightPos = light1;
  diffusecolor = color1;

  initShaders();

  shBind(*shaders);

#if 0
  cout << "Vertex Unit:" << endl;
  vsh.node()->code()->print(cout);
  cout << "--" << endl;
  cout << "Fragment Unit:" << endl;
  fsh.node()->code()->print(cout);
  cout << "--" << endl;
#endif
  cout << "Sh SDL Example Help" << endl;
  cout << "  '1' - Colour 1" << endl;
  cout << "  '2' - Colour 2" << endl;
  cout << "  '3' - Light angle 1" << endl;
  cout << "  '4' - Light angle 2" << endl;
  cout << "  'q' - Quit" << endl;

  reshape(512, 512);
  
  bool quit = false;
  SDL_Event event;
  while (!quit) {
    display();
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_KEYDOWN:
          if ((event.key.keysym.sym == SDLK_ESCAPE)||
            (event.key.keysym.sym == SDLK_q)) {
            quit = true;
            break;
          }
          else
            keyboard(event.key);
          break;

        case SDL_MOUSEMOTION:
          motion(event.motion);
          break;

        case SDL_QUIT:
          quit = true;
          break;

        case SDL_VIDEORESIZE:
          reshape(event.resize.w, event.resize.h);
          break;
      }
    }
  }

  delete shaders;
}
