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
  GR_PLANE, // just a plane with slope (MUL) 
  GR_POLY, // polynomial (tests MAD, ADD, MUL)
/*
  GR_RATPOLY, // rational polynomial (MAD, ADD, MUL, RCP, DIV)
  GR_EXP, // exponential (EXP, EXP2, EXP10)
  GR_LOG, // logarithm (LOG, LOG2, LOG10)
  GR_RCP, // reciprocal (RCP) 
  GR_SQRT, // square root (SQRT)
  GR_RSQ, // inverse square root
  GR_FLR, // floor (FLR)
  GR_FRAC, // frac
  GR_NORM, // norm
*/
  GR_NOISE, // 1 octave 2D perlin noise 
  GR_TEX, // texture lookup (1D)
  GR_TEXI, // texture lookup (RECT)
  GR_LRP  // lerp between EXP and LOG (LRP, EXP, LOG)
};
const int NUMGRAPHS = (int)(GR_LRP) + 1;

const int RES = 200;
const int WIDTH = 2;

const char* GraphName[] = {
  "plane",
  "poly",
  /*
  "ratpoly",
  "exp",
  "log",
  "rcp",
  "sqrt",
  "rsq",
  "floor",
  "frac",
  "norm",
  */
  "noise",
  "tex",
  "texi",
  "lrp"
};

const char* LevelName[] = {
  "base",
  "func",
  "hi",
  "lo"
};
const int NUMLEVELS = 4;

ShAttrib1f rangeWidth;
ShAttrib1f gridWidth;
ShAttrib1f eps;
ShAttrib2f offset;
ShAttrib1f myscale;
ShAttrib1f depthScale;
ShProgram vsh[NUMGRAPHS][NUMLEVELS], fsh;
ShColor4f levelColor[NUMLEVELS];
//#define DBG_MODE
Graph mode = GR_PLANE; 
Camera camera;
ShMatrix4x4f mv, mvp;


ShAttrib<2, SH_TEMP, float> coeff[4]; 

const size_t TEX_SIZE = 128;
// @todo range cannot deal with interpolation yet
ShInterp<0, ShUnclamped<ShTexture2D<ShColor1f> > > tex(TEX_SIZE, TEX_SIZE);
//ShUnclamped<ShTexture2D<ShColor1f> > tex(TEX_SIZE, TEX_SIZE);

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
        ShInputAttrib2f SH_NAMEDECL(t, m_name);
        ShOutputAttrib1f SH_DECL(value);

        switch(g) {
          case GR_PLANE:
            value = sum(mad(t, coeff[1], coeff[0])); 
            break;
          case GR_POLY:
            //value = mad(t, mad(t, coeff(2), coeff(1)), coeff(0)); 
            value = sum(t * mad(t, mad(t, coeff[3], coeff[2]), coeff[1]) + coeff[0]); break; 
              break;
#if 0
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
#endif
          case GR_NOISE:
            {
              value = perlin<1>(t); break;
            }

          case GR_TEX:
            {
              value = tex(t); break; 
            }

          case GR_TEXI:
            {
              value = tex[t]; break; 
            }

          case GR_LRP:
            value = lerp(coeff[0](0), exp(t(0)), log(t(0))); break;
          default:
            std::cout << "Not Good!" << std::endl;
        }
    } SH_END;
    return result;
  }
};

void initShaders() {
    ShMatrix4x4f id;

    rangeWidth.name("rangeWidth");
    rangeWidth = 0.1f; 

    gridWidth.name("gridWidth");
    gridWidth = 0.25f; 

    eps.name("epsilon");
    eps = 0.01f; 

    offset.name("offset");
    offset = ShConstAttrib2f(0.0f, 0.0f);
    
    myscale.name("myscale");
    myscale = 1.0f; 

    depthScale.name("depthScale");
    depthScale = 1.0f; 

    for(int i = 0; i < 4; ++i) coeff[i].name("coeff");

    ShColor4f SH_DECL(gridColor) = ShConstAttrib4f(0,0,0,1);

    levelColor[0] = ShConstAttrib4f(0.5, 0.5, 0.5f, 0.0f); 
    levelColor[1] = ShConstAttrib4f(1,1,1,1); 
    levelColor[2] = ShConstAttrib4f(1,0,0,0.5f); 
    levelColor[3] = ShConstAttrib4f(0,1,0,0.5f); 

    PlotFunction plotfunc("texcoord");

    fsh = SH_BEGIN_PROGRAM("gpu:fragment") {
      ShInputPosition4f SH_DECL(posh);
      ShInOutColor4f SH_DECL(color);
    } SH_END;

    for(int i = GR_PLANE; i <= GR_LRP; ++i) {
#ifdef DBG_MODE // debugging
      if(i != mode) continue;
#endif
      ShProgram func = plotfunc.func(static_cast<Graph>(i));
      ShProgram ifunc = inclusion(func);

      func.node()->dump(std::string("func_") + GraphName[i]); 
      ifunc.node()->dump(std::string("ifunc_") + GraphName[i]); 

      ShProgram afunc = affine_inclusion_syms(func);
      afunc.node()->dump(std::string("afunc_") + GraphName[i]); 


      for(int j = 0; j < NUMLEVELS; ++j) {
        vsh[i][j] = SH_BEGIN_PROGRAM() {
          ShInOutPosition4f SH_DECL(posm);
          ShInOutTexCoord2f SH_DECL(texcoord);
          ShOutputColor4f SH_DECL(color);

          // map to actual texcoord based on offset and scale 
          texcoord = mad(texcoord, myscale, offset);


          ShAttrib1f SH_DECL(posm_offset);
          switch(j) {
            case 0:
              {
                posm_offset = ShConstAttrib1f(0.0f);
                break;
              }
            case 1:
              {
                ShAttrib1f SH_DECL(val) = func(texcoord);  // evaluate function 
                posm_offset = val;  
                break;
              }
            case 2:
            case 3:
              {
                ShAttrib2f start = floor(texcoord / rangeWidth) * rangeWidth; 
                ShAttrib2f end = ceil(texcoord / rangeWidth) * rangeWidth; 

#if 0
                ShAttrib2a_f SH_DECL(range) = make_interval(start, end);
                ShAttrib2f SH_DECL(center) = range_center(range);

                ShAttrib1a_f SH_DECL(result_range) = afunc(range);
                ShAttrib1f SH_DECL(result_center) = range_center(result_range);

                ShAttrib2f SH_DECL(result_inerr);
                result_inerr(0) = affine_lasterr(result_range, range(0));
                result_inerr(1) = affine_lasterr(result_range, range(1));
                
                ShAttrib1f SH_DECL(result_radius) = range_radius(result_range);
                ShAttrib1f SH_DECL(result_other) = result_radius - sum(abs(result_inerr));

                // get high and low points at this texcoord
                ShAttrib1f SH_DECL(approx_center) = result_center + (((texcoord - center) * rcp(range_radius(range))) | result_inerr);
                if(j == 2) {
                  posm_offset = approx_center + result_other;
                } else {
                  posm_offset = approx_center - result_other;
                }
#else // IA 
                ShAttrib2i_f SH_DECL(range) = make_interval(start, end);
                ShAttrib1i_f SH_DECL(result_rangei) = ifunc(range);
                if(j == 2) {
                  posm_offset = range_hi(result_rangei);
                } else {
                  posm_offset = range_lo(result_rangei);
                }
#endif
                break;
              }
          }
          posm(2) += posm_offset * depthScale;

          // check if in grid
          ShAttrib2f diff = abs(texcoord - gridWidth * floor(texcoord * rcp(gridWidth))); 
          ShAttrib1f inGrid = (diff(0) < eps) + (diff(1) < eps) > 0.0f; 

          color = lerp(inGrid, gridColor, levelColor[j]); 
        } SH_END;
        vsh[i][j] = namedConnect(vsh[i][j], ShKernelLib::shVsh(mv, mvp), true);//

        // @todo debug
        //vsh[i][j] = ShKernelLib::shVsh(mv, mvp);

        vsh[i][j] = vsh[i][j] << shExtract("lightPos") << ShConstAttrib3f(0, 0, 0);
        vsh[i][j] = namedAlign(vsh[i][j], fsh);
        vsh[i][j].node()->dump(std::string("vsh") + GraphName[i] + LevelName[j]);
      }
    }
}

GLuint displayList;

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  try {
    // @todo debug
    //for(int i = 0; i < 2; ++i) {
    for(int i = 0; i < NUMLEVELS; ++i) {
      shBind(vsh[mode][i]);
      glCallList(displayList);
    }
  } catch(const ShException &e) {
    cout << "Error: " << e.message() << endl;
  } catch(...) {
    cout << "Caught unknown error: " << endl;
  }

  gprintf(30, 120, "Sh Graph Info");
  gprintf(30, 100, "Mode - %s", GraphName[mode]);
  gprintf(30, 80, "Width - %f", rangeWidth.getValue(0));
  gprintf(30, 60, "Scale - %f", myscale.getValue(0)); 
  gprintf(30, 40, "Offset - (%f, %f)", offset.getValue(0), offset.getValue(1));
  gprintf(30, 20, "Depth Scale - %f", depthScale.getValue(0)); 
  gprintf(30, 0, "Grid Width - %f", gridWidth.getValue(0)); 


  glutSwapBuffers();
}

void setup()
{
  displayList = glGenLists(1);
  std::cout << gluErrorString(glGetError()) << std::endl;
  glNewList(displayList, GL_COMPILE);
  std::cout << gluErrorString(glGetError()) << std::endl;
  glNormal3f(0.0f, 0.0f, 1.0f);
  float diff = WIDTH * 2.0f / RES;
  for(int i = 0; i < RES; ++i) {
    glBegin(GL_QUAD_STRIP);
    for(int j = 0; j <= RES; ++j) {
      float y = (i - RES / 2) * diff; 
      float x = (j - RES / 2) * diff; 
      glTexCoord2f(x, y);
      glVertex3f(x, y, 0);

      glTexCoord2f(x, y + diff);
      glVertex3f(x, y + diff, 0);
    }
    glEnd();
  }
  glEndList();

  ShHostMemoryPtr texmem = new ShHostMemory(TEX_SIZE * TEX_SIZE * sizeof(float));
  float* data = reinterpret_cast<float*>(texmem->hostStorage()->data());
  for(size_t i = 0; i < TEX_SIZE; ++i) {
    for(size_t j = 0; j < TEX_SIZE; ++j) {
      data[i * TEX_SIZE + j] = 0.5 + 0.5 * 
        sin(M_PI * 2.0 * i / float(TEX_SIZE)) * cos(M_PI * 2.0 * j / float(TEX_SIZE));   
    }
  }

  tex.memory(texmem);
}

void setupView()
{
  mv = camera.shModelView();
  mvp = camera.shModelViewProjection(ShMatrix4x4f());
}

void reshape(int width, int height)
{
  glViewport(0, 0, width, height);
  setupView();
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

  if (buttons[GLUT_RIGHT_BUTTON] == GLUT_DOWN) {
    camera.orbit(cur_x, cur_y, x, y,
        glutGet(GLUT_WINDOW_WIDTH),
        glutGet(GLUT_WINDOW_HEIGHT));
    setupView();
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

      case 'g': gridWidth -= 0.05f; break;
      case 'G': gridWidth += 0.05f; break;

      case 'd': depthScale /= 1.01f; break;
      case 'D': depthScale *= 1.01f; break;

      case 'e': eps /= 1.1f; break;
      case 'E': eps *= 1.1f; break;

      case '0': coeff[0](0) -= 0.1f; break;
      case ')': coeff[0](0) += 0.1f; break;

      case '1': coeff[1](0) -= 0.1f; break;
      case '!': coeff[1](0) += 0.1f; break;

      case '2': coeff[2](0) -= 0.1f; break;
      case '@': coeff[2](0) += 0.1f; break;
 
      case '3': coeff[3](0) -= 0.1f; break;
      case '#': coeff[3](0) += 0.1f; break;

      case '6': coeff[0](1) -= 0.1f; break;
      case '^': coeff[0](1) += 0.1f; break;

      case '7': coeff[1](1) -= 0.1f; break;
      case '&': coeff[1](1) += 0.1f; break;

      case '8': coeff[2](1) -= 0.1f; break;
      case '*': coeff[2](1) += 0.1f; break;

      case '9': coeff[2](1) -= 0.1f; break;
      case '(': coeff[2](1) += 0.1f; break;

      case '+': if(mode == GR_LRP) mode = GR_PLANE; 
                else mode = static_cast<Graph>(mode + 1);
                break;
      case '-': if(mode == GR_PLANE) mode = GR_LRP; 
                else mode = static_cast<Graph>(mode - 1);
                break;
    }
  glutPostRedisplay();
}

int main(int argc, char** argv)
{
  for(int i = 0; i < 4; ++i) coeff[i] = ShConstAttrib2f(1.0f, 1.0f);

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

  // Place the camera at its initial position
  camera.move(0.0, 0.0, -10.0);
  setupView();

  try {
    initShaders();
    shBind(fsh);
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
