#include <iostream>
#include <fstream>
#include <string>
#include <sh/sh.hpp>
#include <shutil/shutil.hpp>
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include "Camera.hpp"
#include "ColorFinder.hpp"

using namespace std;
using namespace SH;
using namespace ShUtil;

int gprintf(int x, int y, char* fmt, ...);

// replace with an RPN parser soon
enum GraphMode {
  GR_PLANE, // just a plane with slope (MUL) 
  GR_POLY, // polynomial (tests MAD, ADD, MUL)
  GR_CAMEL, // six hump camel
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
  GR_NOISE, // 1 octave 2D perlin noise 
  GR_TEX, // texture lookup (1D)
  GR_TEXI, // texture lookup (RECT)
*/
  GR_LRP  // lerp between EXP and LOG (LRP, EXP, LOG)
};
const int NUMGRAPHS = (int)(GR_LRP) + 1;

const int RES = 200;
const int WIDTH = 2;

const char* GraphModeName[] = {
  "plane",
  "poly",
  "camel",
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
  "noise",
  "tex",
  "texi",
  */
  "lrp"
};

const char* LevelName[] = {
  "base",
  "func",
  "hi",
  "lo"
};
const int NUMLEVELS = 4;

Attrib1f rangeWidth;
Attrib1f gridWidth;
Attrib1f eps;
Attrib2f myoffset;
Attrib1f myscale;
Attrib1f depthScale;
Program vsh[NUMGRAPHS][NUMLEVELS], fsh;
Color4f levelColor[NUMLEVELS];
//#define DBG_MODE
GraphMode mode = GR_PLANE; 
Camera camera;
Matrix4x4f mv, mvp;


Attrib<2, SH_TEMP, float> coeff[4]; 

const size_t TEX_SIZE = 128;
// @todo range cannot deal with interpolation yet
//Interp<0, Unclamped<Texture2D<Color1f> > > tex(TEX_SIZE, TEX_SIZE);
//Unclamped<Texture2D<Color1f> > tex(TEX_SIZE, TEX_SIZE);

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
  Program func(GraphMode g) {
    Program result = SH_BEGIN_PROGRAM() {
        InputAttrib2f SH_NAMEDECL(t, m_name);
        OutputAttrib1f SH_DECL(value);

        switch(g) {
          case GR_PLANE:
            value = sum(mad(t, coeff[1], coeff[0])); 
            break;
          case GR_POLY:
            //value = mad(t, mad(t, coeff(2), coeff(1)), coeff(0)); 
            value = sum(t * mad(t, mad(t, coeff[3], coeff[2]), coeff[1]) + coeff[0]); break; 
              break;
         case GR_CAMEL:
            value = (4 - 2.1 * t(0) * t(0) + pow(t(0), 4.0 / 3.0)) * t(0) * t(0) + t(0) * t(1) + (-4 + 4 * t(1) * t(1)) * t(1) * t(1);  
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
#endif

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
    Matrix4x4f id;

    rangeWidth.name("rangeWidth");
    rangeWidth = 0.1f; 

    gridWidth.name("gridWidth");
    gridWidth = 0.25f; 

    eps.name("epsilon");
    eps = 0.01f; 

    myoffset.name("myoffset");
    myoffset = ConstAttrib2f(0.0f, 0.0f);
    
    myscale.name("myscale");
    myscale = 1.0f; 

    depthScale.name("depthScale");
    depthScale = 1.0f; 

    for(int i = 0; i < 4; ++i) coeff[i].name("coeff");

    Color4f SH_DECL(gridColor) = ConstAttrib4f(0,0,0,1);

    for(int i = 0; i < 4; ++i) {
      levelColor[i](0, 1, 2) = ColorFinder::color(ColorFinder::Set2, 4, i);
      levelColor[i](3) = 1;
    }

    PlotFunction plotfunc("texcoord");

    fsh = SH_BEGIN_PROGRAM("gpu:fragment") {
      InputPosition4f SH_DECL(posh);
      InOutColor4f SH_DECL(color);
    } SH_END;

    for(int i = GR_PLANE; i <= GR_LRP; ++i) {
#ifdef DBG_MODE // debugging
      if(i != mode) continue;
#endif
      Program func = plotfunc.func(static_cast<GraphMode>(i));
      Program ifunc = inclusion(func);

      func.node()->dump(std::string("func_") + GraphModeName[i]); 
      ifunc.node()->dump(std::string("ifunc_") + GraphModeName[i]); 

      Program afunc = affine_inclusion_syms(func);
      afunc.node()->dump(std::string("afunc_") + GraphModeName[i]); 


      for(int j = 0; j < NUMLEVELS; ++j) {
        vsh[i][j] = SH_BEGIN_PROGRAM() {
          InOutPosition4f SH_DECL(posm);
          InOutTexCoord2f SH_DECL(texcoord);
          OutputColor4f SH_DECL(color);

          // map to actual texcoord based on myoffset and scale 
          texcoord = mad(texcoord, myscale, myoffset);


          Attrib1f SH_DECL(posm_myoffset);
          switch(j) {
            case 0:
              {
                posm_myoffset = ConstAttrib1f(0.0f);
                break;
              }
            case 1:
              {
                Attrib1f SH_DECL(val) = func(texcoord);  // evaluate function 
                posm_myoffset = val;  
                break;
              }
            case 2:
            case 3:
              {
                Attrib2f start = floor(texcoord / rangeWidth) * rangeWidth; 
                Attrib2f end = ceil(texcoord / rangeWidth) * rangeWidth; 

#if 1
                Attrib2a_f SH_DECL(range) = make_interval(start, end);
                Attrib2f SH_DECL(center) = range_center(range);

                Attrib1a_f SH_DECL(result_range) = afunc(range);
                Attrib1f SH_DECL(result_center) = range_center(result_range);

                Attrib2f SH_DECL(result_inerr);
                result_inerr(0) = affine_lasterr(result_range, range(0));
                result_inerr(1) = affine_lasterr(result_range, range(1));
                
                Attrib1f SH_DECL(result_radius) = range_radius(result_range);
                Attrib1f SH_DECL(result_other) = result_radius - sum(abs(result_inerr));

                // get high and low points at this texcoord
                Attrib1f SH_DECL(approx_center) = result_center + (((texcoord - center) * rcp(range_radius(range))) | result_inerr);
                if(j == 2) {
                  posm_myoffset = approx_center + result_other;
                } else {
                  posm_myoffset = approx_center - result_other;
                }
#else // IA 
                Attrib2i_f SH_DECL(range) = make_interval(start, end);
                Attrib1i_f SH_DECL(result_rangei) = ifunc(range);
                if(j == 2) {
                  posm_myoffset = range_hi(result_rangei);
                } else {
                  posm_myoffset = range_lo(result_rangei);
                }
#endif
                break;
              }
          }
          posm(2) += posm_myoffset * depthScale;

          // check if in grid
          Attrib2f diff = abs(texcoord - gridWidth * floor(texcoord * rcp(gridWidth))); 
          Attrib1f inGrid = (diff(0) < eps) + (diff(1) < eps) > 0.0f; 

          color = lerp(inGrid, gridColor, levelColor[j]); 
        } SH_END;
        vsh[i][j] = namedConnect(vsh[i][j], KernelLib::vsh(mv, mvp), true);//

        // @todo debug
        //vsh[i][j] = KernelLib::vsh(mv, mvp);

        vsh[i][j] = vsh[i][j] << extract("lightPos") << ConstAttrib3f(0, 0, 0);
        vsh[i][j] = namedAlign(vsh[i][j], fsh);
        vsh[i][j].node()->dump(std::string("vsh") + GraphModeName[i] + LevelName[j]);
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
      bind(vsh[mode][i]);
      glCallList(displayList);
    }
  } catch(const Exception &e) {
    cout << "Error: " << e.message() << endl;
  } catch(...) {
    cout << "Caught unknown error: " << endl;
  }

  gprintf(30, 120, "Sh GraphMode Info");
  gprintf(30, 100, "Mode - %s", GraphModeName[mode]);
  gprintf(30, 80, "Width - %f", rangeWidth.getValue(0));
  gprintf(30, 60, "Scale - %f", myscale.getValue(0)); 
  gprintf(30, 40, "Offset - (%f, %f)", myoffset.getValue(0), myoffset.getValue(1));
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

/*
  HostMemoryPtr texmem = new HostMemory(TEX_SIZE * TEX_SIZE * sizeof(float));
  float* data = reinterpret_cast<float*>(texmem->hostStorage()->data());
  for(size_t i = 0; i < TEX_SIZE; ++i) {
    for(size_t j = 0; j < TEX_SIZE; ++j) {
      data[i * TEX_SIZE + j] = 0.5 + 0.5 * 
        sin(M_PI * 2.0 * i / float(TEX_SIZE)) * cos(M_PI * 2.0 * j / float(TEX_SIZE));   
    }
  }

  tex.memory(texmem);
*/
}

void setupView()
{
  mv = camera.modelView();
  mvp = camera.modelViewProjection(Matrix4x4f());
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
    myoffset(0) -= (x - cur_x) / factor * myscale;
    myoffset(1) += (y - cur_y) / factor * myscale;
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
                else mode = static_cast<GraphMode>(mode + 1);
                break;
      case '-': if(mode == GR_PLANE) mode = GR_LRP; 
                else mode = static_cast<GraphMode>(mode - 1);
                break;
    }
  glutPostRedisplay();
}

int main(int argc, char** argv)
{
  for(int i = 0; i < 4; ++i) coeff[i] = ConstAttrib2f(1.0f, 1.0f);

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(512, 512);
  glutCreateWindow("Sh Example");
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutKeyboardFunc(keyboard);
  
  setBackend("arb");

  glEnable(GL_DEPTH_TEST);
  std::cout << gluErrorString(glGetError()) << std::endl;
  glEnable(GL_VERTEX_PROGRAM_ARB);
  std::cout << gluErrorString(glGetError()) << std::endl;
  glEnable(GL_FRAGMENT_PROGRAM_ARB);
  std::cout << gluErrorString(glGetError()) << std::endl;
  glClearColor(0.75, 0.75, 0.75, 1.0);
  std::cout << gluErrorString(glGetError()) << std::endl;
  setup();
  std::cout << gluErrorString(glGetError()) << std::endl;

  // Place the camera at its initial position
  camera.move(0.0, 0.0, -10.0);
  setupView();

  try {
    initShaders();
    bind(fsh);
  } catch(const Exception &e) {
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
