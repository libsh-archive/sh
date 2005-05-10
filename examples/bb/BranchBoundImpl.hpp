#ifndef BRANCHBOUNDIMPL_HPP
#define BRANCHBOUNDIMPL_HPP

#include <iostream>
#include <sh/ShMultiArray.hpp>
#include "BranchBound.hpp"

template<typename T>
const int IAStream<T>::m_datasize = T::typesize * sizeof(typename T::mem_type);

template<typename T>
IAStream<T>::IAStream() 
{
  ShStream::append(m_lo);
  ShStream::append(m_hi);

  allocData(1, m_lo, m_loData);
  allocData(1, m_hi, m_hiData);
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
  return m_loData.size() / T::typesize; 
}

template<typename T>
T IAStream<T>::lo(int i) const
{
  SH_DEBUG_ASSERT((i + 1) * typesize <= m_loData.size());
  T result;
  result.setValues(&(*m_loData)[i * T::typesize]);
  return result;
}

template<typename T>
T IAStream<T>::hi(int i) const
{
  SH_DEBUG_ASSERT((i + 1) * typesize <= m_loData.size());
  T result;
  result.setValues(&(*m_hiData)[i * T::typesize]);
  return result;
}

template<typename T>
typename IAStream<T>::IntervalT IAStream<T>::operator[](int i) const
{
  return make_interval(lo(i), hi(i));
}

template<typename T>
IAStream<T>& IAStream<T>::operator=(const SH::ShProgram& program)
{
  SH::ShStream::operator=(program);
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
  lo.getValues(&(*m_loData)[index * T::typesize]);
  hi.getValues(&(*m_hiData)[index * T::typesize]);
}

template<typename T>
void IAStream<T>::append(const IntervalT& bounds)
{
  append(range_lo(bounds), range_hi(bounds));
}

template<typename T>
void IAStream<T>::resize(int length, SH::ShChannel<T>& ch, DataTypePtr& data)
{
  reserve(length, ch, data);
  ch.count(length);

  // @todo debug
  std::cout << "resized " << length << std::endl;
}

template<typename T>
void IAStream<T>::reserve(int length, SH::ShChannel<T>& ch, DataTypePtr& data)
{
  if(length * T::typesize < data->size()) {
    return;
  }

  // double capacity till we have enough, then copy over data
  int newcap = data->size();
  for(;newcap < length; newcap <<= 1); 
  allocData(newcap, ch, data);
  
  // @todo debug
  std::cout << "reserved " << newcap << std::endl;
}

template<typename T>
void IAStream<T>::allocData(int length, SH::ShChannel<T>& ch, DataTypePtr& data)
{
  SH::ShHostMemoryPtr newmem = new SH::ShHostMemory(length * m_datasize);

  int copyLength = 0;
  typename DataType::const_iterator oldBegin = 0; 
  if(data) {
    oldBegin = data->begin(); 
    copyLength = std::min(length * T::typesize, data->size());
  }

  data = new DataType(newmem->hostStorage()->data(), length * T::typesize, false); 
  
  if(copyLength) {
    std::copy(oldBegin, oldBegin + copyLength, data->begin());
  }
  newmem->hostStorage()->dirty();

  ch.memory(newmem, ch.count());

  // @todo debug
  std::cout << "Allocated size " << length  << std::endl;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const IAStream<T>& ias) 
{
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
BranchBound<D, R>::BranchBound(SH::ShProgram f, size_t splitLevel, HostType eps)
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

  //std::cout << "Splitting bounds = " << bounds << std::endl;

  SH::size_t_array start(static_cast<size_t>(0), Domain::typesize);
  SH::size_t_array lengths(amplificationFactor, Domain::typesize);
  SH::size_t_array strides(static_cast<size_t>(1), Domain::typesize);
  SH::MultiArrayIndex S(start, lengths, strides); 

  int boundsSize = bounds.count(); 
  DomainStream splitBounds; 
  splitBounds.reserve(boundsSize * S.length());
  splitBounds.name(bounds.name() + "_split");

  for(int i = 0; i < boundsSize; ++i) {
    Domain lo = bounds.lo(i); 
    Domain hi = bounds.hi(i); 
    Domain width = (hi - lo) / amplificationFactor; // @todo range - should use lengths if not all amplificationFactor

    //std::cout << "  i=" << i << " lo=" << lo << " hi=" << hi << " width=" << width << std::endl;

    for(S.reset();; ++S) {
      Domain splitlo = lo; 
      SH::size_t_array index = S.index();
      for(size_t j = 0; j < index.size(); ++j) {
        splitlo(j) += width(j) * index[j];
      }
      Domain splithi = splitlo + width; 
      //std::cout << "  S=" << S << "splitlo=" << splitlo << " splithi=" << splithi << std::endl;
      splitBounds.append(splitlo, splithi);
      if(S.end()) break;
    }
  }

  //std::cout << "Result = " << splitBounds << std::endl;
  return splitBounds;
}

  // Step 2 - returns an SH::ShProgram that has interval input of type Domain and
  //          interval output of type Range
template<typename D, typename R>
typename BranchBound<D, R>::ResultType 
BranchBound<D, R>::evaluate(DomainStream bounds)
{
  RangeStream result;
  result.resize(bounds.count());
  result.name(bounds.name() + "_result");
  std::cout << "Result size = " << result.count() << " bound size = " << bounds.count() << std::endl;
  SH::ShProgram foo(m_func.node()->clone());
  
  result = foo << bounds;
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
  std::cout << "Filtering" << std::endl;
  std::cout << "In Domains: " << input.domain << std::endl;
  std::cout << "In Ranges:  " << input.range << std::endl;

  // filter out non-candidates
  // find smallest upper bound u and discard ranges with lower bounds > u
  int count = input.domain.count();
  SH_DEBUG_ASSERT(count > 0);
  Range minHi = input.range.hi(0); 
  for(int i = 1; i < count; ++i) {
    minHi = SH::min(minHi, input.range.hi(i));
  }

  std::cout << "minHi " << minHi << std::endl;

  output = ResultType();
  for(int i = 0; i < count; ++i) {
    if((minHi + m_eps >= input.range.lo(i)).getValue(0)) { // keep
      std::cout << "  filter appending " << i << std::endl;
      output.domain.append(input.domain[i]);
      output.range.append(input.range[i]);
    }
  }

  std::cout << "Out Domains: " << output.domain << std::endl;
  std::cout << "Out Ranges:  " << output.range << std::endl;

  output.domain.name(input.domain.name() + "_filter");
  output.range.name(input.range.name() + "_filter");

  // return true if bounds are small enough 
  Domain test = input.domain.hi(0) - input.domain.lo(0);
  for(int i = 0; i < test.size(); ++i) {
    if(test.getValue(i) > eps) return false;
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
  std::cout << "Initial Domain = " << m_current.domain << std::endl;
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
  m_current.domain = m_params.split(m_current.domain);
  m_current = m_params.evaluate(m_current.domain);
  m_done = m_params.filter(m_current, m_current);
}

template<typename B>
bool BranchBoundIterator<B>::done()
{
  return m_done;
}


#endif
