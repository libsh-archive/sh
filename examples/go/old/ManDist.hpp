#ifndef MANDIST_HPP
#define MANDIST_HPP

#include <cassert>
#include <cstdarg>

#include <iostream>
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

template<int I, int O>
SH::Generic<I, float>& minimize(const Man<I, 1>& objective, int splitLevel=2, float eps=1e-5, bool use_aa=true) {
  SH::ConstAttrib1i_f bounds(0, 1);
  typedef typename Man<I, O>::InType InType;
  typedef typename Man<I, O>::OutType OutType;

  SH::Program ifunc = affine_inclusion(objective);

  typedef BranchBound<InType, OutType> BBParams;
  typedef BranchBoundIterator<BBParams> BBIterator;
  typedef typename BBParams::IntervalDomain IDomain;
  typedef typename BBParams::IntervalRange IRange; 
  typedef std::vector<typename BBParams::ResultType> ResultVec;

  BBParams bb;
  BBIterator bbIt; 
  ResultVec results;
  IDomain initialBounds; 
  for(int i = 0; i < initialBounds.size(); ++i) initialBounds(i) = bounds;
  bb = BBParams(ifunc, splitLevel, eps); 
  bbIt = BBIterator(bb, initialBounds);
  try {
    while(!bbIt.done()) {
      bbIt.iterate();
      results.push_back(bbIt.current());
#if 1
      // get ranges and display them
      typename BBParams::ResultType r = results.back();
      int count = r.domain.count();
      std::cout << "After iteration " << results.size(); 
      std::cout << "  count=" << count << std::endl;
      for(int i = 0; i < count; ++i) {
        IDomain domain = r.domain[i]; 
        IRange range = r.range[i];
        std::cout << "  " << domain << " -> " << range << std::endl; 
      }
#endif
    }
  } catch(const SH::Exception &e) {
    std::cout << "Error: " << e.message() << std::endl;
  } catch(...) {
    std::cout << "Caught unknown error: " << std::endl;
  }
  return center(results.back()[0]);
}

struct HostManUpdater {
  virtual ~HostManUpdater() {}
  virtual const SH::Program& getProgram() = 0;
  virtual void update(SH::Program& p) = 0; 
};


/* A function where some of the initial computation may need to be done on the host and stored in uniforms
 * (i.e. it can depend on other Man objects, but the update() method needs to be called if they change */ 
template<int I, int O>
struct HostMan: public Man<I, O> {
  HostMan(HostManUpdater* updater): updater(updater) { *this = updater->getProgram(); } 
  ~HostMan() { delete updater; }
  void update() { updater->update(*this); }

  HostManUpdater* updater;
};

template<int I1, int I2, int O>
struct nearest_points_updater: public HostManUpdater {
  const Man<I1, O>& a;
  const Man<I2, O>& b;

  SH::Program p;
  Man<IntOp<I1, I2>::max, O> objective;
  typedef typename SH::Attrib<O, SH::SH_TEMP, float, SH::SH_POINT> PointO;
  PointO apoint, bpoint;
  int aswiz[I1];
  int bswiz[I2];


  nearest_points_updater(const Man<I1, O>& a, const Man<I2, O>& b): a(a), b(b) {
    typedef typename Man<I1 + I2, O>::InType InType;
    typedef typename Man<I1, O>::InType InTypeA;
    typedef typename Man<I2, O>::InType InTypeB;
    typedef typename Man<I1, O>::OutType OutType;

    for(int i = 0;i < I1; ++i) aswiz[i] = i;
    for(int i = 0; i < I2; ++i) bswiz[i] = I1 + i;

    SH::Program foo = SH_BEGIN_PROGRAM() {
      InType SH_DECL(in);
      SH::OutputAttrib1f SH_DECL(dist);

      typename OutType::TempType pa, pb;
      pa = a(in.template swiz<I1>(aswiz)); 
      pb = b(in.template swiz<I2>(bswiz)); 
      dist = distance(pa, pb); 
    } SH_END;
    objective = foo;

    p = m_lerp(mx_<1>(0), m_(apoint), m_(bpoint)); 
  }

  virtual const SH::Program& getProgram() { return p; }

  /* updates the nearest points by running the global optimization again */
  virtual void update(SH::Program& p) {
    SH::Generic<I1 + I2, float> minparms = minimize(objective);
    apoint = a(minparms.template swiz<I1>(aswiz)); 
    bpoint = b(minparms.template swiz<I1>(bswiz)); 

  };
};


/* Returns a line showing where the closest point between two parametric surfaces is */ 
template<int I1, int I2, int O>
HostMan<1, O> nearest_points(const Man<I1, O>& a, const Man<I2, O>& b) {
  return HostMan<IntOp<I1, I2>::max, O>(new nearest_points_updater<I1, I2, O>(a, b)); 
}

#endif
