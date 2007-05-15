#ifndef PLOTFUNC_HPP
#define PLOTFUNC_HPP
#include <string>

using namespace SH;

/* This is used to group a function and its gradient
 * It would not be needed if we had built-in automatic differentiatin, 
 * but right now it's just a bit of extra cruft.
 */
struct PlotFunc: public RefCountable {
  PlotFunc(std::string name): m_name(name) {}
  virtual ~PlotFunc() {}

  std::string name() { return m_name; }

  Program funcProgram(std::string inName="in", std::string outName="out") {
    Program result = SH_BEGIN_PROGRAM() {
      InputPoint3f SH_NAMEDECL(in, inName);
      OutputAttrib1f SH_NAMEDECL(out, outName);
      out = func(in);
    } SH_END;
    return result;
  }

  Program gradProgram(std::string inName="in", std::string outName="out") {
    Program result = SH_BEGIN_PROGRAM() {
      InputPoint3f SH_NAMEDECL(in, inName);
      OutputAttrib3f SH_NAMEDECL(out, outName);
      out = grad(in);
    } SH_END;
    return result;
  }

  Program colorProgram(std::string inName="in", std::string outName="out") {
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
    std::string m_name;
};
typedef Pointer<PlotFunc> PlotFuncPtr;

/* Unit Sphere generator v = x^2 + y^2 + z^2 */
struct Dist2SpherePF: PlotFunc {
  Color3f &m_color;
  Dist2SpherePF(Color3f& color): PlotFunc("dist2_sphere"), m_color(color) {}
  Attrib1f func(Point3f point) { return point | point; }
  Attrib3f grad(Point3f point) { return 2 * point; }
  Color3f color(Point3f point) { return m_color; }
};

/* drop shape */ 
struct DropPF: PlotFunc {
  Color3f &m_color;
  DropPF(Color3f& color): PlotFunc("drop"), m_color(color) {}
  Attrib1f func(Point3f point) {
    Attrib1f result = 4 * (point(0,1) | point(0,1));
    Attrib1f z = point(2);
    Attrib1f omz = 1 - z;
    result -= (1 + z) * omz * omz * omz + 1;
    return result;
  }

  Attrib3f grad(Point3f point) {
    Attrib3f result = 2 * point;
    Attrib1f z = point(2);
    Attrib1f omz = 1 - z;
    result(2) = -(omz * omz * omz - 3 * (1 + z) * omz * omz); 
    return result;
  }
  Color3f color(Point3f point) { return m_color; }
};

/* drop shape */ 
struct MitchellPF: PlotFunc {
  Color3f &m_color;
  MitchellPF(Color3f& color): PlotFunc("mitchell"), m_color(color) {}
  Attrib1f func(Point3f point) {
    Attrib1f result; 
    Attrib1f xyz = point | point;
    Attrib1f yz = point(1,2) | point(1, 2);
    Attrib1f yz2 = yz * yz;
    Attrib1f x2 = point(0) * point(0);
    Attrib1f x4 = x2 * x2;

    result = 4 * (x4 + yz2) + 17 * x2 * yz - 20 * xyz + 17 + 1; 
    return result;
  }

  Attrib3f grad(Point3f point) {
    Attrib3f result; 
    Attrib1f xyz = point | point;
    Attrib1f yz = point(1,2) | point(1, 2);
    Attrib1f x = point(0); 
    Attrib1f y = point(1); 
    Attrib1f z = point(2); 
    Attrib1f x2 = x * x;
    Attrib1f x3 = x * x2; 

    result(0) = 16 * x3 +  34 * x * yz - 40 * x ;
    result(1) = 16 * yz  * y + 34 * x2 * y - 40 * y;
    result(2) = 16 * yz * z + 34 * x2 * z - 40 * z; 
    return result;
  }
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
  HeightFieldPF(std::string filename, Color3f &color): PlotFunc("heightfield"), m_color(color) { 
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
  Attrib1f func(Point3f point) { return point(0,1) | point(0,1);  }
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
  //static const float radiusSquared = 1.61803399; // golden ratio...huzzah!
  Color3f &m_color;
  Dist2TorusPF(Color3f& color) : PlotFunc("torus"), m_color(color) {}
  Attrib1f func(Point3f point) { 
    Attrib1f pp = point | point;
    Attrib1f ppxy = point(0,1) | point(0,1);

    /*
    Attrib1f ppPlusRadius = pp + radiusSquared; 
    return ppPlusRadius * ppPlusRadius - 4.0f * radiusSquared * ppxy; 
    */
    return pp - 2 * sqrt(ppxy) + 1;
  }

  Attrib3f grad(Point3f point) { 
    Attrib1f pp = point | point;
    Attrib1f ppxy = point(0,1) | point(0,1);
    /*
    Attrib1f ppPlusRadius = pp + radiusSquared; 
    Attrib3f result = 4 * ppPlusRadius * point;
    result(0,1) -= 8 * radiusSquared * point(0,1);
    return result;
    */
    Attrib3f result = 2 * point;
    result(0, 1) -= 2 * rsqrt(ppxy) * point(0, 1); 
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

/* Gaussian - Blinn's original blobby function */ 
struct GaussianPF: PlotFunc {
  GaussianPF(PlotFuncPtr pf): PlotFunc("gaussian_" + pf->name()), m_pf(pf) {}
  Attrib1f func(Point3f point) { return 2 * exp(-m_pf->func(point)); }

  Attrib3f grad(Point3f point) { 
    Attrib1f gpf = func(point);
    Attrib3f gp = m_pf->grad(point);
    return -gpf * gp; 
  }

  Color3f color(Point3f point) { return m_pf->color(point); }
  protected:
    PlotFuncPtr m_pf;
};

struct NullPF: PlotFunc {
  NullPF(PlotFuncPtr pf): PlotFunc("null_" + pf->name()), m_pf(pf) {}
  Attrib1f func(Point3f point) { return m_pf->func(point); } 

  Attrib3f grad(Point3f point) { return m_pf->grad(point); }

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
  Attrib3f grad(Point3f point) { return m_scl * m_pf->grad(point * m_scl); } 
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
  SumPF(Attrib1f& level): PlotFunc("sum"), level(level)  {}

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
    Attrib1f& level;
};
typedef Pointer<SumPF> SumPFPtr;

#endif
