#include <iostream>
#include <fstream>
#include <string>
#include <sh/sh.hpp>
#include <sh/shutil.hpp>
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include "Camera.hpp"
#include "ScalarStack.hpp"

using namespace std;
using namespace SH;
using namespace ShUtil;

ShMatrix4x4f mv, inv_mv, mvd;
ShPoint3f lightPos;
ShPoint3f eyeposm;
ShAttrib1f level;
ShAttrib1f eps;
ShAttrib1f traceDiff;
Camera camera;
ShProgram vsh, fsh;

// Glut data
int buttons[5] = {GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP};
int cur_x, cur_y;

// stack - does not check for overflow or underflow on push/pop ops.
// It is up to you to use the empty/full functions if you believe that you might
// screw up.


#define DBGIMPL 

void dump(ShProgram &foo, string desc) {
  cout << "ShProgram " << desc << endl;
  cout << foo.node()->describe_interface() << endl; 

  ofstream out(desc.c_str()); 
  foo.node()->ctrlGraph->graphvizDump(out);
  string cmd = string("dot -Tps < ") + desc.c_str() + " > " + desc.c_str() + ".ps";
  system(cmd.c_str());
}

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
      ShInputPoint3f SH_NAMEDECL(point, m_name);
      ShOutputAttrib1f SH_DECL(value);

      value = point(1); // manhattan distance from xz plane 

    } SH_END;
    return result;
  }

  ShProgram grad() {
    ShProgram result = SH_BEGIN_PROGRAM() {
      ShInputPoint3f SH_NAMEDECL(point, m_name);
      ShOutputAttrib3f SH_DECL(gradient);

      gradient = ShConstAttrib3f(0, 1, 0);
    } SH_END;
    return result;
  }
};
#endif

// given an interval program that computes some (reasonably nice) function g: ShAttrib1i_f -> ShAttrib1i_f,
// this computes a root and sets hasroot to true if there is a root in the part of the domain  
ShProgram trace(ShProgram func) {
  ShProgram result = SH_BEGIN_PROGRAM() {
    ShInputPoint3f SH_DECL(origin);
    ShInputVector3f SH_DECL(dir);
    ShInputAttrib1f SH_DECL(t);
    ShOutputAttrib1f SH_DECL(value);
    value = func(mad(t, dir, origin));
  } SH_END;
  return result;
}

// does a branch-and-bound style trace to find only the first global optima falling in the given range for t
// 1/2^N * range.width() is the minimum region searched.  If the inclusion function gives a root at this 
// level, then it is considered as a hit, and the middle of the first range that hits is returned.
//
// @param tracer must be a program taking ShAttrib1f as input and outputting ShAttrib1f
#if 0 // stack-based version
template<int N>
ShProgram firsthit(ShProgram tracer) {
  ShProgram i_tracer = inclusion(tracer);

  ShContext::current()->disable_optimization("propagation");
  ShProgram result = SH_BEGIN_PROGRAM() {
    ShInputAttrib1i_f SH_DECL(range); // range to search
    ShOutputAttrib1f SH_DECL(hashit); // bool to indicate if there was a hit
    ShOutputAttrib1f SH_DECL(hit); // hit location

    ScalarStack<N, float> stack;
    stack.push(ShConstAttrib1f(1.0f));
    ShAttrib1f SH_DECL(start) = 0.0f;
    ShAttrib1f SH_DECL(range_lo) = lo(range);
    ShAttrib1f SH_DECL(range_hi) = hi(range);
    ShAttrib1f SH_DECL(range_delta) = range_hi - range_lo; 

    SH_WHILE(!(start >= 1.0f || stack.full())) {
      ShAttrib1i_f SH_DECL(traceRange) = make_interval(range_delta * start, range_delta * stack.top());
      ShAttrib1i_f SH_DECL(traceResult);
      traceResult = i_tracer(traceRange);

      SH_IF(lo(range_contains(traceResult, 0))) {
        ShAttrib1f SH_DECL(next) = lerp(0.5f, start, stack.top());
        stack.push(next);
      } SH_ELSE {
        start = stack.top();
        stack.pop();
      } SH_ENDIF;
    } SH_ENDWHILE;

    SH_IF(stack.full()) {
      hashit = 1.0f;
      hit = lerp(0.5f, start, stack.top());
    } SH_ELSE {
      hashit = 0.0f;
    } SH_ENDIF;
  } SH_END;
  ShContext::current()->enable_optimization("propagation");
  dump(result, "firsthit");
  std::cout << "Optimizing firsthit" << endl;
  optimize(result);
  dump(result, "firsthit-prop");
  return result;
}
#else 

ShProgram firsthit(ShProgram tracer) {
  ShProgram i_tracer = inclusion(tracer);
  dump(i_tracer, "i_tracer");

  ShContext::current()->disable_optimization("propagation");
  ShProgram result = SH_BEGIN_PROGRAM() {
    ShInputAttrib1i_f SH_DECL(range); // range to search

    ShOutputAttrib1f SH_DECL(hashit); // bool to indicate if there was a hit
    ShOutputAttrib1f SH_DECL(hit); // hit location

    ShAttrib1f SH_DECL(start) = range_lo(range);
    ShAttrib1f SH_DECL(end) = range_hi(range);
    ShAttrib1f SH_DECL(mid) = lerp(0.5f, start, end); 

    //SH_WHILE(mid - start > eps && end - mid > eps) {
    ShAttrib1f SH_DECL(count) = 0.0f;
    SH_WHILE(count < 8.99f) {
      count = count + 1;
      ShAttrib1i_f SH_DECL(traceRange) = make_interval(start, mid);
      ShAttrib1i_f SH_DECL(traceResult);
      traceResult = i_tracer(traceRange);

      ShAttrib1f hitcond = range_lo(range_contains(traceResult, 0));
      start = cond(hitcond, start, mid);
      mid = lerp(0.5f, mid, cond(hitcond, start, end)); 
    } SH_ENDWHILE;

    hashit = (end - mid > 1.1 * eps); 
    hit = lerp(0.5f, start, mid);
  } SH_END;
  ShContext::current()->enable_optimization("propagation");
  dump(result, "firsthit");
  std::cout << "Optimizing firsthit" << endl;
  optimize(result);
  dump(result, "firsthit-prop");
  return result;
}
#endif

void initShaders() {
  try {
    vsh = ShKernelLib::shVsh(mv, mvd);
    vsh = vsh << shExtract("lightPos") << lightPos;

    ShColor3f SH_DECL(diffusecolor) = ShColor3f(0.5, 0.7, 0.9);

    level.name("level");
    level = 0.0;

    eps.name("epsilon");
    eps = 0.01;

    traceDiff = 1;
    traceDiff.name("traceDiff");

    eyeposm.name("eyeposm");


    // take plot function, find if there's an intersection,
    // kill if no isct
    // or use the gradient function to get a normal if there is an isct. 
    PlotFunction plotfunc("posm");

    ShProgram func = plotfunc.func();
    ShProgram gradient = plotfunc.grad();

    ShColor3f SH_DECL(kd) = ShConstAttrib3f(1, 1, 1);
    ShColor3f SH_DECL(lightColor) = ShConstAttrib3f(0.75, .75, .75);

    fsh = SH_BEGIN_FRAGMENT_PROGRAM {
      ShInputPoint3f SH_DECL(posm);
      ShInputPosition4f SH_DECL(posh);

#ifndef DBGIMPL 
      ShOutputNormal3f SH_DECL(normal);
      ShOutputVector3f SH_DECL(lightVec);
#else
      ShOutputColor3f SH_DECL(colour);
#endif


      ShVector3f SH_DECL(dir) = posm - eyeposm;
      ShAttrib1f SH_DECL(dist) = length(dir);
      dir = normalize(dir);

      ShProgram tracer = trace(func)(eyeposm, dir); 
      tracer = add<ShAttrib1f>() << -level << tracer;
      dump(tracer, "tracer");
      ShProgram hitter = firsthit(tracer);

      ShAttrib1i_f SH_DECL(range) = make_interval(dist, dist + traceDiff);  // @todo figure out where it exits the sphere
      ShAttrib1f SH_DECL(hashit);
      ShAttrib1f SH_DECL(hit);
      (hashit & hit) = hitter(range);

      /*
      kill(!hashit);
      ShPoint3f SH_DECL(hitp) = mad(hit, dir, eyeposm);
      */


#ifndef DBGIMPL 
      normal = gradient(hitp);
      normal = mv | normal;
      lightVec = mv | (lightPos - hitp); 
#else
      //colour = hashit * hitp; /* DEBUG */
      
      //ShAttrib1f tresult;
     // tresult = tracer(dist);
      //colour = tresult(0,0,0); 

      colour = (hit - dist)(0,0,0);
#endif
    } SH_END;
    dump(fsh, "fsh");
    std::cout << "OptimizationLevel: " << ShContext::current()->optimization() << std::endl;

#ifndef DBGIMPL
    ShProgram light = ShKernelLight::pointLight<ShColor3f>() << lightColor; 
    ShProgram lightNShade = ShKernelSurface::diffuse<ShColor3f>() << kd << light;
    fsh = namedConnect(fsh, lightNShade); 
#endif
    vsh = namedAlign(vsh, fsh);
    dump(vsh, "vsh");

    shBind(vsh);
    shBind(fsh);
  } catch(const ShException &e) {
    cout << "Error: " << e.message() << endl;
  } catch(...) {
    cout << "Caught unknown error: " << endl;
  }
}

GLuint displayList;

void display()
{
  std::cout << "displaying" << std::endl;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glCallList(displayList);
  glutSwapBuffers();
}

void setupView()
{
  mv = camera.shModelView();
  inv_mv = inverse(mv);
  eyeposm = inv_mv | ShConstPoint3f(0.0f, 0.0f, 0.0f); 
  mvd = camera.shModelViewProjection(ShMatrix4x4f());
}

void init()
{
  displayList = glGenLists(1);
  glNewList(displayList, GL_COMPILE);
  //glutSolidCube(1.0);

  GLUquadric* sph;
  sph = gluNewQuadric();
  gluSphere(sph, 1.0, 32, 32);
  glEndList();
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
  std::cout << cur_x << ", " << cur_y << std::endl;

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
    case 'l': level -= 0.1f; break;
    case 'L': level += 0.1f; break;
    case 'd': traceDiff -= 0.1f; break;
    case 'D': traceDiff += 0.1f; break;
    }
  glutPostRedisplay();
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
  
  shSetBackend("arb");

  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_VERTEX_PROGRAM_ARB);
  glEnable(GL_FRAGMENT_PROGRAM_ARB);
  glEnable(GL_CULL_FACE);
  glClearColor(1.0, 1.0, 1.0, 1.0);
  setupView();

  // Place the camera at its initial position
  camera.move(0.0, 0.0, -15.0);

  // Set up the light position
  lightPos = ShPoint3f(5.0, 5.0, 5.0);
  
  init();
  initShaders();

  
  glutMainLoop();
}
