#include <iostream>
#include <fstream>
#include <string>
#include <sh/sh.hpp>
#include <sh/shutil.hpp>
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include "Camera.hpp"

using namespace std;
using namespace SH;
using namespace ShUtil;

ShAttrib1f rangeWidth;
ShAttrib1f eps;
ShAttrib2f offset;
ShAttrib1f myscale;
ShProgram vsh, ifsh, afsh;

ShAttrib<5, SH_TEMP, float> coeff;
ShAttrib<3, SH_TEMP, float> denom;

// Glut data
int buttons[5] = {GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP};
int cur_x, cur_y;

// stack - does not check for overflow or underflow on push/pop ops.
// It is up to you to use the empty/full functions if you believe that you might
// screw up.


// implicit form of  a function in 3D space
// and its gradient (which should be calculated automatically in the future...)
#if 0
struct PlotFunction {
  string m_name;
  PlotFunction(string name) : m_name(name) {}
  ShProgram func() {
    ShProgram result = SH_BEGIN_PROGRAM() {
      ShInputPoint3f SH_NAMEDECL(point, m_name);
      ShOutputAttrib1f SH_DECL(value);

      value = point | point; // euclidean squared distance from 0

    } SH_END;
    return result;
  }

  ShProgram grad() {
    ShProgram result = SH_BEGIN_PROGRAM() {
      ShInputPoint3f SH_NAMEDECL(point, m_name);
      ShOutputAttrib3f SH_DECL(gradient);

      gradient = point * 0.5;
    } SH_END;
    return result;
  }
};
#else
struct PlotFunction {
  string m_name;
  PlotFunction(string name) : m_name(name) {}
  ShProgram func() {
    ShProgram result = SH_BEGIN_PROGRAM() {
      ShInputAttrib1f SH_NAMEDECL(t, m_name);
      ShOutputAttrib1f SH_DECL(value);

     // value = mad(t, mad(t, mad(t, mad(t, coeff(4), coeff(3)), coeff(2)), coeff(1)), coeff(0))
      //   mad(t, mad(t, denom(2), denom(1)), denom(0));

        value = t * t * coeff(2)  + t * coeff(1) + coeff(0);
  //        value = t + t; 
  //      value = t * coeff(0); 
    } SH_END;
    return result;
  }
};
#endif

void initShaders() {
    ShMatrix4x4f id;
    vsh = ShKernelLib::shVsh(id, id);
    vsh = vsh << shExtract("lightPos") << ShConstAttrib3f(0, 0, 0);

    rangeWidth.name("rangeWidth");
    rangeWidth = 0.1f; 

    eps.name("epsilon");
    eps = 0.005f; 

    offset.name("offset");
    offset = ShConstAttrib2f(0.0f, 0.0f);
    
    myscale.name("myscale");
    myscale = 5.0f; 

    coeff.name("coeff");
    denom.name("denom");


    // take plot function, find if there's an intersection,
    // kill if no isct
    // or use the gradient function to get a normal if there is an isct. 
    PlotFunction plotfunc("texcoord");

    ShProgram func = plotfunc.func();
    func.node()->dump("func");

    ShProgram ifunc = inclusion(func);
    ifunc.node()->dump("ifunc");

    ShProgram afunc = affine_inclusion(func);
    afunc.node()->dump("afunc");

    ShColor3f SH_DECL(funcColor) = ShConstAttrib3f(0, 0, 0);
    ShColor3f SH_DECL(bkgdColor) = ShConstAttrib3f(1, 1, 1);
    ShColor3f SH_DECL(gridColor) = ShConstAttrib3f(0, 0, 1);
    ShColor3f SH_DECL(inrangeColor) = ShConstAttrib3f(1, 0.5, 0.5);

    ifsh = SH_BEGIN_FRAGMENT_PROGRAM {
      ShInputTexCoord2f SH_DECL(texcoord);
      ShInputPosition4f SH_DECL(posh);

      ShOutputColor3f SH_DECL(color);

      ShPoint2f pos = texcoord * myscale + offset;

      ShAttrib1f scaled_eps = eps * myscale;

      // check if in curve
      ShAttrib1f SH_DECL(val) = func(pos(0));  // evaluate function 
      ShAttrib1f inCurve = abs(val - pos(1)) < scaled_eps; 

      // check if in range
      ShAttrib1f start = floor(pos(0) / rangeWidth) * rangeWidth; 
      ShAttrib1f end = ceil(pos(0) / rangeWidth) * rangeWidth; 
      ShAttrib1i_f range = make_interval(start, end);
      ShAttrib1i_f SH_DECL(result_range) = ifunc(range);
      ShAttrib1f inRange;
      
      inRange = range_lo(range_contains(result_range, pos(1)));

      // check if in grid
      ShAttrib1f inGrid = (abs(pos(0)) < scaled_eps) + (abs(pos(1)) < scaled_eps); 

      color = lerp(inGrid, gridColor, lerp(inCurve, funcColor, lerp(inRange, inrangeColor, bkgdColor)));
      //color=pos(0,1,0);
    } SH_END;

    // if programs could take ShProgram parameters, we wouldn't have to do this 
    // (although this could be factored out in C++ too...)
    afsh = SH_BEGIN_FRAGMENT_PROGRAM {
      ShInputTexCoord2f SH_DECL(texcoord);
      ShInputPosition4f SH_DECL(posh);

      ShOutputColor3f SH_DECL(color);

      ShPoint2f pos = texcoord * myscale + offset;

      ShAttrib1f scaled_eps = eps * myscale;

      // check if in curve
      ShAttrib1f SH_DECL(val) = func(pos(0));  // evaluate function 
      ShAttrib1f inCurve = abs(val - pos(1)) < scaled_eps; 

      // check if in range
      ShAttrib1f start = floor(pos(0) / rangeWidth) * rangeWidth; 
      ShAttrib1f end = ceil(pos(0) / rangeWidth) * rangeWidth; 
      ShAttrib1i_f range = make_interval(start, end);
      ShAttrib1i_f SH_DECL(result_range) = afunc(range);
      ShAttrib1f inRange;
      
      inRange = range_lo(range_contains(result_range, pos(1)));

      // check if in grid
      ShAttrib1f inGrid = (abs(pos(0)) < scaled_eps) + (abs(pos(1)) < scaled_eps); 

      color = lerp(inGrid, gridColor, lerp(inCurve, funcColor, lerp(inRange, inrangeColor, bkgdColor)));
      //color=pos(0,1,0);
    } SH_END;
    vsh = namedAlign(vsh, ifsh);

    vsh.node()->dump("vsh");
    ifsh.node()->dump("ifsh");
    afsh.node()->dump("afsh");
}

GLuint displayList;

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glCallList(displayList);
  glutSwapBuffers();
}

void setup()
{
  displayList = glGenLists(1);
  std::cout << gluErrorString(glGetError()) << std::endl;
  glNewList(displayList, GL_COMPILE);
  std::cout << gluErrorString(glGetError()) << std::endl;
    glBegin(GL_QUADS);
  std::cout << gluErrorString(glGetError()) << std::endl;
      glNormal3f(0.0f, 0.0f, 1.0f);
      glTexCoord2f(-1.0f, -1.0f);
      glVertex2f(-1.0f, -1.0f);

      glTexCoord2f(1.0f, -1.0f);
      glVertex2f(1.0f, -1.0f);

      glTexCoord2f(1.0f, 1.0f);
      glVertex2f(1.0f, 1.0f);

      glTexCoord2f(-1.0f, 1.0f);
      glVertex2f(-1.0f, 1.0f);
    glEnd();
  glEndList();
}

void reshape(int width, int height)
{
  glViewport(0, 0, width, height);
}

void motion(int x, int y)
{
  const double factor = 100.0;
  const double sfactor = 100.0;
  bool changed = false;
  
  if (buttons[GLUT_LEFT_BUTTON] == GLUT_DOWN) {
    offset(0) -= (x - cur_x) / factor * myscale;
    offset(1) += (y - cur_y) / factor * myscale;
    changed = true;
  }
  if (buttons[GLUT_MIDDLE_BUTTON] == GLUT_DOWN) {
    myscale *= 1.0 + (y - cur_y) / sfactor; 
    changed = true;
  }

  if (changed) {
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
  switch(k)
    {
      case 'q':
      case 'Q':
        exit(0);
        break;
      case 'r': rangeWidth /= 1.1f; break;
      case 'R': rangeWidth *= 1.1f; break;
      case 'e': eps /= 1.1f; break;
      case 'E': eps *= 1.1f; break;

      case '0': coeff(0) -= 0.1f; break;
      case ')': coeff(0) += 0.1f; break;

      case '1': coeff(1) -= 0.1f; break;
      case '!': coeff(1) += 0.1f; break;

      case '2': coeff(2) -= 0.1f; break;
      case '@': coeff(2) += 0.1f; break;
 
      case '3': coeff(3) -= 0.1f; break;
      case '#': coeff(3) += 0.1f; break;

      case '4': coeff(4) -= 0.1f; break;
      case '$': coeff(4) += 0.1f; break;

      case '7': denom(0) -= 0.1f; break;
      case '&': denom(0) += 0.1f; break;

      case '8': denom(1) -= 0.1f; break;
      case '*': denom(1) += 0.1f; break;

      case '9': denom(2) -= 0.1f; break;
      case '(': denom(2) += 0.1f; break;

      case 'i': 
        {
          std::cout << "Changing to IA" << std::endl;
          shBind(ifsh); 
          break; 
        }
      case 'a': 
        {
          std::cout << "Changing to AA" << std::endl;
          shBind(afsh); 
          break; 
        }
    }
  std::cout << "Current Settings: " << std::endl;
  std::cout << "  epsilon = " << eps << std::endl;
  std::cout << "  width = " << rangeWidth << std::endl;
  std::cout << "  coeff = " << coeff << std::endl;
  std::cout << "  denom = " << denom << std::endl;
  glutPostRedisplay();
}

int main(int argc, char** argv)
{
  coeff(0) = 5.0f;;
  coeff(1) = -1.0f;
  coeff(2) = 0.04;;
  coeff(3) = 2.0f;

  denom(0) = 1.0f;

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(512, 512);
  glutCreateWindow("Sh Example");
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutKeyboardFunc(keyboard);
  
  shSetBackend("arb");

  glEnable(GL_DEPTH_TEST);
  std::cout << gluErrorString(glGetError()) << std::endl;
  glEnable(GL_VERTEX_PROGRAM_ARB);
  std::cout << gluErrorString(glGetError()) << std::endl;
  glEnable(GL_FRAGMENT_PROGRAM_ARB);
  std::cout << gluErrorString(glGetError()) << std::endl;
  glClearColor(0.0, 0.0, 1.0, 1.0);
  std::cout << gluErrorString(glGetError()) << std::endl;
  setup();
  std::cout << gluErrorString(glGetError()) << std::endl;

  try {
    initShaders();
    shBind(vsh);
    shBind(ifsh);
  } catch(const ShException &e) {
    cout << "Error: " << e.message() << endl;
  } catch(...) {
    cout << "Caught unknown error: " << endl;
  }
  
  glutMainLoop();
}
