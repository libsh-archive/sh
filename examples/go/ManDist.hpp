#ifndef MANDIST_HPP
#define MANDIST_HPP

#include <cassert>
#include <cstdarg>

#include <iostream>
#include <fstream>
#include <sh/sh.hpp>
#include "man.hpp"
#include "BranchBound.hpp"


/* Global minimization operator (all input parameters assumed to range between 0 and 1) 
 *
 * (The branch-and-bound algorithm is executed partly on the host, hence why this doesn't 
 * return a program, as it cannot be executed entirely as a stream program at present) 
 *
 * Returns only one minimum  
 * */

template<int N>
void minimize(SH::Generic<N, float>& result, const Man& objective, int splitLevel=1, float eps=1e-5, bool use_aa=true) {
  SH::ConstAttrib1i_f bounds(SH::Interval<float>(0.0f, 1.0f));
  typedef SH::Attrib<N, SH::SH_TEMP, float> InType;
  typedef SH::Attrib1f OutType;
  assert(objective.size_matches(N, 1));


  SH::Program ifunc = use_aa ? affine_inclusion(objective) : inclusion(objective);
  ifunc.node()->dump("ifunc");
  if(use_aa) {
    SH::Program foo(ifunc.node()->clone());
    placeAaSyms(foo.node(), true);
  }


  typedef BranchBound<InType, OutType> BBParams;
  typedef BranchBoundIterator<BBParams> BBIterator;
  typedef typename BBParams::IntervalDomain IDomain;
  typedef typename BBParams::IntervalRange IRange; 
  typedef typename BBParams::DomainArray DomainArray;
  typedef std::vector<typename BBParams::ResultType> ResultVec;

  BBParams bb;
  BBIterator bbIt; 
  ResultVec results;
  IDomain initialBounds; 
  for(int i = 0; i < initialBounds.size(); ++i) initialBounds(i) = bounds;
  bb = BBParams(ifunc, splitLevel, eps); 
  bbIt = BBIterator(bb, initialBounds);
  std::ofstream bbout("bbout");
  try {
    while(!bbIt.done()) {
      std::cout << "Iteration " << (results.size() + 1) << std::endl;
      bbIt.iterate();
      results.push_back(bbIt.current());
#if 1
      // get ranges and display them
      typename BBParams::ResultType r = results.back();
      int count = r.domain.count();
      std::cout << "After iteration " << results.size(); 
      std::cout << "  count=" << count << std::endl;
      std::cout << "  first five results:" << std::endl;
      bbout << "Iteration " << results.size() << " count=" << count << std::endl;
      for(int i = 0; i < count; ++i) {
        if(i < 5) {
          std::cout << "  [" << r.domain.lo(i) << "," << r.domain.hi(i) << "] ->  [" 
                    << r.range.lo(i) << "," << r.range.hi(i) << "]" << std::endl; 
        }
        bbout << "  [" << r.domain.lo(i) << "," << r.domain.hi(i) << "] ->  [" 
                  << r.range.lo(i) << "," << r.range.hi(i) << "]" << std::endl; 
      }
#endif
    }
  } catch(const SH::Exception &e) {
    std::cout << "Error: " << e.message() << std::endl;
  } catch(...) {
    std::cout << "Caught unknown error: " << std::endl;
  }
  bbout.close();

  DomainArray temp(results.back().domain.lo(0) + results.back().domain.hi(0));
  temp *= 0.5;
  for(int i = 0; i < result.size(); ++i) {
    result(i) = temp[i];
  }
}

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
  const Man& a; 
  const Man& b; 

  SH::Program p;
  Man objective;
  typedef typename SH::Attrib<N, SH::SH_TEMP, float, SH::SH_POINT> PointNf;
  PointNf apoint, bpoint;
  int aswiz[A];
  int bswiz[B];


  nearest_points_updater(const Man& a, const Man& b): a(a), b(b) {
    typedef typename SH::Attrib<A + B, SH::SH_INPUT, float> InType; 
    typedef typename SH::Attrib<A, SH::SH_INPUT, float> InTypeA; 
    typedef typename SH::Attrib<A, SH::SH_INPUT, float> InTypeB; 
    typedef typename SH::Attrib<N, SH::SH_OUTPUT, float> OutType; 

    a.node()->dump("npu_a");
    b.node()->dump("npu_b");

    assert(A+B > 0); 

    for(int i = 0; i < A; ++i) aswiz[i] = i;
    for(int i = 0; i < B; ++i) bswiz[i] = A + i;

    SH::Program foo = SH_BEGIN_PROGRAM() {
      InType SH_DECL(in);
      SH::OutputAttrib1f SH_DECL(dist);

      typename OutType::TempType pa, pb, diffab;
      pa = A > 0 ? a(in.template swiz<A>(aswiz)) : a; 
      pb = B > 0 ? b(in.template swiz<B>(bswiz)) : b; 
      //dist = SH::distance(pa, pb); 
      diffab = pa - pb;
      dist = diffab | diffab; 
    } SH_END;
    objective = foo;

    p = lerp(m_u(0), m_(apoint), m_(bpoint)); 
  }

  virtual const SH::Program& getProgram() { return p; }

  /* updates the nearest points by running the global optimization again */
  virtual void update(SH::Program& p) {
    typename SH::Attrib<A + B, SH::SH_TEMP, float> minparms;
    minimize(minparms, objective);
    apoint = A > 0 ? a(minparms.template swiz<A>(aswiz)) : a; 
    bpoint = B > 0 ? b(minparms.template swiz<B>(bswiz)) : b; 
    std::cout << "minimize result = " << minparms << std::endl;
    std::cout << "  apoint=" << apoint << std::endl;
    std::cout << "  bpoint=" << bpoint << std::endl;
  };
};


/* Returns a line showing where the closest point between two parametric surfaces is 
 *
 * For now, you need to specify the dimensions of a and b's inputs and 
 * both have output size N */ 
template<int A, int B, int N>
HostMan nearest_points(const Man& a, const Man& b) {
  return HostMan(new nearest_points_updater<A, B, N>(a, b)); 
}

#endif
