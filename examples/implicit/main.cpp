#include <complex>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <sh/sh.hpp>
#include <sh/ShOptimizations.hpp>
#include <sh/shutil.hpp>
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include "Camera.hpp"
#include "ScalarStack.hpp"

using namespace std;
using namespace SH;
using namespace ShUtil;

bool shiftOn;
bool ctrlOn;
ShMatrix4x4f mv, inv_mv, mvd;
ShPoint3f lightPos;
ShPoint3f eyeposm;
ShAttrib1f level;
ShAttrib1f eps;
ShAttrib1f traceDiff;
ShAttrib1f cutoff;
Camera camera;
ShProgram vsh, fsh;
ShAttrib1f sphereRadius = 8.0;

enum DebugMode {
  DEBUG_START,
  NORMAL, // normal plotter
  FUNC, // shows function value at the midplane perpendicular to the viewer in the sphere
  HIT,  // shows hit/nohit, distance to hit
  GRAD, // shows normalized gradients at hitpoints
  LOOP, // shows loop count
  CUTOFF, // cuts off after a fixed loop count
  DEBUG_END // marker
};
DebugMode debugMode = NORMAL;

const char* DebugModeName[] = {
  "debug_start",
  "Normal",
  "Function Value",
  "Hit Distance",
  "Gradient",
  "Loop Count",
  "Loop Cutoff",
  "debug_end"
};

// Glut data
int buttons[5] = {GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP};
int cur_x, cur_y;

//#define DBGIMPL 

void dump(ShProgram &foo, string desc) {
  cout << "ShProgram " << desc << endl;
  cout << foo.node()->describe_interface() << endl; 

  ofstream out(desc.c_str()); 
  foo.node()->ctrlGraph->graphvizDump(out);
  string cmd = string("dot -Tps < ") + desc.c_str() + " > " + desc.c_str() + ".ps";
  system(cmd.c_str());
}

/* This is used to group a function and its gradient
 * It would not be needed if we had built-in automatic differentiatin, 
 * but right now it's just a bit of extra cruft.
 */
struct PlotFunc: public ShRefCountable {
  PlotFunc(string name): m_name(name) {}
  virtual ~PlotFunc() {}

  string name() { return m_name; }

  ShProgram funcProgram(string inName="in", string outName="out") {
    ShProgram result = SH_BEGIN_PROGRAM() {
      ShInputPoint3f SH_NAMEDECL(in, inName);
      ShOutputAttrib1f SH_NAMEDECL(out, outName);
      out = func(in);
    } SH_END;
    return result;
  }

  ShProgram gradProgram(string inName="in", string outName="out") {
    ShProgram result = SH_BEGIN_PROGRAM() {
      ShInputPoint3f SH_NAMEDECL(in, inName);
      ShOutputAttrib3f SH_NAMEDECL(out, outName);
      out = grad(in);
    } SH_END;
    return result;
  }

  virtual ShAttrib1f func(ShPoint3f p) = 0; 
  virtual ShAttrib3f grad(ShPoint3f p) = 0; 

  protected:
    string m_name;
};
typedef ShPointer<PlotFunc> PlotFuncPtr;

/* Unit Sphere generator v = x^2 + y^2 + z^2 */
struct Dist2SpherePF: PlotFunc {
  Dist2SpherePF(): PlotFunc("dist2_sphere") {}
  ShAttrib1f func(ShPoint3f point) { return point | point; }
  ShAttrib3f grad(ShPoint3f point) { return 2 * point; }
};

/* (1-r^2)^n function that mike mentioned  
 * Look up the proper source for this distance function */
#if 0
struct MikeSpherePF: PlotFunc {
  MikeSpherePF(ShAttrib1f &n): PlotFunc("mike_sphere"), m_n(n) {}
  ShAttrib1f func(ShPoint3f point) {
    ShAttrib1f oneMinusR = 1 - point | point;
    // @todo range 
    // hard coded n = 3... for now until we have pow working
    return oneMinusR * oneMinusR * oneMinusR; 
  }

  ShAttrib3f grad(ShPoint3f point) { 
    ShAttrib3f r2 = point * point;
    ShAttrib3f oneMinusR = ShConstAttrib3f(1, 1, 1) - r2; 
    return 2 * oneMinusR * oneMinusR * -2 * r2; 
  }

  protected:
    ShAttrib1f &m_n;
};
#endif

/* treats x, y as the real/im in juliabrot
 * result is the real part after max_level iterations
 * (cuts off early if magnitude > julia_max)
 *
 * @todo this is really slow and doesn't really work...
 */
struct JuliaPF: PlotFunc {
  JuliaPF(ShAttrib1f &maxlvl, ShAttrib1f &julia_max, ShAttrib2f &c): 
    PlotFunc("julia"), m_maxlvl(maxlvl), m_julia_max(julia_max), m_c(c) {}

  /* some complex julia set code taken from shrike */ 
  ShAttrib1f func(ShPoint3f point) {
    ShAttrib2f SH_DECL(z) = point(0,1);
    //ShAttrib1f SH_DECL(count) = 0.0f;
    //SH_DO {
    for(int i = 0; i < 5; ++i) {
      ShAttrib2f zp;
      zp(0) = z(0) * z(0) - z(1) * z(1); 
      zp(1) = 2 * z(0) * z(1);
      z = zp + m_c;
    }
    // can't do a count variable in here cause it gets turned into an interval
    //} SH_UNTIL (/*count < m_maxlvl && */(z(0) * z(0)) < m_julia_max); 
    return z | z; 
  }

  ShAttrib3f grad(ShPoint3f point) { 
    return ShConstAttrib3f(1,0,0);
  }

  protected:
    ShAttrib1f &m_maxlvl;
    ShAttrib1f &m_julia_max;
    ShAttrib2f &m_c;
};

/* Euclidean Distance from textured square 
 *
 * (Wow this is damned simple...) */
struct HeightFieldPF: PlotFunc {
  // given a filename to a black and white image
  HeightFieldPF(string filename): PlotFunc("heightfield") { 
    ShImage img;
    img.loadPng(filename);
    size = img.width();
    m_tex.size(img.width(), img.height());
    m_tex.memory(img.memory());
  }

  ShAttrib1f func(ShPoint3f point) {
    ShTexCoord2f tc = (size * 0.25) *  (point(0,1) + ShConstAttrib2f(0.5f, 0.5f));
    return m_tex[tc] - point(2);
  }

  ShAttrib3f grad(ShPoint3f point) {
    ShAttrib3f grad;
    ShTexCoord2f tc = (size * 0.25) *  (point(0,1) + ShConstAttrib2f(0.5f, 0.5f));

    // @todo instead of just putting 512 here use the height and stuff
    ShAttrib1f val = m_tex[tc];
    grad(0) = (size * 0.25) * (val - m_tex[tc + ShConstAttrib2f(1, 0)]);
    grad(1) = (size * 0.25) * (val - m_tex[tc + ShConstAttrib2f(0, 1)]); 
    grad(2) = ShConstAttrib1f(1.0);
    // @todo
    return grad;
  }

  private:
    ShInterp<0, ShTextureRect<ShColor1f> > m_tex;
    double size;
};


/* Could add in some other typical metaball generators:
 * where rr = x^2 + y^2 + z^2 = r^2
 *
 * a, b constants
 *
 * Blinn's: v = a e^(-b rr)
 * Wyvill: v = a(1 - 4rr^3/9b^6 + 17rr^2/9b^4 - 22rr^2/9b^2) 
 * Classic? metaballs: v = a(1 - 3rr/b^2)     0 <= r <= b/3
 *                         3a/2 (1 - r/b)^2   b/3 <= r <= b
 *                         0                  b <= r
 */


/* Unit Tube Generator (Infinite length) v = x^2 + y^2 */ 
struct Dist2TubePF: PlotFunc {
  Dist2TubePF() : PlotFunc("tube") {}
  ShAttrib1f func(ShPoint3f point) { return point(0,1) | point(0,1);  }
  ShAttrib3f grad(ShPoint3f point) { 
    ShAttrib3f result = ShConstAttrib3f(0,0,0);
    result(0,1) = 2 * point(0,1); 
    return result;
  }
};

/* Plane Generator (Infinite) v = y */
struct DistPlanePF: PlotFunc {
  DistPlanePF() : PlotFunc("plane") {}
  ShAttrib1f func(ShPoint3f point) { return point(2);  }
  ShAttrib3f grad(ShPoint3f point) { return ShConstAttrib3f(0,0,1); }
};

/* Inverts any other generator */
struct InvPF: PlotFunc {
  InvPF(PlotFuncPtr pf): PlotFunc("inv_" + pf->name()), m_pf(pf) {}
  ShAttrib1f func(ShPoint3f point) { return rcp(m_pf->func(point)); }

  /* Use quotient rule (f/g)' = (g'f - f'g) / g^2 
   * Here f = 1 */ 
  ShAttrib3f grad(ShPoint3f point) { 
    ShAttrib1f invg = func(point);
    ShAttrib3f gp = m_pf->grad(point);
    return gp * invg * invg; 
  }
  protected:
    PlotFuncPtr m_pf;
};

/* should just allow arbitrary matrices */
/* Translates any other generator */
struct TranslatePF: PlotFunc {
  TranslatePF(PlotFuncPtr pf, ShVector3f& trans)  
    : PlotFunc("trans_" + pf->name()), m_pf(pf), m_trans(trans) {}
  ShAttrib1f func(ShPoint3f point) { return m_pf->func(point - m_trans); } 
  ShAttrib3f grad(ShPoint3f point) { return m_pf->grad(point - m_trans); } 
  protected:
    PlotFuncPtr m_pf;
    ShVector3f &m_trans;
};

struct ScalePF: PlotFunc {
  ScalePF(PlotFuncPtr pf, ShAttrib3f& scl)  
    : PlotFunc("scl_" + pf->name()), m_pf(pf), m_scl(scl) {}
  ShAttrib1f func(ShPoint3f point) { return m_pf->func(point * m_scl); } 
  ShAttrib3f grad(ShPoint3f point) { return m_pf->grad(point * m_scl); } 
  protected:
    PlotFuncPtr m_pf;
    ShAttrib3f &m_scl;
};

struct FuncScalePF: PlotFunc {
  FuncScalePF(PlotFuncPtr pf, ShAttrib1f& scl)  
    : PlotFunc("funcscl_" + pf->name()), m_pf(pf), m_scl(scl) {}
  ShAttrib1f func(ShPoint3f point) { return m_scl * m_pf->func(point); } 
  ShAttrib3f grad(ShPoint3f point) { return m_scl * m_pf->grad(point); } 
  protected:
    PlotFuncPtr m_pf;
    ShAttrib1f &m_scl;
};

/* Sums up a set of plot functions */
struct SumPF: PlotFunc {
  SumPF(): PlotFunc("sum") {}

  ShAttrib1f func(ShPoint3f point) {
    ShAttrib1f result; 
    for(PfVec::iterator I = m_pfvec.begin(); I != m_pfvec.end(); ++I) {
      if(I == m_pfvec.begin()) {
        result = (*I)->func(point);
      } else {
        result += (*I)->func(point);
      }
    }
    return result;
  }

  ShAttrib3f grad(ShPoint3f point) {
    ShAttrib3f result; 
    for(PfVec::iterator I = m_pfvec.begin(); I != m_pfvec.end(); ++I) {
      if(I == m_pfvec.begin()) {
        result = (*I)->grad(point);
      } else {
        result += (*I)->grad(point);
      }
    }
    return result;
  }

  void add(PlotFuncPtr pf) {
    m_pfvec.push_back(pf);
  }

  protected:
    typedef std::vector<PlotFuncPtr> PfVec;
    PfVec m_pfvec;
};
typedef ShPointer<SumPF> SumPFPtr;

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

/* Branch and Bound, sans stack.
 *
 * Commonly branch and bound approaches use either BFS or DFS (or some hybrid)
 * of the search space.  This is a DFS-style alg, except that we always
 * split the remaining search space in half, so that there's no  
 * need for a stack.
 * 
 * (This has probably been done by someone before...just have to look some more)
 *
 * Convergence compared to regular DFS style in optimal case is the same.
 * Otherwise, it depends on convergence rate of the type of range arithmetic.
 * (TODO proof that with "well behaved" functions, convergence is in the 
 * same order as the usual stack based alg with a small constant)
 * (TODO define "well behaved")
 */ 
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

    ShAttrib1f SH_DECL(count) = 0.0f;

    // Check for hits until feasible range drops below eps
    SH_WHILE(mid - start > eps && end - mid > eps) { 
      count += 1.0f;
      ShAttrib1i_f SH_DECL(traceRange) = make_interval(start, mid);
      ShAttrib1i_f SH_DECL(traceResult);
      traceResult = i_tracer(traceRange);

      // If there's a potential hit, subdivide [start,mid], 
      // else discard and continue with [mid,end]
      // ShAttrib1f hitcond = range_lo(range_contains(traceResult, 0));
      ShAttrib1f hitcond = range_lo(traceResult) < 0 && 0 < range_hi(traceResult);
      start = lerp(hitcond, start, mid);
      mid = lerp(0.5f, mid, lerp(hitcond, start, end)); 
      if(debugMode == CUTOFF) {

        // do something to end the loop
        end = lerp(count > cutoff, mid - 1.0, end);
      }
    } SH_ENDWHILE;

    switch(debugMode) {
      case LOOP:
        hit = count * (1.0f / 255.0f);
        break;
      case CUTOFF:
        hit = (mid - range_lo(range)) / range_width(range);  
        break;
      default:
        hashit = (end - mid > 1.1 * eps); 
        hit = lerp(0.5f, start, mid);
        break;
    }
    if(debugMode == LOOP) {
    } else {
    }
  } SH_END;
  ShContext::current()->enable_optimization("propagation");
  dump(result, "firsthit");
  std::cout << "Optimizing firsthit" << endl;
  optimize(result);
  dump(result, "firsthit-prop");
  return result;
}
#endif

ShVector3f trans[9];
ShAttrib3f scl[9];
ShAttrib1f fscl[9];
PlotFuncPtr pfs[9];
int selected=0; // 0 = no selection (mouse interacts with view), 1-9 means selected object i-1


void initShaders() {
  try {
    vsh = ShKernelLib::shVsh(mv, mvd);
    vsh = vsh << shExtract("lightPos") << lightPos;

    ShColor3f SH_DECL(kd) = ShColor3f(0.5, 0.7, 0.9);
    ShColor3f SH_DECL(ks) = ShColor3f(0.3, 0.3, 0.3);
    ShAttrib1f SH_DECL(specexp) = 64.0f;


    // Build the plot function 
    SumPFPtr plotfunc = new SumPF();
    for(int i = 0; i < 9; ++i) {
      if(pfs[i]) {
        plotfunc->add(new FuncScalePF(new TranslatePF(new ScalePF(pfs[i], scl[i]), trans[i]), fscl[i]));
      }
    }

    ShProgram func = plotfunc->funcProgram("posm");
    ShProgram gradient = plotfunc->gradProgram("posm");

    ShColor3f SH_DECL(lightColor) = ShConstAttrib3f(0.75, .75, .75);

    fsh = SH_BEGIN_FRAGMENT_PROGRAM {
      ShInputPoint3f SH_DECL(posm);
      ShInputVector3f SH_DECL(viewVec);
      ShInputPosition4f SH_DECL(posh);

      ShVector3f SH_DECL(dir) = posm - eyeposm;
      ShAttrib1f SH_DECL(dist) = length(dir);
      dir = normalize(dir);

      ShProgram tracer = trace(func)(eyeposm, dir); 
      tracer = add<ShAttrib1f>() << -level << tracer;
      dump(tracer, "tracer");
      ShProgram hitter = firsthit(tracer);

      ShAttrib1f SH_DECL(trace_start); 
      ShAttrib1f SH_DECL(trace_end) = dist + 2.0  * (dir | -posm) + traceDiff;

      // handle if we're inside the sphere
      trace_start = SH::min(dist, trace_end);
      trace_end = SH::max(dist, trace_end);

      ShAttrib1i_f SH_DECL(range) = make_interval(trace_start, trace_end);  // @todo figure out where it exits the sphere
      ShAttrib1f SH_DECL(hashit);
      ShAttrib1f SH_DECL(hit);
      if(debugMode != FUNC) {
        (hashit & hit) = hitter(range);
      }

      if(debugMode == NORMAL) {
        kill(!hashit);
        ShPoint3f SH_DECL(hitp) = mad(hit, dir, eyeposm);

        ShOutputNormal3f SH_DECL(normal);
        ShOutputVector3f SH_DECL(lightVec);
        ShOutputVector3f SH_DECL(halfVec);
        normal = gradient(hitp);
        normal = mv | normal;
        lightVec = mv | (lightPos - hitp); 
        halfVec = normalize(lightVec + viewVec);

        ShConstAttrib1f ONE(1.0f);
        normal *= lerp((normal | viewVec) < 0.0f, -ONE, ONE); 
      } else {
        ShOutputColor3f SH_DECL(colour);
        switch(debugMode) {
          case FUNC:
            {
              ShAttrib1f SH_DECL(le) = length(eyeposm);
              ShAttrib1f SH_DECL(t) = le * le / (dir | -eyeposm);
              ShAttrib1f SH_DECL(funcval);
              funcval = tracer(t);
              colour = funcval(0,0,0); 
            }
            break;
          case GRAD:
            {
              kill(!hashit);
              ShPoint3f SH_DECL(hitp) = mad(hit, dir, eyeposm);
              colour = gradient(hitp);
            }
            break;
          case HIT:
            colour(0) = hashit;  // hit or not
            colour(1) = hashit * (hit - dist) / abs(trace_end); // distance to hit
            break;

          case LOOP:
          case CUTOFF:
            {
              colour = hit(0,0,0);
            }
            break;

          default:
            break;
        }
      }
    } SH_END;
    dump(fsh, "fsh");
    std::cout << "OptimizationLevel: " << ShContext::current()->optimization() << std::endl;

    if(debugMode == NORMAL) {
      ShProgram light = ShKernelLight::pointLight<ShColor3f>() << lightColor; 
      //ShProgram lightNShade = ShKernelSurface::diffuse<ShColor3f>() << kd << light;
      ShProgram lightNShade = ShKernelSurface::phong<ShColor3f>() << kd << ks << specexp << light;
      fsh = namedConnect(fsh, lightNShade); 
    }
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

bool needrender = false;
void display()
{
  std::cout << "displaying" << std::endl;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  GLUquadric* sph;
  sph = gluNewQuadric();
  gluSphere(sph, sphereRadius.getValue(0), 32, 32);

  glFinish();

  glutSwapBuffers();
  needrender = false;
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
  /*
  displayList = glGenLists(1);
  glNewList(displayList, GL_COMPILE);
  //glutSolidCube(1.0);

  glEndList();
  */

  // set up one plot func (sphere)
  pfs[0] = new InvPF(new Dist2SpherePF());
  /*
  for(int i = 0; i < 3; ++i) {
    pfs[i] = new InvPF(new Dist2SpherePF());
  }
  pfs[3] = new InvPF(new Dist2TubePF());
  */

  // set up names
  for(int i = 0; i < 9; ++i) {
    ostringstream out;
    out << "trans_" << (i+1);
    trans[i].name(out.str());

    ostringstream out2;
    out2 << "scl_" << (i+1);
    scl[i].name(out2.str());
    scl[i] = ShConstAttrib3f(1,1,1);

    ostringstream out3;
    out2 << "fscl_" << (i+1);
    fscl[i].name(out3.str());
    fscl[i] = ShConstAttrib1f(1);
  }


  level.name("level");
  level = 1.0;

  eps.name("epsilon");
  eps = 0.01;

  traceDiff = 0.0f;
  traceDiff.name("traceDiff");

  cutoff.name("cutoff");
  cutoff = 16.0f;

  eyeposm.name("eyeposm");
}

void reshape(int width, int height)
{
  glViewport(0, 0, width, height);
  glPushMatrix();
    camera.glProjection(float(width)/height);
  glPopMatrix();
  setupView();
}


void motion(int x, int y)
{
  if(needrender) return;
  const double factor = 20.0;
  bool changed = false;
  bool viewchanged = false;
  
  double dx = (x - cur_x)/factor;

  if (buttons[GLUT_LEFT_BUTTON] == GLUT_DOWN) {
    if(shiftOn) {
      if(selected != 0) 
        scl[selected-1](0) += dx;
    } else if (ctrlOn) {
      if(selected != 0) 
        fscl[selected-1] += dx;
    } else {
      camera.orbit(cur_x, cur_y, x, y,
                   glutGet(GLUT_WINDOW_WIDTH),
                   glutGet(GLUT_WINDOW_HEIGHT));
      viewchanged = true;
    }
    changed = true;
  }

  if (buttons[GLUT_MIDDLE_BUTTON] == GLUT_DOWN) {
    double dz = (y - cur_y)/factor;
    if(selected == 0) {
      camera.move(0, 0, dz); 
      viewchanged = true;
    } else {
      if(shiftOn) {
        scl[selected-1](1) += dx;
      } else {
        trans[selected-1](2) += dz;
      }
    }
    changed = true;
  }
  if (buttons[GLUT_RIGHT_BUTTON] == GLUT_DOWN) {
    double dy = (cur_y - y)/factor;
    if(selected == 0) {
      camera.move(dx, dy, 0);
      viewchanged = true;
    } else {
      if(shiftOn) {
        scl[selected-1](2) += dx;
      } else {
        trans[selected-1](0) += dx;
        trans[selected-1](1) += dy;
      }
    }
    changed = true;
  }

  if (changed) {
    if(selected != 0) {
      for(int i = 0; i < 9; ++i) {
        if(pfs[i]) {
          cout << "i: " << i << " translate = " << trans[i] << " scale = " << scl[i] << " func_scale = " << fscl[i] << endl;
        }
      }
    }
    if(viewchanged) {
      setupView();
    }
    needrender = true;
    glutPostRedisplay();
  }
  std::cout << cur_x << ", " << cur_y << std::endl;

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

void keyboard(unsigned char k, int x, int y)
{
  shiftOn = glutGetModifiers() & GLUT_ACTIVE_SHIFT;
  ctrlOn = glutGetModifiers() & GLUT_ACTIVE_CTRL;
  if('0' <= k && k <= '9') {
    selected = k - '0'; 
    cout << "Selected ";
    if(selected) {
      cout << (selected - 1);
    } else {
      cout << "None";
    }
    cout << endl;
    return;
  }

  bool reinit = false;
  switch(k)
    {
    case 'q':
    case 'Q':
      exit(0);
      break;

    case 'k': sphereRadius -= 0.1f; break;
    case 'K': sphereRadius += 0.1f; break;

    case 'l': level -= 0.1f; break;
    case 'L': level += 0.1f; break;

    case 'd': traceDiff -= 0.1f; break;
    case 'D': traceDiff += 0.1f; break;

    case 'e': eps *= 0.5f; break;
    case 'E': eps *= 2.0f; break;


    case 'c': cutoff -= 1; break;
    case 'C': cutoff += 1; break;

    case 's': 
     if(selected > 0) {
       pfs[selected - 1] = new InvPF(new Dist2SpherePF()); 
       reinit = true;
     }
     break;

    case 't': 
     if(selected > 0) {
       pfs[selected - 1] = new InvPF(new Dist2TubePF()); 
       reinit = true;
     }
     break;

    case 'p': 
     if(selected > 0) {
       pfs[selected - 1] = new InvPF(new DistPlanePF()); 
       reinit = true;
     }
     break;

    case 'a':
     if(selected > 0) {
       // some constants taken from shrike;
       ShAttrib1f maxlvl=5;
       ShAttrib1f julia_max = 2.0;
       ShAttrib2f c(0.54,-0.51);
       pfs[selected - 1] = new JuliaPF(maxlvl, julia_max, c);
       reinit = true;
     }
     break;

    case 'h':
     if(selected > 0) {
       pfs[selected - 1] = new InvPF(new HeightFieldPF("heightfield.png"));
       reinit = true;
     }
     break;

    case 'n':
     if(selected > 0) {
       pfs[selected - 1] = 0;
       reinit = true;
     }
     break;

    case 'r': // resets view
     camera.reset();
     camera.move(0.0, 0.0, -15.0);
     setupView();
     break;
     

    case 'm': 
     debugMode = static_cast<DebugMode>(debugMode - 1); 
     if(debugMode == DEBUG_START) {
       debugMode = static_cast<DebugMode>(DEBUG_END - 1);
     }
     reinit = true;
     break;

    case 'M': 
     debugMode = static_cast<DebugMode>(debugMode + 1); 
     if(debugMode == DEBUG_END) {
       debugMode = NORMAL;
     }
     reinit = true;
     break;
  }

  cout << "Sphere Radius: " << sphereRadius.getValue(0) << endl;
  cout << "Level: " << level.getValue(0) << endl;
  cout << "Epsilon : " << eps.getValue(0) << endl;
  cout << "Trace Extra : " << traceDiff.getValue(0) << endl;
  cout << "Loop Cutoff : " << cutoff.getValue(0) << endl;

  if(reinit) {
    initShaders();
    for(int i = 0; i < 9; ++i) {
      if(pfs[i]) {
        cout << "PF " << i << ": " << pfs[i]->name() << endl; 
      }
    }
   cout << "Debug Mode: " << DebugModeName[debugMode] << endl; 
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
  glutKeyboardUpFunc(keyboardUp);
  
  shSetBackend("arb");


  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_VERTEX_PROGRAM_ARB);
  glEnable(GL_FRAGMENT_PROGRAM_ARB);
  glEnable(GL_CULL_FACE);
  // stupid hack - we only need one surface of the sphere anyway, so let's
  // enable *front* face culling (the ray generation works for either
  // front/back face)
  glCullFace(GL_FRONT);

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
