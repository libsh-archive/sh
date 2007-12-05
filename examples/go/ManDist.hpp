#ifndef MANDIST_HPP
#define MANDIST_HPP

#include <cassert>
#include <cstdarg>

#include <sstream>
#include <iostream>
#include <fstream>
#include <sh/sh.hpp>
#include "man.hpp"
#include "BranchBound.hpp"
#include "ExprParser.hpp"
#include "Timer.hpp"


/* Global minimization operator (all input parameters assumed to range between 0 and 1) 
 *
 * (The branch-and-bound algorithm is executed partly on the host, hence why this doesn't 
 * return a program, as it cannot be executed entirely as a stream program at present) 
 *
 * Returns only one minimum  
 * */
#define MANDIST_DEBUG false

#define DUMP_COUNT 100000

#define COLORDIST false

template<int N>
struct Minimizer {
  typedef SH::Attrib<N, SH::SH_TEMP, float> InType;
  typedef SH::Attrib1f OutType;
  SH::Program ifunc;

  typedef BranchBound<InType, OutType> BBParams;
  typedef BranchBoundIterator<BBParams> BBIterator;
  typedef typename BBParams::IntervalDomain IDomain;
  typedef typename BBParams::IntervalRange IRange; 
  typedef typename BBParams::DomainArray DomainArray;
  //typedef std::vector<typename BBParams::State> ResultVec;

  BBParams bb;
  BBIterator bbIt; 
  IDomain initialBounds; 

  int splitLevel;
  float eps;


  Minimizer() {}
  Minimizer(const Man& objective, float eps=1e-4, bool use_aa=true, bool doSort=false, int splitLevel=1, float range_eps = 1e-2)
    : splitLevel(splitLevel), eps(eps) 
  {
      assert(objective.size_matches(N, 1));
      ifunc = use_aa ? affine_inclusion(objective) : inclusion(objective);
      ifunc.name((use_aa ? "aa_" : "ia_") + objective.name());

      if(MANDIST_DEBUG) ifunc.node()->dump("ifunc");
      if(use_aa) {
        SH::Program foo(ifunc.node()->clone());
        placeAaSyms(foo.node(), true);
      }

      bb = BBParams(objective, ifunc, doSort, splitLevel, eps); 
  }

  /* Performs minimization based on current uniform settings in the ifunc */
  SH::Generic<N, float> doit() {
    //ResultVec results;
    SH::Attrib<N, SH::SH_TEMP, float> result;
    SH::ConstAttrib1i_f bounds(SH::Interval<float>(0.0f, 1.0f));
    for(int i = 0; i < initialBounds.size(); ++i) initialBounds(i) = bounds;
    std::cout << "constructing iterator" << std::endl;
    bbIt = BBIterator(bb, initialBounds);
    std::cout << "done constructing iterator" << std::endl;
    //std::ofstream bbout("bbout");
    Timer start = Timer::now();
    try {
      int it = 0;
      while(!bbIt.done()) {
        bbIt.iterate();
   //     results.push_back(bbIt.current());
   //     dump(it, bbIt);
        it += 1;
      }

    } catch(const SH::Exception &e) {
      std::cout << "Error: " << e.message() << std::endl;
    } catch(...) {
      std::cout << "Caught unknown error: " << std::endl;
    }
    double elapsed = (Timer::now() - start).value() / 1000.0;
    std::cerr << "total time: " << elapsed << "s" << std::endl;
   // bbout.close();
   //dump(0, bbIt);

    return bbIt.singleResult();
  }

  void dump(int i, BBIterator& bbIt) {
#if DUMP_COUNT > 0 
        float prob = 1.0;
        std::ostringstream doutname;
        doutname << "mandist" << i << ".csv";
        std::ofstream dout(doutname.str().c_str());

        // get ranges and display them
        typename BBParams::State r = bbIt.current();
        int count = r.domain.count();
        if(DUMP_COUNT < count) {
          prob = float(DUMP_COUNT) / count;
        }

        for(size_t j = 0; j < r.domain.lo(0).size(); ++j) dout << (j > 0 ? "," : "") << "domain" << j; 
        dout << ",range\n";

        //int k = 0; 
        for(int i = 0; i < count; ++i) {
          if(drand48() > prob) continue;
          //dout << k++;
          for(size_t j = 0; j < r.domain.lo(i).size(); ++j) {
            dout << (j > 0 ? "," : "")<< (r.domain.lo(i)[j] + r.domain.hi(i)[j]) * 0.5;
          }
          dout << "," << (r.range.lo(i)[0] + r.range.hi(i)[0]) * 0.5 << "\n"; 
        }
#endif
  }
};


struct HostManUpdater: public SH::RefCountable {
  virtual ~HostManUpdater() {}
  virtual const SH::Program& getProgram() = 0;
  virtual void update(SH::Program& p) = 0; 
};
typedef SH::Pointer<HostManUpdater> HostManUpdaterPtr;


/* A function where some of the initial computation may need to be done on the host and stored in uniforms
 * (i.e. it can depend on other Man objects, but the update() method needs to be called if they change */ 
struct HostMan: public Man { 
  HostManUpdaterPtr updater;

  HostMan(): updater(0) {} 
  HostMan(HostManUpdaterPtr updater): Man(updater->getProgram()), updater(updater) {
    update();
  } 
  void update() { updater->update(*this); }

};

template<int A, int B, int N> 
struct nearest_points_updater: public HostManUpdater {
public:
  const Man& a; 
  const Man& b; 

  SH::Program p;
  Man objective;
  typedef typename SH::Attrib<N, SH::SH_TEMP, float, SH::SH_POINT> PointNf;
  typedef typename SH::Attrib<N, SH::SH_OUTPUT, float, SH::SH_POINT> OutputPointNf;
  typename SH::Attrib<A + B, SH::SH_TEMP, float> minparms;
  int aswiz[A];
  int bswiz[B];
  int nswiz[N];
  Minimizer<A + B> minimizer;
  float domain_eps;
  bool use_aa;


  nearest_points_updater(const Man& a, const Man& b, float domain_eps, bool use_aa): a(a), b(b), domain_eps(domain_eps), use_aa(use_aa) {
    typedef typename SH::Attrib<A + B, SH::SH_INPUT, float> InType; 
    typedef typename SH::Attrib<A, SH::SH_INPUT, float> InTypeA; 
    typedef typename SH::Attrib<A, SH::SH_INPUT, float> InTypeB; 
    typedef typename SH::Attrib<4, SH::SH_OUTPUT, float> OutType; 

    if(MANDIST_DEBUG) a.node()->dump("npu_a");
    if(MANDIST_DEBUG) b.node()->dump("npu_b");

    assert(A+B > 0 && N <= 3); 

    for(int i = 0; i < A; ++i) aswiz[i] = i;
    for(int i = 0; i < B; ++i) bswiz[i] = A + i;
    for(int i = 0; i < N; ++i) nswiz[i] = i; 

    SH::Program foo = SH_BEGIN_PROGRAM() {
      InType SH_DECL(in);
      SH::OutputAttrib1f SH_DECL(dist);

      typename OutputPointNf::TempType pa, pb, diffab;
      pa = A > 0 ? a(in.template swiz<A>(aswiz)) : a; 
      pb = B > 0 ? b(in.template swiz<B>(bswiz)) : b; 
      //dist = SH::distance(pa, pb); 
      diffab = pa - pb;
      dist = sqrt(diffab | diffab); 
    } SH_END;
    objective = foo;
    objective.name("dist_" + a.name() + "_" + b.name());

    p = SH_BEGIN_PROGRAM() {
      SH::InputAttrib1f SH_DECL(t);
      PointNf apoint, bpoint;
      OutType result;
      apoint = A > 0 ? a(minparms.template swiz<A>(aswiz)) : a; 
      bpoint = B > 0 ? b(minparms.template swiz<B>(bswiz)) : b; 
      /* Using the color ramp version of the plotting function */
      result = SH::fillcast<4>(0.0f);
      result.template swiz<N>(nswiz) = lerp(t, apoint, bpoint);
      result(3) = 0.0f;
    } SH_END;

    minimizer = Minimizer<A + B>(objective, domain_eps, use_aa);
  }

  virtual const SH::Program& getProgram() { return p; }

  /* updates the nearest points by running the global optimization again */
  virtual void update(SH::Program& p) {
    minparms = minimizer.doit(); 
    std::cout << "minimize result = " << minparms << std::endl;
  };

};

template<int A, int B, int N> 
struct distance_graph_updater: public nearest_points_updater<A, B, N> {
  typename SH::Attrib<A + B, SH::SH_TEMP, float> maxparms;
  Minimizer<A + B> maximizer;

  distance_graph_updater(const Man& a, const Man& b, float domain_eps, bool use_aa) 
    : nearest_points_updater<A, B, N>(a, b, domain_eps, use_aa) { 
    typedef typename SH::Attrib<A + B, SH::SH_INPUT, float> InType; 
    typedef typename SH::Attrib<A + B, SH::SH_TEMP, float> TempType; 
    typedef typename SH::Attrib<4, SH::SH_OUTPUT, float> OutType; 
    typedef typename nearest_points_updater<A, B, N>::PointNf PointNf;

    assert(A + B <= 2);
    this->p = SH_BEGIN_PROGRAM() {
      InType in;
      OutType result;
      PointNf apoint, bpoint;
      SH::Attrib1f minVal, maxVal, valRatio, val;
      apoint = A > 0 ? a(this->minparms.template swiz<A>(this->aswiz)) : a; 
      bpoint = B > 0 ? b(this->minparms.template swiz<B>(this->bswiz)) : b; 
      val = this->objective(in);
      if(A + B == 1) {
        result(0) = in(0) - 0.5f;
        result(1) = val * 0.1f; 
        result(2) = 0.0f;
      } else if (A + B == 2) {
        result(0, 2) = in(0,1) - 0.5f;
        result(1) = val * 0.1f; 
      }
#if COLORDIST
      minVal = this->objective(this->minparms);
      maxVal = this->objective(this->maxparms);
      valRatio = (val - minVal) / (maxVal - minVal);
      result(3) = valRatio;
#else
      result(3) = 0.5;
#endif
    } SH_END;
    maximizer = Minimizer<A + B>(-this->objective, domain_eps * 10, use_aa);
  }

  /* updates the nearest points by running the global optimization again */
  virtual void update(SH::Program& p) {
#if COLORDIST 
    nearest_points_updater<A, B, N>::update(p);
    maxparms = maximizer.doit();
    std::cout << "maximize result = " << maxparms << std::endl;
#endif
  };
};


/* Returns a line showing where the closest point between two parametric surfaces is 
 *
 * For now, you need to specify the dimensions of a and b's inputs and 
 * both have output size N */ 
template<int A, int B, int N>
HostMan nearest_points(const Man& a, const Man& b, float domain_eps, bool use_aa=true) {
  return HostMan(new nearest_points_updater<A, B, N>(a, b, domain_eps, use_aa)); 
}

/* Alternative to nearest_points that plots the distance graph 
 * (Currently hacked to fit 3D vis methods by using a 4th output coordinate to indicate where the minimum is)*/
template<int A, int B, int N>
HostMan distance_graph(const Man& a, const Man& b, float domain_eps, bool use_aa=true) {
  return HostMan(new distance_graph_updater<A, B, N>(a, b, domain_eps, use_aa)); 
}

#endif
