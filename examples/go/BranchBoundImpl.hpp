#ifndef BRANCHBOUNDIMPL_HPP
#define BRANCHBOUNDIMPL_HPP

#include <iostream>
#include <iomanip>
#include <sh/MultiArray.hpp>
#include <algorithm>
#include <GL/gl.h>
#include "BranchBound.hpp"
#include "Timer.hpp"
#include "Case.hpp"

#define BB_DBG_ON true 

#define DEBUG_PRINT_BB(x) { if (BB_DBG_ON) { std::cout << __FILE__ << ":" << __LINE__ << ": " << x << std::endl;} }

template<typename T>
IAStream<T>::IAStream(int expandFactor) 
  : m_expandFactor(expandFactor),
    m_loTimestamp(-1),
    m_hiTimestamp(-1)
{
  allocData(1, m_lo, m_loVariant);
  allocData(1, m_hi, m_hiVariant);
}

template<typename T>
IAStream<T>::operator SH::Stream&() 
{
  rebuild();
  return m_stream;
}

template<typename T>
void IAStream<T>::resize(int length) 
{
  resize(length, m_lo, m_loVariant);
  resize(length, m_hi, m_hiVariant);
}

template<typename T>
void IAStream<T>::reserve(int length) 
{
  reserve(length, m_lo, m_loVariant);
  reserve(length, m_hi, m_hiVariant);
}

template<typename T>
void IAStream<T>::pop_front(int n)
{
  pop_front(n, m_lo, m_loVariant);
  pop_front(n, m_hi, m_hiVariant);
}

template<typename T>
int IAStream<T>::capacity() const
{
  return m_loVariant->size() / Tsize; 
}

template<typename T>
void IAStream<T>::updateStamps() {
  m_loTimestamp = m_lo.memory()->timestamp();
  m_hiTimestamp = m_hi.memory()->timestamp();
}

template<typename T>
void IAStream<T>::checkCurrentStamps() {
  //DEBUG_PRINT_BB("Checking " << this);
  //DEBUG_PRINT_BB("lo stamp=" << m_loTimestamp << " lomem stamp=" << m_lo.memory()->timestamp());
  //DEBUG_PRINT_BB("hi stamp=" << m_hiTimestamp << " himem stamp=" << m_hi.memory()->timestamp());
#if BB_DBG_ON 
  assert(m_loTimestamp == m_lo.memory()->timestamp());
  assert(m_hiTimestamp == m_hi.memory()->timestamp());
#endif
}

template<typename T>
void IAStream<T>::dirty() 
{
  dirty(m_lo);
  dirty(m_hi);
  updateStamps();
  //DEBUG_PRINT_BB("Dirtying " << this);
  //DEBUG_PRINT_BB("lo stamp=" << m_loTimestamp << " lomem stamp=" << m_lo.memory()->timestamp());
  //DEBUG_PRINT_BB("hi stamp=" << m_hiTimestamp << " himem stamp=" << m_hi.memory()->timestamp());
}


template<typename T>
void IAStream<T>::append(const ArrayT& lo, const ArrayT& hi)
{
  int index = m_lo.count(); 
  resize(index + 1);
  checkCurrentStamps();

  //SH_DEBUG_ASSERT((index + 1) * typesize <= m_loVariant.size());
  //SH_DEBUG_ASSERT((index + 1) * typesize <= m_hiVariant.size());

  int start = index * Tsize;
  for(int i = 0; i < Tsize; ++i) {
    (*m_loVariant)[start + i] = lo[i];
    (*m_hiVariant)[start + i] = hi[i];
  }
}

template<typename T>
void IAStream<T>::append(const MemT* lo, const MemT* hi)
{
  int index = m_lo.count(); 
  resize(index + 1);
  checkCurrentStamps();

  //SH_DEBUG_ASSERT((index + 1) * typesize <= m_loVariant.size());
  //SH_DEBUG_ASSERT((index + 1) * typesize <= m_hiVariant.size());

  int start = index * Tsize;
  for(int i = 0; i < Tsize; ++i) {
    (*m_loVariant)[start + i] = lo[i];
    (*m_hiVariant)[start + i] = hi[i];
  }
}

template<typename T>
void IAStream<T>::append(const MemT& lo, const MemT& hi)
{
  int index = m_lo.count(); 
  resize(index + 1);
  checkCurrentStamps();

  //SH_DEBUG_ASSERT((index + 1) * typesize <= m_loVariant.size());
  //SH_DEBUG_ASSERT((index + 1) * typesize <= m_hiVariant.size());
  assert(Tsize == 1);

  int start = index * Tsize;
  (*m_loVariant)[start] = lo; 
  (*m_hiVariant)[start] = hi;
}

template<typename T>
void IAStream<T>::clear(const MemT& val) 
{
  checkCurrentStamps();
  std::fill(m_loVariant->begin(), m_loVariant->end(), val);
  std::fill(m_hiVariant->begin(), m_hiVariant->end(), val);
}

template<typename T>
typename IAStream<T>::ArrayT IAStream<T>::lo(int i) const
{
  return ArrayT(m_loVariant->begin() + (i * Tsize), Tsize);
}

template<typename T>
typename IAStream<T>::ArrayT IAStream<T>::hi(int i) const
{
  return ArrayT(m_hiVariant->begin() + (i * Tsize), Tsize);
}

template<typename T>
const typename IAStream<T>::MemT* IAStream<T>::pointer_lo(int i) const
{
  return m_loVariant->begin() + (i * Tsize);
}

template<typename T>
const typename IAStream<T>::MemT* IAStream<T>::pointer_hi(int i) const
{
  return m_hiVariant->begin() + (i * Tsize);
}

template<typename T>
typename IAStream<T>::MemT IAStream<T>::scalar_lo(int i) const
{
  return *(m_loVariant->begin() + (i * Tsize));
}

template<typename T>
typename IAStream<T>::MemT IAStream<T>::scalar_hi(int i) const
{
  return *(m_hiVariant->begin() + (i * Tsize));
}

template<typename T>
IAStream<T>& IAStream<T>::operator=(const SH::Program& program)
{
  static_cast<SH::Stream&>(*this) = (program);
  return *this;
}

template<typename T>
int IAStream<T>::count() const 
{
  return m_lo.count();
}

template<typename T>
void IAStream<T>::name(std::string name)
{
  m_lo.name(name + "_lo");
  m_hi.name(name + "_hi");
}

template<typename T>
std::string IAStream<T>::name()
{
  std::string foo = m_lo.name();
  if(foo.length() < 3) return foo;
  return foo.substr(0, foo.length() - 3);
}

template<typename T>
void IAStream<T>::dirty(SH::Channel<T>& ch) {
  SH::HostMemoryPtr mem = SH::shref_dynamic_cast<SH::HostMemory>(ch.memory()); 
  mem->hostStorage()->dirty();
}

template<typename T>
void IAStream<T>::rebuild() 
{
  m_stream = SH::Stream();
  m_stream.append(m_lo);
  m_stream.append(m_hi);
}

template<typename T>
void IAStream<T>::resize(int length, SH::Channel<T>& ch, VariantTypePtr& data)
{
  reserve(length, ch, data);
  ch.count(length);
}

template<typename T>
void IAStream<T>::reserve(int length, SH::Channel<T>& ch, VariantTypePtr& data)
{
  if(length * Tsize < data->size()) {
    return;
  }

  // double capacity till we have enough, then copy over data
  int newcap = data->size();
  assert(m_expandFactor > 1);
  for(;newcap < length; newcap *= m_expandFactor); 
  allocData(newcap, ch, data);
  dirty();
}

template<typename T>
void IAStream<T>::pop_front(int n, SH::Channel<T>& ch, VariantTypePtr& data)
{
  checkCurrentStamps();
  size_t count = ch.count();
  /* copy remaining data to front */
  for(size_t i = n * T::typesize; i < count * T::typesize; ++i) { 
    (*data)[i - n * T::typesize] = (*data)[i];
  }
  resize(std::max(0UL, count - n), ch, data); 
}

template<typename T>
void IAStream<T>::allocData(int length, SH::Channel<T>& ch, VariantTypePtr& data)
{
  SH::HostMemoryPtr newmem = new SH::HostMemory(length * Tsize * Dsize, T::value_type);

  int copyLength = 0;
  typename VariantType::const_iterator oldBegin = 0; 
  if(data) {
    oldBegin = data->begin(); 
    copyLength = std::min(length * Tsize, data->size());
  }

  VariantTypePtr newData = new VariantType(length * Tsize, newmem->hostStorage()->data(), false); 
  
  if(copyLength) {
    std::copy(oldBegin, oldBegin + copyLength, newData->begin());
  }
  data = newData;

  /* @todo range - sh is very very finicky about how this is done 
   *
   * Channels (BaseTextures) represent a subset (stride, offset, count)  
   * of some underlying chunk of memory in a texture node, where
   * the dimension of the subsetting is assumed to match that of the
   * texture node.
   *
   * The true size of the memory chunk supporting a channel can only be changed
   * by going in through a few levels to the texture node (which has it's
   * own messy resizing gunk), or by building a new channel 
   *
   * All other methods leave the texture node with the original size,
   * and may lead to weirdness? */ 
  int oldcount = ch.count();
  ch = SH::Channel<T>(newmem, length);
  ch.count(oldcount);
}

template<typename T>
void IAStream<T>::sync() {
  sync(m_lo);
  sync(m_hi);
}

template<typename T>
void IAStream<T>::sync(SH::Channel<T>& ch) {
  SH::HostMemoryPtr mem = SH::shref_dynamic_cast<SH::HostMemory>(ch.memory()); 
  mem->hostStorage()->sync();
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const IAStream<T>& ias) 
{
  sync();
  int count = ias.count();
  out << "IAStream{" << std::endl;
  
  for(int i = 0; i < count ; ++i) {
    out << "    (";
    for(int j = 0; j < IAStream<T>::Tsize; ++j) { 
        if(j > 0) out << ", ";
        out << "[" << ias.lo(i)[j] << ", " << ias.hi(i)[j] << "]";
     }
     out << ")" << std::endl;
  }
  out << "}" << std::endl;
  return out;
}

template<typename D, typename R>
BranchBound<D, R>::BranchBound(SH::Program objective, SH::Program iobjective, bool doSort, size_t splitLevel, HostType eps, HostType range_eps, size_t minSize, size_t maxSize)
  : evaluate_stats("evaluate"),
    filter_minHi_input(DomainSize),
    filter_minHi_stats("filter_minHi"),
    m_doSort(doSort),
    m_splitLevel(splitLevel),
    m_minSize(minSize),
    m_maxSize(maxSize),
    m_eps(eps),
    m_range_eps(range_eps)
{
  //SH_DEBUG_ASSERT(Range::typesize == 1);

  // @todo range - a bit of a hack to handle interval inputs/outputs
  std::string target = iobjective.target(); 
  if(target == "") target = "stream";
  m_iobjective = SH_BEGIN_PROGRAM(target) {
    typename Domain::InputType SH_DECL(in_lo);
    typename Domain::InputType SH_DECL(in_hi);

    typename Range::OutputType SH_DECL(out_lo);
    typename Range::OutputType SH_DECL(out_hi);

    typename IntervalRange::TempType SH_DECL(out);

    out = iobjective(make_interval(in_lo, in_hi));
    out_lo = range_lo(out);
    out_hi = range_hi(out);
  } SH_END;
  m_iobjective.node()->name(iobjective.node()->name());
  if(caseName != "") m_iobjective.node()->name("i" + caseName);

  m_objective = SH_BEGIN_PROGRAM(target) {
    typename Domain::InputType SH_DECL(in_lo);
    typename Domain::InputType SH_DECL(in_hi);
    typename Range::OutputType SH_DECL(result);
    result = objective(SH::lerp(0.5f, in_lo, in_hi));
  } SH_END;
  m_objective.node()->name(objective.node()->name());
  if(caseName != "") m_objective.node()->name(caseName);

  m_minHiMem = new SH::HostMemory(m_maxSize * sizeof(RangeType), Range::value_type);
}

template<typename D, typename R>
BranchBound<D, R>::BranchBound()
  : filter_minHi_input(DomainSize),
    m_doSort(false),
    m_splitLevel(1),
    m_minSize(65536),
    m_maxSize(1048576),
    m_eps(1e-7)
{}

template<typename D, typename R>
typename BranchBound<D, R>::State
BranchBound<D, R>::init(IntervalDomain bounds)
{
  State result;
  DomainArray lo(bounds.size());
  DomainArray hi(bounds.size());
  for(int i = 0; i < bounds.size(); ++i) {
    lo[i] = bounds.getValue(i).lo();
    hi[i] = bounds.getValue(i).hi();
  }
  result.dirty();
  result.domain.append(lo, hi);
  result.range.append(0.0f,0.0f); /* doesn't really matter */
  result.domain.name("init_domain");
  result.range.name("init_range");

  filter_minHi = 1e100;

  return result;
}

template<typename D, typename R>
void BranchBound<D, R>::split(State& state, State& resultState)
{
  Timer start_time = Timer::now();
  /* amplification factor in one dimension */
  size_t splitLevel = m_splitLevel;
  size_t blowupFactor, totalBlowup;
  for(;;) {
    blowupFactor = 1 << splitLevel; 
    totalBlowup = 1 << (splitLevel * Domain::typesize);
    size_t nextTotalBlowup = 1 << ((splitLevel + 1) * Domain::typesize);
    if(totalBlowup * state.count() >= m_minSize || nextTotalBlowup > m_maxSize) break;
    splitLevel++;
  }

  //DEBUG_PRINT_BB( "Splitting bounds = " << bounds );

  SH::size_t_array start(static_cast<size_t>(0), Domain::typesize);
  SH::size_t_array lengths(blowupFactor, Domain::typesize);
  SH::size_t_array strides(static_cast<size_t>(1), Domain::typesize);
  SH::MultiArrayIndex S(start, lengths, strides); 

  /* Determine how many items we're going to split */
  int count = std::min(state.count(), static_cast<int>(m_maxSize / totalBlowup)); 
  resultState.domain.resize(0);
  resultState.domain.dirty();
  state.dirty();

  DomainArray splitlo(DomainSize);
  DomainArray splithi(DomainSize);
  DomainArray width(DomainSize);

  for(int i = 0; i < count; ++i) {
    DomainArray lo(state.domain.lo(i)); 
    DomainArray hi(state.domain.hi(i)); 
    DomainArray width((hi - lo) / blowupFactor); // @todo range - should use lengths if not all blowupFactor 

   //DEBUG_PRINT_BB( "  i=" << i << " lo=" << lo << " hi=" << hi << " width=" << width );

    for(S.reset();; ++S) {
      splitlo = lo; 
      SH::size_t_array index = S.index();
      for(size_t j = 0; j < index.size(); ++j) {
        splitlo[j] += width[j] * index[j];
      }
      splithi = splitlo + width; 
      //DEBUG_PRINT_BB( "  S=" << S << "splitlo=" << splitlo << " splithi=" << splithi );
      resultState.domain.append(splitlo, splithi);
      if(S.end()) break;
    }
  }

  state.domain.pop_front(count);
  state.range.pop_front(count);
  //DEBUG_PRINT_BB( "Result = " << splitBounds );
  split_time = (Timer::now() - start_time).value();
 // DEBUG_PRINT_BB("Split into " << splitBounds.count() << " subranges");
}

  // Step 2 - returns an SH::Program that has interval input of type Domain and
  //          interval output of type Range
template<typename D, typename R>
void BranchBound<D, R>::evaluate(State& state)
{
  state.range.resize(state.domain.count());
  //DEBUG_PRINT_BB( "Result size = " << outputs.count() << " bound size = " << bounds.count() );
  //DEBUG_PRINT_BB( "evaluate(" << bounds << ")" );
  

  state.range = m_iobjective << state.domain;
  glFinish();

  if(evaluate_stats.instr_count == -1) {
      evaluate_stats.update(defaultBackend);
  }
  evaluate_stats.update_times();

  Timer start = Timer::now();
  state.range.sync();
  glFinish();
  evaluate_sync = (Timer::now() - start).value();

#if 0
  std::cerr <<  "evaluate done"; 
  std::cerr << "evaluate outputs";
  for(int i = 0; i < outputs.count(); ++i) {
    std::cerr << "    (";
    for(int j = 0; j < DomainStream::Tsize; ++j) { 
        if(j > 0) std::cerr << ", ";
        std::cerr << "[" << bounds.lo(i)[j] << ", " << bounds.hi(i)[j] << "]";
        std::cerr << " w" << (bounds.hi(i)[j] - bounds.lo(i)[j]);
    }
    std::cerr << ") -> "; 
    std::cerr << "    (";
    for(int j = 0; j < RangeStream::Tsize; ++j) { 
        if(j > 0) std::cerr << ", ";
        std::cerr << "[" << outputs.lo(i)[j] << ", " << outputs.hi(i)[j] << "]";
        std::cerr << " w" << (bounds.hi(i)[j] - bounds.lo(i)[j]);
     }
     std::cerr << ")" << std::endl;
  }
#endif
  evaluate_sort_time = 0; 
  if(m_doSort) {
    start = Timer::now();
    state = statesort(state);
    evaluate_sort_time = (Timer::now() - start).value();
  } 
}

  // Step 3 - checks if we're done.  If so, gives the results.
  // Otherwise, may do some filtering of box regions that no longer need to 
  // be checked in the next iteration.
  //
  // @return true if we're done, false if not
template<typename D, typename R>
bool BranchBound<D, R>::mergefilter(State input, State old, State& output)
{
  //DEBUG_PRINT_BB( "Filtering" );
  //DEBUG_PRINT_BB( "In Domains: " << input.domain );
  //DEBUG_PRINT_BB( "In Ranges:  " << input.range );
  //
  Timer start = Timer::now();

  // filter out non-candidates
  // find smallest upper bound u and discard ranges with lower bounds > u
  int in_count = input.domain.count();
  int old_count = old.domain.count();
  int i, j;
  //SH_DEBUG_ASSERT(count > 0);
  //filter_minHi = input.range.scalar_hi(0);
  
  /* update minHi by sampling the new ranges in input */ 
  m_minHiMem = new SH::HostMemory(m_maxSize * sizeof(RangeType), Range::value_type);
  SH::Channel<Range> samples(m_minHiMem, in_count);
  //m_minHiSamples.memory(m_minHiMem, in_count);
  samples = m_objective << input.domain;  

  if(filter_minHi_stats.instr_count == -1) {
    filter_minHi_stats.update(defaultBackend);
  }
  filter_minHi_stats.update_times();

  Timer syncstart = Timer::now();
  m_minHiMem->hostStorage()->sync();
  filter_minHi_sync = (Timer::now() - syncstart).value();

  RangeType* sample_ptr = reinterpret_cast<RangeType*>(m_minHiMem->hostStorage()->data()); 
  for(i = 0; i < in_count; ++i) {
    if(sample_ptr[i] < filter_minHi) {
      filter_minHi = sample_ptr[i]; 
      filter_minHi_input = (input.domain.lo(i) + input.domain.hi(i)) * 0.5;
      //DEBUG_PRINT_BB("New minHi=" << filter_minHi << " between "
      //              << input.domain.lo(i) << " and " << input.domain.hi(i));
    }
  }

  start = Timer::now();

  output = State();
  output.dirty();
  if(m_doSort) {
      for(i = j = 0; i < in_count && j < old_count;) { 
        RangeType loi = input.range.scalar_lo(i);
        RangeType loj = old.range.scalar_lo(j);
        if(loi < loj) {
          if(filter_minHi >= loi) output.append(input, i);
          ++i;
        } else {
          if(filter_minHi >= loj) output.append(old, j);
          ++j;
        }
      }

      for(; i < in_count; ++i) {
        if(filter_minHi >= input.range.scalar_lo(i)) output.append(input, i);
      }

      for(; j < old_count; ++j) {
        if(filter_minHi >= old.range.scalar_lo(j)) output.append(old, j);
      }
  } else {
      for(i = 0; i < old_count; ++i) {
        if(filter_minHi >= old.range.scalar_lo(i)) output.append(old, i);
      }
      for(i = 0; i < in_count; ++i) {
  //      DEBUG_PRINT_BB(input.domain.lo(i) << " " << input.domain.hi(i));
  //      DEBUG_PRINT_BB(i << "  " << filter_minHi << " " << input.range.scalar_lo(i));
        if(filter_minHi >= input.range.scalar_lo(i)) output.append(input, i);
      }
  }
  filter_discard_time = (Timer::now() - start).value();

  //DEBUG_PRINT_BB( "Out Domains: " << output.domain );
  //DEBUG_PRINT_BB( "Out Ranges:  " << output.range );
  
  //output = statesort(output);

  output.domain.name(input.domain.name() + "_filter");
  output.range.name(input.range.name() + "_filter");

  // return true if all possible bounds are small enough 
  int out_count = output.count();
  max_domain_width = max_range_width = 0;
  for(i = 0; i < out_count; ++i) {
    max_range_width = std::max(max_range_width, output.range.scalar_hi(i) - output.range.scalar_lo(i));
    DomainArray test(output.domain.hi(i) - output.domain.lo(i));
    for(size_t j = 0; j < test.size(); ++j) {
      max_domain_width = std::max(max_domain_width, test[j]);
    }
  }
  return max_domain_width < m_eps && max_range_width < m_range_eps; 
  //return max_domain_width < m_eps; 
}

namespace {
typedef std::vector<int> vecint;

template<typename D, typename R>
struct MappingLessThan {
  typedef typename BranchBound<D, R>::RangeStream RangeStream;   
  MappingLessThan(RangeStream& r): range(r) {}

  bool operator()(int i, int j) {
    return range.scalar_lo(i) < range.scalar_lo(j) || (range.scalar_lo(i) == range.scalar_lo(j) && range.scalar_hi(i) < range.scalar_hi(j));
  }

  RangeStream range;
};


}

template<typename D, typename R>
typename BranchBound<D, R>::State BranchBound<D, R>::statesort(State state) {
  MappingLessThan<D, R> mlt(state.range);
  int count = state.count();
  vecint idx(count);
  for(int i = 0; i < count; ++i) idx[i] = i;

  /* Get a sorted ordering on the indices */
  sort(idx.begin(), idx.end(), mlt);

  /* Copy to result based on sorted ordering */ 
  State result;
  result.dirty();
  result.domain.reserve(count);
  result.range.reserve(count);
  for(int i = 0; i < count; ++i) {
    result.domain.append(state.domain.pointer_lo(idx[i]), state.domain.pointer_hi(idx[i]));
    result.range.append(state.range.pointer_lo(idx[i]), state.range.pointer_hi(idx[i]));
  }
  return result;
}

template<typename D, typename R>
bool BranchBound<D, R>::RangeLessThan::operator()(const RangeArray& a, const RangeArray& b) 
{
  return a[0] < b[0];
}

template<typename B>
BranchBoundIterator<B>::BranchBoundIterator()
  : count(0)
{}

template<typename B>
BranchBoundIterator<B>::BranchBoundIterator(B& params, typename B::IntervalDomain& initialBounds) 
  : count(0),
    m_params(params),
    m_done(false) 
{
  m_host = m_params.init(initialBounds);
  m_gpu = m_params.buildTemp();
  DEBUG_PRINT_BB( "Initial Domain = " << m_host.domain );
}

template<typename B>
typename B::State& BranchBoundIterator<B>::current()
{
  return m_host;
}

template<typename B>
typename B::Domain BranchBoundIterator<B>::singleResult()
{
  typename B::Domain result;
  for(int i = 0; i < result.size(); ++i) {
      result(i) = m_params.filter_minHi_input[i]; 
  }
  return result;
}

template<typename B>
void BranchBoundIterator<B>::iterate()
{
  if(m_done) return;
  typename B::DomainStream splits; 
  typename B::State next;

 // DEBUG_PRINT_BB( "Starting split old count=" << m_current.count());
  m_params.split(m_host, m_gpu);
  //DEBUG_PRINT_BB( "Split into " << splits.count() << " subranges, left of old=" << m_current.count());


  //DEBUG_PRINT_BB( "Starting evaluate:" );
  m_params.evaluate(m_gpu);
  //DEBUG_PRINT_BB("Evaluated new " << next.count() << " old " << m_current.count());

  //DEBUG_PRINT_BB( "Starting filter:" );
  m_done = m_params.mergefilter(m_gpu, m_host, m_host);


  if(count == 0) {
    std::cout << "bbtime, case, iteration, split_time" << m_params.evaluate_stats.header()  
              << ",eval_sync_time, eval_sort_time, filter_minHi"
              << m_params.filter_minHi_stats.header()
              << ",filter_minHi_sync, filter_discard_time, max_domain_width, max_range_width, eval_size, state_size" << std::endl;
  }
  std::cout << "bbtime"
       << "," << caseName 
       << "," << std::setw(3) << count
       << "," << std::setw(10) << m_params.split_time 

       << m_params.evaluate_stats.csv()
       << "," << std::setw(10) << m_params.evaluate_sync
       << "," << std::setw(10) << m_params.evaluate_sort_time 

       << "," << std::setw(10) << m_params.filter_minHi
       << m_params.filter_minHi_stats.csv()
       << "," << std::setw(10) << m_params.filter_minHi_sync
       << "," << std::setw(10) << m_params.filter_discard_time 

       << "," << std::setw(10) << m_params.max_domain_width
       << "," << std::setw(10) << m_params.max_range_width
       << "," << std::setw(10) << m_gpu.count() 
       << "," << std::setw(10) << m_host.count() << std::endl;
  count++;
}


template<typename B>
bool BranchBoundIterator<B>::done()
{
  return m_done;
}


#endif
