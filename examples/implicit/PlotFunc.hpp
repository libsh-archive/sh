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
    Program f = funcProgram(inName, outName);
    Variable p = *f.begin_inputs();  
    Program dfdx = differentiate(f, p(0));  
    Program dfdy = differentiate(f, p(1));  
    Program dfdz = differentiate(f, p(2));  

    Program result = SH_BEGIN_PROGRAM() {
      InputPoint3f SH_NAMEDECL(in, inName);
      OutputAttrib3f SH_NAMEDECL(out, outName);
      out(0) = dfdx(in); 
      out(1) = dfdy(in); 
      out(2) = dfdz(in); 
      out = normalize(out);
    } SH_END;

    return result;
  }

  template<typename T>
  Program nullProgram(std::string inName, const T& value) { 
    Program result = SH_BEGIN_PROGRAM() {
      InputPoint3f SH_NAMEDECL(in, inName);
      typename T::OutputType out;
      out = value;
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
  virtual Color3f color(Point3f p) = 0; 

  protected:
    std::string m_name;
};
typedef Pointer<PlotFunc> PlotFuncPtr;

/* Unit Sphere generator v = x^2 + y^2 + z^2 */
struct Dist2SpherePF: PlotFunc {
  Color3f &m_color;
  Dist2SpherePF(Color3f& color): PlotFunc("sphere"), m_color(color) {}
  Attrib1f func(Point3f point) { return point | point; }
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

  Color3f color(Point3f point) { return m_color; }
};

/* drop shape */ 
struct GumdropTorusPF: PlotFunc {
  Color3f &m_color;
  GumdropTorusPF(Color3f& color): PlotFunc("mitchell"), m_color(color) {}
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

  Color3f color(Point3f point) { return m_color; }
};

struct SuperquadricPF: PlotFunc {
  Color3f &m_color;
  Attrib1f m_power;
  SuperquadricPF(Color3f& color, float power): PlotFunc("superquad"), m_color(color), m_power(power) {}
  Attrib1f func(Point3f point) {
    Attrib3f ap = abs(point);
    return sum(SH::pow(ap, m_power));
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

  protected:
    Attrib1f &m_n;
};
#endif

/* 4D Julia set
 *
 */
struct JuliaPF: PlotFunc {
  JuliaPF(Attrib1f maxlvl, Attrib1f julia_max, Attrib4f c, Color3f color): 
    PlotFunc("julia"), m_maxlvl(maxlvl), m_julia_max(julia_max), m_c(c), m_color(color) {}

  /* some complex julia set code taken from shrike */ 
  Attrib1f func(Point3f point) {
    Attrib4f SH_DECL(z); 
    /* use 0,1,2 as weights for 3 4D  basis vectors  instead */
    z(0,1,2) = point;
    z(3) = 0;

    //Attrib1f SH_DECL(count) = 0.0f;
    //SH_DO {
    for(int i = 0; i < 3; ++i) {
      Attrib4f zp;
      zp(0) = z(0) * z(0) - z(1,2,3) | z(1,2,3); 
      zp(1) = 2 * z(0) * z(1);  
      zp(2) = 2 * z(0) * z(2);  
      zp(3) = 2 * z(0) * z(3);  
      z = zp + m_c;
    }
    // can't do a count variable in here cause it gets turned into an interval
    //} SH_UNTIL (/*count < m_maxlvl && */(z(0) * z(0)) < m_julia_max); 
    return z | z; 
  }

  Color3f color(Point3f point) {
    return m_color; 
  }

  protected:
    Attrib1f m_maxlvl;
    Attrib1f m_julia_max;
    Attrib4f m_c;
    Color3f m_color;
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
  Dist2CylinderPF(Color3f& color) : PlotFunc("cyl"), m_color(color) {}
  Attrib1f func(Point3f point) { return point(0,1) | point(0,1);  }
  Color3f color(Point3f point) { return m_color; } 
};

/* Line segment generator (vertical line from (0,0) to (0,end))*/ 
struct Dist2LinePF: PlotFunc {
  Color3f &m_color;
  float m_end;
  Dist2LinePF(Color3f& color, float end=1) : PlotFunc("line"), m_color(color), m_end(end) {}
  Attrib1f func(Point3f point) { 
    Attrib1f useStart = point(1) < 0; 
    Attrib1f useEnd = point(1) > m_end; 

    Vector3f dstart = point; 
    Vector3f dend = point - ConstPoint3f(0, m_end, 0);

    Attrib1f result = point(0,2) | point(0,2);
    Attrib1f startResult = dstart | dstart; 
    Attrib1f endResult = dend | dend; 

    return lerp(useStart, startResult, lerp(useEnd, endResult, result)); 
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

  Color3f color(Point3f point) { return m_color; } 
};

/* Plane Generator (Infinite) v = y */
struct DistPlanePF: PlotFunc {
  Color3f &m_color;
  DistPlanePF(Color3f& color) : PlotFunc("plane"), m_color(color) {}
  Attrib1f func(Point3f point) { return point(2); }
  Color3f color(Point3f point) { return m_color; } 
};

/* Inverts any other generator */
struct InvPF: PlotFunc {
  InvPF(PlotFuncPtr pf): PlotFunc("inv_" + pf->name()), m_pf(pf) {}
  Attrib1f func(Point3f point) { return rcp(m_pf->func(point)); }
  Color3f color(Point3f point) { return m_pf->color(point); }
  protected:
    PlotFuncPtr m_pf;
};

/* Gaussian - Blinn's original blobby function */ 
struct GaussianPF: PlotFunc {
  GaussianPF(PlotFuncPtr pf): PlotFunc("G" + pf->name()), m_pf(pf) {}
  Attrib1f func(Point3f point) { return 2 * exp(-m_pf->func(point)); }
  Color3f color(Point3f point) { return m_pf->color(point); }
  protected:
    PlotFuncPtr m_pf;
};

struct NullPF: PlotFunc {
  NullPF(PlotFuncPtr pf): PlotFunc("N" + pf->name()), m_pf(pf) {}
  Attrib1f func(Point3f point) { return m_pf->func(point); } 
  Color3f color(Point3f point) { return m_pf->color(point); }
  protected:
    PlotFuncPtr m_pf;
};

/* Twist along y-axis 
 * twist(f)(x, y, z) = f(x', y, z') 
 * where x' = x cos(4 pi y) - z sin(4 pi y)
 * and y' = x sin(4 pi y) + z cos(4 pi y)
 * */ 
struct TwistPF: PlotFunc {
  protected:
    PlotFuncPtr m_pf;
    int m_axis;
    float m_amount;
  public:
  TwistPF(PlotFuncPtr pf, int axis, float amount): PlotFunc("Tw" + pf->name()), m_pf(pf), m_axis(axis), m_amount(amount) {}
  
  Point3f getpp(const Point3f& p) {
    Point3f pp; 
    Attrib1f angle = m_amount * p(m_axis);
    Attrib1f ca = cos(angle);
    Attrib1f sa = sin(angle);
    switch(m_axis) {
      case 0: // rotate around x
          pp(0) = p(0);
          pp(1) = p(1) * ca + p(2) * sa; 
          pp(2) = -p(1) * sa + p(2) * ca;

          break;
      case 1: // rotate around y
          pp(0) = p(0) * ca - p(2) * sa; 
          pp(1) = p(1);
          pp(2) = p(0) * sa + p(2) * ca;

          break;

      case 2: // rotate around z
          pp(0) = p(0) * ca + p(1) * sa; 
          pp(1) = -p(0) * sa + p(1) * ca;
          pp(2) = p(2);
          break;
    }
    return pp;
  }

  Attrib1f func(Point3f point) { 
    return m_pf->func(getpp(point));
  }

  Color3f color(Point3f point) { return m_pf->color(point); }
};

/* Sin distortion 
 * sin(f)(p) = f(p')
 * where p' = p except
 * p'.y = p.y + amount * sin(scale * p.x) * sin(scale * p.z); 
 */
struct SinPF: PlotFunc {
  protected:
    PlotFuncPtr m_pf;
    int m_axis;
    float m_amount;
    float m_scale;
  public:
  SinPF(PlotFuncPtr pf, int axis, float amount, float scale): PlotFunc("Sin" + pf->name()), m_pf(pf), m_axis(axis), m_amount(amount), m_scale(scale) {}

  Point3f getpp(const Point3f& p) {
    Point3f pp = p;
    int i = (m_axis + 2) % 3;
    int j = (m_axis + 1) % 3;

    pp(m_axis) += m_amount * sin(m_scale * p(i)) * sin(m_scale * p(j)); 
    return pp;
  }

  Attrib1f func(Point3f point) { 
    Point3f pp = getpp(point);
    return m_pf->func(pp);
  }

  Color3f color(Point3f point) { return m_pf->color(getpp(point)); }
};

/* should just allow arbitrary matrices */
/* Translates any other generator */
struct TranslatePF: PlotFunc {
  TranslatePF(PlotFuncPtr pf, Vector3f& trans)  
    : PlotFunc("T" + pf->name()), m_pf(pf), m_trans(trans) {}
  Attrib1f func(Point3f point) { return m_pf->func(point - m_trans); } 
  Color3f color(Point3f point) { return m_pf->color(point - m_trans); }
  protected:
    PlotFuncPtr m_pf;
    Vector3f &m_trans;
};

struct ScalePF: PlotFunc {
  ScalePF(PlotFuncPtr pf, Attrib3f& scl)  
    : PlotFunc("S" + pf->name()), m_pf(pf), m_scl(scl) {}
  Attrib1f func(Point3f point) { return m_pf->func(point * m_scl); } 
  Color3f color(Point3f point) { return m_pf->color(point * m_scl); }
  protected:
    PlotFuncPtr m_pf;
    Attrib3f &m_scl;
};

struct FuncScalePF: PlotFunc {
  FuncScalePF(PlotFuncPtr pf, Attrib1f& scl)  
    : PlotFunc("F" + pf->name()), m_pf(pf), m_scl(scl) {}
  Attrib1f func(Point3f point) { return m_scl * m_pf->func(point); } 
  Color3f color(Point3f point) { return m_pf->color(point); }
  protected:
    PlotFuncPtr m_pf;
    Attrib1f &m_scl;
};

struct TransformPF: PlotFunc {
  TransformPF(PlotFuncPtr pf, Matrix4x4f init=Matrix4x4f())
    : PlotFunc("Tr" + pf->name()), m(init), m_pf(pf) {}
  Attrib1f func(Point3f point) { return m_pf->func(m | point); } 
  Color3f color(Point3f point) { return m_pf->color(m | point); }
  Matrix4x4f m; 
  protected: 
    PlotFuncPtr m_pf;
};

struct NoisePF: PlotFunc {
  NoisePF(PlotFuncPtr pf, const Attrib1f& scl)  
    : PlotFunc("Pn" + pf->name()), m_pf(pf), m_scl(scl) {}
  Point3f perturb(Point3f point) { return point + m_scl * sperlin<3>(point, false); }
  Attrib1f func(Point3f point) { return m_pf->func(perturb(point)); } 
  Color3f color(Point3f point) { return m_pf->color(perturb(point)); }
  protected:
    PlotFuncPtr m_pf;
    Attrib1f m_scl;
};

/* Sums up a set of plot functions */
struct SumPF: PlotFunc {
  SumPF(Attrib1f& level): PlotFunc("Sum"), level(level)  {}

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
    m_name += "__" + pf->name();
  }

  protected:
    typedef std::vector<PlotFuncPtr> PfVec;
    PfVec m_pfvec;
    Attrib1f& level;
};
typedef Pointer<SumPF> SumPFPtr;

#endif
