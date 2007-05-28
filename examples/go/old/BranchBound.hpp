#ifndef BRANCHBOUND_HPP
#define BRANCHBOUND_HPP

#include <iostream>
#include <sh/sh.hpp>

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

  IAStream(int expandFactor=2);
  operator SH::Stream& (); 

  /* Resizes channels to the requested length */
  void resize(int length);

  /* Reserves capacity in channel's memory */ 
  void reserve(int length);

  /* Current capacity */
  int capacity() const;

  /* Appends an element, automatically scaling capacity by expandFactor
   * if necessary. */
  void append(const T& lo, const T& hi);

  void append(const IntervalT& bounds);

  T lo(int i) const;
  T hi(int i) const;

  IntervalT operator[](int i) const; 

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

    // never null, always same size as capacity() * Tsize 
    typedef typename T::VariantType DataType;
    typedef SH::Pointer<DataType> DataTypePtr; 
    DataTypePtr m_loData, m_hiData; 

    void rebuild(); /* m_lo or m_hi changed, rebuild stream */ 
    void resize(int length, SH::Channel<T>& ch, DataTypePtr& data);
    void reserve(int length, SH::Channel<T>& ch, DataTypePtr& data);
    void sync(SH::Channel<T>& ch);

    // Allocates a host memory of the given length, sets data to the 
    // host storage's data, and copies over original contents 
    // if the original was not zero-length
    void allocData(int length, SH::Channel<T>& ch, DataTypePtr& data);
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const IAStream<T>& ias);


/* interface for a branch and bound class  */
template<typename D, typename R>
class BranchBound {
  public:
    typedef D Domain;
    typedef R Range;
    typedef IAStream<D> DomainStream; 
    typedef IAStream<R> RangeStream; 
    typedef typename DomainStream::IntervalT IntervalDomain;
    typedef typename RangeStream::IntervalT IntervalRange;

    struct ResultType {
      DomainStream domain;
      RangeStream range;
      ResultType(DomainStream d, RangeStream r)
        : domain(d), range(r) {}

      ResultType() {}
    };

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
    // @param eps        Stops when candidate regions have width < eps in all
    //                   dimensions.
    //
    // @todo range allow different eps/split levels in per dimension 
    BranchBound(SH::Program f, size_t splitLevel=1, HostType eps = 1e-7);

    BranchBound();

    // Step 1  
    DomainStream split(DomainStream bounds);

    // Step 2 - evaluates function  
    ResultType evaluate(DomainStream bounds);

    // Step 3 - checks if we're done.  If so, gives the results.
    // Otherwise, may do some filtering of box regions that no longer need to 
    // be checked in the next iteration.
    //
    // @return true if we're done, false if not
    bool filter(ResultType input,
                ResultType& output);

    // Optional Step 4 - merge output ranges
    // No matter how small the range widths are, the final result may include some adjacent ranges 
    // due to gift wrapping.

  private:
    SH::Program m_func;
    size_t m_splitLevel;
    HostType m_eps; 

};

template<typename B>
class BranchBoundIterator {
  public:
    BranchBoundIterator();
    BranchBoundIterator(B& params, typename B::IntervalDomain& initialBounds);

    typename B::ResultType& current(); 

    // iterates and returns 
    void iterate();
   
    bool done();

  protected:
    B m_params;
    typename B::ResultType m_current;
    bool m_done;
};


#endif

#include "BranchBoundImpl.hpp"
