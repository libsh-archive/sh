#ifndef BRANCHBOUND_HPP
#define BRANCHBOUND_HPP

#include <iostream>
#include <valarray>
#include <sh/sh.hpp>
#include "ProgramStats.hpp"

/* Basic iterative branch and bound 
 * Right now only handles SH::Programs with single value output
 * and one n-tuple input */

/* A branch and bound process
 *
 * Repeat until done
 *  1) CPU: Split domain (function takes input stream of subdomains, splits into smaller subdomains)
 *  2) GPU: Calculate bound function on subdomains
 *  3) CPU/GPU: Filter function, and doneness detection
 *     (May preprocess using reduction to get some global information)
 *     (Then CPU does filtering)
 */

// @todo range - get rid of this when interval/aa textures/streams work 
// (or there's a general means to handle "structs") 
template<typename T> 
struct IAStream {
  typedef typename SH::StorageTypeInfo<typename T::storage_type>::IntervalType IntervalStorageType; 
  static const int Tsize = T::typesize;
  static const int Dsize = sizeof(typename T::mem_type);  
  typedef SH::Attrib<Tsize, T::binding_type, IntervalStorageType> IntervalT;
  typedef std::valarray<typename T::mem_type> ArrayT; 
  typedef typename T::mem_type MemT;
  typedef typename T::VariantType VariantType;
  typedef SH::Pointer<VariantType> VariantTypePtr; 

  IAStream(int expandFactor=2);
  operator SH::Stream& (); 

  /* Resizes channels to the requested length */
  void resize(int length);

  /* Reserves capacity in channel's memory */ 
  void reserve(int length);


  /* Current capacity */
  int capacity() const;

  /* Mark as dirty() 
   * This is necessary before doing any appends, pops, clears */
  void dirty();

  /* Appends an element, automatically scaling capacity by expandFactor
   * if necessary. */
  void append(const ArrayT& lo, const ArrayT& hi);
  void append(const MemT* lo, const MemT* hi);
  void append(const MemT& lo, const MemT& hi);

  /* Removes n elements from the beginning */ 
  void pop_front(int n);


  /* sets all elements to the given value */ 
  void clear(const MemT& val=-1);

  ArrayT lo(int i) const;
  ArrayT hi(int i) const;

  const MemT* pointer_lo(int i) const;
  const MemT* pointer_hi(int i) const;

  /* should use partial specialization, but bleah */
  MemT scalar_lo(int i) const;
  MemT scalar_hi(int i) const;

  IAStream& operator=(const SH::Program& program);

  int count() const;

  // set lo channel to name_lo, ditto for hi
  void name(std::string name);

  std::string name();

  // @todo range should have const iterators to access elements 

  // syncs host memory  
  void sync();

  private:
    int m_expandFactor;
    SH::Stream m_stream;
    SH::Channel<T> m_lo, m_hi;
    int m_loTimestamp, m_hiTimestamp;
    void updateStamps();
    void checkCurrentStamps();

    // never null, always same size as capacity() * Tsize 
    VariantTypePtr m_loVariant, m_hiVariant; 

    void dirty(SH::Channel<T>& ch);
    void rebuild(); /* m_lo or m_hi changed, rebuild stream */ 
    void resize(int length, SH::Channel<T>& ch, VariantTypePtr& data);
    void reserve(int length, SH::Channel<T>& ch, VariantTypePtr& data);
    void pop_front(int n, SH::Channel<T>& ch, VariantTypePtr& data);
    void sync(SH::Channel<T>& ch);

    // Allocates a host memory of the given length, sets data to the 
    // host storage's data, and copies over original contents 
    // if the original was not zero-length
    void allocData(int length, SH::Channel<T>& ch, VariantTypePtr& data);
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const IAStream<T>& ias);


/* Interface for a branch and bound class 
 * Sh types are disgustingly slow in intermediate mode, so we switched here
 * to using valarays */
template<typename D, typename R>
class BranchBound {
  public:
    typedef D Domain;
    typedef R Range;
    typedef IAStream<D> DomainStream; 
    typedef IAStream<R> RangeStream; 
    typedef typename DomainStream::IntervalT IntervalDomain;
    typedef typename RangeStream::IntervalT IntervalRange;
    typedef typename DomainStream::ArrayT  DomainArray;
    typedef typename DomainStream::MemT DomainType;
    static const int DomainSize = DomainStream::Tsize;
    typedef typename RangeStream::ArrayT  RangeArray;
    typedef typename RangeStream::MemT RangeType;
    static const int RangeSize = RangeStream::Tsize;

    /* Keeps current candidate intervals, sorted by minimum range values */ 
    struct State {
      DomainStream domain;
      RangeStream range;
      State(DomainStream d, RangeStream r)
        : domain(d), range(r) {}

      State() {}
      int count() { return domain.count(); }
      void append(const State& state, int i) {
        domain.append(state.domain.pointer_lo(i), state.domain.pointer_hi(i));
        range.append(state.range.pointer_lo(i), state.range.pointer_hi(i));
      }

      void dirty() {
        domain.dirty();
        range.dirty();
      }
    };

    /* Returns a state with maxsize elements reserved*/
    State buildTemp() {
      State result;
      result.domain.reserve(m_maxSize);
      result.range.reserve(m_maxSize);
      return result;
    }

    typedef typename R::host_type HostType; 

    // Set up a basic branch and bound to minimize a given a function that 
    // has one interval typed input tuple and one interval scalar output 
    //
    // (Programs that internally use affine arithmetic are okay, but
    // @todo range have not implemented a nice way of handling noise syms in 
    //             AA inputs/outputs yet)
    //
    // @param splitLevel Splits box regions on each dimension by 2^splitLevel 
    //                   for each iteration. 
    // @param minSize    Minimum number of intervals to test on each iteration (splitLevel increased until we have enough)
    // @param maxSize    Maximum number of intervals to test on each iteration
    //                   (i.e. only the top maxSize / 2^splitLevel input intervals are considered) 
    // @param eps        Stops when candidate regions have width < eps in all
    //                   dimensions.
    //
    // @todo range allow different eps/split levels in per dimension 
    //BranchBound(SH::Program objective, SH::Program iobjective, bool doSort=false, size_t splitLevel=1, HostType eps = 1e-5, size_t maxSize=4194302);
    BranchBound(SH::Program objective, SH::Program iobjective, bool doSort=false, size_t splitLevel=1, HostType eps = 1e-5, HostType range_eps = 1e-2, size_t minSize=65536, size_t maxSize=4194302);
    //BranchBound(SH::Program objective, SH::Program iobjective, bool doSort=false, size_t splitLevel=1, HostType eps = 1e-5, size_t maxSize=2097150);

    BranchBound();

    // Step 0 - Initializes from a given domain stream or single domain entry
    //          (Sets up dummy ranges - assumes starting with way less than maxSize entries) 
    //          (initializes minHi to a large number) 
    State init(IntervalDomain bounds);

    // Step 1 - Splits up to maxSize / 2^splitLevel elements from bounds, removing them 
    //          (The first split must use up all elements)
    //          Places results in resultState.domain
    void split(State& state, State& resultState);

    // Step 2 - evaluates function  
    void evaluate(State& state);

    // Step 3 - merges new results with old, checks if we're done.  If so, gives the results.
    // Otherwise, may do some filtering of box regions that no longer need to 
    // be checked in the next iteration.
    //
    // @return true if we're done, false if not
    bool mergefilter(State input, State old,
                State& output);

    // Optional Step 4 - merge output ranges
    // No matter how small the range widths are, the final result may include some adjacent ranges 
    // due to gift wrapping.
    

    /* public timing statistics on last run*/ 
    float split_time;
    ProgramStats evaluate_stats;
    float evaluate_sync;
    float evaluate_sort_time;
    RangeType filter_minHi;
    DomainArray filter_minHi_input;

    ProgramStats filter_minHi_stats;
    float filter_minHi_sync;

    float filter_discard_time;
    float max_domain_width;
    float max_range_width;

  private:
    SH::Program m_objective;
    SH::Program m_iobjective;
    bool m_doSort;
    size_t m_splitLevel;
    size_t m_minSize;
    size_t m_maxSize;
    HostType m_eps; 
    HostType m_range_eps;
    SH::HostMemoryPtr m_minHiMem;
    SH::Channel<Range> m_minHiSamples;

    struct RangeLessThan {
      bool operator()(const RangeArray& a, const RangeArray& b);
    };

    /* Sorts in place in increasing range lower bounds */ 
    State statesort(State state);
};

template<typename B>
class BranchBoundIterator {
  public:
    BranchBoundIterator();
    BranchBoundIterator(B& params, typename B::IntervalDomain& initialBounds);

    typename B::State& current(); 
    typename B::Domain singleResult();

    // iterates and returns 
    void iterate();
   
    bool done();

  protected:
    int count;
    B m_params;

    /* host holds all the domains/ranges under consideration
     * gpu holds the current batch being evaluated on gpu
     * gpu never changes capacity */
    typename B::State m_host, m_gpu;
    bool m_done;
};


#endif

#include "BranchBoundImpl.hpp"
