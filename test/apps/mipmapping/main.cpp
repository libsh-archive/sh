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
#ifdef WIN32
#include <windows.h>
#endif /* WIN32 */

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include <sh/sh.hpp>
#include <iostream>

using namespace SH;
using namespace std;

const int MIPMAP_LEVELS = 10;

Texture2D<Color3fub> kd(512, 512);

Program vsh;
Program fsh;

// Animation data
float zoom = 1.0;

void display()
{
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  // setup the model view matrix with an initial
  // translate such that the dodecahedron will be
  // visible
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0, 0, -10);
  
  // turn off vertex and fragment programs, this
  // effectively turns off Sh
  unbind(vsh);
  unbind(fsh);

  // push modelview matrix and load the rotation
  // for the point light
  glPushMatrix();

  // set the light position using OpenGL
  float pos[] = {0, 0, 3, 1};
  glLightfv(GL_LIGHT0, GL_POSITION, pos);

  // draw the light source as a point (using
  // standard OpenGL shading since Sh is "off")
  glBegin(GL_POINTS);
  glColor3f(1, 1, 1);
  glVertex3fv(pos);
  glEnd();

  // restore the modelview matrix
  glPopMatrix();
  
  // turn vertex and fragment programs back on 
  // bind programs
  bind(vsh);
  bind(fsh);

  // setup the modelview matrix with the rotation
  // for the dodecahedron and draw it
  glPushMatrix();

  glScalef(zoom, zoom, zoom);

#if 1
  glBegin(GL_QUADS);
  glNormal3f(0, 1, 0);
  glTexCoord2f(0.0, 0.0);
  glVertex3f(-5, -5, 0);
  glTexCoord2f(1.0, 0.0);
  glVertex3f( 5, -5, 0);
  glTexCoord2f(1.0, 1.0);
  glVertex3f( 5, 5,  0);
  glTexCoord2f(0.0, 1.0);
  glVertex3f(-5, 5,  0);
  glEnd();
#else
  glutSolidTeapot(1.0);
#endif

  glPopMatrix();

  unbind();
  
  glutSwapBuffers();
}

void reshape(int width, int height)
{
  glViewport(0, 0, width, height);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, (float)width/(float)height, .1, 100);
}

void idle(void)
{
  // while idling zoom in
  zoom *= 0.995;
  glutPostRedisplay();
}

void keyboard(unsigned char k, int x, int y)
{
  switch(k) {
  case 'q':
    exit(0);
    break;
  case 'r':
    zoom = 1.0;
    break;
  case ' ':
    {
      // toggle animation
      static bool idling = false;
      if (idling) glutIdleFunc(NULL);
      else glutIdleFunc(idle);
      idling = !idling;
      break;
    }
  }

  glutPostRedisplay();
}

void init_gl(void)
{
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glPointSize(5);
}

void init_sh()
{
  // set OpenGL backend
  setBackend("arb");

  // Sh data
  Matrix4x4f mv;
  Matrix4x4f mvp;
  Point3f lightp;
  Color3f diffuse;
  Color3f ambient;

  for (int i=0; i < MIPMAP_LEVELS; i++) {
    int size = 512 >> i;
    HostMemoryPtr memory = new HostMemory(3 * size * size * sizeof(unsigned char), SH_FUBYTE);
    
    unsigned char * data = reinterpret_cast<unsigned char*>(memory->hostStorage()->data());

    for (int x = 0; x < size; x++) {
      for (int y = 0; y < size; y++) {
	data[3 * (y * size + x)] = ((x % 2 == 0 && y % 2 == 0) ? 255 : 0);
	data[3 * (y * size + x) + 1] = (i % 3 == 1 ? 255 : 0);
	data[3 * (y * size + x) + 2] = (i % 3 == 2 ? 255 : 0);
      }
    }

    kd.memory(memory, i);
  }

  // setup OpenGL bindings
  mv.meta("opengl:state", "state.matrix.modelview");
  mv.meta("opengl:readonly", "true");
  mvp.meta("opengl:state", "state.matrix.mvp");
  mvp.meta("opengl:readonly", "true");
  lightp.meta("opengl:state", "state.light[0].position");
  lightp.meta("opengl:readonly", "true");
  diffuse.meta("opengl:state", "state.material.diffuse");
  diffuse.meta("opengl:readonly", "true");
  ambient.meta("opengl:state", "state.material.ambient");
  ambient.meta("opengl:readonly", "true");

  // construct vertex program
  vsh = SH_BEGIN_VERTEX_PROGRAM {
    InOutTexCoord2f u;
    InOutPosition4f pos;
    InOutNormal3f normal;
    OutputVector3f lightv;

    OutputPoint3f posv = (mv | pos)(0,1,2); // Compute viewspace position
    lightv = lightp - posv; // Compute light direction
    
    pos = mvp | pos; // Project position
    normal = mv | normal; // Project normal
  } SH_END;

  Color3f DECL(diffusecolor) = Color3f(0.2, 0.2, 0.2);
  Attrib1f exponent(30.0);

  // construct fragment program
  fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    InputTexCoord2f u;
    InputPosition4f position;
    InputNormal3f normal;
    InputVector3f lightv;
    InputPoint3f posv;

    OutputColor3f color;

    color = kd(u);
  } SH_END;

}

int main(int argc, char** argv)
{
  try {
    // initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
    glutInitWindowSize(512, 512);
    glutCreateWindow("Sh Binding Example");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    Context::current()->throw_errors(false);
  
    // initialize OpenGL
    init_gl();

    // initialize Sh
    init_sh();
  
    glutMainLoop();

  } catch (const Exception& e) {
    std::cerr << "Sh error: " << e.message() << std::endl;
    return 1;
  } catch (const std::exception& e) {
    std::cerr << "C++ error: " << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Unknown error." << std::endl;
    throw;
    return 1;
  }
}
