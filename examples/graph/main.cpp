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

int gprintf(int x, int y, char* fmt, ...);

// replace with an RPN parser soon
enum Graph {
  GR_LINE, // just a line with slope (MUL) 
  GR_POLY, // polynomial (tests MAD, ADD, MUL)
  GR_RATPOLY, // rational polynomial (MAD, ADD, MUL, RCP, DIV)
  GR_EXP, // exponential (EXP, EXP2, EXP10)
  GR_LOG, // logarithm (LOG, LOG2, LOG10)
  GR_RCP, // reciprocal (RCP) 
  GR_SQRT, // square root (SQRT)
  GR_RSQ, // inverse square root
  GR_FLR, // floor (FLR)
  GR_FRAC, // frac
  GR_NORM, // norm
  GR_TEX, // texture lookup (1D)
  GR_TEXI, // texture lookup (RECT)
  GR_LRP  // lerp between EXP and LOG (LRP, EXP, LOG)
};
const int NUMGRAPHS = (int)(GR_LRP) + 1;

const char* GraphName[] = {
  "line",
  "poly",
  "ratpoly",
  "exp",
  "log",
  "rcp",
  "sqrt",
  "rsq",
  "floor",
  "frac",
  "norm",
  "tex",
  "texi",
  "lrp"
};

ShAttrib1f rangeWidth;
ShAttrib1f eps;
ShAttrib2f offset;
ShAttrib1f myscale;
ShProgram vsh, fsh[NUMGRAPHS];
#define DBG_MODE
Graph mode = GR_TEX; 

ShAttrib<5, SH_TEMP, float> coeff;
ShAttrib<3, SH_TEMP, float> denom;

const size_t TEX_SIZE = 128;
// @todo range cannot deal with interpolation yet
ShInterp<0, ShUnclamped<ShTexture1D<ShColor1f> > > tex(TEX_SIZE);

// Glut data
int buttons[5] = {GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP};
int cur_x, cur_y;

// stack - does not check for overflow or underflow on push/pop ops.
// It is up to you to use the empty/full functions if you believe that you might
// screw up.


// implicit form of  a function in 3D space
// and its gradient (which should be calculated automatically in the future...)
struct PlotFunction {
  string m_name;
  PlotFunction(string name) : m_name(name) {}
  ShProgram func(Graph g) {
    ShProgram result = SH_BEGIN_PROGRAM() {
        ShInputAttrib1f SH_NAMEDECL(t, m_name);
        ShOutputAttrib1f SH_DECL(value);

        switch(g) {
          case GR_LINE:
            value = mad(t, coeff(1), coeff(0)); 
            break;
          case GR_POLY:
            //value = mad(t, mad(t, coeff(2), coeff(1)), coeff(0)); 
            value = t * mad(t, mad(t, coeff(3), coeff(2)), coeff(1)) + coeff(0); break; 
              break;
          case GR_RATPOLY:
            value = (t * mad(t, mad(t, coeff(3), coeff(2)), coeff(1)) + coeff(0))
              * rcp(mad(t, mad(t, denom(2), denom(1)), denom(0))); break; 
          case GR_EXP:
            value = exp(t); break;
    //        value = exp(t) * coeff(0) + exp2(t) * coeff(1) + exp10(t) * coeff(2); break;
          case GR_LOG:
            value = log(t); break;
     //       value = log(t) * coeff(0) + log2(t) * coeff(1) + log10(t) * coeff(2); break;
          case GR_RCP:
            value = rcp(t); break;
          case GR_SQRT:
            value = sqrt(t); break;
          case GR_RSQ:
            value = rsqrt(t); break;
          case GR_FLR:
            value = floor(t); break;
          case GR_FRAC:
            value = frac(t); break;
          case GR_NORM:
            value = normalize(t); break;

          case GR_TEX:
            {
              value = tex(t); break; 
            }

          case GR_TEXI:
            {
              value = tex[t]; break; 
            }

          case GR_LRP:
            value = lerp(coeff(0), exp(t), log(t)); break;
          default:
            std::cout << "Not Good!" << std::endl;
        }
    } SH_END;
    return result;
  }
};

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

    ShColor3f SH_DECL(funcColor) = ShConstAttrib3f(0, 0, 0);
    ShColor3f SH_DECL(bkgdColor) = ShConstAttrib3f(1, 1, 1);
    ShColor3f SH_DECL(gridColor) = ShConstAttrib3f(0, 0, 1);
    ShColor3f SH_DECL(inrangeColor) = ShConstAttrib3f(0, 0, 0);

    // take plot function, find if there's an intersection,
    // kill if no isct
    // or use the gradient function to get a normal if there is an isct. 
    PlotFunction plotfunc("texcoord");

    for(int i = GR_LINE; i <= GR_LRP; i = i + 1) {
#ifdef DBG_MODE // debugging
      if(i != mode) continue;
#endif
      ShProgram func = plotfunc.func(static_cast<Graph>(i));
      ShProgram ifunc = inclusion(func);

      func.node()->dump(std::string("func_") + GraphName[i]); 
      ifunc.node()->dump(std::string("ifunc_") + GraphName[i]); 

      ShProgram afunc = affine_inclusion_syms(func);
      afunc.node()->dump(std::string("afunc_") + GraphName[i]); 

      // if programs could take ShProgram parameters, we wouldn't have to do this 
      // (although this could be factored out in C++ too...)
      fsh[i] = SH_BEGIN_FRAGMENT_PROGRAM {
        ShInputTexCoord2f SH_DECL(texcoord);
        ShInputPosition4f SH_DECL(posh);

        ShOutputColor3f SH_DECL(color);

        ShPoint2f pos = texcoord * myscale + offset;

        ShAttrib1f scaled_eps = eps * myscale;

        // check if in curve
        ShAttrib1f SH_DECL(val) = func(pos(0));  // evaluate function 
        ShAttrib1f inCurve = abs(val - pos(1)) < scaled_eps; 

        // check if in range
        ShAttrib1f SH_DECL(inRange) = 0.0f;
        ShColor3f rangeColor = inrangeColor;

        ShAttrib1f start = floor(pos(0) / rangeWidth) * rangeWidth; 
        ShAttrib1f end = ceil(pos(0) / rangeWidth) * rangeWidth; 
        ShAttrib1a_f SH_DECL(range) = make_interval(start, end);
        ShAttrib1f SH_DECL(center) = range_center(range);

        // check if in affine range
#if 1
        ShAttrib1a_f SH_DECL(result_range) = afunc(range);
        ShAttrib1f SH_DECL(result_center) = range_center(result_range);
        ShAttrib1f SH_DECL(result_inerr) = affine_lasterr(result_range, range);
        ShAttrib1f SH_DECL(result_radius) = range_radius(result_range);
        ShAttrib1f SH_DECL(result_other) = result_radius - abs(result_inerr);

        ShAttrib1f errValue = (pos(0) - center) / range_radius(range);
        
        inRange = abs(errValue * result_inerr + result_center - pos(1)) < result_other;
        rangeColor(0) = inRange; 
/*
        // @todo debug
        inRange = 1;
    //    rangeColor = range_center(result_range)(0,0,0); 
        SH_BEGIN_SECTION("foo") {
          SH_BEGIN_SECTION("foo2") {
            rangeColor(0,1) = abs(result_inerr)(0,0); 
          } SH_END_SECTION;
          rangeColor(2) = (result_inerr < 0); 
        } SH_END_SECTION;
        */
#endif

        // check if in IA range
#if 1
        ShAttrib1i_f SH_DECL(rangei) = make_interval(start, end); 
        ShAttrib1i_f SH_DECL(result_rangei) = ifunc(rangei);

        ShAttrib1f SH_DECL(inRangei);
        inRangei = range_lo(range_contains(result_rangei, pos(1)));
        rangeColor(1) = inRangei; 
        inRange = inRange || inRangei;
#endif


        // check if in grid
        ShAttrib1f inGrid = (abs(pos(0)) < scaled_eps) + (abs(pos(1)) < scaled_eps); 

        color = lerp(inGrid, gridColor, lerp(inCurve, funcColor, lerp(inRange, rangeColor, bkgdColor)));
        //color=pos(0,1,0);
      } SH_END;

      fsh[i].node()->dump(std::string("fsh_") + GraphName[i]);
    }
    vsh = namedAlign(vsh, fsh[mode]);
    vsh.node()->dump("vsh");
}

GLuint displayList;

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glCallList(displayList);

  gprintf(30, 100, "Sh Graph Info");
  gprintf(30, 80, "Mode - %s", GraphName[mode]);
  gprintf(30, 60, "Width - %f", rangeWidth.getValue(0));

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

  ShHostMemoryPtr texmem = new ShHostMemory(TEX_SIZE * sizeof(float));
  float* data = reinterpret_cast<float*>(texmem->hostStorage()->data());
  for(size_t i = 0; i < TEX_SIZE; ++i) {
    data[i] = 0.5 + 0.5 * sin(M_PI * 2.0 * i / float(TEX_SIZE));   
  }

  tex.memory(texmem);
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
      case 'r': rangeWidth /= 1.01f; break;
      case 'R': rangeWidth *= 1.01f; break;
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

      case '+': if(mode == GR_LRP) mode = GR_LINE; 
                else mode = static_cast<Graph>(mode + 1);
                shBind(fsh[mode]); break;
      case '-': if(mode == GR_LINE) mode = GR_LRP; 
                else mode = static_cast<Graph>(mode - 1);
                shBind(fsh[mode]); break;
    }
  glutPostRedisplay();
}

int main(int argc, char** argv)
{
  coeff(0) = 0.0f; 
  coeff(1) = 0.0f; 
  coeff(2) = 3.0f;;
  coeff(3) = -2.0f;

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
    shBind(fsh[mode]);
  } catch(const ShException &e) {
    cout << "Error: " << e.message() << endl;
  } catch(...) {
    cout << "Caught unknown error: " << endl;
  }
  
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

  glColor3f(0, 0, 0);

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
