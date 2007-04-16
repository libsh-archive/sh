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

#define SHOWAA true 
#define SHOWIA true 

int gprintf(int x, int y, char* fmt, ...);

//#define DBG_MODE

// replace with an RPN parser soon
enum GraphMode {
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
  /*
  GR_NOISE, // perlin noise
  GR_TEX, // texture lookup (1D)
  GR_TEXI, // texture lookup (RECT)
  */
  GR_POS, // positive of a polynomial 
  GR_MAX, // max of EXP and LOG 
  GR_MIN, // min of EXP and LOG 
  GR_LRP  // lerp between EXP and LOG (LRP, EXP, LOG)
} mode = GR_POLY;
const int NUMGRAPHS = (int)(GR_LRP) + 1;

const char* GraphModeName[] = {
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
  /*
  "noise",
  "tex",
  "texi",
  */
  "pos",
  "max",
  "min",
  "lrp"
};

Attrib1f rangeWidth;
Attrib1f eps;
Attrib2f myoffset;
Attrib1f myscale;
Attrib1f myscale2;
Attrib1f edge;
Attrib1f edge2;
Attrib1f edge3;
Program vsh, fsh[NUMGRAPHS];

Attrib<5, SH_TEMP, float> coeff;
Attrib<3, SH_TEMP, float> denom;

//const size_t TEX_SIZE = 128;
// @todo range cannot deal with interpolation yet
//Interp<0, Unclamped<Texture1D<Color1f> > > tex(TEX_SIZE);

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
        InputAttrib1f SH_NAMEDECL(t, m_name);
        OutputAttrib1f SH_DECL(value);

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

/*
          case GR_NOISE:
            value = perlin<1>(t); break;

          case GR_TEX:
            {
              value = tex(t); break; 
            }

          case GR_TEXI:
            {
              value = tex[t]; break; 
            }
*/
          case GR_POS:
            {
              value = pos(t * mad(t, mad(t, coeff(3), coeff(2)), coeff(1)) + coeff(0)); break; 
            }
          case GR_MAX:
            value = SH::max(coeff(0) * exp(t), coeff(1) * log(t)); break;

          case GR_MIN:
            value = SH::min(coeff(0) * exp(t), coeff(1) * log(t)); break;

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
    Matrix4x4f id;
    vsh = KernelLib::vsh(id, id);
    vsh = vsh << extract("lightPos") << ConstAttrib3f(0, 0, 0);

    rangeWidth.name("rangeWidth");
    rangeWidth = 0.1f; 

    eps.name("epsilon");
    eps = 0.005f; 

    myoffset.name("myoffset");
    myoffset = ConstAttrib2f(0.0f, 0.0f);
    
    myscale.name("myscale");
    myscale = 5.0f; 

    myscale2.name("myscale2");
    myscale2 = 20.0; 

    edge.name("edge");
    edge = 0.01;

    edge2.name("edge2");
    edge2 = 0.01;

    edge3.name("edge3");
    edge3 = 0.01;

    coeff.name("coeff");
    denom.name("denom");

    Color3f SH_DECL(funcColor) = ConstAttrib3f(0, 0, 0);
    Color3f SH_DECL(bkgdColor) = ConstAttrib3f(1, 1, 1);
    Color3f SH_DECL(gridColor) = ConstAttrib3f(.25, .25, .25);
    Color3f SH_DECL(inrangeColor) = ConstAttrib3f(0, 0, 0);
    Color3f SH_DECL(edgeColor) = ConstAttrib3f(0.5,0.5,0.5);

    Color3f SH_DECL(aaColor) = ColorFinder::color(ColorFinder::Dark2, 3, 0); 
    Color3f SH_DECL(iaColor) = ColorFinder::color(ColorFinder::Dark2, 3, 1); 
    cout << aaColor << endl << iaColor << endl;

    // take plot function, find if there's an intersection,
    // kill if no isct
    // or use the gradient function to get a normal if there is an isct. 
    PlotFunction plotfunc("texcoord");

    Context::current()->set_flag("aa_enable_um", false); // disabling unique merging
    //Context::current()->optimization(0);
    for(int i = GR_LINE; i <= GR_LRP; i = i + 1) {
#ifdef DBG_MODE // debugging
      if(i != mode) continue;
#endif
      Program func = plotfunc.func(static_cast<GraphMode>(i));
      Program ifunc = inclusion(func);

      func.node()->dump(std::string("func_") + GraphModeName[i]); 
      ifunc.node()->dump(std::string("ifunc_") + GraphModeName[i]); 

      Program afunc = affine_inclusion_syms(func);
      afunc.node()->dump(std::string("afunc_") + GraphModeName[i]); 

      // if programs could take Program parameters, we wouldn't have to do this 
      // (although this could be factored out in C++ too...)
      fsh[i] = SH_BEGIN_FRAGMENT_PROGRAM {
        InputTexCoord2f SH_DECL(texcoord);
        InputPosition4f SH_DECL(posh);

        OutputColor3f SH_DECL(color);

        Point2f pos = texcoord * myscale + myoffset;

        Attrib1f scaled_eps = eps * myscale;
        Attrib1f SH_DECL(inGrid) = 0.0f;
        Attrib1f SH_DECL(inEdge) = 0.0f;
        Attrib1f SH_DECL(inCurve) = 0.0f; 
        Attrib1f SH_DECL(inRange) = 0.0f;
        Color3f rangeColor = inrangeColor; 

        // check if in curve
        Attrib1f SH_DECL(val) = func(pos(0));  // evaluate function 
        Attrib1f deriv = abs(dx(val));
        inCurve = abs(val - pos(1)) < scaled_eps * myscale2 * deriv; 

        // check if in range

        Attrib1f start = floor(pos(0) / rangeWidth) * rangeWidth; 
        Attrib1f end = ceil(pos(0) / rangeWidth) * rangeWidth; 
        Attrib1a_f SH_DECL(range) = make_interval(start, end);
        Attrib1f SH_DECL(center) = range_center(range);

        // check if in affine range
#if SHOWAA
        Attrib1a_f SH_DECL(result_range) = afunc(range);
        Attrib1f SH_DECL(result_center) = range_center(result_range);
        Attrib1f SH_DECL(result_inerr) = affine_lasterr(result_range, range);
        Attrib1f SH_DECL(result_radius) = range_radius(result_range);
        Attrib1f SH_DECL(result_other) = result_radius - abs(result_inerr);

        Attrib1f errValue = (pos(0) - center) / range_radius(range);
        
        inRange = abs(errValue * result_inerr + result_center - pos(1)) < result_other;
        rangeColor = cond(inRange, aaColor, rangeColor);

        // check if in edge
        inEdge = inRange && (abs(errValue * result_inerr + result_center - pos(1)) - result_other > -edge2);
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
#if SHOWIA 
        Attrib1i_f SH_DECL(rangei) = make_interval(start, end); 
        Attrib1i_f SH_DECL(result_rangei) = ifunc(rangei);

        Attrib1f SH_DECL(inRangei);
        inRangei = range_lo(range_contains(result_rangei, pos(1)));
        rangeColor = cond(inRangei && !inRange, iaColor, aaColor);
        //rangeColor(1) = lerp(inRange, ConstAttrib1f(0.0f), inRangei); 
        inRange = inRange || inRangei;

        inEdge = inEdge || (inRangei && 
           (abs(pos(1) - range_center(result_rangei)) - range_radius(result_rangei) > -edge3));    


#endif
        inEdge = inEdge || (inRange && (abs(pos(0) - center) - rangeWidth > -edge));



        // check if in grid
        inGrid = (abs(pos(0)) < scaled_eps) + (abs(pos(1)) < scaled_eps); 

        color = lerp(inGrid, gridColor, 
            lerp(inEdge, edgeColor, lerp(inCurve, funcColor, lerp(inRange, rangeColor, bkgdColor))));
        //color=pos(0,1,0);
        //color = abs(dx(val))(0, 0, 0) * myscale2;
        //color = inCurve(0,0,0);
        Context::current()->parsing()->name(std::string("initial_fsh_") + GraphModeName[i]);
      } SH_END;

      fsh[i].name(string("fsh_") + GraphModeName[i]);
      placeAaSyms(fsh[i].node(), true);
      fsh[i].node()->dump(std::string("fsh_") + GraphModeName[i]);
    }
    vsh = namedAlign(vsh, fsh[mode]);
    vsh.name("vsh");
    vsh.node()->dump("vsh");
}

GLuint displayList;

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glCallList(displayList);

  gprintf(30, 100, "Sh GraphMode Info");
  gprintf(30, 80, "Mode - %s", GraphModeName[mode]);
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

/*
  HostMemoryPtr texmem = new HostMemory(TEX_SIZE * sizeof(float));
  float* data = reinterpret_cast<float*>(texmem->hostStorage()->data());
  for(size_t i = 0; i < TEX_SIZE; ++i) {
    data[i] = 0.5 + 0.5 * sin(M_PI * 2.0 * i / float(TEX_SIZE));   
  }

  tex.memory(texmem);
*/
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
    myoffset(0) -= (x - cur_x) / factor * myscale;
    myoffset(1) += (y - cur_y) / factor * myscale;
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
                else mode = static_cast<GraphMode>(mode + 1);
                bind(fsh[mode]); break;
      case '-': if(mode == GR_LINE) mode = GR_LRP; 
                else mode = static_cast<GraphMode>(mode - 1);
                bind(fsh[mode]); break;

      case 'a': myscale2 *= 1.05f; break;
      case 'z': myscale2 /= 1.05f; break; 
      case 's': edge *= 1.05f; break;
      case 'x': edge /= 1.05f; break; 
      case 'd': edge2 *= 1.05f; break;
      case 'c': edge2 /= 1.05f; break; 
      case 'f': edge3 *= 1.05f; break;
      case 'v': edge3 /= 1.05f; break; 
    }
  glutPostRedisplay();
}

int main(int argc, char** argv)
{
  coeff(0) = 1.0f; 
  coeff(1) = 1.0f; 
  coeff(2) = 1.5f; 
  coeff(3) = -1.0; 

  denom(0) = 1.0f;
  denom(1) = denom(2) = 0.0f;

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
  glClearColor(0.0, 0.0, 1.0, 1.0);
  std::cout << gluErrorString(glGetError()) << std::endl;
  setup();
  std::cout << gluErrorString(glGetError()) << std::endl;

  try {
    initShaders();
    bind(vsh);
    bind(fsh[mode]);
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
