#define GL_GLEXT_PROTOTYPES
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

GLhandleARB p, f, v;
float lpos[4] = {1,0.5,1,0};

// Glut data
int buttons[5] = {GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP};
int cur_x, cur_y;

bool show_help = false;

int gprintf(int x, int y, char* fmt, ...);

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();
  gluLookAt(0.0,0.0,5.0, 
	    0.0,0.0,-1.0,
	    0.0f,1.0f,0.0f);

  glLightfv(GL_LIGHT0, GL_POSITION, lpos);

  glutSolidTeapot(1);

  // Help information
  if (show_help) {
    gprintf(30, 100, "Sh OpenGL SL Example Help");
    gprintf(30, 30,  "  'Q' - Quit");
  } else {
    gprintf(10, 10, "'H' for help...");
  }
  
  glutSwapBuffers();
}

void reshape(int w, int h)
{
  // Prevent a divide by zero, when window is too short
  // (you cant make a window of zero width).
  if(h == 0)
    h = 1;
  
  float ratio = 1.0* w / h;
  
  // Reset the coordinate system before modifying
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  // Set the viewport to be the entire window
  glViewport(0, 0, w, h);
  
  // Set the correct perspective.
  gluPerspective(45,ratio,1,1000);
  glMatrixMode(GL_MODELVIEW);
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
  case 'q':
  case 'Q':
    exit(0);
    break;
  case 'h':
  case 'H':
    show_help = !show_help;
    break;
  }
  glutPostRedisplay();
}

void setShaders()
{
  v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
  f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);	

  string vstr, fstr;

  ifstream vfile("toon.vert");
  for (char buffer[1024 + 1]; !vfile.eof(); ) {
    vfile.getline(buffer, 1024);
    vstr += buffer;
    vstr += "\n";
  }

  ifstream ffile("toon.frag");
  for (char buffer[1024 + 1]; !ffile.eof(); ) {
    ffile.getline(buffer, 1024);
    fstr += buffer;
    fstr += "\n";
  }

  //cout << vstr << endl;
  //cout << fstr << endl;

  const char * vv = vstr.c_str();
  const char * ff = fstr.c_str();
  
  glShaderSourceARB(v, 1, &vv,NULL);
  glShaderSourceARB(f, 1, &ff,NULL);
  
  glCompileShaderARB(v);
  glCompileShaderARB(f);
  
  p = glCreateProgramObjectARB();
  
  glAttachObjectARB(p,v);
  glAttachObjectARB(p,f);
  
  glLinkProgramARB(p);
  glUseProgramObjectARB(p);
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
  glutKeyboardFunc(keyboard);
  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_VERTEX_PROGRAM_ARB);
  glEnable(GL_FRAGMENT_PROGRAM_ARB);
  glClearColor(0.0, 0.0, 0.0, 1.0);

  setShaders();

  glutMainLoop();
}

void printInfoLog(GLhandleARB obj)
{
  int infologLength = 0;
  int charsWritten  = 0;
  char *infoLog;

  glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB,
			    &infologLength);

  if (infologLength > 0) {
    infoLog = (char *)malloc(infologLength);
    glGetInfoLogARB(obj, infologLength, &charsWritten, infoLog);
    printf("%s\n",infoLog);
    free(infoLog);
  }
}

int gprintf(int x, int y, char* fmt, ...)
{
  char temp[1024];
  sprintf(temp, fmt);
  
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
