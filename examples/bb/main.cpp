#include <sh/sh.hpp>
#include <sh/shutil.hpp>
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include "Camera.hpp"
#include "BranchBound.hpp"

using namespace std;
using namespace SH;
using namespace ShUtil;

ShMatrix4x4f mv, mvd;
ShAttrib3f lightPos;
Camera camera;

ShConstColor3f ZERO(0.5f, 0.5f, 0.0f);
ShConstColor3f HI(0.5f, 1.0f, 0.5f);
ShConstColor3f LO(0.0f, 0.0f, 0.5f);

ShColor3f kd;
ShProgram dispvsh, vsh, fsh;


// Glut data
int buttons[5] = {GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP};
int cur_x, cur_y;
bool shiftOn, ctrlOn;
ShProgram func, ifunc;
int splitLevel = 1;
double eps = 1e-7;
typedef BranchBound<ShAttrib2f, ShAttrib1f> BBParams;
BBParams bb;
typedef BranchBoundIterator<BBParams> BBIterator;
BBIterator bbIt; 
typedef std::vector<BBParams::ResultType> ResultVec;
ResultVec results;
size_t curpos = 0;

ShAttrib2i_f initialBounds;

void bindShaders()
{
  try {
    shBind(vsh);
    shBind(fsh);
  } catch(const ShException &e) {
    cout << "Error: " << e.message() << endl;
  } catch(...) {
    cout << "Caught unknown error: " << endl;
  }
}

void initShaders()
{
  try {
    vsh = ShKernelLib::shVsh(mv, mvd);
    lightPos = ShPoint3f(5.0, 5.0, 5.0);
    vsh = vsh << shExtract("lightPos") << lightPos;

    ShColor3f SH_DECL(lightColor) = ShConstAttrib3f(0.75, .75, .75);
    kd.name("kd");
    //ShColor3f SH_DECL(ks) = ShColor3f(0.3, 0.3, 0.3);
    //ShAttrib1f SH_DECL(specexp) = 64.0f;
    
    ShProgram light = ShKernelLight::pointLight<ShColor3f>() << lightColor; 
    fsh = ShKernelSurface::diffuse<ShColor3f>() << kd << light;

    vsh = namedAlign(vsh, fsh);
  } catch(const ShException &e) {
    cout << "Error: " << e.message() << endl;
  } catch(...) {
    cout << "Caught unknown error: " << endl;
  }

  bindShaders();
}

void iterateBB() 
{
  if(bbIt.done()) return;
  std::cout << "iterating" << std::endl;
  try {
    bbIt.iterate();
    results.push_back(bbIt.current());
    ++curpos;
  } catch(const ShException &e) {
    cout << "Error: " << e.message() << endl;
  } catch(...) {
    cout << "Caught unknown error: " << endl;
  }
}

void initBB() 
{
  bb = BBParams(ifunc, splitLevel, eps);
  bbIt = BBIterator(bb, initialBounds);
  results.clear();
  iterateBB();
  curpos = 0;
}


void init()
{
  initialBounds = make_interval(ShConstAttrib2f(-10.0f, -10.0f), ShConstAttrib2f(10.0f, 10.0f));

  func = SH_BEGIN_PROGRAM("gpu:stream") {
    ShInputAttrib2f SH_DECL(in);
    ShOutputAttrib1f SH_DECL(result);

    ShAttrib2f insq = in * in;
  //  result = insq(0) - insq(1) * in(1) + insq(0) * insq(0); 

    // the famous "three hump camel" function [Hansen 2004 p.291]
    //    result = insq(0) * (2 - insq(0) * (1.05 + insq(0) * (1.0/6.0))) - in(0) * in(1) + insq(1);

    result = insq(0) + insq(1);
  } SH_END;

  ifunc = inclusion(func); 
}

void setupView()
{
  mv = camera.shModelView();
  mvd = camera.shModelViewProjection(ShMatrix4x4f());
}

void reshape(int width, int height)
{
  glMatrixMode(GL_PROJECTION);
  glViewport(0, 0, width, height);
  camera.glProjection(float(width)/height);
  setupView();
}

double zscale = 1.0f;

void drawQuad(ShAttrib2i_f bounds, double value) {
  ShAttrib2f lo = range_lo(bounds); 
  ShAttrib2f hi = range_hi(bounds); 
  value *= zscale;
  glBegin(GL_QUADS); 
    glNormal3f(0, 0, 1);
    glVertex3f(lo.getValue(0), lo.getValue(1), value); 
    glVertex3f(hi.getValue(0), lo.getValue(1), value); 
    glVertex3f(hi.getValue(0), hi.getValue(1), value); 
    glVertex3f(lo.getValue(0), hi.getValue(1), value); 
  glEnd();
}

void display()
{
  std::cout << "displaying" << std::endl;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  bindShaders();

  // draw axes in zero plane
  kd = ZERO; 
  drawQuad(initialBounds * ShConstAttrib2f(0.1f, 1.0f), 0);
  drawQuad(initialBounds * ShConstAttrib2f(1.0f, 0.1f), 0);

  GLUquadric* sph;
  sph = gluNewQuadric();
  gluSphere(sph, 0.25, 32, 32);

  // get ranges and display them
  BBParams::ResultType r = results[curpos]; 
  int count = r.domain.count();
  for(int i = 0; i < count; ++i) {
    ShAttrib2i_f domain = r.domain[i]; 
    ShAttrib1i_f range = r.range[i];

    kd = LO; 
    drawQuad(domain, range_lo(range).getValue(0));

    kd = HI; 
    drawQuad(domain, range_hi(range).getValue(0));
  }


  glFinish();

  glutSwapBuffers();
}


void motion(int x, int y)
{
  double factor = 20.0;
  double dx = (x - cur_x)/factor;
  double dy = (cur_y - y)/factor;
  bool changed = false;

  if (buttons[GLUT_LEFT_BUTTON] == GLUT_DOWN) {
    camera.orbit(cur_x, cur_y, x, y,
                 glutGet(GLUT_WINDOW_WIDTH),
                 glutGet(GLUT_WINDOW_HEIGHT));
    changed = true;
  }

  if (buttons[GLUT_MIDDLE_BUTTON] == GLUT_DOWN) {
    camera.move(0, 0, -dy); 
    changed = true;
  }

  
  if (buttons[GLUT_RIGHT_BUTTON] == GLUT_DOWN) {
    camera.move(dx, dy, 0);
    changed = true;
  }

  if (changed) { 
    setupView();
    glutPostRedisplay();
  }
  //std::cout << cur_x << ", " << cur_y << std::endl;

  cur_x = x;
  cur_y = y;
}

void mouse(int button, int state, int x, int y)
{
  shiftOn = glutGetModifiers() & GLUT_ACTIVE_SHIFT;
  ctrlOn = glutGetModifiers() & GLUT_ACTIVE_CTRL;
  buttons[button] = state;
  cur_x = x;
  cur_y = y;
}

void keyboardUp(unsigned char k, int x, int y)
{
  shiftOn = glutGetModifiers() & GLUT_ACTIVE_SHIFT;
  ctrlOn = glutGetModifiers() & GLUT_ACTIVE_CTRL;
}

void keypress(int k, int x, int y) {
  switch(k)
    {
    case GLUT_KEY_RIGHT: // advance by one
      if(results.size() == curpos + 1) {
        iterateBB();
      } else {
        ++curpos;
      }
      break;

    case GLUT_KEY_LEFT: // advance by one
      if(curpos > 0) --curpos;
      break;

    default:
      return;
  }

  std::cout << "iterations = " << results.size() << std::endl;
  std::cout << "curpos = " << curpos << std::endl;

  glutPostRedisplay();
}

bool animate=false;

void timer(int value)
{
  std::cout << "Timer " << value << std::endl;
  keypress(GLUT_KEY_RIGHT, 0, 0);
  if(animate) {
    glutTimerFunc(500, timer, value + 1);
  }
}

void keyboard(unsigned char k, int x, int y)
{
  shiftOn = glutGetModifiers() & GLUT_ACTIVE_SHIFT;
  ctrlOn = glutGetModifiers() & GLUT_ACTIVE_CTRL;

  switch(k)
    {
    case 'q':
    case 'Q':
      exit(0);
      break;

    case 'b': // reset
      initBB();
      break;

    case 'r': // resets view
     camera.reset();
     camera.move(0.0, 0.0, -20.0);
     setupView();
     break;

    case 's':
     if(splitLevel > 1) splitLevel--;
     initBB();
     break;
    case 'S':
     splitLevel++;
     initBB();
     break;

    case 'a':
     animate = !animate;
     if(animate) {
       glutTimerFunc(500, timer, 0);
     }
     break;

    case 'Z': zscale *= 1.05f; break;
    case 'z': zscale /= 1.05f; break;
  }
  std::cout << "split level = " << splitLevel << std::endl;
  std::cout << "iterations = " << results.size() << std::endl;
  std::cout << "curpos = " << curpos << std::endl;

  glutPostRedisplay();
}


int main(int argc, char** argv)
{
  shSetBackend("arb");

  // @todo debug
/*
  init();
  initBB();
  for(;;) {
    if(getchar() == 'q') break;
    iterateBB();
  }
  return 0;
*/

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

  glutInitWindowSize(512, 512);
  glutCreateWindow("Sh Example");
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutKeyboardFunc(keyboard);
  glutKeyboardUpFunc(keyboardUp);
  glutIgnoreKeyRepeat(1);
  glutSpecialFunc(keypress);
  
  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_VERTEX_PROGRAM_ARB);
  glEnable(GL_FRAGMENT_PROGRAM_ARB);
  glDisable(GL_CULL_FACE);

  glClearColor(0.5, 0.5, 0.5, 1.0);

  // Place the camera at its initial position
  setupView();

  camera.move(0.0, 0.0, -20.0);

  init();
  initShaders();
  initBB();
//  glutTimerFunc(3000, timer, 0);
  glutMainLoop();
}
