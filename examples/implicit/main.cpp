#include <cassert>
#include <complex>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <sh/sh.hpp>
#include <sh/Optimizations.hpp>
#include <shutil/shutil.hpp>
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include "Camera.hpp"
#include "Timer.hpp"
#include "ScalarStack.hpp"
#include "ColorFinder.hpp"

// Hit space while the program is running to get a help message 

// USE_STACK means use a tuple for a stack and do typical branch and bound
// (subdivide by two each time)
//
// USE_STACK false means don't keep a stack.  
// Just keep start, mid, end and move mid closer to start by half
// if there is potentially a hit in [start,mid], else continue on [mid,end]
#define USE_STACK false 
#define DUMP_SYMS false 

using namespace std;
using namespace SH;
using namespace ShUtil; 

bool shiftOn;
bool ctrlOn;
Matrix4x4f mv, inv_mv, mvd;
Point3f lightPos;
Point3f eyeposm;
Attrib1f level;
Attrib1f eps;
Attrib1f traceEps;
ConstAttrib1f singularityScale = 10; // Use singularityScale * eps to remove 1/0 singularities in the implicit function 
Attrib1f traceDiff;
Attrib1f cutoff;
Camera camera;
Program vsh, fsh;
Attrib1f sphereRadius = 8.0;
bool showTiming = true;
bool showAaDebug = false;
string defaultBackend = "arb";

Timer timer;

enum RangeMode {
  AA,
  AA_NOUC,
  IA
} rangeMode = IA;

const char* RangeModeName[] = {
  "Affine Arithmetic",
  "Affine Arithmetic (Unique Condensation Off)",
  "Interval Arithmetic"
};

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

void dump(Program &foo, string desc) {
  foo.node()->dump(desc);
}

void printAll(const string& message, Program& foo) {
  cout << message << endl;
  cout << "Interface:" << endl;
  cout << foo.describe_interface() << endl;
  cout << "Binding spec:" << endl;
  cout << foo.describe_binding_spec() << endl;
  assert(foo.binding_spec.size() == foo.stream_inputs.size() + foo.uniform_inputs.size());
}

void startTimer(string msg) {
  timer = Timer::now();
  if(showTiming) {
    std::cout << "Timer " << msg << " "; 
  }
}

void printElapsed(string msg) {
  double elapsed = (Timer::now() - timer).value() / 1000; 
  if(showTiming) {
    cout << " elapsed " << msg << " " << elapsed << " = " << (1.0 / elapsed) << "Hz" << endl;
  }
}

/* This is used to group a function and its gradient
 * It would not be needed if we had built-in automatic differentiatin, 
 * but right now it's just a bit of extra cruft.
 */
struct PlotFunc: public RefCountable {
  PlotFunc(string name): m_name(name) {}
  virtual ~PlotFunc() {}

  string name() { return m_name; }

  Program funcProgram(string inName="in", string outName="out") {
    Program result = SH_BEGIN_PROGRAM() {
      InputPoint3f SH_NAMEDECL(in, inName);
      OutputAttrib1f SH_NAMEDECL(out, outName);
      out = func(in);
    } SH_END;
    return result;
  }

  Program gradProgram(string inName="in", string outName="out") {
    Program result = SH_BEGIN_PROGRAM() {
      InputPoint3f SH_NAMEDECL(in, inName);
      OutputAttrib3f SH_NAMEDECL(out, outName);
      out = grad(in);
    } SH_END;
    return result;
  }

  Program colorProgram(string inName="in", string outName="out") {
    Program result = SH_BEGIN_PROGRAM() {
      InputPoint3f SH_NAMEDECL(in, inName);
      OutputColor3f SH_NAMEDECL(out, outName);
      out = color(in);
    } SH_END;
    return result;
  }

  virtual Attrib1f func(Point3f p) = 0; 
  virtual Attrib3f grad(Point3f p) = 0; 
  virtual Color3f color(Point3f p) = 0; 

  protected:
    string m_name;
};
typedef Pointer<PlotFunc> PlotFuncPtr;

/* Unit Sphere generator v = x^2 + y^2 + z^2 */
struct Dist2SpherePF: PlotFunc {
  Color3f &m_color;
  Dist2SpherePF(Color3f& color): PlotFunc("dist2_sphere"), m_color(color) {}
  Attrib1f func(Point3f point) { return point | point/* + singularityScale * eps*/; }
  Attrib3f grad(Point3f point) { return 2 * point; }
  Color3f color(Point3f point) { return m_color; }
};

/* (1-r^2)^n function that mike mentioned  
 * Look up the proper source for this distance function */
#if 0
struct MikeSpherePF: PlotFunc {
  MikeSpherePF(Attrib1f &n): PlotFunc("mike_sphere"), m_n(n) {}
  Attrib1f func(Point3f point) {
    Attrib1f oneMinusR = 1 - point | point;
    // @todo range 
    // hard coded n = 3... for now until we have pow working
    return oneMinusR * oneMinusR * oneMinusR; 
  }

  Attrib3f grad(Point3f point) { 
    Attrib3f r2 = point * point;
    Attrib3f oneMinusR = ConstAttrib3f(1, 1, 1) - r2; 
    return 2 * oneMinusR * oneMinusR * -2 * r2; 
  }

  protected:
    Attrib1f &m_n;
};
#endif

/* treats x, y as the real/im in juliabrot
 * result is the real part after max_level iterations
 * (cuts off early if magnitude > julia_max)
 *
 * @todo this is really slow and doesn't really work...
 */
struct JuliaPF: PlotFunc {
  JuliaPF(Attrib1f &maxlvl, Attrib1f &julia_max, Attrib2f &c, Color3f &color): 
    PlotFunc("julia"), m_maxlvl(maxlvl), m_julia_max(julia_max), m_c(c), m_color(color) {}

  /* some complex julia set code taken from shrike */ 
  Attrib1f func(Point3f point) {
    Attrib2f SH_DECL(z) = point(0,1);
    //Attrib1f SH_DECL(count) = 0.0f;
    //SH_DO {
    for(int i = 0; i < 5; ++i) {
      Attrib2f zp;
      zp(0) = z(0) * z(0) - z(1) * z(1); 
      zp(1) = 2 * z(0) * z(1);
      z = zp + m_c;
    }
    // can't do a count variable in here cause it gets turned into an interval
    //} SH_UNTIL (/*count < m_maxlvl && */(z(0) * z(0)) < m_julia_max); 
    return z | z; 
  }

  Attrib3f grad(Point3f point) { 
    return ConstAttrib3f(1,0,0);
  }
  Color3f color(Point3f point) {
    return m_color; 
  }

  protected:
    Attrib1f &m_maxlvl;
    Attrib1f &m_julia_max;
    Attrib2f &m_c;
    Color3f &m_color;
};

/* Euclidean Distance from textured square 
 *
 * (Wow this is damned simple...) */
/*
struct HeightFieldPF: PlotFunc {
  Color3f &m_color;
  // given a filename to a black and white image
  HeightFieldPF(string filename, Color3f &color): PlotFunc("heightfield"), m_color(color) { 
    Image img;
    img.loadPng(filename);
    size = img.width();
    m_tex.size(img.width(), img.height());
    m_tex.memory(img.memory());
  }

  Attrib1f func(Point3f point) {
    TexCoord2f tc = (size * 0.25) *  (point(0,1) + ConstAttrib2f(0.5f, 0.5f));
    return m_tex[tc] - point(2);
  }

  Attrib3f grad(Point3f point) {
    Attrib3f grad;
    TexCoord2f tc = (size * 0.25) *  (point(0,1) + ConstAttrib2f(0.5f, 0.5f));

    // @todo instead of just putting 512 here use the height and stuff
    Attrib1f val = m_tex[tc];
    grad(0) = (size * 0.25) * (val - m_tex[tc + ConstAttrib2f(1, 0)]);
    grad(1) = (size * 0.25) * (val - m_tex[tc + ConstAttrib2f(0, 1)]); 
    grad(2) = ConstAttrib1f(1.0);
    // @todo
    return grad;
  }
  Color3f color(Point3f point) { 
    return m_color; 
  }

  private:
    Interp<0, TextureRect<Color1f> > m_tex;
    double size;
};
*/


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


/* Unit Cylinder Generator (Infinite length) v = x^2 + y^2 */ 
struct Dist2CylinderPF: PlotFunc {
  Color3f &m_color;
  Dist2CylinderPF(Color3f& color) : PlotFunc("cylinder"), m_color(color) {}
  Attrib1f func(Point3f point) { return point(0,1) | point(0,1)/* + singularityScale * eps*/;  }
  Attrib3f grad(Point3f point) { 
    Attrib3f result = ConstAttrib3f(0,0,0);
    result(0,1) = 2 * point(0,1); 
    return result;
  }
  Color3f color(Point3f point) { return m_color; } 
};

/* Torus Generator 
 * (No parameters. Use point scaling and function scaling ops later to change
 * inner/outer radii) */ 
struct Dist2TorusPF: PlotFunc {
  static const float radiusSquared = 1.61803399; // golden ratio...huzzah!
  Color3f &m_color;
  Dist2TorusPF(Color3f& color) : PlotFunc("torus"), m_color(color) {}
  Attrib1f func(Point3f point) { 
    Attrib1f pp = point | point;
    Attrib1f ppPlusRadius = pp + radiusSquared; 
    Attrib1f ppxy = point(0,1) | point(0,1);
    return ppPlusRadius * ppPlusRadius - 4.0f * radiusSquared * ppxy; 
    //return point | point + radius * radius - 4.0f * radius * sqrt(point(1,2) | point(0,2)) + eps; 
  }

  Attrib3f grad(Point3f point) { 
    Attrib1f pp = point | point;
    Attrib1f ppPlusRadius = pp + radiusSquared; 
    Attrib3f result = 4 * ppPlusRadius * point;
    result(0,1) -= 8 * radiusSquared * point(0,1);
    return result;
  }
  Color3f color(Point3f point) { return m_color; } 
};

/* Plane Generator (Infinite) v = y */
struct DistPlanePF: PlotFunc {
  Color3f &m_color;
  DistPlanePF(Color3f& color) : PlotFunc("plane"), m_color(color) {}
  Attrib1f func(Point3f point) { return point(2); }
  Attrib3f grad(Point3f point) { return ConstAttrib3f(0,0,1); }
  Color3f color(Point3f point) { return m_color; } 
};

/* Inverts any other generator */
struct InvPF: PlotFunc {
  InvPF(PlotFuncPtr pf): PlotFunc("inv_" + pf->name()), m_pf(pf) {}
  Attrib1f func(Point3f point) { return rcp(m_pf->func(point)); }

  /* Use quotient rule (f/g)' = (g'f - f'g) / g^2 
   * Here f = 1 */ 
  Attrib3f grad(Point3f point) { 
    Attrib1f invg = func(point);
    Attrib3f gp = m_pf->grad(point);
    return gp * invg * invg; 
  }

  Color3f color(Point3f point) { return m_pf->color(point); }
  protected:
    PlotFuncPtr m_pf;
};

/* should just allow arbitrary matrices */
/* Translates any other generator */
struct TranslatePF: PlotFunc {
  TranslatePF(PlotFuncPtr pf, Vector3f& trans)  
    : PlotFunc("trans_" + pf->name()), m_pf(pf), m_trans(trans) {}
  Attrib1f func(Point3f point) { return m_pf->func(point - m_trans); } 
  Attrib3f grad(Point3f point) { return m_pf->grad(point - m_trans); } 
  Color3f color(Point3f point) { return m_pf->color(point - m_trans); }
  protected:
    PlotFuncPtr m_pf;
    Vector3f &m_trans;
};

struct ScalePF: PlotFunc {
  ScalePF(PlotFuncPtr pf, Attrib3f& scl)  
    : PlotFunc("scl_" + pf->name()), m_pf(pf), m_scl(scl) {}
  Attrib1f func(Point3f point) { return m_pf->func(point * m_scl); } 
  Attrib3f grad(Point3f point) { return m_pf->grad(point * m_scl); } 
  Color3f color(Point3f point) { return m_pf->color(point * m_scl); }
  protected:
    PlotFuncPtr m_pf;
    Attrib3f &m_scl;
};

struct FuncScalePF: PlotFunc {
  FuncScalePF(PlotFuncPtr pf, Attrib1f& scl)  
    : PlotFunc("funcscl_" + pf->name()), m_pf(pf), m_scl(scl) {}
  Attrib1f func(Point3f point) { return m_scl * m_pf->func(point); } 
  Attrib3f grad(Point3f point) { return m_scl * m_pf->grad(point); } 
  Color3f color(Point3f point) { return m_pf->color(point); }
  protected:
    PlotFuncPtr m_pf;
    Attrib1f &m_scl;
};

/* Sums up a set of plot functions */
struct SumPF: PlotFunc {
  SumPF(): PlotFunc("sum") {}

  Attrib1f func(Point3f point) {
    Attrib1f result; 
    for(PfVec::iterator I = m_pfvec.begin(); I != m_pfvec.end(); ++I) {
      if(I == m_pfvec.begin()) {
        result = (*I)->func(point);
      } else {
        result += (*I)->func(point);
      }
    }
    return result;
  }

  Attrib3f grad(Point3f point) {
    Attrib3f result; 
    for(PfVec::iterator I = m_pfvec.begin(); I != m_pfvec.end(); ++I) {
      if(I == m_pfvec.begin()) {
        result = (*I)->grad(point);
      } else {
        result += (*I)->grad(point);
      }
    }
    return result;
  }

  Color3f color(Point3f point) {
    Color3f result;
    ConstAttrib1f ONE(1.0f);

    for(PfVec::iterator I = m_pfvec.begin(); I != m_pfvec.end(); ++I) {
      if(I == m_pfvec.begin()) {
        result = (*I)->color(point) * ((*I)->func(point)); 
      } else {
        result += (*I)->color(point) * ((*I)->func(point)); 
      }
    }

    // @todo - this is not very general, but in this case, we know that
    // at intersection points, the sum of the function values = level, hence
    // this makes the result an affine combination of colors.
    return result * rcp(level); 
  }

  void add(PlotFuncPtr pf) {
    m_pfvec.push_back(pf);
  }

  protected:
    typedef std::vector<PlotFuncPtr> PfVec;
    PfVec m_pfvec;
};
typedef Pointer<SumPF> SumPFPtr;

// given an interval program that computes some (reasonably nice) function g: Attrib1i_f -> Attrib1i_f,
// this computes a root and sets hasroot to true if there is a root in the part of the domain  
Program trace(const Program& func) {
  Program result = SH_BEGIN_PROGRAM() {
    InputPoint3f SH_DECL(origin);
    InputVector3f SH_DECL(dir);
    InputAttrib1f SH_DECL(t);
    OutputAttrib1f SH_DECL(value);
    Attrib3f SH_DECL(p) = mad(t, dir, origin);
    value = func(p);
  } SH_END;
  result.node()->collectAll();
  return result;
}

/* Branch and Bound, with stack or sans stack.
 *
 * Commonly branch and bound approaches use either BFS or DFS (or some hybrid)
 * of the search space.  This is a DFS-style alg, except that we always
 * split the remaining search space in half, so that there's no  
 * need for a stack.
 * 
 * (This has probably been done by someone before...just have to look some more)
 *
 * Convergence compared to regular DFS style in optimal case is the same
 * for a hit (@todo prove), O(1) for stack vs. O(log(n)) for miss.
 * Otherwise, it depends on convergence rate of the type of range arithmetic.
 * (TODO proof that with "well behaved" functions, convergence is in the 
 * same order as the usual stack based alg with a small constant)
 * (TODO define "well behaved")
 */ 
Program firsthit(Program tracer, bool use_aa) {
  Program i_tracer; 
  if(use_aa) {
    i_tracer = affine_inclusion(tracer);
    i_tracer.name("i_tracer_aa");
    dump(i_tracer, "i_tracer_aa");
  } else {
    i_tracer = inclusion(tracer);
    i_tracer.name("i_tracer_ia");
    dump(i_tracer, "i_tracer_ia");
  }
  if(DUMP_SYMS) {
    AaVarSymsMap empty;
    ostringstream sout;
    switch(rangeMode) {
        case IA: sout << "IA_"; break;
        case AA: sout << "AA_"; break;
        case AA_NOUC: sout << "AA-nouc_"; break;
    }
    sout << DebugModeName[debugMode] << "_";
    sout << "symfile.csv";
    i_tracer.meta("aa_symfile", sout.str());
    cout << "dumping symfile " << sout.str() << endl;
    placeAaSyms(i_tracer.node(), empty, true);
    cout << "done dumping symfile" << endl;
  }

  Context::current()->disable_optimization("propagation");
  Program result = SH_BEGIN_PROGRAM() {
    InputAttrib1i_f SH_DECL(range); // range to search

    OutputAttrib1f SH_DECL(hashit); // bool to indicate if there was a hit
    OutputAttrib1f SH_DECL(hit); // hit location

    Attrib1f SH_DECL(start) = range_lo(range);
    Attrib1f SH_DECL(end) = range_hi(range);
    Attrib1f SH_DECL(mid) = lerp(0.5f, start, end); 

    Attrib1f SH_DECL(count) = 0.0f;

    // double up the loops so that in the GPU case, we can break the 255 loop
    // limit

#if USE_STACK
    ScalarStack<16, float> stack;
    stack.push(end);
    stack.push(start);

    SH_WHILE(stack.count() >= 2) {
    SH_WHILE(stack.count() >= 2) {
      start = stack.top();
      stack.pop();
      mid = stack.top();
#else

    // Check for hits until feasible range drops below eps
    SH_WHILE(mid - start > eps && end - mid > eps) { 
    SH_WHILE(mid - start > eps && end - mid > eps) { 
#endif
      count += 1.0f;
      Attrib1i_f SH_DECL(traceRange) = make_interval(start, mid);
      Attrib1i_f SH_DECL(traceResult);
      traceResult = i_tracer(traceRange);

      // If there's a potential hit, subdivide [start,mid], 
      // else discard and continue with [mid,end]
      // Attrib1f hitcond = range_lo(range_contains(traceResult, 0));
      Attrib1f hitcond = range_lo(traceResult) < traceEps && -traceEps < range_hi(traceResult);
#if USE_STACK
      SH_IF(hitcond) {
        stack.push(lerp(0.5f, start, mid));
        stack.push(start);
      } SH_ENDIF; 
#else
      SH_IF(hitcond) {
        mid = lerp(0.5f, start, mid);
      } SH_ELSE {
        start = mid;
        mid = lerp(0.5f, mid, end);
      } SH_ENDIF;
      //start = lerp(hitcond, start, mid);
      //mid = lerp(0.5f, mid, lerp(hitcond, start, end)); 
#endif
      if(debugMode == CUTOFF) {

        // do something to end the loop
        end = lerp(count > cutoff, mid - 1.0, end);
      }
    } SH_ENDWHILE;
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
  Context::current()->enable_optimization("propagation");
  dump(result, "firsthit");
  std::cout << "Optimizing firsthit" << endl;
  optimize(result);
  dump(result, "firsthit-prop");
  return result;
}

Color3f color[9];
Vector3f trans[9];
Attrib3f scl[9];
Attrib1f fscl[9];
PlotFuncPtr pfs[9];
int selected=0; // 0 = no selection (mouse interacts with view), 1-9 means selected object i-1


void initShaders() {
  try {
    bool use_aa = (rangeMode != IA);

    startTimer("Building Shaders");

    vsh = KernelLib::vsh(mv, mvd);
    lightPos.name("lightPos");
    vsh = vsh << extract("lightPos") << lightPos;

    printElapsed("Build vsh");

    Color3f SH_DECL(ks) = Color3f(0.3, 0.3, 0.3);
    Attrib1f SH_DECL(specexp) = 64.0f;

    // set up some desaturated colors 
    // @todo better colors? different color space for nicer blending?
    srand48(time(NULL));
    for(int i = 0; i < 9; ++i) {
      color[i] = ColorFinder::color(ColorFinder::Set1, 9, i); 
    }

    /* Set flags */
    switch(rangeMode) {
      case AA: Context::current()->set_flag("aa_enable_um", true); break; 
      case AA_NOUC: Context::current()->set_flag("aa_enable_um", false); break; 
      case IA: Context::current()->set_flag("aa_enable_um", true); break; 
    }

    // Build the plot function 
    SumPFPtr plotfunc = new SumPF();
    for(int i = 0; i < 9; ++i) {
      if(pfs[i]) {
        plotfunc->add(new FuncScalePF(new TranslatePF(new ScalePF(pfs[i], scl[i]), trans[i]), fscl[i]));
      }
    }

    Program func = plotfunc->funcProgram("posm");
    Program gradient = plotfunc->gradProgram("posm");
    Program colorFunc = plotfunc->colorProgram("posm");

    printElapsed("Build plot functions (function, gradient, color)");

    Color3f SH_DECL(lightColor) = ConstAttrib3f(0.75, .75, .75);

    fsh = SH_BEGIN_FRAGMENT_PROGRAM {
      InputPoint3f SH_DECL(posm);
      InputVector3f SH_DECL(viewVec);
      InputPosition4f SH_DECL(posh);

      Vector3f SH_DECL(dir) = posm - eyeposm;
      Attrib1f SH_DECL(dist) = length(dir);
      dir = normalize(dir);

      cout << "Building tracer" << endl;
      Program tracer = trace(func)(eyeposm, dir);
      tracer = add<Attrib1f>() << -level << tracer;

      cout << "Building firsthit" << endl;
      Program hitter = firsthit(tracer, use_aa);

      Attrib1f SH_DECL(trace_start); 
      Attrib1f SH_DECL(trace_end) = dist + 2.0  * (dir | -posm) + traceDiff;

      // handle if we're inside the sphere
      trace_start = SH::min(dist, trace_end);
      trace_end = SH::max(dist, trace_end);

      Attrib1i_f SH_DECL(range) = make_interval(trace_start, trace_end);  // @todo figure out where it exits the sphere
      Attrib1f SH_DECL(hashit);
      Attrib1f SH_DECL(hit);
      if(debugMode != FUNC) {
        (hashit & hit) = hitter(range);
      }

      if(debugMode == NORMAL) {
        kill(!hashit);
        Point3f SH_DECL(hitp) = mad(hit, dir, eyeposm);

        OutputNormal3f SH_DECL(normal);
        OutputVector3f SH_DECL(lightVec);
        OutputVector3f SH_DECL(halfVec);
        normal = gradient(hitp);
        normal = mv | normal;
        lightVec = mv | (lightPos - hitp); 
        halfVec = normalize(lightVec + viewVec);

        ConstAttrib1f ONE(1.0f);
        normal *= lerp((normal | viewVec) < 0.0f, -ONE, ONE); 

        OutputColor3f SH_DECL(kd) = colorFunc(hitp);
      } else {
        OutputColor3f SH_DECL(color);
        switch(debugMode) {
          case FUNC:
            {
              Attrib1f SH_DECL(le) = length(eyeposm);
              Attrib1f SH_DECL(t) = le * le / (dir | -eyeposm);
              Attrib1f SH_DECL(funcval);
              Point3f SH_DECL(pos) = mad(t, dir, eyeposm);
              funcval = tracer(t);
              color = colorFunc(pos); 
              color *= funcval;
            }
            break;
          case GRAD:
            {
              kill(!hashit);
              Point3f SH_DECL(hitp) = mad(hit, dir, eyeposm);
              color = gradient(hitp);
            }
            break;
          case HIT:
            color(0) = hashit;  // hit or not
            color(1) = hashit * (hit - dist) / abs(trace_end); // distance to hit
            break;

          case LOOP:
          case CUTOFF:
            {
              color = hit(0,0,0);
            }
            break;

          default:
            break;
        }
      }
    } SH_END;
    fsh.name("fsh");
    std::cout << "OptimizationLevel: " << Context::current()->optimization() << std::endl;

    if(debugMode == NORMAL) {
      std::cout << "foo! lightColor" << endl;
      Program light = KernelLight::pointLight<Color3f>() << lightColor; 

      // kd comes from fsh, pass in the rest of the phong params
      Program lightNShade = KernelSurface::phong<Color3f>() << swizzle("ks", "specExp", "irrad", "kd", "normal", "halfVec", "lightVec", "posh") << ks << specexp << light;
      lightNShade.name("lightnshade");
      dump(lightNShade, "lightnshade");

      fsh = namedConnect(fsh, lightNShade); 
    }

    switch(rangeMode) {
      case AA: fsh.name("fsh_aa"); break;
      case AA_NOUC: fsh.name("fsh_aa_nouc"); break;
      case IA: fsh.name("fsh_ia"); break;
    }
    printElapsed("Build fsh");
    dump(fsh, fsh.name());

    vsh = namedAlign(vsh, fsh);
    dump(vsh, "vsh");
    vsh.name("vsh");

    startTimer("Binding Shaders");
    lock_in_uniforms(vsh);
    lock_in_uniforms(fsh);
    ProgramSetPtr ps = new ProgramSet(vsh, fsh);
    bind(*ps);
    printElapsed("Bind vsh, fsh");
  } catch(const Exception &e) {
    cout << "Error: " << e.message() << endl;
  } catch(...) {
    cout << "Caught unknown error: " << endl;
  }
}

GLuint displayList;

bool needrender = false;
void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  startTimer("Rendering");
  GLUquadric* sph;
  sph = gluNewQuadric();
  gluSphere(sph, sphereRadius.getValue(0), 32, 32);

  glFinish();

  glutSwapBuffers();
  printElapsed("Done Render");
  needrender = false;
}

void setupView()
{
  mv = camera.modelView();
  inv_mv = inverse(mv);
  eyeposm = inv_mv | ConstPoint3f(0.0f, 0.0f, 0.0f); 
  mvd = camera.modelViewProjection(Matrix4x4f());
}

void myinit()
{
  /*
  displayList = glGenLists(1);
  glNewList(displayList, GL_COMPILE);
  //glutSolidCube(1.0);

  glEndList();
  */

  // set up two plot func (sphere)
  float side = 1.51;
  //pfs[0] = new InvPF(new Dist2TorusPF(color[0]));
  pfs[0] = new InvPF(new Dist2SpherePF(color[0]));
  trans[0] = Vector3f(-side, -side * 0.5 * sqrt(3), 0);
  pfs[1] = new InvPF(new Dist2SpherePF(color[1]));
  trans[1] = Vector3f(side, -side * 0.5 * sqrt(3), 0);
  pfs[2] = new InvPF(new Dist2SpherePF(color[2]));
  trans[2] = Vector3f(0, side * 0.5 * sqrt(3), 0);

  /*
  for(int i = 0; i < 3; ++i) {
    pfs[i] = new InvPF(new Dist2SpherePF(color[i]));
  }
  pfs[3] = new InvPF(new Dist2CylinderPF(color[3]));
  */

  // set up names
  for(int i = 0; i < 9; ++i) {
    ostringstream out;
    out << "trans_" << (i+1);
    trans[i].name(out.str());

    ostringstream out2;
    out2 << "scl_" << (i+1);
    scl[i].name(out2.str());
    scl[i] = ConstAttrib3f(1,1,1);

    ostringstream out3;
    out2 << "fscl_" << (i+1);
    fscl[i].name(out3.str());
    fscl[i] = ConstAttrib1f(1);
  }


  level.name("level");
  level = 1.0;

  eps.name("epsilon");
  eps = 0.01;

  traceEps.name("traceEpsilon");
  traceEps = 0.01;

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
          //cout << "i: " << i << " translate = " << trans[i] << " scale = " << scl[i] << " func_scale = " << fscl[i] << " color = " << color[i] << endl;
        }
      }
    }
    if(viewchanged) {
      setupView();
    }
    needrender = true;
    glutPostRedisplay();
  }

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

    case 'v': level -= 0.1f; break;
    case 'V': level += 0.1f; break;

    case 'd': traceDiff -= 0.1f; break;
    case 'D': traceDiff += 0.1f; break;

    case 'e': eps *= 0.5f; break;
    case 'E': eps *= 2.0f; break;

    case 'f': traceEps *= 0.5f; break;
    case 'F': traceEps *= 2.0f; break;


    case 'l': cutoff -= 1; break;
    case 'L': cutoff += 1; break;

    case 's': 
     if(selected > 0) {
       int idx = selected - 1;
       pfs[idx] = new InvPF(new Dist2SpherePF(color[idx])); 
       reinit = true;
     }
     break;

    case 'c': 
     if(selected > 0) {
       int idx = selected - 1;
       pfs[idx] = new InvPF(new Dist2CylinderPF(color[idx])); 
       reinit = true;
     }
     break;

    case 't': 
     if(selected > 0) {
       int idx = selected - 1;
       pfs[idx] = new InvPF(new Dist2TorusPF(color[idx])); 
       //pfs[idx] = new Dist2TorusPF(color[idx]); 
       reinit = true;
     }
     break;

    case 'p': 
     if(selected > 0) {
       int idx = selected - 1;
       pfs[idx] = new InvPF(new DistPlanePF(color[idx])); 
       reinit = true;
     }
     break;

    case 'a':
     if(selected > 0) {
       int idx = selected - 1;
       // some constants taken from shrike;
       Attrib1f maxlvl=5;
       Attrib1f julia_max = 2.0;
       Attrib2f c(0.54,-0.51);
       pfs[idx] = new JuliaPF(maxlvl, julia_max, c, color[idx]);
       reinit = true;
     }
     break;

/*
    case 'h':
     if(selected > 0) {
       int idx = selected - 1;
       pfs[idx] = new InvPF(new HeightFieldPF("heightfield.png", color[idx]));
       reinit = true;
     }
     break;
     */

    case 'n':
     if(selected > 0) {
       pfs[selected - 1] = 0;
       reinit = true;
     }
     break;

    case 'y': // resets view
     camera.reset();
     camera.move(0.0, 0.0, -15.0);
     setupView();
     break;

    
    case 'R': if(selected > 0) { color[selected - 1](0) += 0.05; } break;
    case 'r': if(selected > 0) { color[selected - 1](0) -= 0.05; } break;
    case 'G': if(selected > 0) { color[selected - 1](1) += 0.05; } break;
    case 'g': if(selected > 0) { color[selected - 1](1) -= 0.05; } break;
    case 'B': if(selected > 0) { color[selected - 1](2) += 0.05; } break;
    case 'b': if(selected > 0) { color[selected - 1](2) -= 0.05; } break;

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

    case 'z':
     if(rangeMode == AA) rangeMode = IA;
     else rangeMode = static_cast<RangeMode>(rangeMode - 1);
     reinit = true;
     break;

    case 'Z':
     if(rangeMode == IA) rangeMode = AA;
     else rangeMode = static_cast<RangeMode>(rangeMode + 1);
     reinit = true;
     break;

    case 'x':
     showTiming = !showTiming;
     break;

    case 'w':
      showAaDebug = !showAaDebug;
      Context::current()->set_flag("aa_disable_debug", !showAaDebug);
      break;

    default:
     // print out help
     cout << "Available Interaction:" << endl;
     cout << endl; 
     cout << "  q             Quit" << endl;
     cout << "  m/M           Change rendering mode" << endl;
     cout << "  z/Z           Change range arithmetic mode" << endl;
     cout << "  k/K           Change bounding sphere radius" << endl;
     cout << "  v/V           Change value of level set to intersect" << endl; 
     cout << "  e/E           Change epsilon (determines when to end tracing iterations)" << endl;
     cout << "  f/F           Change trace epsilon (determines how close to level set value roots must be)" << endl; 
     cout << "  l/L           Change loop count cutoff in Loop Cutoff mode" << endl; 
     cout << "  w             Show debug" << endl;
     cout << "  x             Show timings" << endl;
     cout << "  y             Reset view" << endl;
     cout << "  0             Change to view mode (deselect object)" << endl;
     cout << "  1-9           Change to object manipulation mode (plus select an object)" << endl;
     cout << "  Left Button   Rotate" << endl;
     cout << endl;
     cout << "View Mode" << endl;
     cout << "  Middle Button Translate x/y" << endl;
     cout << "  Right Button  Translate z" << endl;
     cout << endl;
     cout << "Object Manipulation Mode" << endl;
     cout << "  s             Make object a sphere (point generator)" << endl;
     cout << "  c             Make object a cylinder (line generator)" << endl;
     cout << "  t             Make object a torus (circle generator)" << endl;
     cout << "  p             Make object a plane (plane generator)" << endl;
     //cout << "  h             Make object a texture mapped heightfield (SLOW)" << endl;
     cout << "  a             Make object a julia fractal (SLOW!!)" << endl;
     cout << "  n             Make object empty" << endl;
     cout << "  r/R, g/G, b/B Change object color" << endl;
     cout << "  Middle Button Translate object x/y" << endl;
     cout << "  Right Button  Translate object z" << endl;
     cout << "  Shift-Left/Mddle/Right Button    Change object x/y/z scaling" << endl;
     cout << "  Ctrl-Left Button Change function scaling" << endl;


  }


  if(reinit) {
    initShaders();
    for(int i = 0; i < 9; ++i) {
      if(pfs[i]) {
        cout << "PF " << i << ": " << pfs[i]->name() << endl; 
      }
    }
   cout << "Debug Mode: " << DebugModeName[debugMode] << endl; 
  }
  cout << "Sphere Radius: " << sphereRadius.getValue(0) << endl;
  cout << "Level: " << level.getValue(0) << endl;
  cout << "Epsilon : " << eps.getValue(0) << " Trace Epsilon: " << traceEps.getValue(0) << endl;
  cout << "Trace Extra : " << traceDiff.getValue(0) << endl;
  cout << "Loop Cutoff : " << cutoff.getValue(0) << endl;
  cout << "Arithmetic Type : " << RangeModeName[rangeMode] << endl; 

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
  
  setBackend(defaultBackend);


  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_VERTEX_PROGRAM_ARB);
  glEnable(GL_FRAGMENT_PROGRAM_ARB);
  glEnable(GL_CULL_FACE);
  // stupid hack - we only need one surface of the sphere anyway, so let's
  // enable *front* face culling (the ray generation works for either
  // front/back face)
  glCullFace(GL_FRONT);

  glClearColor(0.25, 0.25, 0.25, 1.0);
  setupView();

  // Place the camera at its initial position
  camera.move(0.0, 0.0, -15.0);

  // Set up the light position
  lightPos = Point3f(5.0, 5.0, 5.0);
  
  myinit();
  initShaders();
  Context::current()->set_flag("aa_disable_debug", !showAaDebug);
  
  glutMainLoop();
}
