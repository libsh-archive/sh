#include <sh/sh.hpp>
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <iostream>

using namespace SH;

ShProgram vsh, fsh;

void initShaders()
{
  vsh = SH_BEGIN_VERTEX_PROGRAM {
    ShInOutPosition4f pos;
    ShInputNormal3f normal;
    ShOutputNormal4f onormal;
    ShOutputVector4f lightv;

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
    lightv(0,1,2) = lightPos - posv; // Compute light direction
    
    pos = mvd | pos; // Project position
    onormal(0,1,2) = mv | normal; // Project normal
  } SH_END;

  fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputNormal4f normal_in;
    ShInputVector4f lightv_in;
    ShOutputColor4f color;

    ShVector3f normal = normalize(normal_in(0,1,2));
    ShVector3f lightv = normalize(lightv_in(0,1,2));
    
    color = (normal | lightv) * ShColor4f(0.5, 0.7, 0.9, 1.0);
  } SH_END;
}

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glutSolidTeapot(2.5);
  glutSwapBuffers();
}

int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(512, 512);
  glutCreateWindow("Sh Example");
  glutDisplayFunc(display);
  
  shSetBackend("glsl");

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_VERTEX_PROGRAM_ARB);
  glEnable(GL_FRAGMENT_PROGRAM_ARB);
  glClearColor(0.0, 0.0, 0.0, 1.0);

  initShaders();

  shBind(vsh);
  shBind(fsh);

#if 1
  using namespace std;
  cout << "Vertex Unit:" << endl;
  vsh.node()->code()->print(cout);
  cout << "--" << endl;
  cout << "Fragment Unit:" << endl;
  fsh.node()->code()->print(cout);
  cout << "--" << endl;
#endif

  glutMainLoop();
}
