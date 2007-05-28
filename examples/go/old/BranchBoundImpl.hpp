#ifndef BRANCHBOUNDIMPL_HPP
#define BRANCHBOUNDIMPL_HPP

#include <iostream>
#include <sh/MultiArray.hpp>
#include "BranchBound.hpp"
#include "Timer.hpp"

#define BB_DBG_ON false 

#define SH_DEBUG_PRINT_BB(x) { if (BB_DBG_ON) {SH_DEBUG_PRINT(x);} }

template<typename T>
IAStream<T>::IAStream(int expandFactor) 
  : m_expandFactor(expandFactor)
{
  allocData(1, m_lo, m_loData);
  allocData(1, m_hi, m_hiData);
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
  resize(length, m_lo, m_loData);
  resize(length, m_hi, m_hiData);
}

template<typename T>
void IAStream<T>::reserve(int length) 
{
  reserve(length, m_lo, m_loData);
  reserve(length, m_hi, m_hiData);
}

template<typename T>
int IAStream<T>::capacity() const
{
  return m_loData.size() / Tsize; 
}

template<typename T>
T IAStream<T>::lo(int i) const
{
  SH_DEBUG_ASSERT((i + 1) * typesize <= m_loData.size());
  T result;
  result.setValues(&(*m_loData)[i * Tsize]);
  return result;
}

template<typename T>
T IAStream<T>::hi(int i) const
{
  SH_DEBUG_ASSERT((i + 1) * typesize <= m_loData.size());
  T result;
  result.setValues(&(*m_hiData)[i * Tsize]);
  return result;
}

template<typename T>
typename IAStream<T>::IntervalT IAStream<T>::operator[](int i) const
{
  return make_interval(lo(i), hi(i));
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
void IAStream<T>::append(const T& lo, const T& hi)
{
  int index = m_lo.count(); 
  resize(index + 1);

  SH_DEBUG_ASSERT((index + 1) * typesize <= m_loData.size());
  SH_DEBUG_ASSERT((index + 1) * typesize <= m_hiData.size());
  lo.getValues(&(*m_loData)[index * Tsize]);
  hi.getValues(&(*m_hiData)[index * Tsize]);
}

template<typename T>
void IAStream<T>::append(const IntervalT& bounds)
{
  append(range_lo(bounds), range_hi(bounds));
}

template<typename T>
void IAStream<T>::rebuild() 
{
  m_stream = SH::Stream();
  m_stream.append(m_lo);
  m_stream.append(m_hi);
}

template<typename T>
void IAStream<T>::resize(int length, SH::Channel<T>& ch, DataTypePtr& data)
{
  reserve(length, ch, data);
  ch.count(length);
}

template<typename T>
void IAStream<T>::reserve(int length, SH::Channel<T>& ch, DataTypePtr& data)
{
  if(length * Tsize < data->size()) {
    return;
  }

  // double capacity till we have enough, then copy over data
  int newcap = data->size();
  assert(m_expandFactor > 1);
  for(;newcap < length; newcap *= m_expandFactor); 
  allocData(newcap, ch, data);
}

template<typename T>
void IAStream<T>::allocData(int length, SH::Channel<T>& ch, DataTypePtr& data)
{
  SH::HostMemoryPtr newmem = new SH::HostMemory(length * Tsize * Dsize, T::value_type);

  int copyLength = 0;
  typename DataType::const_iterator oldBegin = 0; 
  if(data) {
    oldBegin = data->begin(); 
    copyLength = std::min(length * Tsize, data->size());
  }

  DataTypePtr newData = new DataType(length * Tsize, newmem->hostStorage()->data(), false); 
  
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
  out << "IAStream{";
  
  for(int i = 0; i < count ; ++i) {
    if(i > 0) out << ", ";
    out << ias[i]; 
  }
  out << "}";
  return out;
}

template<typename D, typename R>
BranchBound<D, R>::BranchBound(SH::Program f, size_t splitLevel, HostType eps)
  : m_splitLevel(splitLevel),
    m_eps(eps)
{
  SH_DEBUG_ASSERT(Range::typesize == 1);

  // @todo range - a bit of a hack to handle interval inputs/outputs
  m_func = SH_BEGIN_PROGRAM(f.target()) {
    typename Domain::InputType SH_DECL(in_lo);
    typename Domain::InputType SH_DECL(in_hi);

    typename Range::OutputType SH_DECL(out_lo);
    typename Range::OutputType SH_DECL(out_hi);

    typename IntervalRange::TempType SH_DECL(out);

    out = f(make_interval(in_lo, in_hi));
    out_lo = range_lo(out);
    out_hi = range_hi(out);
  } SH_END;
}

template<typename D, typename R>
BranchBound<D, R>::BranchBound()
  : m_splitLevel(1),
    m_eps(1e-7)
{}

template<typename D, typename R>
typename BranchBound<D, R>::DomainStream 
BranchBound<D, R>::split(DomainStream bounds)
{
  size_t amplificationFactor = 1 << m_splitLevel; 

  SH_DEBUG_PRINT_BB( "Splitting bounds = " << bounds );

  SH::size_t_array start(static_cast<size_t>(0), Domain::typesize);
  SH::size_t_array lengths(amplificationFactor, Domain::typesize);
  SH::size_t_array strides(static_cast<size_t>(1), Domain::typesize);
  SH::MultiArrayIndex S(start, lengths, strides); 

  int boundsSize = bounds.count(); 
  DomainStream splitBounds(amplificationFactor); 
  splitBounds.reserve(boundsSize * S.length());
  splitBounds.name(bounds.name() + "_split");

  for(int i = 0; i < boundsSize; ++i) {
    Domain lo = bounds.lo(i); 
    Domain hi = bounds.hi(i); 
    Domain width = (hi - lo) / amplificationFactor; // @todo range - should use lengths if not all amplificationFactor

    SH_DEBUG_PRINT_BB( "  i=" << i << " lo=" << lo << " hi=" << hi << " width=" << width );

    for(S.reset();; ++S) {
      Domain splitlo = lo; 
      SH::size_t_array index = S.index();
      for(size_t j = 0; j < index.size(); ++j) {
        splitlo(j) += width(j) * index[j];
      }
      Domain splithi = splitlo + width; 
      SH_DEBUG_PRINT_BB( "  S=" << S << "splitlo=" << splitlo << " splithi=" << splithi );
      splitBounds.append(splitlo, splithi);
      if(S.end()) break;
    }
  }

  SH_DEBUG_PRINT_BB( "Result = " << splitBounds );
  return splitBounds;
}

  // Step 2 - returns an SH::Program that has interval input of type Domain and
  //          interval output of type Range
template<typename D, typename R>
typename BranchBound<D, R>::ResultType 
BranchBound<D, R>::evaluate(DomainStream bounds)
{
  RangeStream result;
  result.resize(bounds.count());
  for(int i = 0; i < bounds.count(); ++i) {
    result[i] = -1; 
  }
  result.name(bounds.name() + "_result");
  SH_DEBUG_PRINT_BB( "Result size = " << result.count() << " bound size = " << bounds.count() );
  SH::Program foo(m_func.node());
  
  SH_DEBUG_PRINT_BB( "evaluate(" << bounds << ")" );
  result = foo << bounds;
  SH_DEBUG_PRINT_BB( " sync" ); 
  result.sync();
  SH_DEBUG_PRINT_BB("  evaluate result= " << result);
  return ResultType(bounds, result);
}

  // Step 3 - checks if we're done.  If so, gives the results.
  // Otherwise, may do some filtering of box regions that no longer need to 
  // be checked in the next iteration.
  //
  // @return true if we're done, false if not
template<typename D, typename R>
bool BranchBound<D, R>::filter(ResultType input, ResultType& output)
{
  SH_DEBUG_PRINT_BB( "Filtering" );
  SH_DEBUG_PRINT_BB( "In Domains: " << input.domain );
  SH_DEBUG_PRINT_BB( "In Ranges:  " << input.range );

  // filter out non-candidates
  // find smallest upper bound u and discard ranges with lower bounds > u
  int count = input.domain.count();
  SH_DEBUG_ASSERT(count > 0);
  Range minHi = input.range.hi(0); 
  for(int i = 1; i < count; ++i) {
    minHi = SH::min(minHi, input.range.hi(i));
  }

  SH_DEBUG_PRINT_BB( "minHi " << minHi );

  output = ResultType();
  for(int i = 0; i < count; ++i) {
    if((minHi /*+ m_eps*/ >= input.range.lo(i)).getValue(0)) { // keep
      output.domain.append(input.domain[i]);
      output.range.append(input.range[i]);
    }
  }

  SH_DEBUG_PRINT_BB( "Out Domains: " << output.domain );
  SH_DEBUG_PRINT_BB( "Out Ranges:  " << output.range );

  output.domain.name(input.domain.name() + "_filter");
  output.range.name(input.range.name() + "_filter");

  // return true if bounds are small enough 
  Domain test = input.domain.hi(0) - input.domain.lo(0);
  for(int i = 0; i < test.size(); ++i) {
    if(test.getValue(i) > m_eps) return false;
  }
  return true;
}

template<typename B>
BranchBoundIterator<B>::BranchBoundIterator()
{}

template<typename B>
BranchBoundIterator<B>::BranchBoundIterator(B& params, typename B::IntervalDomain& initialBounds) 
  : m_params(params),
    m_done(false) 
{
  m_current.domain.append(initialBounds); 
  m_current.domain.name("init");
  //SH_DEBUG_PRINT_BB( "Initial Domain = " << m_current.domain );
}

template<typename B>
typename B::ResultType& BranchBoundIterator<B>::current()
{
  return m_current;
}

template<typename B>
void BranchBoundIterator<B>::iterate()
{
  if(m_done) return;
  Timer start = Timer::now();
  m_current.domain = m_params.split(m_current.domain);
  SH_DEBUG_PRINT_BB( "Starting split:" );
  double elapsed = (Timer::now() - start).value();
  SH_DEBUG_PRINT_BB( "Elapsed domain split: " << elapsed );

  start = Timer::now();
  SH_DEBUG_PRINT_BB( "Starting evaluate:" );
  m_current = m_params.evaluate(m_current.domain);
  elapsed = (Timer::now() - start).value();
  SH_DEBUG_PRINT_BB( "Elapsed evaluate: " << elapsed );

  start = Timer::now();
  SH_DEBUG_PRINT_BB( "Starting filter:" );
  m_done = m_params.filter(m_current, m_current);
  elapsed = (Timer::now() - start).value();
  SH_DEBUG_PRINT_BB( "Elapsed filter: " << elapsed );
}

template<typename B>
bool BranchBoundIterator<B>::done()
{
  return m_done;
}


#endif
