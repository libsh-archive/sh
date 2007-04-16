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
#include <sh/sh.hpp>
#ifdef __APPLE__
# include <OpenGL/gl.h>
# include <OpenGL/glext.h>
# include <OpenGL/glu.h>
# include <GLUT/glut.h>
#else
# include <GL/glut.h>
# include <GL/glext.h>
# include <GL/glu.h>
#endif
#include "Camera.hpp"
#include <iostream>
#include <cstdio>

using namespace SH;
using namespace std;

Matrix4x4f mv, mvd;
Point3f lightPos;
Camera camera;
Program vsh, fsh;
ProgramSetPtr shaders;

Color3f diffusecolor;

// Glut data
int buttons[5] = {GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP};
int cur_x, cur_y;

int gprintf(int x, int y, char* fmt, ...);

bool show_help = false;

void initShaders()
{
  vsh = SH_BEGIN_VERTEX_PROGRAM {
    InOutPosition4f pos;
    InOutNormal3f normal;
    OutputVector3f lightv;

    Point3f posv = (mv | pos)(0,1,2); // Compute viewspace position
    lightv = lightPos - posv; // Compute light direction
    
    pos = mvd | pos; // Project position
    normal = mv | normal; // Project normal
  } SH_END;

  fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    InputColor3f SH_DECL(kd); 
    InputPosition4f pos;
    InputNormal3f normal;
    InputVector3f lightv;

    OutputColor3f color;

    normal = normalize(normal);
    lightv = normalize(lightv);
    
    color = (normal | lightv) * kd; 
  } SH_END;
  fsh = fsh << diffusecolor;
  lock_in_uniforms(fsh);
  cout << fsh.describe_binding_spec() << endl;

  shaders = new ProgramSet(vsh, fsh);
}

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  bind(*shaders);

  glFrontFace(GL_CW);
  glutSolidTeapot(2.5);
  glFrontFace(GL_CCW);

  unbind(*shaders);
  
  // Help information
  if (show_help) {
    gprintf(30, 120, "Sh Glut Example Help");
    gprintf(30, 95,  "  '1' - Colour 1");
    gprintf(30, 80,  "  '2' - Colour 2");
    gprintf(30, 65,  "  '3' - Light angle 1");
    gprintf(30, 50,  "  '4' - Light angle 2");
    gprintf(30, 30,  "  'Q' - Quit");
  } else {
    gprintf(10, 10, "'H' for help...");
  }
  
  glutSwapBuffers();
}

void setupView()
{
  mv = camera.modelView();
  mvd = camera.modelViewProjection(Matrix4x4f());
}

void reshape(int width, int height)
{
  glViewport(0, 0, width, height);
  setupView();
}

void motion(int x, int y)
{
  const double factor = 20.0;
  bool changed = false;
  
  if (buttons[GLUT_LEFT_BUTTON] == GLUT_DOWN) {
    camera.orbit(cur_x, cur_y, x, y,
                 glutGet(GLUT_WINDOW_WIDTH),
                 glutGet(GLUT_WINDOW_HEIGHT));
    changed = true;
  }
  if (buttons[GLUT_MIDDLE_BUTTON] == GLUT_DOWN) {
    camera.move(0, 0, (y - cur_y)/factor);
    changed = true;
  }
  if (buttons[GLUT_RIGHT_BUTTON] == GLUT_DOWN) {
    camera.move((x - cur_x)/factor, (cur_y - y)/factor, 0);
    changed = true;
  }

  if (changed) {
    setupView();
    glutPostRedisplay();
  }

  cur_x = x;
  cur_y = y;
}

void mouse(int button, int state, int x, int y)
{
  buttons[button] = state;
  cur_x = x;
  cur_y = y;
}

void keyboard(unsigned char k, int x, int y)
{
  switch(k) {
  case '1':
    diffusecolor = Color3f(0.5, 0.1, 0.2);
    break;
  case '2':
    diffusecolor = Color3f(0.5, 0.7, 0.9);
    break;
  case '3':
    lightPos = Point3f(5.0, 5.0, 5.0);
    break;
  case '4':
    lightPos = Point3f(20.0, 20.0, 20.0);
    break;
  case 'h':
  case 'H':
    show_help = !show_help;
    break;
  case 'q':
  case 'Q':
    exit(0);
    break;
  }
  glutPostRedisplay();
}

int gprintf(int x, int y, char* fmt, ...)
{
  char temp[1024];
  va_list va;
  va_start(va, fmt);
  vsprintf(temp, fmt, va);
  va_end(va);
  
  // setup the matrices for a direct
  // screen coordinate transform when
  // using glRasterPos
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, vp[2], 0, vp[3], -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  // just in case, turn lighting and
  // texturing off and disable depth testing
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_DEPTH_TEST);

  // render the character through glut
  char* p = temp;
  glRasterPos2f(x, y);
  while(*p) {
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, (*p));
    p++;
  }
  
  // reset OpenGL to what is was
  // before we started
  glPopAttrib();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  
  return p-temp;
}

int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(512, 512);
  glutCreateWindow("Sh Example");
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutKeyboardFunc(keyboard);

  if (argc > 1) {
    useBackend(argv[1]);
  }

  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  setupView();

  // Initial values for the uniforms
  camera.move(0.0, 0.0, -15.0);
  lightPos = Point3f(5.0, 5.0, 5.0);
  diffusecolor = Color3f(0.5, 0.1, 0.2);

  try {
    initShaders();
    bind(*shaders);
  }
  catch (const std::exception &e) {
    cerr << e.what() << endl;
#ifdef _WIN32
    system("pause");
#endif
    return 1;
  }

#if 1
  cout << "Vertex Unit:" << endl;
  vsh.node()->code()->print(cout);
  cout << "--" << endl;
  cout << "Fragment Unit:" << endl;
  fsh.node()->code()->print(cout);
  cout << "--" << endl;
#endif

  glutMainLoop();
}
