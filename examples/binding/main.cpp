// -*- C++ -*-
#ifdef WIN32
#include <windows.h>
#endif /* WIN32 */

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <sh/sh.hpp>

using namespace SH;

int gprintf(int x, int y, char* fmt, ...);

// Animation data
float angle = 0;

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
  glDisable(GL_VERTEX_PROGRAM_ARB);
  glDisable(GL_FRAGMENT_PROGRAM_ARB);

  // push modelview matrix and load the rotation
  // for the point light
  glPushMatrix();
  glRotatef(angle, 1, 0, 0);
  glRotatef(angle, 0, 1, 0);

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
  glEnable(GL_VERTEX_PROGRAM_ARB);
  glEnable(GL_FRAGMENT_PROGRAM_ARB);

  // setup the modelview matrix with the rotation
  // for the dodecahedron and draw it
  glPushMatrix();
  glRotatef(angle, .5, .5, .5);
  glutSolidDodecahedron();
  glPopMatrix();

  gprintf(10, 10, "Space - Toggle animation");
  gprintf(10, 24, "    c - Change color");

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
  // while idling advance the rotation angle 
  angle += .1;
  glutPostRedisplay();
  }

void keyboard(unsigned char k, int x, int y)
  {
  switch(k)
    {
    case 'q':
      // quit
      exit(0);
      break;
    case 'c':
      // change the color using the standard
      // OpenGL material model
      float col[4];
      col[0] = 0.4*((float)rand()/(RAND_MAX-1));
      col[1] = 0.4*((float)rand()/(RAND_MAX-1));
      col[2] = 0.4*((float)rand()/(RAND_MAX-1));
      col[3] = 1;
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, col);
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
  shSetBackend("arb");

  // Sh data
  ShMatrix4x4f mv;
  ShMatrix4x4f mvp;
  ShPoint3f lightp;
  ShColor3f diffuse;
  ShColor3f ambient;

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
  ShProgram vsh = SH_BEGIN_VERTEX_PROGRAM {
    ShInputPosition4f ipos;
    ShInputNormal3f inrm;

    ShOutputPosition4f opos;
    ShOutputNormal3f onrm;
    ShOutputVector3f olightv;

    // transform position and normal and
    // generate the light vector
    opos = mvp|ipos;
    onrm = mv|inrm;
    olightv = lightp - (mv|ipos)(0, 1, 2);
  } SH_END;

  // construct fragment program
  ShProgram fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f ipos;
    ShInputNormal3f inrm;
    ShInputVector3f ilightv;

    ShOutputColor3f oclr;

    inrm = normalize(inrm);
    ilightv = normalize(ilightv);
    
    // standard diffuse shading
    oclr = pos(inrm|ilightv)*diffuse + ambient;
  } SH_END;

  // bind programs
  shBind(vsh);
  shBind(fsh);
  }

int main(int argc, char** argv)
  {
  // initialize GLUT
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(512, 512);
  glutCreateWindow("Sh Binding Example");

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  
  // initialize OpenGL
  init_gl();

  // initialize Sh
  init_sh();
  
  glutMainLoop();
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
  int vp[4];
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
  glDisable(GL_VERTEX_PROGRAM_ARB);
  glDisable(GL_FRAGMENT_PROGRAM_ARB);

  // render the character through glut
  char* p = temp;
  glRasterPos2f(x, y);
  while(*p)
    {
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
