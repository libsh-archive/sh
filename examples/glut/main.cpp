#include <sh/sh.hpp>
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include "Camera.hpp"
#include <iostream>

using namespace SH;
using namespace std;

//ShMatrix4x4f mv, mvd;
//ShPoint3f lightPos;
Camera camera;
ShProgram vsh, fsh;

// Glut data
int buttons[5] = {GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP};
int cur_x, cur_y;

void initShaders()
{
  vsh = SH_BEGIN_VERTEX_PROGRAM {
    ShInOutPosition4f pos;
    ShInOutNormal3f normal;
    ShOutputVector3f lightv;

    ShConstPoint3f lightPos(5.0, 5.0, 5.0);
    ShMatrix4x4f mv;
    ShMatrix4x4f mvd;

    mv[0] = ShAttrib4f(1, 0, 0, 0);
    mv[1] = ShAttrib4f(0, 1, 0, 0);
    mv[2] = ShAttrib4f(0, 0, 1, -15);
    mv[3] = ShAttrib4f(0, 0, 0, 1);
    
    mvd[0] = ShAttrib4f(2.41421, 0, 0, 0);
    mvd[1] = ShAttrib4f(0, 2.41421, 0, 0);
    mvd[2] = ShAttrib4f(0, 0, -1.002, 14.8298);
    mvd[3] = ShAttrib4f(0, 0, -1, 15);

    ShPoint3f posv = (mv | pos)(0,1,2); // Compute viewspace position
    lightv = lightPos - posv; // Compute light direction
    
    pos = mvd | pos; // Project position
    normal = mv | normal; // Project normal
  } SH_END;

  //ShColor3f SH_DECL(diffusecolor) = ShColor3f(0.5, 0.7, 0.9);
  
  fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputNormal3f normal;
    ShInputVector3f lightv;

    ShOutputColor3f color;

    normal = normalize(normal);
    lightv = normalize(lightv);
    
    //color = (normal | lightv) * diffusecolor;
    color = (normal | lightv) * ShColor3f(0.5, 0.7, 0.9);
  } SH_END;
}

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glFrontFace(GL_CW);
  glutSolidTeapot(2.5);
  glFrontFace(GL_CCW);
  
  glutSwapBuffers();
}

void setupView()
{
  //mv = camera.shModelView();
  //mvd = camera.shModelViewProjection(ShMatrix4x4f());
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
  
  shSetBackend("glsl");

  initShaders();
  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_VERTEX_PROGRAM_ARB);
  glEnable(GL_FRAGMENT_PROGRAM_ARB);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  setupView();

  // Place the camera at its initial position
  camera.move(0.0, 0.0, -15.0);

  // Set up the light position
  //lightPos = ShPoint3f(5.0, 5.0, 5.0);
  
  initShaders();

  shBind(vsh);
  shBind(fsh);

#if 0
  cout << "Vertex Unit:" << endl;
  vsh.node()->code()->print(cout);
  cout << "--" << endl;
  cout << "Fragment Unit:" << endl;
  fsh.node()->code()->print(cout);
  cout << "--" << endl;
#endif

  glutMainLoop();
}
